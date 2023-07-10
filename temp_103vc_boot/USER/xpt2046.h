#ifndef __XPT2046_H
#define __XPT2046_H



#include "gd32f10x.h"
#include "systick.h"
#include "io_bit.h"
#include <stdio.h>
#include "lcd.h"

typedef struct
{
    uint16_t xval;
    uint16_t yval;
} Touch_Typedef;


/* SPI SCK pin */
#define  SPI_SCK_PIN               		GPIO_PIN_5
#define  SPI_SCK_PORT              		GPIOA

/* SPI MOSI pin */
#define  SPI_MOSI_PIN              		GPIO_PIN_7
#define  SPI_MOSI_PORT             		GPIOA

/* SPI MISO pin */
#define  SPI_MISO_PIN              		GPIO_PIN_6
#define  SPI_MISO_PORT             		GPIOA

/* SPI Chip select pin */
#define  SPI_TOUCH_CS_PIN           	GPIO_PIN_4
#define  SPI_TOUCH_CS_PORT          	GPIOE

/* LCD touch interrupt request pin */
#define  TOUCH_PEN_INT_PIN          	GPIO_PIN_5
#define  TOUCH_PEN_INT_PORT         	GPIOE

#define XPT2046_MOSI					PAout(7)
#define XPT2046_MISO					PAin(6)
#define XPT2046_PEN						PEin(5)
#define XPT2046_CS						PEout(4)
#define XPT2046_SCK						PAout(5)

void Xpt2046_GPIO_Init( void );

uint8_t Xpt2046_Scan_Touch(Touch_Typedef *touch, uint8_t mode);

void Xpt2046_TouchAdjust(void);

#endif
