/*
 * lwIP abstraction layer for SAM.
 */

#ifndef CC_H_INCLUDED
#define CC_H_INCLUDED

#include <stdio.h>
#include <stdint.h>

#include "extSysParams.h"

/* Define platform endianness */
#ifndef	BYTE_ORDER	/* toolchains gcc has defined it for AMTEL: arm-none-eabi\include\machine\endian.h */
#define BYTE_ORDER	LITTLE_ENDIAN
#endif

#ifndef		EXT_LAB
/* following options can not be used with LwIP, instead arch.h. J.L. */
/* Types based on stdint.h */
typedef uint8_t            u8_t;
typedef int8_t             s8_t;
typedef uint16_t           u16_t;
typedef int16_t            s16_t;
	/* to comment the warn in debug output. J.L. */
typedef uint32_t           u32_t;
//typedef unsigned int           u32_t;

typedef int32_t            s32_t;
typedef uintptr_t          mem_ptr_t;

/* Define (sn)printf formatters for these lwIP types */
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"
#endif

/* Compiler hints for packing lwip's structures */
#if defined(__CC_ARM)
    /* Setup PACKing macros for MDK Tools */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((packed))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#elif defined (__ICCARM__)
    /* Setup PACKing macros for EWARM Tools */
#define PACK_STRUCT_BEGIN __packed
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#elif defined (__GNUC__)
    /* Setup PACKing macros for GCC Tools */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((packed))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#else
#error "This compiler does not support."
#endif

/* don't use timeval in LwIP. Instead timeval from libc of toolchain. J.L. */
#define LWIP_TIMEVAL_PRIVATE			0

/* define LWIP_COMPAT_MUTEX
    to let sys.h use binary semaphores instead of mutexes - as before in 1.3.2
    Refer CHANGELOG
*/
#define  LWIP_COMPAT_MUTEX			1

/* Make lwip/arch.h define the codes which are used throughout */
#define LWIP_PROVIDE_ERRNO

/* Debug facilities. LWIP_DEBUG must be defined to read output */
#ifdef LWIP_DEBUG
#define LWIP_PLATFORM_DIAG(x)		{printf("[%s-%u]: ", __FILE__, __LINE__); printf x ;}
#define LWIP_PLATFORM_ASSERT(x)	{printf (ANSI_COLOR_RED"Assertion "); printf x; printf(" failed at line %d in %s" LWIP_NEW_LINE ERROR_TEXT_END, __LINE__, __FILE__); while(0);}
#else
#define LWIP_PLATFORM_DIAG(x)   {;}
#define LWIP_PLATFORM_ASSERT(x) {while (1);}
#endif

#endif

