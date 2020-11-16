#include ".\cpu.h"

uint32_t count = 1;
uint32_t duty = 0;
uint32_t period = 100;
uint32_t end = 0;
uint16_t point = 1;

void PWMInit(void)
{
	PWM->PWM_DIS=((1<<CH0)|(1<<CH1));
	PWM->PWM_CLK=((1<<Clock_SelectA)|(0<<Perip_CLK_A)|(1<<Clock_SelectB)|(0<<Perip_CLK_B));
	PWM->PWM_CH_NUM[0].PWM_CMR=((11<<CH_PRE)|(0<<ALIGN_LEFT)|(0<<Polarty_Low)|(1<<CES));
	PWM->PWM_CH_NUM[0].PWM_CPRD=(period<<CPRD);
	PWM->PWM_CH_NUM[0].PWM_CDTY=(duty<<CDTY);
	PWM->PWM_CH_NUM[0].PWM_CMR=((1<<DTE));
	PWM->PWM_CH_NUM[0].PWM_DT=((3<<DTH)|(3<<DTL));
	PWM->PWM_CH_NUM[1].PWM_CMR=((12<<CH_PRE)|(0<<ALIGN_LEFT)|(0<<Polarty_Low)|(1<<CES));
	PWM->PWM_CH_NUM[1].PWM_CPRD=(period<<CPRD);
	PWM->PWM_CH_NUM[1].PWM_CDTY=(duty<<CDTY);
	PWM->PWM_CH_NUM[1].PWM_CMR=((1<<DTE));
	PWM->PWM_CH_NUM[1].PWM_DT=((3<<DTH)|(3<<DTL));
	PWM->PWM_FMR|=((1<<fault_in)|(1<<fault_cle)|(1<<fault_fil));
	PWM->PWM_OS|=((0<<OSL0)|(0<<OSL1)|(0<<OSH0)|(0<<OSH1));
	PWM->PWM_SCM=((1<<Syn_Ch0)|(1<<Syn_Ch1)|(1<<UPDM));
	PWM->PWM_SCUC|=(1<<UPDULOCK);
	PWM->PWM_IER1=((1<<CHDI0)|(1<<CHDI1));
	PWM->PWM_ENA=((1<<CH0)|(1<<CH1));
	
	InitRealTime();
}