/*
 * morse_send.c
 *
 *  Created on: 2024��8��19��
 *      Author: lyj98
 */
#include "debug.h"
#include "ch32v20x.h"
#include "morse_send.h"

void ReadConfig();
void WriteConfig();

void ReadConfigEEPROM();
void WriteConfigEEPROM();

uint32_t sendBufLen = 0;
uint8_t sendbuf[256];  // max for 6 dash 6*20+5*20+20

#define SEND_BUF_CAPACITY ((uint16_t)sizeof (sendbuf))

const char KochAplhaTable[41] = {
    'K', 'M', 'U', 'R', 'E', 'S', 'N', 'A', 'P', 'T',
    'L', 'W', 'I', '.', 'J', 'Z', '=', 'F', 'O', 'Y', ',',
'V', 'G', '5', '/', 'Q', '9', '2', 'H', '3', '8',
'B', '?', '4', '7', 'C', '1', 'D', '6', '0', 'X' 
};

struct MorseCodeMap {
    uint8_t len;
    uint8_t code;
};

struct MorseSymbolMap {
    uint8_t symbol;
    uint8_t pattern_index;
};

static const struct MorseCodeMap morse_code_map[] = {
    {2, 0b01000000}, //  a  1 .-
    {4, 0b10000000}, //  b  2 -...
    {4, 0b10100000}, //  c  3 -.-.
    {3, 0b10000000}, //  d  4 -..
    {1, 0b00000000}, //  e  5 .
    {4, 0b00100000}, //  f  6 ..-.
    {3, 0b11000000}, //  g  7 --.
    {4, 0b00000000}, //  h  8 ....
    {2, 0b00000000}, //  i  9 ..
    {4, 0b01110000}, //  j  10 .---
    {3, 0b10100000}, //  k  11 -.-
    {4, 0b01000000}, //  l  12 .-..
    {2, 0b11000000}, //  m  13 --
    {2, 0b10000000}, //  n  14 -.
    {3, 0b11100000}, //  o  15 ---
    {4, 0b01100000}, //  p  16 .--.
    {4, 0b11010000}, //  q  17 --.-
    {3, 0b01000000}, //  r  18 .-.
    {3, 0b00000000}, //  s  19 ...
    {1, 0b10000000}, //  t  20 -
    {3, 0b00100000}, //  u  21 ..-
    {4, 0b00010000}, //  v  22 ...-
    {3, 0b01100000}, //  w  23 .--
    {4, 0b10010000}, //  x  24 -..-
    {4, 0b10110000}, //  y  25 -.--
    {4, 0b11000000}, //  z  26 --..
    {5, 0b11111000}, //  0  27 -----
    {5, 0b01111000}, //  1  28 .----
    {5, 0b00111000}, //  2  29 ..---
    {5, 0b00011000}, //  3  30 ...--
    {5, 0b00001000}, //  4  31 ....-
    {5, 0b00000000}, //  5  32 .....
    {5, 0b10000000}, //  6  33 -....
    {5, 0b11000000}, //  7  34 --...
    {5, 0b11100000}, //  8  35 ---..
    {5, 0b11110000}, //  9  36 ----.
    {6, 0b00110000}, //  ?  37 ..--..
    {6, 0b10101100}, //  !  38 -.-.--
    {6, 0b01010100}, //  .  39 .-.-.-
    {6, 0b11001100}, //  ,  40 --..--
    {6, 0b10101000}, //  ;  41 -.-.-.
    {6, 0b11100000}, //  :  42 ---... 
    {5, 0b01010000}, //  +  43 -.-.-
    {6, 0b10000100}, //  -  44 -....-
    {5, 0b10010000}, //  /  45 -..-.
    {5, 0b10001000}, //  =  46 -...-
    {6, 0b01111000}, //  '  47 .----.
    {6, 0b01001000}, //  "  48 .-..-.
    {5, 0b01101000}, //  &  49 .-...
    {6, 0b01101000}, //  @  50 .--.-.
    {6, 0b00010010}, //  $  51 ...-..-
    {6, 0b00110100}, //  _  52 ..--.-
    {5, 0b10110000}, //  (  53 -.--.
    {6, 0b10110100}  //  )  54 -.--.-
};

static const struct MorseCodeMap morse_num_cut_A[] = {
    {1, 0b11111000}, //  0  27 -
    {2, 0b01111000}, //  1  28 .-
    {3, 0b00111000}, //  2  29 ..-
    {5, 0b00011000}, //  3  30 ...--
    {5, 0b00001000}, //  4  31 ....-
    {5, 0b00000000}, //  5  32 .....
    {5, 0b10000000}, //  6  33 -....
    {5, 0b11000000}, //  7  34 --...
    {3, 0b10000000}, //  8  35 -..
    {2, 0b10000000}  //  9  36 -.
};

