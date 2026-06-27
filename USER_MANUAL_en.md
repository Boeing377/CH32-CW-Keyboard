<!--
  Print: Chrome → Print → Save as PDF → A4, minimal margins.
-->

<!-- ================================================================== -->
<!--                         COVER PAGE                                  -->
<!-- ================================================================== -->

<div style="text-align:center; page-break-after:always;">

<br/><br/><br/><br/>

# CH32-CW-Keyboard

## User Manual

<br/>

### A tiny device that turns a USB keyboard into a Morse code keyer

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

**Firmware Version: 1.1.9**

**Designed by DeVolt Elec**

<br/><br/><br/>

</div>

<!-- ================================================================== -->
<!--                            TOC                                     -->
<!-- ================================================================== -->

## Table of Contents

|   | Chapter |
|---|------|
| 1 | Quick Start |
| 2 | Main Screen |
| 3 | Two Working Modes |
| 4 | Button Operations |
| 5 | Main Menu |
| 6 | Morse Settings |
| 7 | Button Function Settings |
| 8 | Repeat Sending |
| 9 | Saved Messages |
| 10 | Training Mode |
| 11 | Keyboard Layout |
| 12 | Version Page |
| 13 | Factory Reset |
| Appendix | Supported Characters |

---

<!-- ================================================================== -->
<!--                      1. Quick Start                                -->
<!-- ================================================================== -->

## 1. Quick Start

### Hardware Interfaces

| Port | Type | Purpose |
|---|---|---|
| **USBC** | USB Type-C receptacle | Connect 5V power for supply or Li-ion battery charging |
| **USBA** | USB Type-A receptacle | Connect a standard USB keyboard (HID keyboard) |
| **3.5mm** | 3.5mm audio jack | Outputs straight-key keying signal, connect to radio CW KEY jack |

### What You Need

* One CH32-CW-Keyboard board (buzzer built-in)
* One standard USB keyboard (wired HID)
* 5V USB power or 3.7V Li-ion battery
* (Optional) 3.5mm male-to-male audio cable to connect device to radio KEY jack

### Three Steps

```
  [1] Plug keyboard        [2] Power on            [3] Type and send!
      │                      │                      │
      ▼                      ▼                      ▼
  ┌─────────┐          ┌───────────┐          ┌───────────┐
  │ USB HOST│─────────▶│ OLED on   │─────────▶│  Di Di Dah │
  │  port   │          │ Welcome   │          │  BEEP!    │
  └─────────┘          └───────────┘          └───────────┘
```

1. Plug a USB keyboard into the device's **USB Host (USBA)** port.

2. Power the device (USBC or battery). The OLED lights up with a welcome screen.

3. Type on the USB keyboard — the built-in buzzer sounds Morse code, while the 3.5mm jack outputs the keying signal!

> 💡 **Tip**: The keying signal is always output on the 3.5mm jack. Beeper ON/OFF only controls the built-in buzzer.
>
> 🔑 **Shortcuts**: `Ctrl+M` menu | `Ctrl+T` training | `Ctrl+R` repeat | `Esc` stop repeat

---

<!-- ================================================================== -->
<!--                      2. Main Screen                                -->
<!-- ================================================================== -->

## 2. Main Screen

The main screen after boot — this is where you'll spend most of your time.

```
     ┌──────────────────────────────────────────────────┐
     │ WPM:20  MsgLen:0          ♫  ▓▓▓▓▓▓▓▓▓  ▲  4.0V │  ← Status bar
     ├──────────────────────────────────────────────────┤
     │                                                  │
     │   H E L L O   W O R L D _ │                      │  ← Input area
     │                                                  │
     │                                                  │
     │                                                  │
     └──────────────────────────────────────────────────┘
```

| Status bar icon | Meaning |
|---|---|
| ♫ | Built-in buzzer ON |
| ✕♫ (muted) | Buzzer OFF (keying signal still active) |
| ▓▓▓ | Battery icon (EEPROM only) |
| ▲ | Caps Lock ON |
| 4.0V | Battery voltage (EEPROM only) |
| MsgLen:0 | Characters typed so far |

> **The 3.5mm jack always outputs a straight-key keying signal** (active low). Connect to radio CW KEY with a standard audio cable. The built-in buzzer provides local monitoring and can be toggled in the menu.

