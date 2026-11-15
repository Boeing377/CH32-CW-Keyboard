/*
 * morse_send.h
 *
 *  Created on: 2024��8��19��
 *      Author: lyj98
 */

#ifndef USER_MORSE_SEND_H_
#define USER_MORSE_SEND_H_

#include "global.h"

/* Single-byte codes stored in message buffers, after the cut-number codes. */
#define MORSE_CHAR_UMLAUT_U 0x0B
#define MORSE_CHAR_CUSTOM_QUESTION 0x0C

extern const char KochAplhaTable[];

void TIM2_Init (uint16_t arr, uint16_t psc);
void TIM2_ChangBase (uint16_t arr, uint16_t psc);
void bufCovn (uint8_t theChar);
uint8_t Morse_CanEncodeChar (uint8_t theChar);
void add_wpm (int num);
void sub_wpm (int num);
void starSending();
void endSending();
void bufCovnErrorDots(void);

#endif /* USER_MORSE_SEND_H_ */
