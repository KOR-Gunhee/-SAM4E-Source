#include ".\cpu.h"

#define CAN_MIN_TQ_NUM             8
#define CAN_MAX_TQ_NUM             25

#define CAN_BAUDRATE_MAX_DIV       128

typedef struct {
	uint8_t uc_tq;      /**< CAN_BIT_SYNC + uc_prog + uc_phase1 + uc_phase2
	                       = uc_tq, 8 <= uc_tq <= 25. */
	uint8_t uc_prog;    /**< Propagation segment, (3-bits + 1), 1~8; */
	uint8_t uc_phase1;  /**< Phase segment 1, (3-bits + 1), 1~8; */
	uint8_t uc_phase2;  /**< Phase segment 2, (3-bits + 1), 1~8, CAN_BIT_IPT
	                       <= uc_phase2; */
	uint8_t uc_sjw;     /**< Resynchronization jump width, (2-bits + 1),
	                       min(uc_phase1, 4); */
	uint8_t uc_sp;      /**< Sample point value, 0~100 in percent. */
} can_bit_timing_t;

const can_bit_timing_t can_bit_time[] = {
	{ 8, (2 + 1), (1 + 1), (1 + 1), (2 + 1), 75},
	{ 9, (1 + 1), (2 + 1), (2 + 1), (1 + 1), 67},
	{10, (2 + 1), (2 + 1), (2 + 1), (2 + 1), 70},
	{11, (3 + 1), (2 + 1), (2 + 1), (3 + 1), 72},
	{12, (2 + 1), (3 + 1), (3 + 1), (3 + 1), 67},
	{13, (3 + 1), (3 + 1), (3 + 1), (3 + 1), 77},
	{14, (3 + 1), (3 + 1), (4 + 1), (3 + 1), 64},
	{15, (3 + 1), (4 + 1), (4 + 1), (3 + 1), 67},
	{16, (4 + 1), (4 + 1), (4 + 1), (3 + 1), 69},
	{17, (5 + 1), (4 + 1), (4 + 1), (3 + 1), 71},
	{18, (4 + 1), (5 + 1), (5 + 1), (3 + 1), 67},
	{19, (5 + 1), (5 + 1), (5 + 1), (3 + 1), 68},
	{20, (6 + 1), (5 + 1), (5 + 1), (3 + 1), 70},
	{21, (7 + 1), (5 + 1), (5 + 1), (3 + 1), 71},
	{22, (6 + 1), (6 + 1), (6 + 1), (3 + 1), 68},
	{23, (7 + 1), (7 + 1), (6 + 1), (3 + 1), 70},
	{24, (6 + 1), (7 + 1), (7 + 1), (3 + 1), 67},
	{25, (7 + 1), (7 + 1), (7 + 1), (3 + 1), 
	8}
};

uint32_t can_mailbox_get_status(Can *p_can, uint8_t uc_index)
{
	return (p_can->CAN_MB[uc_index].CAN_MSR);
}

uint32_t can_get_status(Can *p_can)
{
	return (p_can->CAN_SR);
}

void can_enable(Can *p_can)
 
{
	p_can->CAN_MR |= CAN_MR_CANEN;
}

void can_disable(Can *p_can)
{
	p_can->CAN_MR &= ~CAN_MR_CANEN;
}

void can_mailbox_init(Can *p_can, can_mb_conf_t *p_mailbox)
{
	uint8_t uc_index;

	uc_index = (uint8_t)p_mailbox->ul_mb_idx;
	//사서함의 개체 유형을 확인하십시오. 사서함을 사용하지 않도록 설정 한 경우 전체 사서함을 다시 설정합니다.
	if (!p_mailbox->uc_obj_type) {
		p_can->CAN_MB[uc_index].CAN_MMR = 0;
		p_can->CAN_MB[uc_index].CAN_MAM = 0;
		p_can->CAN_MB[uc_index].CAN_MID = 0;
		p_can->CAN_MB[uc_index].CAN_MDL = 0;
		p_can->CAN_MB[uc_index].CAN_MDH = 0;
		p_can->CAN_MB[uc_index].CAN_MCR = 0;
		return;
	}

	//전송 모드에서 우선 순위를 설정하십시오.
	p_can->CAN_MB[uc_index].CAN_MMR = (p_can->CAN_MB[uc_index].CAN_MMR &
			~CAN_MMR_PRIOR_Msk) |
			(p_mailbox->uc_tx_prio << CAN_MMR_PRIOR_Pos);

	//다른 모드에서 사서함의 메시지 ID 및 메시지 허용 마스크를 설정합니다.
	if (p_mailbox->uc_id_ver) {
		p_can->CAN_MB[uc_index].CAN_MAM = p_mailbox->ul_id_msk |
				CAN_MAM_MIDE;
		p_can->CAN_MB[uc_index].CAN_MID = p_mailbox->ul_id |
				CAN_MAM_MIDE;
	} else {
		p_can->CAN_MB[uc_index].CAN_MAM = p_mailbox->ul_id_msk;
		p_can->CAN_MB[uc_index].CAN_MID = p_mailbox->ul_id;
	}

	//다섯 가지 모드 중 하나에서 사서함을 설정하십시오.
	p_can->CAN_MB[uc_index].CAN_MMR = (p_can->CAN_MB[uc_index].CAN_MMR &
			~CAN_MMR_MOT_Msk) | 
			(p_mailbox->uc_obj_type << CAN_MMR_MOT_Pos);
}

