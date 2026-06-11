<!--
  打印建议：Chrome 浏览器 → 打印 → 另存为 PDF → A4，边距最小。
  Print: Chrome → Print → Save as PDF → A4, minimal margins.
-->

<!-- ================================================================== -->
<!--                        封面 / COVER PAGE                            -->
<!-- ================================================================== -->

<div style="text-align:center; page-break-after:always;">

<br/><br/><br/><br/>

# CH32-CW-Keyboard

## 用户手册 / User Manual

<br/>

### 一台将 USB 键盘变成莫尔斯电码自动键的小设备
### A tiny device that turns a USB keyboard into a Morse code keyer

**3.5mm 音频接口输出直键键控信号，用标准音频线连接电台 CW KEY 接口。**
**3.5mm audio jack outputs straight-key keying signal. Connect to radio CW KEY with a standard audio cable.**

<br/>

```
 ██████╗██╗    ██╗    ██╗  ██╗███████╗██╗   ██╗██████╗  ██████╗  █████╗ ██████╗ ██████╗
██╔════╝██║    ██║    ██║ ██╔╝██╔════╝╚██╗ ██╔╝██╔══██╗██╔═══██╗██╔══██╗██╔══██╗██╔══██╗
██║     ██║ █╗ ██║    █████╔╝ █████╗   ╚████╔╝ ██████╔╝██║   ██║███████║██████╔╝██║  ██║
██║     ██║███╗██║    ██╔═██╗ ██╔══╝    ╚██╔╝  ██╔══██╗██║   ██║██╔══██║██╔══██╗██║  ██║
╚██████╗╚███╔███╔╝    ██║  ██╗███████╗   ██║   ██████╔╝╚██████╔╝██║  ██║██║  ██║██████╔╝
 ╚═════╝ ╚══╝╚══╝     ╚═╝  ╚═╝╚══════╝   ╚═╝   ╚═════╝  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝
```

<br/>

**固件版本 / Firmware Version: 1.1.9**

**由 DeVolt Elec 设计 / Designed by DeVolt Elec**

<br/><br/><br/>

</div>

<!-- ================================================================== -->
<!--                        目录 / TOC                                   -->
<!-- ================================================================== -->

## 目录 / Table of Contents

|   | 章节 | 
|---|------|
| 1 | 快速开始 / Quick Start |
| 2 | 主屏幕 / Main Screen |
| 3 | 两个工作模式 / Two Working Modes |
| 4 | 按键操作 / Button Operations |
| 5 | 主菜单 / Main Menu |
| 6 | 莫尔斯电码设置 / Morse Settings |
| 7 | 按键功能设置 / Button Function Settings |
| 8 | 重复发送 / Repeat Sending |
| 9 | 预存消息 / Saved Messages |
| 10 | 训练模式 / Training Mode |
| 11 | 键盘布局 / Keyboard Layout |
| 12 | 版本页面 / Version Page |
| 13 | 恢复出厂设置 / Factory Reset |
| 附录 | 支持字符集 / Supported Characters |

---

<!-- ================================================================== -->
<!--                      1. 快速开始 / Quick Start                      -->
<!-- ================================================================== -->

## 1. 快速开始 / Quick Start

### 你需要准备 / What You Need

* CH32-CW-Keyboard 主板一块（已内置蜂鸣器）/ One CH32-CW-Keyboard board (buzzer built-in)
* 标准 USB 键盘一把（有线 HID 键盘）/ One standard USB keyboard (wired HID)
* 5V USB 电源或 3.7V 锂电池 / 5V USB power or 3.7V Li-ion battery
* （可选）3.5mm 公对公音频线，连接设备与电台 KEY 接口 / (Optional) 3.5mm male-to-male audio cable to connect device to radio KEY jack

### 三步上手 / Three Steps

```
  [1] 插键盘               [2] 上电                [3] 打字，开发送！
  Plug keyboard           Power on               Type and send!
      │                      │                      │
      ▼                      ▼                      ▼
  ┌─────────┐          ┌───────────┐          ┌───────────┐
  │ USB HOST│─────────▶│ OLED 亮起  │─────────▶│  嘀 嘀 哒  │
  │  接口   │          │ 显示欢迎页 │          │  BEEP!    │
  └─────────┘          └───────────┘          └───────────┘
```

