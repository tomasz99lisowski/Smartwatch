/*
 * Copyright (c) 2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/***** Includes *****/

#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>

#include <ti/drivers/PIN.h>
#include <ti/drivers/GPIO.h>
#include <ti/display/Display.h>
#include <ti/display/DisplayExt.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/cpu.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)

/* Board Header files */
#include "Board.h"

#include "SceAdc.h"
#include "NodeTask.h"
#include "NodeRadioTask.h"

/* Sensor Header files */
#include "sensors/SensorI2C.h"
#include "sensors/SensorOpt3001.h"
#include "sensors/SensorBmp280.h"
#include "sensors/SensorHdc1000.h"
#include "sensors/SensorMpu9250.h"
#include "sensors/SensorTmp007.h"
#include "extflash/ExtFlash.h"
#include "sensors/SensorUtil.h"

#include <math.h>

/***** Includes *****/
/* Standard C Libraries */
#include <stdlib.h>
#include <unistd.h>

/* TI Drivers */
#include <ti/drivers/rf/RF.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

/* Driverlib Header files */
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)

/* Board Header files */
#include "Board.h"
#include "smartrf_settings/smartrf_settings.h"

/***** Defines *****/

/* Do power measurement */
//#define POWER_MEASUREMENT

/***** Defines *****/
#define NODE_TASK_STACK_SIZE 1024
#define NODE_TASK_PRIORITY   3

/* A change mask of 0xFF0 means that changes in the lower 4 bits does not trigger a wakeup. */
#define NODE_ADCTASK_CHANGE_MASK                    0xFF0

/* Minimum slow Report interval is 10s (in units of samplingTime)*/
#define NODE_ADCTASK_REPORTINTERVAL_SLOW                10
/* Minimum fast Report interval is 1s (in units of samplingTime) for 30s*/
#define NODE_ADCTASK_REPORTINTERVAL_FAST                1
#define NODE_ADCTASK_REPORTINTERVAL_FAST_DURATION_MS    30000

/* Packet TX Configuration */
#define PAYLOAD_LENGTH      30
#ifdef POWER_MEASUREMENT
#define PACKET_INTERVAL     5  /* For power measurement set packet interval to 5s */
#else
#define PACKET_INTERVAL     500000  /* Set packet interval to 500000us or 500ms */
#endif



/***** Prototypes *****/

/***** Variable declarations *****/
static RF_Object rfObject;
static RF_Handle rfHandle;

/* Pin driver handle */
static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

static uint8_t packet[PAYLOAD_LENGTH];
static uint16_t seqNumber;
int stepCount = 0;


/***** Variable declarations *****/
static Task_Params nodeTaskParams;
Task_Struct nodeTask;    /* not static so you can see in ROV */
static uint8_t nodeTaskStack[NODE_TASK_STACK_SIZE];
Event_Struct nodeEvent;  /* not static so you can see in ROV */
static Event_Handle nodeEventHandle;
static uint16_t latestLuxAdcValue;
float latestTempLocalAdcValue;
float latestPressAdcValue;
float latestHumidAdcValue;
float latestMovementAdcValue[3];
float latestAccelerationAdcValue[3];

uint16_t lastAdcValue = 0;
uint16_t luxThreshold = 100;

/* Clock for the fast report timeout */
Clock_Struct fastReportTimeoutClock;     /* not static so you can see in ROV */
static Clock_Handle fastReportTimeoutClockHandle;

/* Pin driver handle */
static PIN_Handle buttonPinHandle;
static PIN_Handle ledPinHandle;
static PIN_State buttonPinState;
static PIN_State ledPinState;

/* Display driver handles */
static Display_Handle hDisplaySerial;

static uint8_t nodeAddress = 0;

/***** Prototypes *****/
static void nodeTaskFunction(UArg arg0, UArg arg1);
static void updateDisplay(void);
void fastReportTimeoutCallback(UArg arg0);
void adcCallback(uint16_t adcValue);
void buttonCallback(PIN_Handle handle, PIN_Id pinId);

/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config pinTable[] =
{
    Board_PIN_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
#ifdef POWER_MEASUREMENT
#if defined(Board_CC1350_LAUNCHXL)
    Board_DIO30_SWPWR | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
#endif
#endif
    PIN_TERMINATE
};

