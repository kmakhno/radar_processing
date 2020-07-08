#include "stm32f4xx.h"                  // Device header
#include "clock.h"
#include "dac.h"
#include "adc.h"
#include "uart.h"
#include "stdint.h"

int main(void)
{
	Clock_Init();
	DAC_Init();
	ADC_Init();
	UART_Init();
	
	while(1)
	{
		
	}
}