1. 将 USB 键盘插入设备的 **USB Host** 口。
   Plug a USB keyboard into the device's **USB Host** port.

2. 给设备供电（USB 或电池）。OLED 屏幕亮起，显示启动画面。
   Power the device (USB or battery). The OLED lights up with a welcome screen.

3. 直接在 USB 键盘上打字——设备内置蜂鸣器发出莫尔斯电码声，同时 3.5mm 接口输出键控信号！
   Type on the USB keyboard — the built-in buzzer sounds Morse code, while the 3.5mm jack outputs the keying signal!

> 💡 **提示 / Tip**：键控信号始终输出（3.5mm 口）。Beeper 开关仅控制内置蜂鸣器是否响。
> The keying signal is always output on the 3.5mm jack. Beeper ON/OFF only controls the built-in buzzer.
>
> 🔑 **快捷键 / Shortcuts**：`Ctrl+M` 菜单 | `Ctrl+T` 训练 | `Ctrl+R` 重复 | `Esc` 停止重复
> `Ctrl+M` menu | `Ctrl+T` training | `Ctrl+R` repeat | `Esc` stop repeat

---

<!-- ================================================================== -->
<!--                      2. 主屏幕 / Main Screen                        -->
<!-- ================================================================== -->

## 2. 主屏幕 / Main Screen

开机后的主画面，这是你最常面对的界面。

The main screen after boot — this is where you'll spend most of your time.

```
     ┌──────────────────────────────────────────────────┐
     │ WPM:20  MsgLen:0          ♫  ▓▓▓▓▓▓▓▓▓  ▲  4.0V │  ← 状态栏
     ├──────────────────────────────────────────────────┤     Status bar
     │                                                  │
     │   H E L L O   W O R L D _ │                      │  ← 输入区
     │                                                  │     Input area
     │                                                  │
     │                                                  │
     └──────────────────────────────────────────────────┘
```

| 状态栏图标 | 含义 / Meaning |
|---|---|
| ♫ | 内置蜂鸣器开 / Built-in buzzer ON |
| ✕♫ (muted图标) | 内置蜂鸣器关（键控信号仍输出）/ Buzzer OFF (keying signal still active) |
| ▓▓▓ | 电池图标 (仅EEPROM版) / Battery icon (EEPROM only) |
| ▲ | Caps Lock 大写锁定 / Caps Lock ON |
| 4.0V | 电池电压 (仅EEPROM版) / Battery voltage (EEPROM only) |
| MsgLen:0 | 当前已输入字符数 / Characters typed so far |

> **3.5mm 接口始终输出直键键控信号**（低电平有效），用标准音频线直连电台 CW KEY 接口。内置蜂鸣器提供本地监听，可在菜单中开关。
> **The 3.5mm jack always outputs a straight-key keying signal** (active low). Connect to radio CW KEY with a standard audio cable. The built-in buzzer provides local monitoring and can be toggled in the menu.

### 全局快捷键 / Global Shortcuts

| 快捷键 / Shortcut | 功能 / Function |
|---|---|
| **Ctrl+M** | 打开/关闭主菜单 / Open/close main menu |
| **Ctrl+T** | 打开/关闭训练菜单（仅EEPROM版）/ Open/close training menu (EEPROM only) |
| **Ctrl+R** | 启动重复发送 / Start repeat sending |
| **Esc** | 停止重复发送（重复中）/ Stop repeat (when active) |

> 这些快捷键在主界面和菜单中均有效，是最快捷的操作方式。

---

<!-- ================================================================== -->
<!--                   3. 两个工作模式 / Two Working Modes                -->
<!-- ================================================================== -->

## 3. 两个工作模式 / Two Working Modes

设备有两种发送模式。按 **Ctrl+M** → Mode 切换。

The device has two sending modes. Press **Ctrl+M** → Mode to switch.

### 模式 0 — 即时发送 / Mode 0 — Immediate (DIR)

```
   打字即发送，字字不落。
   Type and send instantly.

   ┌──────────────────────────────────┐
   │ 键盘输入:  H → E → L → L → O     │
   │ 听到:      嘀嘀嘀嘀 嘀 嘀嗒嘀嘀 ... │
   │             ....   .  .-..  ...  │
   │                                  │
   │ 屏幕：4行 × 14字符               │
   │ 正在发送的字符 ▐高亮方框▐ 标识    │
   └──────────────────────────────────┘
```

