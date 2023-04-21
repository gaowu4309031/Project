/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, CMOSTEK SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (C) CMOSTEK SZ.
 */

/*!
 * @file    radio.c
 * @brief   Generic radio handlers
 *
 * @version 1.2
 * @date    Jul 17 2017
 * @author  CMOSTEK R@D
 */
 
#include "radio.h"
#include "cmt2300a_params.h"
#include "radio433_protocol.h"
#include "frequency_config_table.h"
#include <string.h>



void RF_Jump_Frequency(enum SupportFreqList list_num)
{
	jump_frequency_list_init(list_num);
	
	/* Config registers */
	CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , frequency_set_register.now_frequency_bank , CMT2300A_FREQUENCY_BANK_SIZE );
	CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR        , frequency_set_register.now_tx_bank        , CMT2300A_TX_BANK_SIZE        );
	
	CMT2300A_GoSleep();
	
	CMT2300A_GoRx();
}

/*****************************************************************************
@brief	配置由RFPDK生成的CMT2300A寄存器参数，已经DOUT输出脚       
@param    
@retval	    
******************************************************************************/
void RF_Init(void)
{
	u8 tmp;
	
	CMT2300A_InitGpio();
	CMT2300A_Init();
	
	/* Config registers */
	CMT2300A_ConfigRegBank(CMT2300A_CMT_BANK_ADDR       , g_cmt2300aCmtBank       , CMT2300A_CMT_BANK_SIZE       );
	CMT2300A_ConfigRegBank(CMT2300A_SYSTEM_BANK_ADDR    , g_cmt2300aSystemBank    , CMT2300A_SYSTEM_BANK_SIZE    );
	CMT2300A_ConfigRegBank(CMT2300A_FREQUENCY_BANK_ADDR , g_cmt2300aFrequencyBank , CMT2300A_FREQUENCY_BANK_SIZE );
	CMT2300A_ConfigRegBank(CMT2300A_DATA_RATE_BANK_ADDR , g_cmt2300aDataRateBank  , CMT2300A_DATA_RATE_BANK_SIZE );
	CMT2300A_ConfigRegBank(CMT2300A_BASEBAND_BANK_ADDR  , g_cmt2300aBasebandBank  , CMT2300A_BASEBAND_BANK_SIZE  );
	CMT2300A_ConfigRegBank(CMT2300A_TX_BANK_ADDR        , g_cmt2300aTxBank        , CMT2300A_TX_BANK_SIZE        );
	
	// xosc_aac_code[2:0] = 2
	tmp = (~0x07) & CMT2300A_ReadReg(CMT2300A_CUS_CMT10);
	CMT2300A_WriteReg(CMT2300A_CUS_CMT10, tmp|0x02);
}

void RF_Config(void)
{
	u8 nInt2Sel;
	(void) nInt2Sel;

// when use packet mode or rx, the fuction CMT2300A_ConfigGpio could take effect, using tx in the direct mode it can't be used
	CMT2300A_ConfigGpio( CMT2300A_GPIO2_SEL_DOUT);
	CMT2300A_GoRx();

    /* Disable low frequency OSC calibration */
    CMT2300A_EnableLfosc(FALSE);
    /* Go to sleep for configuration to take effect */
		
    CMT2300A_GoSleep();
}


