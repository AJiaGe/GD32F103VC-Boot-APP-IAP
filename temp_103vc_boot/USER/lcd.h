#ifndef __LCD_H
#define __LCD_H


#include "gd32f10x.h"
#include "systick.h"
#include "io_bit.h"
#include <stdio.h>
#include "exmc.h"

//A23
//LCD 数据和指令寄存器地址结构体类型
typedef struct
{
	unsigned short CMD;    //命令  0    
	unsigned short DAT;    //数据  1  
} LCD_TypeDef;
		    
#define LCD_BASE        			((unsigned int)(0x60000000 | 0x00FFFFFE))
#define LCD             			((LCD_TypeDef *) LCD_BASE)


#define BANK0_LCD_D         		((uint32_t)0x61000000)    /*!< LCD data address */
#define BANK0_LCD_C         		((uint32_t)0x60000000)    /*!< LCD register address */


#define LCD_PIXEL_WIDTH          	((uint16_t)320)
#define LCD_PIXEL_HEIGHT         	((uint16_t)240)


/* LCD color */
#define LCD_COLOR_WHITE     		0xFFFF
#define LCD_COLOR_BLACK     		0x0000
#define LCD_COLOR_GREY      		0xF7DE
#define LCD_COLOR_BLUE      		0x001F
#define LCD_COLOR_BLUE2     		0x051F
#define LCD_COLOR_RED       		0xF800
#define LCD_COLOR_MAGENTA   		0xF81F
#define LCD_COLOR_GREEN     		0x07E0
#define LCD_COLOR_CYAN      		0x7FFF
#define LCD_COLOR_YELLOW    		0xFFE0


#define LCD_COMMAND_X_SET			0x20
#define LCD_COMMAND_Y_SET			0x21
#define LCD_COMMAND_WRITE_DATE		0x22
#define LCD_COMMAND_X_START			0x50
#define LCD_COMMAND_X_END			0x51
#define LCD_COMMAND_Y_START			0x52
#define LCD_COMMAND_Y_END			0x53

void LCD_Point_Set(uint16_t x, uint16_t y, uint16_t color);

void LCD_Init( void );

void LCD_Clear(uint16_t color);

void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t c_color);

void LCD_Color_Fill(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color);

#endif
