#ifndef __TIM_H
#define __TIM_H

#include "gd32f10x.h"
#include "systick.h"
#include "io_bit.h"
#include <stdio.h>


#define TIMER_LVGL						TIMER6		
#define TIMER_LVGL_CLOCK				RCU_TIMER6 	 	
#define TIMER_LVGL_NVIC_IRQ 			TIMER6_IRQn

void Tim6_Init( uint16_t pre,uint32_t per );



#endif
