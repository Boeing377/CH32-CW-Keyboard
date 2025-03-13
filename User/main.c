/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *USART Print debugging routine:
 *USART1_Tx(PA9).
 *This example demonstrates using USART1(PA9) as a print debug port output.
 *
 */

#include "debug.h"
#include "global.h"
#include "morse_send.h"
#include "OLED/oled.h"
#include "USB_Host/usb_host_conf.h"
#include "string.h"

/* Global typedef */

/* Global define */

/* Global Variable */
uint8_t key1 = 0, key1old = 0, key2 = 0, key2old = 0;

void InitGPIOs();

void ReadConfig();
void WriteConfig();

void ReadSavedMsg(uint8_t sn);
void WriteMsg(uint8_t sn);

void DispWelcome();
void Disp();

void TIM4_Init(uint16_t arr, uint16_t psc);

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
#if DEBUG
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("BG6VSK made USB keyboard electric key\r\n");
#endif
    /*Init TIM2&3*/
    TIM2_Init(3999, (60 * SystemCoreClock / (1000 * 1000) - 1));
    TIM3_Init(9, SystemCoreClock / 1000 - 1);
#if DEBUG
    printf("TIM OK\r\n");
#endif
    /*Init GPIO*/
    InitGPIOs();

    TIM4_Init(3999, 999);

    /*Init OLED*/
    SSD1306_Init();

    /*Init USB Host*/
    USBFS_RCC_Init();
    USBFS_Host_Init(ENABLE);
    memset(&RootHubDev.bStatus, 0, sizeof(ROOT_HUB_DEVICE));
    memset(
            &HostCtl[DEF_USBFS_PORT_INDEX * DEF_ONE_USB_SUP_DEV_TOTAL].InterfaceNum,
            0, DEF_ONE_USB_SUP_DEV_TOTAL * sizeof(HOST_CTL));

    ReadConfig();

    DispWelcome();

    Delay_Ms(1500);

    while(1)
    {
        key1 = GPIO_ReadInputDataBit(KEY_1_IN_PORT, KEY_1_IN);
        key2 = GPIO_ReadInputDataBit(KEY_2_IN_PORT, KEY_2_IN);
        USBH_MainDeal( );
        Disp();
        if(key1 == 1 && key1old == 0)   //key1 pressed
        {
            config.beeper = 1 - config.beeper;        //切换是否使用蜂鸣器
            WriteConfig();
        }
        if(key2 == 1 && key2old == 0)   //key2 pressed
        {
            config.mode = 1 - config.mode;            //切换模式
            endSending();
            inputBuffSize = 0;
//            inputBuff[0] = '\0';
            memset(inputBuff, '\0', BUFFSIZE);
            outputBuffSize = 0;
//            outputBuff[0] = '\0';
            memset(outputBuff, '\0', BUFFSIZE);
            sendCount = 0;
            WriteConfig();
        }
        if(keyboard_in)
        {
            GPIO_WriteBit(LED_OUT_PORT, LED_OUT, Bit_SET);
        }
        else {
            GPIO_WriteBit(LED_OUT_PORT, LED_OUT, Bit_RESET);
        }
        if(!config.beeper)
        {
            GPIO_WriteBit(BEEP_OUT_PORT, BEEP_OUT, Bit_RESET);
        }
        key1old = key1;
        key2old = key2;
    }
}

