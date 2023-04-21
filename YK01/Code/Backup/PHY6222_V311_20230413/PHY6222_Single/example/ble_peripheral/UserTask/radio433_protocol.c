#include <stdio.h>
#include <string.h>
#include "OSAL.h"
#include "OSAL_Tasks.h"
#include "clock.h"

//#include "common.h"
#include "drv_cmt2300a.h"
#include "radio433_protocol.h"
#include "radio.h"
#include "log.h"
#include "UserTask.h"
#include "frequency_config_table.h"





/*******************************************************
; Mode                      = Advanced
; Part Number               = CMT2300A
; Frequency                 = 433.920 MHz
; Xtal Frequency            = 26.0000 MHz
; Demodulation              = GFSK
; AGC                       = On
; Data Rate                 = 9.6 kbps
; Deviation                 = 20.0 kHz
; Tx Xtal Tol.              = 20 ppm
; Rx Xtal Tol.              = 20 ppm
; TRx Matching Network Type = 20 dBm
; Tx Power                  = +20 dBm
; Gaussian BT               = 0.5
; Bandwidth                 = Auto-Select kHz
; Data Mode                 = Packet
; Whitening                 = Disable
; Packet Type               = Fixed Length
; Payload Length            = 32
*********************************************************/


DATA_433_S gs_433Data[CMD_MAX];
timer_sequence_structure decode_buff;
volatile uint32_t neg_now_time = 0;
volatile uint32_t pos_now_time = 0;
volatile uint8_t interrupt_lock_flag = 0;
volatile uint8_t start_scan_flag = 0;
volatile uint8_t receive_flag = 0;
volatile uint16_t current_freq = 0;
volatile uint32_t current_key = 0;

uint8_t frequency_bank_list[FREQ_SUPPORT_MAX][8] = 	{ FREQUENCY_BANK_OF_301,		
																											FREQUENCY_BANK_OF_302,	
																											FREQUENCY_BANK_OF_303,
																											FREQUENCY_BANK_OF_304,
																											FREQUENCY_BANK_OF_305,	
																											FREQUENCY_BANK_OF_306,
																											FREQUENCY_BANK_OF_307,	
																											FREQUENCY_BANK_OF_308,	
																											FREQUENCY_BANK_OF_309,	
																											FREQUENCY_BANK_OF_310,
	
																											FREQUENCY_BANK_OF_311,		
																											FREQUENCY_BANK_OF_312,	
																											FREQUENCY_BANK_OF_313,
																											FREQUENCY_BANK_OF_314,
																											FREQUENCY_BANK_OF_315,	
																											FREQUENCY_BANK_OF_316,
																											FREQUENCY_BANK_OF_317,	
																											FREQUENCY_BANK_OF_318,	
																											FREQUENCY_BANK_OF_319,	
																											FREQUENCY_BANK_OF_320,
	
																											FREQUENCY_BANK_OF_321,		
																											FREQUENCY_BANK_OF_322,	
																											FREQUENCY_BANK_OF_323,
																											FREQUENCY_BANK_OF_324,
																											FREQUENCY_BANK_OF_325,	
																											FREQUENCY_BANK_OF_326,
																											FREQUENCY_BANK_OF_327,	
																											FREQUENCY_BANK_OF_328,	
																											FREQUENCY_BANK_OF_329,	
																											FREQUENCY_BANK_OF_330,
																											
																											FREQUENCY_BANK_OF_419,		
																											FREQUENCY_BANK_OF_420,	
																											FREQUENCY_BANK_OF_421,	
																											FREQUENCY_BANK_OF_422,		
																											FREQUENCY_BANK_OF_423,		
																											FREQUENCY_BANK_OF_424,		
																											FREQUENCY_BANK_OF_425,		
																											FREQUENCY_BANK_OF_426,		
																											FREQUENCY_BANK_OF_427,		
																											FREQUENCY_BANK_OF_428,
																											
																											FREQUENCY_BANK_OF_429,		
																											FREQUENCY_BANK_OF_430,	
																											FREQUENCY_BANK_OF_431,	
																											FREQUENCY_BANK_OF_432,		
																											FREQUENCY_BANK_OF_433,		
																											FREQUENCY_BANK_OF_434,		
																											FREQUENCY_BANK_OF_435,		
																											FREQUENCY_BANK_OF_436,		
																											FREQUENCY_BANK_OF_437,		
																											FREQUENCY_BANK_OF_438,
																											
																											FREQUENCY_BANK_OF_439,		
																											FREQUENCY_BANK_OF_440,	
																											FREQUENCY_BANK_OF_441,	
																											FREQUENCY_BANK_OF_442,		
																											FREQUENCY_BANK_OF_443,		
																											FREQUENCY_BANK_OF_444,		
																											FREQUENCY_BANK_OF_445,		
																											FREQUENCY_BANK_OF_446,		
																											FREQUENCY_BANK_OF_447,		
																											FREQUENCY_BANK_OF_448,
};

