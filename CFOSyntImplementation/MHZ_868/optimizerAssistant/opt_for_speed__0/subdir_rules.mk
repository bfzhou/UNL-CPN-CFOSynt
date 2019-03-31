################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
RF_Toggle_LED_Demo.obj: ../RF_Toggle_LED_Demo.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"E:/Program Files (x86)/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/bin/cl430" -vmspx --abi=eabi --opt_for_speed=0 --use_hw_mpy=F5 --include_path="E:/Program Files (x86)/ti/ccsv6/ccs_base/msp430/include" --include_path="C:/Users/zbf/Documents/GitHub/timesync/test/HAL" --include_path="E:/Program Files (x86)/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.7/include" --advice:power="all" -g --define=__CC430F5137__ --define=MHZ_868 --display_error_number --diag_warning=225 --silicon_errata=CPU18 --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="RF_Toggle_LED_Demo.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


