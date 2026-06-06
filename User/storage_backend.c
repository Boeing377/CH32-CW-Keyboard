#include "storage_backend.h"

#include "string.h"

#if COMPARE_FOR_VERSION_WITH_EEPROM
#include "i2c_eeprom.h"

void StorageBackend_ReadConfigRegion (uint8_t *buffer, uint16_t size)
{
    AT24CXX_Read (EEPROM_CONFIG_ADDR, buffer, size);
}

void StorageBackend_WriteConfigRegion (const uint8_t *buffer, uint16_t size)
{
    AT24CXX_WriteMultiPage (EEPROM_CONFIG_ADDR, (uint8_t *)buffer, size);
}

void StorageBackend_ReadMsgDirectory (uint8_t *buffer, uint16_t size)
{
    AT24CXX_Read (EEPROM_CONFIG_ADDR + 64, buffer, size);
}

void StorageBackend_WriteMsgDirectory (const uint8_t *buffer, uint16_t size)
{
    AT24CXX_WriteMultiPage (EEPROM_CONFIG_ADDR + 64, (uint8_t *)buffer, size);
}

void StorageBackend_ReadMsgSlot (uint8_t slot, uint8_t *buffer, uint16_t size)
{
    uint16_t addr = (uint16_t)(slot * MSG_ZONE_SIZE);

    AT24CXX_Read (addr, buffer, size);
}

void StorageBackend_WriteMsgSlot (uint8_t slot, const uint8_t *buffer,
                                  uint16_t size)
{
    uint16_t addr = (uint16_t)(slot * MSG_ZONE_SIZE);

    AT24CXX_WriteMultiPage (addr, (uint8_t *)buffer, size);
}

#else

static uint8_t FlashStorage_IsValidSlot (uint8_t slot)
{
    return slot < STORAGE_FLASH_MSG_COUNT;
}

static uint32_t FlashStorage_GetSlotAddress (uint8_t slot)
{
    return STORAGE_FLASH_MSG_BASE_ADDR +
           ((uint32_t)slot * (uint32_t)STORAGE_FLASH_MSG_SLOT_SIZE);
}

#if STORAGE_FLASH_IS_CONFIGURED

static void FlashStorage_Read (uint32_t address, uint8_t *buffer, uint32_t size)
{
    memcpy (buffer, (const void *)address, size);
}

static FLASH_Status FlashStorage_WritePage (uint32_t page_addr,
                                            const uint8_t *page_data)
{
    FLASH_Unlock_Fast ();
    FLASH_ClearFlag (FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR);
    FLASH_ErasePage_Fast (page_addr);
    FLASH_ProgramPage_Fast (page_addr, (uint32_t *)page_data);
    FLASH_Lock_Fast ();

    return FLASH_COMPLETE;
}

static void FlashStorage_Write (uint32_t address, const uint8_t *data,
                                uint32_t size)
{
    uint32_t page_base;
    uint32_t page_offset;
    uint32_t chunk_size;
    FLASH_Status flash_status = FLASH_COMPLETE;
    static uint32_t page_shadow[STORAGE_FLASH_PAGE_SIZE / sizeof (uint32_t)];
    uint8_t *page_bytes = (uint8_t *)page_shadow;

    if ((data == 0) || (size == 0)) {
        return;
    }

    while (size > 0) {
        page_base = address & ~(STORAGE_FLASH_PAGE_SIZE - 1UL);
        page_offset = address - page_base;
        chunk_size = STORAGE_FLASH_PAGE_SIZE - page_offset;
        if (chunk_size > size) {
            chunk_size = size;
        }

        FlashStorage_Read (page_base, page_bytes, STORAGE_FLASH_PAGE_SIZE);
        memcpy (page_bytes + page_offset, data, chunk_size);

        flash_status = FlashStorage_WritePage (page_base, page_bytes);
        if (flash_status != FLASH_COMPLETE) {
            break;
        }

        address += chunk_size;
        data += chunk_size;
        size -= chunk_size;
    }
}

#else

static void FlashStorage_Read (uint32_t address, uint8_t *buffer, uint32_t size)
{
    (void)address;
    memset (buffer, 0xFF, size);
}

static void FlashStorage_Write (uint32_t address, const uint8_t *data,
                                uint32_t size)
{
    (void)address;
    (void)data;
    (void)size;
}

#endif

void StorageBackend_ReadConfigRegion (uint8_t *buffer, uint16_t size)
{
    FlashStorage_Read (STORAGE_FLASH_CONFIG_ADDR, buffer, size);
}

void StorageBackend_WriteConfigRegion (const uint8_t *buffer, uint16_t size)
{
    FlashStorage_Write (STORAGE_FLASH_CONFIG_ADDR, buffer, size);
}

void StorageBackend_ReadMsgDirectory (uint8_t *buffer, uint16_t size)
{
#if STORAGE_FLASH_IS_CONFIGURED
    FlashStorage_Read (STORAGE_FLASH_MSG_DIR_ADDR, buffer, size);
#else
    memset (buffer, 0, size);
#endif
}

void StorageBackend_WriteMsgDirectory (const uint8_t *buffer, uint16_t size)
{
    FlashStorage_Write (STORAGE_FLASH_MSG_DIR_ADDR, buffer, size);
}

void StorageBackend_ReadMsgSlot (uint8_t slot, uint8_t *buffer, uint16_t size)
{
    if (!FlashStorage_IsValidSlot (slot)) {
        memset (buffer, 0xFF, size);
        return;
    }

    FlashStorage_Read (FlashStorage_GetSlotAddress (slot), buffer, size);
}

void StorageBackend_WriteMsgSlot (uint8_t slot, const uint8_t *buffer,
                                  uint16_t size)
{
    if (!FlashStorage_IsValidSlot (slot)) {
        return;
    }

    FlashStorage_Write (FlashStorage_GetSlotAddress (slot), buffer, size);
}

#endif