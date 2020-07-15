#include "stm32f4xx.h"                  // Device header
#include "clock.h"
#include "dac.h"
#include "adc.h"
#include "uart.h"
#include "stdint.h"
#include "delay.h"


int main(void)
{
	Clock_Init();
	//SysTick_Config(SystemCoreClock/1000-1);
	UART_Init();
	ADC_Init();
	DAC_Init();
	GPIOB->MODER |= GPIO_MODER_MODE5_0;
	GPIOB->MODER |= GPIO_MODER_MODE6_0;

	while(1)
	{

	}
}

