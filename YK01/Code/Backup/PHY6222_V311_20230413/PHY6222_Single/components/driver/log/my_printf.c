
#include "rom_sym_def.h"
#include "types.h"
#include <stdarg.h>
#include <string.h>
#include "uart.h"
#include "log.h"

#include "OSAL.h"
#include "radio433_protocol.h"
#include "frequency_config_table.h"
#include "UserTask.h"
#include "common.h"



#define ZEROPAD 1               // Pad with zero
#define SIGN    2               // Unsigned/signed long
#define PLUS    4               // Show plus
#define SPACE   8               // Space if plus
#define LEFT    16              // Left justified
#define SPECIAL 32              // 0x
#define LARGE   64              // Use 'ABCDEF' instead of 'abcdef'
#define is_digit(c) ((c) >= '0' && (c) <= '9')
static const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";
static const char* upper_digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

#define RN_CHAR					"\r\n"
#define YK01_VER				"V3.1.1"


extern uint8_t start_scan_flag;
extern volatile uint8_t receive_flag;

uint16_t at_version(uint32_t argc, uint8_t *argv[]);
uint16_t at_scaning(uint32_t argc, uint8_t *argv[]);
uint16_t at_scaned(uint32_t argc, uint8_t *argv[]);
uint16_t at_result(uint32_t argc, uint8_t *argv[]);
uint16_t at_sendcopy(uint32_t argc, uint8_t *argv[]);



// for at_cli_cmd.
uint8_t cmdstr[AT_CMD_LENGTH_MAX];
uint8_t cmdlen = 0;
volatile uint8_t uart_temp_value = 0;
volatile uint8_t uart_data_processing = 0;

const CLI_COMMAND cli_cmd_list[] =
{
		/* at+version cmd. */
    { (uint8_t *)"AT+VER", (uint8_t *)"get current version \n", at_version },

    { (uint8_t *)"AT+SCA", (uint8_t *)"start scan \n", at_scaning },

    { (uint8_t *)"AT+END", (uint8_t *)"stop scan \n", at_scaned },

    { (uint8_t *)"AT+RST", (uint8_t *)"query scan result \n", at_result },

    { (uint8_t *)"AT+SND", (uint8_t *)"send copy key \n", at_sendcopy },
	
};


uint16_t at_version(uint32_t argc, uint8_t *argv[])
{
	hal_uart_send_buff(UART0, (uint8_t *)cli_cmd_list[argc].desc, hal_my_strlen((uint8_t *)cli_cmd_list[argc].desc));
	hal_uart_send_buff(UART0, (uint8_t *)RN_CHAR, hal_my_strlen((uint8_t *)RN_CHAR));
	hal_uart_send_buff(UART0, (uint8_t *)YK01_VER, hal_my_strlen((uint8_t *)YK01_VER));
	hal_uart_send_buff(UART0, (uint8_t *)RN_CHAR, hal_my_strlen((uint8_t *)RN_CHAR));
	
	return 0;
}

uint16_t at_scaning(uint32_t argc, uint8_t *argv[])
{
	hal_uart_send_buff(UART0, (uint8_t *)cli_cmd_list[argc].desc, hal_my_strlen((uint8_t *)cli_cmd_list[argc].desc));
	
	start_scan_flag = 1;
	
	return 0;
}

uint16_t at_scaned(uint32_t argc, uint8_t *argv[])
{
	hal_uart_send_buff(UART0, (uint8_t *)cli_cmd_list[argc].desc, hal_my_strlen((uint8_t *)cli_cmd_list[argc].desc));
	
	start_scan_flag = 0;
	
	return 0;
}

uint16_t at_result(uint32_t argc, uint8_t *argv[])
{
	extern uint16_t current_freq;
	extern uint32_t current_key;
	extern timer_sequence_structure decode_buff;

	hal_uart_send_buff(UART0, (uint8_t *)cli_cmd_list[argc].desc, hal_my_strlen((uint8_t *)cli_cmd_list[argc].desc));

	if(receive_flag)
	{
		/* 获取当前频段 */
		current_freq = GET_CURRENT_FREQ_BAND_NUM(frequency_set_register.now_freq_num);	
		current_key = decode_buff.key_value;
		PRINT_DEBUG("\r\n In course of send key\r\n Current frequency is %d \r\n Key value is %06X \r\n", current_freq, decode_buff.key_value);
		
		return 1;
	}
	else
	{
		hal_uart_send_buff(UART0, (uint8_t *)"\r\n Not Get Key \r\n", 17);
	}
	
	return 0;
}

uint16_t at_sendcopy(uint32_t argc, uint8_t *argv[])
{
	hal_uart_send_buff(UART0, (uint8_t *)cli_cmd_list[argc].desc, hal_my_strlen((uint8_t *)cli_cmd_list[argc].desc));

	if(receive_flag)
	{
		send_rec_test(3, 0);
	}
	else
	{
		hal_uart_send_buff(UART0, (uint8_t *)"\r\n Not Get Key \r\n", 17);
	}
	
	return 0;
}

