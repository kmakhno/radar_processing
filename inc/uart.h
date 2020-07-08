#ifndef INC_UART_H_
#define INC_UART_H_

#include <stdint.h>

void UART_Init(void);
void UART_Send(uint8_t *buff, uint32_t len);

#endif /* INC_UART_H_ */
