#include "stm32f4xx.h"                  // Device header
#include "clock.h"

#define SYSCLOCK_DEBUG 0


int Clock_Init(void)
{
	int startUpCounter;
	
#if SYSCLOCK_DEBUG
	RCC->CFGR &=  ~(RCC_CFGR_MCO2_0 | RCC_CFGR_MCO2_1); //sysclk source
	RCC->CFGR |= RCC_CFGR_MCO2PRE_2 | RCC_CFGR_MCO2PRE_1; // sysclk/4
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER |= GPIO_MODER_MODE9_1;
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_0 | GPIO_OSPEEDER_OSPEEDR9_1;
#endif
	
	/* HSE enable */
	RCC->CR |= RCC_CR_HSEON;
	for (startUpCounter = 0; ; startUpCounter++)
	{
		if (RCC->CR & RCC_CR_HSERDY)
		{
			break;
		}
		
		if (startUpCounter > 0x1000)
		{
			RCC->CR &= ~RCC_CR_HSEON;
			return -1;
		}
	}
	
	/* PLL configuration */
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_2; //Fhse/4 = 8/4 = 2 MHz
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_4; //clear this bit because it sets by default

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_7); //clear this bit because it sets by default
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_3; //xN = 72 -> Fvcoout = 144 MHz
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP_1 | RCC_PLLCFGR_PLLP_0); // 144/2 = 72
	RCC->CR |= RCC_CR_PLLON;
	for (startUpCounter = 0; ; startUpCounter++)
	{
		if (RCC->CR & RCC_CR_PLLRDY)
		{
			break;
		}
		
		if (startUpCounter > 0x1000)
		{
			RCC->CR &= ~RCC_CR_PLLON;
			RCC->CR &= ~RCC_CR_HSEON;
			return -2;
		}
	}
	
	FLASH->ACR |= FLASH_ACR_LATENCY_3WS; //latency - 5 wait state
	
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1; //F = 72/1 = 72 Mhz
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; //Fapb1 = 36 Mhz
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; //Fapb2 = 72 Mhz
	
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));

	RCC->CR &= ~RCC_CR_HSION;
	
	return 0;
}
