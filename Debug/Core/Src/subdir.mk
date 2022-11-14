################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/UI.c \
../Core/Src/alarm.c \
../Core/Src/buzzer.c \
../Core/Src/dht11.c \
../Core/Src/lcdtp.c \
../Core/Src/main.c \
../Core/Src/pet.c \
../Core/Src/printf.c \
../Core/Src/rtc.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c \
../Core/Src/timer.c \
../Core/Src/xpt2046.c 

OBJS += \
./Core/Src/UI.o \
./Core/Src/alarm.o \
./Core/Src/buzzer.o \
./Core/Src/dht11.o \
./Core/Src/lcdtp.o \
./Core/Src/main.o \
./Core/Src/pet.o \
./Core/Src/printf.o \
./Core/Src/rtc.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o \
./Core/Src/timer.o \
./Core/Src/xpt2046.o 

C_DEPS += \
./Core/Src/UI.d \
./Core/Src/alarm.d \
./Core/Src/buzzer.d \
./Core/Src/dht11.d \
./Core/Src/lcdtp.d \
./Core/Src/main.d \
./Core/Src/pet.d \
./Core/Src/printf.d \
./Core/Src/rtc.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d \
./Core/Src/timer.d \
./Core/Src/xpt2046.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"  -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/UI.d ./Core/Src/UI.o ./Core/Src/UI.su ./Core/Src/alarm.d ./Core/Src/alarm.o ./Core/Src/alarm.su ./Core/Src/buzzer.d ./Core/Src/buzzer.o ./Core/Src/buzzer.su ./Core/Src/dht11.d ./Core/Src/dht11.o ./Core/Src/dht11.su ./Core/Src/lcdtp.d ./Core/Src/lcdtp.o ./Core/Src/lcdtp.su ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/pet.d ./Core/Src/pet.o ./Core/Src/pet.su ./Core/Src/printf.d ./Core/Src/printf.o ./Core/Src/printf.su ./Core/Src/rtc.d ./Core/Src/rtc.o ./Core/Src/rtc.su ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su ./Core/Src/timer.d ./Core/Src/timer.o ./Core/Src/timer.su ./Core/Src/xpt2046.d ./Core/Src/xpt2046.o ./Core/Src/xpt2046.su

.PHONY: clean-Core-2f-Src

