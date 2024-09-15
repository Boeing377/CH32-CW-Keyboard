/*
 * oled.c
 *
 *  Created on: 2024��8��20��
 *      Author: lyj98
 */

#include "oled.h"
#include "ch32v20x.h"

#define I2C_Speed           400000
#define I2C_Addr            0x00

#define SSD1306_I2C_ADDR    0x78
#define SSD1306_WIDTH       128
#define SSD1306_HIGH        64

//#define SSD1306_WRITECOMMAND(command)      ssd1306_I2C_Write(SSD1306_I2C_ADDR, 0x00, (command))
#define SSD1306_WRITECOMMAND(command)      I2C_WRITE_ONE_BYTE(0x00, (command))
#define SSD1306_WRITEDATA(data)            ssd1306_I2C_Write(SSD1306_I2C_ADDR, 0x40, (data))
#define ABS(x)   ((x) > 0 ? (x) : -(x))

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A
#define SSD1306_DEACTIVATE_SCROLL                    0x2E // Stop scroll
#define SSD1306_ACTIVATE_SCROLL                      0x2F // Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3 // Set scroll range

#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_INVERTDISPLAY       0xA7

/* SSD1306 data buffer */
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HIGH / 8];

/* Private SSD1306 structure */
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Inverted;
    uint8_t Initialized;
} SSD1306_t;

/* Private variable */
static SSD1306_t SSD1306;

void I2C_init() {
    /*PB0��SCL   PB11��SDA*/
    I2C_InitTypeDef I2C_initstruct;
    GPIO_InitTypeDef GPIO_initstruct;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_initstruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_initstruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_initstruct);

    I2C_initstruct.I2C_Ack = I2C_Ack_Enable;
    I2C_initstruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_initstruct.I2C_ClockSpeed = I2C_Speed;
    I2C_initstruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_initstruct.I2C_Mode = I2C_Mode_I2C;
    I2C_initstruct.I2C_OwnAddress1 = I2C_Addr;
    I2C_Init(I2C2, &I2C_initstruct);

    I2C_Cmd(I2C2, ENABLE);
    I2C_AcknowledgeConfig(I2C2, ENABLE);
}

void I2C_WRITE_ONE_BYTE(u16 writeaddress, u8 data_write) {
    while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)!=RESET);
    I2C_GenerateSTART(I2C2, ENABLE);
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C2, 0X78, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C2, (u8) writeaddress);
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C2, data_write);
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C2, ENABLE);
}

void i2c_transmit(uint8_t addr, uint8_t * data, uint32_t len) {
    int i;
    while( I2C_GetFlagStatus( I2C2, I2C_FLAG_BUSY ) != RESET );

    I2C_GenerateSTART( I2C2, ENABLE);

    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_MODE_SELECT ) );
    I2C_Send7bitAddress( I2C2, addr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    for (i = 0; i < len; i++) {
        I2C_SendData( I2C2, data[i]);
        while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }
//    while( !I2C_CheckEvent( I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED ) );
    I2C_GenerateSTOP( I2C2, ENABLE);
}

void ssd1306_I2C_Write(uint8_t address, uint8_t reg, uint8_t data) {
    uint8_t dt[2];
    dt[0] = reg;
    dt[1] = data;
    printf("i2c transmit addr %x, reg %x data %x\r\n", address, reg, data);
    i2c_transmit(address, dt, 2);
}

void ssd1306_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t* data,
        uint16_t count) {
    uint8_t dt[256];
    dt[0] = reg;
    uint8_t i;
    for (i = 0; i < count; i++)
        dt[i + 1] = data[i];
    i2c_transmit(address, dt, count + 1);
}

void SSD1306_UpdateScreen(void) {
    uint8_t m;
//    printf("UPdata screen\r\n");
    for (m = 0; m < 8; m++) {
//        printf("UPdata screen prepal %d\r\n", m);
        SSD1306_WRITECOMMAND(0xB0 + m);
        SSD1306_WRITECOMMAND(0x00);
        SSD1306_WRITECOMMAND(0x10);

//        printf("UPdata screen %d\r\n", m);
        /* Write multi data */
        ssd1306_I2C_WriteMulti(SSD1306_I2C_ADDR, 0x40,
                &SSD1306_Buffer[SSD1306_WIDTH * m], SSD1306_WIDTH);
    }
}

void SSD1306_ToggleInvert(void) {
    uint16_t i;

    /* Toggle invert */
    SSD1306.Inverted = !SSD1306.Inverted;

    /* Do memory toggle */
    for (i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = ~SSD1306_Buffer[i];
    }
}