PIN_Config buttonPinTable[] = {
    Board_PIN_BUTTON0  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    Board_PIN_BUTTON1  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};

uint32_t lastStepTime = 0;

double t = 0;
double dynamicAcc = 0;
float latestAccXValue = 0;
float currentAccXValue = 0;
double threshold = 0.004;

void countSteps(float ax, float ay, float az) {

    //Take only 2-axis movment under consideration

    double totalAcc = sqrt(ax*ax + az*az);
    //double restAcc = 0.007;
    dynamicAcc = totalAcc;
    currentAccXValue = latestAccelerationAdcValue[0];
    double diffX = sqrt((latestAccXValue - currentAccXValue) * (latestAccXValue - currentAccXValue));

    uint32_t time = Clock_getTicks();


    if (diffX > threshold) {
        if (dynamicAcc > 0.01 && (time - lastStepTime) > 300) {
            stepCount++;
            latestAccXValue = currentAccXValue;
            lastStepTime = time;
        }
    }


    

}

void NodeTask_init(void)
{

    /* Create event used internally for state changes */
    Event_Params eventParam;
    Event_Params_init(&eventParam);
    Event_construct(&nodeEvent, &eventParam);
    nodeEventHandle = Event_handle(&nodeEvent);

    /* Create clock object which is used for fast report timeout */
    Clock_Params clkParams;
    Clock_Params_init(&clkParams);

    clkParams.period = 0;
    clkParams.startFlag = FALSE;
    Clock_construct(&fastReportTimeoutClock, fastReportTimeoutCallback, 1, &clkParams);
    fastReportTimeoutClockHandle = Clock_handle(&fastReportTimeoutClock);

    /* Create the node task */
    Task_Params_init(&nodeTaskParams);
    nodeTaskParams.stackSize = NODE_TASK_STACK_SIZE;
    nodeTaskParams.priority = NODE_TASK_PRIORITY;
    nodeTaskParams.stack = &nodeTaskStack;
    Task_construct(&nodeTask, nodeTaskFunction, &nodeTaskParams, NULL);

}

