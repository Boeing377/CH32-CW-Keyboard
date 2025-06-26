#ifndef __BUTTON_FUNC_H__
#define  __BUTTON_FUNC_H__

#include "debug.h"
#include "global.h"
#include "morse_send.h"
#include "USB_Host/usb_host_conf.h"
#include "string.h"
#include "ch32_u8g2.h"
#include "screen_disp.h"
#include "i2c_eeprom.h"

void ButtonChangeBeeper();

void ButtonChangeMode();

void ButtonOpenMenu();


#endif
