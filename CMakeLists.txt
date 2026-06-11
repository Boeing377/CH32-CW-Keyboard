cmake_minimum_required(VERSION 3.20)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(PROJECT_NAME CH32V203USBKEY)

find_program(WCH_GCC riscv-wch-elf-gcc)
find_program(WCH32_GCC riscv32-wch-elf-gcc)
find_program(EMBED_GCC riscv-none-embed-gcc)
find_program(ELF_GCC riscv-none-elf-gcc)
find_program(UNKNOWN_ELF_GCC riscv64-unknown-elf-gcc)

set(DETECTED_GCC "")
if(WCH_GCC)
    set(DEFAULT_TOOLCHAIN_PREFIX riscv-wch-elf)
    set(DETECTED_GCC "${WCH_GCC}")
elseif(WCH32_GCC)
    set(DEFAULT_TOOLCHAIN_PREFIX riscv32-wch-elf)
    set(DETECTED_GCC "${WCH32_GCC}")
elseif(EMBED_GCC)
    set(DEFAULT_TOOLCHAIN_PREFIX riscv-none-embed)
    set(DETECTED_GCC "${EMBED_GCC}")
elseif(ELF_GCC)
    set(DEFAULT_TOOLCHAIN_PREFIX riscv-none-elf)
    set(DETECTED_GCC "${ELF_GCC}")
elseif(UNKNOWN_ELF_GCC)
    set(DEFAULT_TOOLCHAIN_PREFIX riscv64-unknown-elf)
    set(DETECTED_GCC "${UNKNOWN_ELF_GCC}")
else()
    set(DEFAULT_TOOLCHAIN_PREFIX riscv-none-elf)
endif()

set(TOOLCHAIN_PREFIX "${DEFAULT_TOOLCHAIN_PREFIX}" CACHE STRING "RISC-V GCC toolchain prefix")

if(DETECTED_GCC)
    set(CMAKE_C_COMPILER "${DETECTED_GCC}" CACHE FILEPATH "C compiler" FORCE)
    set(CMAKE_ASM_COMPILER "${DETECTED_GCC}" CACHE FILEPATH "ASM compiler" FORCE)
elseif(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}-gcc" CACHE STRING "C compiler")
    set(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}-gcc" CACHE STRING "ASM compiler")
endif()
if(NOT CMAKE_OBJCOPY)
    set(CMAKE_OBJCOPY "${TOOLCHAIN_PREFIX}-objcopy" CACHE FILEPATH "objcopy tool")
endif()
if(NOT CMAKE_SIZE)
    set(CMAKE_SIZE "${TOOLCHAIN_PREFIX}-size" CACHE FILEPATH "size tool")
endif()
if(NOT CMAKE_OBJDUMP)
    set(CMAKE_OBJDUMP "${TOOLCHAIN_PREFIX}-objdump" CACHE FILEPATH "objdump tool")
endif()

project(${PROJECT_NAME} LANGUAGES C ASM)

set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/Ld/Link.ld")
set(OPENOCD_EXE "C:/MounRiver/MounRiver_Studio2/resources/app/resources/win32/components/WCH/OpenOCD/OpenOCD/bin/openocd.exe")
set(OPENOCD_CFG "C:/MounRiver/MounRiver_Studio2/resources/app/resources/win32/components/WCH/OpenOCD/OpenOCD/bin/wch-riscv.cfg")

set(STORAGE_USE_EEPROM "1" CACHE STRING "Use EEPROM storage: 1=EEPROM, 0=FLASH")
set(OLED_TYPE "SH1106" CACHE STRING "OLED display type: SH1106 (1.3 inch) or SSD1306 (2.42 inch)")

set(MCU_FLAGS
    -march=rv32imacxw
    -mabi=ilp32
    -msmall-data-limit=8
    -msave-restore
)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Debug/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Peripheral/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/User/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/User/USB_Host/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/User/u8g2/*.c"
)

set(EXCLUDED_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/User/u8g2/u8x8_d_ssd1309.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/User/u8g2/u8x8_d_sh1106_72x40.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/User/u8g2/u8x8_d_sh1106_64x32.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/User/u8g2/u8x8_fonts.c"
)

list(REMOVE_ITEM APP_SOURCES ${EXCLUDED_SOURCES})

# 自动生成 version.h（每次构建时更新）
add_custom_target(gen_version ALL
    COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/tools/gen_version.ps1" -ProjectRoot "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Generating version.h..."
    BYPRODUCTS "${CMAKE_CURRENT_SOURCE_DIR}/User/version.h"
)

add_executable(${PROJECT_NAME}.elf
    ${APP_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/Startup/startup_ch32v20x_D6.S"
)

add_dependencies(${PROJECT_NAME}.elf gen_version)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Startup"
    "${CMAKE_CURRENT_SOURCE_DIR}/Core"
    "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
    "${CMAKE_CURRENT_SOURCE_DIR}/Peripheral/inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/User"
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    U8G2_USE_LARGE_FONTS
    COMPARE_FOR_VERSION_WITH_EEPROM=${STORAGE_USE_EEPROM}
    OLED_TYPE=OLED_${OLED_TYPE}
)

target_compile_features(${PROJECT_NAME}.elf PRIVATE c_std_99)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}
    -Os
    -flto
    -fmerge-all-constants
    -fno-ident
    -ffunction-sections
    -fdata-sections
    -fno-common
    -fmessage-length=0
    -fsigned-char
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    ${MCU_FLAGS}
    -flto
    "-T${LINKER_SCRIPT}"
    -nostartfiles
    --specs=nano.specs
    --specs=nosys.specs
    -Wl,--gc-sections
    -Wl,--relax
    -Wl,-Map=${PROJECT_NAME}.map
)

set_target_properties(${PROJECT_NAME}.elf PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    SUFFIX ".elf"
)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> "${PROJECT_NAME}.hex"
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> "${PROJECT_NAME}.bin"
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
    BYPRODUCTS
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin"
    COMMENT "Generating HEX/BIN and printing size"
)

add_custom_target(flash
    COMMAND "${OPENOCD_EXE}" -f "${OPENOCD_CFG}" -c "program $<TARGET_FILE:${PROJECT_NAME}.elf> verify reset exit"
    DEPENDS ${PROJECT_NAME}.elf
    USES_TERMINAL
    COMMENT "Flashing ${PROJECT_NAME}.elf with OpenOCD"
)
