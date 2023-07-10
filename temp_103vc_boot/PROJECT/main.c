#include "main.h"

typedef  void (*pFunction)(void);

pFunction Jump_To_Application;

uint32_t jump_address = 0;


int main( void )
{
	gpio_config();
	
	if(SET == gpio_input_bit_get(GPIOA, GPIO_PIN_0))
    {
		if (((*(__IO uint32_t*)ADDRESS_APPLICATION) & 0x2FFE0000 ) == 0x20000000)    //ApplicationAddressΪ�³������ʼ��ַ,���ջ����ַ�Ƿ�Ϸ�,��ջ����ַ�Ƿ�Ϊ0x2000xxxx(����SRAM)
		{
			usart_disable(USART0);
			dma_channel_disable(DMA0,DMA_CH4);
			jump_address = *(__IO uint32_t*) (ADDRESS_APPLICATION + 4);               //�û��������ڶ����ִ洢Ϊ�³�����ʼ��ַ���³���λ����ָ�룩
			Jump_To_Application = (pFunction) jump_address;
			__set_MSP(*(__IO uint32_t*) ADDRESS_APPLICATION);                        //��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
			Jump_To_Application();                                                  // ����PCָ��Ϊ�³���λ�жϺ����ĵ�ַ
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