> 适用：实时通联、日常练习、随打随发。
> Best for: Live QSO, daily practice, type-as-you-go.

### 模式 1 — 缓存发送 / Mode 1 — Buffer (BUF)

```
   先编辑好，再一键发送。像发短信。
   Compose first, send with one key. Like texting.

   ┌──────────────┬──────────────┐
   │  输入编辑区   │  正在发送区   │  ← 屏幕竖线分成两半
   │  Input/Edit  │  Sending     │    Split by vertical line
   │              │              │
   │  CQ CQ CQ   │  CQ CQ CQ    │
   │  DE BG7WG   │  DE BG7WG    │
   │  K_         │  K           │
   │              │              │
   └──────────────┴──────────────┘
    每半屏 4行×7字符 / 4 lines × 7 chars each side

   输入时可随意修改删除，确认无误按 Enter 一次性全部发送。
   Edit freely, then press Enter to send everything at once.
```

> 适用：比赛预设呼号、CQ 呼叫、需要反复修改的报文。
> Best for: Contest macros, CQ calls, messages needing editing.

---

<!-- ================================================================== -->
<!--                   4. 按键操作 / Button Operations                    -->
<!-- ================================================================== -->

## 4. 按键操作 / Button Operations

### 全局快捷键（推荐）/ Global Shortcuts (Recommended)

最快捷的操作方式——在主界面直接按组合键：

The fastest way to operate — press key combos directly on the main screen:

| 快捷键 | 功能 |
|---|---|
| **Ctrl+M** | 打开/关闭主菜单 / Open/close main menu |
| **Ctrl+T** | 打开/关闭训练菜单 / Open/close training menu |
| **Ctrl+R** | 启动重复发送 / Start repeat sending |
| **Esc** | 停止重复发送（重复中）/ Stop repeat (when active) |

### 硬件按键 / Hardware Buttons

```
        KEY1 (PB13)              KEY2 (PB14)
        ┌──────┐                 ┌──────┐
        │  BT1 │                 │  BT2 │
        └──────┘                 └──────┘

   短按 (<0.8s) ───▶ 执行自定义功能      短按 (<0.8s) ───▶ 执行自定义功能
   长按 (≥0.8s) ───▶ 强制打开主菜单      长按 (≥0.8s) ───▶ (保留)

   Short press: custom function      Short press: custom function
   Long press:  force open menu      Long press:  (reserved)
```

> 默认：KEY1 短按 = OPEN MENU，KEY2 短按 = MODE SW。可在菜单 → Btn Func 中修改。

### USB 键盘快捷键 / USB Keyboard Shortcuts

| 按键 / Key | 功能 / Function |
|---|---|
| **Enter** | 发送（模式0）/ 开始发送（模式1）/ 确认菜单 / Send (Mode 0) / Start send (Mode 1) / Confirm |
| **Backspace** | 删除一个字符 / Delete one character |
| **Esc** | 退出菜单 / 清空缓冲区 / 停止重复 / Exit menu / Clear buffer / Stop repeat |
| **Caps Lock** | 切换大小写（屏幕▲）/ Toggle case (▲ icon) |
| **↑ ↓** | 菜单上下移动 / Navigate menu |
| **← →** | 菜单中调数值 / Adjust values |
| **0–9** | 菜单项跳转 / 输数字 / Jump to item / Enter numbers |
| **F1–F12** | 读取预存消息 1–12 / Load saved message 1–12 |
| **Shift+F1–F12** | 保存当前文字到槽位 / Save current text to slot |
| **Ctrl+M** | 打开/关闭主菜单 / Open/close main menu |
| **Ctrl+T** | 打开/关闭训练菜单 / Open/close training menu |
| **Ctrl+R** | 启动重复发送 / Start repeat sending |
| **Esc** | 停止重复发送（重复中）/ Stop repeat (when active) |

---

<!-- ================================================================== -->
<!--                      5. 主菜单 / Main Menu                          -->
<!-- ================================================================== -->

## 5. 主菜单 / Main Menu

按 **Ctrl+M** 或长按 KEY1 进入。↑↓ 移动，Enter 确认，Esc 退出。

