/*
 * global.h
 *
 *  Created on: 2024年9月8日
 *      Author: lyj98
 */

#ifndef USER_GLOBAL_H_
#define USER_GLOBAL_H_
#include "ch32v20x.h"
#include "u8g2/u8g2.h"

#define VERSION "Ver 1.1.6"

#define STARUP_FLAG 0x20

#define KEY_OUT GPIO_Pin_1
#define BEEP_OUT GPIO_Pin_9
#define LED_OUT GPIO_Pin_12
#define KEY_1_IN GPIO_Pin_13
#define KEY_2_IN GPIO_Pin_14
#define POWBOTTON_OUT GPIO_Pin_3
#define USB_SW_OUT GPIO_Pin_15
#define BAT_ADC_PIN GPIO_Pin_2

#define KEY_OUT_PORT GPIOA
#define BEEP_OUT_PORT GPIOB
#define LED_OUT_PORT GPIOB
#define KEY_1_IN_PORT GPIOB
#define KEY_2_IN_PORT GPIOB
#define POWBOTTON_PORT GPIOA
#define BAT_ADC_PORT GPIOA

#define MAX_WPM 99
#define MIN_WPN 1

#define DEF_WPM 20
#define DEF_DOT_LEN 1
#define DEF_DASH_LEN 3
#define DEF_BREAK_LEN 1
#define DEF_LETTER_BREAK_LEN 3
#define DEF_WORD_BREAK_LEN 7

#define DEF_BT1_FUN_INDEX 0
#define DEF_BT2_FUN_INDEX 1

#define BUFFSIZE 2048
#define INPUTZONE_SIZE 2048
#define MAXSAVEBUFSIZE (BUFFSIZE - 4)

#define MSG_ADDR 0x0800D000
#define MSG_ZONE_SIZE 2048
#define U8G2_WITHOUT_UNICODE
#define U8G2_WITHOUT_FONT_ROTATION

#define EEPROM_CONFIG_ADDR 0x6000

#define MSG_NUM 12

struct Morse_Config {
    uint8_t dot_len;
    uint8_t dash_len;
    uint8_t break_len;
    uint8_t letter_break_len;
    uint8_t word_break_len;
    uint8_t cut_num;
};

struct Button_Func {
    uint8_t bt1_func_index;
    void (* bt1_func)();
    uint8_t bt2_func_index;
    void (* bt2_func)();
};

struct Config {
    uint8_t beeper;
    uint8_t mode;  // 模式        0：按下立即输出         1：回车输出
    int8_t wpm;
    uint8_t initial_startup;
    struct Morse_Config morse_config;
    struct Button_Func button_func;
};

struct Train_Info{
    uint8_t methon; //方式 0：KOCH 1：顺序
    uint8_t lesson; //当前进度
    uint8_t group_num; //训练字母组数
    uint8_t letter_num_per_group; //每组字母数量
};

extern struct Config config;
extern uint8_t msg[];

extern struct Train_Info train_info;

extern uint8_t stge, bufCovnMark, keyboard_in, caps_lock_stg, saving, disp_menu,
    menu_item, disp_ver, disp_morse_conf, disp_button_func, morse_conf_item, button_conf_item ,curse_flash, disp_confirm_reset, disp_train_menu, tain_menu_item;
extern char inputBuff[], outputBuff[];
extern int sendCount, send_now;
extern uint32_t inputBuffSize, outputBuffSize;
extern uint16_t bat_adc_val;

extern struct Config config;

extern u8g2_t u8g2;

void WriteConfigEEPROM();
void ReadConfigEEPROM();
void ResetConfig();

#endif /* USER_GLOBAL_H_ */
