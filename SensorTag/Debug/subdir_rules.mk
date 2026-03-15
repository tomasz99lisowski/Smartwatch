################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ti-cgt-arm_18.12.5.LTS/bin/armcl" -mv7M3 --code_state=16 --float_support=vfplib -me --include_path="C:/Users/Tomasz/Desktop/Tomek/Erasmus/Smartwatch/SensorTag" --include_path="C:/ti/simplelink_cc13x0_sdk_4_20_02_07/source/ti/posix/ccs" --include_path="C:/ti/ti-cgt-arm_18.12.5.LTS/include" --define=OAD_BLOCK_SIZE=64 --define=SUPPORT_PHY_CUSTOM --define=SUPPORT_PHY_50KBPS2GFSK --define=SUPPORT_PHY_625BPSLRM --define=SUPPORT_PHY_5KBPSSLLR --define=DeviceFamily_CC13X0 --define=CCFG_FORCE_VDDR_HH=0 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1108325903:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-1108325903-inproc

build-1108325903-inproc: ../rfWsnNode.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"C:/ti/xdctools_3_51_03_28_core/xs" --xdcpath="C:/ti/simplelink_cc13x0_sdk_4_20_02_07/source;C:/ti/simplelink_cc13x0_sdk_4_20_02_07/kernel/tirtos/packages;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M3 -p ti.platforms.simplelink:CC1350F128 -r release -c "C:/ti/ti-cgt-arm_18.12.5.LTS" --compileOptions "-mv7M3 --code_state=16 --float_support=vfplib -me --include_path=\"C:/Users/Tomasz/Desktop/Tomek/Erasmus/Smartwatch/SensorTag\" --include_path=\"C:/ti/simplelink_cc13x0_sdk_4_20_02_07/source/ti/posix/ccs\" --include_path=\"C:/ti/ti-cgt-arm_18.12.5.LTS/include\" --define=OAD_BLOCK_SIZE=64 --define=SUPPORT_PHY_CUSTOM --define=SUPPORT_PHY_50KBPS2GFSK --define=SUPPORT_PHY_625BPSLRM --define=SUPPORT_PHY_5KBPSSLLR --define=DeviceFamily_CC13X0 --define=CCFG_FORCE_VDDR_HH=0 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on  " "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-1108325903 ../rfWsnNode.cfg
configPkg/compiler.opt: build-1108325903
configPkg: build-1108325903


