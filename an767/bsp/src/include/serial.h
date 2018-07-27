/**
 * \file
 *
 * \brief Serial Mode management
 *
 *
 */

#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED


/**
 * \typedef usart_if
 *
 * This type can be used independently to refer to USART module for the
 * architecture used. It refers to the correct type definition for the
 * architecture, ie. USART_t* for XMEGA or avr32_usart_t* for UC3.
 */

#include "compact.h"
#include "sysclk.h"

#include "uart.h"
#include "usart.h"

/** 
 * \name Serial Management Configuration
 */

/** Input parameters when initializing RS232 and similar modes. */
typedef struct uart_rs232_options
{
	/** Set baud rate of the USART (unused in slave modes). */
	uint32_t baudrate;

	/** Number of bits to transmit as a character (5-bit to 9-bit). */
	uint32_t charlength;

	/**
	 * Parity type: USART_PMODE_DISABLED_gc, USART_PMODE_EVEN_gc,
	 * USART_PMODE_ODD_gc.
	 */
	uint32_t paritytype;

	/** 1, 1.5 or 2 stop bits. */
	uint32_t stopbits;

} usart_rs232_options_t;

typedef usart_rs232_options_t usart_serial_options_t;

typedef Usart *usart_if;

/** 
 * \brief Initializes the Usart in master mode.
 *
 * \param p_usart  Base address of the USART instance.
 * \param opt      Options needed to set up RS232 communication (see
 * \ref usart_options_t).
 */
static inline void usart_serial_init(usart_if p_usart, usart_serial_options_t *opt)
{
	sam_uart_opt_t uart_settings;
	uart_settings.ul_mck = sysclk_get_peripheral_hz();
	uart_settings.ul_baudrate = opt->baudrate;
	uart_settings.ul_mode = opt->paritytype;

	sam_usart_opt_t usart_settings;
	usart_settings.baudrate = opt->baudrate;
	usart_settings.char_length = opt->charlength;
	usart_settings.parity_type = opt->paritytype;
	usart_settings.stop_bits= opt->stopbits;
	usart_settings.channel_mode= US_MR_CHMODE_NORMAL;
	
#ifdef UART
	if (UART == (Uart*)p_usart) {
		sysclk_enable_peripheral_clock(ID_UART);
		/* Configure UART */
		uart_init((Uart*)p_usart, &uart_settings);
	}
#else
# ifdef UART0
	if (UART0 == (Uart*)p_usart) {
		sysclk_enable_peripheral_clock(ID_UART0);
		/* Configure UART */
		uart_init((Uart*)p_usart, &uart_settings);
	}
# endif
# ifdef UART1
	if (UART1 == (Uart*)p_usart) {
		sysclk_enable_peripheral_clock(ID_UART1);
		/* Configure UART */
		uart_init((Uart*)p_usart, &uart_settings);
	}
# endif
# ifdef UART2
	if (UART2 == (Uart*)p_usart) {
		sysclk_enable_peripheral_clock(ID_UART2);
		/* Configure UART */
		uart_init((Uart*)p_usart, &uart_settings);
	}
# endif
# ifdef UART3
	if (UART3 == (Uart*)p_usart) {
		sysclk_enable_peripheral_clock(ID_UART3);
		/* Configure UART */
		uart_init((Uart*)p_usart, &uart_settings);
	}
# endif
#endif /* ifdef UART */


#ifdef USART
	if (USART == p_usart) {
		sysclk_enable_peripheral_clock(ID_USART);
		/* Configure USART */
		usart_init_rs232(p_usart, &usart_settings,
				sysclk_get_peripheral_hz());

		/* Enable the receiver and transmitter. */
		usart_enable_tx(p_usart);
		usart_enable_rx(p_usart);
	}
#else
# ifdef USART0
	if (USART0 == p_usart) {
		sysclk_enable_peripheral_clock(ID_USART0);

		/* Configure USART */
		usart_init_rs232(p_usart, &usart_settings,
				sysclk_get_peripheral_hz());

		/* Enable the receiver and transmitter. */
		usart_enable_tx(p_usart);
		usart_enable_rx(p_usart);
	}
# endif
# ifdef USART1
	if (USART1 == p_usart) {
		sysclk_enable_peripheral_clock(ID_USART1);
		/* Configure USART */
		usart_init_rs232(p_usart, &usart_settings,
				sysclk_get_peripheral_hz());

		/* Enable the receiver and transmitter. */
		usart_enable_tx(p_usart);
		usart_enable_rx(p_usart);
	}
# endif
# ifdef USART2
	if (USART2 == p_usart) {

		sysclk_enable_peripheral_clock(ID_USART2);
		/* Configure USART */
		usart_init_rs232(p_usart, &usart_settings,
				sysclk_get_peripheral_hz());

		/* Enable the receiver and transmitter. */
		usart_enable_tx(p_usart);
		usart_enable_rx(p_usart);
	}
# endif
# ifdef USART3
	if (USART3 == p_usart) {

		sysclk_enable_peripheral_clock(ID_USART3);

		/* Configure USART */
		usart_init_rs232(p_usart, &usart_settings,
				sysclk_get_peripheral_hz());

		/* Enable the receiver and transmitter. */
		usart_enable_tx(p_usart);
		usart_enable_rx(p_usart);
	}
# endif
# ifdef USART4
	if (USART4 == p_usart) {

		sysclk_enable_peripheral_clock(ID_USART4);
		/* Configure USART */
		usart_init_rs232(p_usart, &usart_settings,
				sysclk_get_peripheral_hz());

		/* Enable the receiver and transmitter. */
		usart_enable_tx(p_usart);
		usart_enable_rx(p_usart);
	}
# endif
# ifdef USART5
	if (USART5 == p_usart) {
		sysclk_enable_peripheral_clock(ID_USART5);
		/* Configure USART */
		usart_init_rs232(p_usart, &usart_settings,
				sysclk_get_peripheral_hz());

		/* Enable the receiver and transmitter. */
		usart_enable_tx(p_usart);
		usart_enable_rx(p_usart);
	}
# endif
# ifdef USART6
	if (USART6 == p_usart) {
		sysclk_enable_peripheral_clock(ID_USART6);
		/* Configure USART */
		usart_init_rs232(p_usart, &usart_settings,
				sysclk_get_peripheral_hz());

		/* Enable the receiver and transmitter. */
		usart_enable_tx(p_usart);
		usart_enable_rx(p_usart);
	}
# endif
# ifdef USART7
	if (USART7 == p_usart) {

		sysclk_enable_peripheral_clock(ID_USART7);
		/* Configure USART */
		usart_init_rs232(p_usart, &usart_settings,
				sysclk_get_peripheral_hz());

		/* Enable the receiver and transmitter. */
		usart_enable_tx(p_usart);
		usart_enable_rx(p_usart);
	}
# endif

#endif /* ifdef USART */

}