static size_t _strnlen(const char* s, size_t count)
{
    const char* sc;

    for (sc = s; *sc != '\0' && count--; ++sc);

    return sc - s;
}
static int skip_atoi(const char** s)
{
    int i = 0;

    while (is_digit(**s)) i = i * 10 + *((*s)++) - '0';

    return i;
}
static void number(std_putc putc, long num, int base, int size, int precision, int type)
{
    char c, sign, tmp[66];
    const char* dig = digits;
    int i;
    char tmpch;

    if (type & LARGE)  dig = upper_digits;

    if (type & LEFT) type &= ~ZEROPAD;

    if (base < 2 || base > 36) return;

    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;

    if (type & SIGN)
    {
        if (num < 0)
        {
            sign = '-';
            num = -num;
            size--;
        }
        else if (type & PLUS)
        {
            sign = '+';
            size--;
        }
        else if (type & SPACE)
        {
            sign = ' ';
            size--;
        }
    }

    if (type & SPECIAL)
    {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }

    i = 0;

    if (num == 0)
        tmp[i++] = '0';
    else
    {
        while (num != 0)
        {
            tmp[i++] = dig[((unsigned long)num) % (unsigned)base];
            num = ((unsigned long)num) / (unsigned)base;
        }
    }

    if (i > precision) precision = i;

    size -= precision;

    if (!(type & (ZEROPAD | LEFT)))
    {
        while (size-- > 0)
        {
            tmpch = ' ';
            putc(&tmpch, 1);
        }
    }

    if (sign)
    {
        putc(&sign, 1);
    }

    if (type & SPECIAL)
    {
        if (base == 8)
        {
            tmpch = '0';
            putc(&tmpch, 1);
        }
        else if (base == 16)
        {
            tmpch = '0';
            putc(&tmpch, 1);
            tmpch = digits[33];
            putc(&tmpch, 1);
        }
    }

    if (!(type & LEFT))
    {
        while (size-- > 0)
        {
            putc(&c, 1);
        }
    }

    while (i < precision--)
    {
        tmpch = '0';
        putc(&tmpch, 1);
    }

    while (i-- > 0)
    {
        tmpch = tmp[i];
        putc(&tmpch, 1);
    }

    while (size-- > 0)
    {
        tmpch = ' ';
        putc(&tmpch, 1);
    }
}



