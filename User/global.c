/*
 * global.c
 *
 *  Created on: 2024��9��8��
 *      Author: lyj98
 */

#include "global.h"

struct Config config;

uint8_t stge = 0;   //��ǰ״̬ 0��stb            1��sending
uint8_t mode = 0;   //ģʽ        0�������������         1���س����
uint8_t beeper = 0; //���������ѡ��
uint8_t wpm = 20;    //����ٶ�
uint8_t bufCovnMark = 0;
uint8_t keyboard_in = 0;
uint8_t saving = 0;
int sendCount = 0;

uint8_t msg[MSG_NUM];

uint8_t inputBuff[BUFFSIZE], outputBuff[BUFFSIZE];  //����buffer�����buffer

uint32_t inputBuffSize = 0, outputBuffSize = 0;

