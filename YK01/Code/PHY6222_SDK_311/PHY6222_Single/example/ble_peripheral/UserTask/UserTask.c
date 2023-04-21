#include "UserTask.h"
#include "OSAL.h"
#include "OSAL_Tasks.h"
#include "flash.h"
#include "log.h"
#include "param.h"
#include "fs.h"
#include "led_task.h"

#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"
#include "peripheral.h"
#include "radio433_protocol.h"
#include "common.h"
#include "gapbondmgr.h"
#include "ll.h"
#include "ll_sleep.h"
#include "drv_cmt2300a.h"


/* 数据相关的结构体 */
typedef struct
{
	uint8_t cmd_id;
	uint8_t msg_len;
	uint8_t master_token;
	uint8_t slave_token;
	uint8_t msg_data[BLE_DATA_LEN];
	
} MsgHead;

MsgHead BleDataPack = {0, 0, 0, 0};

/* 协议状态相关的结构体 */
typedef struct
{
	uint8_t auth;
	uint8_t auth_master;	//鉴权成功后主机令牌
	uint8_t auth_slave;		//鉴权成功后从机的令牌
	uint16_t scan_time;		//扫描时间
	
} BleStatusStructure;

BleStatusStructure BleStatus = {0};

uint8_t BleTransmissionBuff[BLE_DATA_LEN];
uint8_t BleTransmissionLen = 0;
uint8_t BleBuffProcessFlag = 0;					//蓝牙数据处理标志
uint8_t BleTurnBuff[BLE_DATA_LEN];

uint8 UserBase1_TaskID;
uint8 UserBase2_TaskID;
uint8 UserBase3_TaskID;
uint8 UserBase4_TaskID;

// Simple Profile Characteristic 4 Configuration Each client has its own
// instantiation of the Client Characteristic Configuration. Reads of the
// Client Characteristic Configuration only shows the configuration for
// that client and writes only affect the configuration of that client.
//static gattCharCfg_t UserProfileChar1Config[GATT_MAX_NUM_CONN];
static gattCharCfg_t UserProfileChar2Config[GATT_MAX_NUM_CONN];
// Characteristic 1 Value
static uint8 UserProfileChar1[255]; 
static uint8 UserProfileChar2[255]; 
CONST uint8 UserProfilechar1UUID[ATT_BT_UUID_SIZE] =
{
    LO_UINT16(USERPROFILE_CHAR1_UUID), HI_UINT16(USERPROFILE_CHAR1_UUID)
};
CONST uint8 UserProfilechar2UUID[ATT_BT_UUID_SIZE] =
{
    LO_UINT16(USERPROFILE_CHAR2_UUID), HI_UINT16(USERPROFILE_CHAR2_UUID)
};
// Simple GATT Profile Service UUID: 0xFFF1
CONST uint8 UserProfileServUUID[ATT_BT_UUID_SIZE] =
{
    LO_UINT16(USERPROFILE_SERV_UUID), HI_UINT16(USERPROFILE_SERV_UUID)
};
static uint8 UserProfileChar1Props =  GATT_PROP_WRITE_NO_RSP ;
static uint8 UserProfileChar2Props =  GATT_PROP_NOTIFY ;
// Simple Profile Service attribute
static CONST gattAttrType_t UserProfileService = { ATT_BT_UUID_SIZE, UserProfileServUUID };


extern uint8_t uart_data_processing;
extern uint8_t uart_temp_value;
extern uint8_t cmdstr[];
extern uint8_t cmdlen;
extern const CLI_COMMAND cli_cmd_list[];
uint8_t proc_buff[64];

/*********************************************************************
    Profile Attributes - Table
*/

