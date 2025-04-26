/*
 * global.c
 *
 *  Created on: 2024年9月8日
 *      Author: lyj98
 */

#include "global.h"

struct Config config;

uint8_t stge = 0;         // 当前状态           0：stb        1：sending
uint8_t bufCovnMark = 0;
uint8_t keyboard_in = 0;  // 键盘接入状态    0：未接入           1：已接入
uint8_t caps_lock_stg = 0;
uint8_t saving = 0;
uint8_t disp_menu = 0;
uint8_t menu_item = 0;
uint8_t disp_ver = 0;
uint8_t disp_morse_conf = 0;
uint8_t morse_conf_item = 0;
int sendCount = 0, send_now;
uint8_t curse_flash = 0;
uint16_t bat_adc_val;

uint8_t msg[MSG_NUM];

char inputBuff[INPUTZONE_SIZE], outputBuff[INPUTZONE_SIZE];  // 键入buffer和输出buffer

uint32_t inputBuffSize = 0, outputBuffSize = 0;

u8g2_t u8g2;