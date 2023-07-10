#include "gd32f10x.h"
#include "systick.h"





#ifdef SYSTICK_SELF

static unsigned char 	fac_us  = 0;	//us��ʱ������
static unsigned short 	fac_ms 	= 0;	//ms��ʱ������


/*********************************************************************************************************
* �� �� �� : Systick_Init
* ����˵�� : Systick ��ʼ��
* ��    �� : clk��ʱ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 

void Systick_Init(uint8_t clk)
{
    SysTick->CTRL &= ~(1 << 2);	//ѡ���ⲿʱ����Ϊ�δ��ʱ��Դ
    fac_us = clk / 8.0;			//�õ���1us��Ҫ�Ƶ���
    fac_ms = clk / 8.0 * 1000;	//�õ���1ms��Ҫ�Ƶ���
}


/*********************************************************************************************************
* �� �� �� : Delay_Us
* ����˵�� : us���ӳ�
* ��    �� : us����Ҫ�ӳٵ�us��
* �� �� ֵ : ��
* ��    ע : ����ӳ�ʱ�� = ��2^24-1��/ 10.5 = 798915(us)��ע�⣺������Ҫ���ж��е���
*********************************************************************************************************/

void Delay_Us(uint32_t us)
{
    uint32_t temp = 0;
    SysTick->LOAD = us * fac_us;				//��10.5������1us
    SysTick->VAL  = 0;							//�����ǰ����ֵ
    SysTick->CTRL |= (1 << 0);					//ʹ�ܼ�����

    do
    {
        //����Ҫ�ȶ����Ĵ�����ֵ���жϣ������������
        temp = SysTick->CTRL;				//�ȴ���16λΪ1
    }
    while(!(temp & (1 << 16)) && (temp & (1 << 0)));	//����ȷ����ʱ����������

    SysTick->CTRL &= ~(1 << 0);				//�رռ�����
}


/*********************************************************************************************************
* �� �� �� : Delay_Ms
* ����˵�� : ms���ӳ�
* ��    �� : ms��Ҫ�ӳٵ�ms��
* �� �� ֵ : ��
* ��    ע : ����ӳ� = ��2^24-1��/ 21 = 798(ms)
*********************************************************************************************************/

void Delay_Ms(uint32_t Ms)
{
    uint32_t temp = 0;
    SysTick->LOAD = Ms * fac_ms;					//��10.5������1us��1ms��1000us
    SysTick->VAL  = 0;							//�����ǰ����ֵ
    SysTick->CTRL |= (1 << 0);					//ʹ�ܼ�����

    do
    {
        //����Ҫ�ȶ����Ĵ�����ֵ���жϣ������������
        temp = SysTick->CTRL;					//�ȴ���16λΪ1
    }
    while(!(temp & (1 << 16)) && (temp & (1 << 0)));		//����ȷ����ʱ����������

    SysTick->CTRL &= ~(1 << 0);					//�رռ�����
}



#else

volatile static uint32_t delay;
volatile static float count_1us = 0;

void systick_config( void )
{
    /* setup systick timer for 1000Hz interrupts */
    if( SysTick_Config( SystemCoreClock / 1000U ) )
    {
        /* capture error */
        while( 1 )
        {
        }
    }

    /* configure the systick handler priority */
    NVIC_SetPriority( SysTick_IRQn, 0x00U );
}

/*!
    \brief      delay a time in milliseconds
    \param[in]  count: count in milliseconds
    \param[out] none
    \retval     none
*/
void delay_1ms( uint32_t count )
{
    delay = count;

    while( 0U != delay )
    {
    }
}

/*!
    \brief      delay decrement
    \param[in]  none
    \param[out] none
    \retval     none
*/
void delay_decrement( void )
{
    if( 0U != delay )
    {
        delay--;
    }
}


void SysTick_Handler( void )
{
    delay_decrement();
}

void delay_1us(uint32_t count)
{
    uint32_t ctl;

    /* reload the count value */
    SysTick->LOAD = (uint32_t)(count * count_1us);
    /* clear the current count value */
    SysTick->VAL = 0x0000U;
    /* enable the systick timer */
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;

    /* wait for the COUNTFLAG flag set */
    do
    {
        ctl = SysTick->CTRL;
    }
    while((ctl & SysTick_CTRL_ENABLE_Msk) && !(ctl & SysTick_CTRL_COUNTFLAG_Msk));

    /* disable the systick timer */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    /* clear the current count value */
    SysTick->VAL = 0x0000U;
}

#endif
