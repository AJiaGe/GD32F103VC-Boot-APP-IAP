#include "xpt2046.h"
#include <math.h>
#include "fmc.h"


float 	Kx, Ky;
short 	Bx, By;

/*********************************************************************************************************
* 函 数 名 : 
* 功能说明 : 
* 形    参 : 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 

void Xpt2046_GPIO_Init( void )
{	
	/* GPIO clock enable */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOE);

    /* MOSI(PA7) pin configure */
    gpio_init(SPI_MOSI_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI_MOSI_PIN);
	
    /* MISO(PA6) pins configure */
    gpio_init(SPI_MISO_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SPI_MISO_PIN);

    /* Chip select(SPI-Touch) PE4 pin configure */
    gpio_init(SPI_TOUCH_CS_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI_TOUCH_CS_PIN);

	/* SCK(PA5) pin configure */
    gpio_init(SPI_SCK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI_SCK_PIN);
	
    /* Touch pen IRQ pin (PE5) configure */
    gpio_init(TOUCH_PEN_INT_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, TOUCH_PEN_INT_PIN);

    /* Set chip select pin high */
	XPT2046_CS 	= 1;
	XPT2046_SCK = 1;
}


/*********************************************************************************************************
* 函 数 名 : Xpt2046_WriteByte
* 功能说明 : XPT2046写字节函数
* 形    参 : byte：需要写入的字节
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/
void Xpt2046_Write_Byte(uint8_t byte)
{
    uint8_t i;
    #if 1	//模式三

    for(i = 0; i < 8; i++)
    {
        XPT2046_SCK = 0;				//产生下沿，STM32写数据
        XPT2046_MOSI = !!(byte & 0x80);	//把当前的最高位放置到数据线上
        byte <<= 1;				//更新最高位
        Delay_Us(1);
        XPT2046_SCK = 1;				//产生上沿，XPT2046读数据，手册说明：数据在DCLK上升沿锁存进来
        Delay_Us(1);
    }

    #else	//模式零

    for(i = 0; i < 8; i++)
    {
        XPT2046_MOSI = !!(byte & 0x80);	//把当前的最高位放置到数据线上
        byte <<= 1;				//更新最高位
        Delay_Us(1);
        XPT2046_SCK = 1;				//产生上沿，XPT2046读数据，手册说明：数据在DCLK上升沿锁存进来
        Delay_Us(1);
        XPT2046_SCK = 0;				//产生下沿，STM32写数据
    }

    #endif
}


/*********************************************************************************************************
* 函 数 名 : Xpt2046_ReadByte
* 功能说明 : XPT2046读字节函数
* 形    参 : 无
* 返 回 值 : 读到的字节数据
* 备    注 : 无
*********************************************************************************************************/

uint8_t Xpt2046_Read_Byte(void)
{
    uint8_t i;
    uint8_t byte;
    #if 1	//模式三

    for(i = 0; i < 8; i++)
    {
        XPT2046_SCK = 0;				//产生下沿，XPT2046写数据，手册说明：数据在DCLK的下降沿移出
        Delay_Us(1);
        XPT2046_SCK = 1;				//产生上沿，STM读数据
        byte <<= 1;
        byte |= !!XPT2046_MISO;		//读取数据线的状态
        Delay_Us(1);
    }

    #else	//模式零

    for(i = 0; i < 8; i++)
    {
        Delay_Us(1);
        XPT2046_SCK = 1;				//产生上沿，STM读数据
        byte <<= 1;
        byte |= !!XPT2046_MISO;		//读取数据线的状态
        Delay_Us(1);
        XPT2046_SCK = 0;				//产生下沿，XPT2046写数据，手册说明：数据在DCLK的下降沿移出
    }

    #endif
    return byte;
}


/*********************************************************************************************************
* 函 数 名 : Xpt2046_ReadAD
* 功能说明 : 读取轴AD值
* 形    参 : cmd：选择工作模式
* 返 回 值 : 无
* 备    注 : cmd=0x90测量y轴AD值
			 cmd=0xd0测量x轴AD值
*********************************************************************************************************/

