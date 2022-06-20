#ifndef DWT_TIMER_H
#define DWT_TIMER_H

#include "main.h"
#include "stdint.h"

uint32_t DWT_Init(void);
void DWT_delay_us(volatile uint32_t micsec);
void DWT_delay_ms(volatile uint32_t milsec);
uint32_t DWT_getTick_us(); // return value of useconds 

#endif // DWT_TIMER_H
