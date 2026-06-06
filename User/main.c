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
#include "USB_Host/usb_host_conf.h"
#include "string.h"
#include "ch32_u8g2.h"
#include "screen_disp.h"
#include "ButtonFunc.h"
#include "storage_backend.h"

/* Global typedef */

/* Global define */
/* Global Variable */
uint8_t key1 = 0, key1old = 0, key2 = 0, key2old = 0;

#define BUTTON_LONG_PRESS_MS 800

typedef struct {
    uint8_t was_pressed;
    uint8_t long_press_handled;
    uint32_t pressed_at_ms;
} ButtonState;

void InitGPIOs();
void InitADC();
static void HandleButtonState (ButtonState *state, uint8_t is_pressed,
                               uint8_t button_id);
static void ProcessButtonInputs (void);

u16 Get_ADC_Val(u8 ch);
static uint16_t FilterBatteryAdc (uint16_t sample);
static uint16_t ConvertBatteryToDeciVolt (uint16_t adc_value);

void ReadConfigEEPROM();
void WriteConfigEEPROM();

void ReadSavedMsg (uint8_t sn);
void WriteMsg (uint8_t sn);

void TIM4_Init (uint16_t arr, uint16_t psc);

s16 Calibrattion_Val = 0;
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main (void) {
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();

    /*Init TIMs*/
    TIM3_Init (9, SystemCoreClock / 1000 - 1);
    TIM4_Init (3999, 999);

    /*Init GPIO*/
    InitGPIOs();
    InitADC();

    /*Init OLED*/
    Delay_Ms (100); //wait for OLED hardware OK
    u8g2Init(&u8g2);

    /*Set USB input*/
    GPIO_WriteBit(GPIOB, USB_SW_OUT, Bit_SET);  //Switch USB A

    /*Init USB Host*/
    USBFS_RCC_Init();
    USBFS_Host_Init (ENABLE);
    memset (&RootHubDev.bStatus, 0, sizeof (ROOT_HUB_DEVICE));
    memset (
        &HostCtl[DEF_USBFS_PORT_INDEX * DEF_ONE_USB_SUP_DEV_TOTAL].InterfaceNum,
        0, DEF_ONE_USB_SUP_DEV_TOTAL * sizeof (HOST_CTL));

    // DispWelcome();
    show_welcome();

    // ReadConfig();
    ReadConfigEEPROM();

    TIM2_Init (3999, (60 * SystemCoreClock / (config.wpm * 50 * 1000) - 1));

    Delay_Ms (1500);

    while (1) {
        key1 = GPIO_ReadInputDataBit (KEY_1_IN_PORT, KEY_1_IN);
        key2 = GPIO_ReadInputDataBit (KEY_2_IN_PORT, KEY_2_IN);

        USBH_MainDeal();

        bat_adc_val = FilterBatteryAdc (Get_ADC_Val (ADC_Channel_2));
        bat_adc_val = ConvertBatteryToDeciVolt (bat_adc_val);

        dispf();

        ProcessButtonInputs();

        if (keyboard_in) {
            GPIO_WriteBit (LED_OUT_PORT, LED_OUT, Bit_SET);
        } else {
            GPIO_WriteBit (LED_OUT_PORT, LED_OUT, Bit_RESET);
        }
        if (!config.beeper) {
            GPIO_WriteBit (BEEP_OUT_PORT, BEEP_OUT, Bit_RESET);
        }
        key1old = key1;
        key2old = key2;
    }
}

static void HandleButtonState (ButtonState *state, uint8_t is_pressed,
                               uint8_t button_id) {
    if (is_pressed) {
        if (!state->was_pressed) {
            state->was_pressed = 1;
            state->long_press_handled = 0;
            state->pressed_at_ms = app_tick_ms;
            return;
        }

        if (!state->long_press_handled &&
            (app_tick_ms - state->pressed_at_ms >= BUTTON_LONG_PRESS_MS)) {
            state->long_press_handled = 1;
            if (button_id == BUTTON_ID_1) {
                ButtonHandleKey1LongPress();
            } else if (button_id == BUTTON_ID_2) {
                ButtonHandleKey2LongPress();
            }
        }

        return;
    }

    if (state->was_pressed && !state->long_press_handled) {
        ButtonExecuteConfiguredAction (button_id);
    }

    state->was_pressed = 0;
    state->long_press_handled = 0;
    state->pressed_at_ms = 0;
}

static void ProcessButtonInputs (void) {
    static ButtonState key1_state = {0};
    static ButtonState key2_state = {0};

    HandleButtonState (&key1_state, key1, BUTTON_ID_1);
    HandleButtonState (&key2_state, key2, BUTTON_ID_2);
}

