################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := E:/Program Files (x86)/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./RF_Toggle_LED_Demo.obj" \
"./HAL/HAL_PMM.obj" \
"./HAL/RF1A.obj" \
"./HAL/RfRegSettings.obj" \
"../lnk_cc430f5137.cmd" \
$(GEN_CMDS__FLAG) \
-l"libc.a" \
-l"libmath.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include HAL/subdir_vars.mk
-include subdir_rules.mk
-include HAL/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
test.out \

EXE_OUTPUTS__QUOTED += \
"test.out" \

BIN_OUTPUTS += \
test.hex \

BIN_OUTPUTS__QUOTED += \
"test.hex" \


# All Target
all: test.out

# Tool invocations
test.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: MSP430 Linker'
	"E:/Program Files (x86)/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/bin/cl430" -vmspx --abi=eabi --opt_for_speed=0 --use_hw_mpy=F5 --advice:power="all" -g --define=__CC430F5137__ --define=MHZ_868 --display_error_number --diag_warning=225 --silicon_errata=CPU18 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal -z -m"test.map" --stack_size=80 --heap_size=80 --cinit_hold_wdt=on -i"E:/Program Files (x86)/ti/ccsv6/ccs_base/msp430/include" -i"E:/Program Files (x86)/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/lib" -i"E:/Program Files (x86)/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/include" -i"E:/Program Files (x86)/ti/ccsv6/ccs_base/msp430/lib/5xx_6xx_FRxx" --reread_libs --display_error_number --warn_sections --xml_link_info="test_linkInfo.xml" --use_hw_mpy=F5 --rom_model -o "test.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

test.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: MSP430 Hex Utility'
	"E:/Program Files (x86)/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/bin/hex430" --memwidth=8 --romwidth=8 -o "test.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "RF_Toggle_LED_Demo.pp" "HAL\HAL_PMM.pp" "HAL\RF1A.pp" "HAL\RfRegSettings.pp" 
	-$(RM) "RF_Toggle_LED_Demo.obj" "HAL\HAL_PMM.obj" "HAL\RF1A.obj" "HAL\RfRegSettings.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

