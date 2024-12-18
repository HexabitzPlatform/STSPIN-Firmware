################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/MotorControl/subdir.mk
-include Drivers/STM32F0xx_HAL_Driver/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Command_Driver_APIs/subdir.mk
-include Application/User/subdir.mk
-include Application/Startup/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := BLDC_STSPIN_KIT
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
BLDC_STSPIN_KIT.elf \

MAP_FILES += \
BLDC_STSPIN_KIT.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
BLDC_STSPIN_KIT.list \


# All Target
all: main-build

# Main-build Target
main-build: BLDC_STSPIN_KIT.elf secondary-outputs

# Tool invocations
BLDC_STSPIN_KIT.elf BLDC_STSPIN_KIT.map: $(OBJS) $(USER_OBJS) C:\Users\Control\Documents\Hexabitz\Hexabitz\ BLDC\en.STSW-ROBOT-1_R1_LL\EVALKIT_ROBOT_1_SDK544_Positioning_LL\STM32CubeIDE\STM32F031C6TX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "BLDC_STSPIN_KIT.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0 -T"C:\Users\Control\Documents\Hexabitz\Hexabitz BLDC\en.STSW-ROBOT-1_R1_LL\EVALKIT_ROBOT_1_SDK544_Positioning_LL\STM32CubeIDE\STM32F031C6TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="BLDC_STSPIN_KIT.map" -Wl,--gc-sections -static -L../../MCSDK_v5.4.4/MotorControl/lib --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

BLDC_STSPIN_KIT.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "BLDC_STSPIN_KIT.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) BLDC_STSPIN_KIT.elf BLDC_STSPIN_KIT.list BLDC_STSPIN_KIT.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
