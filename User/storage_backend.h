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
#endif

void StorageBackend_ReadConfigRegion (uint8_t *buffer, uint16_t size);
void StorageBackend_WriteConfigRegion (const uint8_t *buffer, uint16_t size);
void StorageBackend_ReadMsgDirectory (uint8_t *buffer, uint16_t size);
void StorageBackend_WriteMsgDirectory (const uint8_t *buffer, uint16_t size);
void StorageBackend_ReadMsgSlot (uint8_t slot, uint8_t *buffer, uint16_t size);
void StorageBackend_WriteMsgSlot (uint8_t slot, const uint8_t *buffer,
                                  uint16_t size);

#endif