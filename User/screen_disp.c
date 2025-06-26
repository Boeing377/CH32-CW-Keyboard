#include "screen_disp.h"
#include "stdio.h"
#include "string.h"


static const uint8_t image_Volup_bits[] = {0x48,0x8c,0xaf,0xaf,0x8c,0x48};
static const uint8_t image_Muted_bits[] = {0x71,0xf2,0x14,0x18,0x16,0x2f,0x5f,0x8e};
// static const uint8_t image_Battery_bits[] = {0xfe,0xff,0x7f,0x00,0x01,0x00,0x80,0x00,0x01,0x00,0x80,0x03,0x01,0x00,0x80,0x02,0x01,0x00,0x80,0x02,0x01,0x00,0x80,0x03,0x01,0x00,0x80,0x00,0xfe,0xff,0x7f,0x00};
static const uint8_t image_Battery_bits[] ={
 0xfe,0xff,0x7f,0x00,0x01,0x00,0x80,0x00,0x01,0x00,0x80,0x03,
 0x01,0x00,0x80,0x02,0x01,0x00,0x80,0x03,0x01,0x00,0x80,0x00,
 0xfe,0xff,0x7f,0x00,0x00,0x00,0x00,0x00};
static const uint8_t image_Pin_arrow_up_bits[] = {0x08,0x1c,0x3e,0x7f,0x1c,0x1c,0x1c,0x1c,0x1c};

char str_buff[64];

void show_welcome()
{
    u8g2_ClearBuffer(&u8g2); 
    u8g2_SetBitmapMode(&u8g2, 1);
    u8g2_SetFontMode(&u8g2, 1);

    // Layer 1
    u8g2_SetFont(&u8g2, u8g2_font_profont22_tr);
    u8g2_DrawStr(&u8g2, 8, 21, "CW");

    // Layer 2
    u8g2_DrawStr(&u8g2, 16, 39, "KEYBOARD");

    // Layer 3
    u8g2_SetFont(&u8g2, u8g2_font_profont10_tr);
    u8g2_DrawStr(&u8g2, 55, 51, "BY DeVolt Elec");
    
    u8g2_SendBuffer(&u8g2); 
}

void show_ver()
{
    // Layer 1
    u8g2_SetFont(&u8g2, u8g2_font_profont17_tr);
    u8g2_DrawStr(&u8g2, 33, 23, "VERSION");

    // Layer 2
    u8g2_DrawStr(&u8g2, 24, 39, VERSION);

    // Layer 3
    u8g2_SetFont(&u8g2, u8g2_font_profont10_tr);
    u8g2_DrawStr(&u8g2, 29, 52, "Press ENT Back");
}

void show_confirm_reset()
{
    u8g2_SetFont(&u8g2, u8g2_font_profont17_tr);
    u8g2_DrawStr(&u8g2, 33, 23, "WARING");

    u8g2_SetFont(&u8g2, u8g2_font_profont10_tr);
    u8g2_DrawStr(&u8g2, 20, 39, "ALL SET WILL BE RESET");

    u8g2_DrawStr(&u8g2, 24, 52, "Press ENT For Sure");
    u8g2_DrawStr(&u8g2, 24, 62, "Press ESC For Back");
}

void menu_page_1()
{
    static int target_y = 2;
    static int now_y = 2;

    static int y_shift = 0;
    static int now_y_shift = 0;

    static uint8_t menu_item_old = 0;

    if(menu_item_old < menu_item)   //菜单下移
    {
        if(menu_item > 4)
            y_shift = (menu_item - 4) * 12;
    }
    else if ((menu_item_old > 2) && (menu_item == 0) )  //重新进入菜单
    {
        y_shift = now_y_shift = target_y = now_y = 0;
    }
    else if (menu_item_old > menu_item)     //菜单上移
    {
        if(menu_item * 12 < y_shift)
            y_shift = menu_item * 12;
    }
    
    if(now_y_shift != y_shift)
    {
        if (y_shift > now_y_shift)
            now_y_shift +=2;
        else
            now_y_shift -=2;
    }


    u8g2_SetDrawColor(&u8g2, 2);
    u8g2_SetFont(&u8g2, u8g2_font_profont17_tr);

    u8g2_DrawStr(&u8g2, 5, 13 - now_y_shift, "Mode");
    u8g2_DrawStr(&u8g2, 101, 13 - now_y_shift, config.mode == 1 ? "BUF" : "DIR");

    u8g2_DrawStr(&u8g2, 5, 25 - now_y_shift, "beeper");
    u8g2_DrawStr(&u8g2, 101, 25 - now_y_shift, config.beeper == 1 ? "ON" : "OFF");

    u8g2_DrawStr(&u8g2, 5, 37 - now_y_shift, "Morse Cfg");

    u8g2_DrawStr(&u8g2, 5, 49 - now_y_shift, "Btn Func");

    u8g2_DrawStr(&u8g2, 5, 61 - now_y_shift, "Version");

    u8g2_DrawStr(&u8g2, 5, 73 - now_y_shift, "RESET");

    u8g2_DrawStr(&u8g2, 5, 85 - now_y_shift, "SAVE&EXIT");

    // Layer 6
    target_y = 2 + menu_item * 12;
    if(now_y != target_y)
    {
        if (target_y > now_y)
            now_y +=2;
        else
            now_y -=2;
    }
    u8g2_DrawBox(&u8g2, 3, now_y - now_y_shift, 93, 11);
    menu_item_old = menu_item;
}