void InitGPIOs() {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = KEY_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = BEEP_OUT | LED_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = KEY_1_IN | KEY_2_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Pin = POWBOTTON_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ReadConfig() {
    struct Config *savedConfig;
    savedConfig = (struct Config *) (CONFIG_ADDR);
    if (savedConfig->initial_startup != STARUP_FLAG)    //确认是否初次启动，此处为初次启动
    {
        config.initial_startup = STARUP_FLAG;
        config.mode = 0;
        config.beeper = 0;  //not using beeper
        config.wpm = DEF_WPM;
        config.morse_config.dot_len = DEF_DOT_LEN;
        config.morse_config.dash_len = DEF_DASH_LEN;
        config.morse_config.break_len = DEF_BREAK_LEN;
        config.morse_config.letter_break_len = DEF_LETTER_BREAK_LEN;
        config.morse_config.word_break_len = DEF_WORD_BREAK_LEN;
        config.morse_config.cut_num = 0;
        WriteConfig();
    } else {
        config.mode = savedConfig->mode;
        config.beeper = savedConfig->beeper;
        config.wpm = savedConfig->wpm;
        config.morse_config.dot_len = DEF_DOT_LEN;
        config.morse_config.dash_len = DEF_DASH_LEN;
        config.morse_config.break_len = DEF_BREAK_LEN;
        config.morse_config.letter_break_len =
        DEF_LETTER_BREAK_LEN;
        config.morse_config.word_break_len =
                savedConfig->morse_config.word_break_len;
        config.morse_config.cut_num = savedConfig->morse_config.cut_num;
    }

    memcpy(msg, (uint8_t *) (CONFIG_ADDR + 16), 12);
}

void WriteConfig() {
    uint8_t tmp[16] = { 0 };
    int i;
    FLASH_Unlock();

    FLASH_ClearFlag( FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR);
//    FLASH_ErasePage(CONFIG_ADDR);
//    FLASH_ROM_ERASE(CONFIG_ADDR, 256);
    FLASH_ErasePage_Fast(CONFIG_ADDR);

    config.initial_startup = STARUP_FLAG;

    memcpy(tmp, &config, sizeof(struct Config));

    for (i = 0; i < 4; i++) {
        FLASH_ProgramWord((CONFIG_ADDR + 4 * i), *((uint32_t *) (tmp + 4 * i)));
    }

    FLASH_ProgramWord(CONFIG_ADDR + 16, *((uint32_t *) msg));
    FLASH_ProgramWord(CONFIG_ADDR + 20, *((uint32_t *) (msg + 4)));
    FLASH_ProgramWord(CONFIG_ADDR + 24, *((uint32_t *) (msg + 8)));

    FLASH_Lock();
}

void ReadSavedMsg(uint8_t sn) {
    uint32_t addr;

    if (sn < 4) {
        addr = MSG_ADDR + sn * MSG_ZONE_SIZE;
    } else if (sn < 8) {
        addr = MSG_ADDR + 0x1000 + (sn - 4) * MSG_ZONE_SIZE;
    } else {
        addr = MSG_ADDR + 0x2000 + (sn - 8) * MSG_ZONE_SIZE;
    }

    if (msg[sn] == 0xcd) {
        inputBuffSize = *(uint32_t *) (addr + BUFFSIZE - 4);
        memcpy(inputBuff, (uint32_t*) addr, inputBuffSize);
        inputBuff[inputBuffSize] = '\0';
        if (!config.mode) {
            sendCount = 0;
            starSending();
        }
    } else {
        sprintf(inputBuff, "no saved msg");
        inputBuffSize = strlen(inputBuff);
    }
}

void WriteMsg(uint8_t sn) {
    uint32_t eraseAddr;
    uint8_t buff[0x0800];
    uint32_t saveingBuffSize;

    saving = 1;
//    TIM_Cmd( TIM4, ENABLE);
    /* Enable timer3 interrupt */
//    NVIC_EnableIRQ(TIM4_IRQn);

    if (sn < 4)
        eraseAddr = MSG_ADDR;
    else if (sn < 8)
        eraseAddr = MSG_ADDR + 0x1000;
    else
        eraseAddr = MSG_ADDR + 0x2000;

    memcpy(buff, (uint8_t *) eraseAddr, 0x0800);

    FLASH_Unlock();

    FLASH_ClearFlag( FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage(eraseAddr);

    saveingBuffSize = inputBuffSize;
    if(saveingBuffSize > MAXSAVEBUFSIZE)
        saveingBuffSize = MAXSAVEBUFSIZE;

    if (sn < 4) {
        memcpy(buff + MSG_ZONE_SIZE * sn, inputBuff, saveingBuffSize);
        *(uint32_t*) (buff + MSG_ZONE_SIZE * sn + BUFFSIZE - 4) = saveingBuffSize;
    } else if (sn < 8) {
        memcpy(buff + MSG_ZONE_SIZE * (sn - 4), inputBuff, saveingBuffSize);
        *(uint32_t*) (buff + MSG_ZONE_SIZE * (sn - 4) + BUFFSIZE - 4) =
                saveingBuffSize;
    } else {
        memcpy(buff + MSG_ZONE_SIZE * (sn - 8), inputBuff, saveingBuffSize);
        *(uint32_t*) (buff + MSG_ZONE_SIZE * (sn - 8) + BUFFSIZE - 4) =
                saveingBuffSize;
    }

    FLASH_ROM_WRITE(eraseAddr, (uint32_t*) buff, 0x0800);

    FLASH_Lock();

    msg[sn] = 0xcd;
    WriteConfig();

//    TIM_Cmd( TIM4, DISABLE);
//    NVIC_DisableIRQ(TIM4_IRQn);

    saving = 0;
}

void DispWelcome() {
    SSD1306_GotoXY(10, 10);
    SSD1306_Puts("CW", &Font_11x18, 1);
    SSD1306_GotoXY(10, 30);
    SSD1306_Puts("Keyboard", &Font_11x18, 1);
    SSD1306_GotoXY(20, 50);
    SSD1306_Puts("By DeVolt Elec.", &Font_7x10, 1);
//    SSD1306_GotoXY(60, 50);
//    SSD1306_Puts(VERSION, &Font_7x10, 1); //print Soft Version
    SSD1306_UpdateScreen(); // update screen
}

void Disp_Ver() {
    char str[19];
    SSD1306_GotoXY(30, 20);
    sprintf(str, "%s", VERSION);
    SSD1306_Puts(str, &Font_7x10, 1);
}

void Disp_Morse_Conf() {
    char str[19];

    SSD1306_GotoXY(0, 0);
    sprintf(str, "Word Split");
    SSD1306_Puts(str, &Font_7x10, morse_conf_item == 0 ? 0 : 1);
    SSD1306_GotoXY(86, 0);
    switch (config.morse_config.word_break_len) {
    case 7:
        sprintf(str, "Short");
        break;
    case 10:
        sprintf(str, "Mid");
        break;
    case 14:
        sprintf(str, "Long");
        break;
    default:
        sprintf(str, "ERR");
    }
    SSD1306_Puts(str, &Font_7x10, 1);

    SSD1306_GotoXY(0, 10);
    sprintf(str, "Cut Num");
    SSD1306_Puts(str, &Font_7x10, morse_conf_item == 1 ? 0 : 1);
    SSD1306_GotoXY(86, 10);
    switch (config.morse_config.cut_num) {
    case 0:
        sprintf(str, "OFF");
        break;
    case 1:
        sprintf(str, "Mod A");
        break;
    case 2:
        sprintf(str, "Mod B");
        break;
    case 3:
        sprintf(str, "Mod C");
        break;
    default:
        sprintf(str, "ERR");
        break;
    }
    SSD1306_Puts(str, &Font_7x10, 1);
    SSD1306_GotoXY(0, 0);

//    SSD1306_GotoXY(0, 20);
//    sprintf(str, "BREAK LEN");
//    SSD1306_Puts(str, &Font_7x10, morse_conf_item == 2 ? 0 : 1);
//    SSD1306_GotoXY(100, 20);
//    sprintf(str, "%d", config.morse_config.break_len);
//    SSD1306_Puts(str, &Font_7x10, 1);
//
//    SSD1306_GotoXY(0, 30);
//    sprintf(str, "LETTER BREAK");
//    SSD1306_Puts(str, &Font_7x10, morse_conf_item == 3 ? 0 : 1);
//    SSD1306_GotoXY(100, 30);
//    sprintf(str, "%d", config.morse_config.letter_break_len);
//    SSD1306_Puts(str, &Font_7x10, 1);
//
//    SSD1306_GotoXY(0, 40);
//    sprintf(str, "WORD BREAK");
//    SSD1306_Puts(str, &Font_7x10, morse_conf_item == 4 ? 0 : 1);
//    SSD1306_GotoXY(100, 40);
//    sprintf(str, "%d", config.morse_config.word_break_len);
//    SSD1306_Puts(str, &Font_7x10, 1);

    SSD1306_GotoXY(0, 50);
    sprintf(str, "BACK");
    SSD1306_Puts(str, &Font_7x10, morse_conf_item == 2 ? 0 : 1);

}

void Disp_Menu() {
    char str[19];
    if (disp_ver) {
        Disp_Ver();
    } else if (disp_morse_conf) {
        Disp_Morse_Conf();
    } else {
        SSD1306_GotoXY(0, 0);
        sprintf(str, "MODE");
        SSD1306_Puts(str, &Font_7x10, menu_item == 0 ? 0 : 1);
        SSD1306_GotoXY(100, 0);
        sprintf(str, "%s", config.mode == 1 ? "BUF" : "DIR");
        SSD1306_Puts(str, &Font_7x10, 1);

        SSD1306_GotoXY(0, 10);
        sprintf(str, "BEEPER");
        SSD1306_Puts(str, &Font_7x10, menu_item == 1 ? 0 : 1);
        SSD1306_GotoXY(100, 10);
        sprintf(str, "%s", config.beeper == 1 ? "ON" : "OFF");
        SSD1306_Puts(str, &Font_7x10, 1);

        SSD1306_GotoXY(0, 20);
        sprintf(str, "MORSE CONF");
        SSD1306_Puts(str, &Font_7x10, menu_item == 2 ? 0 : 1);

        SSD1306_GotoXY(0, 30);
        sprintf(str, "VERSION");
        SSD1306_Puts(str, &Font_7x10, menu_item == 3 ? 0 : 1);

        SSD1306_GotoXY(0, 50);
        sprintf(str, "SAVE & EXIT");
        SSD1306_Puts(str, &Font_7x10, menu_item == 4 ? 0 : 1);
    }
}

void Disp() {
    char str[19];
    int diff;

    SSD1306_Fill(SSD1306_COLOR_BLACK);

    if (disp_menu) {
        Disp_Menu();
    } else {
        SSD1306_GotoXY(0, 0);
        sprintf(str, "WPM:%d", config.wpm);
        SSD1306_Puts(str, &Font_7x10, 1);

        if (config.beeper) {
            SSD1306_GotoXY(113, 0);
            SSD1306_Puts("B", &Font_7x10, 1);
        }

        SSD1306_GotoXY(42, 0);
        sprintf(str, "S:%03d", sendCount);
        SSD1306_Puts(str, &Font_7x10, 1);

        SSD1306_GotoXY(77, 0);
        if (stge == 1 && config.mode == 1)
            sprintf(str, "L:%03d", outputBuffSize);
        else
            sprintf(str, "L:%03d", inputBuffSize);
        SSD1306_Puts(str, &Font_7x10, 1);

        if (caps_lock_stg) {
            SSD1306_GotoXY(120, 0);
            SSD1306_Puts("C", &Font_7x10, 1);
        }

        if (config.mode) {
            SSD1306_DrawLine(64, 10, 64, 64, 1);
            SSD1306_DrawLine(63, 10, 63, 64, 1);
            SSD1306_DrawLine(0, 10, 128, 10, 1);

            SSD1306_GotoXY(0, 11);
            SSD1306_Puts("INPUT:", &Font_7x10, 0);

            if (strlen(inputBuff) <= 9) {
                SSD1306_GotoXY(0, 22);
                SSD1306_Puts(inputBuff, &Font_7x10, 1);
            } else if (strlen(inputBuff) < 9 * 4) {
                for (int i = 0; i < (strlen(inputBuff) / 9 + 1); i++) {
                    if (strlen(inputBuff + i * 9) <= 9) {
                        strcpy(str, inputBuff + i * 9);

                    } else {
                        memcpy(str, inputBuff + i * 9, 9);
                        str[9] = '\0';
                    }
                    SSD1306_GotoXY(0, 22 + i * 10);
                    SSD1306_Puts(str, &Font_7x10, 1);
                }
            } else {
                diff = strlen(inputBuff) / 9 - 3;
                for (int i = diff; i < (strlen(inputBuff) / 9 + 1); i++) {
                    if (strlen(inputBuff + i * 9) <= 9) {
                        strcpy(str, inputBuff + i * 9);

                    } else {
                        memcpy(str, inputBuff + i * 9, 9);
                        str[9] = '\0';
                    }
                    SSD1306_GotoXY(0, 22 + (i - diff) * 10);
                    SSD1306_Puts(str, &Font_7x10, 1);
                }
            }

            SSD1306_GotoXY(64, 11);
            SSD1306_Puts("SENDING:", &Font_7x10, 0);

            if (strlen(outputBuff) <= 9) {
                SSD1306_GotoXY(64, 22);
                SSD1306_Puts(outputBuff, &Font_7x10, 1);
            } else {
                for (int i = 0; i < (strlen(outputBuff) / 9 + 1); i++) {
                    if (strlen(outputBuff + i * 9) <= 9) {
                        strcpy(str, outputBuff + i * 9);

                    } else {
                        memcpy(str, outputBuff + i * 9, 9);
                        str[9] = '\0';
                    }
                    SSD1306_GotoXY(64, 22 + i * 10);
                    SSD1306_Puts(str, &Font_7x10, 1);
                }
            }
        } else {
            SSD1306_DrawLine(0, 10, 128, 10, 1);
            SSD1306_GotoXY(0, 11);
            SSD1306_Puts("INPUT:", &Font_7x10, 0);

            if (strlen(inputBuff) <= 18) {
                SSD1306_GotoXY(0, 22);
                SSD1306_Puts(inputBuff, &Font_7x10, 1);
            } else if (strlen(inputBuff) <= 18 * 4) {
                for (int i = 0; i < (strlen(inputBuff) / 18 + 1); i++) {
                    if (strlen(inputBuff + i * 18) <= 18) {
                        strcpy(str, inputBuff + i * 18);

                    } else {
                        memcpy(str, inputBuff + i * 18, 18);
                        str[18] = '\0';
                    }
                    SSD1306_GotoXY(0, 22 + i * 10);
                    SSD1306_Puts(str, &Font_7x10, 1);
                }
            } else {
                diff = strlen(inputBuff) / 18 - 3;
                for (int i = diff; i < (strlen(inputBuff) / 18 + 1); i++) {
                    if (strlen(inputBuff + i * 18) <= 18) {
                        strcpy(str, inputBuff + i * 18);

                    } else {
                        memcpy(str, inputBuff + i * 18, 18);
                        str[18] = '\0';
                    }
                    SSD1306_GotoXY(0, 22 + (i - diff) * 10);
                    SSD1306_Puts(str, &Font_7x10, 1);
                }
            }
        }

    }
    SSD1306_UpdateScreen();
}

void TIM4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM4_Init(uint16_t arr, uint16_t psc) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };
    NVIC_InitTypeDef NVIC_InitStructure = { 0 };

    /* Enable timer4 clock */
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE);

    /* Initialize timer4 */
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM4, &TIM_TimeBaseStructure);

    /* Enable updating timer3 interrupt */
    TIM_ITConfig( TIM4, TIM_IT_Update, ENABLE);

    /* Configure timer3 interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM4, ENABLE);
    NVIC_EnableIRQ(TIM4_IRQn);
}

void TIM4_IRQHandler(void) {
    if (TIM_GetITStatus( TIM4, TIM_IT_Update) != RESET) {
        /* Clear interrupt flag */
        TIM_ClearITPendingBit( TIM4, TIM_IT_Update);
        static int powbutton_timer;
        if(powbutton_timer > 0)
            powbutton_timer --;
        else{
            if (GPIO_ReadOutputDataBit(POWBOTTON_PORT, POWBOTTON_OUT)) {
                GPIO_WriteBit(POWBOTTON_PORT, POWBOTTON_OUT, Bit_RESET);
                powbutton_timer = 490;
            } else {
                GPIO_WriteBit(POWBOTTON_PORT, POWBOTTON_OUT, Bit_SET);
                powbutton_timer = 10;
            }
        }
    }
}
