#include "led_task.h"
#include "uart.h"
#include "log.h"
#include "OSAL.h"
#include "OSAL_Tasks.h"
#include "UserTask.h"


LedsMixStructure leds_mix_param[LED_NUM_MAX];

/*****************************************************************************
@brief	LED打开函数
@param 	
@retval	    
******************************************************************************/
static void Leds_Switch(uint8_t sw, uint8_t led_num)
{

	if(sw == LED_OPEN)
	{
		switch((1 << led_num))
		{
			case LED_NUM_0:
				LED0_ON();
				break;
			case LED_NUM_1:
				LED1_ON();
				break;
			default:
				break;
		}
		leds_mix_param[led_num].led_status = LED_STATUS_ON; //记录当前操作的LED状态
		leds_mix_param[led_num].op_code = LED_NULL; //LED本次操作结束
		
	}
	else if(sw == LED_CLOSE)
	{

		switch((1 << led_num))
		{
			case LED_NUM_0:
				LED0_OFF();
				break;
			case LED_NUM_1:
				LED1_OFF();
				break;
			default:
				break;
		}
		leds_mix_param[led_num].led_status = LED_STATUS_OFF; //记录当前操作的LED状态
		leds_mix_param[led_num].op_code = LED_NULL; //LED本次操作结束
		
	}
		
}

/*****************************************************************************
@brief	LED闪烁操作函数
@param 	
@retval	    
******************************************************************************/
static void Leds_Turn(uint8_t sw, uint8_t led_num)
{

	switch((1 << led_num))
	{
		case LED_NUM_0:
			if(leds_mix_param[led_num].led_status == LED_STATUS_OFF) //根据当前LED的状态进行翻转
			{	
				LED0_ON();
				leds_mix_param[led_num].led_status = LED_STATUS_ON; //记录当前操作的LED状态
			}
			else
			{
				LED0_OFF();
				leds_mix_param[led_num].led_status = LED_STATUS_OFF; //记录当前操作的LED状态
			}
			break;
		case LED_NUM_1:
			if(leds_mix_param[led_num].led_status == LED_STATUS_OFF) //根据当前LED的状态进行翻转
			{	
				LED1_ON();
				leds_mix_param[led_num].led_status = LED_STATUS_ON; //记录当前操作的LED状态
			}
			else
			{
				LED1_OFF();
				leds_mix_param[led_num].led_status = LED_STATUS_OFF; //记录当前操作的LED状态
			}
			break;
		default:
			break;
	}
	
		
}




/*****************************************************************************
@brief	LED控制函数，找到要执行的led操作，向对应的操作发生指令
@param 	
@retval	    
******************************************************************************/
void Leds_Control(void)
{
	uint8_t i = 0;

	/* 遍历每个LED结构体下的操作 */
	for(i = 0; i < LED_NUM_MAX; i++)
	{
		switch(leds_mix_param[i].op_code)
		{
			case LED_OPEN:
				Leds_Switch(leds_mix_param[i].op_code, i);
			case LED_CLOSE:
				Leds_Switch(leds_mix_param[i].op_code, i); 
				break;
			case LED_TURN:
				break;
			case LED_TWINKLE:
				Leds_Turn(leds_mix_param[i].op_code, i);
				break;
			default:
				break;
		}
	}
	
	
}




































