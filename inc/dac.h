#ifndef DAC_H
#define DAC_H

#include "stdint.h"

void DAC_Init(void);
void DAC_StartConv(const uint16_t *buff, uint32_t len);
void DAC_StopConv(void);

#endif /* DAC_H */