### Global Shortcuts

| Shortcut | Function |
|---|---|
| **Ctrl+M** | Open/close main menu |
| **Ctrl+T** | Open/close training menu (EEPROM only) |
| **Ctrl+R** | Start repeat sending |
| **Esc** | Stop repeat (when active) |

> These shortcuts work on the main screen and in menus — the fastest way to operate.

---

<!-- ================================================================== -->
<!--                   3. Two Working Modes                             -->
<!-- ================================================================== -->

## 3. Two Working Modes

The device has two sending modes. Press **Ctrl+M** → Mode to switch.

### Mode 0 — Immediate (DIR)

```
   Type and send instantly.

   ┌──────────────────────────────────┐
   │ Keyboard:   H → E → L → L → O    │
   │ Hear:       ....   .  .-..  ...  │
   │                                  │
   │ Screen: 4 lines × 14 chars       │
   │ Sending char ▐highlighted▐       │
   └──────────────────────────────────┘
```

> Best for: Live QSO, daily practice, type-as-you-go.

### Mode 1 — Buffer (BUF)

```
   Compose first, send with one key. Like texting.

   ┌──────────────┬──────────────┐
   │  Input/Edit  │  Sending     │  ← Split by vertical line
   │              │              │
   │  CQ CQ CQ   │  CQ CQ CQ    │
   │  DE BG7WG   │  DE BG7WG    │
   │  K_         │  K           │
   │              │              │
   └──────────────┴──────────────┘
    4 lines × 7 chars each side

   Edit freely, then press Enter to send everything at once.
```

> Best for: Contest macros, CQ calls, messages needing editing.

---

<!-- ================================================================== -->
<!--                   4. Button Operations                             -->
<!-- ================================================================== -->

## 4. Button Operations

### Global Shortcuts (Recommended)

The fastest way to operate — press key combos directly on the main screen:

| Shortcut | Function |
|---|---|
| **Ctrl+M** | Open/close main menu |
| **Ctrl+T** | Open/close training menu |
| **Ctrl+R** | Start repeat sending |
| **Esc** | Stop repeat (when active) |

### Hardware Buttons

```
        KEY1 (PB13)              KEY2 (PB14)
        ┌──────┐                 ┌──────┐
        │  BT1 │                 │  BT2 │
        └──────┘                 └──────┘

   Short press (<0.8s) → custom function    Short press (<0.8s) → custom function
   Long press (≥0.8s)  → force open menu    Long press (≥0.8s)  → (reserved)
```

> Default: KEY1 short = OPEN MENU, KEY2 short = MODE SW. Configurable in menu → Btn Func.

### USB Keyboard Shortcuts

| Key | Function |
|---|---|
| **Enter** | Send (Mode 0) / Start send (Mode 1) / Confirm |
| **Backspace** | Delete one character |
| **Esc** | Exit menu / Clear buffer / Stop repeat |
| **Caps Lock** | Toggle case (▲ icon) |
| **↑ ↓** | Navigate menu |
| **← →** | Adjust values in menu |
| **0–9** | Jump to menu item / Enter numbers |
| **F1–F12** | Load saved message 1–12 |
| **Shift+F1–F12** | Save current text to slot |
| **Ctrl+M** | Open/close main menu |
| **Ctrl+T** | Open/close training menu |
| **Ctrl+R** | Start repeat sending |
| **Esc** | Stop repeat (when active) |

---

<!-- ================================================================== -->
<!--                      5. Main Menu                                  -->
<!-- ================================================================== -->

## 5. Main Menu

Press **Ctrl+M** or long-press KEY1 to enter. ↑↓ to move, Enter to confirm, Esc to exit.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │   Mode                      BUF     │  ← Current value
   │  ▐beeper                    OFF ▐   │  ← Highlighted = selected
   │   Morse Cfg                         │
   │   Btn Func                          │
   │   Repeat                            │  Menu scrolls, max 5 visible
   │   Keyboard                  QWERTY  │
   │   Version                           │
   │   RESET                             │
   │   SAVE&EXIT                         │
   │                                     │
   └─────────────────────────────────────┘
