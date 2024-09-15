/*
 * global.h
 *
 *  Created on: 2024Äê9ÔÂ8ÈÕ
 *      Author: lyj98
 */

#ifndef USER_GLOBAL_H_
#define USER_GLOBAL_H_

#include "ch32v20x.h"

#define KEY_OUT     GPIO_Pin_1
#define BEEP_OUT    GPIO_Pin_9
#define LED_OUT     GPIO_Pin_12
#define KEY_1_IN    GPIO_Pin_13
#define KEY_2_IN    GPIO_Pin_14

#define KEY_OUT_PORT    GPIOA
#define BEEP_OUT_PORT   GPIOB
#define LED_OUT_PORT    GPIOB
#define KEY_1_IN_PORT   GPIOB
#define KEY_2_IN_PORT   GPIOB

#define MAX_WPM 99
#define MIN_WPN 1

#define BUFFSIZE 512

#define CONFIG_ADDR 0x0800C000
#define MSG_ADDR 0x0800D000
#define MSG_ZONE_SIZE 0x0200

#define MSG_NUM 12

struct Config {
    uint8_t beeper;
    uint8_t mode;
    int8_t wpm;
    uint8_t initial_startup;
};

extern struct Config config;
extern uint8_t msg[];

extern uint8_t stge, mode, wpm, beeper, bufCovnMark, keyboard_in, saving;
extern uint8_t inputBuff[], outputBuff[];
extern int sendCount;
extern uint32_t inputBuffSize, outputBuffSize;

extern struct Config config;

#endif /* USER_GLOBAL_H_ */
