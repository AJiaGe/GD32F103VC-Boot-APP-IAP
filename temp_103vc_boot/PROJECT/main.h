#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <math.h>
#include "gd32f10x.h"
#include "systick.h"
#include "io_bit.h"
#include <string.h>
#include "usart0.h"
#include "iic.h"
#include "led.h"
#include "exmc.h"
#include "lcd.h"
#include "xpt2046.h"
#include "fmc.h"
#include "tim.h"
#include "at24cxx.h"
#include "flash.h"
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#include "dma.h"
#include "usart0.h"




#define ADDRESS_BASE_FLASH 		(0x08000000U)
#define ADDRESS_APPLICATION		(0x08002000U)


void gpio_config(void);


#endif
