#include "usart0.h"
#include "systick.h"
#include <stdio.h>
#include "dma.h"
#include "flash.h"

static uint8_t USART_RX[12];

uint8_t checksum;
uint8_t read_mem_num;
uint16_t num_pages;  		//flash需要擦除的页数
uint16_t page_num;  		//指定哪一页
uint8_t read_mem_data[12];
uint8_t command_data[11] = {0x00, 0x01, 0x02, 0x11, 0x21, 0x31, 0x43, 0x63, 0x73, 0x82, 0x92};



/*************************************************************************************************************
*  @Name	    : USART0_Init
*  @Brief    	: USART0 Init
*  @Param[IN]  	: baud:波特率
*  @Retval  	: none
*  @Notes		: none
************************************************************************************************************/

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
    usart_deinit( UASRT0_RECEIVE );

    usart_baudrate_set(UASRT0_RECEIVE, baud);

    usart_word_length_set(UASRT0_RECEIVE, USART_WL_8BIT);

    usart_stop_bit_set(UASRT0_RECEIVE, USART_STB_1BIT);

    usart_parity_config(UASRT0_RECEIVE, USART_PM_NONE);

    usart_hardware_flow_rts_config(UASRT0_RECEIVE, USART_RTS_DISABLE);

    usart_hardware_flow_cts_config(UASRT0_RECEIVE, USART_CTS_DISABLE);

    usart_receive_config(UASRT0_RECEIVE, USART_RECEIVE_ENABLE);

    usart_transmit_config(UASRT0_RECEIVE, USART_TRANSMIT_ENABLE);

    nvic_irq_enable(USART0_IRQn, 0, 0);

    /* enable USART0 receive interrupt */
    usart_interrupt_enable(UASRT0_RECEIVE, USART_INT_RBNE);

    usart_dma_receive_config(UASRT0_RECEIVE, USART_RECEIVE_DMA_DISABLE);

    usart_enable(UASRT0_RECEIVE);
}


/*************************************************************************************************************
*  @Name	    :
*  @Brief    	:
*  @Param[IN]  	: none
*  @Retval  	: none
*  @Notes		: none
************************************************************************************************************/

void usart_rx_clear(void)
{
    uint8_t i;

    for(i = 0; i < 12; i++)
    {
        USART_RX[i] = 0;
    }
}


/*************************************************************************************************************
*  @Name	    : USART0_IRQHandler
*  @Brief    	: USART0 IRQHandler function
*  @Param[IN]  	: none
*  @Retval  	: none
*  @Notes		: none
************************************************************************************************************/

