#include "adc.h"

#define EXTERNAL_TRIGGER 0

void ADC_Init()
{
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; //enable clocking ADC1
	
	if (!(RCC->AHB1ENR & RCC_AHB1ENR_DMA2EN))
	{
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	}

	if (!(RCC->AHB1ENR & RCC_AHB1ENR_GPIOAEN))
	{
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; //enable clocking PORTA
	}

	GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE0_1; //analog mode
	
#if EXTERNAL_TRIGGER
	/*Configure external trigger PC11*/
	if (!(RCC->APB2ENR & RCC_APB2ENR_SYSCFGEN))
	{
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	}

	if (!(RCC->AHB1ENR & RCC_AHB1ENR_GPIOCEN))
	{
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	}
#if DEBUG_EXTI
	if (!(RCC->AHB1ENR & RCC_AHB1ENR_GPIODEN))
	{
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	}
	GPIOD->MODER |= GPIO_MODER_MODE14_0;
#endif
	GPIOC->MODER &= ~(GPIO_MODER_MODE11_0 | GPIO_MODER_MODE11_1); //PA11 as input
	SYSCFG->EXTICR[2] = SYSCFG_EXTICR3_EXTI11_PC; //PC11 is source input for EXTI11
	EXTI->FTSR |= EXTI_FTSR_TR11; //Falling trigger enabled for input line.
	EXTI->PR = EXTI_PR_PR11; //clear interrupt flag
	EXTI->IMR |= EXTI_IMR_IM11; //Interrupt request from line 11 is not masked !!!!
	NVIC_EnableIRQ(EXTI15_10_IRQn);
#endif /* EXTERNAL_TRIGGER */

	/* ADC configure */
	ADC1->CR1 |= ADC_RES_8b; //8-bit (11 ADCCLK cycles)
	//ADC1->CR2 |= ADC_CR2_EXTEN_1; //Trigger detection on the falling edge
	ADC1->SQR3 = 0; //channel 0
	ADC1->CR2 |= ADC_CR2_DDS; //DMA requests are issued as long as data are converted and DMA=1
	ADC1->CR2 |= ADC_CR2_DMA; //DMA mode enabled
	ADC1->CR2 |= ADC_CR2_CONT; //Continuous conversion mode
}

void ADC_StartConv(uint8_t *buff, uint32_t len)
{
	DMA2_Stream4->CR &= ~DMA_SxCR_EN; //disable DMA
	ADC1->CR2 |= ADC_CR2_ADON; //enable ADC

	/* Configure DMA */
	DMA2_Stream4->CR |= DMA_SxCR_PL_1; //priority level - high
	DMA2_Stream4->CR |= DMA_SxCR_MINC; //Memory address pointer is incremented after each data transfer
	DMA2_Stream4->CR |= DMA_SxCR_CIRC; //circular mode enable
	DMA2_Stream4->NDTR = len;
	DMA2_Stream4->PAR  = (uint32_t)&ADC1->DR;
	DMA2_Stream4->M0AR = (uint32_t)buff;
	DMA2->HIFCR |= DMA_HIFCR_CTCIF4; //clear interrupt flag
	NVIC_EnableIRQ(DMA2_Stream4_IRQn);
	DMA2_Stream4->CR |= DMA_SxCR_TCIE; //enable transfer complete interrupt
	DMA2_Stream4->CR |= DMA_SxCR_EN; //enable DMA
	ADC1->CR2 |= ADC_CR2_SWSTART; //start ADC conversion
}

void ADC_StopConv()
{
	DMA2_Stream4->CR &= ~DMA_SxCR_EN; //disable DMA
	DMA2_Stream4->CR &= ~DMA_SxCR_TCIE; //disable transfer complete interrupt
	ADC1->CR2 &= ~ADC_CR2_ADON; //disable ADC
}