void SSD1306_Fill(SSD1306_COLOR_t color) {
    /* Set memory */
    memset(SSD1306_Buffer, (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF,
            sizeof(SSD1306_Buffer));
}

void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color) {
    if (x >= SSD1306_WIDTH || y >= SSD1306_HIGH) {
        /* Error */
        return;
    }

    /* Check if pixels are inverted */
    if (SSD1306.Inverted) {
        color = (SSD1306_COLOR_t) !color;
    }

    /* Set color */
    if (color == SSD1306_COLOR_WHITE) {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } else {
        SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

void SSD1306_GotoXY(uint16_t x, uint16_t y) {
    /* Set write pointers */
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}

char SSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color) {
    uint32_t i, b, j;

    /* Check available space in LCD */
    if (
    SSD1306_WIDTH <= (SSD1306.CurrentX + Font->FontWidth) ||
    SSD1306_HIGH <= (SSD1306.CurrentY + Font->FontHeight)) {
        /* Error */
        return 0;
    }

    /* Go through font */
    for (i = 0; i < Font->FontHeight; i++) {
        b = Font->data[(ch - 32) * Font->FontHeight + i];
        for (j = 0; j < Font->FontWidth; j++) {
            if ((b << j) & 0x8000) {
                SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i),
                        (SSD1306_COLOR_t) color);
            } else {
                SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i),
                        (SSD1306_COLOR_t) !color);
            }
        }
    }

    /* Increase pointer */
    SSD1306.CurrentX += Font->FontWidth;

    /* Return character written */
    return ch;
}

char SSD1306_Puts(char* str, FontDef_t* Font, SSD1306_COLOR_t color) {
    /* Write characters */
    while (*str) {
        /* Write character by character */
        if (SSD1306_Putc(*str, Font, color) != *str) {
            /* Return error */
            return *str;
        }

        /* Increase string pointer */
        str++;
    }

    /* Everything OK, zero should be returned */
    return *str;
}

void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
        SSD1306_COLOR_t c) {
    int16_t dx, dy, sx, sy, err, e2, i, tmp;

    /* Check for overflow */
    if (x0 >= SSD1306_WIDTH) {
        x0 = SSD1306_WIDTH - 1;
    }
    if (x1 >= SSD1306_WIDTH) {
        x1 = SSD1306_WIDTH - 1;
    }
    if (y0 >= SSD1306_HIGH) {
        y0 = SSD1306_HIGH - 1;
    }
    if (y1 >= SSD1306_HIGH) {
        y1 = SSD1306_HIGH - 1;
    }

    dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
    dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
    sx = (x0 < x1) ? 1 : -1;
    sy = (y0 < y1) ? 1 : -1;
    err = ((dx > dy) ? dx : -dy) / 2;

    if (dx == 0) {
        if (y1 < y0) {
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }

        if (x1 < x0) {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
        }

        /* Vertical line */
        for (i = y0; i <= y1; i++) {
            SSD1306_DrawPixel(x0, i, c);
        }

        /* Return from function */
        return;
    }

    if (dy == 0) {
        if (y1 < y0) {
            tmp = y1;
            y1 = y0;
            y0 = tmp;
        }

        if (x1 < x0) {
            tmp = x1;
            x1 = x0;
            x0 = tmp;
        }

        /* Horizontal line */
        for (i = x0; i <= x1; i++) {
            SSD1306_DrawPixel(i, y0, c);
        }

        /* Return from function */
        return;
    }

    while (1) {
        SSD1306_DrawPixel(x0, y0, c);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }
}

void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
        SSD1306_COLOR_t c) {
    /* Check input parameters */
    if (x >= SSD1306_WIDTH || y >= SSD1306_HIGH) {
        /* Return error */
        return;
    }

    /* Check width and height */
    if ((x + w) >= SSD1306_WIDTH) {
        w = SSD1306_WIDTH - x;
    }
    if ((y + h) >= SSD1306_HIGH) {
        h = SSD1306_HIGH - y;
    }

    /* Draw 4 lines */
    SSD1306_DrawLine(x, y, x + w, y, c); /* Top line */
    SSD1306_DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
    SSD1306_DrawLine(x, y, x, y + h, c); /* Left line */
    SSD1306_DrawLine(x + w, y, x + w, y + h, c); /* Right line */
}

void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
        SSD1306_COLOR_t c) {
    uint8_t i;

    /* Check input parameters */
    if (x >= SSD1306_WIDTH || y >= SSD1306_HIGH) {
        /* Return error */
        return;
    }

    /* Check width and height */
    if ((x + w) >= SSD1306_WIDTH) {
        w = SSD1306_WIDTH - x;
    }
    if ((y + h) >= SSD1306_HIGH) {
        h = SSD1306_HIGH - y;
    }

    /* Draw lines */
    for (i = 0; i <= h; i++) {
        /* Draw lines */
        SSD1306_DrawLine(x, y + i, x + w, y + i, c);
    }
}

