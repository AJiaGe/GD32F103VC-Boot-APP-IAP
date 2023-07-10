#include "flash.h"



/*************************************************************************************************************
*  @Name	    : flash_program
*  @Brief    	: flash program function
*  @Param[IN]  	: start_addr:start addr
				  data:data
				  size:data size
*  @Retval  	: TestStatus:fail--FAILED;success--PASSED
*  @Notes		: none
************************************************************************************************************/

TestStatus flash_program(uint32_t start_addr,uint32_t * data, uint16_t size)
{
	uint32_t Address;
    TestStatus TransferStatus = FAILED;
    uint32_t i;
    TransferStatus = PASSED;
	
    /* Unlock the Flash Bank1 Program Erase controller */
    fmc_unlock();

    /* Clear All pending flags */
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_END );


    /* Program Flash Bank1 */
    Address = start_addr;
    i = 0;

    while(Address < (start_addr + size))
    {
        fmc_word_program(Address, data[i]);
        i++;
        Address = Address + 4;
		fmc_flag_clear(FMC_FLAG_BANK0_PGERR | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_END );
    }

    fmc_lock();

    return TransferStatus;
}


/*************************************************************************************************************
*  @Name	    : flash_read_memory  
*  @Brief    	: flash read memory function
*  @Param[IN]  	: address:address
				  data:storage address
				  data_num:size
*  @Retval  	: none  
*  @Notes		: none
************************************************************************************************************/

void flash_read_memory(uint32_t address, uint8_t *data, uint8_t data_num)
{
	uint8_t i;
    uint8_t *p = (uint8_t *)address;

    for(i = 0; i < data_num; i++)
    {
        data[i] = *p;
        p++;
    }
}


/*************************************************************************************************************
*  @Name	    : flash_erase_page 
*  @Brief    	: flash erase page function
*  @Param[IN]  	: none
*  @Retval  	: none  
*  @Notes		: none
************************************************************************************************************/

void flash_erase_page(uint8_t num_pages, uint8_t page_num)
{
	uint16_t i;

    uint32_t base_address = 0x08000000;
    uint32_t page_address = base_address + (FMC_PAGE_SIZE * page_num);
    fmc_unlock();
	fmc_flag_clear(FMC_FLAG_BANK0_PGERR | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_END );

    for(i = 0; i < num_pages; i++)
    {
        fmc_page_erase(page_address);
        page_address += FMC_PAGE_SIZE;
		fmc_flag_clear(FMC_FLAG_BANK0_PGERR | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_END );
    }
}


/*************************************************************************************************************
*  @Name	    : check_sum
*  @Brief    	: check sum
*  @Param[IN]  	: data:source data
				  data_num:data size
				  check_data:check data
*  @Retval  	: none  
*  @Notes		: none
************************************************************************************************************/

ErrStatus check_sum(uint8_t *data, uint8_t data_num, uint8_t check_data)
{
    uint8_t sum = 0;
    uint8_t *p = data;
    uint8_t i;

    if(1 == data_num)
    {
        sum = ~(*p);
    }
    else
    {
        for(i = 0; i < data_num; i++)
        {
            sum ^= *p;
            p++;
        }
    }

    if(sum == check_data)
    {
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}


/*************************************************************************************************************
*  @Name	    : flash_crc
*  @Brief    	: flash crc function
*  @Param[IN]  	: start_addr:start addr
				  size:size
				  crc_value:crc value
*  @Retval  	: TestStatus:fail--FAILED;success--PASSED  
*  @Notes		: none
************************************************************************************************************/

TestStatus flash_crc(uint32_t start_addr, uint32_t size, uint16_t crc_value)
{
    uint8_t *p = (uint8_t*)start_addr;
    uint32_t len = size / 4 - 1;
    uint8_t temp[4] = {0};
	
	
    rcu_periph_clock_enable(RCU_PMU);
    rcu_periph_clock_enable(RCU_CRC);
	
    CRC_CTL = CRC_CTL_RST;

    do
    {
        temp[0] = *(p + 3);
        temp[1] = *(p + 2);
        temp[2] = *(p + 1);
        temp[3] = *p;
        CRC_DATA = * (uint32_t *)temp;
        p = p + 4;
    }
    while (len--);

    if((CRC_DATA & 0xFFFF) != (uint32_t)crc_value)
    {
        return FAILED;
    }
    else
    {
        return PASSED;
    }
}


/*************************************************************************************************************
*  @Name	    : flash_read_pid 
*  @Brief    	: flash read pid function
*  @Param[IN]  	: none
*  @Retval  	: none  
*  @Notes		: none
************************************************************************************************************/

void flash_read_pid(uint8_t *data)
{
	uint32_t temp = FMC_PID;
	
    data[0] = (uint8_t)(temp 	& 	0XFF);
    data[1] = (uint8_t)((temp 	>> 	8) & 0XFF);
    data[2] = (uint8_t)((temp 	>> 	16) & 0XFF);
    data[3] = (uint8_t)((temp 	>> 	24) & 0XFF);
}

