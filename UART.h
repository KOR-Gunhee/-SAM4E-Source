/*
 * UART.h
 *
 * Created: 2019-02-15 오전 10:03:43
 *  Author: ghhan
 */ 


#ifndef UART_H_
#define UART_H_


#define TXD0 10
#define RXD0 9
#define RSTTX 3
#define RSTRX 2
#define CHMODE 14
#define PAR 9
#define	TXEN 6
#define RXEN 4
#define TXRDY 1
#define RXRDY 0



void UARTInit(void);
void UARTSTART(void);
void SendByte(unsigned char ch);
int SendString(const char *S_str);
uint32_t receive(void);
uint32_t ReadByte(uint16_t *data);

#endif /* UART_H_ */