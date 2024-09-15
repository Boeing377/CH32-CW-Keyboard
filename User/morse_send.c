/*
 * morse_send.c
 *
 *  Created on: 2024Äê8ÔÂ19ÈÕ
 *      Author: lyj98
 */
#include "debug.h"
#include "ch32v20x.h"
#include "morse_send.h"

void ReadConfig();
void WriteConfig();

uint32_t sendBufLen = 0;
uint8_t sendbuf[26];    //max for 6 dash 3*6 + 5 + 3

struct MorseCodeMap {
    uint8_t len;
    uint8_t code;
} morse_code_map[46] = { { 2, 0b01000000 }, //a  1
        { 4, 0b10000000 }, //b  2
        { 4, 0b10100000 }, //c  3
        { 3, 0b10000000 }, //d  4
        { 1, 0b00000000 }, //e  5
        { 4, 0b00100000 }, //f  6
        { 3, 0b11000000 }, //g  7
        { 4, 0b00000000 }, //h  8
        { 2, 0b00000000 }, //i  9
        { 4, 0b01110000 }, //j  10
        { 3, 0b10100000 }, //k  11
        { 4, 0b01000000 }, //l  12
        { 2, 0b11000000 }, //m  13
        { 2, 0b10000000 }, //n  14
        { 3, 0b11100000 }, //o  15
        { 4, 0b01100000 }, //p  16
        { 4, 0b11010000 }, //q  17
        { 3, 0b10100000 }, //r  18
        { 3, 0b00000000 }, //s  19
        { 1, 0b10000000 }, //t  20
        { 3, 0b00100000 }, //u  21
        { 4, 0b00010000 }, //v  22
        { 3, 0b01100000 }, //w  23
        { 4, 0b10010000 }, //x  24
        { 4, 0b10110000 }, //y  25
        { 4, 0b11000000 }, //z  26
        { 5, 0b11111000 }, //0  27
        { 5, 0b01111000 }, //1  28
        { 5, 0b00111000 }, //2  29
        { 5, 0b00011000 }, //3  30
        { 5, 0b00001000 }, //4  31
        { 5, 0b00000000 }, //5  32
        { 5, 0b10000000 }, //6  33
        { 5, 0b11000000 }, //7  34
        { 5, 0b11100000 }, //8  35
        { 5, 0b11110000 }, //9  36
        { 6, 0b00110000 }, //?  37
        { 6, 0b10101100 }, //!  38
        { 6, 0b01010100 }, //.  39
        { 6, 0b11001100 }, //,  40
        { 6, 0b10101000 }, //;  41
        { 6, 0b11100000 }, //:  42
        { 5, 0b01010000 }, //+  43
        { 6, 0b10000100 }, //-  44
        { 5, 0b10010000 }, ///  45
        { 5, 0b10001000 }  //=  46
};

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM2_Init(uint16_t arr, uint16_t psc) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };
    NVIC_InitTypeDef NVIC_InitStructure = { 0 };

    /* Enable timer3 clock */
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE);

    /* Initialize timer3 */
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure);

    /* Enable updating timer3 interrupt */
    TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE);

    /* Configure timer3 interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable timer3 */
    //TIM_Cmd( TIM2, ENABLE);
    /* Enable timer3 interrupt */
    //NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_ChangBase(uint16_t arr, uint16_t psc) {
    if (stge) {
        NVIC_DisableIRQ(TIM2_IRQn);
        TIM_Cmd(TIM2, DISABLE);
    }
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure);

    if (stge) {
        TIM_Cmd( TIM2, ENABLE);
        NVIC_EnableIRQ(TIM2_IRQn);
    }
}

void add_wpm(int num) {
    config.wpm += num;
    if (config.wpm > MAX_WPM) {
        config.wpm = MAX_WPM;
    }
//    printf("wpm %d\r\n", wpm);
    TIM2_ChangBase(3999, (60 * SystemCoreClock / (config.wpm * 50 * 1000) - 1));
    WriteConfig();
}

void sub_wpm(int num) {
    config.wpm -= num;
    if (config.wpm < MIN_WPN) {
        config.wpm = MIN_WPN;
    }
//    printf("wpm %d\r\n", wpm);
    TIM2_ChangBase(3999, (60 * SystemCoreClock / (config.wpm * 50 * 1000) - 1));
    WriteConfig();
}

void starSending() {
    if (!stge) {
        stge = 1;
        bufCovnMark = 1;
        if (config.mode) {
            sendCount = 0;
        }
        TIM_Cmd( TIM2, ENABLE);
        NVIC_EnableIRQ(TIM2_IRQn);
    }
}

void endSending() {
    if (stge) {
        stge = 0;
        NVIC_DisableIRQ(TIM2_IRQn);
        TIM_Cmd(TIM2, DISABLE);
        GPIO_WriteBit(KEY_OUT_PORT, KEY_OUT, Bit_RESET);
        GPIO_WriteBit(BEEP_OUT_PORT, BEEP_OUT, Bit_RESET);
    }
}

