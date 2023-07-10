#ifndef __DMA_H
#define __DMA_H


#include "gd32f10x.h"

#define RX_BUFF_SIZE   			0x200
#define MAX_FLASH_SIZE 			0x2C00




extern uint32_t num_of_data_write;

extern uint8_t rxbuffer[RX_BUFF_SIZE];

extern uint32_t start_addr;

#define USART0_DATA_ADDR			(0x40013804U)




void DMA_Config(void);


#endif
