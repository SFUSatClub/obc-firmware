################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
SFUsat/unit_tests/test_adc.obj: ../SFUsat/unit_tests/test_adc.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/unit_tests/test_adc.d_raw" --obj_directory="SFUsat/unit_tests" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/unit_tests/test_flash.obj: ../SFUsat/unit_tests/test_flash.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/unit_tests/test_flash.d_raw" --obj_directory="SFUsat/unit_tests" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


