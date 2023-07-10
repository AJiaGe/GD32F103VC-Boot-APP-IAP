#ifndef __FMC_H
#define __FMC_H


#include "gd32f10x.h"
#include "systick.h"
#include "io_bit.h"
#include <stdio.h>

#define FMC_FLASH_PAGE_SIZE 			(2048U)
#define FMC_FLASH_START_PAGE			(126)
#define FMC_FLASH_START_ADDR			(0x0803F000)
#define FMC_FLASH_END_ADDR				(FMC_FLASH_START_ADDR + FMC_FLASH_START_ADDR)


void FMC_Flash_Write( uint32_t Address, uint16_t *pData, uint8_t Size );

void FMC_Flash_Read( uint32_t Address, uint16_t *pData, uint8_t Size );



#endif