```

| # | Menu item | Press Enter to... |
|---|---|---|
| **Mode** | Toggle DIR(Immediate) / BUF(Buffer) |
| **beeper** | Toggle built-in buzzer ON/OFF (does not affect 3.5mm keying output) |
| **Morse Cfg** | Enter Morse code settings submenu |
| **Btn Func** | Enter button function settings submenu |
| **Repeat** | Enter repeat sending settings submenu |
| **Keyboard** | Switch QWERTY / AZERTY |
| **Version** | Show version info |
| **RESET** | Factory reset (confirmation required) |
| **SAVE&EXIT** | Save settings and exit |

> ⚠️ Training mode is NOT in the main menu. Use **Ctrl+T** to open it.

---

<!-- ================================================================== -->
<!--                  6. Morse Settings                                 -->
<!-- ================================================================== -->

## 6. Morse Settings

Menu → **Morse Cfg** to enter.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │  ▐Word Split              Short  ▐  │  ← Word spacing
   │                                     │
   │   Cut Num                 OFF       │  ← Shortened numbers
   │                                     │
   │   BACK                               │  ← Back
   │                                     │
   └─────────────────────────────────────┘
```

### Word Split

Controls the pause length between words.

| Option | Multiple vs dot | Feel |
|---|---|---|
| **Short** | 7× | Fast, for QSO |
| **Mid** | 10× | Moderate |
| **Long** | 14× | Slow, for practice |

### Cut Num

Standard Morse digits are 5 elements each — long. Cut Num uses fewer elements per digit to speed up numbers.

| Scheme | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|---|---|---|---|---|---|---|---|---|---|---|
| **OFF**(Standard) | −−−−− | ·−−−− | ··−−− | ···−− | ····− | ····· | −···· | −−··· | −−−·· | −−−−· |
| **Mod A** | − | ·− | ··− | ···− | ····− | ····· | −···· | −−··· | −·· | −· |
| **Mod B** | − | ·− | ··− | ···− | ····− | · | −···· | −··· | −·· | −· |
| **Mod C** | − | ·− | ··− | ·−− | ···− | ··· | −··· | −−· | −·· | −· |

> Cut Num only affects digits 0-9. Letters and punctuation are unaffected.

---

<!-- ================================================================== -->
<!--               7. Button Function Settings                          -->
<!-- ================================================================== -->

## 7. Button Function Settings

Menu → **Btn Func** to enter.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │  ▐BT1 FUN              OPEN MENU ▐  │  ← KEY1 short-press function
   │                                     │
   │   BT2 FUN              MODE SW      │  ← KEY2 short-press function
   │                                     │
   │   BACK                               │
   │                                     │
   └─────────────────────────────────────┘
```

Each button can be assigned one of three functions:

| Function | Effect |
|---|---|
| **BEEPER SW** | Short press toggles built-in buzzer ON/OFF |
| **MODE SW** | Short press toggles DIR/BUF mode |
| **OPEN MENU** | Short press opens main menu |

> Note: Regardless of KEY1's short-press setting, **long-pressing KEY1 always opens the main menu**.

---

<!-- ================================================================== -->
<!--                  8. Repeat Sending                                 -->
<!-- ================================================================== -->

## 8. Repeat Sending

Have the device repeatedly send the same text, like a beacon.

Menu → **Repeat** to configure.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │   RPT COUNT                          │
   │      003_                           │  ← How many times (000=INF)
   │                                     │
   │   RPT INTER                         │
   │       5s                            │  ← Interval
   │                                     │
   │   BACK                               │
   │                                     │
   └─────────────────────────────────────┘
```

| Parameter | Default | Range | Description |
|---|---|---|---|
| RPT COUNT | 3 | 1–999; 0=infinite(INF) | How many times |
| RPT INTER | 5s | 1–999s | Interval between sends |

### Start & Stop

> **Press `Ctrl+R` to start repeat, press `Esc` to stop.**

```
  [1] Set count & interval   [2] Type text on main     [3] Ctrl+R to start loop

   ┌──────────┐            ┌──────────────┐          ┌──────────────┐
   │RPT COUNT 3│            │ CQ CQ CQ     │          │R1/3   5s     │  ← Status bar
   │RPT INTER 5│──────▶     │ DE BG7WG     │─────▶    │ CQ CQ CQ     │
   └──────────┘            │ K            │          │ DE BG7WG     │
                           └──────────────┘          │ K            │
                                                     └──────────────┘
  Press Esc to stop.
```