void bufCovn(uint8_t theChar) {
    int i, j = 0, k = 0;
    if (theChar >= 'a' && theChar <= 'z') {
        for (j = 0; j < morse_code_map[theChar - 'a'].len; j++) {
            i = ((uint8_t) (morse_code_map[(theChar - 'a')].code >> (7 - j))
                    & (uint8_t) 0x01);
            if (i) {
                sendbuf[k] = sendbuf[k + 1] = sendbuf[k + 2] = 1;
                sendbuf[k + 3] = 0;
                k = k + 4;
            } else {
                sendbuf[k] = 1;
                sendbuf[k + 1] = 0;
                k = k + 2;
            }
        }

    } else if (theChar >= 'A' && theChar <= 'Z') {
        for (j = 0; j < morse_code_map[theChar - 'A'].len; j++) {
            i = ((uint8_t) (morse_code_map[(theChar - 'A')].code >> (7 - j))
                    & (uint8_t) 0x01);
            if (i) {
                sendbuf[k] = sendbuf[k + 1] = sendbuf[k + 2] = 1;
                sendbuf[k + 3] = 0;
                k = k + 4;
            } else {
                sendbuf[k] = 1;
                sendbuf[k + 1] = 0;
                k = k + 2;
            }
        }
    } else if (theChar >= '0' && theChar <= '9') {
        for (j = 0; j < morse_code_map[(theChar - '0' + 26)].len; j++) {
            i = ((uint8_t) (morse_code_map[(theChar - '0')].code >> (7 - j))
                    & (uint8_t) 0x01);
            if (i) {
                sendbuf[k] = sendbuf[k + 1] = sendbuf[k + 2] = 1;
                sendbuf[k + 3] = 0;
                k = k + 4;
            } else {
                sendbuf[k] = 1;
                sendbuf[k + 1] = 0;
                k = k + 2;
            }
        }
    } else if (theChar == ' ') {
        sendbuf[k++] = 0;
        sendbuf[k++] = 0;
    } else {
        switch (theChar) {
        case '?':
            for (j = 0; j < morse_code_map[36].len; j++) {
                i = ((uint8_t) (morse_code_map[36].code >> (7 - j))
                        & (uint8_t) 0x01);
                if ((morse_code_map[36].code >> 7 - j) & 0x01) {
                    sendbuf[k] = sendbuf[k + 1] = sendbuf[k + 2] = 1;
                    sendbuf[k + 3] = 0;
                    k = k + 4;
                } else {
                    sendbuf[k] = 1;
                    sendbuf[k + 1] = 0;
                    k = k + 2;
                }
            }
            break;
        case '!':
            for (j = 0; j < morse_code_map[37].len; j++) {
                i = ((uint8_t) (morse_code_map[37].code >> (7 - j))
                        & (uint8_t) 0x01);
                if ((morse_code_map[37].code >> 7 - j) & 0x01) {
                    sendbuf[k] = sendbuf[k + 1] = sendbuf[k + 2] = 1;
                    sendbuf[k + 3] = 0;
                    k = k + 4;
                } else {
                    sendbuf[k] = 1;
                    sendbuf[k + 1] = 0;
                    k = k + 2;
                }
            }
            break;
        case '.':
            for (j = 0; j < morse_code_map[38].len; j++) {
                i = ((uint8_t) (morse_code_map[38].code >> (7 - j))
                        & (uint8_t) 0x01);
                if ((morse_code_map[38].code >> 7 - j) & 0x01) {
                    sendbuf[k] = sendbuf[k + 1] = sendbuf[k + 2] = 1;
                    sendbuf[k + 3] = 0;
                    k = k + 4;
                } else {
                    sendbuf[k] = 1;
                    sendbuf[k + 1] = 0;
                    k = k + 2;
                }
            }
            break;
        case ',':
            for (j = 0; j < morse_code_map[39].len; j++) {
                i = ((uint8_t) (morse_code_map[39].code >> (7 - j))
                        & (uint8_t) 0x01);
                if ((morse_code_map[39].code >> 7 - j) & 0x01) {
                    sendbuf[k] = sendbuf[k + 1] = sendbuf[k + 2] = 1;
                    sendbuf[k + 3] = 0;
                    k = k + 4;
                } else {
                    sendbuf[k] = 1;
                    sendbuf[k + 1] = 0;
                    k = k + 2;
                }
            }
            break;
        case ';':
            for (j = 0; j < morse_code_map[40].len; j++) {
                i = ((uint8_t) (morse_code_map[40].code >> (7 - j))
                        & (uint8_t) 0x01);
                if ((morse_code_map[40].code >> 7 - j) & 0x01) {
                    sendbuf[k] = sendbuf[k + 1] = sendbuf[k + 2] = 1;
                    sendbuf[k + 3] = 0;
                    k = k + 4;
                } else {
                    sendbuf[k] = 1;
                    sendbuf[k + 1] = 0;
                    k = k + 2;
                }
            }
            break;
        case ':':
            for (j = 0; j < morse_code_map[41].len; j++) {
                i = ((uint8_t) (morse_code_map[41].code >> (7 - j))
                        & (uint8_t) 0x01);
                if ((morse_code_map[41].code >> 7 - j) & 0x01) {
                    sendbuf[k] = sendbuf[k + 1] = sendbuf[k + 2] = 1;
                    sendbuf[k + 3] = 0;
                    k = k + 4;
                } else {
                    sendbuf[k] = 1;
                    sendbuf[k + 1] = 0;
                    k = k + 2;
                }
            }
            break;
        case '+':
            for (j = 0; j < morse_code_map[42].len; j++) {
                i = ((uint8_t) (morse_code_map[42].code >> (7 - j))
                        & (uint8_t) 0x01);
                if ((morse_code_map[42].code >> 7 - j) & 0x01) {
                    sendbuf[k] = sendbuf[k + 1] = sendbuf[k + 2] = 1;
                    sendbuf[k + 3] = 0;
                    k = k + 4;
                } else {
                    sendbuf[k] = 1;
                    sendbuf[k + 1] = 0;
                    k = k + 2;
                }
            }
            break;
        case '-':
            for (j = 0; j < morse_code_map[43].len; j++) {
                i = ((uint8_t) (morse_code_map[43].code >> (7 - j))
                        & (uint8_t) 0x01);
                if ((morse_code_map[43].code >> 7 - j) & 0x01) {
                    sendbuf[k] = sendbuf[k + 1] = sendbuf[k + 2] = 1;
                    sendbuf[k + 3] = 0;
                    k = k + 4;
                } else {
                    sendbuf[k] = 1;
                    sendbuf[k + 1] = 0;
                    k = k + 2;
                }
            }
            break;
        case '/':
            for (j = 0; j < morse_code_map[44].len; j++) {
                i = ((uint8_t) (morse_code_map[44].code >> (7 - j))
                        & (uint8_t) 0x01);
                if ((morse_code_map[44].code >> 7 - j) & 0x01) {
                    sendbuf[k] = sendbuf[k + 1] = sendbuf[k + 2] = 1;
                    sendbuf[k + 3] = 0;
                    k = k + 4;
                } else {
                    sendbuf[k] = 1;
                    sendbuf[k + 1] = 0;
                    k = k + 2;
                }
            }
            break;
        case '=':
            for (j = 0; j < morse_code_map[45].len; j++) {
                i = ((uint8_t) (morse_code_map[45].code >> (7 - j))
                        & (uint8_t) 0x01);
                if ((morse_code_map[45].code >> 7 - j) & 0x01) {
                    sendbuf[k] = sendbuf[k + 1] = sendbuf[k + 2] = 1;
                    sendbuf[k + 3] = 0;
                    k = k + 4;
                } else {
                    sendbuf[k] = 1;
                    sendbuf[k + 1] = 0;
                    k = k + 2;
                }
            }
            break;
        default:
            break;
        }
    }
    sendbuf[k] = 0;
    k = k + 1;
    sendBufLen = k;
}

