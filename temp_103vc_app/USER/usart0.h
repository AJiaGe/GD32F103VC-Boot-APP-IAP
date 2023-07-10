#ifndef __USART0_H
#define __USART0_H

#include "stdint.h"


#define UASRT0_DEFAULT_BAUD 				115200
#define UASRT0_PRINT						USART0

#define UASRT0_CLK                    		RCU_USART0
#define UASRT0_TX_PIN                 		GPIO_PIN_9
#define UASRT0_RX_PIN                 		GPIO_PIN_10
#define UASRT0_GPIO_PORT              		GPIOA
#define UASRT0_GPIO_CLK               		RCU_GPIOA
#define BUFFER_SIZE							256


extern uint8_t   rx_buffer[] ;



void USART0_Init( uint32_t baud );


#endif