uint8_t tx_bank_list[FREQ_SUPPORT_MAX][11] = { 
																								TX_BANK_OF_301,
																								TX_BANK_OF_302,
																								TX_BANK_OF_303,	
																								TX_BANK_OF_304,	
																								TX_BANK_OF_305,	
																								TX_BANK_OF_306,	
																								TX_BANK_OF_307,	
																								TX_BANK_OF_308,	
																								TX_BANK_OF_309,	
																								TX_BANK_OF_310,
	
																								TX_BANK_OF_311,
																								TX_BANK_OF_312,
																								TX_BANK_OF_313,	
																								TX_BANK_OF_314,	
																								TX_BANK_OF_315,	
																								TX_BANK_OF_316,	
																								TX_BANK_OF_317,	
																								TX_BANK_OF_318,	
																								TX_BANK_OF_319,	
																								TX_BANK_OF_320,	
																								
																								TX_BANK_OF_321,
																								TX_BANK_OF_322,
																								TX_BANK_OF_323,	
																								TX_BANK_OF_324,	
																								TX_BANK_OF_325,	
																								TX_BANK_OF_326,	
																								TX_BANK_OF_327,	
																								TX_BANK_OF_328,	
																								TX_BANK_OF_329,	
																								TX_BANK_OF_330,
																								
																								TX_BANK_OF_419, 	
																								TX_BANK_OF_420, 	
																								TX_BANK_OF_421, 
																								TX_BANK_OF_422, 	
																								TX_BANK_OF_423, 	
																								TX_BANK_OF_424, 
																								TX_BANK_OF_425, 	
																								TX_BANK_OF_426, 
																								TX_BANK_OF_427, 	
																								TX_BANK_OF_428,
																								
																								TX_BANK_OF_429, 	
																								TX_BANK_OF_430, 	
																								TX_BANK_OF_431, 
																								TX_BANK_OF_432, 	
																								TX_BANK_OF_433, 	
																								TX_BANK_OF_434, 
																								TX_BANK_OF_435, 	
																								TX_BANK_OF_436, 
																								TX_BANK_OF_437, 	
																								TX_BANK_OF_438,
																								
																								TX_BANK_OF_439, 	
																								TX_BANK_OF_440, 	
																								TX_BANK_OF_441, 
																								TX_BANK_OF_442, 	
																								TX_BANK_OF_443, 	
																								TX_BANK_OF_444, 
																								TX_BANK_OF_445, 	
																								TX_BANK_OF_446, 
																								TX_BANK_OF_447, 	
																								TX_BANK_OF_448,
};

jump_frequency_register frequency_set_register;