void TIM2_IRQHandler(void) {
    static int count = 0;
    if (TIM_GetITStatus( TIM2, TIM_IT_Update) != RESET) {
        /* Clear interrupt flag */
        TIM_ClearITPendingBit( TIM2, TIM_IT_Update);

        if (bufCovnMark) {
            if (config.mode) {
                bufCovn(outputBuff[sendCount]);
            } else {
                bufCovn(inputBuff[sendCount]);
            }
            sendCount++;
            bufCovnMark = 0;
        }

        if (count < sendBufLen) {
            if (sendbuf[count]) {
                GPIO_WriteBit(KEY_OUT_PORT, KEY_OUT, Bit_SET);
                if (config.beeper)
                    GPIO_WriteBit(BEEP_OUT_PORT, BEEP_OUT, Bit_SET);
            } else {
                GPIO_WriteBit(KEY_OUT_PORT, KEY_OUT, Bit_RESET);
                if (config.beeper)
                    GPIO_WriteBit(BEEP_OUT_PORT, BEEP_OUT, Bit_RESET);
            }
            count++;
        } else {
            count = 0;
            if (config.mode) {
                if (sendCount < outputBuffSize) {
                    bufCovnMark = 1;
                } else {
                    sendCount = 0;
                    outputBuff[0] = '\0';
                    outputBuffSize = 0;
                    endSending();
                }
            } else {
                if (sendCount < inputBuffSize) {
                    bufCovnMark = 1;
                } else {
                    endSending();
                }
            }

        }
    }
}