void menu_train_page()
{
    static int target_y = 2;
    static int now_y = 2;

    static int y_shift = 0;
    static int now_y_shift = 0;

    static uint8_t tain_menu_item_old = 0;

    if(tain_menu_item_old < tain_menu_item)   //菜单下移
    {
        if(tain_menu_item > 4)
            y_shift = (tain_menu_item - 4) * 12;
    }
    else if ((tain_menu_item_old > 2) && (tain_menu_item == 0) )  //重新进入菜单
    {
        y_shift = now_y_shift = target_y = now_y = 0;
    }
    else if (tain_menu_item_old > tain_menu_item)     //菜单上移
    {
        if(tain_menu_item * 12 < y_shift)
            y_shift = tain_menu_item * 12;
    }
    
    if(now_y_shift != y_shift)
    {
        if (y_shift > now_y_shift)
            now_y_shift +=2;
        else
            now_y_shift -=2;
    }


    u8g2_SetDrawColor(&u8g2, 2);
    u8g2_SetFont(&u8g2, u8g2_font_profont17_tr);

    u8g2_DrawStr(&u8g2, 5, 13 - now_y_shift, "METHOD");
    u8g2_DrawStr(&u8g2, 90, 13 - now_y_shift, train_info.methon == 1 ? "NORM" : "KOCH");

    char char_buf[20];
    sprintf(char_buf, "%d", train_info.lesson);

    u8g2_DrawStr(&u8g2, 5, 25 - now_y_shift, "LESSON");
    u8g2_DrawStr(&u8g2, 101, 25 - now_y_shift, char_buf);

    u8g2_DrawStr(&u8g2, 5, 37 - now_y_shift, "SETTING");

    u8g2_DrawStr(&u8g2, 5, 49 - now_y_shift, "START");

    u8g2_DrawStr(&u8g2, 5, 61 - now_y_shift, "EXIT");

    // Layer 6
    target_y = 2 + tain_menu_item * 12;
    if(now_y != target_y)
    {
        if (target_y > now_y)
            now_y +=2;
        else
            now_y -=2;
    }
    u8g2_DrawBox(&u8g2, 3, now_y - now_y_shift, 86, 11);
    tain_menu_item_old = tain_menu_item;
}

void menu_page_morse()
{
    static int target_y = 2;
    static int now_y = 2;
    // Layer 1 (copy)
    u8g2_SetDrawColor(&u8g2, 2);
    u8g2_SetFont(&u8g2, u8g2_font_profont17_tr);
    u8g2_DrawStr(&u8g2, 5, 13, "Word Split");

    // Layer 1
    u8g2_DrawStr(&u8g2, 5, 37, "Cut Num");

    // Layer 1 (copy)
    u8g2_DrawStr(&u8g2, 5, 61, "BACK");

    switch (config.morse_config.cut_num) {
        case 0:
            sprintf (str_buff, "OFF");
            break;
        case 1:
            sprintf (str_buff, "Mod A");
            break;
        case 2:
            sprintf (str_buff, "Mod B");
            break;
        case 3:
            sprintf (str_buff, "Mod C");
            break;
        default:
            sprintf (str_buff, "ERR");
            break;
    }
    // Layer 1 (copy)
    u8g2_DrawStr(&u8g2, 74, 49, str_buff);
    
    switch (config.morse_config.word_break_len) {
        case 7:
            sprintf (str_buff, "Short");
            break;
        case 10:
            sprintf (str_buff, "Mid");
            break;
        case 14:
            sprintf (str_buff, "Long");
            break;
        default:
            sprintf (str_buff, "ERR");
    }
    // Layer 1 (copy) (copy)
    u8g2_DrawStr(&u8g2, 74, 25, str_buff);

    target_y = 2 + morse_conf_item * 24;
    if(now_y != target_y)
    {
        if (target_y > now_y)
            now_y +=4;
        else
            now_y -=4;
    }
    // Layer 6
    u8g2_DrawBox(&u8g2, 3, now_y, 93, 11);
}