/**
 * \brief Sends a character with the USART.
 *
 * \param p_usart   Base address of the USART instance.
 * \param c       Character to write.
 *
 * \return Status.
 *   \retval 1  The character was written.
 *   \retval 0  The function timed out before the USART transmitter became
 * ready to send.
 */
static inline int usart_serial_putchar(usart_if p_usart, const uint8_t c)
{
#ifdef UART
	if (UART == (Uart*)p_usart) {
		while (uart_write((Uart*)p_usart, c)!=0);
		return 1;
	}
#else
# ifdef UART0
	if (UART0 == (Uart*)p_usart) {
		while (uart_write((Uart*)p_usart, c)!=0);
		return 1;
	}
# endif
# ifdef UART1
	if (UART1 == (Uart*)p_usart) {
		while (uart_write((Uart*)p_usart, c)!=0);
		return 1;
	}
# endif
# ifdef UART2
	if (UART2 == (Uart*)p_usart) {
		while (uart_write((Uart*)p_usart, c)!=0);
		return 1;
	}
# endif
# ifdef UART3
	if (UART3 == (Uart*)p_usart) {
		while (uart_write((Uart*)p_usart, c)!=0);
		return 1;
	}
# endif
#endif /* ifdef UART */


#ifdef USART
	if (USART == p_usart) {
		while (usart_write(p_usart, c)!=0);
		return 1;
	}
#else
# ifdef USART0
	if (USART0 == p_usart) {
		while (usart_write(p_usart, c)!=0);
		return 1;
	}
# endif
# ifdef USART1
	if (USART1 == p_usart) {
		while (usart_write(p_usart, c)!=0);
		return 1;
	}
# endif
# ifdef USART2
	if (USART2 == p_usart) {
		while (usart_write(p_usart, c)!=0);
		return 1;
	}
# endif
# ifdef USART3
	if (USART3 == p_usart) {
		while (usart_write(p_usart, c)!=0);
		return 1;
	}
# endif
# ifdef USART4
	if (USART4 == p_usart) {
		while (usart_write(p_usart, c)!=0);
		return 1;
	}
# endif
# ifdef USART5
	if (USART5 == p_usart) {
		while (usart_write(p_usart, c)!=0);
		return 1;
	}
# endif
# ifdef USART6
	if (USART6 == p_usart) {
		while (usart_write(p_usart, c)!=0);
		return 1;
	}
# endif
# ifdef USART7
	if (USART7 == p_usart) {
		while (usart_write(p_usart, c)!=0);
		return 1;
	}
# endif
#endif /* ifdef USART */

	return 0;
}
/**
 * \brief Waits until a character is received, and returns it.
 *
 * \param p_usart   Base address of the USART instance.
 * \param data   Data to read
 *
 */
