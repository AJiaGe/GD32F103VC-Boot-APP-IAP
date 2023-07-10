#include "xpt2046.h"
#include <math.h>
#include "fmc.h"


float 	Kx, Ky;
short 	Bx, By;

/*********************************************************************************************************
* �� �� �� : 
* ����˵�� : 
* ��    �� : 
* �� �� ֵ : ��
* ��    ע : ��
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
* �� �� �� : Xpt2046_WriteByte
* ����˵�� : XPT2046д�ֽں���
* ��    �� : byte����Ҫд����ֽ�
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/
void Xpt2046_Write_Byte(uint8_t byte)
{
    uint8_t i;
    #if 1	//ģʽ��

    for(i = 0; i < 8; i++)
    {
        XPT2046_SCK = 0;				//�������أ�STM32д����
        XPT2046_MOSI = !!(byte & 0x80);	//�ѵ�ǰ�����λ���õ���������
        byte <<= 1;				//�������λ
        Delay_Us(1);
        XPT2046_SCK = 1;				//�������أ�XPT2046�����ݣ��ֲ�˵����������DCLK�������������
        Delay_Us(1);
    }

    #else	//ģʽ��

    for(i = 0; i < 8; i++)
    {
        XPT2046_MOSI = !!(byte & 0x80);	//�ѵ�ǰ�����λ���õ���������
        byte <<= 1;				//�������λ
        Delay_Us(1);
        XPT2046_SCK = 1;				//�������أ�XPT2046�����ݣ��ֲ�˵����������DCLK�������������
        Delay_Us(1);
        XPT2046_SCK = 0;				//�������أ�STM32д����
    }

    #endif
}


/*********************************************************************************************************
* �� �� �� : Xpt2046_ReadByte
* ����˵�� : XPT2046���ֽں���
* ��    �� : ��
* �� �� ֵ : �������ֽ�����
* ��    ע : ��
*********************************************************************************************************/

uint8_t Xpt2046_Read_Byte(void)
{
    uint8_t i;
    uint8_t byte;
    #if 1	//ģʽ��

    for(i = 0; i < 8; i++)
    {
        XPT2046_SCK = 0;				//�������أ�XPT2046д���ݣ��ֲ�˵����������DCLK���½����Ƴ�
        Delay_Us(1);
        XPT2046_SCK = 1;				//�������أ�STM������
        byte <<= 1;
        byte |= !!XPT2046_MISO;		//��ȡ�����ߵ�״̬
        Delay_Us(1);
    }

    #else	//ģʽ��

    for(i = 0; i < 8; i++)
    {
        Delay_Us(1);
        XPT2046_SCK = 1;				//�������أ�STM������
        byte <<= 1;
        byte |= !!XPT2046_MISO;		//��ȡ�����ߵ�״̬
        Delay_Us(1);
        XPT2046_SCK = 0;				//�������أ�XPT2046д���ݣ��ֲ�˵����������DCLK���½����Ƴ�
    }

    #endif
    return byte;
}


/*********************************************************************************************************
* �� �� �� : Xpt2046_ReadAD
* ����˵�� : ��ȡ��ADֵ
* ��    �� : cmd��ѡ����ģʽ
* �� �� ֵ : ��
* ��    ע : cmd=0x90����y��ADֵ
			 cmd=0xd0����x��ADֵ
*********************************************************************************************************/

uint16_t Xpt2046_Read_AD(uint8_t cmd)
{
    uint8_t vh, vl;

    XPT2046_CS = 0;					//����Ƭѡ��ѡ����������ʼͨ��
    Xpt2046_Write_Byte(cmd);		//���Ͳ�������
    Delay_Us(1);					//�ȵ�оƬæ����
    vh = Xpt2046_Read_Byte();		//��ȡ���ݵĸ�λ
    vl = Xpt2046_Read_Byte();		//��ȡ���ݵĵ�λ
    XPT2046_CS = 1;					//����Ƭѡ��ȡ��ѡ�У�����ͨ��

    return ((vh << 8 | vl) >> 4);	//���ؽ��������ֻ��12λ��Ч
}


