#include "drv_cmt2300a.h"
#include "clock.h"


/* ************************************************************************
*  The following need to be modified by user
*  ************************************************************************ */
#define cmt_spi3_csb_out()      SET_GPIO_OUT(GPIO_SPI_CS)
#define cmt_spi3_fcsb_out()     SET_GPIO_OUT(GPIO_SPI_FIFO_CS)
#define cmt_spi3_sclk_out()     SET_GPIO_OUT(GPIO_SPI_CLK)
#define cmt_spi3_sdio_out()     SET_GPIO_OUT(GPIO_SPI_SDIO)
#define cmt_spi3_sdio_in()      SET_GPIO_IN(GPIO_SPI_SDIO)

#define cmt_spi3_csb_1()        SET_GPIO_H(GPIO_SPI_CS)
#define cmt_spi3_csb_0()        SET_GPIO_L(GPIO_SPI_CS)

#define cmt_spi3_fcsb_1()       SET_GPIO_H(GPIO_SPI_FIFO_CS)
#define cmt_spi3_fcsb_0()       SET_GPIO_L(GPIO_SPI_FIFO_CS)
    
#define cmt_spi3_sclk_1()       SET_GPIO_H(GPIO_SPI_CLK)
#define cmt_spi3_sclk_0()       SET_GPIO_L(GPIO_SPI_CLK)

#define cmt_spi3_sdio_1()       SET_GPIO_H(GPIO_SPI_SDIO)
#define cmt_spi3_sdio_0()       SET_GPIO_L(GPIO_SPI_SDIO)
#define cmt_spi3_sdio_read()    READ_GPIO_PIN(GPIO_SPI_SDIO)

#define cmt_GPIOx_1()      		SET_GPIO_H(CMT_GPIO1)
#define cmt_GPIOx_0()      		SET_GPIO_L(CMT_GPIO1)


/* ************************************************************************ */

void drv_cmt2300a_init(void)
{
    cmt_spi3_csb_1();
    cmt_spi3_csb_out();
    cmt_spi3_csb_1();   /* CSB has an internal pull-up resistor */
    
    cmt_spi3_sclk_0();
    cmt_spi3_sclk_out();
    cmt_spi3_sclk_0();   /* SCLK has an internal pull-down resistor */
    
    cmt_spi3_sdio_1();
    cmt_spi3_sdio_out();
    cmt_spi3_sdio_1();
    
    cmt_spi3_fcsb_1();
    cmt_spi3_fcsb_out();
    cmt_spi3_fcsb_1();  /* FCSB has an internal pull-up resistor */

    cmt_spi3_delay();
}

void cmt_spi3_delay(void)
{
    u32 n = 7;
    while(n--);
}

void cmt_spi3_delay_us(void)
{
    u16 n = 8;
    while(n--);
}


void cmt_spi3_send(u8 data8)
{
    u8 i;

    for(i=0; i<8; i++)
    {
        cmt_spi3_sclk_0();

        /* Send byte on the rising edge of SCLK */
        if(data8 & 0x80)
            cmt_spi3_sdio_1();
        else            
            cmt_spi3_sdio_0();

        cmt_spi3_delay();
//		WaitUs(500);
        data8 <<= 1;
        cmt_spi3_sclk_1();
        cmt_spi3_delay();
    }
}

void cmt_spi3_send_bit(u32 start_len, s32 ptime, s32 ntime)
{
	/* Send byte on the rising edge of SCLK */
	cmt_GPIOx_1();
	WaitUs(ptime);
	/* Send byte on the rising edge of SCLK */	
	cmt_GPIOx_0();
	WaitUs(ntime);

}

void cmt_spi3_send_test(void)
{
	/* Send byte on the rising edge of SCLK */
	cmt_GPIOx_1();
	WaitUs(411);
	/* Send byte on the rising edge of SCLK */	
	//	cmt_GPIOx_0();

}



void cmt_spi3_send_end(u32 ptime, u32 ntime)
{
    cmt_spi3_sclk_0();

    /* Send byte on the rising edge of SCLK */
    cmt_spi3_sdio_0();
    WaitUs(ptime - 2);
    cmt_spi3_sclk_1();
    cmt_spi3_delay();
	cmt_spi3_sclk_0();
	

	
    /* Send byte on the rising edge of SCLK */	
	cmt_spi3_sdio_0();
	WaitUs(ntime - 2);
    cmt_spi3_sclk_1();
    cmt_spi3_delay();
	cmt_spi3_sclk_0();

}



