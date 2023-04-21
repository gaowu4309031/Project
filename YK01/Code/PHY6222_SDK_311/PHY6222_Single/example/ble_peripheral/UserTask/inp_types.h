#ifndef __INP_TYPES_H
#define __INP_TYPES_H

#include "types.h"


//#define xdata
//#define data
//#define code const

#define NOP() __NOP()

#define INFINITE 0xFFFFFFFF


typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef  int32_t  vs32;
typedef  int16_t  vs16;
typedef  int8_t   vs8;

typedef  int32_t vsc32;  /*!< Read Only */
typedef  int16_t vsc16;  /*!< Read Only */
typedef  int8_t vsc8;   /*!< Read Only */

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */





#endif