void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
        uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
    /* Draw lines */
    SSD1306_DrawLine(x1, y1, x2, y2, color);
    SSD1306_DrawLine(x2, y2, x3, y3, color);
    SSD1306_DrawLine(x3, y3, x1, y1, color);
}

void SSD1306_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2,
        uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 =
            0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
            curpixel = 0;

    deltax = ABS(x2 - x1);
    deltay = ABS(y2 - y1);
    x = x1;
    y = y1;

    if (x2 >= x1) {
        xinc1 = 1;
        xinc2 = 1;
    } else {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) {
        yinc1 = 1;
        yinc2 = 1;
    } else {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay) {
        xinc1 = 0;
        yinc2 = 0;
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;
    } else {
        xinc2 = 0;
        yinc1 = 0;
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++) {
        SSD1306_DrawLine(x, y, x3, y3, color);

        num += numadd;
        if (num >= den) {
            num -= den;
            x += xinc1;
            y += yinc1;
        }
        x += xinc2;
        y += yinc2;
    }
}

void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    SSD1306_DrawPixel(x0, y0 + r, c);
    SSD1306_DrawPixel(x0, y0 - r, c);
    SSD1306_DrawPixel(x0 + r, y0, c);
    SSD1306_DrawPixel(x0 - r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_DrawPixel(x0 + x, y0 + y, c);
        SSD1306_DrawPixel(x0 - x, y0 + y, c);
        SSD1306_DrawPixel(x0 + x, y0 - y, c);
        SSD1306_DrawPixel(x0 - x, y0 - y, c);

        SSD1306_DrawPixel(x0 + y, y0 + x, c);
        SSD1306_DrawPixel(x0 - y, y0 + x, c);
        SSD1306_DrawPixel(x0 + y, y0 - x, c);
        SSD1306_DrawPixel(x0 - y, y0 - x, c);
    }
}

void SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r,
        SSD1306_COLOR_t c) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    SSD1306_DrawPixel(x0, y0 + r, c);
    SSD1306_DrawPixel(x0, y0 - r, c);
    SSD1306_DrawPixel(x0 + r, y0, c);
    SSD1306_DrawPixel(x0 - r, y0, c);
    SSD1306_DrawLine(x0 - r, y0, x0 + r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
        SSD1306_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);

        SSD1306_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
        SSD1306_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
    }
}

void SSD1306_ScrollRight(uint8_t start_row, uint8_t end_row) {
    SSD1306_WRITECOMMAND(SSD1306_RIGHT_HORIZONTAL_SCROLL);  // send 0x26
    SSD1306_WRITECOMMAND(0x00);  // send dummy
    SSD1306_WRITECOMMAND(start_row);  // start page address
    SSD1306_WRITECOMMAND(0X00);  // time interval 5 frames
    SSD1306_WRITECOMMAND(end_row);  // end page address
    SSD1306_WRITECOMMAND(0X00);
    SSD1306_WRITECOMMAND(0XFF);
    SSD1306_WRITECOMMAND(SSD1306_ACTIVATE_SCROLL); // start scroll
}

void SSD1306_ScrollLeft(uint8_t start_row, uint8_t end_row) {
    SSD1306_WRITECOMMAND(SSD1306_LEFT_HORIZONTAL_SCROLL);  // send 0x26
    SSD1306_WRITECOMMAND(0x00);  // send dummy
    SSD1306_WRITECOMMAND(start_row);  // start page address
    SSD1306_WRITECOMMAND(0X00);  // time interval 5 frames
    SSD1306_WRITECOMMAND(end_row);  // end page address
    SSD1306_WRITECOMMAND(0X00);
    SSD1306_WRITECOMMAND(0XFF);
    SSD1306_WRITECOMMAND(SSD1306_ACTIVATE_SCROLL); // start scroll
}

