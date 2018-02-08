################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
SFUsat/eric-test.obj: ../SFUsat/eric-test.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/eric-test.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/flash_mibspi.obj: ../SFUsat/flash_mibspi.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/flash_mibspi.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/sfu_cmds.obj: ../SFUsat/sfu_cmds.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/sfu_cmds.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/sfu_rtc.obj: ../SFUsat/sfu_rtc.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/sfu_rtc.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/sfu_scheduler.obj: ../SFUsat/sfu_scheduler.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/sfu_scheduler.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/sfu_spi.obj: ../SFUsat/sfu_spi.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/sfu_spi.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/sfu_startup.obj: ../SFUsat/sfu_startup.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/sfu_startup.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/sfu_state.obj: ../SFUsat/sfu_state.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/sfu_state.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/sfu_task_main.obj: ../SFUsat/sfu_task_main.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/sfu_task_main.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/sfu_task_radio.obj: ../SFUsat/sfu_task_radio.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/sfu_task_radio.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/sfu_tasks.obj: ../SFUsat/sfu_tasks.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/sfu_tasks.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/sfu_triumf.obj: ../SFUsat/sfu_triumf.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/sfu_triumf.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/sfu_uart.obj: ../SFUsat/sfu_uart.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/sfu_uart.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

SFUsat/sfu_utils.obj: ../SFUsat/sfu_utils.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="/Users/Richard/Desktop/obc-firmware/SFUsat" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --include_path="/Users/Richard/Desktop/obc-firmware/include" --define=_DEBUG --define=SFUSAT_BOARD=2 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="SFUsat/sfu_utils.d_raw" --obj_directory="SFUsat" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


