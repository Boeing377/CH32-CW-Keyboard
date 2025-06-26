#include "ButtonFunc.h"

void ButtonChangeBeeper()
{
    config.beeper = 1 - config.beeper;  // 切换是否使用蜂鸣器
    WriteConfigEEPROM();
}

void ButtonChangeMode()
{
    config.mode = 1 - config.mode;  // 切换模式
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
    disp_menu = 1 - disp_menu;
}

void ButtonOpenTrain()
{
    disp_train_menu = 1 - disp_train_menu;
}

