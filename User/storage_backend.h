#ifndef USER_STORAGE_BACKEND_H_
#define USER_STORAGE_BACKEND_H_

#include "global.h"

#ifndef COMPARE_FOR_VERSION_WITH_EEPROM
#define COMPARE_FOR_VERSION_WITH_EEPROM 1
#endif

#if !COMPARE_FOR_VERSION_WITH_EEPROM
#ifndef STORAGE_FLASH_IS_CONFIGURED
#define STORAGE_FLASH_IS_CONFIGURED 1
#endif

#ifndef STORAGE_FLASH_PAGE_SIZE
#define STORAGE_FLASH_PAGE_SIZE 0x0100UL
#endif

#ifndef STORAGE_FLASH_CONFIG_ADDR
#define STORAGE_FLASH_CONFIG_ADDR 0x0800E700UL
#endif

#ifndef STORAGE_FLASH_MSG_DIR_ADDR
#define STORAGE_FLASH_MSG_DIR_ADDR (STORAGE_FLASH_CONFIG_ADDR + 64UL)
#endif

#ifndef STORAGE_FLASH_MSG_BASE_ADDR
#define STORAGE_FLASH_MSG_BASE_ADDR 0x0800E800UL
#endif

#ifndef STORAGE_FLASH_MSG_SLOT_SIZE
#define STORAGE_FLASH_MSG_SLOT_SIZE 0x0200UL
#endif

#ifndef STORAGE_FLASH_MSG_COUNT
#define STORAGE_FLASH_MSG_COUNT MSG_NUM
#endif

/* ── Migration stamp (FLASH version only) ────────────────────────── */
/* Written once after old→new upgrade; survives ResetConfig().       */
/* Located at offset 44 in the config page (gap between Config       */
/* struct end ~32 and msg directory start at 64).                    */
/*                                                                   */
/* Layout:  4 B magic "MGRT" + 12 B chip UID = 16 B total            */
/*                                                                   */
/* If the magic matches but the UID does NOT match the chip, the     */
/* firmware was cloned from a different device → refuse to run.      */
/* ───────────────────────────────────────────────────────────────── */

#ifndef STORAGE_FLASH_MIGRATION_ADDR
#define STORAGE_FLASH_MIGRATION_ADDR   (STORAGE_FLASH_CONFIG_ADDR + 44UL)
#endif

#ifndef STORAGE_FLASH_MIGRATION_MAGIC
#define STORAGE_FLASH_MIGRATION_MAGIC   0x4D475254UL  /* "MGRT" */
#endif

#ifndef STORAGE_FLASH_MIGRATION_SIZE
#define STORAGE_FLASH_MIGRATION_SIZE    16UL  /* 4 magic + 12 UID */
#endif

/* CH32V203 96-bit unique device ID */
#ifndef MCU_UID_ADDR
#define MCU_UID_ADDR   0x1FFFF7E8UL
#endif
#endif

void StorageBackend_ReadConfigRegion (uint8_t *buffer, uint16_t size);
void StorageBackend_WriteConfigRegion (const uint8_t *buffer, uint16_t size);
void StorageBackend_ReadMsgDirectory (uint8_t *buffer, uint16_t size);
void StorageBackend_WriteMsgDirectory (const uint8_t *buffer, uint16_t size);
void StorageBackend_ReadMsgSlot (uint8_t slot, uint8_t *buffer, uint16_t size);
void StorageBackend_WriteMsgSlot (uint8_t slot, const uint8_t *buffer,
                                  uint16_t size);

#if !COMPARE_FOR_VERSION_WITH_EEPROM
uint8_t  StorageBackend_IsMigrationValid (void);
void     StorageBackend_WriteMigrationStamp (void);
void     StorageBackend_EraseOldConfigArea (void);
#endif

#endif