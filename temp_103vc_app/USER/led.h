#ifndef __LED_H
#define __LED_H

#include "gd32f10x.h"
#include "systick.h"
#include "io_bit.h"
#include <stdio.h>


#define LED2	PCout(0)	
#define LED3	PCout(2)	
#define LED4	PEout(0)	
#define LED5	PEout(1)	

void LED_Init( void );

void LED_Flash( void );

#endif
