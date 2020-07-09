#include "stm32f4xx.h"                  // Device header
#include "clock.h"
#include "dac.h"
#include "adc.h"
#include "uart.h"
#include "stdint.h"

#pragma GCC diagnostic ignored "-Wnewline-eof"
volatile uint8_t rxBuff = 0;
static uint8_t txBuff = 10;

int main(void)
{
	Clock_Init();
	DAC_Init();
	ADC_Init();
	UART_Init();
	UART_Send(&txBuff, 1);
	txBuff = 0xDC;
	UART_Send(&txBuff, 1);
	
	while(1)
	{
		
	}
}