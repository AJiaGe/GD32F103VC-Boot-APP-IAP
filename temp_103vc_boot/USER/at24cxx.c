#include "at24cxx.h"
#include "gd32f10x.h"
#include "systick.h"
#include "usart0.h"
#include <stdio.h>



/*********************************************************************************************************
* 函 数 名 : 
* 功能说明 : 
* 形    参 : 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 

uint8_t AT24CXX_Read_One_Byte(uint16_t ReadAddr)
{
    uint8_t temp = 0;
    I2C_Start();

    if(EE_TYPE > AT24C16)
    {
        I2C_Write_Byte(0XA0);	   //发送写命令
        I2C_Wait_ACK();
        I2C_Write_Byte(ReadAddr >> 8); //发送高地址
    }
    else I2C_Write_Byte(0XA0 + ((ReadAddr / 256) << 1)); //发送器件地址0XA0,写数据

    I2C_Wait_ACK();
    I2C_Write_Byte(ReadAddr % 256); //发送低地址
    I2C_Wait_ACK();
    I2C_Start();
    I2C_Write_Byte(0XA1);           //进入接收模式
    I2C_Wait_ACK();
    temp = I2C_Read_Byte(0);
    I2C_Stop();						//产生一个停止条件
    return temp;
}


/*********************************************************************************************************
* 函 数 名 : 
* 功能说明 : 
* 形    参 : 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 

void AT24CXX_Write_One_Byte(uint16_t WriteAddr, uint8_t Data)
{
    I2C_Start();

    if(EE_TYPE > AT24C16)
    {
        I2C_Write_Byte(MODULE_ADDRESS);	    //发送写命令
        I2C_Wait_ACK();
        I2C_Write_Byte(WriteAddr >> 8); //发送高地址
    }
    else I2C_Write_Byte(MODULE_ADDRESS + ((WriteAddr / 256) << 1)); //发送器件地址0XA0,写数据

    I2C_Wait_ACK();
    I2C_Write_Byte(WriteAddr % 256); 	//发送低地址
    I2C_Wait_ACK();
    I2C_Write_Byte(Data);     			//发送字节
    I2C_Wait_ACK();
    I2C_Stop();							//产生一个停止条件
    Delay_Ms(10);
}

/*********************************************************************************************************
* 函 数 名 : AT24CXX_Check
* 功能说明 : 
* 形    参 : 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 

uint8_t AT24CXX_Check(void)
{
    uint8_t volatile temp;
    temp = AT24CXX_Read_One_Byte(255); 	//避免每次开机都写AT24CXX

    if(temp == 0X55)return 0;
    else								//排除第一次初始化的情况
    {
        AT24CXX_Write_One_Byte(255, 0X55);
        temp = AT24CXX_Read_One_Byte(255);

        if(temp == 0X55)return 0;
    }
    return 1;
}

