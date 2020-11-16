/*
 * PWM.h
 *
 * Created: 2019-04-11 오전 10:39:31
 *  Author: ghhan
 */ 


#ifndef PWM_H_
#define PWM_H_

#define PWMH0 20
#define PWMH1 21
#define PWML0 24
#define PWML1 25
#define Clock_SelectA 0
#define Clock_SelectB 16
#define Perip_CLK_A 8
#define Perip_CLK_B 24
#define CH0 0
#define CH1 1
#define WDT_DIS 15
#define CH_PRE 0
#define ALIGN_LEFT 8
#define Polarty_Low 9
#define CPRD 0
#define CDTY 0
#define DTE 16
#define DTH 0
#define DTL 16
#define Syn_Ch0 0
#define Syn_Ch1 1
#define OSH0 0
#define OSH1 1
#define OSL0 16
#define OSL1 17
#define SPRD 0
#define SPRDM 24
#define fault_in 0
#define fault_cle 8
#define fault_fil 16
#define UPDM 16
#define CDTYUPD 0
#define UPDULOCK 0
#define CHDI1 1
#define CHDI0 0
#define CV 0
#define CVM 24
#define CEN 0
#define CES 10

#define  RealTimeStart() RTT->RTT_MR|=(1<<18)


#endif /* PWM_H_ */