/*****************************************************************************
@brief	上升沿中断处理函数       
@param    
@retval	    
******************************************************************************/
static void dout_pos_edge_cb(GPIO_Pin_e pin, IO_Wakeup_Pol_e type)
{
	u32 temp = 0;
	
	if(start_scan_flag == 0)
	{
		return;
	}
	
	pos_now_time = (TIME_BASE - ((AP_TIM3->CurrentCount) >> 2));//getMcuPrecisionCount();
	
	if(!interrupt_lock_flag)
	{
		if(decode_buff.recode_start)
		{
			temp = RETURN_DIFFERENCE(pos_now_time, neg_now_time);
			
			if(decode_buff.buff1_flag == 0)	//buff1_flag的标志等于0，说明第一个buff还未记录完
			{
				decode_buff.rec_buff1[decode_buff.recode_index++] = temp;		
			}
			else
			{
				decode_buff.rec_buff2[decode_buff.recode_index++] = temp;	
			}
#if	HIGH_START_CHECK_ENABLE
			if((decode_buff.recode_index + 1) == BUFF_LEN)	//上升沿记录完成
			{
				if(decode_buff.buff1_flag == 0)
				{
					/* 这里说明第一个buff记录完成了,然后等待下一个起始信号，记录下一个数据 */
					decode_buff.buff1_flag = 1;
					decode_buff.recode_start = 0;
					decode_buff.recode_index = 0;
				}
				else
				{
					/* 这里说明两个buff都记录完成了，发送消息通知任务进行处理 */
					decode_buff.buff2_flag = 1;
					decode_buff.recode_index = 0;
					decode_buff.recode_start = 0;
					interrupt_lock_flag = 1;	//锁住，在任务对数据处理完之前，不对后面的数据做响应
					osal_set_event(UserBase1_TaskID, USER_TASK1_EVENT0);
				}
			}
#endif
		}
		else
		{
			/* 低电平起始信号 */
			temp = RETURN_DIFFERENCE(pos_now_time, neg_now_time);

			if((temp > HIGH_START_SIGNAL_MIN) && (temp < HIGH_START_SIGNAL_MAX))				//是否为起始信号范围
			{ 
				if(decode_buff.buff1_flag == 0)				//buff空间是否还未使用
				{
					decode_buff.recode_start = 1;				//开始记录
					decode_buff.buff1_start_len = temp;	//保存起始信号长度
				}
				else
				{
					decode_buff.recode_start = 1;				//开始记录,进入到这里开始记录第二个数据
					decode_buff.buff2_start_len = temp;
				}
			}
		}
	}
}


/*****************************************************************************
@brief	下降沿中断处理函数       
@param    
@retval	    
******************************************************************************/
static void dout_neg_edge_cb(GPIO_Pin_e pin, IO_Wakeup_Pol_e type)
{
	u32 temp;
	
	if(start_scan_flag == 0)
	{
		return;
	}
	
	neg_now_time = (TIME_BASE - ((AP_TIM3->CurrentCount) >> 2));//getMcuPrecisionCount();

	if(!interrupt_lock_flag)
	{	
		if(decode_buff.recode_start)
		{	
			temp = RETURN_DIFFERENCE(neg_now_time, pos_now_time);	//计算差值
			
			if(decode_buff.buff1_flag == 0)	//buff1_flag的标志等于0，说明第一个buff还未记录完
			{
				decode_buff.rec_buff1[decode_buff.recode_index++] = temp;		
			}
			else
			{
				decode_buff.rec_buff2[decode_buff.recode_index++] = temp;		
			}
			
			if((decode_buff.recode_index + 1) == BUFF_LEN)	//下降沿记录完成
			{
				if(decode_buff.buff1_flag == 0)
				{
					/* 这里说明第一个buff记录完成了,然后等待下一个起始信号，记录下一个数据 */
					decode_buff.buff1_flag = 1;
					decode_buff.recode_start = 0;
					decode_buff.recode_index = 0;
				}
				else
				{
					/* 这里说明两个buff都记录完成了，发送消息通知任务进行处理 */
					decode_buff.buff2_flag = 1;
					decode_buff.recode_index = 0;
					decode_buff.recode_start = 0;
					interrupt_lock_flag = 1;	//锁住，在任务对数据处理完之前，不对后面的数据做响应
					osal_set_event(UserBase1_TaskID, USER_TASK1_EVENT0);
				}
			}
		}
#if	HIGH_START_CHECK_ENABLE
		else
		{
			/* 高电平起始信号 */
			temp = RETURN_DIFFERENCE(neg_now_time, pos_now_time);

			if(temp>HIGH_START_SIGNAL_MIN && temp<HIGH_START_SIGNAL_MAX)					//是否为起始信号范围
			{
				if(decode_buff.buff1_flag == 0)				//buff空间是否还未使用
				{
					decode_buff.recode_start = 1;				//开始记录
					decode_buff.buff1_start_len = temp;	//保存起始信号长度
				}
				else
				{
					decode_buff.recode_start = 1;				//开始记录,进入到这里开始记录第二个数据
					decode_buff.buff2_start_len = temp;
				}
			}
		}
#endif
	}
}