void InitGPIOs() {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = KEY_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = BEEP_OUT | LED_OUT | USB_SW_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = KEY_1_IN | KEY_2_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Pin = POWBOTTON_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    GPIO_WriteBit (POWBOTTON_PORT, POWBOTTON_OUT, Bit_RESET);
    Delay_Ms(50);
    GPIO_WriteBit (POWBOTTON_PORT, POWBOTTON_OUT, Bit_SET);
}

void InitADC() {
    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = BAT_ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(BAT_ADC_PORT, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE); //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    Calibrattion_Val = Get_CalibrationValue(ADC1);
}

u16 Get_ConversionVal(s16 val)
{
    if((val + Calibrattion_Val) < 0)
        return 0;
    if((Calibrattion_Val + val) > 4095 || val==4095)
        return 4095;
    return (val + Calibrattion_Val);
}

u16 Get_ADC_Val(u8 ch)
{
    u16 val;

    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_41Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    val = ADC_GetConversionValue(ADC1);

    return val;
}

static uint16_t FilterBatteryAdc (uint16_t sample)
{
    static uint32_t filtered_q4 = 0;
    uint32_t sample_q4 = ((uint32_t)sample) << 4;

    if (filtered_q4 == 0) {
        filtered_q4 = sample_q4;
    } else {
        int32_t delta = (int32_t)sample_q4 - (int32_t)filtered_q4;

        filtered_q4 = (uint32_t)((int32_t)filtered_q4 + (delta >> 2));
    }

    return (uint16_t)(filtered_q4 >> 4);
}

static uint16_t ConvertBatteryToDeciVolt (uint16_t adc_value)
{
    return (uint16_t)((((uint32_t)adc_value) * 66 + 2048) / 4096);
}

void WriteConfigEEPROM(){
    config.initial_startup = STARUP_FLAG;
    Delay_Ms(10);
    StorageBackend_WriteConfigRegion ((const uint8_t *)&config,
                                      sizeof (struct Config));
    Delay_Ms(10);
    StorageBackend_WriteMsgDirectory (msg, MSG_NUM);
}

void ReadConfigEEPROM(){
    struct Config savedConfig;
    uint8_t needs_config_rewrite = 0;

    StorageBackend_ReadConfigRegion ((uint8_t *)&savedConfig,
                                     sizeof (struct Config));
    if (savedConfig.initial_startup != STARUP_FLAG)  // ȷ���Ƿ�����������˴�Ϊ��������
    {
        config.initial_startup = STARUP_FLAG;
        config.mode = 0;
        config.beeper = 0;  // not using beeper
        config.wpm = DEF_WPM;

        config.morse_config.dot_len = DEF_DOT_LEN;
        config.morse_config.dash_len = DEF_DASH_LEN;
        config.morse_config.break_len = DEF_BREAK_LEN;
        config.morse_config.letter_break_len = DEF_LETTER_BREAK_LEN;
        config.morse_config.word_break_len = DEF_WORD_BREAK_LEN;
        config.morse_config.cut_num = 0;

        config.button_func.bt1_func_index = DEF_BT1_FUN_INDEX;
        config.button_func.bt2_func_index = DEF_BT2_FUN_INDEX;
        ButtonApplyConfiguredActions();
        config.keyboard_layout = KEYBOARD_LAYOUT_QWERTY;
        WriteConfigEEPROM();
    } else {
        config.mode = savedConfig.mode;
        config.beeper = savedConfig.beeper;
        config.wpm = savedConfig.wpm;

        config.morse_config.dot_len = DEF_DOT_LEN;
        config.morse_config.dash_len = DEF_DASH_LEN;
        config.morse_config.break_len = DEF_BREAK_LEN;
        config.morse_config.letter_break_len =
            DEF_LETTER_BREAK_LEN;
        config.morse_config.word_break_len =
            savedConfig.morse_config.word_break_len;
        config.morse_config.cut_num = savedConfig.morse_config.cut_num;

        config.button_func.bt1_func_index = savedConfig.button_func.bt1_func_index;
        config.button_func.bt2_func_index = savedConfig.button_func.bt2_func_index;
        if (savedConfig.button_func.bt1_func_reserved != 0 ||
            savedConfig.button_func.bt2_func_reserved != 0 ||
            savedConfig.button_func.bt1_func_index >= BUTTON_ACTION_COUNT ||
            savedConfig.button_func.bt2_func_index >= BUTTON_ACTION_COUNT) {
            needs_config_rewrite = 1;
        }
        ButtonApplyConfiguredActions();

        if (savedConfig.keyboard_layout < KEYBOARD_LAYOUT_COUNT) {
            config.keyboard_layout = savedConfig.keyboard_layout;
        } else {
            config.keyboard_layout = KEYBOARD_LAYOUT_QWERTY;
            needs_config_rewrite = 1;
        }

        if (needs_config_rewrite) {
            WriteConfigEEPROM();
        }
    }

    // memcpy (msg, (uint8_t *)(CONFIG_ADDR + 16), 12);
    StorageBackend_ReadMsgDirectory (msg, MSG_NUM);
}

