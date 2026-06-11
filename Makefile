# Makefile for RISC-V project
SHELL=cmd.exe
# Project output name
TARGET_BASE ?= CH32V203USBKEY
DEFAULT_KEYBOARD ?= QWERTY
VARIANT_SUFFIX = _$(DEFAULT_KEYBOARD)$(if $(filter 0,$(STORAGE_USE_EEPROM)),_FLASH)
TARGET_NAME = $(TARGET_BASE)$(VARIANT_SUFFIX)
TARGET_ELF := $(TARGET_NAME).elf
TARGET_BIN := $(TARGET_NAME).bin
TARGET_HEX := $(TARGET_NAME).hex
TARGET_LST := $(TARGET_NAME).lst

BUILD_DIR ?= build-make
STORAGE_USE_EEPROM ?= 1
OLED_TYPE ?= SH1106
LINKER_SCRIPT ?= Ld/Link.ld

# Toolchain settings
# Prefer the WCH toolchain because this project uses WCH-specific interrupt attributes.
TOOLCHAIN_PREFIX_CANDIDATES := riscv-wch-elf riscv32-wch-elf riscv-none-embed riscv-none-elf riscv64-unknown-elf
TOOLCHAIN_PREFIX ?= $(firstword $(foreach prefix,$(TOOLCHAIN_PREFIX_CANDIDATES),$(if $(strip $(shell where $(prefix)-gcc 2>NUL)),$(prefix),)))
ifeq ($(strip $(TOOLCHAIN_PREFIX)),)
TOOLCHAIN_PREFIX := riscv-none-embed
endif

CROSS_COMPILE ?= $(TOOLCHAIN_PREFIX)-
CC := $(CROSS_COMPILE)gcc
CXX := $(CROSS_COMPILE)g++
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump
SIZE := $(CROSS_COMPILE)size

SRC_DIRS := Core Debug Peripheral/src User User/USB_Host User/u8g2
INC_DIRS := Startup Debug Core User Peripheral/inc

C_SRCS := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.c))
CPP_SRCS := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/*.cpp))

EXCLUDED_C_SRCS := User/u8g2/u8x8_d_ssd1309.c \
	User/u8g2/u8x8_d_sh1106_72x40.c \
	User/u8g2/u8x8_d_sh1106_64x32.c \
	User/u8g2/u8x8_fonts.c

C_SRCS := $(filter-out $(EXCLUDED_C_SRCS),$(C_SRCS))

ifeq ($(strip $(STORAGE_USE_EEPROM)),0)
C_SRCS := $(filter-out User/i2c_eeprom.c User/train.c,$(C_SRCS))
LINKER_SCRIPT := Ld/Link_flash.ld
endif

# CH32V203C8T6 uses the D6 startup file and the linker script already maps 64K Flash / 20K RAM.
STARTUP_FILE ?= Startup/startup_ch32v20x_D6.S

ASM_SRCS := $(STARTUP_FILE)

OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SRCS)) \
	$(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CPP_SRCS)) \
	$(patsubst %.S,$(BUILD_DIR)/%.o,$(ASM_SRCS))
DEPS := $(OBJS:.o=.d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPP_DEFS ?= -DU8G2_USE_LARGE_FONTS -DCOMPARE_FOR_VERSION_WITH_EEPROM=$(STORAGE_USE_EEPROM) -DOLED_TYPE=OLED_$(OLED_TYPE) -DDEFAULT_KEYBOARD=KEYBOARD_LAYOUT_$(DEFAULT_KEYBOARD)

RISCV_ABI ?= ilp32
MCU_FLAGS ?= -march=rv32imacxw -mabi=$(RISCV_ABI) -msmall-data-limit=8 -msave-restore

COMMON_FLAGS ?= $(MCU_FLAGS) -Os -flto -fmerge-all-constants -fno-ident -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g
CFLAGS ?= $(COMMON_FLAGS) $(INC_FLAGS) $(CPP_DEFS) -std=gnu99 -MMD -MP
CXXFLAGS ?= $(COMMON_FLAGS) $(INC_FLAGS) $(CPP_DEFS) -MMD -MP
ASFLAGS ?= $(COMMON_FLAGS) $(INC_FLAGS) $(CPP_DEFS) -x assembler-with-cpp -MMD -MP
LDFLAGS ?= $(MCU_FLAGS) -Os -flto -T $(LINKER_SCRIPT) -nostartfiles -Wl,--gc-sections -Wl,--relax -Wl,-Map,$(BUILD_DIR)/$(TARGET_NAME).map --specs=nano.specs --specs=nosys.specs

.PHONY: all clean size gen_version

all: gen_version $(BUILD_DIR)/$(TARGET_ELF) $(BUILD_DIR)/$(TARGET_HEX) $(BUILD_DIR)/$(TARGET_BIN) $(BUILD_DIR)/$(TARGET_LST) size

# 自动生成 version.h
gen_version:
	powershell -ExecutionPolicy Bypass -File tools/gen_version.ps1 -ProjectRoot .

$(BUILD_DIR)/$(TARGET_ELF): $(OBJS)
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(BUILD_DIR)/$(TARGET_HEX): $(BUILD_DIR)/$(TARGET_ELF)
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/$(TARGET_BIN): $(BUILD_DIR)/$(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/$(TARGET_LST): $(BUILD_DIR)/$(TARGET_ELF)
	$(OBJDUMP) --all-headers --demangle --disassemble $< > $@

size: $(BUILD_DIR)/$(TARGET_ELF)
	$(SIZE) --format=berkeley $<

$(BUILD_DIR)/%.o: %.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.S
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(ASFLAGS) -c $< -o $@

clean:
	-cmd /c "if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)"

-include $(DEPS)