**How it works:**
- Type the text to repeat on the main screen.
- Press `Ctrl+R`: the current input is saved and sent repeatedly at the configured interval.
- The status bar shows `R1/3 5s` meaning "Round 1 of 3, countdown 5 seconds".
- Press `Esc` to stop.

---

<!-- ================================================================== -->
<!--                   9. Saved Messages                                -->
<!-- ================================================================== -->

## 9. Saved Messages

12 slots, each stores a message. Accessed directly via **F1–F12** keys — no menu needed.

### Operation

```
   Load:
     Press F1~F12 → message from that slot auto-loads and sends (Mode 0)
     Press F1~F12 → message from that slot auto-loads into editor (Mode 1)

   Save:
     Type text on main → press Shift+F1~Shift+F12 → save to that slot
```

```
   Slot mapping:
   ┌────────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┐
   │ Slot   │  1 │  2 │  3 │  4 │  5 │  6 │  7 │  8 │  9 │ 10 │ 11 │ 12 │
   ├────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
   │ Load   │ F1 │ F2 │ F3 │ F4 │ F5 │ F6 │ F7 │ F8 │ F9 │F10 │F11 │F12 │
   ├────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
   │ Save   │S-F1│S-F2│S-F3│S-F4│S-F5│S-F6│S-F7│S-F8│S-F9│S-F10│S-F11│S-F12│
   └────────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
   S-F1 = Shift+F1
```

| Version | Max per slot |
|---|---|
| EEPROM | ~2KB (2044 bytes) |
| Flash | 508 bytes |

> 💡 Suggested use:
> F1 = CQ call, F2 = Callsign, F3 = Signal report,
> F4 = Name/QTH, F5 = 73 closing …

---

<!-- ================================================================== -->
<!--                   10. Training Mode                                -->
<!-- ================================================================== -->

## 10. Training Mode

> ⚠️ Training mode is only available in the **EEPROM version**.

> 🔑 Press **Ctrl+T** to open the training menu. Training is separate from the main menu.

### Training Main Menu

Press **Ctrl+T** to enter.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │   METHOD                  KOCH      │  ← Training method
   │   LESSON                    1       │  ← Current lesson
   │  ▐SETTING                 ▐         │  ← Enter settings
   │   START                             │  ← Start training
   │   EXIT                              │  ← Exit
   │                                     │
   └─────────────────────────────────────┘
```

### Three Methods

| Method | Description |
|---|---|
| **KOCH** | Adds 1 new letter per lesson in frequency order. Lesson 1 = K, M. Standard ham radio training. |
| **SEQU** | Sequential A→B→C… then 0→1→2… after letters. |
| **FREE** | Free mode: all A–Z and 0–9 randomized. No lesson progression. |

### Koch Lesson Character Sequence

Characters ordered by radio usage frequency. Lesson N covers the first N+1 characters.

```
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
  13            . (period)                 ... + .
  14            J                          ... + J
  15            Z                          ... + Z
  16            =                          ... + =
  17            F                          ... + F
  18            O                          ... + O
  19            Y                          ... + Y
  20            , (comma)                  ... + ,
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
  32            ? (question mark)          ... + ?
  33            4                          ... + 4
  34            7                          ... + 7
  35            C                          ... + C
  36            1                          ... + 1
  37            D                          ... + D
  38            6                          ... + 6
  39            0                          ... + 0
  40            X                          K M U R E S N A P T L W I . J Z
                                            = F O Y , V G 5 / Q 9 2 H 3 8
                                            B ? 4 7 C 1 D 6 0 X  (40 total)
```

> 💡 **Tip**: Start from Lesson 1 and ensure >90% accuracy before moving to the next lesson.

### Sequential Lesson Character Sequence

In alphabetical order A→Z, then digits 0→9. Lesson N covers the first N+1 characters.

```
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
  35            9                          A–Z + 0–9 (36 total)