static inline void usart_serial_getchar(usart_if p_usart, uint8_t *data)
{
	uint32_t val = 0;

	/* Avoid Cppcheck Warning */
	UNUSED(val);

#ifdef UART
	if (UART == (Uart*)p_usart) {
		while (uart_read((Uart*)p_usart, data));
	}
#else
# ifdef UART0
	if (UART0 == (Uart*)p_usart) {
		while (uart_read((Uart*)p_usart, data));
	}
# endif
# ifdef UART1
	if (UART1 == (Uart*)p_usart) {
		while (uart_read((Uart*)p_usart, data));
	}
# endif
# ifdef UART2
	if (UART2 == (Uart*)p_usart) {
		while (uart_read((Uart*)p_usart, data));
	}
# endif
# ifdef UART3
	if (UART3 == (Uart*)p_usart) {
		while (uart_read((Uart*)p_usart, data));
	}
# endif
#endif /* ifdef UART */


#ifdef USART
	if (USART == p_usart) {
		while (usart_read(p_usart, &val));
		*data = (uint8_t)(val & 0xFF);
	}
#else
# ifdef USART0
	if (USART0 == p_usart) {
		while (usart_read(p_usart, &val));
		*data = (uint8_t)(val & 0xFF);
	}
# endif
# ifdef USART1
	if (USART1 == p_usart) {
		while (usart_read(p_usart, &val));
		*data = (uint8_t)(val & 0xFF);
	}
# endif
# ifdef USART2
	if (USART2 == p_usart) {
		while (usart_read(p_usart, &val));
		*data = (uint8_t)(val & 0xFF);
	}
# endif
# ifdef USART3
	if (USART3 == p_usart) {
		while (usart_read(p_usart, &val));
		*data = (uint8_t)(val & 0xFF);
	}
# endif
# ifdef USART4
	if (USART4 == p_usart) {
		while (usart_read(p_usart, &val));
		*data = (uint8_t)(val & 0xFF);
	}
# endif
# ifdef USART5
	if (USART5 == p_usart) {
		while (usart_read(p_usart, &val));
		*data = (uint8_t)(val & 0xFF);
	}
# endif
# ifdef USART6
	if (USART6 == p_usart) {
		while (usart_read(p_usart, &val));
		*data = (uint8_t)(val & 0xFF);
	}
# endif
# ifdef USART7
	if (USART7 == p_usart) {
		while (usart_read(p_usart, &val));
		*data = (uint8_t)(val & 0xFF);
	}
# endif
#endif /* ifdef USART */

}

/**
 * \brief Check if Received data is ready.
 *
 * \param p_usart   Base address of the USART instance.
 *
 * \retval 1 One data has been received.
 * \retval 0 No data has been received.
 */
