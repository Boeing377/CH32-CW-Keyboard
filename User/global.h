/*
 * global.h
 *
 *  Created on: 2024��9��8��
 *      Author: lyj98
 */

#ifndef USER_GLOBAL_H_
#define USER_GLOBAL_H_
#include "ch32v20x.h"
#include "u8g2/u8g2.h"

#ifndef COMPARE_FOR_VERSION_WITH_EEPROM
#define COMPARE_FOR_VERSION_WITH_EEPROM 1
#endif

/* 固件版本类型：1=定制版本，0=通用版本 */
#ifndef FW_CUSTOM_VERSION
#define FW_CUSTOM_VERSION 0
#endif

/* 定制版本的接收者（仅 FW_CUSTOM_VERSION=1 时生效） */
#ifndef FW_CUSTOM_RECIPIENT
#define FW_CUSTOM_RECIPIENT "BG7WG"
#endif

/* 版本号由 tools/gen_version.ps1 自动生成，Git 提交后 BUILD 号自动递增 */
#include "version.h"

#define STARUP_FLAG      0x25
#define OLD_STARUP_FLAG  0x15  /* v1.x firmware used 0x15; accepted during upgrade detection */

/* ── Legacy upgrade detection ─────────────────────────────────── */
/* Old no-EEPROM flash firmware stored Config at 0x0800CF00.       */
/* We probe that address on boot to decide whether this is an      */
/* in-place upgrade (old data survived ISP programming) or a       */
/* fresh flash onto a blank / pirated chip.                        */
/* Only active in FLASH (no-EEPROM) builds.                        */
/* ─────────────────────────────────────────────────────────────── */
#if !COMPARE_FOR_VERSION_WITH_EEPROM
#define OLD_CONFIG_ADDR     0x0800CF00UL
#endif

#define KEY_OUT GPIO_Pin_1
#define BEEP_OUT GPIO_Pin_9
#define LED_OUT GPIO_Pin_12
#define KEY_1_IN GPIO_Pin_13
#define KEY_2_IN GPIO_Pin_14
#define POWBOTTON_OUT GPIO_Pin_3
#define USB_SW_OUT GPIO_Pin_15
#if COMPARE_FOR_VERSION_WITH_EEPROM
#define BAT_ADC_PIN GPIO_Pin_2
#endif

#define KEY_OUT_PORT GPIOA
#define BEEP_OUT_PORT GPIOB
#define LED_OUT_PORT GPIOB
#define KEY_1_IN_PORT GPIOB
#define KEY_2_IN_PORT GPIOB
#define POWBOTTON_PORT GPIOA
#if COMPARE_FOR_VERSION_WITH_EEPROM
#define BAT_ADC_PORT GPIOA
#endif

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

#define DEF_REPEAT_COUNT 3
#define DEF_REPEAT_INTERVAL_S 5

#define KEYBOARD_LAYOUT_QWERTY 0
#define KEYBOARD_LAYOUT_AZERTY 1
#define KEYBOARD_LAYOUT_COUNT 2

#ifndef DEFAULT_KEYBOARD
#define DEFAULT_KEYBOARD KEYBOARD_LAYOUT_QWERTY
#endif

#define BUFFSIZE 2048
#define INPUTZONE_SIZE 2048

#if COMPARE_FOR_VERSION_WITH_EEPROM
#define MSG_ADDR 0x0800D000
#define MSG_ZONE_SIZE 2048
#else
#define MSG_ADDR 0x0800E800UL
#define MSG_ZONE_SIZE 0x0200
#endif

#define MAXSAVEBUFSIZE (MSG_ZONE_SIZE - 4)
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
    uint32_t bt1_func_reserved;
    uint8_t bt2_func_index;
    uint32_t bt2_func_reserved;
};

struct Repeat_Config {
    uint16_t repeat_count;
    uint16_t repeat_interval_s;
};

