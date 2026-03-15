################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ti-cgt-arm_18.12.5.LTS

GEN_OPTS__FLAG := --cmd_file="configPkg/compiler.opt" 
GEN_CMDS__FLAG := -l"configPkg/linker.cmd" 

ORDERED_OBJS += \
"./CC1350STK.obj" \
"./CC1350STK_fxns.obj" \
"./NodeRadioTask.obj" \
"./NodeTask.obj" \
"./SceAdc.obj" \
"./rfWsnNode.obj" \
"./easylink/EasyLink.obj" \
"./easylink/easylink_config.obj" \
"./extflash/ExtFlash.obj" \
"./sce/scif.obj" \
"./sce/scif_framework.obj" \
"./sce/scif_osal_tirtos.obj" \
"./sensors/SensorBmp280.obj" \
"./sensors/SensorHdc1000.obj" \
"./sensors/SensorI2C.obj" \
"./sensors/SensorMpu9250.obj" \
"./sensors/SensorOpt3001.obj" \
"./sensors/SensorTmp007.obj" \
"./sensors/SensorUtil.obj" \
"./smartrf_settings/smartrf_settings.obj" \
"./smartrf_settings/smartrf_settings_predefined.obj" \
"../CC1350STK_TIRTOS_OAD.cmd" \
$(GEN_CMDS__FLAG) \
-l"ti/display/lib/display.aem3" \
-l"ti/grlib/lib/ccs/m3/grlib.a" \
-l"third_party/spiffs/lib/ccs/m3/spiffs_cc26xx.a" \
-l"ti/drivers/rf/lib/rf_multiMode_cc13x0.aem3" \
-l"ti/drivers/lib/drivers_cc13x0.aem3" \
-l"ti/dpl/lib/dpl_cc13x0.aem3" \
-l"ti/devices/cc13x0/driverlib/bin/ccs/driverlib.lib" \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include easylink/subdir_vars.mk
-include extflash/subdir_vars.mk
-include sce/subdir_vars.mk
-include sensors/subdir_vars.mk
-include smartrf_settings/subdir_vars.mk
-include subdir_rules.mk
-include easylink/subdir_rules.mk
-include extflash/subdir_rules.mk
-include sce/subdir_rules.mk
-include sensors/subdir_rules.mk
-include smartrf_settings/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
SensorTag.out 

EXE_OUTPUTS__QUOTED += \
"SensorTag.out" 

BIN_OUTPUTS += \
SensorTag.hex 

BIN_OUTPUTS__QUOTED += \
"SensorTag.hex" 


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "SensorTag.out" secondary-outputs

# Tool invocations
SensorTag.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"C:/ti/ti-cgt-arm_18.12.5.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=vfplib -me --define=OAD_BLOCK_SIZE=64 --define=SUPPORT_PHY_CUSTOM --define=SUPPORT_PHY_50KBPS2GFSK --define=SUPPORT_PHY_625BPSLRM --define=SUPPORT_PHY_5KBPSSLLR --define=DeviceFamily_CC13X0 --define=CCFG_FORCE_VDDR_HH=0 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on -z -m"SensorTag.map" -i"C:/ti/simplelink_cc13x0_sdk_4_20_02_07/source" -i"C:/ti/simplelink_cc13x0_sdk_4_20_02_07/kernel/tirtos/packages" -i"C:/ti/ti-cgt-arm_18.12.5.LTS/lib" --diag_wrap=off --display_error_number --warn_sections --xml_link_info="SensorTag_linkInfo.xml" --rom_model -o "SensorTag.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

SensorTag.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: Arm Hex Utility'
	"C:/ti/ti-cgt-arm_18.12.5.LTS/bin/armhex" --memwidth=8 --diag_wrap=off --intel -o "SensorTag.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "CC1350STK.obj" "CC1350STK_fxns.obj" "NodeRadioTask.obj" "NodeTask.obj" "SceAdc.obj" "rfWsnNode.obj" "easylink\EasyLink.obj" "easylink\easylink_config.obj" "extflash\ExtFlash.obj" "sce\scif.obj" "sce\scif_framework.obj" "sce\scif_osal_tirtos.obj" "sensors\SensorBmp280.obj" "sensors\SensorHdc1000.obj" "sensors\SensorI2C.obj" "sensors\SensorMpu9250.obj" "sensors\SensorOpt3001.obj" "sensors\SensorTmp007.obj" "sensors\SensorUtil.obj" "smartrf_settings\smartrf_settings.obj" "smartrf_settings\smartrf_settings_predefined.obj" 
	-$(RM) "CC1350STK.d" "CC1350STK_fxns.d" "NodeRadioTask.d" "NodeTask.d" "SceAdc.d" "rfWsnNode.d" "easylink\EasyLink.d" "easylink\easylink_config.d" "extflash\ExtFlash.d" "sce\scif.d" "sce\scif_framework.d" "sce\scif_osal_tirtos.d" "sensors\SensorBmp280.d" "sensors\SensorHdc1000.d" "sensors\SensorI2C.d" "sensors\SensorMpu9250.d" "sensors\SensorOpt3001.d" "sensors\SensorTmp007.d" "sensors\SensorUtil.d" "smartrf_settings\smartrf_settings.d" "smartrf_settings\smartrf_settings_predefined.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-@echo 'Finished clean'
	-@echo ' '

secondary-outputs: $(BIN_OUTPUTS)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

