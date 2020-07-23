#include "adc.h"
#include "uart.h"

#define EXTERNAL_TRIGGER 0

extern void DMA2_Stream4_IRQHandler(void);

static volatile uint8_t adc_tcc = 0;

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

	/* ADC configuration */
	
	/* ADC prescaler is 2 */
	ADC->CCR &= ~(ADC_CCR_ADCPRE_0 | ADC_CCR_ADCPRE_1);
	
	/*ADC resolution is 12 bit */
	ADC1->CR1 |= ADC_RES_12b;
	
	/* Using continious conversion mode */
	ADC1->CR2 |= ADC_CR2_CONT;
	
	/* Channel for conversion is 0 */
	ADC1->SQR3 = 0;
	ADC1->CR2 |= ADC_CR2_EOCS;
	
	/* Allow global interrupt */
	NVIC_EnableIRQ(DMA2_Stream4_IRQn);
}

void ADC_StartConv(uint16_t *buff, uint32_t len)
{
	/*If ADC is disabled we need to enable it */
	if ((ADC1->CR2 & ADC_CR2_ADON) != ADC_CR2_ADON)
	{
		ADC1->CR2 |= ADC_CR2_ADON; //enable ADC
#if 0
		volatile uint32_t counter = 0U;
		counter = (3 * (SystemCoreClock / 1000000U));
		
		/* Waiting for ADC is stabilize */
		while (counter != 0U)
		{
			counter--;
		} 
#endif
		
		if (ADC1->CR2 & ADC_CR2_ADON)
		{
			/* Clear end of conversion and overrun flag */
			ADC1->SR &= ~(ADC_SR_OVR | ADC_SR_EOC);
			
			/* Enable ADC DMA mode*/
			ADC1->CR2 |= ADC_CR2_DMA; 
			
			/* Start DMA channel */
			
			if (DMA2_Stream4->CR & DMA_SxCR_EN)
			{
				/* Disable DMA stream */
				DMA2_Stream4->CR &= ~DMA_SxCR_EN;
				
				/* Waitng for stream is disabled */
				while (DMA2_Stream4->CR & DMA_SxCR_EN);
			}
		}
		
		/* Prioriyt level high*/
		DMA2_Stream4->CR |= DMA_SxCR_PL_1;
		
		/* Memory address pointer is incremented after each data transfer */
		DMA2_Stream4->CR |= DMA_SxCR_MINC;
		
		/* Circular mode enable */
		DMA2_Stream4->CR |= DMA_SxCR_CIRC;
		
		/* Memory word width 16 bit */
		DMA2_Stream4->CR |= DMA_SxCR_MSIZE_0;
		
		/* Periph word width 16 bit */
		DMA2_Stream4->CR |= DMA_SxCR_PSIZE_0;
		
		/* Configure DMA Stream data length */
		DMA2_Stream4->NDTR = len;
		
		/* Configure DMA Stream direction as periph to mem */
		DMA2_Stream4->CR &= ~(DMA_SxCR_DIR_0 | DMA_SxCR_DIR_1); 
		
		/* Configure DMA Stream source address */
		DMA2_Stream4->PAR  = (uint32_t)&ADC1->DR;
		
		/* Configure DMA Stream destination address */
		DMA2_Stream4->M0AR = (uint32_t)buff;
		
		/* Clear all interrupt flags */
		DMA2->HIFCR |= (DMA_HIFCR_CTCIF4 | DMA_HIFCR_CHTIF4 | DMA_HIFCR_CTEIF4 | DMA_HIFCR_CDMEIF4 | DMA_HIFCR_CFEIF4);
		
		/* Enable transfer complete interrupt */
		DMA2_Stream4->CR |= DMA_SxCR_TCIE;
		
		/* Enable the Peripheral */
		DMA2_Stream4->CR |= DMA_SxCR_EN;
		
		/* Enable the selected ADC software conversion for regular group */ 
		ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART;
	}
}

void ADC_StopConv()
{
	/* If ADC is enabled we need to disable it */
	if (ADC1->CR2 & ADC_CR2_ADON)
	{
		/* Disable ADC */ 
		ADC1->CR2 &= ~ADC_CR2_ADON;
		
		/* Check if ADC is effectively disabled */
		if ((ADC1->CR2 & ADC_CR2_ADON) == 0U)
		{
			/* Disable the selected ADC DMA mode */
			ADC1->CR2 &= ~ADC_CR2_DMA;
			
			/* Disable transfer complete interrupt */
			DMA2_Stream4->CR &= ~DMA_SxCR_TCIE;
			
			/* Disable the stream */
			DMA2_Stream4->CR &= ~DMA_SxCR_EN;
			
			/* Check if the DMA Stream is effectively disabled */
			while (DMA2_Stream4->CR & DMA_SxCR_EN);
			
			/* Clear all interrupt flags */
			DMA2->HIFCR |= (DMA_HIFCR_CTCIF4 | DMA_HIFCR_CHTIF4 | DMA_HIFCR_CTEIF4 | DMA_HIFCR_CDMEIF4 | DMA_HIFCR_CFEIF4);
		}
	}
}

uint8_t ADC_GetTransferStatus()
{
	return adc_tcc;
}


void ADC_ClearTransferStatus()
{
	adc_tcc = 0;
}

void DMA2_Stream4_IRQHandler()
{
	if (DMA2->HISR & DMA_HISR_TCIF4)
	{
		/* Clear interrupt flag */
		DMA2->HIFCR |= DMA_HIFCR_CTCIF4;
		adc_tcc = 1;
	}
}