```

> Sequential has a maximum of 35 lessons (26 letters + 10 digits).

### Training Settings (Koch/Sequential)

```
   ┌─────────────────────────────────────┐
   │                                     │
   │  ▐CHAR/GRP                 5     ▐  │  ← Chars per group (0=RAND)
   │   WPM                      20      │  ← Training speed
   │   GAP                      1       │  ← Spaces between groups
   │   TIME                    2min     │  ← Duration 1-5 min
   │   BACK                             │
   │                                     │
   └─────────────────────────────────────┘

   Free mode only: WPM, TIME, BACK.
```

### Training Flow

```
    Settings done ──▶  READY ──▶  RUNNING ──▶  SCORING
                       1s ready     Morse plays     Compare & score
                                   User types input

   Status bar:  Method  Lesson  WPM
   Bottom:      "Playing..." or "Done  Ent=finish"
```

### Scoring Screen

```
   ┌──────────────────────────────────────────────┐
   │ Score: 18/20  90%               Esc=exit     │
   ├──────────────┬───────────────────────────────┤
   │  Your input  │  Correct answer               │
   │              │                               │
   │ ▐K▐M R E ▐S▐│  K M ▐U▐R E ▐A▐               │  ← Box=error
   │  N A P T L  │  N A P T L                    │
   │  W I . J Z  │  W I . J Z                    │
   │  = F O Y ,  │  = F O Y ,                    │
   └──────────────┴───────────────────────────────┘
     ↑↓ Scroll for more
```

---

<!-- ================================================================== -->
<!--                 11. Keyboard Layout                                -->
<!-- ================================================================== -->

## 11. Keyboard Layout

Menu → **Keyboard** to toggle. Press Enter to switch between QWERTY and AZERTY.

```
   QWERTY                   AZERTY
   ┌─────────────────┐    ┌─────────────────┐
   │ Q W E R T Y ... │    │ A Z E R T Y ... │
   │ A S D F G H ... │    │ Q S D F G H ... │
   │ Z X C V B N ... │    │ W X C V B N ... │
   └─────────────────┘    └─────────────────┘
```

Choose AZERTY if using an AZERTY keyboard.

---

<!-- ================================================================== -->
<!--                 12. Version Page                                   -->
<!-- ================================================================== -->

## 12. Version Page

Menu → **Version** to view.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │            VERSION                   │
   │                                     │
   │             v1.1.9                  │  ← Firmware version
   │                                     │
   │     BG6VSK Provided to              │  ← (Custom version)
   │           BG7WG                     │
   │                                     │
   │        Press ENT Back               │
   │                                     │
   └─────────────────────────────────────┘

   Generic version only shows: VERSION + version number.
```

---

<!-- ================================================================== -->
<!--               13. Factory Reset                                    -->
<!-- ================================================================== -->

## 13. Factory Reset

Menu → **RESET** to enter. Confirmation required.

```
   ┌─────────────────────────────────────┐
   │                                     │
   │              WARING                  │
   │                                     │
   │       ALL SET WILL BE RESET          │  ← All settings will be cleared
   │                                     │
   │       Press ENT For Sure            │  ← Enter = Confirm
   │       Press ESC For Back            │  ← Esc   = Cancel
   │                                     │
   └─────────────────────────────────────┘
```

After reset, these defaults are restored:

| Item | Default |
|---|---|
| Mode | DIR (Immediate) |
| Beeper | OFF |
| WPM | 20 |
| Word Split | Short (7×) |
| Cut Num | OFF |
| KEY1 function | OPEN MENU |
| KEY2 function | MODE SW |
| Keyboard | QWERTY |
| Repeat Count | 3 |
| Repeat Interval | 5s |
| 12 saved messages | All cleared |

---

## Appendix: Supported Characters

54 characters, full ITU Morse coverage.

```
  Letters:  A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
            a b c d e f g h i j k l m n o p q r s t u v w x y z

  Digits:   0 1 2 3 4 5 6 7 8 9

  Punctuation:
   ?  !  .  ,  ;  :  +  -  /  =  '  "  &  @  $  _  (  )
```

---

<!-- ================================================================== -->
<!--                         BACK COVER                                 -->
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

Based on CH32V203 RISC-V MCU

GPL v3 Open Source License

<br/>

**Happy CW !  73 !**

> 📦 Project Home: [README.md](./README.md)

</div>