uint16_t Xpt2046_Read_AD(uint8_t cmd)
{
    uint8_t vh, vl;

    XPT2046_CS = 0;					//拉低片选，选中器件，开始通信
    Xpt2046_Write_Byte(cmd);		//发送测量命令
    Delay_Us(1);					//等到芯片忙结束
    vh = Xpt2046_Read_Byte();		//读取数据的高位
    vl = Xpt2046_Read_Byte();		//读取数据的低位
    XPT2046_CS = 1;					//拉低片选，取消选中，结束通信

    return ((vh << 8 | vl) >> 4);	//返回结果，其中只有12位有效
}


/*********************************************************************************************************
* 函 数 名 : Xpt2046_ReadXYAD
* 功能说明 : 读取X/Y轴AD值，并滤波处理
* 形    参 : touch：用于存放最终的x/y轴AD值
* 返 回 值 : 无
* 备    注 : adx[5]  ady[5]  分别存储采集到的值
						 sum1  sum2  分别存储X轴 Y轴的平均值
*********************************************************************************************************/

void Xpt2046_Read_XY_AD(Touch_Typedef *touch)
{
    uint8_t i, j;
    uint16_t adx[5], ady[5], temp, sum1 = 0, sum2 = 0;

    for(i = 0; i < 5; i++) //先采集多次数据
    {
        adx[i] = Xpt2046_Read_AD(0XD0);
        ady[i] = Xpt2046_Read_AD(0X90);
    }

    for(i = 0; i < 5; i++) //先排序，找出最大和最小的值
    {
        for(j = 0; j < 5 - i - 1; j++)
        {
            if(adx[j] > adx[j + 1]) //先排序，找出X轴的最大和最小的值
            {
                temp = adx[j];
                adx[j] = adx[j + 1];
                adx[j + 1] = temp;
            }

            if(ady[j] > ady[j + 1]) //先排序，找出Y轴的最大和最小的值
            {
                temp = ady[j];
                ady[j] = ady[j + 1];
                ady[j + 1] = temp;
            }
        }

        for(i = 1; i < 4; i++) //将剩余的X轴数据累加求和
        {
            sum1 += adx[i];
            sum2 += ady[i];
        }

        touch ->xval = sum1 / 3;	//返回X轴平均值
        touch ->yval = sum2 / 3;	//返回Y轴平均值
    }
}


/*********************************************************************************************************
* 函 数 名 : Xpt2046_ScanTouch
* 功能说明 : 扫描触摸屏
* 形    参 : touch：存放读取到的x/y坐标值
* 返 回 值 : 0：有按下；1：无按下
* 备    注 : 无
*********************************************************************************************************/

uint8_t Xpt2046_Scan_Touch(Touch_Typedef *touch, uint8_t mode)
{
    static uint8_t flag = 1;
    Touch_Typedef ad;

    if(mode)
    {
        flag = 1;
    }

    if(XPT2046_PEN == 0 && flag == 1) 				//判断触摸屏有没有被按下
    {
        flag = 0;

        if(XPT2046_PEN == 0)
        {
            Xpt2046_Read_XY_AD(&ad);
			printf("%d %d\r\n", ad.xval, ad.yval);
            touch->xval = ad.xval * Kx + Bx;
            touch->yval = ad.yval * Ky + By;
            return 0;
        }
    }
    else if(XPT2046_PEN == 1 && flag == 0)
    {
        flag = 1;
    }

    if(XPT2046_PEN == 1)//解屏幕锁死
    {
        touch->xval = 0xffff;
        touch->yval = 0xffff;
    }

    return 1;
}