void SSD1306_Scrolldiagright(uint8_t start_row, uint8_t end_row) {
    SSD1306_WRITECOMMAND(SSD1306_SET_VERTICAL_SCROLL_AREA);  // sect the area
    SSD1306_WRITECOMMAND(0x00);   // write dummy
    SSD1306_WRITECOMMAND(SSD1306_HIGH);

    SSD1306_WRITECOMMAND(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
    SSD1306_WRITECOMMAND(0x00);
    SSD1306_WRITECOMMAND(start_row);
    SSD1306_WRITECOMMAND(0X00);
    SSD1306_WRITECOMMAND(end_row);
    SSD1306_WRITECOMMAND(0x01);
    SSD1306_WRITECOMMAND(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_Scrolldiagleft(uint8_t start_row, uint8_t end_row) {
    SSD1306_WRITECOMMAND(SSD1306_SET_VERTICAL_SCROLL_AREA);  // sect the area
    SSD1306_WRITECOMMAND(0x00);   // write dummy
    SSD1306_WRITECOMMAND(SSD1306_HIGH);

    SSD1306_WRITECOMMAND(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
    SSD1306_WRITECOMMAND(0x00);
    SSD1306_WRITECOMMAND(start_row);
    SSD1306_WRITECOMMAND(0X00);
    SSD1306_WRITECOMMAND(end_row);
    SSD1306_WRITECOMMAND(0x01);
    SSD1306_WRITECOMMAND(SSD1306_ACTIVATE_SCROLL);
}

void SSD1306_Stopscroll(void) {
    SSD1306_WRITECOMMAND(SSD1306_DEACTIVATE_SCROLL);
}

void SSD1306_InvertDisplay(int i) {
    if (i)
        SSD1306_WRITECOMMAND(SSD1306_INVERTDISPLAY);

    else
        SSD1306_WRITECOMMAND(SSD1306_NORMALDISPLAY);

}

void SSD1306_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap,
        int16_t w, int16_t h, uint16_t color) {

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for (int16_t j = 0; j < h; j++, y++) {
        for (int16_t i = 0; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte =
                        (*(const unsigned char *) (&bitmap[j * byteWidth + i / 8]));
            }
            if (byte & 0x80)
                SSD1306_DrawPixel(x + i, y, color);
        }
    }
}

void SSD1306_Clear(void) {
    SSD1306_Fill(0);
    SSD1306_UpdateScreen();
}
void SSD1306_ON(void) {
    SSD1306_WRITECOMMAND(0x8D);
    SSD1306_WRITECOMMAND(0x14);
    SSD1306_WRITECOMMAND(0xAF);
}
void SSD1306_OFF(void) {
    SSD1306_WRITECOMMAND(0x8D);
    SSD1306_WRITECOMMAND(0x10);
    SSD1306_WRITECOMMAND(0xAE);
}

void SSD1306_Init() {
    /*Init the I2C interface*/
    I2C_init();

    Delay_Ms(100);

    SSD1306_WRITECOMMAND(0xAE); //display off
    SSD1306_WRITECOMMAND(0x20); //Set Memory Addressing Mode
    SSD1306_WRITECOMMAND(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    SSD1306_WRITECOMMAND(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
    SSD1306_WRITECOMMAND(0xC8); //Set COM Output Scan Direction
    SSD1306_WRITECOMMAND(0x00); //---set low column address
    SSD1306_WRITECOMMAND(0x10); //---set high column address
    SSD1306_WRITECOMMAND(0x40); //--set start line address
    SSD1306_WRITECOMMAND(0x81); //--set contrast control register
    SSD1306_WRITECOMMAND(0xFF);
    SSD1306_WRITECOMMAND(0xA1); //--set segment re-map 0 to 127
    SSD1306_WRITECOMMAND(0xA6); //--set normal display
    SSD1306_WRITECOMMAND(0xA8); //--set multiplex ratio(1 to 64) - CHECK
    SSD1306_WRITECOMMAND(0x3F); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    SSD1306_WRITECOMMAND(0xA4); //-set display offset
    SSD1306_WRITECOMMAND(0xD3); //-not offset
    SSD1306_WRITECOMMAND(0x00); //--set display clock divide ratio/oscillator frequency
    SSD1306_WRITECOMMAND(0xD5); //--set divide ratio
    SSD1306_WRITECOMMAND(0xF0); //--set pre-charge period
    SSD1306_WRITECOMMAND(0xD9); //
    SSD1306_WRITECOMMAND(0x22); //--set com pins hardware configuration
    SSD1306_WRITECOMMAND(0xDA); //--set vcomh
    SSD1306_WRITECOMMAND(0x12); //0x20,0.77xVcc
    SSD1306_WRITECOMMAND(0xDB); //--set DC-DC enable
    SSD1306_WRITECOMMAND(0x20); //
    SSD1306_WRITECOMMAND(0x8D); //--turn on SSD1306 panel
    SSD1306_WRITECOMMAND(0x14);
    SSD1306_WRITECOMMAND(0xAF);

//    printf("clear screen\r\n");
    /* Clear screen */
    SSD1306_Fill(SSD1306_COLOR_BLACK);

    /* Update screen */
    SSD1306_UpdateScreen();

    /* Set default values */
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;

//    printf("init OLED OK\r\n");
    /* Initialized OK */
    SSD1306.Initialized = 1;
}

