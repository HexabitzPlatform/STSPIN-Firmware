################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Control/Documents/Hexabitz/Hexabitz\ BLDC/en.STSW-ROBOT-1_R1_LL/EVALKIT_ROBOT_1_SDK544_Positioning_LL/Src/system_stm32f0xx.c 

OBJS += \
./Drivers/CMSIS/system_stm32f0xx.o 

C_DEPS += \
./Drivers/CMSIS/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/system_stm32f0xx.o: C:/Users/Control/Documents/Hexabitz/Hexabitz\ BLDC/en.STSW-ROBOT-1_R1_LL/EVALKIT_ROBOT_1_SDK544_Positioning_LL/Src/system_stm32f0xx.c Drivers/CMSIS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -DARM_MATH_CM0 -DUSE_HAL_DRIVER -DSTM32F031x6 -DUSE_FULL_LL_DRIVER -c -I../../Drivers/CMSIS/Include -I../../MCSDK_v5.4.4/MotorControl/MCSDK/SystemDriveParams -I../../MCSDK_v5.4.4/MotorControl/MCSDK/MCLib/Any/Inc -I../../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../../Inc -I../../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../../MCSDK_v5.4.4/MotorControl/MCSDK/MCLib/F0xx/Inc -I../../MCSDK_v5.4.4/MotorControl/MCSDK/UILibrary/Inc -I../../Drivers/CMSIS/DSP/Include -I../../Drivers/STM32F0xx_HAL_Driver/Inc -I"C:/Users/Control/Documents/Hexabitz/Hexabitz BLDC/en.STSW-ROBOT-1_R1_LL/EVALKIT_ROBOT_1_SDK544_Positioning_LL/STM32CubeIDE/Command_Driver_APIs" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Drivers/CMSIS/system_stm32f0xx.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-CMSIS

clean-Drivers-2f-CMSIS:
	-$(RM) ./Drivers/CMSIS/system_stm32f0xx.cyclo ./Drivers/CMSIS/system_stm32f0xx.d ./Drivers/CMSIS/system_stm32f0xx.o ./Drivers/CMSIS/system_stm32f0xx.su

.PHONY: clean-Drivers-2f-CMSIS

