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
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	GPIOD->MODER |= GPIO_MODER_MODE15_0;
	UART_Send("Hello\r\n", 7);
	UART_Send("Hello\r\n", 7);
	
	while(1)
	{
		
	}
}

static void DMA1_Stream5_IRQHandler()
{

}



