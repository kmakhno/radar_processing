#include "stm32f4xx.h"                  // Device header
#include "clock.h"
#include "dac.h"
#include "adc.h"
#include "uart.h"
#include "stdint.h"
#include "delay.h"
#include "common.h"

enum states
{	
	PROCESS_OFF = 0,
	PROCESS_ON
};

enum states state;
enum states nextState = PROCESS_OFF;

#if 0
static struct packets rPacket = {0};
#else
struct packets rPacket = {0};
#endif

static uint16_t matrix[SAWTOOTH100MCS_SIZE] = {0}; 

int main(void)
{
	Clock_Init();
#if 0
	SysTick_Config(SystemCoreClock/1000-1);
#endif
	UART_Init();
#if 1
	ADC_Init();
	DAC_Init();
	DAC_Enable(0);
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
							ADC_StartConv(matrix, SAWTOOTH100MCS_SIZE);
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
					switch(rPacket.stg.tsweep)
					{
						case TSWEEP_100mcs:
							UART_Send((uint8_t *)matrix, 2*SAWTOOTH100MCS_SIZE);
						break;
						
						case TSWEEP_50mcs:
							UART_Send((uint8_t *)matrix, 2*SAWTOOTH50MCS_SIZE);
						break;
						
						case TSWEEP_33_5mcs:
							UART_Send((uint8_t *)matrix, 2*SAWTOOTH33_5MCS_SIZE);
						break;
						
						case TSWEEP_25mcs:
							UART_Send((uint8_t *)matrix, 2*SAWTOOTH25MCS_SIZE);
						break;
					}
				}
				
				/*If transfer was done we need to clear matrix array */
				if(UART_Get_DataTransferStatus())
				{
					UART_Clear_DataTransferStatus();
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

