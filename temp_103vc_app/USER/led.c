#include "led.h"


/*************************************************************************************************************
*  @Name	    : LED_Init 
*  @Brief    	: LED Init function
*  @Param[IN]  	: none
*  @Retval  	: none  
*  @Notes		: none
************************************************************************************************************/

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


/*************************************************************************************************************
*  @Name	    : LED_Flash 
*  @Brief    	: LED Flash function
*  @Param[IN]  	: none
*  @Retval  	: none  
*  @Notes		: none
************************************************************************************************************/

void LED_Flash( void )
{
	/* reset LEDs GPIO pin */
	LED2 = 0;	
	LED3 = 0;	
	LED4 = 0;	
	LED5 = 0;	
	
	Delay_Ms(1000);
	
	/* set LEDs GPIO pin */
	LED2 = 1;	
	LED3 = 1;	
	LED4 = 1;	
	LED5 = 1;	
	
	Delay_Ms(1000);
}