uint32_t can_mailbox_write(Can *p_can, can_mb_conf_t *p_mailbox)
{
	uint32_t ul_status;
	uint8_t uc_index;

	uc_index = (uint8_t)p_mailbox->ul_mb_idx;
	//먼저 사서함 상태를 읽고 사서함이 준비 상태인지 확인합니다.
	p_mailbox->ul_status = can_mailbox_get_status(p_can, uc_index);
	ul_status = p_mailbox->ul_status;
	if (!(ul_status & CAN_MSR_MRDY)) {
		return CAN_MAILBOX_NOT_READY;
	}

	//쓰기 전송 식별자.
	if (p_mailbox->uc_id_ver) {
		p_can->CAN_MB[uc_index].CAN_MID = p_mailbox->ul_id |
		CAN_MAM_MIDE;
		} else {
		p_can->CAN_MB[uc_index].CAN_MID = p_mailbox->ul_id;
	}

	//송신 데이터를 메일 박스 데이터 레지스터에 씁니다.
	p_can->CAN_MB[uc_index].CAN_MDL = p_mailbox->ul_datal;
	if (p_mailbox->uc_length > 4) {
		p_can->CAN_MB[uc_index].CAN_MDH = p_mailbox->ul_datah;
	}

	//송신 데이터 길이를 메일 박스 제어 레지스터에 쓴다.
	p_can->CAN_MB[uc_index].CAN_MCR = CAN_MCR_MDLC(p_mailbox->uc_length);

	return CAN_MAILBOX_TRANSFER_OK;
}

static uint32_t can_set_baudrate(Can *p_can, uint32_t ul_mck,uint32_t ul_baudrate)
{
	uint8_t uc_tq;
	uint8_t uc_prescale;
	uint32_t ul_mod;
	uint32_t ul_cur_mod;
	can_bit_timing_t *p_bit_time;

	//baudrate prescale이 최대 나눗셈 값보다 큰지 확인하십시오.
	if (((ul_mck + (ul_baudrate * CAN_MAX_TQ_NUM * 1000 - 1)) /
	(ul_baudrate * CAN_MAX_TQ_NUM * 1000)) >
	CAN_BAUDRATE_MAX_DIV) {
		return 0;
	}

	//입력 MCK가 너무 작은지 확인하십시오

	if ((ul_mck / 2)  < ul_baudrate * CAN_MIN_TQ_NUM * 1000) {
		return 0;
	}

	//최소 시간 퀀텀으로 초기화하십시오.
	uc_tq = CAN_MIN_TQ_NUM;

	//나머지를 최대 값으로 초기화하십시오. 나머지가 0 일 때, 올바른 TQ 번호를 얻으십시오.
	ul_mod = 0xffffffff;
	//baudrate에 따라 대략적인 Time Quantum을 찾으십시오.
	for (uint8_t i = CAN_MIN_TQ_NUM; i <= CAN_MAX_TQ_NUM; i++) {
		if ((ul_mck / (ul_baudrate * i * 1000)) <=
		CAN_BAUDRATE_MAX_DIV) {
			ul_cur_mod = ul_mck % (ul_baudrate * i * 1000);
			if (ul_cur_mod < ul_mod) {
				ul_mod = ul_cur_mod;
				uc_tq = i;
				if (!ul_mod) {
					break;
				}
			}
		}
	}

	//baudrate prescale 값을 계산하십시오
	uc_prescale = ul_mck / (ul_baudrate * uc_tq * 1000);
	if (uc_prescale < 2) {
		return 0;
	}

	//올바른 CAN BIT 타이밍 그룹 가져 오기
	p_bit_time = (can_bit_timing_t *)&can_bit_time[uc_tq - CAN_MIN_TQ_NUM];

	//CANBR 레지스터를 수정하기 전에 CAN 컨트롤러를 비활성화하십시오.
	can_disable(p_can);

	//CAN 통신 속도 레지스터에 기록하십시오.
	p_can->CAN_BR = CAN_BR_PHASE2(p_bit_time->uc_phase2 - 1) |
	CAN_BR_PHASE1(p_bit_time->uc_phase1 - 1) |
	CAN_BR_PROPAG(p_bit_time->uc_prog - 1) |
	CAN_BR_SJW(p_bit_time->uc_sjw - 1) |
	CAN_BR_BRP(uc_prescale - 1);
	return 1;
}

