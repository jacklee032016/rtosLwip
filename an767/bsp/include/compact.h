/**
 *
 *
 */

#ifndef __COMPACT_H__
#define __COMPACT_H__


#define SAM_PART_IS_DEFINED(part) (defined(__ ## part ## __))

#define SAME70Q ( \
		SAM_PART_IS_DEFINED(SAME70Q19) || \
		SAM_PART_IS_DEFINED(SAME70Q20) || \
		SAM_PART_IS_DEFINED(SAME70Q21) \
	)

#define SAME70 ( SAME70Q)

#define SAM ( SAME70 )

#define SAM3S  0
#define SAM3U  0
#define SAM3N 0
#define SAM3XA 0
#define SAM4S 0
#define SAM4L 0
#define SAMD20 0
#define SAMD21 0
#define SAMD09 0
#define SAMD10 0
#define SAMD11 0
#define SAMDA1 0
#define SAMD   0
#define SAMR21 0
#define SAMB11 0
#define SAML21 0
#define SAML22 0
#define SAMC20 0
#define SAMC21 0
#define SAM4E 0
#define SAM4N 0
#define SAM4C_0 0
#define SAM4C_1 0
#define SAM4C   0
#define SAM4CM_0 0
#define SAM4CM_1 0
#define SAM4CM   0
#define SAM4CP_0 0
#define SAM4CP_1 0
#define SAM4CP   0
#define SAMG 0
#define SAMB 0
#define SAMV71 0
#define SAMV70 0
#define SAMS70 0
#define SAM0 0

#define SAMG51 0
#define SAMG53 0
#define SAMG54 0
#define SAMG55 0

#define MEGA_RF 0
#define MEGA 0
#define XMEGA 0
#define UC3 0


/********** build options **************/
#define	MUXLAB_DEBUG_CHIP			1
#define	MUX_DEBUG_WHITS				0
#define	MUXLAB_VALIDATE_CHIP			1

//#define	DEBUG_SPI

/* more details in BIST */
#define	BSP_BIST_DEBUG				

/********  typedefs ******************/
#define _CODE	const

#define	xdata
#define	idata		volatile

#include <stdbool.h>
//typedef bool	BOOL; 
#define BOOL			bool

#define  BIT_0		0x0001
#define  BIT_1		0x0002
#define  BIT_2		0x0004
#define  BIT_3		0x0008
#define  BIT_4		0x0010
#define  BIT_5		0x0020
#define  BIT_6		0x0040
#define  BIT_7		0x0080
#define  BIT_8		0x0100
#define  BIT_9		0x0200
#define  BIT_10		0x0400
#define  BIT_11		0x0800
#define  BIT_12		0x1000
#define  BIT_13		0x2000
#define  BIT_14		0x4000
#define  BIT_15		0x8000


typedef    _CODE unsigned char    cBYTE;


#ifdef	ARM


/*
 * This file includes all API header files for the selected drivers from ASF.
 * Note: There might be duplicate includes required by more than one driver.
 *
 * The file is automatically generated and will be re-written when
 * running the ASF driver selector tool. Any changes will be discarded.
 */

// From module: Common SAM compiler driver
#include <compiler.h>

#include "same70_xplained.h"


#include "hardware.h"


// From module: IOPORT - General purpose I/O service
#include <ioport.h>


// From module: PIO - Parallel Input/Output Controller
#include <pio.h>

#include "gpio.h"

// From module: PMC - Power Management Controller
#include <pmc.h>
//#include <sleep.h>

// From module: System Clock Control - SAME70 implementation
#include <sysclk.h>

// From module: TC - Timer Counter
//#include <tc.h>


#include "delay.h"

#endif

#include <stdint.h>
#include <string.h>
#include <stdlib.h>		/*atoi */



/** Number of buffer for RX */
#define GMAC_RX_BUFFERS						3

/** Number of buffer for TX */
#define GMAC_TX_BUFFERS						3



/**
 * \brief Set peripheral mode for IOPORT pins.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param port IOPORT port to configure
 * \param masks IOPORT pin masks to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_port_peripheral_mode(port, masks, mode) \
	do {\
		ioport_set_port_mode(port, masks, mode);\
		ioport_disable_port(port, masks);\
	} while (0)

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)

/**
 * \brief Set input mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 * \param sense Sense for interrupt detection (\ref ioport_sense)
 */
#define ioport_set_pin_input_mode(pin, mode, sense) \
	do {\
		ioport_set_pin_dir(pin, IOPORT_DIR_INPUT);\
		ioport_set_pin_mode(pin, mode);\
		ioport_set_pin_sense_mode(pin, sense);\
	} while (0)



#define	MUX_RS232_DEBUG		1



#define MUX_MAKEU32(a,b,c,d) (((int)((a) & 0xff) << 24) | \
                               ((int)((b) & 0xff) << 16) | \
                               ((int)((c) & 0xff) << 8)  | \
                                (int)((d) & 0xff))


