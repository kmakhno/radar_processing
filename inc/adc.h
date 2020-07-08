#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32f4xx.h"
#include <stdint.h>

#define DEBUG_EXTI 0

#define ADC_RES_12b (0)
#define ADC_RES_10b (ADC_CR1_RES_0)
#define ADC_RES_8b  (ADC_CR1_RES_1)
#define ADC_RES_6b  (ADC_CR1_RES_0 | ADC_CR1_RES_1)

void ADC_Init(void);
void ADC_StartConv(uint8_t *buff, uint32_t len);
void ADC_StopConv(void);

#endif /* INC_ADC_H_ */