Press **Ctrl+M** or long-press KEY1 to enter. ↑↓ to move, Enter to confirm, Esc to exit.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │   Mode                      BUF     │  ← 当前值
   │  ▐beeper                    OFF ▐   │  ← 反白方块 = 当前选中
   │   Morse Cfg                         │     Highlighted = selected
   │   Btn Func                          │
   │   Repeat                            │  菜单可滚动，最多5项可见
   │   Keyboard                  QWERTY  │  Menu scrolls, max 5 visible
   │   Version                           │
   │   RESET                             │
   │   SAVE&EXIT                         │
   │                                     │
   └─────────────────────────────────────┘
```

| # | 菜单项 | 按 Enter 后... |
|---|---|---|
| **Mode** | 切换 DIR(即时) / BUF(缓存) |
| **beeper** | 切换内置蜂鸣器 ON / OFF（不影响 3.5mm 键控输出） |
| **Morse Cfg** | 进入电码设置子菜单 |
| **Btn Func** | 进入按键功能设置子菜单 |
| **Repeat** | 进入重复发送设置子菜单 |
| **Keyboard** | 切换 QWERTY / AZERTY |
| **Version** | 显示版本信息 |
| **RESET** | 恢复出厂设置（需二次确认） |
| **SAVE&EXIT** | 保存配置并退出 |

> ⚠️ 训练模式不在主菜单中，请用 **Ctrl+T** 打开。
> ⚠️ Training mode is NOT in the main menu. Use **Ctrl+T** to open it.

---

<!-- ================================================================== -->
<!--                  6. 莫尔斯电码设置 / Morse Settings                  -->
<!-- ================================================================== -->

## 6. 莫尔斯电码设置 / Morse Settings

菜单 → **Morse Cfg** 进入。

Menu → **Morse Cfg** to enter.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │  ▐Word Split              Short  ▐  │  ← 单词间隔 / Word spacing
   │                                     │
   │   Cut Num                 OFF       │  ← 数字短码 / Shortened numbers
   │                                     │
   │   BACK                               │  ← 返回
   │                                     │
   └─────────────────────────────────────┘
```

### 单词间隔 / Word Split

控制每两个单词之间的停顿长度。

Controls the pause length between words.

| 选项 | 倍数 vs 点 | 感觉 |
|---|---|---|
| **Short** | 7× | 快，适合通联 / Fast, QSO |
| **Mid** | 10× | 适中 / Moderate |
| **Long** | 14× | 慢，适合练习 / Slow, practice |

### 数字短码 / Cut Num

标准摩尔斯数字码每个数字 5 个符号，比较长。短码用更少的符号来发数字，加快速度。

Standard Morse digits are 5 elements each — long. Cut Num uses fewer elements per digit to speed up numbers.

| 方案 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|---|---|---|---|---|---|---|---|---|---|---|
| **OFF**(标准) | −−−−− | ·−−−− | ··−−− | ···−− | ····− | ····· | −···· | −−··· | −−−·· | −−−−· |
| **Mod A** | − | ·− | ··− | ···− | ····− | ····· | −···· | −−··· | −·· | −· |
| **Mod B** | − | ·− | ··− | ···− | ····− | · | −···· | −··· | −·· | −· |
| **Mod C** | − | ·− | ··− | ·−− | ···− | ··· | −··· | −−· | −·· | −· |

> 短码仅影响 0-9 数字。字母和标点符号不受影响。

---

<!-- ================================================================== -->
<!--               7. 按键功能设置 / Button Function Settings             -->
<!-- ================================================================== -->

## 7. 按键功能设置 / Button Function Settings

菜单 → **Btn Func** 进入。

Menu → **Btn Func** to enter.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │  ▐BT1 FUN              OPEN MENU ▐  │  ← KEY1 的短按功能
   │                                     │     KEY1 short-press function
   │   BT2 FUN              MODE SW      │  ← KEY2 的短按功能
   │                                     │     KEY2 short-press function
   │   BACK                               │
   │                                     │
   └─────────────────────────────────────┘