/* Maximum string size allowed (in bytes) in std C library. */
#ifndef __MUX_RELEASE__
#define MAX_STRING_SIZE         1024
#else
#define MAX_STRING_SIZE         256
#endif


#define CMD_BUFFER_SIZE   (1024)

/* commands in both */
#define	MUX_CMD_DEFAULT				"help"
#define	MUX_CMD_VERSION				"version"

#define	MUX_CMD_REBOOT				"reboot"
#define	MUX_CMD_FACTORY				"factory"

#define	MUX_CMD_TX					"tx"		/* configure as TX or RX */

#define	MUX_CMD_UPDATE				"update"	/* need update: enter into bootloader */
#define	MUX_CMD_CONFIG_FPGA			"cfs"			/* whether configure FPAG or not when MCU reboot */

#define	MUX_CMD_DEBUG				"debug"

#define	MUX_CMD_DEBUG_ENABLE		"enable"
#define	MUX_CMD_DEBUG_DISABLE		"disable"

/* commands in bootloader only */
#define	MUX_CMD_EFC_FLASH			"ift"

#define	MUX_CMD_SPI_FLASH_READ		"sfr"
#define	MUX_CMD_SPI_FLASH_ERASE		"sfe"

#define	MUX_CMD_BOOT					"boot"

#define	MUX_CMD_LOAD					"load"
#define	MUX_CMD_LOAD_X				"lrx"		/* Load Rtos YModem */
#define	MUX_CMD_LOAD_Y				"lry"		/* Load Rtos YModem */
#define	MUX_CMD_LOAD_K				"loadk"


#define	MUX_CMD_LOAD_FPGA_X			"lfx"		/* load FPGA image with XModem */
#define	MUX_CMD_LOAD_FPGA_Y			"lfy"		/* load FPGA image with XModem */


#define	MUX_CMD_BIST					"bist"


/* commands in RTOS only */
#define	MUX_CMD_TASKS					"tasks"
#define	MUX_CMD_STATS					"stats"
#define	MUX_CMD_HEAP					"heap"

#define	MUX_CMD_PING					"ping"
#define	MUX_CMD_IGMP					"igmp"

#define	MUX_CMD_IGMP_JOIN			"join"
#define	MUX_CMD_IGMP_LEAVE			"leave"

#define	MUX_CMD_NAME					"name"	/* change name of device */

#define	MUX_CMD_PARAMS				"params"
#define	MUX_CMD_NET_INFO				"net"
#define	MUX_CMD_MAC_INFO				"mac"	/* MAC address */

#define	MUX_CMD_LOCAL_INFO			"local"	/* MAC/IP and A/V port of local, both on TX/RX */
#define	MUX_CMD_DEST_INFO			"dest"	/* MAC/IP and A/V port of dest, on TX */

#define	MUX_CMD_TESTS					"tests"

#define	MUX_CMD_FPGA					"fpga"
#define	MUX_CMD_SPI					"spi"

#define	MUX_CMD_FPGA_READ			"rgr"
#define	MUX_CMD_FPGA_WRITE			"rgw"


#define	CMD_HELP_EFC_FLASH			MUX_NEW_LINE MUX_CMD_EFC_FLASH" [r/w/e] [pageNo.]:"MUX_NEW_LINE" Internal Flash Test(read/write/erase). 'pageNo.' is decimal "MUX_NEW_LINE

#define	CMD_HELP_SPI_FLASH			MUX_NEW_LINE MUX_CMD_SPI_FLASH_READ" [startPage [count]]:"MUX_NEW_LINE" SPI Flash Read. 'startPage' and 'count' are decimal "MUX_NEW_LINE
#define	CMD_HELP_SPI_FLASH_ERASE		MUX_NEW_LINE MUX_CMD_SPI_FLASH_ERASE" [startSector [count]]:"MUX_NEW_LINE" SPI Flash Erase. 'startSector' and 'count' are decimal "MUX_NEW_LINE

#define	CMD_HELP_BIST					MUX_NEW_LINE MUX_CMD_BIST":"MUX_NEW_LINE" Built-In Self-Test"MUX_NEW_LINE
#define	CMD_HELP_REBOOT				MUX_NEW_LINE MUX_CMD_REBOOT":"MUX_NEW_LINE" Reboot system"MUX_NEW_LINE

#define	CMD_HELP_UPDATE				MUX_NEW_LINE MUX_CMD_UPDATE " 1|0 "MUX_NEW_LINE" Set 1 to enter Bootloader to update"MUX_NEW_LINE
#define	CMD_HELP_FACTORY				MUX_NEW_LINE MUX_CMD_FACTORY" :"MUX_NEW_LINE" Restore factory configuration " MUX_NEW_LINE

