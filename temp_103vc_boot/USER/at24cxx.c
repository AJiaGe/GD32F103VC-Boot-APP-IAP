#include "at24cxx.h"
#include "gd32f10x.h"
#include "systick.h"
#include "usart0.h"
#include <stdio.h>



/*********************************************************************************************************
* �� �� �� : 
* ����˵�� : 
* ��    �� : 
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 

uint8_t AT24CXX_Read_One_Byte(uint16_t ReadAddr)
{
    uint8_t temp = 0;
    I2C_Start();

    if(EE_TYPE > AT24C16)
    {
        I2C_Write_Byte(0XA0);	   //����д����
        I2C_Wait_ACK();
        I2C_Write_Byte(ReadAddr >> 8); //���͸ߵ�ַ
    }
    else I2C_Write_Byte(0XA0 + ((ReadAddr / 256) << 1)); //����������ַ0XA0,д����

    I2C_Wait_ACK();
    I2C_Write_Byte(ReadAddr % 256); //���͵͵�ַ
    I2C_Wait_ACK();
    I2C_Start();
    I2C_Write_Byte(0XA1);           //�������ģʽ
    I2C_Wait_ACK();
    temp = I2C_Read_Byte(0);
    I2C_Stop();						//����һ��ֹͣ����
    return temp;
}


/*********************************************************************************************************
* �� �� �� : 
* ����˵�� : 
* ��    �� : 
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 

void AT24CXX_Write_One_Byte(uint16_t WriteAddr, uint8_t Data)
{
    I2C_Start();

    if(EE_TYPE > AT24C16)
    {
        I2C_Write_Byte(MODULE_ADDRESS);	    //����д����
        I2C_Wait_ACK();
        I2C_Write_Byte(WriteAddr >> 8); //���͸ߵ�ַ
    }
    else I2C_Write_Byte(MODULE_ADDRESS + ((WriteAddr / 256) << 1)); //����������ַ0XA0,д����

    I2C_Wait_ACK();
    I2C_Write_Byte(WriteAddr % 256); 	//���͵͵�ַ
    I2C_Wait_ACK();
    I2C_Write_Byte(Data);     			//�����ֽ�
    I2C_Wait_ACK();
    I2C_Stop();							//����һ��ֹͣ����
    Delay_Ms(10);
}

/*********************************************************************************************************
* �� �� �� : AT24CXX_Check
* ����˵�� : 
* ��    �� : 
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 

uint8_t AT24CXX_Check(void)
{
    uint8_t volatile temp;
    temp = AT24CXX_Read_One_Byte(255); 	//����ÿ�ο�����дAT24CXX

    if(temp == 0X55)return 0;
    else								//�ų���һ�γ�ʼ�������
    {
        AT24CXX_Write_One_Byte(255, 0X55);
        temp = AT24CXX_Read_One_Byte(255);

        if(temp == 0X55)return 0;
    }
    return 1;
}

