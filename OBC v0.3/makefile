################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./SFUsat/flash_mibspi.obj" \
"./SFUsat/sfu_cmds.obj" \
"./SFUsat/sfu_rtc.obj" \
"./SFUsat/sfu_scheduler.obj" \
"./SFUsat/sfu_spi.obj" \
"./SFUsat/sfu_startup.obj" \
"./SFUsat/sfu_state.obj" \
"./SFUsat/sfu_task_main.obj" \
"./SFUsat/sfu_task_radio.obj" \
"./SFUsat/sfu_tasks.obj" \
"./SFUsat/sfu_triumf.obj" \
"./SFUsat/sfu_uart.obj" \
"./SFUsat/sfu_utils.obj" \
"./SFUsat/unit_tests/test_adc.obj" \
"./source/adc.obj" \
"./source/dabort.obj" \
"./source/esm.obj" \
"./source/gio.obj" \
"./source/mibspi.obj" \
"./source/notification.obj" \
"./source/pinmux.obj" \
"./source/rti.obj" \
"./source/rtos_croutine.obj" \
"./source/rtos_event_groups.obj" \
"./source/rtos_heap.obj" \
"./source/rtos_list.obj" \
"./source/rtos_mpu_wrappers.obj" \
"./source/rtos_port.obj" \
"./source/rtos_portasm.obj" \
"./source/rtos_queue.obj" \
"./source/rtos_task.obj" \
"./source/rtos_timer.obj" \
"./source/sci.obj" \
"./source/spi.obj" \
"./source/sys_core.obj" \
"./source/sys_dma.obj" \
"./source/sys_intvecs.obj" \
"./source/sys_main.obj" \
"./source/sys_mpu.obj" \
"./source/sys_pcr.obj" \
"./source/sys_phantom.obj" \
"./source/sys_pmm.obj" \
"./source/sys_pmu.obj" \
"./source/sys_selftest.obj" \
"./source/sys_startup.obj" \
"./source/sys_vim.obj" \
"./source/system.obj" \
"../source/sys_link.cmd" \
$(GEN_CMDS__FLAG) \
-lrtsv7R4_T_be_v3D16_eabi.lib \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include SFUsat/subdir_vars.mk
-include SFUsat/unit_tests/subdir_vars.mk
-include source/subdir_vars.mk
-include SFUsat/subdir_rules.mk
-include SFUsat/unit_tests/subdir_rules.mk
-include source/subdir_rules.mk
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
OBC.out \

EXE_OUTPUTS__QUOTED += \
"OBC.out" \

BIN_OUTPUTS += \
OBC.hex \

BIN_OUTPUTS__QUOTED += \
"OBC.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "OBC.out"

# Tool invocations
OBC.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --define=_DEBUG --define=SFUSAT_BOARD=1 -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi -z -m"OBC.map" --heap_size=0x800 --stack_size=0x800 -i"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/lib" -i"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="OBC_linkInfo.xml" --rom_model --be32 -o "OBC.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

OBC.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armhex"  -o "OBC.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "SFUsat/flash_mibspi.obj" "SFUsat/sfu_cmds.obj" "SFUsat/sfu_rtc.obj" "SFUsat/sfu_scheduler.obj" "SFUsat/sfu_spi.obj" "SFUsat/sfu_startup.obj" "SFUsat/sfu_state.obj" "SFUsat/sfu_task_main.obj" "SFUsat/sfu_task_radio.obj" "SFUsat/sfu_tasks.obj" "SFUsat/sfu_triumf.obj" "SFUsat/sfu_uart.obj" "SFUsat/sfu_utils.obj" "SFUsat/unit_tests/test_adc.obj" "source/adc.obj" "source/dabort.obj" "source/esm.obj" "source/gio.obj" "source/mibspi.obj" "source/notification.obj" "source/pinmux.obj" "source/rti.obj" "source/rtos_croutine.obj" "source/rtos_event_groups.obj" "source/rtos_heap.obj" "source/rtos_list.obj" "source/rtos_mpu_wrappers.obj" "source/rtos_port.obj" "source/rtos_portasm.obj" "source/rtos_queue.obj" "source/rtos_task.obj" "source/rtos_timer.obj" "source/sci.obj" "source/spi.obj" "source/sys_core.obj" "source/sys_dma.obj" "source/sys_intvecs.obj" "source/sys_main.obj" "source/sys_mpu.obj" "source/sys_pcr.obj" "source/sys_phantom.obj" "source/sys_pmm.obj" "source/sys_pmu.obj" "source/sys_selftest.obj" 
	-$(RM) "source/sys_startup.obj" "source/sys_vim.obj" "source/system.obj" 
	-$(RM) "SFUsat/flash_mibspi.d" "SFUsat/sfu_cmds.d" "SFUsat/sfu_rtc.d" "SFUsat/sfu_scheduler.d" "SFUsat/sfu_spi.d" "SFUsat/sfu_startup.d" "SFUsat/sfu_state.d" "SFUsat/sfu_task_main.d" "SFUsat/sfu_task_radio.d" "SFUsat/sfu_tasks.d" "SFUsat/sfu_triumf.d" "SFUsat/sfu_uart.d" "SFUsat/sfu_utils.d" "SFUsat/unit_tests/test_adc.d" "source/adc.d" "source/esm.d" "source/gio.d" "source/mibspi.d" "source/notification.d" "source/pinmux.d" "source/rti.d" "source/rtos_croutine.d" "source/rtos_event_groups.d" "source/rtos_heap.d" "source/rtos_list.d" "source/rtos_mpu_wrappers.d" "source/rtos_port.d" "source/rtos_queue.d" "source/rtos_task.d" "source/rtos_timer.d" "source/sci.d" "source/spi.d" "source/sys_dma.d" "source/sys_main.d" "source/sys_pcr.d" "source/sys_phantom.d" "source/sys_pmm.d" "source/sys_selftest.d" "source/sys_startup.d" "source/sys_vim.d" "source/system.d" 
	-$(RM) "source/dabort.d" "source/rtos_portasm.d" "source/sys_core.d" "source/sys_intvecs.d" "source/sys_mpu.d" "source/sys_pmu.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

