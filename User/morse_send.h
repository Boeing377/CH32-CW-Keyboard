/*
 * morse_send.h
 *
 *  Created on: 2024Äê8ÔÂ19ÈÕ
 *      Author: lyj98
 */

#ifndef USER_MORSE_SEND_H_
#define USER_MORSE_SEND_H_

#include "global.h"

void TIM2_Init(uint16_t arr, uint16_t psc);
void bufCovn(uint8_t theChar);
void add_wpm(int num);
void sub_wpm(int num);
void starSending();
void endSending();

#endif /* USER_MORSE_SEND_H_ */
