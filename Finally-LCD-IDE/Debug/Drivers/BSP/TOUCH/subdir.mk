################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/TOUCH/touch.c 

OBJS += \
./Drivers/BSP/TOUCH/touch.o 

C_DEPS += \
./Drivers/BSP/TOUCH/touch.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/TOUCH/%.o Drivers/BSP/TOUCH/%.su Drivers/BSP/TOUCH/%.cyclo: ../Drivers/BSP/TOUCH/%.c Drivers/BSP/TOUCH/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"../Drivers/BSP/LCD" -I"../Drivers/BSP/TOUCH" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-TOUCH

clean-Drivers-2f-BSP-2f-TOUCH:
	-$(RM) ./Drivers/BSP/TOUCH/touch.cyclo ./Drivers/BSP/TOUCH/touch.d ./Drivers/BSP/TOUCH/touch.o ./Drivers/BSP/TOUCH/touch.su

.PHONY: clean-Drivers-2f-BSP-2f-TOUCH