void menu_page_button()
{
    static int target_y = 2;
    static int now_y = 2;
    // Layer 1 (copy)
    u8g2_SetDrawColor(&u8g2, 2);
    u8g2_SetFont(&u8g2, u8g2_font_profont17_tr);
    u8g2_DrawStr(&u8g2, 5, 13, "BT1 FUN");

    // Layer 1
    u8g2_DrawStr(&u8g2, 5, 37, "BT2 FUN");

    // Layer 1 (copy)
    u8g2_DrawStr(&u8g2, 5, 61, "BACK");

    switch (config.button_func.bt2_func_index) {
        case 0:
            sprintf (str_buff, "BEEPER SW");
            break;
        case 1:
            sprintf (str_buff, "MODE SW");
            break;
        case 2:
            sprintf (str_buff, "OPEN MENU");
            break;
        default:
            sprintf (str_buff, "ERR");
            break;
    }
    // Layer 1 (copy)
    u8g2_DrawStr(&u8g2, 40, 49, str_buff);
    
    switch (config.button_func.bt1_func_index) {
        case 0:
            sprintf (str_buff, "BEEPER SW");
            break;
        case 1:
            sprintf (str_buff, "MODE SW");
            break;
        case 2:
            sprintf (str_buff, "OPEN MENU");
            break;
        default:
            sprintf (str_buff, "ERR");
            break;
    }
    // Layer 1 (copy) (copy)
    u8g2_DrawStr(&u8g2, 40, 25, str_buff);

    target_y = 2 + button_conf_item * 24;
    if(now_y != target_y)
    {
        if (target_y > now_y)
            now_y +=4;
        else
            now_y -=4;
    }
    // Layer 6
    u8g2_DrawBox(&u8g2, 3, now_y, 93, 11);
}

void show_menu()
{
    if(disp_ver)
        show_ver();
    else if(disp_morse_conf)
        menu_page_morse();
    else if(disp_button_func)
        menu_page_button();
    else if(disp_confirm_reset)
        show_confirm_reset();
    else
        menu_page_1();
}

void show_train_menu()
{
    menu_train_page();
}

void mode_0_word_disp()
{
    int tail_line = (strlen(inputBuff) / 14);
    int tail_num = (strlen(inputBuff) % 14);

    int send_line = ((send_now) / 14);
    int send_num = ((send_now) % 14) + 1;

    int i;

    u8g2_SetDrawColor(&u8g2, 2);
    u8g2_SetFont(&u8g2, u8g2_font_profont17_tr);

    if(tail_line > 3)
    {
        for(i = 3; i >= 0; i --)
        {
            strncpy(str_buff, inputBuff + (tail_line - i) * 14,  14);
            u8g2_DrawStr(&u8g2, 1, 21 + (3 - i) * 13, str_buff);
        }

        u8g2_SetDrawColor(&u8g2, 1);
        if(curse_flash)
            u8g2_DrawLine(&u8g2, 2 + 9 * tail_num, 50, 2 + 9 * tail_num, 61);

        if(stge && ((tail_line - send_line) < 4))
        {
            u8g2_SetDrawColor(&u8g2, 2);
            u8g2_DrawBox(&u8g2, 9*send_num -8, 9+13*(send_line + 3 - tail_line), 9, 13);
        }
    }
    else
    {
        for(i = 0; i < 4; i ++)
        {
            strncpy(str_buff, inputBuff + i * 14,  14);
            u8g2_DrawStr(&u8g2, 1, 21 + i * 13, str_buff);
        }

        u8g2_SetDrawColor(&u8g2, 1);
        if(curse_flash)
            u8g2_DrawLine(&u8g2, 2 + 9 * tail_num, 10 + tail_line * 13 , 2 + 9 * tail_num, 21 + tail_line * 13);

        if(stge)
        {
            u8g2_SetDrawColor(&u8g2, 2);
            u8g2_DrawBox(&u8g2, 9*send_num -8, 9+13*send_line, 9, 13);
        }
    }
}

