#include "dma.h"
#include "flash.h"

uint8_t rxbuffer[RX_BUFF_SIZE];
uint32_t num_of_data_write = RX_BUFF_SIZE;
uint32_t start_addr;

/*************************************************************************************************************
*  @Name	    : DMA_Config 
*  @Brief    	: DMA Config function
*  @Param[IN]  	: none
*  @Retval  	: none
*  @Notes		: none
************************************************************************************************************/

void DMA_Config(void)
{
    dma_parameter_struct dma_init_struct;
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);

    /* deinitialize DMA channel4 */
    dma_deinit(RCU_DMA0, DMA_CH4);
    dma_init_struct.direction 			= DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr 		= (uint32_t)&rxbuffer[1];
    dma_init_struct.memory_inc 			= DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width 		= DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number 				= (uint32_t)num_of_data_write;

    dma_init_struct.periph_addr 		= USART0_DATA_ADDR;
    dma_init_struct.periph_inc 			= DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width 		= DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority 			= DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH4, &dma_init_struct);

    dma_flag_clear( DMA0, DMA_CH4, DMA_FLAG_FTF );
    dma_interrupt_enable( DMA0, DMA_CH4, DMA_INT_FTF);

    /* Enable the DMA Interrupt */
    nvic_irq_enable(DMA0_Channel4_IRQn, 0, 0);
}


/*************************************************************************************************************
*  @Name	    : DMA0_Channel4_IRQHandler
*  @Brief    	: DMA0 Channel4 IRQHandler function
*  @Param[IN]  	: none
*  @Retval  	: none
*  @Notes		: none
************************************************************************************************************/

void DMA0_Channel4_IRQHandler(void)
{
    /* Wait until USARTy RX DMA1 Channel Transfer Complete */
    if(RESET != dma_flag_get(DMA0, DMA_CH4, DMA_FLAG_FTF))             //DMA发送完成
    {
        dma_flag_clear(DMA0, DMA_CH4, DMA_FLAG_FTF);                   //清除DMA中断标志
        dma_channel_disable(DMA0, DMA_CH4);
        rxbuffer[0] = num_of_data_write - 1;

        if(SUCCESS == check_sum(rxbuffer, num_of_data_write + 1, rxbuffer[num_of_data_write + 1]))
        {
            if(flash_program(start_addr, (uint32_t *)&rxbuffer[1],num_of_data_write ) != FAILED)
            {
                while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));

                usart_data_transmit(USART0, 0x79);

                //一包数据写入Flash完成，关闭串口DMA，打开串口中断等待下一包数据
                usart_dma_receive_config(USART0, USART_RECEIVE_DMA_DISABLE);
                usart_interrupt_enable(USART0, USART_INT_RBNE);
            }
        }
    }
}