static const struct MorseCodeMap morse_num_cut_B[] = {
    {1, 0b11111000}, //  0  27 -
    {2, 0b01111000}, //  1  28 .-
    {3, 0b00111000}, //  2  29 ..-
    {4, 0b00011000}, //  3  30 ...-
    {5, 0b00001000}, //  4  31 ....-
    {1, 0b00000000}, //  5  32 .
    {5, 0b10000000}, //  6  33 -....
    {4, 0b10000000}, //  7  34 -...
    {3, 0b10000000}, //  8  35 -..
    {2, 0b10000000}  //  9  36 -.
};

static const struct MorseCodeMap morse_num_cut_C[] = {
    {1, 0b11111000}, //  0  27 -
    {2, 0b01111000}, //  1  28 .-
    {3, 0b00111000}, //  2  29 ..-
    {3, 0b01111000}, //  3  30 .--
    {4, 0b00011000}, //  4  31 ...-
    {3, 0b00000000}, //  5  32 ...
    {4, 0b10000000}, //  6  33 -...
    {3, 0b11000000}, //  7  34 --.
    {3, 0b10000000}, //  8  35 -..
    {2, 0b10000000}  //  9  36 -.
};

static const struct MorseSymbolMap morse_symbol_map[] = {
    {'?', 36},
    {'!', 37},
    {'.', 38},
    {',', 39},
    {';', 40},
    {':', 41},
    {'+', 42},
    {'-', 43},
    {'/', 44},
    {'=', 45},
    {'\'', 46},
    {'"', 47},
    {'&', 48},
    {'@', 49},
    {'$', 50},
    {'_', 51},
    {'(', 52},
    {')', 53},
};

static uint8_t append_signal (uint8_t level, uint8_t repeat, uint16_t *offset) {
    uint16_t i;

    if ((*offset + repeat) > SEND_BUF_CAPACITY) {
        return 0;
    }

    for (i = 0; i < repeat; i++) {
        sendbuf[(*offset)++] = level;
    }

    return 1;
}

static uint8_t append_mark (uint8_t is_dash, uint16_t *offset) {
    uint8_t pulse_len = is_dash ? config.morse_config.dash_len
                                : config.morse_config.dot_len;

    return append_signal (1, pulse_len, offset) &&
           append_signal (0, config.morse_config.break_len, offset);
}

static const struct MorseCodeMap *find_symbol_pattern (uint8_t theChar) {
    uint8_t i;

    for (i = 0; i < (sizeof (morse_symbol_map) / sizeof (morse_symbol_map[0]));
         i++) {
        if (morse_symbol_map[i].symbol == theChar) {
            return &morse_code_map[morse_symbol_map[i].pattern_index];
        }
    }

    return 0;
}

static const struct MorseCodeMap *select_pattern (uint8_t theChar) {
    if ((theChar >= 'a') && (theChar <= 'z')) {
        return &morse_code_map[theChar - 'a'];
    }

    if ((theChar >= 'A') && (theChar <= 'Z')) {
        return &morse_code_map[theChar - 'A'];
    }

    if ((theChar >= '0') && (theChar <= '9')) {
        uint8_t digit = theChar - '0';

        if (config.morse_config.cut_num == 1) {
            return &morse_num_cut_A[digit];
        }
        if (config.morse_config.cut_num == 2) {
            return &morse_num_cut_B[digit];
        }
        if (config.morse_config.cut_num == 3) {
            return &morse_num_cut_C[digit];
        }

        return &morse_code_map[digit + 26];
    }

    return find_symbol_pattern (theChar);
}

static uint8_t encode_pattern (const struct MorseCodeMap *pattern,
                               uint16_t *offset) {
    uint8_t j;

    if ((pattern == 0) || (pattern->len == 0)) {
        return 0;
    }

    for (j = 0; j < pattern->len; j++) {
        uint8_t is_dash = (pattern->code >> (7 - j)) & 0x01;

        if (!append_mark (is_dash, offset)) {
            return 0;
        }
    }

    *offset -= config.morse_config.break_len;
    return append_signal (0, config.morse_config.letter_break_len, offset);
}

uint8_t Morse_CanEncodeChar (uint8_t theChar) {
    if (theChar == ' ') {
        return 1;
    }

    return select_pattern (theChar) != 0;
}

void TIM2_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

