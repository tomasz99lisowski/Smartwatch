################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1297958319:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-1297958319-inproc

build-1297958319-inproc: ../release.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"C:/ti/xdctools_3_51_03_28_core/xs" --xdcpath="C:/ti/simplelink_cc13x0_sdk_4_20_02_07/source;C:/ti/simplelink_cc13x0_sdk_4_20_02_07/kernel/tirtos/packages;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M3 -p ti.platforms.simplelink:CC1350F128 -r release -c "C:/ti/ti-cgt-arm_18.12.5.LTS" --compileOptions "-DDeviceFamily_CC13X0" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-1297958319 ../release.cfg
configPkg/compiler.opt: build-1297958319
configPkg: build-1297958319