static void nodeTaskFunction(UArg arg0, UArg arg1)
{


    /* Initialize display and try to open UART for display. */
    Display_Params params;
    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;

    /* Open both an available UART display.
     * Note that for SensorTag evaluation boards combined with the SHARP96x96
     * Watch DevPack, there is a pin conflict with UART such that one must be
     * excluded, and UART is preferred by default. To display on the Watch
     * DevPack, add the precompiler define BOARD_DISPLAY_EXCLUDE_UART.
     */
    hDisplaySerial = Display_open(Display_Type_UART, &params);

    /* Check if the selected Display type was found and successfully opened */
    if (hDisplaySerial)
    {
        Display_printf(hDisplaySerial, 0, 0, "Waiting for SCE ADC reading...");
    }

    /* Open LED pins */
    // ledPinHandle = PIN_open(&ledPinState, pinTable);
    // if (ledPinHandle)
    // {
    //     System_abort("Error initializing board 3.3V domain pins\n");
    // }

    if (SensorI2C_open())
    {
        /* Put unused external sensors and flash into Sleep */


         // Humidity

        ExtFlash_open();
        ExtFlash_close();



        /* Init Light sensor */
        SensorOpt3001_init();
        SensorOpt3001_enable(true);

        /* Init Temp sensor */
        SensorTmp007_init();            // Infrared Thermopile Sensor
        SensorTmp007_enable(true);

        SensorBmp280_init();            // Pressure Sensor
        SensorBmp280_enable(true);

        SensorHdc1000_init();           // Humidity Sensor  

        SensorMpu9250_init();           // Gyroscope and accelerometer
        SensorMpu9250_powerOn();
        SensorMpu9250_enable(0x0038);
        
    }
    else
    {
        System_abort("Error initializing sensors\n");
    }

    /* Start the SCE ADC task with 1s sample period and reacting to change in ADC value. */
    SceAdc_init(0x00010000, NODE_ADCTASK_REPORTINTERVAL_FAST, NODE_ADCTASK_CHANGE_MASK);
    SceAdc_registerAdcCallback(adcCallback);
    SceAdc_start();

    /* setup timeout for fast report timeout */
    Clock_setTimeout(fastReportTimeoutClockHandle,
            NODE_ADCTASK_REPORTINTERVAL_FAST_DURATION_MS * 1000 / Clock_tickPeriod);

    /* start fast report and timeout */
    Clock_start(fastReportTimeoutClockHandle);

    buttonPinHandle = PIN_open(&buttonPinState, buttonPinTable);
    if (!buttonPinHandle)
    {
        System_abort("Error initializing button pins\n");
    }

    /* Setup callback for button pins */
    if (PIN_registerIntCb(buttonPinHandle, &buttonCallback) != 0)
    {
        System_abort("Error registering button callback function");
    }

    while (1)
    {
        /* Wait for event */
        uint32_t events = Event_pend(nodeEventHandle, 0, NODE_EVENT_ALL, BIOS_WAIT_FOREVER);

        /* If new ADC value, send this data */
        if (events & NODE_EVENT_NEW_ADC_VALUE)
        {



            uint16_t rawLux;
            float lux;

            /* Read sensor */
            SensorOpt3001_read(&rawLux);
            lux = SensorOpt3001_convert(rawLux);
            latestLuxAdcValue = (uint16_t) lux;
            int32_t diff = (int32_t)latestLuxAdcValue - (int32_t)lastAdcValue;


            if (diff < 0) diff = -diff;
            if (diff > luxThreshold) {
                lastAdcValue = latestLuxAdcValue;
                // GPIO_toggle(Board_GPIO_LED0);
                
                /* Toggle activity LED */
                PIN_setOutputValue(ledPinHandle, NODE_ACTIVITY_LED,!PIN_getOutputValue(NODE_ACTIVITY_LED));
            } else {
                lastAdcValue = latestLuxAdcValue;
            }

            // uint16_t rawTempTarget, rawTempLocal;
            // float tempTarget, tempLocal;
            // SensorTmp007_read(&rawTempLocal, &rawTempTarget);
            // SensorTmp007_convert(rawTempLocal, rawTempTarget, &tempLocal, &tempTarget);
            // latestTempTargetAdcValue = (uint16_t) tempTarget;
            // latestTempLocalAdcValue = (uint16_t) tempLocal;

            // --- Temperature & Humidity ---
            uint16_t rawTemp, rawHum;
            float tempHdc, humHdc;
            SensorHdc1000_start();
            DELAY_MS(15); //in sensors/SensorUtil.h
            SensorHdc1000_read(&rawTemp, &rawHum);
            SensorHdc1000_convert(rawTemp, rawHum, &tempHdc, &humHdc);
            latestTempLocalAdcValue = tempHdc;
            latestHumidAdcValue  = humHdc;
            



            // Pressure & Temperature
            uint8_t rawPres;
            int32_t tempBMP;
            uint32_t press;
            SensorBmp280_read(&rawPres);
            SensorBmp280_convert(&rawPres, &tempBMP, &press);
            latestPressAdcValue = press;

            //Acceleration
            uint8_t accData[3];
            if (SensorMpu9250_accRead((uint16_t*)accData)) {
                // int16_t ax = (int16_t)((accData[0] << 8) | accData[1]);
                // int16_t ay = (int16_t)((accData[2] << 8) | accData[3]);
                // int16_t az = (int16_t)((accData[4] << 8) | accData[5]);

                
                // latestAccelerationAdcValue[0] = SensorMpu9250_accConvert(ax);
                // latestAccelerationAdcValue[1] = SensorMpu9250_accConvert(ay);
                // latestAccelerationAdcValue[2] = SensorMpu9250_accConvert(az);


                float ax, ay, az;

                ax = accData[0] / 16384.0;
                ay = accData[1] / 16384.0;
                az = accData[2] / 16384.0;

                countSteps(ax, ay, az);

                latestAccelerationAdcValue[0] = ax;
                latestAccelerationAdcValue[1] = ay;
                latestAccelerationAdcValue[2] = az;

            }





            /* Send ADC value to concentrator */
            NodeRadioTask_sendAdcData(latestLuxAdcValue);
            NodeRadioTask_sendAdcData(latestTempLocalAdcValue);
            NodeRadioTask_sendAdcData(latestHumidAdcValue);
            NodeRadioTask_sendAdcData(latestPressAdcValue);
            

            /* update display */
            updateDisplay();
        }
        /* If new ADC value, send this data */
        if (events & NODE_EVENT_UPDATE_LCD)
        {
            /* update display */
            updateDisplay();
        }
    }
}
static void updateDisplay(void)
{
    /* get node address if not already done */
    if (nodeAddress == 0)
    {
        nodeAddress = nodeRadioTask_getNodeAddr();
    }

    /* Print to UART clear screen, put cursor to beginning of terminal and print the header */
    Display_printf(hDisplaySerial, 0, 0, "\033[2J \033[0;0HNode ID: 0x%02x", nodeAddress);
    Display_printf(hDisplaySerial, 0, 0, "Node ADC Reading: %04d", latestLuxAdcValue);

    if (latestTempLocalAdcValue > 30.0f) {
        Display_printf(hDisplaySerial, 1, 0,
            "\033[31mTemp Local Reading: %.2f C\033[0m", latestTempLocalAdcValue);
    } else {
        Display_printf(hDisplaySerial, 1, 0,
            "\033[32mTemp Local Reading: %.2f C\033[0m", latestTempLocalAdcValue);
    }


    
    Display_printf(hDisplaySerial, 2, 0, "Humidity Reading: %04f", latestHumidAdcValue);
    Display_printf(hDisplaySerial, 3, 0, "Press Reading: %04f", latestPressAdcValue / 100);

    Display_printf(hDisplaySerial, 7, 0, "AccX: %.2f g", latestAccelerationAdcValue[0]);
    Display_printf(hDisplaySerial, 8, 0, "AccY: %.2f g", latestAccelerationAdcValue[1]);
    Display_printf(hDisplaySerial, 9, 0, "AccZ: %.2f g", latestAccelerationAdcValue[2]);

    Display_printf(hDisplaySerial, 10, 0, "Steps: %d", stepCount);
    Display_printf(hDisplaySerial, 11, 0, "Steps: %lf", dynamicAcc);

    

}

