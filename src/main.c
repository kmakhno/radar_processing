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
			break;
		}
#endif
	}
}

