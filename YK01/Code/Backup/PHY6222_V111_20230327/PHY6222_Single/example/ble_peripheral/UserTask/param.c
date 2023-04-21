#include "param.h"
#include "uart.h"
#include "log.h"
#include "flash.h"



/*****************************************************************************
								私有变量
******************************************************************************/
static xParam_t xParam[TAB_MAX];
static uint8_t ucDataBuffer[DATA_ITEM_MAX_SIZE] = {0};
ParamItem_t xParamHandler = 
{
	.ucLock 		= 0,
	.pxReadHandler 	= NULL,
	.pxWriteHandler = NULL,
	.pxParamData 	= xParam,
	.pucReadBuffer	= ucDataBuffer,
};

const int MOD_ADLER = 65521;




/*****************************************************************************
@brief	flash写入函数通用转换接口	      
@param    
@retval	    
******************************************************************************/
uint8_t vGeneralFlashWrite(uint32_t addr, uint32_t size, uint8_t *data)
{
	/* 添加flash接口函数*/
	hal_flash_write(addr, data, size);
	
	return 0;
}

/*****************************************************************************
@brief	flash读取函数通用转换接口	      
@param    
@retval	    
******************************************************************************/
uint8_t vGeneralFlashRead(uint32_t addr, uint32_t size, uint8_t *data)
{
	/* 添加flash接口函数*/
	hal_flash_read(addr, data, size);
	
	return 0;
}



