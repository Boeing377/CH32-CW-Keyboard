################################################################################
# MRS Version: 1.9.2
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v20x_it.c \
../User/global.c \
../User/main.c \
../User/morse_send.c \
../User/system_ch32v20x.c 

OBJS += \
./User/ch32v20x_it.o \
./User/global.o \
./User/main.o \
./User/morse_send.o \
./User/system_ch32v20x.o 

C_DEPS += \
./User/ch32v20x_it.d \
./User/global.d \
./User/main.d \
./User/morse_send.d \
./User/system_ch32v20x.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\MRS_DATA\workspace\CH32V203USBKEY\Debug" -I"C:\MRS_DATA\workspace\CH32V203USBKEY\Core" -I"C:\MRS_DATA\workspace\CH32V203USBKEY\User" -I"C:\MRS_DATA\workspace\CH32V203USBKEY\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

