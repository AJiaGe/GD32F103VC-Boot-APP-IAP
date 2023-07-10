#include "tim.h"
#include "led.h"


/*********************************************************************************************************
* 函 数 名 : Tim6_Init
* 功能说明 : 
* 形    参 : 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 

void Tim6_Init( uint16_t pre,uint32_t per )
{
	timer_parameter_struct 		timer_initpara;
	
	rcu_periph_clock_enable(TIMER_LVGL_CLOCK);

    timer_deinit(TIMER_LVGL);

    /* TIM_BASE configuration */
    timer_initpara.prescaler         = pre - 1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = per - 1;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER_LVGL, &timer_initpara);
	
	nvic_priority_group_set( NVIC_PRIGROUP_PRE3_SUB1 );
	nvic_irq_enable(TIMER_LVGL_NVIC_IRQ, 2U, 0U);
	
	timer_interrupt_enable( TIMER_LVGL, TIMER_FLAG_UP);
	
    /* TIM_BASE enable */
    timer_enable(TIMER_LVGL);
}


/*********************************************************************************************************
* 函 数 名 : TIMER6_IRQHandler
* 功能说明 : 
* 形    参 : 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 

void TIMER6_IRQHandler( void )
{
	if( timer_interrupt_flag_get(TIMER_LVGL,TIMER_FLAG_UP) == SET )
    {
//		LED2 = !LED2;
//		gpio_bit_toggle( GPIOC, GPIO_PIN_0);
		timer_interrupt_flag_clear(TIMER_LVGL,TIMER_FLAG_UP);
    }
}

