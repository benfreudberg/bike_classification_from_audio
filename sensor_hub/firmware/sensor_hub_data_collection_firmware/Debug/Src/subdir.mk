################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/bsp_driver_sd.c \
../Src/dfsdm.c \
../Src/dma.c \
../Src/fatfs.c \
../Src/fatfs_platform.c \
../Src/freertos.c \
../Src/gpio.c \
../Src/i2c.c \
../Src/leds.c \
../Src/main.c \
../Src/rtc.c \
../Src/sd_diskio.c \
../Src/sdmmc.c \
../Src/spi.c \
../Src/stm32l4xx_hal_msp.c \
../Src/stm32l4xx_hal_timebase_tim.c \
../Src/stm32l4xx_it.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/system_stm32l4xx.c \
../Src/usart.c 

OBJS += \
./Src/bsp_driver_sd.o \
./Src/dfsdm.o \
./Src/dma.o \
./Src/fatfs.o \
./Src/fatfs_platform.o \
./Src/freertos.o \
./Src/gpio.o \
./Src/i2c.o \
./Src/leds.o \
./Src/main.o \
./Src/rtc.o \
./Src/sd_diskio.o \
./Src/sdmmc.o \
./Src/spi.o \
./Src/stm32l4xx_hal_msp.o \
./Src/stm32l4xx_hal_timebase_tim.o \
./Src/stm32l4xx_it.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/system_stm32l4xx.o \
./Src/usart.o 

C_DEPS += \
./Src/bsp_driver_sd.d \
./Src/dfsdm.d \
./Src/dma.d \
./Src/fatfs.d \
./Src/fatfs_platform.d \
./Src/freertos.d \
./Src/gpio.d \
./Src/i2c.d \
./Src/leds.d \
./Src/main.d \
./Src/rtc.d \
./Src/sd_diskio.d \
./Src/sdmmc.d \
./Src/spi.d \
./Src/stm32l4xx_hal_msp.d \
./Src/stm32l4xx_hal_timebase_tim.d \
./Src/stm32l4xx_it.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/system_stm32l4xx.d \
./Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4R7xx -c -I../Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/bsp_driver_sd.cyclo ./Src/bsp_driver_sd.d ./Src/bsp_driver_sd.o ./Src/bsp_driver_sd.su ./Src/dfsdm.cyclo ./Src/dfsdm.d ./Src/dfsdm.o ./Src/dfsdm.su ./Src/dma.cyclo ./Src/dma.d ./Src/dma.o ./Src/dma.su ./Src/fatfs.cyclo ./Src/fatfs.d ./Src/fatfs.o ./Src/fatfs.su ./Src/fatfs_platform.cyclo ./Src/fatfs_platform.d ./Src/fatfs_platform.o ./Src/fatfs_platform.su ./Src/freertos.cyclo ./Src/freertos.d ./Src/freertos.o ./Src/freertos.su ./Src/gpio.cyclo ./Src/gpio.d ./Src/gpio.o ./Src/gpio.su ./Src/i2c.cyclo ./Src/i2c.d ./Src/i2c.o ./Src/i2c.su ./Src/leds.cyclo ./Src/leds.d ./Src/leds.o ./Src/leds.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/rtc.cyclo ./Src/rtc.d ./Src/rtc.o ./Src/rtc.su ./Src/sd_diskio.cyclo ./Src/sd_diskio.d ./Src/sd_diskio.o ./Src/sd_diskio.su ./Src/sdmmc.cyclo ./Src/sdmmc.d ./Src/sdmmc.o ./Src/sdmmc.su ./Src/spi.cyclo ./Src/spi.d ./Src/spi.o ./Src/spi.su ./Src/stm32l4xx_hal_msp.cyclo ./Src/stm32l4xx_hal_msp.d ./Src/stm32l4xx_hal_msp.o ./Src/stm32l4xx_hal_msp.su ./Src/stm32l4xx_hal_timebase_tim.cyclo ./Src/stm32l4xx_hal_timebase_tim.d ./Src/stm32l4xx_hal_timebase_tim.o ./Src/stm32l4xx_hal_timebase_tim.su ./Src/stm32l4xx_it.cyclo ./Src/stm32l4xx_it.d ./Src/stm32l4xx_it.o ./Src/stm32l4xx_it.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/system_stm32l4xx.cyclo ./Src/system_stm32l4xx.d ./Src/system_stm32l4xx.o ./Src/system_stm32l4xx.su ./Src/usart.cyclo ./Src/usart.d ./Src/usart.o ./Src/usart.su

.PHONY: clean-Src

