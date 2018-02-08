################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../source/sys_link.cmd 

ASM_SRCS += \
../source/dabort.asm \
../source/rtos_portasm.asm \
../source/sys_core.asm \
../source/sys_intvecs.asm \
../source/sys_mpu.asm \
../source/sys_pmu.asm 

C_SRCS += \
../source/adc.c \
../source/esm.c \
../source/gio.c \
../source/mibspi.c \
../source/notification.c \
../source/pinmux.c \
../source/rti.c \
../source/rtos_croutine.c \
../source/rtos_event_groups.c \
../source/rtos_heap.c \
../source/rtos_list.c \
../source/rtos_mpu_wrappers.c \
../source/rtos_port.c \
../source/rtos_queue.c \
../source/rtos_task.c \
../source/rtos_timer.c \
../source/sci.c \
../source/spi.c \
../source/sys_dma.c \
../source/sys_main.c \
../source/sys_pcr.c \
../source/sys_phantom.c \
../source/sys_pmm.c \
../source/sys_selftest.c \
../source/sys_startup.c \
../source/sys_vim.c \
../source/system.c 

C_DEPS += \
./source/adc.d \
./source/esm.d \
./source/gio.d \
./source/mibspi.d \
./source/notification.d \
./source/pinmux.d \
./source/rti.d \
./source/rtos_croutine.d \
./source/rtos_event_groups.d \
./source/rtos_heap.d \
./source/rtos_list.d \
./source/rtos_mpu_wrappers.d \
./source/rtos_port.d \
./source/rtos_queue.d \
./source/rtos_task.d \
./source/rtos_timer.d \
./source/sci.d \
./source/spi.d \
./source/sys_dma.d \
./source/sys_main.d \
./source/sys_pcr.d \
./source/sys_phantom.d \
./source/sys_pmm.d \
./source/sys_selftest.d \
./source/sys_startup.d \
./source/sys_vim.d \
./source/system.d 

OBJS += \
./source/adc.obj \
./source/dabort.obj \
./source/esm.obj \
./source/gio.obj \
./source/mibspi.obj \
./source/notification.obj \
./source/pinmux.obj \
./source/rti.obj \
./source/rtos_croutine.obj \
./source/rtos_event_groups.obj \
./source/rtos_heap.obj \
./source/rtos_list.obj \
./source/rtos_mpu_wrappers.obj \
./source/rtos_port.obj \
./source/rtos_portasm.obj \
./source/rtos_queue.obj \
./source/rtos_task.obj \
./source/rtos_timer.obj \
./source/sci.obj \
./source/spi.obj \
./source/sys_core.obj \
./source/sys_dma.obj \
./source/sys_intvecs.obj \
./source/sys_main.obj \
./source/sys_mpu.obj \
./source/sys_pcr.obj \
./source/sys_phantom.obj \
./source/sys_pmm.obj \
./source/sys_pmu.obj \
./source/sys_selftest.obj \
./source/sys_startup.obj \
./source/sys_vim.obj \
./source/system.obj 

ASM_DEPS += \
./source/dabort.d \
./source/rtos_portasm.d \
./source/sys_core.d \
./source/sys_intvecs.d \
./source/sys_mpu.d \
./source/sys_pmu.d 

OBJS__QUOTED += \
"source/adc.obj" \
"source/dabort.obj" \
"source/esm.obj" \
"source/gio.obj" \
"source/mibspi.obj" \
"source/notification.obj" \
"source/pinmux.obj" \
"source/rti.obj" \
"source/rtos_croutine.obj" \
"source/rtos_event_groups.obj" \
"source/rtos_heap.obj" \
"source/rtos_list.obj" \
"source/rtos_mpu_wrappers.obj" \
"source/rtos_port.obj" \
"source/rtos_portasm.obj" \
"source/rtos_queue.obj" \
"source/rtos_task.obj" \
"source/rtos_timer.obj" \
"source/sci.obj" \
"source/spi.obj" \
"source/sys_core.obj" \
"source/sys_dma.obj" \
"source/sys_intvecs.obj" \
"source/sys_main.obj" \
"source/sys_mpu.obj" \
"source/sys_pcr.obj" \
"source/sys_phantom.obj" \
"source/sys_pmm.obj" \
"source/sys_pmu.obj" \
"source/sys_selftest.obj" \
"source/sys_startup.obj" \
"source/sys_vim.obj" \
"source/system.obj" 

C_DEPS__QUOTED += \
"source/adc.d" \
"source/esm.d" \
"source/gio.d" \
"source/mibspi.d" \
"source/notification.d" \
"source/pinmux.d" \
"source/rti.d" \
"source/rtos_croutine.d" \
"source/rtos_event_groups.d" \
"source/rtos_heap.d" \
"source/rtos_list.d" \
"source/rtos_mpu_wrappers.d" \
"source/rtos_port.d" \
"source/rtos_queue.d" \
"source/rtos_task.d" \
"source/rtos_timer.d" \
"source/sci.d" \
"source/spi.d" \
"source/sys_dma.d" \
"source/sys_main.d" \
"source/sys_pcr.d" \
"source/sys_phantom.d" \
"source/sys_pmm.d" \
"source/sys_selftest.d" \
"source/sys_startup.d" \
"source/sys_vim.d" \
"source/system.d" 

ASM_DEPS__QUOTED += \
"source/dabort.d" \
"source/rtos_portasm.d" \
"source/sys_core.d" \
"source/sys_intvecs.d" \
"source/sys_mpu.d" \
"source/sys_pmu.d" 

C_SRCS__QUOTED += \
"../source/adc.c" \
"../source/esm.c" \
"../source/gio.c" \
"../source/mibspi.c" \
"../source/notification.c" \
"../source/pinmux.c" \
"../source/rti.c" \
"../source/rtos_croutine.c" \
"../source/rtos_event_groups.c" \
"../source/rtos_heap.c" \
"../source/rtos_list.c" \
"../source/rtos_mpu_wrappers.c" \
"../source/rtos_port.c" \
"../source/rtos_queue.c" \
"../source/rtos_task.c" \
"../source/rtos_timer.c" \
"../source/sci.c" \
"../source/spi.c" \
"../source/sys_dma.c" \
"../source/sys_main.c" \
"../source/sys_pcr.c" \
"../source/sys_phantom.c" \
"../source/sys_pmm.c" \
"../source/sys_selftest.c" \
"../source/sys_startup.c" \
"../source/sys_vim.c" \
"../source/system.c" 

ASM_SRCS__QUOTED += \
"../source/dabort.asm" \
"../source/rtos_portasm.asm" \
"../source/sys_core.asm" \
"../source/sys_intvecs.asm" \
"../source/sys_mpu.asm" \
"../source/sys_pmu.asm" 


