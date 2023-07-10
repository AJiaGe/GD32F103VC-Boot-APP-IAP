#include "usart0.h"
#include "gd32f10x.h"
#include "systick.h"
#include <stdio.h>

uint16_t rx_counter = 0;
uint8_t rx_buffer[BUFFER_SIZE];

/*********************************************************************************************************
* 函 数 名 : USART0_Init
* 功能说明 : USART0 初始化
* 形    参 : baud：波特率
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 

void USART0_Init( uint32_t baud )
{
    /* enable GPIO clock */
    rcu_periph_clock_enable( UASRT0_GPIO_CLK );

    /* enable USART clock */
    rcu_periph_clock_enable( UASRT0_CLK );

    /* connect port to USARTx_Tx */
    gpio_init(UASRT0_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, UASRT0_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_init(UASRT0_GPIO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, UASRT0_RX_PIN);

    /* USART configure */
    usart_deinit( UASRT0_PRINT );
	
    usart_baudrate_set(UASRT0_PRINT, baud);
	
    usart_word_length_set(UASRT0_PRINT, USART_WL_8BIT);
	
    usart_stop_bit_set(UASRT0_PRINT, USART_STB_1BIT);
	
    usart_parity_config(UASRT0_PRINT, USART_PM_NONE);
	
    usart_hardware_flow_rts_config(UASRT0_PRINT, USART_RTS_DISABLE);
	
    usart_hardware_flow_cts_config(UASRT0_PRINT, USART_CTS_DISABLE);
	
    usart_receive_config(UASRT0_PRINT, USART_RECEIVE_ENABLE);
	
    usart_transmit_config(UASRT0_PRINT, USART_TRANSMIT_ENABLE);
	
//	nvic_irq_enable(USART0_IRQn, 0, 0);
//	
//	 /* enable USART0 receive interrupt */
//	usart_interrupt_enable(UASRT0_PRINT, USART_INT_RBNE);
	 
    usart_enable(UASRT0_PRINT);
}


/*********************************************************************************************************
* 函 数 名 : 
* 功能说明 : 
* 形    参 : 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 

//void USART0_IRQHandler(void)
//{
//    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
//    {
//        /* read one byte from the receive data register */
//        rx_buffer[rx_counter++] = (uint8_t)usart_data_receive(USART0);
//		for( uint8_t i = 0; i < sizeof(rx_buffer); i++)                                  
//		{
//			printf("%x\r\n",rx_buffer[ i ]);
//		}
//    }
////	else
////	{
////		usart_interrupt_disable(USART0, USART_INT_RBNE);
////	}
//}

/*********************************************************************************************************
* 函 数 名 : fputc
* 功能说明 : 重写 fputc
* 形    参 : 
* 返 回 值 : 无
* 备    注 : retarget the C library printf function to the USART
*********************************************************************************************************/ 

int fputc( int ch, FILE* f )
{
    usart_data_transmit( UASRT0_PRINT, ( uint8_t )ch );
    while( RESET == usart_flag_get( UASRT0_PRINT, USART_FLAG_TBE ) );
    return ch;
}