struct Config {
    uint8_t beeper;
    uint8_t mode;  // ģʽ        0�������������         1���س����
    int8_t wpm;
    uint8_t initial_startup;
    struct Morse_Config morse_config;
    struct Button_Func button_func;
    uint8_t keyboard_layout;
    struct Repeat_Config repeat_config;
};

/* Training method constants */
#define TRAIN_METHOD_KOCH  0
#define TRAIN_METHOD_SEQU  1
#define TRAIN_METHOD_FREE  2
#define TRAIN_METHOD_COUNT 3

/* Training phases */
#define TRAIN_PHASE_IDLE    0
#define TRAIN_PHASE_READY   1  /* 1s pre-roll prompt before audio */
#define TRAIN_PHASE_RUNNING 2  /* audio playing, user can type */
#define TRAIN_PHASE_SCORING 3  /* show score comparison */

/* Training setting item counts per method */
#define TRAIN_SETTING_ITEMS_KOCH  5  /* CHAR/GRP, WPM, GAP, TIME, BACK */
#define TRAIN_SETTING_ITEMS_FREE  3  /* WPM, TIME, BACK */

#if COMPARE_FOR_VERSION_WITH_EEPROM
struct Train_Info{
    uint8_t methon;
    uint8_t lesson;
    uint8_t chars_per_group;     /* default 5 */
    uint8_t group_gap_spaces;    /* spaces between groups, default 1 */
    uint8_t train_duration_min;  /* 1-5 minutes, default 2 */
};
#endif

extern struct Config config;
extern uint8_t msg[];

#if COMPARE_FOR_VERSION_WITH_EEPROM
extern struct Train_Info train_info;
#endif

extern uint8_t stge, bufCovnMark, keyboard_in, caps_lock_stg, saving, disp_menu,
    menu_item, disp_ver, disp_morse_conf, disp_button_func, morse_conf_item, button_conf_item ,curse_flash, disp_confirm_reset, disp_repeat_conf, repeat_conf_item, disp_repeat_input, repeat_input_target;
#if COMPARE_FOR_VERSION_WITH_EEPROM
extern uint8_t disp_train_menu, tain_menu_item;
extern uint8_t disp_training, disp_train_setting, train_setting_item;
extern uint8_t train_setting_wpm_input;
extern uint16_t train_setting_wpm_val;
extern uint8_t train_setting_wpm_pos;
extern char train_generated_text[];
extern uint16_t train_text_len;
extern char train_buf[];
extern uint16_t train_buf_size;
extern uint8_t train_phase;
extern uint8_t train_beeper_saved;
extern uint8_t train_mode_saved;
extern uint8_t train_score_scroll;
extern char    train_align_user[];
extern uint8_t train_align_match[];
extern uint16_t train_align_len;
#endif
extern uint16_t repeat_input_value;
extern uint8_t repeat_input_pos;
extern uint8_t cursor_edit_mode;
extern uint16_t cursor_pos;
extern uint8_t send_correction_error;
extern char inputBuff[], outputBuff[];
extern int sendCount, send_now;
extern uint32_t inputBuffSize, outputBuffSize;

/* Repeat mode state */
#define REPEAT_PHASE_COUNTDOWN 0
#define REPEAT_PHASE_SENDING    1
#define REPEAT_PHASE_BUF_PEND   2
extern uint8_t repeat_active;
extern uint8_t repeat_phase;
extern uint32_t repeat_counter;
extern uint16_t repeat_countdown_s;
extern char repeat_saved_text[];
extern uint32_t repeat_saved_text_size;
#if COMPARE_FOR_VERSION_WITH_EEPROM
extern uint16_t bat_adc_val;
#endif
extern volatile uint32_t app_tick_ms;

extern struct Config config;

extern u8g2_t u8g2;

void WriteConfigEEPROM();
void ReadConfigEEPROM();
void ReadSavedMsgEEPROM (uint8_t sn);
void WriteMsgEEPROM (uint8_t sn);
void ResetConfig();

#endif /* USER_GLOBAL_H_ */