```

每个按键可从以下三个功能中选一个：

Each button can be assigned one of three functions:

| 可选功能 | 效果 |
|---|---|
| **BEEPER SW** | 短按切换内置蜂鸣器 开/关 / Toggle built-in buzzer ON/OFF |
| **MODE SW** | 短按切换 即时/缓存 模式 / Toggle DIR/BUF mode |
| **OPEN MENU** | 短按打开主菜单 / Open main menu |

> 注意：无论 KEY1 短按设成什么，**长按 KEY1 始终会打开主菜单**。
> Note: Regardless of KEY1's short-press setting, **long-pressing KEY1 always opens the main menu**.

---

<!-- ================================================================== -->
<!--                  8. 重复发送 / Repeat Sending                       -->
<!-- ================================================================== -->

## 8. 重复发送 / Repeat Sending

让设备反复发送同一段文字，像信标 (beacon) 一样工作。

Have the device repeatedly send the same text, like a beacon.

菜单 → **Repeat** 进入设置。

Menu → **Repeat** to configure.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │   RPT COUNT                          │
   │      003_                           │  ← 发几遍 (000=无限/INF)
   │                                     │     Repeat count
   │   RPT INTER                         │
   │       5s                            │  ← 隔几秒 / Interval
   │                                     │
   │   BACK                               │
   │                                     │
   └─────────────────────────────────────┘
```

| 参数 | 默认 | 范围 | 说明 |
|---|---|---|---|
| RPT COUNT | 3 | 1–999；0=无限(INF) | 发送几遍 / How many times |
| RPT INTER | 5s | 1–999s | 两遍之间隔几秒 / Interval between sends |

### 启动与停止 / Start & Stop

> **按 `Ctrl+R` 启动重复发送，按 `Esc` 停止。**
> **Press `Ctrl+R` to start repeat, press `Esc` to stop.**

```
  [1] 设置次数和间隔          [2] 主界面输文字           [3] Ctrl+R 开始循环
  Set count & interval      Type text on main        Ctrl+R to start loop

   ┌──────────┐            ┌──────────────┐          ┌──────────────┐
   │RPT COUNT 3│            │ CQ CQ CQ     │          │R1/3   5s     │  ← 状态栏进度
   │RPT INTER 5│──────▶     │ DE BG7WG     │─────▶    │ CQ CQ CQ     │     Status bar
   └──────────┘            │ K            │          │ DE BG7WG     │
                           └──────────────┘          │ K            │
                                                     └──────────────┘
  按 Esc 停止 / Press Esc to stop.
```

**工作流程 / How it works：**
- 在主界面输入要重复发送的文字。
- 按 `Ctrl+R`：当前输入内容被保存，开始按设定的间隔反复发送。
- 状态栏显示 `R1/3 5s` 表示「第1遍/共3遍，倒计时5秒」。
- 按 `Esc` 停止。

Type the text to repeat on the main screen, then press `Ctrl+R`. The text is saved and sent repeatedly at the configured interval. The status bar shows progress. Press `Esc` to stop.

---

<!-- ================================================================== -->
<!--                   9. 预存消息 / Saved Messages                       -->
<!-- ================================================================== -->

## 9. 预存消息 / Saved Messages

12 个槽位，每个可存一段文字。用键盘 **F1–F12** 键直接操作，不经过菜单。

12 slots, each stores a message. Accessed directly via **F1–F12** keys — no menu needed.

### 操作 / Operation

```
   读取 / Load：
     按 F1~F12 → 对应槽位的消息自动加载到输入区并开始发送（模式0）
     按 F1~F12 → 对应槽位的消息自动加载到输入区（模式1）
     Press F1~F12 → message from that slot auto-loads and sends (Mode 0)
     Press F1~F12 → message from that slot auto-loads into editor (Mode 1)

   保存 / Save：
     在主界面输好文字 → 按 Shift+F1~Shift+F12 → 保存到对应槽位
     Type text on main → press Shift+F1~Shift+F12 → save to that slot
```

```
   槽位对照 / Slot mapping:
   ┌────────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐
   │ 槽位   │  1 │  2 │  3 │  4 │  5 │  6 │  7 │  8 │  9 │ 10 │ 11 │ 12 │
   │ Slot   │    │    │    │    │    │    │    │    │    │    │    │    │
   ├────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
   │ 读取   │ F1 │ F2 │ F3 │ F4 │ F5 │ F6 │ F7 │ F8 │ F9 │F10 │F11 │F12 │
   │ Load   │    │    │    │    │    │    │    │    │    │    │    │    │
   ├────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
   │ 保存   │S-F1│S-F2│S-F3│S-F4│S-F5│S-F6│S-F7│S-F8│S-F9│S-F10│S-F11│S-F12│
   │ Save   │    │    │    │    │    │    │    │    │    │    │    │    │
   └────────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
   S-F1 = Shift+F1
```

