#include "ButtonFunc.h"

typedef void (*ButtonActionFunc) (void);

static ButtonActionFunc ButtonResolveAction (uint8_t action_index) {
    switch (action_index) {
    case BUTTON_ACTION_BEEPER:
        return ButtonChangeBeeper;

    case BUTTON_ACTION_MODE:
        return ButtonChangeMode;

    case BUTTON_ACTION_OPEN_MENU:
        return ButtonOpenMenu;

    default:
        return ButtonOpenMenu;
    }
}

static uint8_t ButtonSanitizeActionIndex (uint8_t action_index) {
    if (action_index >= BUTTON_ACTION_COUNT) {
        return BUTTON_ACTION_BEEPER;
    }

    return action_index;
}

void ButtonChangeBeeper()
{
    if (repeat_active) return;
    config.beeper = 1 - config.beeper;  // 切换是否使用蜂鸣器
    WriteConfigEEPROM();
}

void ButtonChangeMode()
{
    if (repeat_active) return;
    config.mode = 1 - config.mode;  // �л�ģʽ
    endSending();
    inputBuffSize = 0;
    memset (inputBuff, '\0', BUFFSIZE);
    outputBuffSize = 0;
    memset (outputBuff, '\0', BUFFSIZE);
    sendCount = 0;
    WriteConfigEEPROM();
}

void ButtonOpenMenu()
{
    if (repeat_active) return;
    disp_menu = 1 - disp_menu;
}

void ButtonHandleKey1LongPress()
{
    disp_train_menu = 0;
    disp_ver = 0;
    disp_morse_conf = 0;
    disp_button_func = 0;
    disp_confirm_reset = 0;
    menu_item = 0;
    disp_menu = 1;
}

void ButtonHandleKey2LongPress()
{
}

void ButtonApplyActionIndex (uint8_t button_id, uint8_t action_index)
{
    action_index = ButtonSanitizeActionIndex (action_index);

    if (button_id == BUTTON_ID_1) {
        config.button_func.bt1_func_index = action_index;
        config.button_func.bt1_func_reserved = 0;
    } else if (button_id == BUTTON_ID_2) {
        config.button_func.bt2_func_index = action_index;
        config.button_func.bt2_func_reserved = 0;
    }
}

void ButtonApplyConfiguredActions (void)
{
    ButtonApplyActionIndex (BUTTON_ID_1, config.button_func.bt1_func_index);
    ButtonApplyActionIndex (BUTTON_ID_2, config.button_func.bt2_func_index);
}

void ButtonExecuteConfiguredAction (uint8_t button_id)
{
    ButtonActionFunc action = ButtonOpenMenu;

    if (button_id == BUTTON_ID_1) {
        action = ButtonResolveAction (config.button_func.bt1_func_index);
    } else if (button_id == BUTTON_ID_2) {
        action = ButtonResolveAction (config.button_func.bt2_func_index);
    }

    action();
}

void ButtonOpenTrain()
{
    disp_train_menu = 1 - disp_train_menu;
}