void USART0_IRQHandler(void)
{
    if(RESET != usart_flag_get(USART0, USART_FLAG_RBNE))                                          //等待串口命令
    {
        static uint8_t i;
        uint8_t j;

        USART_RX[i++] = usart_data_receive(USART0);

        if(0x7F == USART_RX[0])
        {
            i = 0;
            usart_data_transmit(USART0, 0x79);

            while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
        }

        /* read memory */
        if((0x11 == USART_RX[0]) && (0xEE == USART_RX[1]))
        {
            if(2 == i)
            {
                usart_data_transmit(USART0, 0x79);

                while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
            }

            if(7 == i)
            {
                start_addr = ((uint32_t)USART_RX[2] << 24) | ((uint32_t)USART_RX[3] << 16) | (uint32_t)(USART_RX[4] << 8) | (uint32_t)USART_RX[5];
                checksum = USART_RX[6];

                if(SUCCESS == check_sum(&USART_RX[2], 4, checksum))
                {
                    usart_data_transmit(USART0, 0x79);

                    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
                }
            }

            if(9 == i)
            {
                i = 0;
                read_mem_num = USART_RX[7];
                checksum 	= USART_RX[8];

                if(SUCCESS == check_sum(&USART_RX[7], 1, checksum))
                {
                    usart_data_transmit(USART0, 0x79);

                    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
                }

                flash_read_memory(start_addr, read_mem_data, read_mem_num + 1);

                for(j = 0; j < read_mem_num + 1; j++)
                {
                    usart_data_transmit(USART0, read_mem_data[j]);

                    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
                }

                usart_rx_clear();
            }
        }

        /* Get the GD product ID 0x40022100*/
        if((0x06 == USART_RX[0]) && (0xF9 == USART_RX[1]))
        {
            usart_data_transmit(USART0, 0x79);

            while(RESET == usart_flag_get(USART0, USART_FLAG_TC));

            usart_data_transmit(USART0, 0x04);      //获取的数据字节数

            while(RESET == usart_flag_get(USART0, USART_FLAG_TC));

            flash_read_pid(read_mem_data);            //读FMC_PID寄存器，并放入数组

            for(j = 0; j < 4; j++)
            {
                usart_data_transmit(USART0, read_mem_data[j]);

                while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
            }

            usart_data_transmit(USART0, 0x79);

            while(RESET == usart_flag_get(USART0, USART_FLAG_TC));

            usart_rx_clear();
            i = 0;
        }

        /* get the version of the bootloader and the supported commands */
        if((0x00 == USART_RX[0]) && (0xFF == USART_RX[1]))
        {
            usart_data_transmit(USART0, 0x79);      //ACK

            while(RESET == usart_flag_get(USART0, USART_FLAG_TC));

            usart_data_transmit(USART0, 0x0B);      //数据长度（boot版本+支持的命令）

            while(RESET == usart_flag_get(USART0, USART_FLAG_TC));

            usart_data_transmit(USART0, 0x22);      //boot版本

            while(RESET == usart_flag_get(USART0, USART_FLAG_TC));

            /* 发送支持的命令 */
            for(j = 0; j < 11; j++)
            {
                usart_data_transmit(USART0, command_data[j]);

                while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
            }

            usart_data_transmit(USART0, 0x79);

            while(RESET == usart_flag_get(USART0, USART_FLAG_TC));

            usart_rx_clear();
            i = 0;
        }


        //擦除Flash，上位机会计算bin文件的大小，将需要擦除的页数发送到下位机
        if((0x43 == USART_RX[0]) && (0xBC == USART_RX[1]))
        {
            if(2 == i)
            {
                usart_data_transmit(USART0, 0x79);

                while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
            }

            if(5 == i)
            {
                num_pages 	= (uint16_t)(USART_RX[2] + 1);
                page_num 	= (uint16_t)USART_RX[3];
                checksum 	= USART_RX[4];

                if(SUCCESS == check_sum(&USART_RX[2], 2, checksum))
                {
                    flash_erase_page(num_pages, page_num);
                    usart_data_transmit(USART0, 0x79);

                    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
                }
                else
                {
                    usart_data_transmit(USART0, 0xFF);

                    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
                }

                usart_rx_clear();
                i = 0;
            }
        }

        //Extended Erase Memory command
        if((0x44 == USART_RX[0]) && (0XBB == USART_RX[1]))
        {
            if(2 == i)
            {
                usart_data_transmit(USART0, 0x79);

                while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
            }

            if(7 == i)
            {
                num_pages 	= (USART_RX[2] << 8) | (USART_RX[3]) + 1;
                page_num 	= (USART_RX[4] << 8) | (USART_RX[5]);
                checksum 	= USART_RX[6];

                if(SUCCESS == check_sum(&USART_RX[2], 4, checksum))
                {
                    flash_erase_page(num_pages, page_num);
                    usart_data_transmit(USART0, 0x79);

                    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
                }
                else
                {
                    usart_data_transmit(USART0, 0xFF);

                    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
                }

                usart_rx_clear();
                i = 0;
            }
        }

        //Write Memory命令
        if((0x31 == USART_RX[0]) && 0xCE == USART_RX[1])
        {
            if( 2 == i)
            {
                usart_data_transmit(USART0, 0x79);

                while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
            }

            //写入内存的地址
            if(7 == i)
            {
                start_addr = (USART_RX[2] << 24) | (USART_RX[3] << 16) | (USART_RX[4] << 8) | USART_RX[5];
                checksum = USART_RX[6];

                if(SUCCESS == check_sum(&USART_RX[2], 4, checksum))
                {
                    usart_data_transmit(USART0, 0x79);

                    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
                }
            }

            if(8 == i)
            {
                i = 0;
                num_of_data_write = (uint32_t)USART_RX[7] + 1;

//                dma_transfer_number_config(DMA_CH2, num_of_data_write + 1);
                dma_transfer_number_config(DMA0, DMA_CH4, (num_of_data_write + 1));
                usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);
                usart_interrupt_disable(USART0, USART_INT_RBNE);
//                dma_channel_enable(DMA_CH2);
                dma_channel_enable(DMA0, DMA_CH4);

                usart_rx_clear();
            }
        }

        if((0x21 == USART_RX[0]) && 0xDE == USART_RX[1])
        {
            if(2 == i)
            {
                while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));

                usart_data_transmit(USART0, 0x79);
            }

            if(7 == i)
            {
                i = 0;
                checksum = USART_RX[6];

                if(SUCCESS == check_sum(&USART_RX[2], 4, checksum))
                {
                    usart_data_transmit(USART0, 0x79);

                    while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
                }

                usart_rx_clear();
                NVIC_SystemReset();
            }
        }
    }
}


/*************************************************************************************************************
*  @Name	    : fputc
*  @Brief    	: rewrite fputc
*  @Param[IN]  	: none
*  @Retval  	: none
*  @Notes		: retarget the C library printf function to the USART
************************************************************************************************************/

//int fputc( int ch, FILE* f )
//{
//    usart_data_transmit( UASRT0_RECEIVE, ( uint8_t )ch );
//    while( RESET == usart_flag_get( UASRT0_RECEIVE, USART_FLAG_TBE ) );
//    return ch;
//}

