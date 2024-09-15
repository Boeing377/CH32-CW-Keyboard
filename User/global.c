/*
 * global.c
 *
 *  Created on: 2024年9月8日
 *      Author: lyj98
 */

#include "global.h"

struct Config config;

uint8_t stge = 0;   //当前状态 0：stb            1：sending
uint8_t mode = 0;   //模式        0：按下立即输出         1：回车输出
uint8_t beeper = 0; //蜂鸣器输出选择
uint8_t wpm = 20;    //输出速度
uint8_t bufCovnMark = 0;
uint8_t keyboard_in = 0;
uint8_t saving = 0;
int sendCount = 0;

uint8_t msg[MSG_NUM];

uint8_t inputBuff[BUFFSIZE], outputBuff[BUFFSIZE];  //键入buffer和输出buffer

uint32_t inputBuffSize = 0, outputBuffSize = 0;

