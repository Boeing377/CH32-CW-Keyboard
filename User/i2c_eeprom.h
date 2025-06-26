#ifndef __I2C_EEPROM_H__
#define __I2C_EEPROM_H__

#include "global.h"

#define Address_8bit  0
#define Address_16bit  1

#define PageSize_32byte 32
#define PageSize_64byte 64

#define Address_Lenth   Address_16bit
#define PageSize        PageSize_64byte

void init_eeprom();
void AT24CXX_Write(u16 WriteAddr, u8 *pBuffer, u16 NumToWrite);
void AT24CXX_Read(u16 ReadAddr, u8 *pBuffer, u16 NumToRead);
void AT24CXX_WriteMultiPage(u16 WriteAddr, u8 *pBuffer, int32_t NumToWrite);

#endif