void ReadSavedMsgEEPROM(uint8_t sn) {
    uint8_t buff[MSG_ZONE_SIZE];
    uint32_t saved_size;

    if (msg[sn] == 0xcd) {
        StorageBackend_ReadMsgSlot (sn, buff, MSG_ZONE_SIZE);
        saved_size = *(uint32_t *)(buff + MAXSAVEBUFSIZE);
        if (saved_size > MAXSAVEBUFSIZE) {
            saved_size = MAXSAVEBUFSIZE;
        }

        inputBuffSize = saved_size;
        memcpy (inputBuff, buff, inputBuffSize);
        inputBuff[inputBuffSize] = '\0';
        if (!config.mode) {
            sendCount = 0;
            starSending();
        }
    } else {
        static const char no_saved_msg[] = "no saved msg";

        memcpy (inputBuff, no_saved_msg, sizeof (no_saved_msg));
        inputBuffSize = sizeof (no_saved_msg) - 1;
    }
}

void WriteMsgEEPROM (uint8_t sn)
{
    uint8_t buff[MSG_ZONE_SIZE] = {0};
    uint32_t saveingBuffSize = inputBuffSize;

    saving = 1;

    if (saveingBuffSize > MAXSAVEBUFSIZE)
        saveingBuffSize = MAXSAVEBUFSIZE;
    memcpy (buff, inputBuff, saveingBuffSize);
    *(uint32_t *)(buff + MAXSAVEBUFSIZE) = saveingBuffSize;

    StorageBackend_WriteMsgSlot (sn, buff, MSG_ZONE_SIZE);

    msg[sn] = 0xcd;

    WriteConfigEEPROM();

    saving = 0;
}

void ResetConfig()
{
        config.initial_startup = STARUP_FLAG;
        config.mode = 0;
        config.beeper = 0;  // not using beeper
        config.wpm = DEF_WPM;

        config.morse_config.dot_len = DEF_DOT_LEN;
        config.morse_config.dash_len = DEF_DASH_LEN;
        config.morse_config.break_len = DEF_BREAK_LEN;
        config.morse_config.letter_break_len = DEF_LETTER_BREAK_LEN;
        config.morse_config.word_break_len = DEF_WORD_BREAK_LEN;
        config.morse_config.cut_num = 0;

        config.button_func.bt1_func_index = DEF_BT1_FUN_INDEX;
        config.button_func.bt2_func_index = DEF_BT2_FUN_INDEX;
        ButtonApplyConfiguredActions();
        config.keyboard_layout = KEYBOARD_LAYOUT_QWERTY;

        for(int i = 0; i < 12; i++)
        {
            msg[i]= 0x00;
        }
        WriteConfigEEPROM();
}


void TIM4_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

void TIM4_Init (uint16_t arr, uint16_t psc) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    /* Enable timer4 clock */
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM4, ENABLE);

    /* Initialize timer4 */
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM4, &TIM_TimeBaseStructure);

    /* Enable updating timer3 interrupt */
    TIM_ITConfig (TIM4, TIM_IT_Update, ENABLE);

    /* Configure timer3 interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    TIM_Cmd (TIM4, ENABLE);
    NVIC_EnableIRQ (TIM4_IRQn);
}

void TIM4_IRQHandler (void) {
    if (TIM_GetITStatus (TIM4, TIM_IT_Update) != RESET) {
        /* Clear interrupt flag */
        TIM_ClearITPendingBit (TIM4, TIM_IT_Update);
        static int powbutton_timer;
        static int curse_flash_timer;
        
        if (powbutton_timer > 0)
            powbutton_timer--;
        else {
            if (GPIO_ReadOutputDataBit (POWBOTTON_PORT, POWBOTTON_OUT)) {
                GPIO_WriteBit (POWBOTTON_PORT, POWBOTTON_OUT, Bit_RESET);
                powbutton_timer = 15;
            } else {
                GPIO_WriteBit (POWBOTTON_PORT, POWBOTTON_OUT, Bit_SET);
                powbutton_timer = 235;
            }
        }
        if (curse_flash_timer > 0)
            curse_flash_timer--;
        else{
            curse_flash = ! curse_flash;
            curse_flash_timer = 10;
        }
    }
}
