# CH32-CW-Keyboard

基于沁恒 CH32V203 RISC-V 微控制器的 CW（莫尔斯电码）自动键键盘。

A CW (Morse code) keyboard keyer based on the WCH CH32V203 RISC-V microcontroller.

> 📖 **用户手册 / User Manual**：[USER_MANUAL.md](./USER_MANUAL.md)

---

## 功能特性 / Features

| 功能 / Feature | 说明 / Description |
|---|---|
| USB 键盘输入 | 标准 USB HID 键盘 → 自动莫尔斯电码键控输出 |
| 双模式发送 | 即时发送 (DIR) / 缓存发送 (BUF) |
| 速度可调 | 1–99 WPM（PARIS 标准） |
| 数字短码 | 3 种短码方案可选 |
| 预存消息 | 12 条，F1–F12 读取，Shift+F1–F12 保存 |
| 重复发送 | 可设次数 1–999（0=无限）和间隔 1–999s |
| CW 训练 | Koch / Sequential / Free 三种方法，含自动评分 |
| OLED 显示 | 1.3" SH1106 或 2.42" SSD1306 (128×64 I²C) |
| 键控输出 | 3.5mm 音频接口，直键键控信号（低有效） |
| 内置蜂鸣器 | 本地监听，可开关 |
| 电池监测 | ADC 电压检测（仅 EEPROM 版） |
| 键盘布局 | QWERTY / AZERTY 可选 |
| 配置存储 | 外部 EEPROM 或片内 Flash |

## 硬件规格 / Hardware

| 项目 / Item | 规格 / Specification |
|---|---|
| **主控 / MCU** | WCH CH32V203C8T6 (RISC-V RV32IMAC, 144 MHz) |
| **闪存 / Flash** | 64 KB |
| **内存 / SRAM** | 20 KB |
| **显示 / Display** | 1.3" SH1106 或 2.42" SSD1306 OLED (128×64, I²C) |
| **存储 / Storage** | 外部 AT24Cxx EEPROM 或片内 Flash 模拟 |
| **接口 / Interfaces** | USB Host (键盘), 3.5mm 键控输出, 2×按键, 蜂鸣器, LED |

## 快速上手 / Quick Start

```
  [1] 插 USB 键盘      [2] 上电            [3] 打字 → 莫尔斯电码输出！
  Plug USB keyboard    Power on           Type → Morse code outputs!
```

## 编译 / Build

需要 RISC-V GNU 工具链 / Requires RISC-V GNU toolchain (`riscv-wch-elf-gcc` 或 `riscv-none-embed-gcc`)。

### Makefile

```bash
# 默认 / Default：EEPROM + QWERTY + SH1106 (1.3")
make -j1

# EEPROM + AZERTY + SSD1306 (2.42")
make -j1 DEFAULT_KEYBOARD=AZERTY OLED_TYPE=SSD1306

# Flash 存储版 / Flash storage version (无 EEPROM，无训练模式)
make -j1 STORAGE_USE_EEPROM=0 BUILD_DIR=build-make-flashprep
```

### CMake

```bash
cmake -B build-cmake -G Ninja -DOLED_TYPE=SH1106
ninja -C build-cmake
```

### 编译选项 / Build Options

| 选项 / Option | 可选值 / Values | 说明 / Description |
|---|---|---|
| `DEFAULT_KEYBOARD` | `QWERTY` (默认), `AZERTY` | 键盘布局 / Keyboard layout |
| `STORAGE_USE_EEPROM` | `1` (默认), `0` | EEPROM 或片内 Flash 存储 |
| `OLED_TYPE` | `SH1106` (默认), `SSD1306` | 1.3" 或 2.42" 屏幕 |
| `BUILD_DIR` | `build-make` (默认) | 编译输出目录 |

### 输出文件 / Output Files

```
CH32V203USBKEY_QWERTY.bin         EEPROM + QWERTY
CH32V203USBKEY_AZERTY.bin         EEPROM + AZERTY
CH32V203USBKEY_QWERTY_FLASH.bin   Flash + QWERTY
CH32V203USBKEY_AZERTY_FLASH.bin   Flash + AZERTY
```

### 版本区别 / Version Differences

| 特性 / Feature | EEPROM 版 | Flash 版 |
|---|---|---|
| 训练模式 / Training | ✅ | ❌ |
| 电池监测 / Battery monitor | ✅ | ❌ |
| 每条消息大小 / Msg per slot | ~2 KB | 508 B |
| 防克隆保护 / Anti-clone | ❌ | ✅ |
| 存储介质 / Storage medium | 外部 AT24Cxx | 片内 Flash |

## 烧录 / Flash

```powershell
# WCH-Link + OpenOCD
.\flash_openocd.ps1
```

也可用 WCH ISP Tool 通过 USART1 烧录 `.hex` 文件。

Or use WCH ISP Tool via USART1 to flash the `.hex` file.

> ⚠️ Flash 存储版固件从 `0x08005000` 起始。ISP 烧录不会擦除 EEPROM 模拟区和配置区。
> Flash storage firmware starts at `0x08005000`. ISP flashing preserves the emulated EEPROM and config areas.

## 项目结构 / Project Structure

```
├── Core/               # RISC-V 内核支持 / core support
├── Debug/              # USART 调试输出 / debug print
├── Ld/                 # 链接脚本 / linker scripts
├── Peripheral/         # CH32V20x 外设库 / peripheral library
├── Startup/            # 启动汇编 / startup assembly
├── User/
│   ├── main.c          # 主程序 / main program
│   ├── morse_send.c    # 莫尔斯编码与 TIM2 键控 / Morse encoding & keying
│   ├── screen_disp.c   # OLED 界面渲染 / UI rendering
│   ├── ButtonFunc.c    # 硬件按键处理 / button handling
│   ├── train.c         # 训练状态机与文本生成 / training state machine
│   ├── storage_backend.c # EEPROM / Flash 存储抽象层
│   ├── ch32_u8g2.c     # U8g2 OLED 驱动 (CH32V20x 适配)
│   ├── USB_Host/       # USB Host HID 键盘驱动
│   └── u8g2/           # U8g2 图形库
├── tools/              # 构建脚本 / build scripts
├── Makefile            # GNU Make 构建
├── CMakeLists.txt      # CMake 构建
└── USER_MANUAL.md      # 用户手册 (中英双语)
```

## 许可证 / License

[Apache License 2.0](./LICENSE)
