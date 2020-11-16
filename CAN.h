/*
 * CAN.h
 *
 * Created: 2019-02-25 오전 9:29:29
 *  Author: ghhan
 */ 


#ifndef CAN_H_
#define CAN_H_

#define CANRX0 3
#define CANRX1 12
#define CANTX0 2
#define CANTX1 15

#define GLOBAL_MAILBOX_MASK           0x000000ff

/** Define the mailbox mode. */
#define CAN_MB_DISABLE_MODE           0
#define CAN_MB_RX_MODE                1
#define CAN_MB_RX_OVER_WR_MODE        2
#define CAN_MB_TX_MODE                3
#define CAN_MB_CONSUMER_MODE          4
#define CAN_MB_PRODUCER_MODE          5

/** Define CAN mailbox transfer status code. */
#define CAN_MAILBOX_TRANSFER_OK       0     /**< Read from or write into mailbox
	                                           successfully. */
#define CAN_MAILBOX_NOT_READY         0x01  /**< Receiver is empty or
	                                           transmitter is busy. */
#define CAN_MAILBOX_RX_OVER           0x02  /**< Message overwriting happens or
	                                           there're messages lost in
	                                           different receive modes. */
#define CAN_MAILBOX_RX_NEED_RD_AGAIN  0x04  /**< Application needs to re-read
	                                           the data register in Receive with
	                                           Overwrite mode. */

typedef struct {
	uint32_t ul_mb_idx;
	uint8_t uc_obj_type;  /**< Mailbox object type, one of the six different
	                         objects. */
	uint8_t uc_id_ver;    /**< 0 stands for standard frame, 1 stands for
	                         extended frame. */
	uint8_t uc_length;    /**< Received data length or transmitted data
	                         length. */
	uint8_t uc_tx_prio;   /**< Mailbox priority, no effect in receive mode. */
	uint32_t ul_status;   /**< Mailbox status register value. */
	uint32_t ul_id_msk;   /**< No effect in transmit mode. */
	uint32_t ul_id;       /**< Received frame ID or the frame ID to be
	                         transmitted. */
	uint32_t ul_fid;      /**< Family ID. */
	uint32_t ul_datal;
	uint32_t ul_datah;
} can_mb_conf_t;

uint32_t can_mailbox_get_status(Can *p_can, uint8_t uc_index);
uint32_t can_get_status(Can *p_can);
void can_enable(Can *p_can);
void can_disable(Can *p_can);
void can_mailbox_init(Can *p_can, can_mb_conf_t *p_mailbox);
uint32_t can_mailbox_write(Can *p_can, can_mb_conf_t *p_mailbox);
void can_mailbox_send_transfer_cmd(Can *p_can, can_mb_conf_t *p_mailbox);
uint32_t can_mailbox_read(Can *p_can, can_mb_conf_t *p_mailbox);
void can_reset_all_mailbox(Can *p_can);
void SEND_MSGBOX(Can *p_can, uint8_t uc_mask);
uint32_t can_init(Can *p_can, uint32_t ul_mck, uint32_t ul_baudrate);


#endif /* CAN_H_ */