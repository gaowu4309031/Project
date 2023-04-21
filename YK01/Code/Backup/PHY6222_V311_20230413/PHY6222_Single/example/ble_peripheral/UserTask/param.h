#ifndef __PARAM_H__
#define __PARAM_H__ 


#include "stdint.h"




/*****************************************************************************
									宏定义
******************************************************************************/
#define FLASH_SPIF_BASE_ADDR					(0x11000000u)
#define FLASH_ADDR_UCDS_PARAM         			(0x11034000u)		//8K 34000-35FFF
#define FLASH_ADDR_UCDS_SEC1         			(0x11034000u)		
#define FLASH_ADDR_UCDS_SEC2         			(0x11035000u)

#define RELATIVE_FLASH_ADDR_UCDS_PARAM			(FLASH_ADDR_UCDS_PARAM + 0x0000)
#define RELATIVE_FLASH_ADDR_UCDS_BACKUP_PARAM	(FLASH_ADDR_UCDS_PARAM + 0x2000)


#define DATA_ITEM_MAX_SIZE						(64)
#define FLASH_DATA_UNIT_SIZE					(4)


#define FLASH_ADDR_ALIGN(a) 	((a % 4) ? (((a + 4) & 0xFC)) : (a)) //4字节对齐
/* 存储参数个数 */
#define TAB_MAX		(PV_DATA_TAIL + 1)


/* Error codes definitions */
#define ERR_OK                     (0)           /* No error */
#define ERR_ERROR                  (1)          /* A generic error happens */
#define ERR_TIMEOUT                (2)          /* Timeout */
#define ERR_FULL                   (3)          /* The resource is full */
#define ERR_EMPTY                  (4)          /* The resource is empty */
#define ERR_PARAM                  (5)          /* Parameter error */




/*****************************************************************************
								类型定义
******************************************************************************/
typedef uint8_t (*PARAM_READ_FUNC)(uint32_t, uint32_t, uint8_t*);
typedef uint8_t (*PARAM_WRITE_FUNC)(uint32_t, uint32_t, uint8_t*);


enum emParamNum
{
	PV_DATA_HEAD,
	
	PV_DATA1 = PV_DATA_HEAD,
	PV_DATA2,
	PV_DATA3,
	PV_DATA4,
	PV_DATA5,
	
	PV_DATA_TAIL = PV_DATA5
};


typedef struct
{
	uint32_t 	 			cs;
	uint32_t 		 		len;
	uint32_t 		 		addr;
} xParam_t;


typedef struct
{
	uint8_t 				ucLock;
	PARAM_READ_FUNC 		pxReadHandler;
	PARAM_WRITE_FUNC 		pxWriteHandler;
	xParam_t				*pxParamData;
	uint8_t					*pucReadBuffer;		
	
} ParamItem_t;






/*****************************************************************************
								函数声明
******************************************************************************/
uint8_t ucParamInit(PARAM_READ_FUNC pxRead, PARAM_WRITE_FUNC pxWrite);
uint8_t ucParamWrite(uint16_t tab_num, uint8_t *buff, uint32_t len);
uint8_t ucParamRead(uint16_t tab_num);
uint8_t vGeneralFlashRead(uint32_t addr, uint32_t size, uint8_t *data);
uint8_t vGeneralFlashWrite(uint32_t addr, uint32_t size, uint8_t *data);




#endif //__PARAM_H__






