static gattAttribute_t UserProfileAttrTbl[] =
{
    /* 服务的定义 */
    {
        { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
        GATT_PERMIT_READ,                         /* permissions */
        0,                                        /* handle */
        (uint8*)& UserProfileService            /* pValue */
    },

	/* 写特性 */
    {
        { ATT_BT_UUID_SIZE, characterUUID },
		GATT_PERMIT_READ | GATT_PERMIT_WRITE,
		0,
        &UserProfileChar1Props
    },

    {
        { ATT_BT_UUID_SIZE, UserProfilechar1UUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8*)& UserProfileChar1
    },

	/* 通知特性 */

    {
        { ATT_BT_UUID_SIZE, characterUUID },
		GATT_PERMIT_READ,
		0,
        &UserProfileChar2Props
    },

    {
        { ATT_BT_UUID_SIZE, UserProfilechar2UUID },
        GATT_PERMIT_READ,
        0,
        (uint8*)& UserProfileChar2
    },

    {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE ,
        0,
        (uint8*)UserProfileChar2Config
    },
	

};


/*****************************************************************************
@brief	用户任务1开始之前的相关初始化  
@param 	
	task_id ：任务ID
@retval	    
******************************************************************************/
void UserBase1_Init( uint8 task_id )
{
	UserBase1_TaskID = task_id;

	PRINT_DEBUG("UserBase1_Init \n");
	
	cmt2300a_int_init();
	cmt2300a_bsp_init();

	WaitUs(10);
	
	PRINT_DEBUG("cmt2300 init OK \n");

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
	
//	osal_start_reload_timer(UserBase2_TaskID, USER_TASK2_EVENT0, 250);
}


/*****************************************************************************
@brief	用户任务3开始之前的相关初始化      
@param 	
	task_id ：任务ID
@retval	    
******************************************************************************/
void UserBase3_Init( uint8 task_id )
{
	UserBase3_TaskID = task_id;
	
	VOID osal_memset( BleTransmissionBuff, 0, BLE_DATA_LEN );
	VOID osal_memset( BleTurnBuff, 0, BLE_DATA_LEN );
	BleTransmissionLen = 0;
	BleBuffProcessFlag = 0;
}

/*****************************************************************************
@brief	用户任务4开始之前的相关初始化      
@param 	
	task_id ：任务ID
@retval	    
******************************************************************************/
void UserBase4_Init( uint8 task_id )
{
	UserBase4_TaskID = task_id;
	extern LedsMixStructure leds_mix_param[];
	
	leds_mix_param[0].op_code = LED_CLOSE;
	leds_mix_param[1].op_code = LED_CLOSE;
	
	osal_set_event( UserBase4_TaskID, USER_TASK4_EVENT0 );
	osal_start_reload_timer(UserBase4_TaskID, USER_TASK4_EVENT1, 1000);
}



/*****************************************************************************
@brief	用户任务1事件处理
@param 	
@retval	    
******************************************************************************/
uint16 UserBase1_ProcessEvent( uint8 task_id, uint16 events )
{
	VOID task_id;
//	uint8_t pucWriteBuffer[20] = {0x0A, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x16, 0x00, 0x3E, 0x00};
//	uint8_t pucItemBuffer[20] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
//	uint8_t pucReadBuffer[40] = {0};
	if( events & USER_TASK1_EVENT0 )
	{
		radio433_process();
		
//		PRINT_DEBUG("#1\n");
//		hal_flash_erase_sector(FLASH_ADDR_UCDS_SEC1);
//		LOG("#2\n");
//		vGeneralFlashWrite(RELATIVE_FLASH_ADDR_UCDS_PARAM, 16, pucWriteBuffer);
//		LOG("#3\n");
//		vGeneralFlashWrite(RELATIVE_FLASH_ADDR_UCDS_PARAM + DATA_ITEM_MAX_SIZE, 16, pucWriteBuffer);
//		LOG("#4\n");
////		vGeneralFlashRead(FLASH_ADDR_UCDS_SEC1, 20, pucReadBuffer);
////		for(uint8_t i = 0; i < 20; i++)
////		{
////			LOG("buff :%d \n",pucReadBuffer[i]);
////		}
//		ucParamInit(vGeneralFlashRead, vGeneralFlashWrite);
//		
//		ucParamWrite(PV_DATA4, pucItemBuffer, 6);
//		
//		ucParamRead(PV_DATA4);
		
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
	uint8_t i = 0, temp = 0;
	uint8_t result = 0;
	uint8_t response[BLE_DATA_LEN] = {0};
	extern uint16_t current_freq;
	extern uint32_t current_key;

	if( events & USER_TASK2_EVENT0 )
	{
		Jump_Frequency_Process();
		
		return (events ^ USER_TASK2_EVENT0);
	}

	if( events & USER_TASK2_EVENT1 )
	{
		uart_data_processing = 1;
		
		switch(uart_temp_value)
		{
			case 1:
				cli_cmd_list[0].cmd_hdlr(0, NULL);
				break;
			case 2:
				cli_cmd_list[1].cmd_hdlr(1, NULL);
				break;
			case 3:
				cli_cmd_list[2].cmd_hdlr(2, NULL);
				break;
			case 4:
				cli_cmd_list[3].cmd_hdlr(3, NULL);
				break;
			case 5:
				cli_cmd_list[4].cmd_hdlr(4, NULL);
				break;
			default:
				break;
		}
		
		osal_memcpy(proc_buff, cmdstr, cmdlen);	//拷贝
		for(i = 0; i < 5; i++)
		{
			if(hal_my_strcmp(proc_buff, cli_cmd_list[i].cmd))
			{
				cli_cmd_list[i].cmd_hdlr(i, NULL);
				break;
			}
		}
		osal_memset(cmdstr, 0, AT_CMD_LENGTH_MAX);		//清除
		osal_memset(proc_buff, 0, AT_CMD_LENGTH_MAX);	//清除
		
		uart_temp_value = 0;
		uart_data_processing = 0;
		
		return (events ^ USER_TASK2_EVENT1);
	}
	
	if( events & USER_TASK2_EVENT2 )
	{
		cli_cmd_list[1].cmd_hdlr(1, NULL);
		/* 通过定时器指定扫描时间 */
		osal_start_timerEx(UserBase2_TaskID, USER_TASK2_EVENT3, BleStatus.scan_time);

		return (events ^ USER_TASK2_EVENT2);
	}
	
	if( events & USER_TASK2_EVENT3 )
	{
		cli_cmd_list[2].cmd_hdlr(2, NULL);
//		result = cli_cmd_list[3].cmd_hdlr(3, NULL);
		temp = 0;
		if(result == TRUE)
		{
//			response[temp++] =  ((current_freq >> 8) & 0xFF);
//			response[temp++] |= ((current_freq >> 0) & 0xFF);
//			
//			response[temp++] =  ((current_key >> 24) & 0xFF);
//			response[temp++] |= ((current_key >> 16) & 0xFF);
//			response[temp++] |= ((current_key >> 8) & 0xFF);
//			response[temp++] |= ((current_key >> 0) & 0xFF);
			
			Data_Packaging(BLE_RESPONSE_COPY, response, temp);
		}
		else
		{
//			response[temp++] = 0;
//			response[temp++] = 0;
//			
//			response[temp++] = 0;
//			response[temp++] = 0;
//			response[temp++] = 0;
//			response[temp++] = 0;
			Data_Packaging(BLE_RESPONSE_COPY, response, temp);
		}

		return (events ^ USER_TASK2_EVENT3);
	}
	
	if( events & USER_TASK2_EVENT4 )
	{
		cli_cmd_list[2].cmd_hdlr(2, NULL);
//		result = cli_cmd_list[3].cmd_hdlr(3, NULL);
		temp = 0;
		if(result == TRUE)
		{
//			response[temp++] =  ((current_freq >> 8) & 0xFF);
//			response[temp++] |= ((current_freq >> 0) & 0xFF);
//			
//			response[temp++] =  ((current_key >> 24) & 0xFF);
//			response[temp++] |= ((current_key >> 16) & 0xFF);
//			response[temp++] |= ((current_key >> 8) & 0xFF);
//			response[temp++] |= ((current_key >> 0) & 0xFF);
			
			Data_Packaging(BLE_RESPONSE_EXIT, response, temp);
		}
		else
		{
//			response[temp++] = 0;
//			response[temp++] = 0;
//			
//			response[temp++] = 0;
//			response[temp++] = 0;
//			response[temp++] = 0;
//			response[temp++] = 0;
			Data_Packaging(BLE_RESPONSE_EXIT, response, temp);
		}

		return (events ^ USER_TASK2_EVENT4);
	}
	
	if( events & USER_TASK2_EVENT5 )
	{
		result = cli_cmd_list[3].cmd_hdlr(3, NULL);
		temp = 0;
		if(result == TRUE)
		{
			response[temp++] =  ((current_freq >> 8) & 0xFF);
			response[temp++] |= ((current_freq >> 0) & 0xFF);
			
			response[temp++] =  ((current_key >> 24) & 0xFF);
			response[temp++] |= ((current_key >> 16) & 0xFF);
			response[temp++] |= ((current_key >> 8) & 0xFF);
			response[temp++] |= ((current_key >> 0) & 0xFF);
			
			Data_Packaging(BLE_RESPONSE_GIT, response, temp);
		}
		else
		{
			response[temp++] = 0;
			response[temp++] = 0;
			
			response[temp++] = 0;
			response[temp++] = 0;
			response[temp++] = 0;
			response[temp++] = 0;
			Data_Packaging(BLE_RESPONSE_GIT, response, temp);
		}

		return (events ^ USER_TASK2_EVENT5);
	}
	
	if( events & USER_TASK2_EVENT6 )
	{
		result = cli_cmd_list[4].cmd_hdlr(4, NULL);
		temp = 0;
		if(result == TRUE)
		{
			response[temp++] = 1;
			Data_Packaging(BLE_RESPONSE_REMOTE, response, temp);
		}
		else
		{
			response[temp++] = 0;
			Data_Packaging(BLE_RESPONSE_REMOTE, response, temp);
		}
		return (events ^ USER_TASK2_EVENT6);
	}



	return 0;
}

/*****************************************************************************
@brief	还原转译数据
@param 	
@retval	    
******************************************************************************/
uint8_t Turn_Data(uint8_t *pDataSource, uint8_t *pSave, uint8_t len)
{
	uint8_t i = 0, j = 0;
	
	for(i = 0; i < len; i++)
	{
		if(pDataSource[i] == 0x7D && pDataSource[i + 1] == 0x01) //7d 01
		{
			pSave[j] = 0x7D;
			j++;
			i++; //后面01跳过
		}
		else if(pDataSource[i] == 0x7D && pDataSource[i + 1] == 0x02) //7d 02
		{
			pSave[j] = 0x7E;
			j++;
			i++; //后面02跳过
		}
		else
		{
			pSave[j] = pDataSource[i];
			j++;
		}
	}
	
	LOG("Turn_Data [%d]:", j);
	for(i = 0; i < j; i++)
	{
		LOG("%02X ", pSave[i]);
	}
	
	return j; //返回还原后的数据长度
}

/*****************************************************************************
@brief	c8异或数据校验
@param 	
@retval	    
******************************************************************************/
uint8_t Data_Verifiction(uint8_t *buff, uint8_t len)
{
	uint8 value = 0;
	
	for(int i = 0; i < len; i++)
	{
		value ^= *(buff+i);
	}
	
	return value;
}

/*****************************************************************************
@brief	数据指令组包
@param 	
@retval	    
******************************************************************************/
void Data_Packaging(uint8_t cmd, uint8_t *pDdata, uint8_t len)
{
	uint8_t temp = 0;
	uint8_t cs = 0;
	uint8_t pack_buff[BLE_DATA_LEN];
	
	osal_memset(BleDataPack.msg_data, 0, BLE_DATA_LEN);
	
	pack_buff[temp++] = 0x7E;	//标识符   
	pack_buff[temp++] = len + 4;//应用层数据长度	
	
	pack_buff[temp++] = cmd;	//命令
	pack_buff[temp++] = len;	//数据长度
	pack_buff[temp++] = BleStatus.auth_master;	//主机令牌
	pack_buff[temp++] = BleStatus.auth_slave;	//从机令牌
	
	osal_memcpy(pack_buff + temp, pDdata, len);	//消息体
	temp += len;
	
	cs = Data_Verifiction(pack_buff + 1, temp - 1);	//校验码
	pack_buff[temp++] = cs;
	pack_buff[temp++] = 0x7E;
	
	User_NotifyData(pack_buff, temp);
	
	PRINT_DEBUG("Packaging \n");
	for(uint8_t i = 0; i < temp; i++)
	{
		PRINT_DEBUG("%02X ",pack_buff[i]);
	}
	PRINT_DEBUG(" \n");
	
}


void Ble_Status_Clear(void)
{
	BleStatus.auth = 0;
	BleStatus.auth_master = 0;
	BleStatus.auth_slave = 0;
	
	VOID osal_memset( BleTransmissionBuff, 0, BLE_DATA_LEN );
	
	CMT_POWER_OFF(); //关闭CMT2300A的电源
}
/*****************************************************************************
@brief	数据指令解析
@param 	
@retval	    
******************************************************************************/
uint8_t Data_To_Command(uint8_t *pDdata)
{
	uint8_t idx = GAP_BONDINGS_MAX;
	uint8_t len = 0;
	uint8_t temp = 0; 
	uint8_t response[10] = {0};
	VOID len;
	
	len = pDdata[temp++]; //应用层消息的总长度
	BleDataPack.cmd_id = pDdata[temp++]; //命令ID
	BleDataPack.msg_len = pDdata[temp++]; //消息体数据长度
	BleDataPack.master_token = pDdata[temp++]; //主机令牌
	BleDataPack.slave_token = pDdata[temp++]; //从机令牌
	
	osal_memset(BleDataPack.msg_data, 0, BLE_DATA_LEN);
	osal_memcpy(BleDataPack.msg_data, (pDdata + temp), BleDataPack.msg_len);
	
	switch(BleDataPack.cmd_id)
	{
		case BLE_CMD_AUTH:
			PRINT_DEBUG("BLE_CMD_AUTH \n");
			
			/* 打印一下应用层有效数据 */
			for(uint8_t i = 0; i < BleDataPack.msg_len; i++)
			{
				PRINT_DEBUG(" %02X ", BleDataPack.msg_data[i]);
			}
			PRINT_DEBUG(" \n");
			
			idx = GAPBondMgr_ResolveAddr(ADDRTYPE_PUBLIC, BleDataPack.msg_data, NULL);
//			if ( (idx < GAP_BONDINGS_MAX))
			{
				PRINT_DEBUG("auth success \n");
				/* 关闭低功耗 */
				disableSleep();
				/* 鉴权成功保存主机令牌，产生一个从机令牌 */
				BleStatus.auth_master = BleDataPack.master_token;
//				LL_Rand( ((uint8*) & BleStatus.auth_slave), sizeof( uint8 ));
				BleStatus.auth_slave = 0x0D;	//test
				BleStatus.auth = 1;
				
				/* 打开CMT2300A的电源，并初始化,进入待命工作状态 */
				cmt2300a_int_init();
				cmt2300a_bsp_init();
				
				response[0] = 1;
				Data_Packaging(BLE_RESPONSE_AUTH, response, 1);
			}
//			else
//			{
//				/* 鉴权失败，清除令牌,立即终止连接 */
//				PRINT_DEBUG("auth fail \n");
//				
//				GAPRole_TerminateConnection();
//				
//			}
			
			break;
		case BLE_CMD_COPY:
			PRINT_DEBUG("BLE_CMD_COPY \n");
			/* 打印一下应用层有效数据 */
			for(uint8_t i = 0; i < BleDataPack.msg_len; i++)
			{
				PRINT_DEBUG(" %02X ", BleDataPack.msg_data[i]);
			}
			PRINT_DEBUG(" \n");
			
			if(BleStatus.auth == 0)	//如果是授权未通过的命令，则断开连接
			{
				GAPRole_TerminateConnection();
				break;
			}
			
			BleStatus.scan_time = (*(BleDataPack.msg_data + 4) << 8);
			BleStatus.scan_time |= (*(BleDataPack.msg_data + 5));
			PRINT_DEBUG("scan time: %d \n", BleStatus.scan_time);
			
			if(BleStatus.scan_time > 0)
			{
				osal_set_event(UserBase2_TaskID, USER_TASK2_EVENT2);
			}
			
			break;
		case BLE_CMD_QUERY:
			
			break;
		case BLE_CMD_EXIT:
			PRINT_DEBUG("BLE_CMD_EXIT \n");
			/* 打印一下应用层有效数据 */
			for(uint8_t i = 0; i < BleDataPack.msg_len; i++)
			{
				PRINT_DEBUG(" %02X ", BleDataPack.msg_data[i]);
			}
			PRINT_DEBUG(" \n");
			
			if(BleStatus.auth == 0)	//如果是授权未通过的命令，则断开连接
			{

				GAPRole_TerminateConnection();
				break;
			}
			
			osal_set_event(UserBase2_TaskID, USER_TASK2_EVENT4);
			
			break;
		case BLE_CMD_GET:
			PRINT_DEBUG("BLE_CMD_GET \n");
			/* 打印一下应用层有效数据 */
			for(uint8_t i = 0; i < BleDataPack.msg_len; i++)
			{
				PRINT_DEBUG(" %02X ", BleDataPack.msg_data[i]);
			}
			PRINT_DEBUG(" \n");
			
			if(BleStatus.auth == 0)	//如果是授权未通过的命令，则断开连接
			{

				GAPRole_TerminateConnection();
				break;
			}
			
			osal_set_event(UserBase2_TaskID, USER_TASK2_EVENT5);
			
			break;
		case BLE_CMD_CLEAR:
			
			break;
		case BLE_CMD_MODE:
			
			break;
		case BLE_CMD_REMOTE:
			PRINT_DEBUG("BLE_CMD_REMOTE \n");
			/* 打印一下应用层有效数据 */
			for(uint8_t i = 0; i < BleDataPack.msg_len; i++)
			{
				PRINT_DEBUG(" %02X ", BleDataPack.msg_data[i]);
			}
			PRINT_DEBUG(" \n");
			
			if(BleStatus.auth == 0)	//如果是授权未通过的命令，则断开连接
			{
				Ble_Status_Clear();
				GAPRole_TerminateConnection();
				break;
			}
			
			osal_set_event(UserBase2_TaskID, USER_TASK2_EVENT6);
			break;
		case BLE_CMD_UPDATE:
			
			break;
		default :
			break;
		
	}
	
	
	return 0;
}
	
	

/*****************************************************************************
@brief	用户任务3事件处理
@param 	
@retval	    
******************************************************************************/
uint16 UserBase3_ProcessEvent( uint8 task_id, uint16 events )
{
	VOID task_id;
	uint8_t *IndexPoint = NULL;
	uint8_t data_len = 0;
	uint8_t cs = 0;

	if( events & USER_TASK3_EVENT0 )
	{
		BleBuffProcessFlag = 1;
		
		/* 打印测试 */
		PRINT_DEBUG("MSG LEN: %d : ",BleTransmissionLen);
		for(uint8_t i = 0; i < BleTransmissionLen; i++)
		{
			PRINT_DEBUG("%02X ",*(BleTransmissionBuff + i));
		}
		PRINT_DEBUG("\n");
		
		

		#if 0	//是否需要对数据进行转义
		/* 对转译数据进行还原，并返回长度 */
		data_len = Turn_Data(BleTransmissionBuff, BleTurnBuff, BleTransmissionLen); 
		IndexPoint = BleTurnBuff;
		#else
		data_len = BleTransmissionLen;
		IndexPoint = BleTransmissionBuff;
		#endif
		if(IndexPoint[0] == 0x7E && IndexPoint[data_len - 1] == 0x7E)	//外层标识符
		{
			cs = Data_Verifiction(IndexPoint + 1, data_len - 3); //减去校验值和标识符的长度
			if(cs == IndexPoint[data_len - 2]) //校验值对比
			{
				PRINT_DEBUG("cs ok \n");
				
				Data_To_Command(IndexPoint + 1);
			}
			else
			{
				PRINT_DEBUG("cs fail \n");
				PRINT_DEBUG("%02X  %02X \n", cs, IndexPoint[data_len - 2]);
			}
		}
		
		
		BleBuffProcessFlag = 0;
		return (events ^ USER_TASK3_EVENT0);
	}

	return 0;
}


/*****************************************************************************
@brief	用户任务4事件处理
@param 	
@retval	    
******************************************************************************/
uint16 UserBase4_ProcessEvent( uint8 task_id, uint16 events )
{
	VOID task_id;
	uint16_t i = 0;
	static uint8_t t = 0;

	if( events & USER_TASK4_EVENT0 )
	{
		
		Leds_Control();
		
		return (events ^ USER_TASK4_EVENT0);
	}
	
	if( events & USER_TASK4_EVENT1 )
	{
//		LOG("WDI %d", t);
//		if(t++ > 20)
//		{
//			osal_stop_timerEx(UserBase4_TaskID, USER_TASK4_EVENT1);
//			enableSleep();
//		}
		
		hal_gpio_write(P07_WDI, 1);
		for(i = 0; i < 100; i++)
		{
			
		}
		hal_gpio_write(P07_WDI, 0);
		
		return (events ^ USER_TASK4_EVENT1);
	}

	return 0;
}


/*********************************************************************
    @fn          UserProfile_ReadAttrCB

    @brief       Read an attribute.

    @param       connHandle - connection message was received on
    @param       pAttr - pointer to attribute
    @param       pValue - pointer to data to be read
    @param       pLen - length of data to be read
    @param       offset - offset of the first octet to be read
    @param       maxLen - maximum length of data to be read

    @return      Success or Failure
*/
static uint8 UserProfile_ReadAttrCB( uint16 connHandle, gattAttribute_t* pAttr,
                                       uint8* pValue, uint16* pLen, uint16 offset, uint8 maxLen )
{
    bStatus_t status = SUCCESS;


    return ( status );
}

/*********************************************************************
    @fn      UserProfile_WriteAttrCB

    @brief   Validate attribute data prior to a write operation

    @param   connHandle - connection message was received on
    @param   pAttr - pointer to attribute
    @param   pValue - pointer to data to be written
    @param   len - length of data
    @param   offset - offset of the first octet to be written

    @return  Success or Failure
*/
// TODO: test this function
static bStatus_t UserProfile_WriteAttrCB( uint16 connHandle, gattAttribute_t* pAttr,
                                            uint8* pValue, uint16 len, uint16 offset )
{
    bStatus_t status = SUCCESS;
    uint8 notifyApp = 0xFF;
	uint8* pCurValue = NULL;
	VOID notifyApp;
	
//	LOG("UserProfile_WriteAttrCB \n");
    // If attribute permissions require authorization to write, return error
    if ( gattPermitAuthorWrite( pAttr->permissions ) )
    {
        // Insufficient authorization
		LOG("ATT_ERR_INSUFFICIENT_AUTHOR \n");
        return ( ATT_ERR_INSUFFICIENT_AUTHOR );
    }
	
	if ( pAttr->type.len == ATT_BT_UUID_SIZE )
    {
        // 16-bit UUID
        uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
		
		LOG("UUID %X \n", uuid);
		
        switch ( uuid )
        {
			case USERPROFILE_CHAR1_UUID:
				//Validate the value
				// Make sure it's not a blob oper
				if ( offset == 0 )
				{
					if ( len >ATT_GetCurrentMTUSize(0)-3 )
					{
						LOG("ATT_ERR_INVALID_VALUE_SIZE \n");
						status = ATT_ERR_INVALID_VALUE_SIZE;
					}
				}
				else
				{
					LOG("ATT_ERR_ATTR_NOT_LONG \n");
					status = ATT_ERR_ATTR_NOT_LONG;
				}

				//Write the value
				if ( status == SUCCESS && BleBuffProcessFlag == 0)
				{
					LOG("UserProfile_WriteAttrCB \n");
//					User_NotifyData(pValue, len);
					
//					uint8* pCurValue = (uint8*)pAttr->pValue;
					pCurValue = BleTransmissionBuff;
					BleTransmissionLen = len;
					VOID osal_memcpy( pCurValue, pValue, len );
					
					notifyApp = USERPROFILE_CHAR1;
					
					osal_set_event(UserBase3_TaskID, USER_TASK3_EVENT0);	
				}
            break;
				
			case GATT_CLIENT_CHAR_CFG_UUID:
				status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                     offset, GATT_CLIENT_CFG_NOTIFY );
			break;
			
			default:
				// Should never get here! (characteristics 2 and 4 do not have write permissions)
				status = ATT_ERR_ATTR_NOT_FOUND;
            break;
		}
	}
	else
    {
		LOG("ATT_ERR_INVALID_HANDLE \n");
        // 128-bit UUID
        status = ATT_ERR_INVALID_HANDLE;
    }
	
	// If a charactersitic value changed then callback function to notify application of change
//    if ( (notifyApp != 0xFF ) && simpleProfile_AppCBs && simpleProfile_AppCBs->pfnSimpleProfileChange )
//    {
//        simpleProfile_AppCBs->pfnSimpleProfileChange( notifyApp );
//    }

    return ( status );
}

/*********************************************************************
    PROFILE CALLBACKS
*/
// Simple Profile Service Callbacks
CONST gattServiceCBs_t UserProfileCBs =
{
    UserProfile_ReadAttrCB,  // Read callback function pointer
    UserProfile_WriteAttrCB, // Write callback function pointer
    NULL                       // Authorization callback function pointer
};


/*********************************************************************
    @fn          UserProfile_HandleConnStatusCB

    @brief       Simple Profile link status change handler function.

    @param       connHandle - connection handle
    @param       changeType - type of change

    @return      none
*/
static void UserProfile_HandleConnStatusCB( uint16 connHandle, uint8 changeType )
{
    // Make sure this is not loopback connection
    if ( connHandle != LOOPBACK_CONNHANDLE )
    {
        // Reset Client Char Config if connection has dropped
        if ( ( changeType == LINKDB_STATUS_UPDATE_REMOVED )      ||
                ( ( changeType == LINKDB_STATUS_UPDATE_STATEFLAGS ) &&
                  ( !linkDB_Up( connHandle ) ) ) )
        {
            GATTServApp_InitCharCfg( connHandle, UserProfileChar2Config );
        }
    }
}

/*********************************************************************
    @fn      SimpleProfile_AddService

    @brief   Initializes the Simple Profile service by registering
            GATT attributes with the GATT server.

    @param   services - services to add. This is a bit map and can
                       contain more than one service.

    @return  Success or Failure
*/
bStatus_t UserProfile_AddService( uint32 services )
{
    uint8 status = SUCCESS;
    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg( INVALID_CONNHANDLE, UserProfileChar2Config );
    // Register with Link DB to receive link status change callback
    VOID linkDB_Register( UserProfile_HandleConnStatusCB );

	// Register GATT attribute list and CBs with GATT Server App
	status = GATTServApp_RegisterService( UserProfileAttrTbl,
										  GATT_NUM_ATTRS( UserProfileAttrTbl ),
										  &UserProfileCBs );
    

    return ( status );
}



/*********************************************************************
 * @fn          User_NotifyData
 *
 * @brief       Send a notification containing the data of key.
 *
 * @param       connHandle - connection handle
 * @param       pNoti - pointer to notification structure
 *
 * @return      Success or Failure
 */

//Once max send 230 B, more than need subpackage
bStatus_t User_NotifyData(uint8_t *notiData, uint8_t notiLen)
{
	extern gattAttribute_t simpleProfileAttrTbl[];
	if (notiData == NULL || notiLen == 0 || notiLen > MTU_SIZE)
	{
		LOG("notify fail, due to [data == NULL || len == 0 || notiLen > BLE_MTU_VAL]\n");
		return 0xFF;
	}

	uint16 connHandle;
    GAPRole_GetParameter(GAPROLE_CONNHANDLE, &connHandle);
	if (connHandle == INVALID_CONNHANDLE)
	{
		LOG("notify fail, due to [connHandle == INVALID_CONNHANDLE]\n");
		return 0xFE;
	}

	attHandleValueNoti_t noti;
    noti.handle = simpleProfileAttrTbl[8].handle;
    noti.len = notiLen;
	osal_memcpy(noti.value, notiData, notiLen);

//	LOG("len:%d\r\n", noti.len);
//	
//	for(int i=0; i<noti.len; i++)
//	{
//		LOG("[----->>noti.value:%x, index:%d]\r\n", noti.value[i], i);
//	}

  return GATT_Notification( connHandle, &noti, FALSE );
}


















