################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Command_Driver_APIs/Command_APIs.c 

OBJS += \
./Command_Driver_APIs/Command_APIs.o 

C_DEPS += \
./Command_Driver_APIs/Command_APIs.d 


# Each subdirectory must supply rules for building sources it contributes
Command_Driver_APIs/%.o Command_Driver_APIs/%.su Command_Driver_APIs/%.cyclo: ../Command_Driver_APIs/%.c Command_Driver_APIs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DARM_MATH_CM0 -DUSE_HAL_DRIVER -DSTM32F031x6 -DUSE_FULL_LL_DRIVER -DDEBUG -c -I../../Drivers/CMSIS/Include -I../../MCSDK_v5.4.4/MotorControl/MCSDK/SystemDriveParams -I../../MCSDK_v5.4.4/MotorControl/MCSDK/MCLib/Any/Inc -I../../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Inc -I../../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../../MCSDK_v5.4.4/MotorControl/MCSDK/MCLib/F0xx/Inc -I../../MCSDK_v5.4.4/MotorControl/MCSDK/UILibrary/Inc -I../../Drivers/CMSIS/DSP/Include -I../../Drivers/STM32F0xx_HAL_Driver/Inc -I"C:/Users/Control/Documents/Hexabitz/Hexabitz BLDC/STSPIN-Firmware/EVALKIT_ROBOT_1_SDK544_Positioning_LL/STM32CubeIDE/Command_Driver_APIs" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Command_Driver_APIs

clean-Command_Driver_APIs:
	-$(RM) ./Command_Driver_APIs/Command_APIs.cyclo ./Command_Driver_APIs/Command_APIs.d ./Command_Driver_APIs/Command_APIs.o ./Command_Driver_APIs/Command_APIs.su

.PHONY: clean-Command_Driver_APIs