static inline uint32_t usart_serial_is_rx_ready(usart_if p_usart)
{
#ifdef UART
	if (UART == (Uart*)p_usart) {
		return uart_is_rx_ready((Uart*)p_usart);
	}
#else
# ifdef UART0
	if (UART0 == (Uart*)p_usart) {
		return uart_is_rx_ready((Uart*)p_usart);
	}
# endif
# ifdef UART1
	if (UART1 == (Uart*)p_usart) {
		return uart_is_rx_ready((Uart*)p_usart);
	}
# endif
# ifdef UART2
	if (UART2 == (Uart*)p_usart) {
		return uart_is_rx_ready((Uart*)p_usart);
	}
# endif
# ifdef UART3
	if (UART3 == (Uart*)p_usart) {
		return uart_is_rx_ready((Uart*)p_usart);
	}
# endif
#endif /* ifdef UART */


#ifdef USART
	if (USART == p_usart) {
		return usart_is_rx_ready(p_usart);
	}
#else
# ifdef USART0
	if (USART0 == p_usart) {
		return usart_is_rx_ready(p_usart);
	}
# endif
# ifdef USART1
	if (USART1 == p_usart) {
		return usart_is_rx_ready(p_usart);
	}
# endif
# ifdef USART2
	if (USART2 == p_usart) {
		return usart_is_rx_ready(p_usart);
	}
# endif
# ifdef USART3
	if (USART3 == p_usart) {
		return usart_is_rx_ready(p_usart);
	}
# endif
# ifdef USART4
	if (USART4 == p_usart) {
		return usart_is_rx_ready(p_usart);
	}
# endif
# ifdef USART5
	if (USART5 == p_usart) {
		return usart_is_rx_ready(p_usart);
	}
# endif
# ifdef USART6
	if (USART6 == p_usart) {
		return usart_is_rx_ready(p_usart);
	}
# endif
# ifdef USART7
	if (USART7 == p_usart) {
		return usart_is_rx_ready(p_usart);
	}
# endif
#endif /* ifdef USART */

	return 0;
}

/**
 * \brief Send a sequence of bytes to a USART device
 *
 * \param usart Base address of the USART instance.
 * \param data   data buffer to write
 * \param len    Length of data
 *
 */
char serial_write_packet( const uint8_t *data, size_t len);

/**
 * \brief Receive a sequence of bytes to a USART device
 *
 * \param usart Base address of the USART instance.
 * \param data   data buffer to write
 * \param len    Length of data
 *
 */
char serial_read_packet(uint8_t *data, size_t len);

/**
 *
 * \defgroup serial_group Serial Interface (Serial)
 *
 * See \ref serial_quickstart.
 *
 * This is the common API for serial interface. Additional features are available
 * in the documentation of the specific modules.
 *
 * \section serial_group_platform Platform Dependencies
 *
 * The serial API is partially chip- or platform-specific. While all
 * platforms provide mostly the same functionality, there are some
 * variations around how different bus types and clock tree structures
 * are handled.
 *
 * The following functions are available on all platforms, but there may
 * be variations in the function signature (i.e. parameters) and
 * behaviour. These functions are typically called by platform-specific
 * parts of drivers, and applications that aren't intended to be
 * portable:
 *   - usart_serial_init()
 *   - usart_serial_putchar()
 *   - usart_serial_getchar()
 *   - usart_serial_write_packet()
 *   - usart_serial_read_packet()
 *
 *
 * @{
 */
 
//! @}

/**
 * \page serial_quickstart Quick start guide for Serial Interface service
 *
 * This is the quick start guide for the \ref serial_group "Serial Interface module", with
 * step-by-step instructions on how to configure and use the serial in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section serial_use_cases Serial use cases
 * - \ref serial_basic_use_case
 * - \subpage serial_use_case_1
 *
 * \section serial_basic_use_case Basic use case - transmit a character
 * In this use case, the serial module is configured for:
 * - Using USARTD0
 * - Baudrate: 9600
 * - Character length: 8 bit
 * - Parity mode: Disabled
 * - Stop bit: None
 * - RS232 mode
 *
 * The use case waits for a received character on the configured USART and
 * echoes the character back to the same USART.
 *
 * \section serial_basic_use_case_setup Setup steps
 *
 * \subsection serial_basic_use_case_setup_prereq Prerequisites
 * -# \ref sysclk_group "System Clock Management (sysclk)"
 *
 * \subsection serial_basic_use_case_setup_code Example code
 * The following configuration must be added to the project (typically to a 
 * conf_uart_serial.h file, but it can also be added to your main application file.)
 *
 * \note The following takes SAM3X configuration for example, other devices have similar
 * configuration, but their parameters may be different, refer to corresponding header files.
 *
 * \code
	#define USART_SERIAL                     &USARTD0
	#define USART_SERIAL_BAUDRATE            9600
	#define USART_SERIAL_CHAR_LENGTH         US_MR_CHRL_8_BIT
	#define USART_SERIAL_PARITY              US_MR_PAR_NO
	#define USART_SERIAL_STOP_BIT            false
\endcode
 *
 * A variable for the received byte must be added:
 * \code uint8_t received_byte; \endcode
 *
 * Add to application initialization:
 * \code
	    sysclk_init();

	    static usart_serial_options_t usart_options = {
	       .baudrate = USART_SERIAL_BAUDRATE,
	       .charlength = USART_SERIAL_CHAR_LENGTH,
	       .paritytype = USART_SERIAL_PARITY,
	       .stopbits = USART_SERIAL_STOP_BIT
	    };

	    usart_serial_init(USART_SERIAL, &usart_options);
\endcode
 *
 * \subsection serial_basic_use_case_setup_flow Workflow
 * -# Initialize system clock:
 *   - \code sysclk_init(); \endcode
 * -# Create serial USART options struct:
 *   - \code
	static usart_serial_options_t usart_options = {
	   .baudrate = USART_SERIAL_BAUDRATE,
	   .charlength = USART_SERIAL_CHAR_LENGTH,
	   .paritytype = USART_SERIAL_PARITY,
	   .stopbits = USART_SERIAL_STOP_BIT
	};
\endcode
 * -# Initialize the serial service:
 *   - \code usart_serial_init(USART_SERIAL, &usart_options);\endcode
 *
 * \section serial_basic_use_case_usage Usage steps
 *
 * \subsection serial_basic_use_case_usage_code Example code
 * Add to application C-file:
 * \code
	usart_serial_getchar(USART_SERIAL, &received_byte);
	usart_serial_putchar(USART_SERIAL, received_byte);
\endcode
 *
 * \subsection serial_basic_use_case_usage_flow Workflow
 * -# Wait for reception of a character:
 *   - \code usart_serial_getchar(USART_SERIAL, &received_byte); \endcode
 * -# Echo the character back:
 *   - \code usart_serial_putchar(USART_SERIAL, received_byte); \endcode
 */

