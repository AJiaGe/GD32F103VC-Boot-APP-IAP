#include "fmc.h"



/*********************************************************************************************************
* 函 数 名 : 
* 功能说明 : 
* 形    参 : 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 

void FMC_Flash_Write( uint32_t Address, uint16_t *pData, uint8_t Size )
{
    uint16_t i;    
	
    /* 解锁 */
    fmc_unlock();	
	fmc_page_erase( FMC_FLASH_START_ADDR );

	
    /* 操作FMC前先清空STAT 状态寄存器，非常必要*/
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_END );
    
    for (i = 0; i < Size; i++)
    {
        fmc_halfword_program(Address, pData[i]);
 
        Address += 2;//地址累加
    }
	
    /* 上锁 */
    fmc_lock();
}


/*********************************************************************************************************
* 函 数 名 : 
* 功能说明 : 
* 形    参 : 
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 

void FMC_Flash_Read( uint32_t Address, uint16_t *pData, uint8_t Size )
{
    for( uint8_t dataIndex = 0; dataIndex < Size; dataIndex++ )
    {
        pData[dataIndex] = *( __IO uint16_t* )Address;
        Address += 2;					//地址累加
//		printf("%0.4x",pData[dataIndex]);
    }
	printf("\n");
}
