#include "main.h"




int main( void )
{
	nvic_vector_table_set(NVIC_VECTTAB_FLASH,0x2000);
	Systick_Init( 108 );
	LED_Init();
	USART0_Init(115200);
	
	printf("hello world\r\n");
	
    while( TRUE )
	{
		LED_Flash();
	}
}
