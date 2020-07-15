#ifndef DAC_H
#define DAC_H

#include "stdint.h"

void DAC_Init(void);
void DAC_StartConv(void);
void DAC_StopConv(void);
uint8_t DAC_GetTransferStatus(void);
void DAC_ClearTransferStatus(void);

#endif /* DAC_H */