void TIM2_Init (uint16_t arr, uint16_t psc) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    /* Enable timer3 clock */
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2, ENABLE);

    /* Initialize timer3 */
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM2, &TIM_TimeBaseStructure);

    /* Enable updating timer3 interrupt */
    TIM_ITConfig (TIM2, TIM_IT_Update, ENABLE);

    /* Configure timer3 interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
}

void TIM2_ChangBase (uint16_t arr, uint16_t psc) {
    if (stge) {
        NVIC_DisableIRQ (TIM2_IRQn);
        TIM_Cmd (TIM2, DISABLE);
    }
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM2, &TIM_TimeBaseStructure);

    if (stge) {
        TIM_Cmd (TIM2, ENABLE);
        NVIC_EnableIRQ (TIM2_IRQn);
    }
}

void add_wpm (int num) {
    config.wpm += num;
    if (config.wpm > MAX_WPM) {
        config.wpm = MAX_WPM;
    }
    //  printf("wpm %d\r\n", wpm);
    TIM2_ChangBase (3999, (60 * SystemCoreClock / (config.wpm * 50 * 1000) - 1));
    // WriteConfig();
    WriteConfigEEPROM();
}

void sub_wpm (int num) {
    config.wpm -= num;
    if (config.wpm < MIN_WPN) {
        config.wpm = MIN_WPN;
    }
    //  printf("wpm %d\r\n", wpm);
    TIM2_ChangBase (3999, (60 * SystemCoreClock / (config.wpm * 50 * 1000) - 1));
    // WriteConfig();
    WriteConfigEEPROM();
}

void starSending() {
    if (!stge) {
        stge = 1;
        bufCovnMark = 1;
        if (config.mode) {
            send_now = sendCount = 0;
            
        }
        TIM_Cmd (TIM2, ENABLE);
        NVIC_EnableIRQ (TIM2_IRQn);
    }
}

void endSending() {
    if (stge) {
        stge = 0;
        NVIC_DisableIRQ (TIM2_IRQn);
        TIM_Cmd (TIM2, DISABLE);
        GPIO_WriteBit (KEY_OUT_PORT, KEY_OUT, Bit_RESET);
        GPIO_WriteBit (BEEP_OUT_PORT, BEEP_OUT, Bit_RESET);
    }
}

void bufCovn (uint8_t theChar) {
    uint16_t offset = 0;
    const struct MorseCodeMap *pattern;

    if (theChar == ' ') {
        if (config.morse_config.word_break_len >=
            config.morse_config.letter_break_len) {
            append_signal (0,
                           config.morse_config.word_break_len -
                               config.morse_config.letter_break_len,
                           &offset);
        }
        sendBufLen = offset;
        return;
    }

    pattern = select_pattern (theChar);
    if ((pattern == 0) || !encode_pattern (pattern, &offset)) {
        sendBufLen = 0;
        return;
    }

    sendBufLen = offset;
}

void TIM2_IRQHandler (void) {
    static int count = 0;
    if (TIM_GetITStatus (TIM2, TIM_IT_Update) != RESET) {
        /* Clear interrupt flag */
        TIM_ClearITPendingBit (TIM2, TIM_IT_Update);

        if (bufCovnMark) {
            send_now = sendCount;
            if (config.mode) {
                bufCovn (outputBuff[sendCount++]);
            } else {
                bufCovn (inputBuff[sendCount++]);
            }
            bufCovnMark = 0;
        }

        if (count >= sendBufLen) {
            count = 0;
            GPIO_WriteBit (KEY_OUT_PORT, KEY_OUT, Bit_RESET);
            if (config.beeper)
                GPIO_WriteBit (BEEP_OUT_PORT, BEEP_OUT, Bit_RESET);
                
            send_now = sendCount;
            if (config.mode) {
                if (sendCount < outputBuffSize) {
                    bufCovn (outputBuff[sendCount++]);
                } else {
                    sendCount = 0;
                    outputBuff[0] = '\0';
                    outputBuffSize = 0;
                    endSending();
                }
            } else {
                if (sendCount < inputBuffSize) {
                    bufCovn (inputBuff[sendCount++]);
                } else {
                    endSending();
                }
            }
        }

        if (count < sendBufLen && stge == 1) {
            if (sendbuf[count++]) {
                GPIO_WriteBit (KEY_OUT_PORT, KEY_OUT, Bit_SET);
                if (config.beeper)
                    GPIO_WriteBit (BEEP_OUT_PORT, BEEP_OUT, Bit_SET);
            } else {
                GPIO_WriteBit (KEY_OUT_PORT, KEY_OUT, Bit_RESET);
                if (config.beeper)
                    GPIO_WriteBit (BEEP_OUT_PORT, BEEP_OUT, Bit_RESET);
            }
        }
    }
}