| 版本 | 每条最大 |
|---|---|
| EEPROM 版 | ~2KB (2044 字节) |
| Flash 版 | 508 字节 |

> 💡 推荐规划 / Suggested use：
> F1 = CQ 呼叫 / CQ call，F2 = 呼号 / Callsign，F3 = 信号报告 / Signal report，
> F4 = 姓名/QTH / Name/QTH，F5 = 73 结束语 / 73 closing …

---

<!-- ================================================================== -->
<!--                   10. 训练模式 / Training Mode                       -->
<!-- ================================================================== -->

## 10. 训练模式 / Training Mode

> ⚠️ 训练模式仅在 **EEPROM 版本**中可用。
> ⚠️ Training mode is only available in the **EEPROM version**.

> 🔑 按 **Ctrl+T** 打开训练菜单。训练模式独立于主菜单，不在主菜单中。
> 🔑 Press **Ctrl+T** to open the training menu. Training is separate from the main menu.

### 训练主菜单 / Training Main Menu

按 **Ctrl+T** 进入。

Press **Ctrl+T** to enter.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │   METHOD                  KOCH      │  ← 训练方法 / Training method
   │   LESSON                    1       │  ← 当前课次 / Current lesson
   │  ▐SETTING                 ▐         │  ← 进入设置
   │   START                             │  ← 开始训练
   │   EXIT                              │  ← 退出
   │                                     │
   └─────────────────────────────────────┘
```

### 三种方法 / Three Methods

| 方法 | 原理 |
|---|---|
| **KOCH** | 按字母使用频率排序，每课＋1个新字母。第1课只练 K 和 M。标准业余无线电训练法。 / Adds 1 new letter per lesson in frequency order. Lesson 1 = K, M. Standard ham radio training. |
| **SEQU** | 按字母表顺序 A→B→C… 逐步学习，学完 26 个字母后接着 0→1→2… 数字。 / Sequential A→B→C… then 0→1→2… after letters. |
| **FREE** | 自由模式：A–Z 和 0–9 全部随机出题。没有课程进度。 / Free mode: all A–Z and 0–9 randomized. No lesson progression. |

### Koch 课程字符序列表 / Koch Lesson Character Sequence

按无线电使用频率从高到低排列。第 N 课涵盖前 N+1 个字符。

Characters ordered by radio usage frequency. Lesson N covers the first N+1 characters.

```
  课次          新增字符                    累计字符
  Lesson        New char                   Cumulative pool
  ─────────────────────────────────────────────────────────────
   1            K  M                       K M
   2            U                          K M U
   3            R                          K M U R
   4            E                          K M U R E
   5            S                          K M U R E S
   6            N                          K M U R E S N
   7            A                          K M U R E S N A
   8            P                          K M U R E S N A P
   9            T                          K M U R E S N A P T
  10            L                          K M U R E S N A P T L
  11            W                          ... + W
  12            I                          ... + I
  13            . (句号)                    ... + .
  14            J                          ... + J
  15            Z                          ... + Z
  16            =                          ... + =
  17            F                          ... + F
  18            O                          ... + O
  19            Y                          ... + Y
  20            , (逗号)                    ... + ,
  21            V                          ... + V
  22            G                          ... + G
  23            5                          ... + 5
  24            /                          ... + /
  25            Q                          ... + Q
  26            9                          ... + 9
  27            2                          ... + 2
  28            H                          ... + H
  29            3                          ... + 3
  30            8                          ... + 8
  31            B                          ... + B
  32            ? (问号)                    ... + ?
  33            4                          ... + 4
  34            7                          ... + 7
  35            C                          ... + C
  36            1                          ... + 1
  37            D                          ... + D
  38            6                          ... + 6
  39            0                          ... + 0
  40            X                          K M U R E S N A P T L W I . J Z
                                            = F O Y , V G 5 / Q 9 2 H 3 8
                                            B ? 4 7 C 1 D 6 0 X  (共40个)
