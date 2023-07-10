#include "main.h"

typedef  void (*pFunction)(void);

pFunction Jump_To_Application;

uint32_t jump_address = 0;


int main( void )
{
	gpio_config();
	
	if(SET == gpio_input_bit_get(GPIOA, GPIO_PIN_0))
    {
		if (((*(__IO uint32_t*)ADDRESS_APPLICATION) & 0x2FFE0000 ) == 0x20000000)    //ApplicationAddress为新程序的起始地址,检查栈顶地址是否合法,即栈顶地址是否为0x2000xxxx(内置SRAM)
		{
			usart_disable(USART0);
			dma_channel_disable(DMA0,DMA_CH4);
			jump_address = *(__IO uint32_t*) (ADDRESS_APPLICATION + 4);               //用户代码区第二个字存储为新程序起始地址（新程序复位向量指针）
			Jump_To_Application = (pFunction) jump_address;
			__set_MSP(*(__IO uint32_t*) ADDRESS_APPLICATION);                        //初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
			Jump_To_Application();                                                  // 设置PC指针为新程序复位中断函数的地址
		}
	}
	DMA_Config();                          
	USART0_Init( 115200 );

    while(1);
}


/*************************************************************************************************************
*  @Name	    : gpio_config 
*  @Brief    	: gpio config function
*  @Param[IN]  	: none
*  @Retval  	: none  
*  @Notes		: PA0-WKUP
************************************************************************************************************/

void gpio_config(void)
{
	//config wkup pin
    rcu_periph_clock_enable(RCU_GPIOA);
	gpio_init(GPIOA,GPIO_MODE_IPU, GPIO_OSPEED_10MHZ,GPIO_PIN_0);
}