/**
 * \page serial_use_case_1 Advanced use case - Send a packet of serial data
 *
 * In this use case, the USART module is configured for:
 * - Using USARTD0
 * - Baudrate: 9600
 * - Character length: 8 bit
 * - Parity mode: Disabled
 * - Stop bit: None
 * - RS232 mode
 *
 * The use case sends a string of text through the USART.
 *
 * \section serial_use_case_1_setup Setup steps
 *
 * \subsection serial_use_case_1_setup_prereq Prerequisites
 * -# \ref sysclk_group "System Clock Management (sysclk)"
 *
 * \subsection serial_use_case_1_setup_code Example code
 * The following configuration must be added to the project (typically to a 
 * conf_uart_serial.h file, but it can also be added to your main application file.):
 *
 * \note The following takes SAM3X configuration for example, other devices have similar
 * configuration, but their parameters may be different, refer to corresponding header files.
 *
 * \code
	#define USART_SERIAL                     &USARTD0
	#define USART_SERIAL_BAUDRATE            9600
	#define USART_SERIAL_CHAR_LENGTH         US_MR_CHRL_8_BIT
	#define USART_SERIAL_PARITY              US_MR_PAR_NO
	#define USART_SERIAL_STOP_BIT            false
\endcode
 *
 * Add to application initialization:
 * \code
	    sysclk_init();

	    static usart_serial_options_t usart_options = {
	       .baudrate = USART_SERIAL_BAUDRATE,
	       .charlength = USART_SERIAL_CHAR_LENGTH,
	       .paritytype = USART_SERIAL_PARITY,
	       .stopbits = USART_SERIAL_STOP_BIT
	    };

	    usart_serial_init(USART_SERIAL, &usart_options);
\endcode
 *
 * \subsection serial_use_case_1_setup_flow Workflow
 * -# Initialize system clock:
 *   - \code sysclk_init(); \endcode
 * -# Create USART options struct:
 *   - \code
	static usart_serial_options_t usart_options = {
	   .baudrate = USART_SERIAL_BAUDRATE,
	   .charlength = USART_SERIAL_CHAR_LENGTH,
	   .paritytype = USART_SERIAL_PARITY,
	   .stopbits = USART_SERIAL_STOP_BIT
	};
\endcode
 * -# Initialize in RS232 mode:
 *   - \code usart_serial_init(USART_SERIAL_EXAMPLE, &usart_options); \endcode
 *
 * \section serial_use_case_1_usage Usage steps
 *
 * \subsection serial_use_case_1_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
	usart_serial_write_packet(USART_SERIAL, "Test String", strlen("Test String"));
\endcode
 *
 * \subsection serial_use_case_1_usage_flow Workflow
 * -# Write a string of text to the USART:
 *   - \code usart_serial_write_packet(USART_SERIAL, "Test String", strlen("Test String")); \endcode
 */

#endif /* SERIAL_H_INCLUDED */