static void log_vsprintf(std_putc putc, const char* fmt, va_list args)
{
    int len;
    unsigned long num;
    int base;
    char* s;
    int flags;            // Flags to number()
    int field_width;      // Width of output field
    int precision;        // Min. # of digits for integers; max number of chars for from string
    int qualifier;        // 'h', 'l', or 'L' for integer fields
    char* tmpstr = NULL;
    int tmpstr_size = 0;
    char tmpch;

    for (; *fmt; fmt++)
    {
        if (*fmt != '%')
        {
            if (tmpstr == NULL)
            {
                tmpstr = (char*)fmt;
                tmpstr_size = 0;
            }

            tmpstr_size ++;
            continue;
        }
        else if (tmpstr_size)
        {
            putc(tmpstr, tmpstr_size);
            tmpstr = NULL;
            tmpstr_size = 0;
        }

        // Process flags
        flags = 0;
repeat:
        fmt++; // This also skips first '%'

        switch (*fmt)
        {
        case '-':
            flags |= LEFT;
            goto repeat;

        case '+':
            flags |= PLUS;
            goto repeat;

        case ' ':
            flags |= SPACE;
            goto repeat;

        case '#':
            flags |= SPECIAL;
            goto repeat;

        case '0':
            flags |= ZEROPAD;
            goto repeat;
        }

        // Get field width
        field_width = -1;

        if (is_digit(*fmt))
            field_width = skip_atoi(&fmt);
        else if (*fmt == '*')
        {
            fmt++;
            field_width = va_arg(args, int);

            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        // Get the precision
        precision = -1;

        if (*fmt == '.')
        {
            ++fmt;

            if (is_digit(*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*')
            {
                ++fmt;
                precision = va_arg(args, int);
            }

            if (precision < 0) precision = 0;
        }

        // Get the conversion qualifier
        qualifier = -1;

        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
        {
            qualifier = *fmt;
            fmt++;
        }

        // Default base
        base = 10;

        switch (*fmt)
        {
        case 'c':
            if (!(flags & LEFT))
            {
                while (--field_width > 0)
                {
                    tmpch = ' ';
                    putc(&tmpch, 1);
                }
            }

            tmpch = (unsigned char)va_arg(args, int);
            putc(&tmpch, 1);

            while (--field_width > 0)
            {
                tmpch = ' ';
                putc(&tmpch, 1);
            }

            continue;

        case 's':
            s = va_arg(args, char*);

            if (!s)
                s = "<NULL>";

            len = _strnlen(s, precision);

            if (!(flags & LEFT))
            {
                while (len < field_width--)
                {
                    tmpch = ' ';
                    putc(&tmpch, 1);
                }
            }

            putc(s, len);

            while (len < field_width--)
            {
                tmpch = ' ';
                putc(&tmpch, 1);
            }

            continue;

        case 'p':
            if (field_width == -1)
            {
                field_width = 2 * sizeof(void*);
                flags |= ZEROPAD;
            }

            number(putc,(unsigned long)va_arg(args, void*), 16, field_width, precision, flags);
            continue;

        case 'n':
            continue;

        case 'A':
            continue;

        // Integer number formats - set up the flags and "break"
        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;

        case 'x':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;

        case 'u':
            break;

        default:
            if (*fmt != '%')
            {
                tmpch = '%';
                putc(&tmpch, 1);
            }

            if (*fmt)
            {
                tmpch = *fmt;
                putc(&tmpch, 1);
            }
            else
            {
                --fmt;
            }

            continue;
        }

        if (qualifier == 'l')
            num = va_arg(args, unsigned long);
        else if (qualifier == 'h')
        {
            if (flags & SIGN)
                num = va_arg(args, int);
            else
                num = va_arg(args, unsigned int);
        }
        else if (flags & SIGN)
            num = va_arg(args, int);
        else
            num = va_arg(args, unsigned int);

        number(putc, num, base, field_width, precision, flags);
    }

    if (tmpstr_size)
    {
        putc(tmpstr, tmpstr_size);
        tmpstr = NULL;
        tmpstr_size = 0;
    }
}

static void _uart_putc(char* data, uint16_t size)
{
    hal_uart_send_buff(UART0, (uint8_t*)data, size);
}

void dbg_printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log_vsprintf(_uart_putc, format, args);
    va_end(args);
}

void ProcessUartData(uart_Evt_t *evt)
{
	static uint8_t i = 0;
	uint8_t ch = 0;
	
  switch(evt->type){
    case  UART_EVT_TYPE_RX_DATA:
		case  UART_EVT_TYPE_RX_DATA_TO:
        {
					ch = *(evt->data);
					cmdstr[i++] = ch;
					if(uart_data_processing == 0)
					{
						switch(i)
						{
							case 1:	
								if(cmdstr[i - 1] != 'A')
								{
									i = 0;
								}
								break;
							case 2:	
								if(cmdstr[i - 1] != 'T')
								{
									i = 0;
								}
							default:
								if((cmdstr[i - 2] == 0x0D) && (cmdstr[i - 1] == 0x0A))
								{
									cmdlen = i - 1;
									cmdstr[i - 2] = 0; 	//\r\n不加入buff
									i = 0;
									osal_set_event( UserBase2_TaskID, USER_TASK2_EVENT1 );		//通知任务，进行新的指令处理
									
								}
								else if(i > 60)
								{
									osal_memset(cmdstr, 0, AT_CMD_LENGTH_MAX);		//数据溢出错误清除
									i = 0;
								}
								break;
						}
						
						if(ch == '1')
						{
							uart_temp_value = 1;
							osal_set_event( UserBase2_TaskID, USER_TASK2_EVENT1 );		//通知任务，进行新的指令处理
						}
						else if(ch == '2')
						{
							uart_temp_value = 2;
							osal_set_event( UserBase2_TaskID, USER_TASK2_EVENT1 );		//通知任务，进行新的指令处理
						}
						else if(ch == '3')
						{
							uart_temp_value = 3;
							osal_set_event( UserBase2_TaskID, USER_TASK2_EVENT1 );		//通知任务，进行新的指令处理
						}
						else if(ch == '4')
						{
							uart_temp_value = 4;
							osal_set_event( UserBase2_TaskID, USER_TASK2_EVENT1 );		//通知任务，进行新的指令处理
						}
						else if(ch == '5')
						{
							uart_temp_value = 5;
							osal_set_event( UserBase2_TaskID, USER_TASK2_EVENT1 );		//通知任务，进行新的指令处理
						}
					}
        }  
      break;
    case  UART_EVT_TYPE_TX_COMPLETED: // should not be here in AT mode.
      break;
    default:
      break;
  }  
}

void dbg_printf_init(void)
{
    uart_Cfg_t cfg =
    {
        .tx_pin = P9,
        .rx_pin = P10,
        .rts_pin = GPIO_DUMMY,
        .cts_pin = GPIO_DUMMY,
        .baudrate = 115200,
        .use_fifo = TRUE,
        .hw_fwctrl = FALSE,
        .use_tx_buf = FALSE,
        .parity     = FALSE,
        .evt_handler = ProcessUartData,//uart_input_evt,
    };
    hal_uart_init(cfg, UART0);//uart init
}

void my_dump_byte(uint8_t* pData, int dlen)
{
    for(int i=0; i<dlen; i++)
    {
        dbg_printf("%02x ",pData[i]);
    }

    dbg_printf("\n");
}


