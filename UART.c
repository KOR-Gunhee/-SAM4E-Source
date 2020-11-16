#include ".\cpu.h"


void UARTInit(void)
{
	UART0->UART_CR=(1<<RSTTX)|(1<<RSTRX)|(1<<TXEN)|(1<<RXEN);
	UART0->UART_MR=(0<<CHMODE)|(4<<PAR);
	UART0->UART_BRGR=(SystemCoreClock)/(16*115200);
}

void UARTSTART(void)
{
	UART0->UART_CR|=((1<<TXEN)|(1<<RXEN));
}

void SendByte(unsigned char ch)
{
	while((UART0->UART_SR&(1<<TXRDY))==0);

	UART0->UART_THR=ch;
}

int SendString(const char *S_str)
{
	while(*S_str)
	{
		SendByte(*S_str);
		S_str++;
	}
	return 0;
}

uint32_t receive(void)
{
	while((UART0->UART_SR&(1<<RXRDY))==0);
	return UART0->UART_RHR;
}

uint32_t ReadByte(uint16_t *data)
{
	if((UART0->UART_SR&(1<<RXRDY))==0)
	return 1;     

	*data = (uint16_t)UART0->UART_RHR;
	return 0;
}
