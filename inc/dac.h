#ifndef DAC_H
#define DAC_H

#include "stdint.h"

void DAC_Init(void);
void DAC_Enable(uint8_t type);
void DAC_StopConv(void);

#endif /* DAC_H */