/*****************************************************************************
@brief 	CMT2300A的相关GPIO配置，以及IRQ脚上的电平中断回调函数      
@param 	
@retval	    
******************************************************************************/
void cmt2300a_int_init(void)
{
	CMT_POWER_ON();
	
	hal_gpio_cfg_analog_io(CMT_GPIO2, Bit_DISABLE);
	hal_gpio_fmux(CMT_GPIO2, Bit_DISABLE);
	hal_gpio_pull_set(CMT_GPIO2, WEAK_PULL_UP);
	hal_gpioin_register(CMT_GPIO2, dout_pos_edge_cb, dout_neg_edge_cb);
}



/*****************************************************************************
@brief	配置CMT2300A       
@param    
@retval	    
******************************************************************************/
void cmt2300a_bsp_init(void)
{

	RF_Init();
	RF_Config();
	
	if(FALSE==CMT2300A_IsExist()) 
	{
		PRINT_DEBUG("CMT2300A not found!\r\n");
	}
	else 
	{
		PRINT_DEBUG("CMT2300A ready ok \r\n");
	}
	
	PRINT_DEBUG("CMT2300A Config finished into Rx mode \r\n");
	CMT2300A_GoRx();
}

/*****************************************************************************
@brief	发送指定的buff数据       
@param    
@retval	    
******************************************************************************/
void radio433_protocol_send(u32 buff_start, uint8_t id)
{
	/* start signal */
	cmt_GPIOx_0();
	WaitUs(buff_start);
	for(u8 i=0; i< CYCLE_MAX; i++)
	{
		cmt_spi3_send_bit(buff_start, gs_433Data[id].postTime[i], gs_433Data[id].negaTime[i]);
	}
	
}

/*****************************************************************************
@brief	解码数据发送测试       
@param    
@retval	    
******************************************************************************/
void send_rec_test(uint16_t send_num, uint16_t send_interval)
{
	/* urn transmit mode */
	CMT2300A_GoStby();
	WaitMs(10);	
	CMT2300A_EnableTxDin(TRUE);
	CMT2300A_ConfigTxDin( CMT2300A_TX_DIN_SEL_GPIO1 );
	CMT2300A_GoTx();
	WaitMs(10);	
	for(u8 i=0; i < send_num; i++)
	{
		radio433_protocol_send(decode_buff.start_average_len, 0);
		WaitMs(send_interval);
	}

	/* Turn receive mode */
	CMT2300A_InitGpio();
    CMT2300A_GoStby();
	CMT2300A_EnableTxDin(FALSE);	
	CMT2300A_ConfigGpio( CMT2300A_GPIO2_SEL_DOUT);		
	CMT2300A_GoRx();
	WaitMs(1);
}


