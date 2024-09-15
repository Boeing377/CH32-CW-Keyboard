################################################################################
# MRS Version: 1.9.2
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/OLED/fonts.c \
../User/OLED/oled.c 

OBJS += \
./User/OLED/fonts.o \
./User/OLED/oled.o 

C_DEPS += \
./User/OLED/fonts.d \
./User/OLED/oled.d 


# Each subdirectory must supply rules for building sources it contributes
User/OLED/%.o: ../User/OLED/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\MRS_DATA\workspace\CH32V203USBKEY\Debug" -I"C:\MRS_DATA\workspace\CH32V203USBKEY\Core" -I"C:\MRS_DATA\workspace\CH32V203USBKEY\User" -I"C:\MRS_DATA\workspace\CH32V203USBKEY\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

