#include "screen_disp.h"
#include "string.h"

#if COMPARE_FOR_VERSION_WITH_EEPROM
#include "train.h"
#define UI_SHOW_BATTERY_ICON 1
#else
#define UI_SHOW_BATTERY_ICON 0
#endif

#define STATUS_SPEAKER_ICON_X 119
#define STATUS_BATTERY_ICON_X 91
#define STATUS_BATTERY_TEXT_X 95


static const uint8_t image_Volup_bits[] = {0x48,0x8c,0xaf,0xaf,0x8c,0x48};
static const uint8_t image_Muted_bits[] = {0x71,0xf2,0x14,0x18,0x16,0x2f,0x5f,0x8e};
// static const uint8_t image_Battery_bits[] = {0xfe,0xff,0x7f,0x00,0x01,0x00,0x80,0x00,0x01,0x00,0x80,0x03,0x01,0x00,0x80,0x02,0x01,0x00,0x80,0x02,0x01,0x00,0x80,0x03,0x01,0x00,0x80,0x00,0xfe,0xff,0x7f,0x00};
#if UI_SHOW_BATTERY_ICON
static const uint8_t image_Battery_bits[] ={
 0xfe,0xff,0x7f,0x00,0x01,0x00,0x80,0x00,0x01,0x00,0x80,0x03,
 0x01,0x00,0x80,0x02,0x01,0x00,0x80,0x03,0x01,0x00,0x80,0x00,
 0xfe,0xff,0x7f,0x00,0x00,0x00,0x00,0x00};
#endif
static const uint8_t image_Pin_arrow_up_bits[] = {0x08,0x1c,0x3e,0x7f,0x1c,0x1c,0x1c,0x1c,0x1c};

char str_buff[64];

static void CopyText (char *dst, const char *src)
{
    strcpy (dst, src);
}