void can_mailbox_send_transfer_cmd(Can *p_can, can_mb_conf_t *p_mailbox)
{
	uint8_t uc_index;

	uc_index = (uint8_t)p_mailbox->ul_mb_idx;

	p_can->CAN_MB[uc_index].CAN_MCR = CAN_MCR_MTCR | CAN_MCR_MDLC(p_mailbox->uc_length);;
}

uint32_t can_mailbox_read(Can *p_can, can_mb_conf_t *p_mailbox)
{
	uint32_t ul_status;
	uint8_t uc_index;
	uint32_t ul_retval;

	ul_retval = 0;
	uc_index = (uint8_t)p_mailbox->ul_mb_idx;
	ul_status = p_mailbox->ul_status;

	/* Check whether there is overwriting happening in Receive with
	 * Overwrite mode,
	 * or there're messages lost in Receive mode. */
	if ((ul_status & CAN_MSR_MRDY) && (ul_status & CAN_MSR_MMI)) {
		ul_retval = 0x02;
	}

	/* Read the message family ID. */
//	p_mailbox->ul_fid = p_can->CAN_MB[uc_index].CAN_MFID &
//			CAN_MFID_MFID_Msk; 

	/* Read received data length. */
	p_mailbox->uc_length
		= (ul_status & CAN_MSR_MDLC_Msk) >> CAN_MSR_MDLC_Pos;

	/* Read received data. */
	p_mailbox->ul_datal = p_can->CAN_MB[uc_index].CAN_MDL;
	if (p_mailbox->uc_length > 4) {
		p_mailbox->ul_datah = p_can->CAN_MB[uc_index].CAN_MDH;
	}

	/* Read the mailbox status again to check whether the software needs to
	 * re-read mailbox data register. */
	p_mailbox->ul_status = p_can->CAN_MB[uc_index].CAN_MSR;
	ul_status = p_mailbox->ul_status;
	if (ul_status & CAN_MSR_MMI) {
		ul_retval |= 0x04;
	} else {
		ul_retval |= 0;
	}

	/* Enable next receive process. */
	can_mailbox_send_transfer_cmd(p_can, p_mailbox);

	return ul_retval;
}

void can_reset_all_mailbox(Can *p_can)
{
	can_mb_conf_t mb_config_t;

	/* Set the mailbox object type parameter to disable the mailbox. */
	mb_config_t.uc_obj_type = CAN_MB_DISABLE_MODE;

	for (uint8_t i = 0; i < CANMB_NUMBER; i++) {
		mb_config_t.ul_mb_idx = i;
		can_mailbox_init(p_can, &mb_config_t);
	}
}

void SEND_MSGBOX(Can *p_can, uint8_t uc_mask)
{
	uint32_t ul_reg;

	ul_reg = p_can->CAN_TCR & ((uint32_t) ~GLOBAL_MAILBOX_MASK);
	p_can->CAN_TCR = ul_reg | uc_mask;
}

uint32_t can_init(Can *p_can, uint32_t ul_mck, uint32_t ul_baudrate)
{
	uint32_t ul_flag;
	uint32_t ul_tick;

	//CAN 모듈의 전송 속도를 초기화하십시오.
	ul_flag = can_set_baudrate(p_can, ul_mck, ul_baudrate);
	if (ul_flag == 0) {
		return 0;
	}

	//8개 사서함을 재설정합니다.
	can_reset_all_mailbox(p_can);

	//CAN 컨트롤러를 활성화합니다.
	can_enable(p_can);

	//CAN이 버스 활동과 동기화 될 때까지 기다리십시오.
	ul_flag = 0;
	ul_tick = 0;
	while (!(ul_flag & CAN_SR_WAKEUP) && (ul_tick < 100000)) {

		ul_flag = can_get_status(p_can);
		ul_tick++;
	}

	//타임 아웃 또는 CAN 모듈이 버스와 동기화되었습니다.
	if (100000 == ul_tick) {
		return 0;
		} else {
		return 1;
	}
}