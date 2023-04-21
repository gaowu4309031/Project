#ifndef __CMT2300A_HAL_H
#define __CMT2300A_HAL_H

#include "inp_types.h"
#include "time_server.h"
#include "drv_cmt2300a.h"


#ifdef __cplusplus 
extern "C" { 
#endif

/* ************************************************************************
*  The following need to be modified by user
*  ************************************************************************ */
#define CMT2300A_SetGpio1In()           SET_GPIO_IN(CMT_GPIO1)
#define CMT2300A_SetGpio2In()           SET_GPIO_IN(CMT_GPIO2_GPIO)
#define CMT2300A_SetGpio3In()           SET_GPIO_IN(CMT_GPIO3_GPIO)
#define CMT2300A_ReadGpio1()            READ_GPIO_PIN(CMT_GPIO1)
#define CMT2300A_ReadGpio2()            READ_GPIO_PIN(CMT_GPIO2_GPIO)
#define CMT2300A_ReadGpio3()            READ_GPIO_PIN(CMT_GPIO3_GPIO)
#define CMT2300A_DelayMs(ms)            system_delay_ms(ms)
#define CMT2300A_DelayUs(us)            system_delay_us(us)
#define CMT2300A_GetTickCount()         g_nSysTickCount
/* ************************************************************************ */

void CMT2300A_InitGpio(void);

u8 CMT2300A_ReadReg(u8 addr);
void CMT2300A_WriteReg(u8 addr, u8 dat);

void CMT2300A_ReadFifo(u8 buf[], u16 len);
void CMT2300A_WriteFifo(const u8 buf[], u16 len);

#ifdef __cplusplus 
} 
#endif

#endif
