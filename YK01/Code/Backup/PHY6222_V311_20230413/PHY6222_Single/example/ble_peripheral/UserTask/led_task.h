#ifndef __LED_TASK_H__
#define __LED_TASK_H__

#include "stdint.h"


/* LED 灯号*/
#define LED_NUM_0			(uint8_t)(1 << 0)
#define LED_NUM_1			(uint8_t)(1 << 1)

/* LED 操作命令 */
#define LED_NULL			0
#define LED_OPEN			1
#define LED_CLOSE			2
#define LED_TURN			3
#define LED_TWINKLE			4

/* LED 电平输出 */
#define LED0_ON()			hal_gpio_write(GPIO_P18, 1); //绿
#define LED0_OFF()			hal_gpio_write(GPIO_P18, 0); //绿
#define LED1_ON()			hal_gpio_write(GPIO_P20, 1); //红
#define LED1_OFF()			hal_gpio_write(GPIO_P20, 0); //红

/* LED 灯的状态 */
#define LED_STATUS_ON		1
#define LED_STATUS_OFF		0

#define EMPTY_VAL			0

#define LED_NUM_MAX			2 //不能超过8个LED

	
typedef struct LedStructure
{
	uint8_t op_code;
	uint8_t led_status;
	uint8_t params;
}LedsMixStructure;














void Leds_Control(void);




#endif //__LED_TASK_H__


