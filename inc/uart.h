#ifndef INC_UART_H_
#define INC_UART_H_

#include <stdint.h>
#include "common.h"

void UART_Init(void);
void UART_Send(uint8_t *buff, uint32_t len);
void UART_UpdateCommands(struct packets *pck);
uint8_t UART_Get_DataTransferStatus(void);
void UART_Clear_DataTransferStatus(void);

#endif /* INC_UART_H_ */