u8 cmt_spi3_recv(void)
{
    u8 i;
    u8 data8 = 0xFF;

    for(i=0; i<8; i++)
    {
        cmt_spi3_sclk_0();
        cmt_spi3_delay();
        data8 <<= 1;

        cmt_spi3_sclk_1();

        /* Read byte on the rising edge of SCLK */
        if(cmt_spi3_sdio_read())
            data8 |= 0x01;
        else
            data8 &= ~0x01;

        cmt_spi3_delay();
    }

    return data8;
}

void cmt_spi3_write(u8 addr, u8 dat)
{
    cmt_spi3_sdio_1();
    cmt_spi3_sdio_out();

    cmt_spi3_sclk_0();
    cmt_spi3_sclk_out();
    cmt_spi3_sclk_0(); 

    cmt_spi3_fcsb_1();
    cmt_spi3_fcsb_out();
    cmt_spi3_fcsb_1();

    cmt_spi3_csb_0();

    /* > 0.5 SCLK cycle */
    cmt_spi3_delay();
    cmt_spi3_delay();

    /* r/w = 0 */
    cmt_spi3_send(addr&0x7F);

    cmt_spi3_send(dat);

    cmt_spi3_sclk_0();

    /* > 0.5 SCLK cycle */
    cmt_spi3_delay();
    cmt_spi3_delay();

    cmt_spi3_csb_1();
    
    cmt_spi3_sdio_1();
    cmt_spi3_sdio_in();
    
    cmt_spi3_fcsb_1();    
}

void cmt_spi3_read(u8 addr, u8* p_dat)
{
    cmt_spi3_sdio_1();
    cmt_spi3_sdio_out();

    cmt_spi3_sclk_0();
    cmt_spi3_sclk_out();
    cmt_spi3_sclk_0(); 

    cmt_spi3_fcsb_1();
    cmt_spi3_fcsb_out();
    cmt_spi3_fcsb_1();

    cmt_spi3_csb_0();

    /* > 0.5 SCLK cycle */
    cmt_spi3_delay();
    cmt_spi3_delay();

    /* r/w = 1 */
    cmt_spi3_send(addr|0x80);

    /* Must set SDIO to input before the falling edge of SCLK */
    cmt_spi3_sdio_in();
    
    *p_dat = cmt_spi3_recv();

    cmt_spi3_sclk_0();

    /* > 0.5 SCLK cycle */
    cmt_spi3_delay();
    cmt_spi3_delay();

    cmt_spi3_csb_1();
    
    cmt_spi3_sdio_1();
    cmt_spi3_sdio_in();
    
    cmt_spi3_fcsb_1();
}

void cmt_spi3_write_fifo(const u8* p_buf, u16 len)
{
    u16 i;

    cmt_spi3_fcsb_1();
    cmt_spi3_fcsb_out();
    cmt_spi3_fcsb_1();

    cmt_spi3_csb_1();
    cmt_spi3_csb_out();
    cmt_spi3_csb_1();

    cmt_spi3_sclk_0();
    cmt_spi3_sclk_out();
    cmt_spi3_sclk_0();

    cmt_spi3_sdio_out();

    for(i=0; i<len; i++)
    {
        cmt_spi3_fcsb_0();

        /* > 1 SCLK cycle */
        cmt_spi3_delay();
        cmt_spi3_delay();

        cmt_spi3_send(p_buf[i]);

        cmt_spi3_sclk_0();

        /* > 2 us */
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();

        cmt_spi3_fcsb_1();

        /* > 4 us */
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
    }

    cmt_spi3_sdio_in();
    
    cmt_spi3_fcsb_1();
}

void cmt_spi3_read_fifo(u8* p_buf, u16 len)
{
    u16 i;

    cmt_spi3_fcsb_1();
    cmt_spi3_fcsb_out();
    cmt_spi3_fcsb_1();

    cmt_spi3_csb_1();
    cmt_spi3_csb_out();
    cmt_spi3_csb_1();

    cmt_spi3_sclk_0();
    cmt_spi3_sclk_out();
    cmt_spi3_sclk_0();

    cmt_spi3_sdio_in();

    for(i=0; i<len; i++)
    {
        cmt_spi3_fcsb_0();

        /* > 1 SCLK cycle */
        cmt_spi3_delay();
        cmt_spi3_delay();

        p_buf[i] = cmt_spi3_recv();

        cmt_spi3_sclk_0();

        /* > 2 us */
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();

        cmt_spi3_fcsb_1();

        /* > 4 us */
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
        cmt_spi3_delay_us();
    }

    cmt_spi3_sdio_in();
    
    cmt_spi3_fcsb_1();
}








