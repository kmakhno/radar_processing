#include "stm32f4xx.h"                  // Device header
#include "clock.h"
#include "dac.h"
#include "adc.h"
#include "uart.h"
#include "stdint.h"
#include "delay.h"
#include "common.h"
#include <stdlib.h>

#define DEV_BOARD 1

static uint32_t createPacket(const uint16_t *src, uint16_t *dst, uint32_t len);
static void clearPacket(uint16_t *packet);

enum states
{	
	PROCESS_OFF = 0,
	PROCESS_ON
};

#if 0
static enum states state;
static enum states nextState = PROCESS_OFF;
#else
enum states state;
enum states nextState = PROCESS_OFF;
#endif

#if 0
static struct packets rPacket = {0};
static uint16_t *tPacket = NULL;
#else
struct packets rPacket = {0};
uint16_t *tPacket = NULL;
#endif

#if 0
static uint16_t matrix[SAWTOOTH100MCS_SIZE] = {0}; 
#else
uint16_t matrix[SAWTOOTH100MCS_SIZE] = {0};
#endif

int main(void)
{
	Clock_Init();
	
#if DEV_BOARD == 1
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	GPIOD->MODER |= GPIO_MODER_MODE14_0;
#endif
	
#if 0
	SysTick_Config(SystemCoreClock/1000-1);
#endif
	UART_Init();

	ADC_Init();
	DAC_Init();
#if 0
	GPIOB->MODER |= GPIO_MODER_MODE5_0;
	GPIOB->MODER |= GPIO_MODER_MODE6_0;
#endif
 
	while(1)
	{
#if 1
		UART_UpdateCommands(&rPacket);
		
		state = nextState;
		
		/* Next state logic */
		switch(state)
		{
			case PROCESS_OFF:
				if (rPacket.cmd == START_CMD)
				{
					nextState = PROCESS_ON;
				}
				else if (rPacket.cmd == STOP_CMD)
				{
					nextState = PROCESS_OFF;
				}
			break;
			
			case PROCESS_ON:
				if (rPacket.cmd == START_CMD)
				{
					nextState = PROCESS_ON;
				}
				else if(rPacket.cmd == STOP_CMD)
				{
					nextState = PROCESS_OFF;
				}
			break;
		}
		
		/* Output logic */
		switch (state)
		{
			case PROCESS_OFF:
				/* Stop conversions */
				ADC_StopConv();
				DAC_StopConv();
			break;
			
			case PROCESS_ON:
				DAC_Enable(rPacket.stg.tsweep);
			
				/*If conversion is allowed we need to enable ADC depending on period of sawtooth */
				if (Get_ADC_ConversionStatus())
				{
					switch(rPacket.stg.tsweep)
					{
						case TSWEEP_100mcs:
							ADC_StartConv(matrix, SAWTOOTH100MCS_SIZE-10);
						break;
						
						case TSWEEP_50mcs:
							ADC_StartConv(matrix, SAWTOOTH50MCS_SIZE);
						break;
						
						case TSWEEP_33_5mcs:
							ADC_StartConv(matrix, SAWTOOTH33_5MCS_SIZE);
						break;
						
						case TSWEEP_25mcs:
							ADC_StartConv(matrix, SAWTOOTH25MCS_SIZE);
						break;
					}
				}
				else
				{
					ADC_StopConv();
				}
				
				/* When ADC DMA evemt occured we need to transmit data via UART */
				if (ADC_GetTransferStatus())
				{
					ADC_ClearTransferStatus();
					uint32_t sz = 0;
					switch(rPacket.stg.tsweep)
					{
						case TSWEEP_100mcs:
							sz = createPacket(matrix, tPacket, SAWTOOTH100MCS_SIZE);
							UART_Send((uint8_t *)tPacket, 2*sz);
						break;
						
						case TSWEEP_50mcs:
							sz = createPacket(matrix, tPacket, SAWTOOTH50MCS_SIZE);
							UART_Send((uint8_t *)tPacket, 2*sz);
						break;
						
						case TSWEEP_33_5mcs:
							sz = createPacket(matrix, tPacket, SAWTOOTH33_5MCS_SIZE);
							UART_Send((uint8_t *)tPacket, 2*sz);
						break;
						
						case TSWEEP_25mcs:
							sz = createPacket(matrix, tPacket, SAWTOOTH25MCS_SIZE);
							UART_Send((uint8_t *)tPacket, 2*sz);
						break;
					}
				}
				
				/*If transfer was done we need to clear matrix array */
				if(UART_Get_DataTransferStatus())
				{
					UART_Clear_DataTransferStatus();
					clearPacket(tPacket);
					for (unsigned int i = 0; i < SAWTOOTH100MCS_SIZE; i++)
					{
						matrix[i] = 0;
					}
				}
				
			break;
		}
#endif
	}
}


static uint32_t createPacket(const uint16_t *src, uint16_t *dst, uint32_t len)
{
	
	dst = (uint16_t *)calloc(len + 2, sizeof(uint16_t));
	
	if (dst == NULL)
	{
#if DEV_BOARD == 1
	GPIOD->ODR = GPIO_ODR_OD14;
#endif
		return 0;
	}
	
	dst[0] = 0xFFFF;
	
	uint32_t i = 1;
	
	for (; i < len + 1; i++)
	{
		dst[i] = src[i - 1];
	}
	
	dst[i++] = 0xAAAA;
	
	return i;
}

static void clearPacket(uint16_t *packet)
{
	free(packet);
}