```

> 💡 **提示 / Tip**：建议从第 1 课开始，确保每课正确率 >90% 后再进入下一课。

### Sequential 课程字符序列表 / Sequential Lesson Character Sequence

按字母表顺序 A→Z，然后 0→9。第 N 课涵盖前 N+1 个字符。

In alphabetical order A→Z, then digits 0→9. Lesson N covers the first N+1 characters.

```
  课次          新增字符                    累计字符
  Lesson        New char                   Cumulative pool
  ─────────────────────────────────────────────────────────────
   1            A  B                       A B
   2            C                          A B C
   3            D                          A B C D
   4            E                          A B C D E
   5            F                          A B C D E F
   6            G                          A B C D E F G
   7            H                          ... + H
   8            I                          ... + I
   9            J                          ... + J
  10            K                          ... + K
  11            L                          ... + L
  12            M                          ... + M
  13            N                          ... + N
  14            O                          ... + O
  15            P                          ... + P
  16            Q                          ... + Q
  17            R                          ... + R
  18            S                          ... + S
  19            T                          ... + T
  20            U                          ... + U
  21            V                          ... + V
  22            W                          ... + W
  23            X                          ... + X
  24            Y                          ... + Y
  25            Z                          A B C D E F G H I J K L M
                                            N O P Q R S T U V W X Y Z
  26            0                          ... + 0
  27            1                          ... + 1
  28            2                          ... + 2
  29            3                          ... + 3
  30            4                          ... + 4
  31            5                          ... + 5
  32            6                          ... + 6
  33            7                          ... + 7
  34            8                          ... + 8
  35            9                          A–Z + 0–9 (共36个)
```

> Sequential 最多 35 课（26 个字母 + 10 个数字）。适合想按熟悉程度逐步推进的用户。

### 训练设置 (Koch/Sequential) / Training Settings (Koch/Sequential)

```
   ┌─────────────────────────────────────┐
   │                                     │
   │  ▐CHAR/GRP                 5     ▐  │  ← 每组几个字符 (0=随机/RAND)
   │   WPM                      20      │  ← 训练速度 / Training speed
   │   GAP                      1       │  ← 组间空格数 / Spaces between groups
   │   TIME                    2min     │  ← 训练时长 1-5分钟 / Duration 1-5min
   │   BACK                             │
   │                                     │
   └─────────────────────────────────────┘

   自由模式仅: WPM, TIME, BACK 三项。
   Free mode only: WPM, TIME, BACK.
```

### 训练流程 / Training Flow

```
    设置完成 ──▶  READY ──▶  RUNNING ──▶  SCORING
                 准备 1s       播放电码       打分对比
                             用户打字输入

   状态栏显示:  方法  课次  WPM
   底部显示:    "Playing..." 或 "Done  Ent=finish"
```

### 评分界面 / Scoring Screen

```
   ┌──────────────────────────────────────────────┐
   │ Score: 18/20  90%               Esc=exit     │
   ├──────────────┬───────────────────────────────┤
   │  你的输入     │  正确答案                      │
   │  Your input  │  Correct answer               │
   │              │                               │
   │ ▐K▐M R E ▐S▐│  K M ▐U▐R E ▐A▐               │  ← 方框=错误
   │  N A P T L  │  N A P T L                    │     Box=error
   │  W I . J Z  │  W I . J Z                    │
   │  = F O Y ,  │  = F O Y ,                    │
   └──────────────┴───────────────────────────────┘
     ↑↓ 滚动查看更多 / Scroll for more
```

---

<!-- ================================================================== -->
<!--                 11. 键盘布局 / Keyboard Layout                      -->
<!-- ================================================================== -->

## 11. 键盘布局 / Keyboard Layout

菜单 → **Keyboard** 切换。按 Enter 在 QWERTY 和 AZERTY 之间轮换。

Menu → **Keyboard** to toggle. Press Enter to switch between QWERTY and AZERTY.

```
   QWERTY                   AZERTY
   ┌─────────────────┐    ┌─────────────────┐
   │ Q W E R T Y ... │    │ A Z E R T Y ... │
   │ A S D F G H ... │    │ Q S D F G H ... │
   │ Z X C V B N ... │    │ W X C V B N ... │
   └─────────────────┘    └─────────────────┘
