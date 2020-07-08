#include "stm32f4xx.h"                  // Device header
#include "clock.h"
#include "dac.h"
#include "adc.h"
//#include "uart.h"
#include "stdint.h"
#include <string.h>

static const uint16_t sin[32] = {
								 2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
								 3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
								 599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};
static uint8_t rxBuff[32] = {0};

int main(void)
{
	Clock_Init();
	DAC_Init();
	ADC_Init();
	//UART_Init();
	DAC_StartConv(sin, 32);
	ADC_StartConv((uint8_t *)rxBuff, 64);
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	GPIOD->MODER |= GPIO_MODER_MODE15_0;
	//UART_Send(rxBuff, 32);
	//UART_Send(rxBuff, 32);
	
	while(1)
	{
		
	}
}

static void DMA1_Stream5_IRQHandler()
{

}

static void DMA1_Stream6_IRQHandler(void)
{
	if (DMA1->HISR & DMA_HISR_TCIF6)
	{
		DMA1->HIFCR |= DMA_HIFCR_CTCIF6; //clear interrupt before sending
		DMA1_Stream6->CR &= ~DMA_SxCR_TCIE; //disable interrupt
		GPIOD->ODR |= GPIO_ODR_OD15;
	}
}