/*****************************************************************************
@brief 	将buff中的高低电平数据进行分离保存      
@param    
@retval	    
******************************************************************************/
static bool radio433_translate(DATA_433_S* st433Data, s32* src)
{
	if((src != NULL) && (st433Data != NULL))
	{
		for(u8 i = 0; i < CYCLE_MAX; i++)
		{
			st433Data->postTime[i] = src[2*i];
			st433Data->negaTime[i] = src[2*i + 1];		
		}
		return TRUE;
	}

	return FALSE;
}



static void radio433_save_key(DATA_433_S* st433Data, u8 id)
{
	if(st433Data == NULL)
		return;
	
	memcpy(&gs_433Data[id], st433Data, sizeof(DATA_433_S));
}


/*****************************************************************************
@brief 	收到处理事情消息后对两个buff中的数据进行处理      
@param    
@retval	    
******************************************************************************/
void radio433_process(void)
{
//	uint16_t print_count = 5;
	volatile int32_t *pd1 = decode_buff.rec_buff1;
	volatile int32_t *pd2 = decode_buff.rec_buff2;
	uint8_t error_num = 0;
	uint8_t i = 0;
	int32_t temp = 0;
	uint32_t temp_value = 0;
	DATA_433_S tempSt;
	memset((void*)&tempSt, 0x00, sizeof(DATA_433_S));
	
//#define PRINT_TIME_TAB  1
#if (PRINT_TIME_TAB == 1)
	PRINT_DEBUG("\t\t		Print buff data \r\n");
	PRINT_DEBUG("\t\tBuff 1 start signal:%d\r\n\t\tBuff 2 start signal:%d \r\n", decode_buff.buff1_start_len, decode_buff.buff2_start_len);
	PRINT_DEBUG("\t --------------------------------------- \r\n");
	PRINT_DEBUG("\t|  Buff1 list   |       |  Buff2 list   | \r\n");
	PRINT_DEBUG("\t --------------------------------------- \r\n");
	for(i = 0; i < BUFF_LEN; i += 2)
	{
		PRINT_DEBUG("\t|%d\t|%d\t|\t|%d\t|%d\t| \r\n", decode_buff.rec_buff1[i], decode_buff.rec_buff1[i + 1], decode_buff.rec_buff2[i], decode_buff.rec_buff2[i + 1]);
		PRINT_DEBUG("\t --------------------------------------- \r\n");
	}
#endif
	/* 校验buff中的数据 */
	for(i = 0; i < (BUFF_LEN - 1); i++)	//第49个数据不参与验证
	{
		if(!(((pd1[i] > 200) && (pd1[i] < 600)) || ((pd1[i] > 700) && (pd1[i] < 1500))))  //先验证buff1中保存的时序
		{
			error_num = 1;

			PRINT_DEBUG("\t error_num = %d: buff1[%d] = %d \r\n", error_num, i, pd1[i]);

			
			break;
		}
		else 
		{
			temp = pd1[i] - pd2[i];
			if((temp > 100) || temp < -100)	//然后对比两个buff之间的差值，不能超过200
			{
				error_num = 2;	

				PRINT_DEBUG("\t error_num = %d: buff2[%d] = %d \r\n", error_num, i, pd2[i]);

				decode_buff.key_value = 0;
				break;
			}
			if((i % 2) == 0 && (i < 48)) //最后一个数据位不做值位
			{
				if((pd1[i] > 700) && (pd1[i] < 1500))
				{
					temp_value |= ((u32)0x800000 >> (i / 2));
				}
			}
			
		}
	}
	
	if(error_num == 0)
	{
		/* 关闭定时器 */
		osal_stop_timerEx(UserBase2_TaskID, USER_TASK2_EVENT3);
		/* 接收标志置1 */
		receive_flag = 1;		
		/* 保存接收到的按键值 */
		decode_buff.key_value = temp_value;	
		/* 将buff1和buff2中的数据进行平均，如果想节省时间可以跳过此步，直接保存buff1或者buff2的数据即可 */
		decode_buff.start_average_len = ((decode_buff.buff1_start_len + decode_buff.buff2_start_len) / 2);
		for(i = 0; i < BUFF_LEN; i++)
		{
			decode_buff.ave_buff[i] = ((decode_buff.rec_buff1[i] + decode_buff.rec_buff2[i]) / 2);
		}
		radio433_translate(&tempSt, (s32 *)decode_buff.ave_buff);
		radio433_save_key(&tempSt, 0);
		at_scaned(2, NULL);	//停止扫描
		osal_set_event(UserBase2_TaskID, USER_TASK2_EVENT3);	//主动向任务发送事件
		
#if (PRINT_TIME_TAB == 1)
		PRINT_DEBUG("\tPrint averaged data \r\n");
		for(i = 0; i < 25; i++)
		{
			PRINT_DEBUG("\t --------------------------------------- \r\n");
			PRINT_DEBUG("\t|%d\t|%d\t|\t|%d\t|%d\t| \r\n", i, gs_433Data[0].postTime[i], i, gs_433Data[0].negaTime[i]);
			PRINT_DEBUG("\t --------------------------------------- \r\n");
		}
#endif
		
//		while(1)
//		{
//			send_rec_test(3, 500);
//			WaitMs(3000); 
//			if(print_count++ >= 5)
//			{
//				print_count = 0;
//				current_freq = GET_CURRENT_FREQ_BAND_NUM(frequency_set_register.now_freq_num);
//				LOG("\r\n In course of send key\r\n Current frequency is %d \r\n Key value is %06X \r\n", current_freq, decode_buff.key_value);
//			}
//		}
	}
	


	if(interrupt_lock_flag == 1)
	{
		
		/* 解锁前先对记录进行清空 */
		decode_buff.buff1_flag = 0;
		decode_buff.buff2_flag = 0;
		decode_buff.buff1_start_len = 0;
		decode_buff.buff2_start_len = 0;
		decode_buff.recode_index = 0;
		decode_buff.recode_start = 0;
		
		interrupt_lock_flag = 0;	//任务处理完成，解锁中断数据记录流程
	}
	

}

