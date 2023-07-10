#include "led.h"


/*********************************************************************************************************
* 函 数 名 : 
* 功能说明 : 
* 形    参 : 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 

void LED_Init( void )
{
	/* enable the LEDs clock */
    rcu_periph_clock_enable( RCU_GPIOC );
    rcu_periph_clock_enable( RCU_GPIOE );
	
    /* configure LEDs GPIO port */
    gpio_init( GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_2 );
    gpio_init( GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 );
	
    /* reset LEDs GPIO pin */
	LED2 = 0;	
	LED3 = 0;	
	LED4 = 0;	
	LED5 = 0;	
}



