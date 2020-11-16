/*
 * USART.h
 *
 * Created: 2019-05-14 오후 3:18:08
 *  Author: ghhan
 */ 


#ifndef USART_H_
#define USART_H_

#define US_RX1 21
#define US_TX1 22
#define US_TXRDY 1
#define US_RXRDY 0
#define US_TXEN 6
#define US_RXEN 4
#define US_CD 0

#define US_RST(tx,rx,state) ((((tx)&1)<<3)|(((rx)&1)<<2)| (((state)&1)<<8))

#define US_RST_RXSTATUS(puart) (puart)->US_CR=US_RST(0,0,1)
#define US_GET_RXDATA(puart) ((puart)->US_RHR)
#define US_SET_TXDATA(puart,val) (puart)->US_THR=(val)
#define US_GET_STATUS(puart)  ((puart)->US_CSR)
#define US_GET_TXRDY(puart)	((((puart)->US_CSR&(1<<1))==0)? 0:1)
#define US_GET_TXEMPTY(puart)	((((puart)->US_CSR&(1<<9))==0)? 0:1)

extern char US_Buff[32];
extern int US_msg_lengh;

void Init_USART();
void US_SendByte(uint32_t ch);
int US_GetByte(uint8_t *data);
int US_GetString(uint8_t *data);
int US_SendString(const char *S_str);
void enter();



#endif /* USART_H_ */