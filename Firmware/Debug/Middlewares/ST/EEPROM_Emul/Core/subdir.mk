################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/EEPROM_Emul/Core/eeprom_emul.c \
../Middlewares/ST/EEPROM_Emul/Core/flash_interface.c 

C_DEPS += \
./Middlewares/ST/EEPROM_Emul/Core/eeprom_emul.d \
./Middlewares/ST/EEPROM_Emul/Core/flash_interface.d 

OBJS += \
./Middlewares/ST/EEPROM_Emul/Core/eeprom_emul.o \
./Middlewares/ST/EEPROM_Emul/Core/flash_interface.o 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/EEPROM_Emul/Core/%.o Middlewares/ST/EEPROM_Emul/Core/%.su: ../Middlewares/ST/EEPROM_Emul/Core/%.c Middlewares/ST/EEPROM_Emul/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WLE5xx -c -I../Core/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middlewares-2f-ST-2f-EEPROM_Emul-2f-Core

clean-Middlewares-2f-ST-2f-EEPROM_Emul-2f-Core:
	-$(RM) ./Middlewares/ST/EEPROM_Emul/Core/eeprom_emul.d ./Middlewares/ST/EEPROM_Emul/Core/eeprom_emul.o ./Middlewares/ST/EEPROM_Emul/Core/eeprom_emul.su ./Middlewares/ST/EEPROM_Emul/Core/flash_interface.d ./Middlewares/ST/EEPROM_Emul/Core/flash_interface.o ./Middlewares/ST/EEPROM_Emul/Core/flash_interface.su

.PHONY: clean-Middlewares-2f-ST-2f-EEPROM_Emul-2f-Core