void mode_1_word_disp()
{
    u8g2_DrawLine(&u8g2, 64, 10, 64, 63);

    int tail_line = (strlen(inputBuff) / 7);
    int tail_num = (strlen(inputBuff) % 7);

    int send_line = ((send_now) / 7);
    int send_num = ((send_now) % 7) + 1;

    int i;

    u8g2_SetFont(&u8g2, u8g2_font_profont17_tr);

    if(tail_line > 3)
    {
        for(i = 3; i >=0; i --)
        {
            strncpy(str_buff, inputBuff + (tail_line - i) * 7,  7);
            u8g2_DrawStr(&u8g2, 1, 21 + (3 - i) * 13, str_buff);
        }
        u8g2_SetDrawColor(&u8g2, 1);
        if(curse_flash)
            u8g2_DrawLine(&u8g2, 2 + 9 * tail_num, 49, 2 + 9 * tail_num, 60);
    }
    else {
        for(i = 0; i < 4; i ++)
        {
            strncpy(str_buff, inputBuff + i * 7,  7);
            u8g2_DrawStr(&u8g2, 1, 21 + i * 13, str_buff);
        }
        u8g2_SetDrawColor(&u8g2, 1);
        if(curse_flash)
            u8g2_DrawLine(&u8g2, 2 + 9 * tail_num, 10 + tail_line * 13 , 2 + 9 * tail_num, 21 + tail_line * 13);
    }

    if(stge){
        if(send_line > 3){
            for(i = 3; i >= 0; i --)
            {
                strncpy(str_buff, outputBuff + (send_line - i) * 7,  7);
                u8g2_DrawStr(&u8g2, 65, 21 + (3 - i) * 13, str_buff);
            }

            if(stge)
            {
                u8g2_SetDrawColor(&u8g2, 2);
                u8g2_DrawBox(&u8g2, 9*send_num +56, 48, 9, 13);
            }
        }
        else {
            for(i = 0; i < 4; i ++)
            {
                strncpy(str_buff, outputBuff + i * 7,  7);
                u8g2_DrawStr(&u8g2, 65, 21 + i * 13, str_buff);
            }

            if(stge)
            {
                u8g2_SetDrawColor(&u8g2, 2);
                u8g2_DrawBox(&u8g2, 9*send_num +56, 9+13*send_line, 9, 13);
            }
        }
    }
}

void show_main_page(void) {
    u8g2_DrawLine(&u8g2, 127, 8, 0, 8);

    if(config.beeper)
        u8g2_DrawXBM(&u8g2, 92, 1, 8, 6, image_Volup_bits);
    else
        u8g2_DrawXBM(&u8g2, 92, 0, 8, 8, image_Muted_bits);

    u8g2_DrawXBM(&u8g2, 102, 0, 26, 8, image_Battery_bits);

    u8g2_SetFont(&u8g2, u8g2_font_profont10_tr);

    sprintf (str_buff, "WPM:%02d", config.wpm);
    u8g2_DrawStr(&u8g2, 2, 7, str_buff);

    sprintf (str_buff, "MsgLen:%d", strlen(inputBuff));
    u8g2_DrawStr(&u8g2, 35, 7, str_buff);

    u8g2_SetFont(&u8g2, u8g2_font_tinyunicode_tr);
    sprintf(str_buff, "%d.%dV", bat_adc_val / 10 , bat_adc_val % 10);
    u8g2_DrawStr(&u8g2, 106, 6, str_buff);

    memset(str_buff, 0, 64);

    // Pin_arrow_up
    if(caps_lock_stg)
        u8g2_DrawXBM(&u8g2, 83, 0, 7, 9, image_Pin_arrow_up_bits);

    switch (config.mode) {
        default:
        case 0:
            mode_0_word_disp();
            break;
        case 1:
            mode_1_word_disp();
            break;
    }
}

void dispf()
{
    u8g2_ClearBuffer(&u8g2); 
    u8g2_SetBitmapMode(&u8g2, 1);
    u8g2_SetFontMode(&u8g2, 1);
    
    if(disp_menu)
        show_menu();
    else if(disp_train_menu)
        show_train_menu();
    else
        show_main_page();

    u8g2_SendBuffer(&u8g2); 
}