#ifndef __IIC_H
#define __IIC_H

#include <stdint.h>
#include "io_bit.h"

#define I2C0_SPEED              		400000
#define I2C0_SLAVE_ADDRESS7     		0xA0
#define I2C_PAGE_SIZE           		8


#define AT24C01							127
#define AT24C02							255
#define AT24C04							511
#define AT24C08							1023
#define AT24C16							2047
#define AT24C32							4095
#define AT24C64	    					8191
#define AT24C128						16383
#define AT24C256						32767  

//Mini STM32开发板使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE AT24C02

//PB6 to I2C0_SCL
//PB7 to I2C0_SDA
#define I2C_SCL       					PBout(6)     //SCL时钟引脚
#define I2C_SDA    	  					PBout(7)     //输出SDA	 
#define I2C_SDAIN     					PBin(7)      //输入SDA 


void I2C_Init(void);

void I2C_Start(void);

void I2C_Stop(void);

void I2C_ACK_NACK(uint8_t ack);

uint8_t I2C_Wait_ACK(void);

void I2C_Write_Byte(uint8_t data);

uint8_t I2C_Read_Byte(uint8_t ack);


#endif
