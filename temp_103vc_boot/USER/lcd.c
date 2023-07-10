#include "lcd.h"


/*********************************************************************************************************
* �� �� �� : LCD_Register_Write
* ����˵�� : LCD д�Ĵ���
* ��    �� : register_id���Ĵ���x 	value��д���ֵ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 

void LCD_Register_Write(uint16_t register_id, uint16_t value)
{
//    *(__IO uint16_t *) (BANK0_LCD_C) = register_id;
//    *(__IO uint16_t *) (BANK0_LCD_D) = value;
	
	LCD->CMD = register_id;	 
	LCD->DAT = value;
}


/*********************************************************************************************************
* �� �� �� : LCD_Command_Write
* ����˵�� : LCD д����
* ��    �� : value������ֵ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 

void LCD_Command_Write(uint16_t value)
{
    /* write 16-bit index, then write reg */
//    *(__IO uint16_t *) (BANK0_LCD_C) = value;
	LCD->CMD = value;
}


/*********************************************************************************************************
* �� �� �� : LCD_Point_Set
* ����˵�� : LCD ���û��㺯��
* ��    �� : x��x���� y��y���� point����ɫֵ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 

void LCD_Point_Set(uint16_t x, uint16_t y, uint16_t color)
{
    if ((x > LCD_PIXEL_HEIGHT) || (y > LCD_PIXEL_WIDTH))
    {
        return;
    }

	LCD_Register_Write(LCD_COMMAND_X_SET, x);
	LCD_Register_Write(LCD_COMMAND_Y_SET, y);
	LCD_Register_Write(LCD_COMMAND_WRITE_DATE, color);
}


/*********************************************************************************************************
* �� �� �� : LCD_Init
* ����˵�� : LCD ��ʼ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ����EXMC LCD_GPIO ��ʼ��
*********************************************************************************************************/ 

void LCD_Init(void)
{
	EXMC_GPIO_Init();
	EXMC_Config_Init();
	
	LCD_Register_Write(0x01, 0x0100);        //driver output control
	LCD_Register_Write(0x02, 0x0700);        //lcd driver waveform control
	LCD_Register_Write(0x03, 0x1030);        //entry mode set

	LCD_Register_Write(0x04, 0x0000);        //resizing control
	LCD_Register_Write(0x08, 0x0202);        //display control 2
	LCD_Register_Write(0x09, 0x0000);        //display control 3
	LCD_Register_Write(0x0a, 0x0000);        //frame cycle control
	LCD_Register_Write(0x0c, (1 << 0));      //extern display interface control 1
	LCD_Register_Write(0x0d, 0x0000);        //frame maker position
	LCD_Register_Write(0x0f, 0x0000);        //extern display interface control 2

	Delay_Ms( 100 );

	LCD_Register_Write(0x07, 0x0101);        //display control

	Delay_Ms( 100 );

	LCD_Register_Write(0x10, (1 << 12) | (0 << 8) | (1 << 7) | (1 << 6) | (0 << 4)); //power control 1
	LCD_Register_Write(0x11, 0x0007);                                    //power control 2
	LCD_Register_Write(0x12, (1 << 8) | (1 << 4) | (0 << 0));            //power control 3
	LCD_Register_Write(0x13, 0x0b00);                                    //power control 4
	LCD_Register_Write(0x29, 0x0000);                                    //power control 7
	LCD_Register_Write(0x2b, (1 << 14) | (1 << 4));
	LCD_Register_Write(0x50, 0);             //set x start
	LCD_Register_Write(0x51, 239);           //set x end
	LCD_Register_Write(0x52, 0);             //set y start
	LCD_Register_Write(0x53, 319);           //set y end

	LCD_Register_Write(0x60, 0x2700);        //driver output control
	LCD_Register_Write(0x61, 0x0001);        //driver output control
	LCD_Register_Write(0x6a, 0x0000);        //vertical srcoll control

	LCD_Register_Write(0x80, 0x0000);        //display position? partial display 1
	LCD_Register_Write(0x81, 0x0000);        //ram address start? partial display 1
	LCD_Register_Write(0x82, 0x0000);        //ram address end-partial display 1
	LCD_Register_Write(0x83, 0x0000);        //display position? partial display 2
	LCD_Register_Write(0x84, 0x0000);        //ram address start? partial display 2
	LCD_Register_Write(0x85, 0x0000);        //ram address end? partial display 2

	LCD_Register_Write(0x90, (0 << 7) | (16 << 0)); //frame cycle control
	LCD_Register_Write(0x92, 0x0000);        //panel interface control 2
	LCD_Register_Write(0x93, 0x0001);        //panel interface control 3
	LCD_Register_Write(0x95, 0x0110);        //frame cycle control
	LCD_Register_Write(0x97, (0 << 8));
	LCD_Register_Write(0x98, 0x0000);        //frame cycle control

	Delay_Ms( 100 );

	LCD_Register_Write(0x07, 0x0173);

	LCD_Clear( LCD_COLOR_WHITE );			//Ĭ������
	
	Delay_Ms( 100 );
}


/*********************************************************************************************************
* �� �� �� : LCD_Clear
* ����˵�� : LCD ͳһ���ó�ĳ����ɫ
* ��    �� : color����ɫֵ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 

void LCD_Clear(uint16_t color)
{
	uint32_t index = 0;

	LCD_Register_Write(0x20, 0);
	LCD_Register_Write(0x21, 0);
	LCD_Command_Write(0x22);

	for(index = 0; index < LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT; index++)
	{
//		*(__IO uint16_t *) (BANK0_LCD_D) = color;
		LCD->DAT = color;
	}
}


/*********************************************************************************************************
* �� �� �� : LCD_Draw_Circle
* ����˵�� : ��ָ��λ�û�һ��ָ����С��Բ
* ��    �� : //x,y:���ĵ� r:�뾶 c_color:��ɫ
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/

void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t c_color)
{
    int a, b;
    int di;
    a 	= 0;
    b 	= r;
    di 	= 3 - (r << 1);             						 //�ж��¸���λ�õı�־

    while(a <= b)
    {
        LCD_Point_Set(x0 + a, y0 - b, c_color);       //5
        LCD_Point_Set(x0 + b, y0 - a, c_color);       //0
        LCD_Point_Set(x0 + b, y0 + a, c_color);       //4
        LCD_Point_Set(x0 + a, y0 + b, c_color);       //6
        LCD_Point_Set(x0 - a, y0 + b, c_color);       //1
        LCD_Point_Set(x0 - b, y0 + a, c_color);
        LCD_Point_Set(x0 - a, y0 - b, c_color);       //2
        LCD_Point_Set(x0 - b, y0 - a, c_color);       //7
        a++;

        //ʹ��Bresenham�㷨��Բ
        if(di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}


/*********************************************************************************************************
* �� �� �� : LCD_Color_Fill
* ����˵�� : 
* ��    �� : 
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 

void LCD_Color_Fill(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color)
{
    uint16_t x, y;

    for (x = start_x; x < end_x; x++)
    {
        for (y = start_y; y < end_y; y++)
        {
            LCD_Point_Set(x, y, color);
        }
    }
}

