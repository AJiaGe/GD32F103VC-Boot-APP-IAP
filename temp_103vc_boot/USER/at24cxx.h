#ifndef __AT24CXX_H
#define __AT24CXX_H

#include "iic.h"


#define MODULE_ADDRESS				0XA0




uint8_t AT24CXX_Read_One_Byte(uint16_t ReadAddr);

void AT24CXX_Write_One_Byte(uint16_t WriteAddr, uint8_t Data);

uint8_t AT24CXX_Check(void);


#endif