static char *AppendUnsigned (char *dst, uint32_t value)
{
    char digits[10];
    uint8_t count = 0;

    do {
        digits[count++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);

    while (count > 0) {
        *dst++ = digits[--count];
    }

    *dst = '\0';
    return dst;
}

static void FormatUnsigned (char *dst, uint32_t value)
{
    AppendUnsigned (dst, value);
}

static void FormatWpmText (char *dst, uint8_t wpm)
{
    *dst++ = 'W';
    *dst++ = 'P';
    *dst++ = 'M';
    *dst++ = ':';
    *dst++ = '0' + (wpm / 10);
    *dst++ = '0' + (wpm % 10);
    *dst = '\0';
}

static void FormatMsgLenText (char *dst, uint32_t msg_len)
{
    memcpy (dst, "MsgLen:", 7);
    AppendUnsigned (dst + 7, msg_len);
}

#if COMPARE_FOR_VERSION_WITH_EEPROM
static void FormatBatteryText (char *dst, uint16_t decivolt)
{
    dst = AppendUnsigned (dst, decivolt / 10);
    *dst++ = '.';
    *dst++ = '0' + (decivolt % 10);
    *dst++ = 'V';
    *dst = '\0';
}
#endif

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
#if FW_CUSTOM_VERSION
    // Layer 1 - 标题用小号字体
    u8g2_SetFont(&u8g2, u8g2_font_profont12_tr);
    u8g2_DrawStr(&u8g2, 43, 11, "VERSION");

    // Layer 2 - 版本号用大字体
    u8g2_SetFont(&u8g2, u8g2_font_profont17_tr);
    u8g2_DrawStr(&u8g2, 24, 28, VERSION);

    // Layer 3 - 定制信息分两行，交错排列
    u8g2_SetFont(&u8g2, u8g2_font_profont10_tr);
    u8g2_DrawStr(&u8g2, 3, 41, "BG6VSK Provided to");

    // Layer 4 - 接收者独立一行，右偏移形成错落感
    u8g2_DrawStr(&u8g2, 48, 52, FW_CUSTOM_RECIPIENT);

    // Layer 5
    u8g2_DrawStr(&u8g2, 29, 63, "Press ENT Back");
#else
    // Layer 1
    u8g2_SetFont(&u8g2, u8g2_font_profont17_tr);
    u8g2_DrawStr(&u8g2, 33, 23, "VERSION");

    // Layer 2
    u8g2_DrawStr(&u8g2, 24, 39, VERSION);

    // Layer 3
    u8g2_SetFont(&u8g2, u8g2_font_profont10_tr);
    u8g2_DrawStr(&u8g2, 29, 52, "Press ENT Back");
#endif
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

    u8g2_DrawStr(&u8g2, 5, 61 - now_y_shift, "Repeat");

    u8g2_DrawStr(&u8g2, 5, 73 - now_y_shift, "Keyboard");
    u8g2_DrawStr(&u8g2, 83, 73 - now_y_shift,
                 config.keyboard_layout == KEYBOARD_LAYOUT_AZERTY ? "AZERTY"
                                                                  : "QWERTY");

    u8g2_DrawStr(&u8g2, 5, 85 - now_y_shift, "Version");

    u8g2_DrawStr(&u8g2, 5, 97 - now_y_shift, "RESET");

    u8g2_DrawStr(&u8g2, 5, 109 - now_y_shift, "SAVE&EXIT");

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

#if COMPARE_FOR_VERSION_WITH_EEPROM
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

    {
        const char *method_str;
        if (train_info.methon == TRAIN_METHOD_KOCH)
            method_str = "KOCH";
        else if (train_info.methon == TRAIN_METHOD_SEQU)
            method_str = "SEQU";
        else
            method_str = "FREE";
        u8g2_DrawStr(&u8g2, 5, 13 - now_y_shift, "METHOD");
        u8g2_DrawStr(&u8g2, 90, 13 - now_y_shift, method_str);
    }

    {
        char char_buf[20];
        u8g2_DrawStr(&u8g2, 5, 25 - now_y_shift, "LESSON");
        if (train_info.methon == TRAIN_METHOD_FREE) {
            u8g2_DrawStr(&u8g2, 101, 25 - now_y_shift, "---");
        } else {
            FormatUnsigned (char_buf, train_info.lesson);
            u8g2_DrawStr(&u8g2, 101, 25 - now_y_shift, char_buf);
        }
    }

    u8g2_DrawStr(&u8g2, 5, 37 - now_y_shift, "SETTING");

    u8g2_DrawStr(&u8g2, 5, 49 - now_y_shift, "START");

    u8g2_DrawStr(&u8g2, 5, 61 - now_y_shift, "EXIT");

    // Layer 6: snap if gap > 1 row (page re-entry), else animate
    target_y = 2 + tain_menu_item * 12;
    if(now_y != target_y)
    {
        if (target_y > now_y + 12 || now_y > target_y + 12)
            now_y = target_y;
        else if (target_y > now_y)
            now_y +=2;
        else
            now_y -=2;
    }
    u8g2_DrawBox(&u8g2, 3, now_y - now_y_shift, 86, 11);
    tain_menu_item_old = tain_menu_item;
}
#endif /* COMPARE_FOR_VERSION_WITH_EEPROM */

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
            CopyText (str_buff, "OFF");
            break;
        case 1:
            CopyText (str_buff, "Mod A");
            break;
        case 2:
            CopyText (str_buff, "Mod B");
            break;
        case 3:
            CopyText (str_buff, "Mod C");
            break;
        default:
            CopyText (str_buff, "ERR");
            break;
    }
    // Layer 1 (copy)
    u8g2_DrawStr(&u8g2, 74, 49, str_buff);
    
    switch (config.morse_config.word_break_len) {
        case 7:
            CopyText (str_buff, "Short");
            break;
        case 10:
            CopyText (str_buff, "Mid");
            break;
        case 14:
            CopyText (str_buff, "Long");
            break;
        default:
            CopyText (str_buff, "ERR");
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
            CopyText (str_buff, "BEEPER SW");
            break;
        case 1:
            CopyText (str_buff, "MODE SW");
            break;
        case 2:
            CopyText (str_buff, "OPEN MENU");
            break;
        default:
            CopyText (str_buff, "ERR");
            break;
    }
    // Layer 1 (copy)
    u8g2_DrawStr(&u8g2, 40, 49, str_buff);
    
    switch (config.button_func.bt1_func_index) {
        case 0:
            CopyText (str_buff, "BEEPER SW");
            break;
        case 1:
            CopyText (str_buff, "MODE SW");
            break;
        case 2:
            CopyText (str_buff, "OPEN MENU");
            break;
        default:
            CopyText (str_buff, "ERR");
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

void menu_page_repeat()
{
    static int target_y = 2;
    static int now_y = 2;

    u8g2_SetDrawColor(&u8g2, 2);
    u8g2_SetFont(&u8g2, u8g2_font_profont17_tr);

    /* Row 0: RPT COUNT */
    u8g2_DrawStr(&u8g2, 5, 12, "RPT COUNT");
    {
        char buf[5] = "    ";
        uint16_t val;
        if (disp_repeat_input && repeat_input_target == 0) {
            val = repeat_input_value > 999 ? 999 : repeat_input_value;
        } else {
            val = config.repeat_config.repeat_count;
        }
        if (!disp_repeat_input && val == 0) {
            /* Infinite mode */
            u8g2_DrawStr(&u8g2, 13, 23, " INF");
        } else {
            buf[0] = ' ';
            buf[1] = '0' + ((val / 100) % 10);
            buf[2] = '0' + ((val / 10) % 10);
            buf[3] = '0' + (val % 10);
            buf[4] = '\0';
            u8g2_DrawStr(&u8g2, 13, 23, buf);
        }
        if (disp_repeat_input && repeat_input_target == 0 && curse_flash) {
            u8g2_DrawLine(&u8g2, 13 + repeat_input_pos * 9, 24,
                          13 + repeat_input_pos * 9 + 7, 24);
        }
    }

    /* Row 1: RPT INTER */
    u8g2_DrawStr(&u8g2, 5, 36, "RPT INTER");
    {
        char buf[8];
        uint16_t val;
        if (disp_repeat_input && repeat_input_target == 1) {
            val = repeat_input_value;
        } else {
            val = config.repeat_config.repeat_interval_s;
        }
        buf[0] = ' ';
        FormatUnsigned (buf + 1, val);
        strcpy (buf + strlen (buf), "s");
        u8g2_DrawStr(&u8g2, 13, 47, buf);
        if (disp_repeat_input && repeat_input_target == 1 && curse_flash) {
            u8g2_DrawLine(&u8g2, 13 + repeat_input_pos * 9, 48,
                          13 + repeat_input_pos * 9 + 7, 48);
        }
    }

    /* Row 2: BACK */
    u8g2_DrawStr(&u8g2, 5, 60, "BACK");

    target_y = 2 + repeat_conf_item * 24;
    if (now_y != target_y) {
        if (target_y > now_y)
            now_y += 4;
        else
            now_y -= 4;
    }
    u8g2_DrawBox(&u8g2, 3, now_y, 115, repeat_conf_item < 2 ? 22 : 11);
}

void show_menu()
{
    if(disp_ver)
        show_ver();
    else if(disp_morse_conf)
        menu_page_morse();
    else if(disp_button_func)
        menu_page_button();
    else if(disp_repeat_conf)
        menu_page_repeat();
    else if(disp_confirm_reset)
        show_confirm_reset();
    else
        menu_page_1();
}

#if COMPARE_FOR_VERSION_WITH_EEPROM
void show_train_menu()
{
    menu_train_page();
}
#endif

#if COMPARE_FOR_VERSION_WITH_EEPROM
/* ── Training active page ────────────────────────────────── */
void show_training_page (void)
{
    int line;
    const int chars_per_line = 14;

    if (train_phase == TRAIN_PHASE_SCORING) {
        /* ── SCORING: dual-pane comparison ──────────────── */
        const char *user   = train_align_user;  /* aligned */
        const char *answer = train_buf;         /* aligned */
        const uint8_t *match = train_align_match;
        uint16_t total = train_align_len;
        uint16_t pos;
        uint16_t correct = 0;

        for (pos = 0; pos < total; pos++) {
            if (match[pos]) correct++;
        }

        /* Status bar */
        u8g2_DrawLine (&u8g2, 127, 8, 0, 8);
        u8g2_SetFont (&u8g2, u8g2_font_profont10_tr);
        {
            char score_buf[24];
            uint16_t pct = (total > 0) ? (correct * 100 / total) : 0;
            score_buf[0] = 'S'; score_buf[1] = 'c'; score_buf[2] = 'o';
            score_buf[3] = 'r'; score_buf[4] = 'e'; score_buf[5] = ':';
            score_buf[6] = ' ';
            {
                char *p = score_buf + 7;
                p = AppendUnsigned (p, correct);
                *p++ = '/';
                p = AppendUnsigned (p, total);
                *p++ = ' ';
                p = AppendUnsigned (p, pct);
                *p++ = '%';
                *p = '\0';
            }
            u8g2_DrawStr (&u8g2, 2, 7, score_buf);
            u8g2_DrawStr (&u8g2, 98, 7, "Esc=exit");
        }

        /* Vertical divider */
        u8g2_DrawVLine (&u8g2, 64, 10, 54);

        /* Text body: one group per line */
        u8g2_SetFont (&u8g2, u8g2_font_profont17_tr);
        u8g2_SetFontMode (&u8g2, 1);  /* transparent */
        {
            /* Build group index: record start/len of each group */
            uint16_t g_start[32], g_len[32];
            uint8_t  g_count = 0;
            uint16_t p = 0;
            while (p < total && g_count < 32) {
                uint16_t s = p;
                while (p < total && answer[p] != ' ') p++;
                g_start[g_count] = s;
                g_len[g_count]   = p - s;
                g_count++;
                if (p < total) p++;  /* skip space */
            }

            if (g_count < 1) { g_start[0] = 0; g_len[0] = total; g_count = 1; }

            /* Clamp scroll */
            if (train_score_scroll + 4 > g_count)
                train_score_scroll = (g_count > 4) ? (uint8_t)(g_count - 4) : 0;

            uint8_t gi;
            for (gi = 0; gi < 4; gi++) {
                uint8_t idx = train_score_scroll + gi;
                if (idx >= g_count) break;
                uint16_t gs = g_start[idx];
                uint16_t gl = g_len[idx];
                int y = 21 + gi * 12;

                /* Too-long group: wrap to next line(s) if needed */
                uint16_t chunk_start = gs;
                while (chunk_start < gs + gl) {
                    uint16_t chunk_len = gl - (chunk_start - gs);
                    if (chunk_len > 7) chunk_len = 7;

                    {
                        char buf[8]; uint8_t i;
                        for (i = 0; i < chunk_len; i++)
                            buf[i] = user[chunk_start + i];
                        buf[i] = '\0';
                        u8g2_SetDrawColor (&u8g2, 2);
                        u8g2_DrawStr (&u8g2, 1, y, buf);
                    }
                    {
                        char buf[8]; uint8_t i;
                        for (i = 0; i < chunk_len; i++)
                            buf[i] = answer[chunk_start + i];
                        buf[i] = '\0';
                        u8g2_SetDrawColor (&u8g2, 2);
                        u8g2_DrawStr (&u8g2, 65, y, buf);
                    }
                    /* Error boxes */
                    {
                        uint8_t i;
                        for (i = 0; i < chunk_len; i++) {
                            uint16_t idx = chunk_start + i;
                            if (!match[idx]) {
                                u8g2_SetDrawColor (&u8g2, 2);
                                u8g2_DrawBox (&u8g2, 1 + i * 9, y - 11, 9, 13);
                                u8g2_DrawBox (&u8g2, 65 + i * 9, y - 11, 9, 13);
                            }
                        }
                    }
                    chunk_start += chunk_len;
                    if (chunk_start < gs + gl) {
                        y += 12;
                        if (y > 63) break;
                    }
                }
            }
        }
        return;
    }

    /* ── READY / RUNNING : common status bar ──────────── */
    u8g2_DrawLine (&u8g2, 127, 8, 0, 8);
    u8g2_SetFont (&u8g2, u8g2_font_profont10_tr);

    {
        char buf[32];
        const char *method;
        if (train_info.methon == TRAIN_METHOD_KOCH)
            method = "KOCH";
        else if (train_info.methon == TRAIN_METHOD_SEQU)
            method = "SEQU";
        else
            method = "FREE";

        if (cursor_edit_mode) {
            buf[0] = 'e'; buf[1] = 'd'; buf[2] = 'i'; buf[3] = 't';
            buf[4] = ' '; buf[5] = 'L'; buf[6] = ' '; buf[7] = ' ';
        } else {
            buf[0] = 'W'; buf[1] = 'P'; buf[2] = 'M'; buf[3] = ':';
            buf[4] = '0' + (config.wpm / 10);
            buf[5] = '0' + (config.wpm % 10);
            buf[6] = ' ';
            buf[7] = 'L';
        }
        {
            char *p = buf + 8;
            p = AppendUnsigned (p, train_info.lesson);
            *p++ = ' ';
            {
                uint8_t mi = 0;
                while (method[mi]) { *p++ = method[mi++]; }
                *p = '\0';
            }
        }
        u8g2_DrawStr (&u8g2, 2, 7, buf);
    }

    if (train_phase == TRAIN_PHASE_READY) {
        /* Pre-roll prompt */
        u8g2_SetFont (&u8g2, u8g2_font_profont17_tr);
        u8g2_SetDrawColor (&u8g2, 2);
        u8g2_DrawStr (&u8g2, 1, 25, "Get ready...");
        u8g2_SetFont (&u8g2, u8g2_font_profont12_tr);
        u8g2_DrawStr (&u8g2, 1, 42, "Training starting");
        u8g2_DrawStr (&u8g2, 1, 56, "Esc = cancel");
    } else {
        /* RUNNING: 3 lines input + status bar at bottom */
        uint16_t len = inputBuffSize;
        uint16_t cur_pos = cursor_edit_mode ? cursor_pos : inputBuffSize;
        const char *src = inputBuff;
        uint16_t max_chars = (cur_pos > len) ? cur_pos : len;
        int total_lines = (max_chars > 0)
            ? ((int)(max_chars - 1) / chars_per_line) + 1 : 1;
        int cur_line = (cur_pos > 0) ? (int)(cur_pos / chars_per_line) : 0;
        int start_line;

        /* Compute visible start line */
        if (total_lines <= 3) {
            start_line = 0;
        } else if (cursor_edit_mode) {
            /* Edit mode: keep cursor in the visible 3-line window */
            if (cur_line < 1) start_line = 0;
            else if (cur_line > total_lines - 2) start_line = total_lines - 3;
            else start_line = cur_line - 1;
        } else {
            /* Normal: anchor to bottom */
            start_line = total_lines - 3;
        }

        /* 3 input lines (scroll when > 3 lines) */
        u8g2_SetFont (&u8g2, u8g2_font_profont17_tr);
        u8g2_SetDrawColor (&u8g2, 2);

        for (line = 0; line < 3; line++) {
            char line_buf[15];
            int src_line = start_line + line;
            uint16_t line_start = (uint16_t)src_line * chars_per_line;
            uint8_t i;

            if (line_start >= len) break;

            for (i = 0; i < chars_per_line && (line_start + i) < len; i++) {
                line_buf[i] = src[line_start + i];
            }
            line_buf[i] = '\0';
            u8g2_DrawStr (&u8g2, 1, 21 + line * 12, line_buf);
        }

        /* Blinking cursor: always in the visible area */
        if (curse_flash) {
            int cur_line = (cur_pos > 0) ? (int)(cur_pos / chars_per_line) : 0;
            int cur_col  = (cur_pos > 0) ? (int)(cur_pos % chars_per_line) : 0;
            int vis_line = cur_line - start_line;
            if (vis_line >= 0 && vis_line < 3) {
                u8g2_SetDrawColor (&u8g2, 1);
                u8g2_DrawLine (&u8g2,
                    1 + cur_col * 9,
                    11 + vis_line * 12,
                    1 + cur_col * 9,
                    21 + vis_line * 12);
            }
        }

        /* Divider line below 3rd input line (y=45+12=57, divider at y=51) */
        u8g2_DrawHLine (&u8g2, 0, 52, 128);

        /* Bottom status */
        u8g2_SetFont (&u8g2, u8g2_font_profont12_tr);
        if (stge) {
            u8g2_DrawStr (&u8g2, 2, 63, "Playing...");
        } else {
            u8g2_DrawStr (&u8g2, 2, 63, "Done  Ent=finish");
        }
    }
}

/* ── Training SETTING submenu ────────────────────────────── */
void show_train_setting (void)
{
    static int target_y = 2;
    static int now_y = 2;
    const uint8_t is_free = (train_info.methon == TRAIN_METHOD_FREE);

    u8g2_SetDrawColor (&u8g2, 2);
    u8g2_SetFont (&u8g2, u8g2_font_profont17_tr);

    if (is_free) {
        /* FREE mode: WPM, TIME, BACK */
        {
            char buf[8];
            if (train_setting_wpm_input) {
                buf[0] = ' ';
                if (train_setting_wpm_pos > 0)
                    buf[1] = '0' + ((train_setting_wpm_val / 10) % 10);
                else
                    buf[1] = ' ';
                buf[2] = '0' + (train_setting_wpm_val % 10);
                buf[3] = '\0';
            } else {
                buf[0] = ' '; buf[1] = '0' + (config.wpm / 10);
                buf[2] = '0' + (config.wpm % 10);
                buf[3] = '\0';
            }
            u8g2_DrawStr (&u8g2, 5, 13, "WPM");
            u8g2_DrawStr (&u8g2, 101, 13, buf);
            if (train_setting_wpm_input && curse_flash) {
                u8g2_DrawLine (&u8g2,
                    101 + train_setting_wpm_pos * 9, 13,
                    101 + train_setting_wpm_pos * 9 + 7, 13);
            }
        }

        {
            char buf[8];
            buf[0] = ' ';
            buf[1] = '0' + train_info.train_duration_min;
            buf[2] = 'm';
            buf[3] = 'i';
            buf[4] = 'n';
            buf[5] = '\0';
            u8g2_DrawStr (&u8g2, 5, 25, "TIME");
            u8g2_DrawStr (&u8g2, 101, 25, buf);
        }

        u8g2_DrawStr (&u8g2, 5, 37, "BACK");
    } else {
        /* KOCH / SEQU: CHAR/GRP, WPM, GAP, TIME, BACK */
        {
            char buf[8];
            if (train_info.chars_per_group == 0) {
                CopyText (buf, " RAND");
            } else {
                FormatUnsigned (buf, train_info.chars_per_group);
            }
            u8g2_DrawStr (&u8g2, 5, 13, "CHAR/GRP");
            u8g2_DrawStr (&u8g2, 101, 13, buf);
        }

        {
            char buf[8];
            if (train_setting_wpm_input) {
                buf[0] = ' ';
                if (train_setting_wpm_pos > 0)
                    buf[1] = '0' + ((train_setting_wpm_val / 10) % 10);
                else
                    buf[1] = ' ';
                buf[2] = '0' + (train_setting_wpm_val % 10);
                buf[3] = '\0';
            } else {
                buf[0] = ' ';
                buf[1] = '0' + (config.wpm / 10);
                buf[2] = '0' + (config.wpm % 10);
                buf[3] = '\0';
            }
            u8g2_DrawStr (&u8g2, 5, 25, "WPM");
            u8g2_DrawStr (&u8g2, 101, 25, buf);
            if (train_setting_wpm_input && curse_flash) {
                u8g2_DrawLine (&u8g2,
                    101 + train_setting_wpm_pos * 9, 25,
                    101 + train_setting_wpm_pos * 9 + 7, 25);
            }
        }

        {
            char buf[8];
            FormatUnsigned (buf, train_info.group_gap_spaces);
            u8g2_DrawStr (&u8g2, 5, 37, "GAP");
            u8g2_DrawStr (&u8g2, 101, 37, buf);
        }

        {
            char buf[8];
            buf[0] = ' ';
            buf[1] = '0' + train_info.train_duration_min;
            buf[2] = 'm';
            buf[3] = 'i';
            buf[4] = 'n';
            buf[5] = '\0';
            u8g2_DrawStr (&u8g2, 5, 49, "TIME");
            u8g2_DrawStr (&u8g2, 101, 49, buf);
        }

        u8g2_DrawStr (&u8g2, 5, 61, "BACK");
    }

    /* Cursor box: snap if gap > 1 row (page re-entry), else animate */
    target_y = 2 + train_setting_item * 12;
    if (now_y != target_y) {
        if (target_y > now_y + 12 || now_y > target_y + 12)
            now_y = target_y;
        else if (target_y > now_y)
            now_y += 2;
        else
            now_y -= 2;
    }
    u8g2_DrawBox (&u8g2, 3, now_y, 86, 11);
}
#endif /* COMPARE_FOR_VERSION_WITH_EEPROM */

void mode_0_word_disp()
{
    /* Use cursor_pos in edit mode, otherwise append position */
    uint16_t cur_pos = cursor_edit_mode ? cursor_pos : (uint16_t)strlen(inputBuff);
    uint32_t total_chars = (uint32_t)strlen(inputBuff);
    if (cur_pos > total_chars) total_chars = cur_pos;
    int total_lines = (total_chars > 0) ? ((int)(total_chars - 1) / 14) + 1 : 1;
    int cur_line = (int)(cur_pos / 14);
    int cur_col  = (int)(cur_pos % 14);
    int start_line;

    /* Compute visible start line */
    if (total_lines <= 4) {
        start_line = 0;
    } else if (cursor_edit_mode) {
        /* Edit mode: keep cursor in the visible 4-line window */
        if (cur_line < 1) start_line = 0;
        else if (cur_line > total_lines - 3) start_line = total_lines - 4;
        else start_line = cur_line - 1;
    } else {
        /* Normal: anchor to bottom, cursor at end */
        start_line = total_lines - 4;
    }

    int send_line = ((send_now) / 14);
    int send_num = ((send_now) % 14) + 1;
    int i;

    u8g2_SetDrawColor(&u8g2, 2);
    u8g2_SetFont(&u8g2, u8g2_font_profont17_tr);

    /* Draw 4 visible lines */
    for (i = 0; i < 4; i++) {
        int src_line = start_line + i;
        if (src_line * 14 < (int)strlen(inputBuff)) {
            strncpy(str_buff, inputBuff + src_line * 14, 14);
            u8g2_DrawStr(&u8g2, 1, 21 + i * 13, str_buff);
        }
    }

    /* Blinking cursor */
    {
        int vis_line = cur_line - start_line;
        if (vis_line >= 0 && vis_line < 4 && curse_flash) {
            u8g2_SetDrawColor(&u8g2, 1);
            u8g2_DrawLine(&u8g2,
                2 + 9 * cur_col, 10 + vis_line * 13,
                2 + 9 * cur_col, 21 + vis_line * 13);
        }
    }

    /* Send-progress highlight */
    if (stge) {
        int send_vis = send_line - start_line;
        if (send_vis >= 0 && send_vis < 4) {
            u8g2_SetDrawColor(&u8g2, 2);
            u8g2_DrawBox(&u8g2,
                9 * send_num - 8,
                9 + 13 * send_vis,
                9, 13);
        }
    }
}

void mode_1_word_disp()
{
    u8g2_DrawLine(&u8g2, 64, 10, 64, 63);

    /* Use cursor_pos in edit mode, otherwise append position */
    uint16_t cur_pos = cursor_edit_mode ? cursor_pos : (uint16_t)strlen(inputBuff);
    uint32_t total_chars = (uint32_t)strlen(inputBuff);
    if (cur_pos > total_chars) total_chars = cur_pos;
    int total_lines = (total_chars > 0) ? ((int)(total_chars - 1) / 7) + 1 : 1;
    int cur_line = (int)(cur_pos / 7);
    int cur_col  = (int)(cur_pos % 7);
    int start_line;

    /* Compute visible start line */
    if (total_lines <= 4) {
        start_line = 0;
    } else if (cursor_edit_mode) {
        /* Edit mode: keep cursor in the visible 4-line window */
        if (cur_line < 1) start_line = 0;
        else if (cur_line > total_lines - 3) start_line = total_lines - 4;
        else start_line = cur_line - 1;
    } else {
        /* Normal: anchor to bottom, cursor at end */
        start_line = total_lines - 4;
    }

    int send_line = ((send_now) / 7);
    int send_num = ((send_now) % 7) + 1;
    int i;

    u8g2_SetFont(&u8g2, u8g2_font_profont17_tr);

    /* Draw 4 visible lines (left pane: input) */
    for (i = 0; i < 4; i++) {
        int src_line = start_line + i;
        if (src_line * 7 < (int)strlen(inputBuff)) {
            strncpy(str_buff, inputBuff + src_line * 7, 7);
            u8g2_DrawStr(&u8g2, 1, 21 + i * 13, str_buff);
        }
    }

    /* Blinking cursor (left pane) */
    {
        int vis_line = cur_line - start_line;
        if (vis_line >= 0 && vis_line < 4 && curse_flash) {
            u8g2_SetDrawColor(&u8g2, 1);
            u8g2_DrawLine(&u8g2,
                2 + 9 * cur_col, 10 + vis_line * 13,
                2 + 9 * cur_col, 21 + vis_line * 13);
        }
    }

    /* Right pane: output / send progress */
    if (stge) {
        uint32_t out_chars = (uint32_t)strlen(outputBuff);
        int out_lines = (out_chars > 0) ? ((int)(out_chars - 1) / 7) + 1 : 1;
        int out_start = (out_lines > 4) ? (out_lines - 4) : 0;

        for (i = 0; i < 4; i++) {
            int src_line = out_start + i;
            if (src_line * 7 < (int)out_chars) {
                strncpy(str_buff, outputBuff + src_line * 7, 7);
                u8g2_DrawStr(&u8g2, 65, 21 + i * 13, str_buff);
            }
        }

        /* Send-progress highlight (right pane) */
        {
            int send_vis = send_line - out_start;
            if (send_vis >= 0 && send_vis < 4) {
                u8g2_SetDrawColor(&u8g2, 2);
                u8g2_DrawBox(&u8g2,
                    9 * send_num + 56,
                    9 + 13 * send_vis,
                    9, 13);
            }
        }
    }
}

void show_main_page(void) {
    u8g2_DrawLine(&u8g2, 127, 8, 0, 8);

    if(config.beeper)
        u8g2_DrawXBM(&u8g2, STATUS_SPEAKER_ICON_X, 1, 8, 6, image_Volup_bits);
    else
        u8g2_DrawXBM(&u8g2, STATUS_SPEAKER_ICON_X, 0, 8, 8, image_Muted_bits);

#if UI_SHOW_BATTERY_ICON
    u8g2_DrawXBM(&u8g2, STATUS_BATTERY_ICON_X, 0, 26, 8, image_Battery_bits);
#endif

    u8g2_SetFont(&u8g2, u8g2_font_profont10_tr);

    if (cursor_edit_mode) {
        u8g2_DrawStr(&u8g2, 2, 7, "edit");
    } else {
        FormatWpmText (str_buff, config.wpm);
        u8g2_DrawStr(&u8g2, 2, 7, str_buff);
    }

    if (repeat_active) {
        /* Compact repeat display: "R1/3 5s" or infinite "R123 5s" */
        char rpt_buf[20];
        char *p = rpt_buf;
        if (repeat_phase == REPEAT_PHASE_BUF_PEND) {
            /* Initial send in buf mode: show "R--- SND" */
            memcpy (p, "R--- SND", 8);
            p += 8;
        } else {
            *p++ = 'R';
            p = AppendUnsigned (p, repeat_counter);
            if (config.repeat_config.repeat_count != 0) {
                /* Finite: show counter/total */
                *p++ = '/';
                p = AppendUnsigned (p, config.repeat_config.repeat_count);
            }
            *p++ = ' ';
            p = AppendUnsigned (p, repeat_countdown_s);
            *p++ = 's';
        }
        *p = '\0';
        u8g2_DrawStr(&u8g2, 35, 7, rpt_buf);
    } else {
        FormatMsgLenText (str_buff, strlen(inputBuff));
        u8g2_DrawStr(&u8g2, 35, 7, str_buff);
    }

#if UI_SHOW_BATTERY_ICON
    u8g2_SetFont(&u8g2, u8g2_font_tinyunicode_tr);
    FormatBatteryText (str_buff, bat_adc_val);
    u8g2_DrawStr(&u8g2, STATUS_BATTERY_TEXT_X, 6, str_buff);
#endif

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
#if COMPARE_FOR_VERSION_WITH_EEPROM
    else if(disp_train_menu)
        show_train_menu();
    else if(disp_train_setting)
        show_train_setting();
    else if(disp_training)
        show_training_page();
#endif
    else
        show_main_page();

    u8g2_SendBuffer(&u8g2); 
}