#define	CMD_HELP_LOADX				MUX_NEW_LINE MUX_CMD_LOAD_X" :"MUX_NEW_LINE" Load Rtos through Xmodem serial port"MUX_NEW_LINE
#define	CMD_HELP_LOADY				MUX_NEW_LINE MUX_CMD_LOAD_Y" :"MUX_NEW_LINE" Load Rtos through Ymodem serial port"MUX_NEW_LINE
#define	CMD_HELP_LOAD_FPGA_X		MUX_NEW_LINE MUX_CMD_LOAD_FPGA_X" [startSector]:"MUX_NEW_LINE" Load Fpga image through XModem serial port"MUX_NEW_LINE
#define	CMD_HELP_LOAD_FPGA_Y		MUX_NEW_LINE MUX_CMD_LOAD_FPGA_Y" [startSector]:"MUX_NEW_LINE" Load Fpga image through YModem serial port"MUX_NEW_LINE

#define	CMD_HELP_VERSION				MUX_NEW_LINE MUX_CMD_VERSION":"MUX_NEW_LINE" Show version of current system"MUX_NEW_LINE

#define	CMD_HELP_HELP					MUX_NEW_LINE MUX_CMD_DEFAULT":"MUX_NEW_LINE" Lists all the registered commands"MUX_NEW_LINE


#define	KB(x)						((x) / 1024)
#define	PERCENTAGE(x,total)			(((x) * 100) / (total))

#define	UNIT_OF_KILO					1024
#define	UNIT_OF_MEGA					(UNIT_OF_KILO*UNIT_OF_KILO)
#define	UNIT_OF_GIGA					(UNIT_OF_KILO*UNIT_OF_MEGA)

#define MUX_ARRAYSIZE(x)		(sizeof(x)/sizeof((x)[0]))

#define MUX_MAKEU32(a,b,c,d) (((int)((a) & 0xff) << 24) | \
                               ((int)((b) & 0xff) << 16) | \
                               ((int)((c) & 0xff) << 8)  | \
                                (int)((d) & 0xff))




/** The MAX value of shifting. */
#define MAX_SHIFTING_NUMBER    (32)



#define	GET_BIT(value, bit)				(((value)>>(bit))&0x01)
#define	SET_BIT(value, bit)				((value) << (bit))



#define	CMN_SET_BIT(flags, bitPosition)	\
		flags |= SET_BIT(0x01, (bitPosition) ) 

#define	CMN_CLEAR_BIT(flags, bitPosition)	\
		flags &= ~(SET_BIT(0x01, (bitPosition) ) )	

#define	CMN_CHECK_BIT(flags, bitPosition)	\
		( (flags&SET_BIT(0x01,(bitPosition) ) )!=0 )

#define	CMN_SET_FLAGS(flags, value)	\
		flags |= (value) 

#define	CMN_CLEAR_FLAGS(flags, value)	\
		flags &= ~((value) ) )	

#define	CMN_CHECK_FLAGS(flags, value)	\
		( (flags&(value) )!=0 )


typedef enum flash_rc
{
	FLASH_RC_OK = 0,        //!< Operation OK
	FLASH_RC_YES = 1,       //!< Yes
	FLASH_RC_NO = 0,        //!< No
	FLASH_RC_ERROR = 0x10,  //!< General error
	FLASH_RC_NOPERM,		/* no perm */
	FLASH_RC_INVALID,       //!< Invalid argument input
	FLASH_RC_NOT_SUPPORT = 0xFFFFFFFF    //!< Operation is not supported
} flash_rc_t;


//COMPILER_PACK_SET(1);
#pragma		pack(1)


#include "muxSysParams.h"

struct _MUX_CLI_CMD;

/* return TRUE, more data needed; FALSE: no mor data */
typedef char (*MUX_CMD_CALLBACK)(const struct _MUX_CLI_CMD *cmd, char *outBuffer, size_t bufferSzie );

/* The structure that defines command line commands.  A command line command
should be defined by declaring a const structure of this type. */
struct _MUX_CLI_CMD
{
	const char * const			name;
	const char * const			helpString;/* String that describes how to use the command.  Should start with the command itself, and end with "\r\n".  For example "help: Returns a list of all the commands\r\n". */

	/* return MUX_TRUE, continue this command again */
	const MUX_CMD_CALLBACK	callback;
	
//	int8_t					numberOfParams;
};

typedef struct _MUX_CLI_CMD		MUX_CLI_CMD_T;

typedef char (*MUX_BIST_HANDLER)(char *outBuffer, size_t bufferSize );


/* from delay.h*/
#define	MUX_DELAY_S(ms)		delay_s((ms))
#define	MUX_DELAY_MS(ms)		delay_ms((ms))
#define	MUX_DELAY_US(ms)		delay_us((ms))


//#define	MUX_REBOOT()			rstc_start_software_reset(RSTC)
#define	MUX_REBOOT()			rstc_reset_processor_and_peripherals_and_ext()


#define	MUX_LOAD_OS()					loadApplication(AN767_MCU_MMAP_OS);

uint16_t crc16_ccitt(uint16_t crc_start, unsigned char *buf, int len);

#include "declarations.h"

#endif


