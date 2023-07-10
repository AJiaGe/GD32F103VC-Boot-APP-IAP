#include "gd32f10x.h"
#include "systick.h"





#ifdef SYSTICK_SELF

static unsigned char 	fac_us  = 0;	//us延时倍乘数
static unsigned short 	fac_ms 	= 0;	//ms延时倍乘数


/*********************************************************************************************************
* 函 数 名 : Systick_Init
* 功能说明 : Systick 初始化
* 形    参 : clk：时钟
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 

void Systick_Init(uint8_t clk)
{
    SysTick->CTRL &= ~(1 << 2);	//选择外部时钟作为滴答的时钟源
    fac_us = clk / 8.0;			//得到计1us需要计的数
    fac_ms = clk / 8.0 * 1000;	//得到计1ms需要计的数
}


/*********************************************************************************************************
* 函 数 名 : Delay_Us
* 功能说明 : us级延迟
* 形    参 : us：需要延迟的us数
* 返 回 值 : 无
* 备    注 : 最大延迟时间 = （2^24-1）/ 10.5 = 798915(us)，注意：尽量不要在中断中调用
*********************************************************************************************************/

void Delay_Us(uint32_t us)
{
    uint32_t temp = 0;
    SysTick->LOAD = us * fac_us;				//计10.5个数是1us
    SysTick->VAL  = 0;							//清除当前计数值
    SysTick->CTRL |= (1 << 0);					//使能计数器

    do
    {
        //必须要先读出寄存器的值再判断，否则会产生误差
        temp = SysTick->CTRL;				//等待第16位为1
    }
    while(!(temp & (1 << 16)) && (temp & (1 << 0)));	//并且确定定时器还在运行

    SysTick->CTRL &= ~(1 << 0);				//关闭计数器
}


/*********************************************************************************************************
* 函 数 名 : Delay_Ms
* 功能说明 : ms级延迟
* 形    参 : ms：要延迟的ms数
* 返 回 值 : 无
* 备    注 : 最大延迟 = （2^24-1）/ 21 = 798(ms)
*********************************************************************************************************/

void Delay_Ms(uint32_t Ms)
{
    uint32_t temp = 0;
    SysTick->LOAD = Ms * fac_ms;					//计10.5个数是1us，1ms有1000us
    SysTick->VAL  = 0;							//清除当前计数值
    SysTick->CTRL |= (1 << 0);					//使能计数器

    do
    {
        //必须要先读出寄存器的值再判断，否则会产生误差
        temp = SysTick->CTRL;					//等待第16位为1
    }
    while(!(temp & (1 << 16)) && (temp & (1 << 0)));		//并且确定定时器还在运行

    SysTick->CTRL &= ~(1 << 0);					//关闭计数器
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
