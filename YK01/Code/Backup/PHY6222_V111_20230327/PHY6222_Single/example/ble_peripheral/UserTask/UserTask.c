#include "UserTask.h"
#include "OSAL.h"
#include "OSAL_Tasks.h"
#include "flash.h"
#include "log.h"
#include "param.h"
#include "fs.h"


uint8 UserBase1_TaskID;
uint8 UserBase2_TaskID;








/*****************************************************************************
@brief	用户任务1开始之前的相关初始化  
@param 	
	task_id ：任务ID
@retval	    
******************************************************************************/
void UserBase1_Init( uint8 task_id )
{
	UserBase1_TaskID = task_id;

//	cmt2300a_int_init();
//	cmt2300a_bsp_init();

//	inp_delay_us(10);

	osal_start_reload_timer(UserBase1_TaskID, USER_TASK1_EVENT0, 5000);
}


/*****************************************************************************
@brief	用户任务2开始之前的相关初始化      
@param 	
	task_id ：任务ID
@retval	    
******************************************************************************/
void UserBase2_Init( uint8 task_id )
{
	UserBase2_TaskID = task_id;

	osal_start_reload_timer(UserBase2_TaskID, USER_TASK2_EVENT0, 250);
}


/*****************************************************************************
@brief	用户任务1事件处理
@param 	
@retval	    
******************************************************************************/
uint16 UserBase1_ProcessEvent( uint8 task_id, uint16 events )
{
	VOID task_id;
	uint8_t pucWriteBuffer[20] = {0x0A, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x16, 0x00, 0x3E, 0x00};
	uint8_t pucItemBuffer[20] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
	uint8_t pucReadBuffer[40] = {0};
	if( events & USER_TASK1_EVENT0 )
	{
//		radio433_process();
		
		LOG("#1\n");
		hal_flash_erase_sector(FLASH_ADDR_UCDS_SEC1);
		LOG("#2\n");
		vGeneralFlashWrite(RELATIVE_FLASH_ADDR_UCDS_PARAM, 16, pucWriteBuffer);
		LOG("#3\n");
		vGeneralFlashWrite(RELATIVE_FLASH_ADDR_UCDS_PARAM + DATA_ITEM_MAX_SIZE, 16, pucWriteBuffer);
		LOG("#4\n");
//		vGeneralFlashRead(FLASH_ADDR_UCDS_SEC1, 20, pucReadBuffer);
//		for(uint8_t i = 0; i < 20; i++)
//		{
//			LOG("buff :%d \n",pucReadBuffer[i]);
//		}
		ucParamInit(vGeneralFlashRead, vGeneralFlashWrite);
		
		ucParamWrite(PV_DATA4, pucItemBuffer, 6);
		
		ucParamRead(PV_DATA4);

		return (events ^ USER_TASK1_EVENT0);
	}

	return 0;
}


/*****************************************************************************
@brief	用户任务2事件处理    
@param 	
	task_id ：任务ID
@retval	    
******************************************************************************/
uint16 UserBase2_ProcessEvent( uint8 task_id, uint16 events )
{
	VOID task_id;
//	uint8_t i = 0;
	
//	extern ParamItem_t xParamHandler;
//	static uint8_t a = 0;
//	if(a == 0)
//	{
//		a = 1;
//		read_deviceID();
//		LOG_CHIP_ID();
//		LOG("#1\n");
//		flash_sector_erase(FLASH_ADDR_UCDS_PARAM);
//		LOG("#2\n");
//		flash_write_ucds_block_byte(0, 12, xParamHandler.pucReadBuffer);
//		LOG("#3\n");
//		ucParamInit(flash_read_ucds_block_byte, flash_write_ucds_block_byte);
//	}
	
	
	if( events & USER_TASK2_EVENT0 )
	{
//		Jump_Frequency_Process();

		return (events ^ USER_TASK2_EVENT0);
	}

	if( events & USER_TASK2_EVENT1 )
	{
//		uart_data_processing = 1;
//		
//		switch(uart_temp_value)
//		{
//			case 1:
//				cli_cmd_list[0].cmd_hdlr(0, NULL);
//				break;
//			case 2:
//				cli_cmd_list[1].cmd_hdlr(1, NULL);
//				break;
//			case 3:
//				cli_cmd_list[2].cmd_hdlr(2, NULL);
//				break;
//			case 4:
//				cli_cmd_list[3].cmd_hdlr(3, NULL);
//				break;
//			case 5:
//				cli_cmd_list[4].cmd_hdlr(4, NULL);
//				break;
//			default:
//				break;
//		}
//		
//		osal_memcpy(proc_buff, cmdstr, cmdlen);	//拷贝
//		for(i = 0; i < 5; i++)
//		{
//			if(hal_my_strcmp(proc_buff, cli_cmd_list[i].cmd))
//			{
//				cli_cmd_list[i].cmd_hdlr(i, NULL);
//				break;
//			}
//		}
//		memset(cmdstr, 0, AT_CMD_LENGTH_MAX);		//清除
//		memset(proc_buff, 0, AT_CMD_LENGTH_MAX);		//清除
//		
//		uart_temp_value = 0;
//		uart_data_processing = 0;
		
		return (events ^ USER_TASK2_EVENT1);
	}

	return 0;
}


























