#include "fmc.h"



/*********************************************************************************************************
* �� �� �� : 
* ����˵�� : 
* ��    �� : 
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 

void FMC_Flash_Write( uint32_t Address, uint16_t *pData, uint8_t Size )
{
    uint16_t i;    
	
    /* ���� */
    fmc_unlock();	
	fmc_page_erase( FMC_FLASH_START_ADDR );

	
    /* ����FMCǰ�����STAT ״̬�Ĵ������ǳ���Ҫ*/
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_END );
    
    for (i = 0; i < Size; i++)
    {
        fmc_halfword_program(Address, pData[i]);
 
        Address += 2;//��ַ�ۼ�
    }
	
    /* ���� */
    fmc_lock();
}


/*********************************************************************************************************
* �� �� �� : 
* ����˵�� : 
* ��    �� : 
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 

void FMC_Flash_Read( uint32_t Address, uint16_t *pData, uint8_t Size )
{
    for( uint8_t dataIndex = 0; dataIndex < Size; dataIndex++ )
    {
        pData[dataIndex] = *( __IO uint16_t* )Address;
        Address += 2;					//��ַ�ۼ�
//		printf("%0.4x",pData[dataIndex]);
    }
	printf("\n");
}