/*********************************************************************************************************
* �� �� �� : Xpt2046_ReadXYAD
* ����˵�� : ��ȡX/Y��ADֵ�����˲�����
* ��    �� : touch�����ڴ�����յ�x/y��ADֵ
* �� �� ֵ : ��
* ��    ע : adx[5]  ady[5]  �ֱ�洢�ɼ�����ֵ
						 sum1  sum2  �ֱ�洢X�� Y���ƽ��ֵ
*********************************************************************************************************/

void Xpt2046_Read_XY_AD(Touch_Typedef *touch)
{
    uint8_t i, j;
    uint16_t adx[5], ady[5], temp, sum1 = 0, sum2 = 0;

    for(i = 0; i < 5; i++) //�Ȳɼ��������
    {
        adx[i] = Xpt2046_Read_AD(0XD0);
        ady[i] = Xpt2046_Read_AD(0X90);
    }

    for(i = 0; i < 5; i++) //�������ҳ�������С��ֵ
    {
        for(j = 0; j < 5 - i - 1; j++)
        {
            if(adx[j] > adx[j + 1]) //�������ҳ�X���������С��ֵ
            {
                temp = adx[j];
                adx[j] = adx[j + 1];
                adx[j + 1] = temp;
            }

            if(ady[j] > ady[j + 1]) //�������ҳ�Y���������С��ֵ
            {
                temp = ady[j];
                ady[j] = ady[j + 1];
                ady[j + 1] = temp;
            }
        }

        for(i = 1; i < 4; i++) //��ʣ���X�������ۼ����
        {
            sum1 += adx[i];
            sum2 += ady[i];
        }

        touch ->xval = sum1 / 3;	//����X��ƽ��ֵ
        touch ->yval = sum2 / 3;	//����Y��ƽ��ֵ
    }
}


/*********************************************************************************************************
* �� �� �� : Xpt2046_ScanTouch
* ����˵�� : ɨ�败����
* ��    �� : touch����Ŷ�ȡ����x/y����ֵ
* �� �� ֵ : 0���а��£�1���ް���
* ��    ע : ��
*********************************************************************************************************/

uint8_t Xpt2046_Scan_Touch(Touch_Typedef *touch, uint8_t mode)
{
    static uint8_t flag = 1;
    Touch_Typedef ad;

    if(mode)
    {
        flag = 1;
    }

    if(XPT2046_PEN == 0 && flag == 1) 				//�жϴ�������û�б�����
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

    if(XPT2046_PEN == 1)//����Ļ����
    {
        touch->xval = 0xffff;
        touch->yval = 0xffff;
    }

    return 1;
}