```

用 AZERTY 键盘请选 AZERTY，否则键位不对。 / Choose AZERTY if using an AZERTY keyboard.

---

<!-- ================================================================== -->
<!--                 12. 版本页面 / Version Page                         -->
<!-- ================================================================== -->

## 12. 版本页面 / Version Page

菜单 → **Version** 查看。 / Menu → **Version** to view.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │            VERSION                   │
   │                                     │
   │             v1.1.9                  │  ← 当前固件版本 / Firmware version
   │                                     │
   │     BG6VSK Provided to              │  ← (定制版)
   │           BG7WG                     │     (Custom version recipient)
   │                                     │
   │        Press ENT Back               │
   │                                     │
   └─────────────────────────────────────┘

   通用版本只显示: VERSION + 版本号。
   Generic version only shows: VERSION + version number.
```

---

<!-- ================================================================== -->
<!--               13. 恢复出厂设置 / Factory Reset                       -->
<!-- ================================================================== -->

## 13. 恢复出厂设置 / Factory Reset

菜单 → **RESET** 进入。有二次确认，防止误操作。

Menu → **RESET** to enter. Confirmation required.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │              WARING                  │
   │                                     │
   │       ALL SET WILL BE RESET          │  ← 所有设置将被清除
   │                                     │     All settings will be cleared
   │       Press ENT For Sure            │  ← Enter = 确认重置 / Confirm
   │       Press ESC For Back            │  ← Esc   = 取消 / Cancel
   │                                     │
   └─────────────────────────────────────┘
```

重置后将恢复以下默认值 / After reset, these defaults are restored:

| 项目 | 默认值 |
|---|---|
| 模式 / Mode | DIR (即时) |
| 蜂鸣器/键控 / Beeper | OFF |
| 速度 / WPM | 20 |
| 单词间隔 / Word Split | Short (7×) |
| 数字短码 / Cut Num | OFF |
| KEY1 功能 | OPEN MENU |
| KEY2 功能 | MODE SW |
| 键盘布局 / Keyboard | QWERTY |
| 重复次数 / Repeat Count | 3 |
| 重复间隔 / Repeat Interval | 5s |
| 12 条预存消息 | 全部清空 / All cleared |

---

## 附录: 支持字符集 / Appendix: Supported Characters

共 54 个字符，覆盖标准国际摩尔斯电码 / 54 characters, full ITU Morse coverage.

```
  字母 / Letters:  A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
                    a b c d e f g h i j k l m n o p q r s t u v w x y z

  数字 / Digits:   0 1 2 3 4 5 6 7 8 9

  标点 / Punctuation:
   ?  !  .  ,  ;  :  +  -  /  =  '  "  &  @  $  _  (  )
   问  叹  句  逗  分  冒  加  减  除  等  单  双  并  at  元  下  左  右
   号  号  号  号  号  号  号  号  号  号  引  引  号  号  符  划  括  括
                                           号  号           号  线  号  号
```

---

<!-- ================================================================== -->
<!--                         封底 / BACK COVER                            -->
<!-- ================================================================== -->

<div style="text-align:center; page-break-before:always;">

<br/><br/><br/><br/>

```
    ██████╗ ███████╗    ██╗   ██╗ ██████╗ ██╗  ████████╗
    ██╔══██╗██╔════╝    ██║   ██║██╔═══██╗██║  ╚══██╔══╝
    ██║  ██║█████╗      ██║   ██║██║   ██║██║     ██║
    ██║  ██║██╔══╝      ╚██╗ ██╔╝██║   ██║██║     ██║
    ██████╔╝███████╗     ╚████╔╝ ╚██████╔╝███████╗██║
    ╚═════╝ ╚══════╝      ╚═══╝   ╚═════╝ ╚══════╝╚═╝

                 ███████╗██╗     ███████╗ ██████╗
                 ██╔════╝██║     ██╔════╝██╔════╝
                 █████╗  ██║     █████╗  ██║
                 ██╔══╝  ██║     ██╔══╝  ██║
                 ███████╗███████╗███████╗╚██████╗
                 ╚══════╝╚══════╝╚══════╝ ╚═════╝
```

<br/>

**CH32-CW-Keyboard v1.1.9**

基于 CH32V203 RISC-V 微控制器 / Based on CH32V203 RISC-V MCU

MIT 开源协议 / MIT Open Source License

> 📦 [项目主页 / Project README](./README.md)

<br/>

**Happy CW !  73 !**

> 📦 项目主页 / Project Home：[README.md](./README.md)

</div>
