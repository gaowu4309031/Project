#ifndef __RADIO433_PROTOCOL_H
#define __RADIO433_PROTOCOL_H

#include "inp_types.h"

#define HIGH_START_CHECK_ENABLE			0		/* 高电平起始信号检测功能 */

#define BUFF_LEN										50
#define HIGH_START_SIGNAL_MIN				6500
#define HIGH_START_SIGNAL_MAX				13000
#define LOW_START_SIGNAL_MIN				7500
#define LOW_START_SIGNAL_MAX				13000
#define FREQ_SUPPORT_MAX						60
#define FREQ_SUPPORT_MAX_HALF				(FREQ_SUPPORT_MAX / 2)

#define RETURN_DIFFERENCE(X,Y)					((uint32_t)((X > Y)?(X - Y):(1000000 + X - Y)))
#define GET_CURRENT_FREQ_BAND_NUM(X)		(((enum SupportFreqList)X / FREQ_SUPPORT_MAX_HALF)?(X - FREQ_SUPPORT_MAX_HALF + 419):(X + 301))

#define CYCLE_MAX	25

#define CMD_MAX		4

#define BUFF_SIZE	256

#define PACKET_SIZE	(BUFF_SIZE / CYCLE_MAX)

//#define TEST_433
#define cmt_GPIOx_1()      		SET_GPIO_H(CMT_GPIO1)
#define cmt_GPIOx_0()      		SET_GPIO_L(CMT_GPIO1)


typedef struct 
{
	s32 postTime[CYCLE_MAX];
	s32 negaTime[CYCLE_MAX];
	u8	duty;

}DATA_433_S;


typedef struct 
{
	volatile u32	key_value;
	volatile u8	 	recode_index;
	volatile u8 	recode_start;
	volatile u32 	buff1_start_len;
	volatile u32 	buff2_start_len;
	volatile u32 	start_average_len;
	
	volatile u8		buff1_flag;
	volatile u8		buff2_flag;	

	volatile s32 	rec_buff1[BUFF_LEN];
	volatile s32 	rec_buff2[BUFF_LEN];	
	volatile s32 	ave_buff[BUFF_LEN];
	
}timer_sequence_structure;

typedef struct 
{
	uint8_t now_freq_num;
//	uint8_t (*now_cmt_bank)[12];
//	uint8_t (*now_system_bank)[12];
	uint8_t *now_frequency_bank;
//	uint8_t (*now_data_rate_bank)[24];
//	uint8_t (*now_baseband_bank)[29];
	uint8_t *now_tx_bank;
	
} jump_frequency_register;



extern jump_frequency_register frequency_set_register;




extern void cmt2300a_bsp_init(void);
void radio433_protocol_send(u32 buff_start, uint8_t id);
void send_rec_test(uint16_t send_num, uint16_t send_interval);
void radio433_process(void);
void Jump_Frequency_Process(void);
void jump_frequency_list_init(uint8_t list_value);
void cmt2300a_int_init(void);

uint16_t at_version(uint32_t argc, uint8_t *argv[]);
uint16_t at_scaning(uint32_t argc, uint8_t *argv[]);
uint16_t at_scaned(uint32_t argc, uint8_t *argv[]);
uint16_t at_result(uint32_t argc, uint8_t *argv[]);
uint16_t at_sendcopy(uint32_t argc, uint8_t *argv[]);


#endif






