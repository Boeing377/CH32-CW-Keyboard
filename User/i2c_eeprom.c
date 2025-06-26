#include "i2c_eeprom.h"

void init_eeprom(){

}

u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{
	u8 temp=0;

	while( I2C_GetFlagStatus( I2C2, I2C_FLAG_BUSY ) != RESET );
	I2C_GenerateSTART( I2C2, ENABLE );

	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) );
	I2C_Send7bitAddress( I2C2, 0XA0, I2C_Direction_Transmitter );

	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

#if (Address_Lenth  == Address_8bit)
	I2C_SendData( I2C2, (u8)(ReadAddr&0x00FF) );
	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

#elif (Address_Lenth  == Address_16bit)
	I2C_SendData( I2C2, (u8)(ReadAddr>>8) );
	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

	I2C_SendData( I2C2, (u8)(ReadAddr&0x00FF) );
	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

#endif

	I2C_GenerateSTART( I2C2, ENABLE );

	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) );
	I2C_Send7bitAddress( I2C2, 0XA0, I2C_Direction_Receiver );

	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED ) )
    ;
    while( I2C_GetFlagStatus( I2C2, I2C_FLAG_RXNE ) ==  RESET )
	I2C_AcknowledgeConfig( I2C2, DISABLE );

	temp = I2C_ReceiveData( I2C2 );
  I2C_GenerateSTOP( I2C2, ENABLE );

	return temp;
}

/*********************************************************************
 * @fn      AT24CXX_WriteOneByte
 *
 * @brief   Write one data to EEPROM.
 *
 * @param   WriteAddr - Write frist address.
 *
 * @return  DataToWrite - Write data.
 */
void AT24CXX_WriteOneByte(u16 WriteAddr, u8 DataToWrite)
{
	while( I2C_GetFlagStatus( I2C2, I2C_FLAG_BUSY ) != RESET );
	I2C_GenerateSTART( I2C2, ENABLE );

	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) );
	I2C_Send7bitAddress( I2C2, 0XA0, I2C_Direction_Transmitter );

	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

#if (Address_Lenth  == Address_8bit)
	I2C_SendData( I2C2, (u8)(WriteAddr&0x00FF) );
	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

#elif (Address_Lenth  == Address_16bit)
	I2C_SendData( I2C2, (u8)(WriteAddr>>8) );
	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

	I2C_SendData( I2C2, (u8)(WriteAddr&0x00FF) );
	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

#endif

	if( I2C_GetFlagStatus( I2C2, I2C_FLAG_TXE ) !=  RESET )
	{
		I2C_SendData( I2C2, DataToWrite );
	}

	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
	I2C_GenerateSTOP( I2C2, ENABLE );
}

/*********************************************************************
 * @fn      AT24CXX_Read
 *
 * @brief   Read multiple data from EEPROM.
 *
 * @param   ReadAddr - Read frist address. (AT24c02: 0~255)
 *          pBuffer - Read data.
 *          NumToRead - Data number.
 *
 * @return  none
 */
void AT24CXX_Read(u16 ReadAddr, u8 *pBuffer, u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);
		NumToRead--;
	}
}

/*********************************************************************
 * @fn      AT24CXX_Write
 *
 * @brief   Write multiple data to EEPROM.
 *
 * @param   WriteAddr - Write frist address. (AT24c02: 0~255)
 *          pBuffer - Write data.
 *          NumToWrite - Data number.
 *
 * @return  none
 */
void AT24CXX_Write(u16 WriteAddr, u8 *pBuffer, u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
		Delay_Ms(5);
	}
}

void AT24CXX_WriteOnePage(u16 WriteAddr, u8 *pBuffer, u16 NumToWrite)
{
	while( I2C_GetFlagStatus( I2C2, I2C_FLAG_BUSY ) != RESET );
	I2C_GenerateSTART( I2C2, ENABLE );

	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) );
	I2C_Send7bitAddress( I2C2, 0XA0, I2C_Direction_Transmitter );

	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ) );

#if (Address_Lenth  == Address_8bit)
	I2C_SendData( I2C2, (u8)(WriteAddr&0x00FF) );
	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

#elif (Address_Lenth  == Address_16bit)
	I2C_SendData( I2C2, (u8)(WriteAddr>>8) );
	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

	I2C_SendData( I2C2, (u8)(WriteAddr&0x00FF) );
	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );

#endif

	while(NumToWrite--)
	{
	if( I2C_GetFlagStatus( I2C2, I2C_FLAG_TXE ) !=  RESET )
	{
		I2C_SendData( I2C2, *pBuffer);
	}
		pBuffer++;
	while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
	}


	I2C_GenerateSTOP( I2C2, ENABLE );
}

void AT24CXX_WriteMultiPage(u16 WriteAddr, u8 *pBuffer, int32_t NumToWrite)
{
	while(NumToWrite > 0) 
	{
		if(NumToWrite >= PageSize)
			AT24CXX_WriteOnePage(WriteAddr, pBuffer, PageSize);
		else
			AT24CXX_WriteOnePage(WriteAddr, pBuffer, NumToWrite); ;
		WriteAddr += PageSize;
		pBuffer += PageSize;
		Delay_Ms(5);
		NumToWrite -= PageSize;
	}
}
