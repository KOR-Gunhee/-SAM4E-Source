#include ".\cpu.h"

ISR_ST_USART_FIFO volatile *pusart_fifo=0;

char US_Buff[32];
int US_msg_lengh = 0;
int a = 0;


void Init_USART()
{
	USART1->US_BRGR=(SystemCoreClock)/(16*115200);
	
	USART1->US_MR|=((1<<6)|(1<<7));
	USART1->US_MR|=(1<<11);
	USART1->US_MR&=~((1<<10)|(1<<9));
	USART1->US_MR&=~((1<<13)|(1<<12));
	USART1->US_MR&=~((1<<15)|(1<<14));
	
	NVIC_ClearPendingIRQ(USART1_IRQn);
	NVIC_EnableIRQ(USART1_IRQn);
	
	USART1->US_CR=((1<<US_TXEN)|(1<<US_RXEN));
	//	USART1->US_IER=US_IER_RXRDY;
}

void US_SendByte(uint32_t ch)
{
	while((USART1->US_CSR&(1<<US_TXRDY))==0);
	USART1->US_THR=ch;
}

int US_SendString(const char *S_str)
{
	while(*S_str)
	{
		US_SendByte(*S_str);
		S_str++;
	}
	return 0;
}

void enter()
{
	US_SendByte('\r');
	US_SendByte('\n');
}

int US_GetByte(uint8_t *data)
{
	int szbuff=0;
	
	if(ISR_Usart1_Buff.Head==ISR_Usart1_Buff.Tail)return 0;
	pusart_fifo=&ISR_Usart1_Buff;
	szbuff=ISR_SZ_US1_BUFF;
	
	if(pusart_fifo->Head>=szbuff){pusart_fifo->Head=0;}
	
	return 1;
}

int US_GetString(uint8_t *data)
{
	int szbuff=0;
	
	if(ISR_Usart1_Buff.Head==ISR_Usart1_Buff.Tail)return 0;
	pusart_fifo=&ISR_Usart1_Buff;
	szbuff=ISR_SZ_US1_BUFF;
	
	if(pusart_fifo->Head>=szbuff){pusart_fifo->Head=0;}
	
	if (pusart_fifo->Buff[pusart_fifo->Head]==13)
	{
		*data=pusart_fifo->Buff[pusart_fifo->Head++];
		enter();
		US_SendString("Input String : ");
		for (volatile int a =0; a<=US_msg_lengh-1;a++)
		{
			US_SendByte(US_Buff[a]);
			if (a==US_msg_lengh-1)
			{
				enter();
//				US_msg_status = 1;
				break;
			}
		}
	}
	else
	{
		*data=pusart_fifo->Buff[pusart_fifo->Head++];
		US_Buff[US_msg_lengh]=(char)*data;
		US_SendByte(US_Buff[US_msg_lengh++]);
	}
	return 1;
}
