#ifndef __DRV_CMT2300A_H__
#define __DRV_CMT2300A_H__

#include "gpio.h"
#include "inp_types.h"

__inline void cmt_spi3_delay(void);
__inline void cmt_spi3_delay_us(void);


#define CMT_POWER_ON()				hal_gpio_write(GPIO_P00, 1); //绿
#define CMT_POWER_OFF()				hal_gpio_write(GPIO_P00, 0); //绿

#define CMT_GPIO3 								
#define CMT_GPIO2 								GPIO_P31
#define CMT_GPIO1								GPIO_P32
#define GPIO_SPI_SDIO							GPIO_P24
#define GPIO_SPI_CS								GPIO_P26
#define GPIO_SPI_CLK							GPIO_P23
#define GPIO_SPI_FIFO_CS						GPIO_P25




#define SET_GPIO_OUT(x)             hal_gpio_pin_init(x, OEN)
#define SET_GPIO_IN(x)              hal_gpio_pin_init(x, IE)

#define SET_GPIO_H(x)               hal_gpio_write(x, 1)
#define SET_GPIO_L(x)               hal_gpio_write(x, 0)
#define READ_GPIO_PIN(x)            hal_gpio_read(x) 


//#define SET_GPIO_OUT(x)             GPIO_Pin_Setting(x, x##_PIN, GPIO_Speed_50MHz, GPIO_Mode_Out_PP)
//#define SET_GPIO_IN(x)              GPIO_Pin_Setting(x, x##_PIN, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING)
//#define SET_GPIO_OD(x)              GPIO_Pin_Setting(x, x##_PIN, GPIO_Speed_50MHz, GPIO_Mode_Out_OD)
//#define SET_GPIO_AIN(x)             GPIO_Pin_Setting(x, x##_PIN, GPIO_Speed_50MHz, GPIO_Mode_AIN)
//#define SET_GPIO_AFOUT(x)           GPIO_Pin_Setting(x, x##_PIN, GPIO_Speed_50MHz, GPIO_Mode_AF_PP)
//#define SET_GPIO_AFOD(x)            GPIO_Pin_Setting(x, x##_PIN, GPIO_Speed_50MHz, GPIO_Mode_AF_OD)
//#define SET_GPIO_H(x)               (x->BSRR = x##_PIN) //GPIO_SetBits(x, x##_PIN)
//#define SET_GPIO_L(x)               (x->BRR  = x##_PIN) //GPIO_ResetBits(x, x##_PIN)
//#define READ_GPIO_PIN(x)            (((x->IDR & x##_PIN)!=Bit_RESET) ?1 :0) //GPIO_ReadInputDataBit(x, x##_PIN) 


void drv_cmt2300a_init(void);
void cmt_spi3_send(u8 data8);
void cmt_spi3_send_bit(u32 start_len, s32 ptime, s32 ntime);
void cmt_spi3_send_end(u32 ptime, u32 ntime);
void cmt_spi3_send_test(void);



u8 cmt_spi3_recv(void);

void cmt_spi3_write(u8 addr, u8 dat);
void cmt_spi3_read(u8 addr, u8* p_dat);

void cmt_spi3_write_fifo(const u8* p_buf, u16 len);
void cmt_spi3_read_fifo(u8* p_buf, u16 len);

#endif