void jump_frequency_list_init(uint8_t list_value)
{
	frequency_set_register.now_freq_num = list_value;
	frequency_set_register.now_frequency_bank = frequency_bank_list[frequency_set_register.now_freq_num];
	frequency_set_register.now_tx_bank = tx_bank_list[frequency_set_register.now_freq_num];
}

/*****************************************************************************
@brief 	跳频处理,每250ms切换一次      
@param    
@retval	    
******************************************************************************/
void Jump_Frequency_Process(void)
{
	static enum SupportFreqList temp = FREQ_301; //FREQ_434;//FREQ_301;
	static enum SupportFreqList temp_insert = FREQ_314;
	static uint8_t insert = 0;

	if(start_scan_flag == 0)	//是否开始扫描
	{
		return;
	}

	if(interrupt_lock_flag == 0)
	{

		if(temp <= FREQ_448)
		{
			if(insert == 0)
			{
				RF_Jump_Frequency(temp);
				PRINT_DEBUG("\r\n Jump Frequency of %d \r\n", GET_CURRENT_FREQ_BAND_NUM(temp));
				if((temp) >= FREQ_448)
				{
					temp = FREQ_301;
				}
				else
				{
					temp++;
				}
				
				if(!(((temp >= FREQ_314) && (temp <= FREQ_317)) || ((temp <= FREQ_432) && (temp >= FREQ_435))))
				{
					insert = 1;	//不在常用频段时，需要穿插
				}
			}
			else
			{
				RF_Jump_Frequency(temp_insert);
				PRINT_DEBUG("\r\n Jump Frequency of %d \r\n", GET_CURRENT_FREQ_BAND_NUM(temp_insert));
				if((temp_insert) == FREQ_435)
				{
					temp_insert = FREQ_314;
				}
				else if((temp_insert) == FREQ_317)
				{
					temp_insert = FREQ_432;
				}
				else
				{
					temp_insert++;
				}
				
				insert = 0;
			}
		}

	}
	
}

