void adcCallback(uint16_t adcValue)
{
    /* Save latest value */
    
    
    // if (abs(latestLuxAdcValue - adcValue) > 5) {
    //     latestLuxAdcValue = adcValue;
    // }
    

    /* Post event */
    Event_post(nodeEventHandle, NODE_EVENT_NEW_ADC_VALUE);
}

/*
 *  ======== buttonCallback ========
 *  Pin interrupt Callback function board buttons configured in the pinTable.
 */
void buttonCallback(PIN_Handle handle, PIN_Id pinId)
{
    /* Debounce logic, only toggle if the button is still pushed (low) */
    CPUdelay(8000*50);

    if (PIN_getInputValue(Board_PIN_BUTTON0) == 0)
    {
        //start fast report and timeout
        SceAdc_setReportInterval(NODE_ADCTASK_REPORTINTERVAL_FAST, NODE_ADCTASK_CHANGE_MASK);
        Clock_start(fastReportTimeoutClockHandle);
    }
}

void fastReportTimeoutCallback(UArg arg0)
{
    //stop fast report
    SceAdc_setReportInterval(NODE_ADCTASK_REPORTINTERVAL_SLOW, NODE_ADCTASK_CHANGE_MASK);
}

/***** Function definitions *****/



void *mainThread(void *arg0)
{
    RF_Params rfParams;
    RF_Params_init(&rfParams);

    /* Open LED pins */
    ledPinHandle = PIN_open(&ledPinState, pinTable);
    if (ledPinHandle == NULL)
    {
        while(1);
    }

#ifdef POWER_MEASUREMENT
#if defined(Board_CC1350_LAUNCHXL)
    /* Route out PA active pin to Board_DIO30_SWPWR */
    PINCC26XX_setMux(ledPinHandle, Board_DIO30_SWPWR, PINCC26XX_MUX_RFC_GPO1);
#endif
#endif

    RF_cmdPropTx.pktLen = PAYLOAD_LENGTH;
    RF_cmdPropTx.pPkt = packet;
    RF_cmdPropTx.startTrigger.triggerType = TRIG_NOW;

    /* Request access to the radio */
#if defined(DeviceFamily_CC26X0R2)
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioSetup, &rfParams);
#else
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
#endif// DeviceFamily_CC26X0R2

    /* Set the frequency */
    RF_postCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

    while(1)
    {
        /* Create packet with incrementing sequence number and random payload */
        // packet[0] = (uint8_t)(seqNumber >> 8);
        // packet[1] = (uint8_t)(seqNumber++);
        // uint8_t i;
        // for (i = 2; i < PAYLOAD_LENGTH; i++)
        // {
        //     packet[i] = rand();
        // }

        uint8_t *ptr = packet;

        // --- Lux (uint16_t) ---
        ptr[0] = latestLuxAdcValue & 0xFF;
        ptr[1] = (latestLuxAdcValue >> 8) & 0xFF;
        ptr += 2;

        // --- Temp (float) ---
        memcpy(ptr, &latestTempLocalAdcValue, sizeof(float));
        ptr += 4;

        // --- Humidity (float) ---
        memcpy(ptr, &latestHumidAdcValue, sizeof(float));
        ptr += 4;

        // --- Pressure (float) ---
        memcpy(ptr, &latestPressAdcValue, sizeof(float));
        ptr += 4;

        // --- Acceleration (float ax, ay, az) ---
        // memcpy(ptr, &latestAccelerationAdcValue[0], sizeof(float)); ptr += 4;
        // memcpy(ptr, &latestAccelerationAdcValue[1], sizeof(float)); ptr += 4;
        // memcpy(ptr, &latestAccelerationAdcValue[2], sizeof(float)); ptr += 4;

        memcpy(ptr, &stepCount, sizeof(int));
        ptr += 4;


        /* Send packet */
        RF_EventMask terminationReason = RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropTx,
                                                   RF_PriorityNormal, NULL, 0);

        switch(terminationReason)
        {
            case RF_EventLastCmdDone:
                // A stand-alone radio operation command or the last radio
                // operation command in a chain finished.
                break;
            case RF_EventCmdCancelled:
                // Command cancelled before it was started; it can be caused
            // by RF_cancelCmd() or RF_flushCmd().
                break;
            case RF_EventCmdAborted:
                // Abrupt command termination caused by RF_cancelCmd() or
                // RF_flushCmd().
                break;
            case RF_EventCmdStopped:
                // Graceful command termination caused by RF_cancelCmd() or
                // RF_flushCmd().
                break;
            default:
                // Uncaught error event
                while(1);
        }

        uint32_t cmdStatus = ((volatile RF_Op*)&RF_cmdPropTx)->status;
        switch(cmdStatus)
        {
            case PROP_DONE_OK:
                // Packet transmitted successfully
                break;
            case PROP_DONE_STOPPED:
                // received CMD_STOP while transmitting packet and finished
                // transmitting packet
                break;
            case PROP_DONE_ABORT:
                // Received CMD_ABORT while transmitting packet
                break;
            case PROP_ERROR_PAR:
                // Observed illegal parameter
                break;
            case PROP_ERROR_NO_SETUP:
                // Command sent without setting up the radio in a supported
                // mode using CMD_PROP_RADIO_SETUP or CMD_RADIO_SETUP
                break;
            case PROP_ERROR_NO_FS:
                // Command sent without the synthesizer being programmed
                break;
            case PROP_ERROR_TXUNF:
                // TX underflow observed during operation
                break;
            default:
                // Uncaught error event - these could come from the
                // pool of states defined in rf_mailbox.h
                while(1);
        }

#ifndef POWER_MEASUREMENT
        PIN_setOutputValue(ledPinHandle, Board_PIN_LED1,!PIN_getOutputValue(Board_PIN_LED1));
#endif
        /* Power down the radio */
        RF_yield(rfHandle);

#ifdef POWER_MEASUREMENT
        /* Sleep for PACKET_INTERVAL s */
        sleep(PACKET_INTERVAL);
#else
        /* Sleep for PACKET_INTERVAL us */
        usleep(PACKET_INTERVAL);
#endif

    }
}