/*********************************************************************************************************
* �� �� �� : Xpt2046_TouchAdjust
* ����˵�� : ������У׼����
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/

void Xpt2046_TouchAdjust(void)
{
    uint8_t i, j;
    uint16_t lcd_pos[4][2] = {20, 20, 220, 20, 20, 300, 220, 300};
    Touch_Typedef touch_pos[4];//�������x,y��4��ADֵ
    double len1 = 0.00f, len2 = 0.00f;
//    Lcd_ShowString(30, 130, (uint8_t *)"touch adjust start", LCD_COLOR_BLACK, LCD_COLOR_WHITE, 16);

    while(1)
    {
        for(i = 0; i < 4; i++) //������ȡ4��LCD�����Ӧ�Ĵ���������
        {
            for(j = 0; j < 30; j++)								//��һ��ʮ�ּ�
            {
                LCD_Point_Set(lcd_pos[i][0] - 15 + j, lcd_pos[i][1], LCD_COLOR_RED);
                LCD_Point_Set(lcd_pos[i][0], lcd_pos[i][1] - 15 + j, LCD_COLOR_RED);
            }

            printf("�ȴ�У��\r\n");

            while(XPT2046_PEN == 1);								//�ȴ����´�����

            Delay_Ms(50);											//��ʱ50ms�������ȶ�
            printf("���´�����\r\n");
            Xpt2046_Read_XY_AD(&touch_pos[i]);	//��ô�����������x,y����ֵ

            while(XPT2046_PEN == 0); 								//�ȴ��ɿ���

            Delay_Ms(200);

            for(j = 0; j < 30; j++)								//���ʮ�ּ�ͼ��
            {
                LCD_Point_Set(lcd_pos[i][0] - 15 + j, lcd_pos[i][1], LCD_COLOR_WHITE);
                LCD_Point_Set(lcd_pos[i][0], lcd_pos[i][1] - 15 + j, LCD_COLOR_WHITE);
            }
        }

        //У������-�������Ĵ������Ƿ���ȷ  �������ȷ����У׼
        //ˮƽ������֮��ľ���Ƚ�
        len1 = (float)sqrt((touch_pos[1].xval - touch_pos[0].xval) * (touch_pos[1].xval - touch_pos[0].xval) \
                           + (touch_pos[1].yval - touch_pos[0].yval) * (touch_pos[1].yval - touch_pos[0].yval));
        len2 = (float) sqrt((touch_pos[3].xval - touch_pos[2].xval) * (touch_pos[3].xval - touch_pos[2].xval) \
                            + (touch_pos[3].yval - touch_pos[2].yval) * (touch_pos[3].yval - touch_pos[2].yval));

        if(((len1 / len2) < 0.95) || ((len1 / len2) > 1.05))
        {
            continue; 	//����ĵ㲻����Ҫ��
        }

        //��ֱ������֮��ľ���Ƚ�
        len1 = (float)sqrt((touch_pos[2].xval - touch_pos[0].xval) * (touch_pos[2].xval - touch_pos[0].xval) \
                           + (touch_pos[2].yval - touch_pos[0].yval) * (touch_pos[2].yval - touch_pos[0].yval));
        len2 = (float)sqrt((touch_pos[3].xval - touch_pos[1].xval) * (touch_pos[3].xval - touch_pos[1].xval) \
                           + (touch_pos[3].yval - touch_pos[1].yval) * (touch_pos[3].yval - touch_pos[1].yval));

        if(((len1 / len2) < 0.95) || ((len1 / len2) > 1.05))
        {
            continue;	//����ĵ㲻����Ҫ��
        }

        //�Խ���������֮��ľ���Ƚ�
        len1 = (float)sqrt((touch_pos[3].xval - touch_pos[0].xval) * (touch_pos[3].xval - touch_pos[0].xval) \
                           + (touch_pos[3].yval - touch_pos[0].yval) * (touch_pos[3].yval - touch_pos[0].yval));
        len2 = (float)sqrt((touch_pos[2].xval - touch_pos[1].xval) * (touch_pos[2].xval - touch_pos[1].xval) \
                           + (touch_pos[2].yval - touch_pos[1].yval) * (touch_pos[2].yval - touch_pos[1].yval));

        if(((len1 / len2) < 0.95) || ((len1 / len2) > 1.05))
        {
            continue;	//����ĵ㲻����Ҫ��
        }

        //����У׼����   Kx (Ky)--б�ʣ�Bx(By) --ƫ����
        //����xӳ�� Xlcd = Kx * touch_x + Bx
        Kx = (float)(lcd_pos[1][0] - lcd_pos[0][0]) / (touch_pos[1].xval - touch_pos[0].xval);
        Bx = lcd_pos[0][0] - Kx * touch_pos[0].xval;
        //����yӳ�� Ylcd = Ky*touch_y + By
        Ky = (float)(lcd_pos[2][1] - lcd_pos[0][1]) / (touch_pos[2].yval - touch_pos[0].yval);
        By = lcd_pos[0][1] - Ky * touch_pos[0].yval;
        LCD_Color_Fill(0, 0, 240, 320, LCD_COLOR_WHITE);
//        Lcd_ShowString(30, 130, (uint8_t *)"touch adjust OK", LCD_COLOR_RED, LCD_COLOR_WHITE, 16);


        printf("У׼���� Ky=%f;Kx=%f;By=%d;Bx=%d;\r\n", Ky, Kx, By, Bx);
        Delay_Ms(1000);
        LCD_Color_Fill(0, 0, 240, 320, LCD_COLOR_WHITE);
        break;
    }
}
