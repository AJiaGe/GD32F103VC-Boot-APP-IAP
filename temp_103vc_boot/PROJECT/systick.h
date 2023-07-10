#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

#define SYSTICK_SELF

#ifdef SYSTICK_SELF

void Systick_Init(uint8_t clk);

void Delay_Us(uint32_t us);

void Delay_Ms(uint32_t Ms);





#else

/* configure systick */
void systick_config(void);

/* delay a time in milliseconds */
void delay_1ms(uint32_t count);

/* delay decrement */
void delay_decrement(void);

void delay_1us(uint32_t count);

#endif


#endif /* SYSTICK_H */
