################################################################################
# MRS Version: 1.9.2
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/USB_Host/USB_KB_Key_app.c \
../User/USB_Host/ch32v20x_usbfs_host.c \
../User/USB_Host/usb_host_hid.c \
../User/USB_Host/usb_host_hub.c 

OBJS += \
./User/USB_Host/USB_KB_Key_app.o \
./User/USB_Host/ch32v20x_usbfs_host.o \
./User/USB_Host/usb_host_hid.o \
./User/USB_Host/usb_host_hub.o 

C_DEPS += \
./User/USB_Host/USB_KB_Key_app.d \
./User/USB_Host/ch32v20x_usbfs_host.d \
./User/USB_Host/usb_host_hid.d \
./User/USB_Host/usb_host_hub.d 


# Each subdirectory must supply rules for building sources it contributes
User/USB_Host/%.o: ../User/USB_Host/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\MRS_DATA\workspace\CH32V203USBKEY\Debug" -I"C:\MRS_DATA\workspace\CH32V203USBKEY\Core" -I"C:\MRS_DATA\workspace\CH32V203USBKEY\User" -I"C:\MRS_DATA\workspace\CH32V203USBKEY\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