/*********************************************************************************************************
* 函 数 名 : Xpt2046_TouchAdjust
* 功能说明 : 触摸屏校准函数
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/

void Xpt2046_TouchAdjust(void)
{
    uint8_t i, j;
    uint16_t lcd_pos[4][2] = {20, 20, 220, 20, 20, 300, 220, 300};
    Touch_Typedef touch_pos[4];//用来存放x,y的4个AD值
    double len1 = 0.00f, len2 = 0.00f;
//    Lcd_ShowString(30, 130, (uint8_t *)"touch adjust start", LCD_COLOR_BLACK, LCD_COLOR_WHITE, 16);

    while(1)
    {
        for(i = 0; i < 4; i++) //连续读取4个LCD坐标对应的触摸屏坐标
        {
            for(j = 0; j < 30; j++)								//画一个十字架
            {
                LCD_Point_Set(lcd_pos[i][0] - 15 + j, lcd_pos[i][1], LCD_COLOR_RED);
                LCD_Point_Set(lcd_pos[i][0], lcd_pos[i][1] - 15 + j, LCD_COLOR_RED);
            }

            printf("等待校验\r\n");

            while(XPT2046_PEN == 1);								//等待按下触摸屏

            Delay_Ms(50);											//延时50ms待数据稳定
            printf("按下触摸屏\r\n");
            Xpt2046_Read_XY_AD(&touch_pos[i]);	//获得触摸屏测量的x,y轴数值

            while(XPT2046_PEN == 0); 								//等待松开手

            Delay_Ms(200);

            for(j = 0; j < 30; j++)								//清掉十字架图标
            {
                LCD_Point_Set(lcd_pos[i][0] - 15 + j, lcd_pos[i][1], LCD_COLOR_WHITE);
                LCD_Point_Set(lcd_pos[i][0], lcd_pos[i][1] - 15 + j, LCD_COLOR_WHITE);
            }
        }

        //校验坐标-计算点击的触摸点是否正确  如果不正确重新校准
        //水平两个点之间的距离比较
        len1 = (float)sqrt((touch_pos[1].xval - touch_pos[0].xval) * (touch_pos[1].xval - touch_pos[0].xval) \
                           + (touch_pos[1].yval - touch_pos[0].yval) * (touch_pos[1].yval - touch_pos[0].yval));
        len2 = (float) sqrt((touch_pos[3].xval - touch_pos[2].xval) * (touch_pos[3].xval - touch_pos[2].xval) \
                            + (touch_pos[3].yval - touch_pos[2].yval) * (touch_pos[3].yval - touch_pos[2].yval));

        if(((len1 / len2) < 0.95) || ((len1 / len2) > 1.05))
        {
            continue; 	//点击的点不符合要求
        }

        //垂直两个点之间的距离比较
        len1 = (float)sqrt((touch_pos[2].xval - touch_pos[0].xval) * (touch_pos[2].xval - touch_pos[0].xval) \
                           + (touch_pos[2].yval - touch_pos[0].yval) * (touch_pos[2].yval - touch_pos[0].yval));
        len2 = (float)sqrt((touch_pos[3].xval - touch_pos[1].xval) * (touch_pos[3].xval - touch_pos[1].xval) \
                           + (touch_pos[3].yval - touch_pos[1].yval) * (touch_pos[3].yval - touch_pos[1].yval));

        if(((len1 / len2) < 0.95) || ((len1 / len2) > 1.05))
        {
            continue;	//点击的点不符合要求
        }

        //对角线两个点之间的距离比较
        len1 = (float)sqrt((touch_pos[3].xval - touch_pos[0].xval) * (touch_pos[3].xval - touch_pos[0].xval) \
                           + (touch_pos[3].yval - touch_pos[0].yval) * (touch_pos[3].yval - touch_pos[0].yval));
        len2 = (float)sqrt((touch_pos[2].xval - touch_pos[1].xval) * (touch_pos[2].xval - touch_pos[1].xval) \
                           + (touch_pos[2].yval - touch_pos[1].yval) * (touch_pos[2].yval - touch_pos[1].yval));

        if(((len1 / len2) < 0.95) || ((len1 / len2) > 1.05))
        {
            continue;	//点击的点不符合要求
        }

        //计算校准参数   Kx (Ky)--斜率；Bx(By) --偏移量
        //计算x映射 Xlcd = Kx * touch_x + Bx
        Kx = (float)(lcd_pos[1][0] - lcd_pos[0][0]) / (touch_pos[1].xval - touch_pos[0].xval);
        Bx = lcd_pos[0][0] - Kx * touch_pos[0].xval;
        //计算y映射 Ylcd = Ky*touch_y + By
        Ky = (float)(lcd_pos[2][1] - lcd_pos[0][1]) / (touch_pos[2].yval - touch_pos[0].yval);
        By = lcd_pos[0][1] - Ky * touch_pos[0].yval;
        LCD_Color_Fill(0, 0, 240, 320, LCD_COLOR_WHITE);
//        Lcd_ShowString(30, 130, (uint8_t *)"touch adjust OK", LCD_COLOR_RED, LCD_COLOR_WHITE, 16);


        printf("校准参数 Ky=%f;Kx=%f;By=%d;Bx=%d;\r\n", Ky, Kx, By, Bx);
        Delay_Ms(1000);
        LCD_Color_Fill(0, 0, 240, 320, LCD_COLOR_WHITE);
        break;
    }
}
