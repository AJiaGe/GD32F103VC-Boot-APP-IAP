#ifndef __FLASH_H
#define __FLASH_H

#include "gd32f10x.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

#define FMC_PAGE_SIZE           ((uint16_t)0x400)

TestStatus flash_program(uint32_t start_addr,uint32_t * data, uint16_t size);

void flash_read_memory(uint32_t address, uint8_t *data, uint8_t data_num);

void flash_erase_page(uint8_t num_pages, uint8_t page_num);

ErrStatus check_sum(uint8_t *data, uint8_t data_num, uint8_t check_data);

TestStatus flash_crc(uint32_t start_addr, uint32_t size, uint16_t crc_value);

void flash_read_pid(uint8_t *data);


#endif