/*****************************************************************************
@brief		      
@param    
@retval	    
******************************************************************************/
static uint32_t _param_adler32(uint8_t *data, uint32_t len)
{
    uint32_t a = 1, b = 0;
    uint32_t index = 0;

    /* Process each byte of the data in order */
    for(index = 0; index < len; ++index)
    {
        a = (a + data[index]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }

    return (b << 16) | a;
}

/*****************************************************************************
@brief		      
@param    
@retval	    
******************************************************************************/
static uint8_t _param_pv_data_validity_check(uint8_t *p_data, uint32_t len, uint32_t *FlashAdler32)
{
    uint32_t adler32 = 0;

    if(NULL == p_data)
    {
        return FALSE;
    }

    adler32 = _param_adler32(p_data, (len - 4));
	*FlashAdler32 = ((p_data[len - 1] & 0xFF) << 24);
	*FlashAdler32 += ((p_data[len - 2] & 0xFF) << 16);
	*FlashAdler32 += ((p_data[len - 3] & 0xFF) << 8);
	*FlashAdler32 += ((p_data[len - 4] & 0xFF));
	LOG("adler %X # Flash %X\n", adler32, *FlashAdler32);
	
    if(*FlashAdler32 == adler32)
    {
		LOG("Param pv data validity check success\r\n");

		return TRUE;
    }
    else
    {
        LOG("Param pv data validity check fail\r\n");
    }

    return FALSE;
}


/*****************************************************************************
@brief		      
@param    
@retval	    
******************************************************************************/
static uint8_t _param_pv_param_init(void)
{
	uint8_t i = 0, j = 0;
	uint32_t temp_len = 0;
	uint32_t temp_addr = 0;
	uint32_t temp_cs = 0;
	uint8_t result = FALSE;

	(void) i; 
	(void) j;
	
	/**************测试打印***************/
	xParamHandler.pxReadHandler((RELATIVE_FLASH_ADDR_UCDS_PARAM + 0), 128, xParamHandler.pucReadBuffer);
	for(j = 0; j < 128; j++)
	{
		LOG("%d = %X \n", j, xParamHandler.pucReadBuffer[j]);
	}
	/*************************************/
	
	/* 将flash中存已保存的数据记录到登记表中 */
	for(i = 0; i < TAB_MAX; i++)
	{
		xParamHandler.pxReadHandler((RELATIVE_FLASH_ADDR_UCDS_PARAM + temp_addr), FLASH_DATA_UNIT_SIZE, (uint8_t *)&temp_len);
		LOG("data len:%X \n", temp_len);
		
		
		if((temp_len > 0) && (temp_len < DATA_ITEM_MAX_SIZE))	//如果读取的长度在正常范围，将flash中的数据按这个长度读出
		{
			
			//读出数据
			xParamHandler.pxReadHandler((RELATIVE_FLASH_ADDR_UCDS_PARAM + temp_addr + FLASH_DATA_UNIT_SIZE), temp_len, xParamHandler.pucReadBuffer);
			
			//进行校验
			for(j = 0; j < temp_len; j++)
			{
				LOG("%d = %d \n", j, xParamHandler.pucReadBuffer[j]);
			}
			
			result = _param_pv_data_validity_check(xParamHandler.pucReadBuffer, temp_len, &temp_cs);
			if(result == TRUE)
			{
				xParamHandler.pxParamData[i].len = temp_len + 4; //记录表中的长度包含长度记录的4个字节
				xParamHandler.pxParamData[i].addr = RELATIVE_FLASH_ADDR_UCDS_PARAM + temp_addr;
				xParamHandler.pxParamData[i].cs = temp_cs;
			}
			else
			{
				break;
			}
			
			temp_addr += DATA_ITEM_MAX_SIZE;
			temp_addr = FLASH_ADDR_ALIGN(temp_addr);
			temp_cs = 0;
			temp_len = 0;
			
		}
		else
		{
			LOG("data len error \n");

			break;
		}
	}
	
	/* 对登记表中剩余部分进行初始化 */
	while(i < TAB_MAX)
	{
		xParamHandler.pxParamData[i].len = 0;
		xParamHandler.pxParamData[i].addr = temp_addr;
		xParamHandler.pxParamData[i].cs = 0;
		
		temp_addr += DATA_ITEM_MAX_SIZE;
		i++;
	}
	
	return 0;
}


/*****************************************************************************
@brief	Flash参数存储初始化	      
@param    
@retval	    
******************************************************************************/
uint8_t ucParamInit(PARAM_READ_FUNC pxRead, PARAM_WRITE_FUNC pxWrite)
{
	if((NULL == pxRead) || (NULL == pxWrite))
    {
        return ERR_ERROR;
    }
	
	xParamHandler.ucLock = 0;
	
	xParamHandler.pxReadHandler = pxRead;
	xParamHandler.pxWriteHandler = pxWrite;
	
//	hal_setMem(xParamHandler.pucReadBuffer, 0, DATA_ITEM_MAX_SIZE);
	
	_param_pv_param_init();
	
	return 0;
}


/*****************************************************************************
@brief	Flash参数写入	      
@param  
@retval	    
******************************************************************************/
uint8_t ucParamWrite(uint16_t tab_num, uint8_t *buff, uint32_t len)
{
	uint8_t i = 0, j = 0;
	uint8_t temp_buff[64];
	uint32_t temp_cs = 0;
	uint8_t temp_len = 0;
	uint32_t temp_addr = 0;
	
	if((NULL == buff) || (NULL == buff) || (len == 0) || (tab_num > TAB_MAX))
    {
        return ERR_ERROR;
    }
	
	for(i = 0; i < TAB_MAX; i++)
	{
		if(i == tab_num)
		{	
			/* 长度 */
			temp_buff[temp_len++] = ((len >> 0) & 0xFF);
			temp_buff[temp_len++] = ((len >> 8) & 0xFF); 
			temp_buff[temp_len++] = ((len >> 16) & 0xFF);
			temp_buff[temp_len++] = ((len >> 24) & 0xFF);
			/* 数据 */
			for(j = 0; j < len; j++)
			{
				temp_buff[temp_len++] = buff[j];
			}
			/* CS */
			temp_cs = _param_adler32(buff, len);		
			temp_buff[temp_len++] = ((temp_cs >> 0) & 0xFF);
			temp_buff[temp_len++] = ((temp_cs >> 8) & 0xFF); 
			temp_buff[temp_len++] = ((temp_cs >> 16) & 0xFF);
			temp_buff[temp_len++] = ((temp_cs >> 24) & 0xFF);
			/* 计算flash地址 */
			temp_addr = RELATIVE_FLASH_ADDR_UCDS_PARAM + (DATA_ITEM_MAX_SIZE * i);
			/* 将数据特性写入参数表 */
			xParamHandler.pxParamData[i].len = temp_len;
			xParamHandler.pxParamData[i].cs = temp_cs;
			xParamHandler.pxParamData[i].addr = temp_addr;
			/* 将完整数据写入flash */
			xParamHandler.pxWriteHandler(temp_addr, temp_len, temp_buff);
			
			break;
		}
	}
	
	return 0;
}

/*****************************************************************************
@brief	Flash参数读取	      
@param    
@retval	    
******************************************************************************/
uint8_t ucParamRead(uint16_t tab_num)
{
	uint8_t i = 0, j = 0;
	uint32_t temp_cs = 0;
	uint8_t result = 0;
	
	if(tab_num > TAB_MAX)
    {
        return ERR_ERROR;
    }
	
	for(i = 0; i < TAB_MAX; i++)
	{
		if(i == tab_num)
		{
			if(xParamHandler.pxParamData[i].len == 0)
			{
				/* 还没有存储参数 */
				return ERR_ERROR;
			}
			else
			{
				xParamHandler.pxReadHandler(xParamHandler.pxParamData[i].addr, xParamHandler.pxParamData[i].len, xParamHandler.pucReadBuffer);
				result = _param_pv_data_validity_check(xParamHandler.pucReadBuffer + 4, xParamHandler.pxParamData[i].len - 4, &temp_cs);
				if(result == TRUE)
				{
					for(j = 0; j < xParamHandler.pxParamData[i].len; j++)
					{
						LOG("read %d = %X \n", j, xParamHandler.pucReadBuffer[j]);
					}
					break;
				}
				else 
				{
					/* 如果数据错误，则将参数记录表中的记录抹除 */
					xParamHandler.pxParamData[i].len = 0;
					xParamHandler.pxParamData[i].addr = RELATIVE_FLASH_ADDR_UCDS_PARAM + (DATA_ITEM_MAX_SIZE * i);
					xParamHandler.pxParamData[i].cs = 0;
					
					return ERR_ERROR;
				}
				
			}
		}
	}
	
	return 0;
}


















