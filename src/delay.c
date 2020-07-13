#include "stm32f4xx.h"                  // Device header
#include "delay.h"

extern void SysTick_Handler(void);

volatile static uint32_t ticks_delay = 0;

void delay_ms(uint32_t milliseconds)
{
	uint32_t start = ticks_delay;
	while((ticks_delay - start) < milliseconds);
}


void SysTick_Handler(void)
{
	ticks_delay++;
}
