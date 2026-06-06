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

#define BUTTON_ID_1 0
#define BUTTON_ID_2 1

#define BUTTON_ACTION_BEEPER 0
#define BUTTON_ACTION_MODE 1
#define BUTTON_ACTION_OPEN_MENU 2
#define BUTTON_ACTION_COUNT 3

void ButtonChangeBeeper();

void ButtonChangeMode();

void ButtonOpenMenu();

void ButtonHandleKey1LongPress();

void ButtonHandleKey2LongPress();

void ButtonApplyActionIndex (uint8_t button_id, uint8_t action_index);

void ButtonApplyConfiguredActions (void);

void ButtonExecuteConfiguredAction (uint8_t button_id);


#endif
