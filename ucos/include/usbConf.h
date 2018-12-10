/**
 *
 * \brief USB configuration file
 */

#ifndef __USB_CONF_H__
#define __USB_CONF_H__

#include "compiler.h"

//! \name Vendor Identifier assigned by USB org to ATMEL
#define  USB_VID_ATMEL                             0x03EB


//! \name Product Identifier assigned by ATMEL to AVR applications
//! @{

//! \name The range from 2000h to 20FFh is reserved to the old PID for C51, MEGA, and others.
//! @{
#define  USB_PID_ATMEL_MEGA_HIDGENERIC             0x2013
#define  USB_PID_ATMEL_MEGA_HIDKEYBOARD            0x2017
#define  USB_PID_ATMEL_MEGA_CDC                    0x2018
#define  USB_PID_ATMEL_MEGA_AUDIO_IN               0x2019
#define  USB_PID_ATMEL_MEGA_MS                     0x201A
#define  USB_PID_ATMEL_MEGA_AUDIO_IN_OUT           0x201B
#define  USB_PID_ATMEL_MEGA_HIDMOUSE               0x201C
#define  USB_PID_ATMEL_MEGA_HIDMOUSE_CERTIF_U4     0x201D
#define  USB_PID_ATMEL_MEGA_CDC_MULTI              0x201E
#define  USB_PID_ATMEL_MEGA_MS_HIDMS_HID_USBKEY    0x2022
#define  USB_PID_ATMEL_MEGA_MS_HIDMS_HID_STK525    0x2023
#define  USB_PID_ATMEL_MEGA_MS_2                   0x2029
#define  USB_PID_ATMEL_MEGA_MS_HIDMS               0x202A
#define  USB_PID_ATMEL_MEGA_MS_3                   0x2032
#define  USB_PID_ATMEL_MEGA_LIBUSB                 0x2050
//! @}

//! \name The range 2100h to 21FFh is reserved to PIDs for AVR Tools.
//! @{
#define  USB_PID_ATMEL_XPLAINED                    0x2122
#define  USB_PID_ATMEL_XMEGA_USB_ZIGBIT_2_4GHZ     0x214A
#define  USB_PID_ATMEL_XMEGA_USB_ZIGBIT_SUBGHZ     0x214B
//! @}

//! \name The range 2300h to 23FFh is reserved to PIDs for demo from ASF1.7=>
//! @{
#define  USB_PID_ATMEL_UC3_ENUM                    0x2300
#define  USB_PID_ATMEL_UC3_MS                      0x2301
#define  USB_PID_ATMEL_UC3_MS_SDRAM_LOADER         0x2302
#define  USB_PID_ATMEL_UC3_EVK1100_CTRLPANEL       0x2303
#define  USB_PID_ATMEL_UC3_HID                     0x2304
#define  USB_PID_ATMEL_UC3_EVK1101_CTRLPANEL_HID   0x2305
#define  USB_PID_ATMEL_UC3_EVK1101_CTRLPANEL_HID_MS 0x2306
#define  USB_PID_ATMEL_UC3_CDC                     0x2307
#define  USB_PID_ATMEL_UC3_AUDIO_MICRO             0x2308
#define  USB_PID_ATMEL_UC3_CDC_DEBUG               0x2310 // Virtual Com (debug interface) on EVK11xx
#define  USB_PID_ATMEL_UC3_AUDIO_SPEAKER_MICRO     0x2311
#define  USB_PID_ATMEL_UC3_CDC_MSC                 0x2312
//! @}

//! \name The range 2400h to 24FFh is reserved to PIDs for ASF applications
//! @{
#define  USB_PID_ATMEL_ASF_HIDMOUSE                0x2400
#define  USB_PID_ATMEL_ASF_HIDKEYBOARD             0x2401
#define  USB_PID_ATMEL_ASF_HIDGENERIC              0x2402
#define  USB_PID_ATMEL_ASF_MSC                     0x2403
#define  USB_PID_ATMEL_ASF_CDC                     0x2404
#define  USB_PID_ATMEL_ASF_PHDC                    0x2405
#define  USB_PID_ATMEL_ASF_HIDMTOUCH               0x2406
#define  USB_PID_ATMEL_ASF_MSC_HIDMOUSE            0x2420
#define  USB_PID_ATMEL_ASF_MSC_HIDS_CDC            0x2421
#define  USB_PID_ATMEL_ASF_MSC_HIDKEYBOARD         0x2422
#define  USB_PID_ATMEL_ASF_VENDOR_CLASS            0x2423
#define  USB_PID_ATMEL_ASF_MSC_CDC                 0x2424
#define  USB_PID_ATMEL_ASF_TWO_CDC                 0x2425
#define  USB_PID_ATMEL_ASF_SEVEN_CDC               0x2426
#define  USB_PID_ATMEL_ASF_XPLAIN_BC_POWERONLY     0x2430
#define  USB_PID_ATMEL_ASF_XPLAIN_BC_TERMINAL      0x2431
#define  USB_PID_ATMEL_ASF_XPLAIN_BC_TOUCH         0x2432
#define  USB_PID_ATMEL_ASF_AUDIO_SPEAKER           0x2433
#define  USB_PID_ATMEL_ASF_XMEGA_B1_XPLAINED       0x2434
//! @}

//! \name The range 2F00h to 2FFFh is reserved to official PIDs for AVR bootloaders
//! Note, !!!! don't use this range for demos or examples !!!!
//! @{
#define  USB_PID_ATMEL_DFU_ATXMEGA64C3             0x2FD6
#define  USB_PID_ATMEL_DFU_ATXMEGA128C3            0x2FD7
#define  USB_PID_ATMEL_DFU_ATXMEGA16C4             0x2FD8
#define  USB_PID_ATMEL_DFU_ATXMEGA32C4             0x2FD9
#define  USB_PID_ATMEL_DFU_ATXMEGA256C3            0x2FDA
#define  USB_PID_ATMEL_DFU_ATXMEGA384C3            0x2FDB
#define  USB_PID_ATMEL_DFU_ATUCL3_L4               0x2FDC
#define  USB_PID_ATMEL_DFU_ATXMEGA64A4U            0x2FDD
#define  USB_PID_ATMEL_DFU_ATXMEGA128A4U           0x2FDE

#define  USB_PID_ATMEL_DFU_ATXMEGA64B3             0x2FDF
#define  USB_PID_ATMEL_DFU_ATXMEGA128B3            0x2FE0
#define  USB_PID_ATMEL_DFU_ATXMEGA64B1             0x2FE1
#define  USB_PID_ATMEL_DFU_ATXMEGA256A3BU          0x2FE2
#define  USB_PID_ATMEL_DFU_ATXMEGA16A4U            0x2FE3
#define  USB_PID_ATMEL_DFU_ATXMEGA32A4U            0x2FE4
#define  USB_PID_ATMEL_DFU_ATXMEGA64A3U            0x2FE5
#define  USB_PID_ATMEL_DFU_ATXMEGA128A3U           0x2FE6
#define  USB_PID_ATMEL_DFU_ATXMEGA192A3U           0x2FE7
#define  USB_PID_ATMEL_DFU_ATXMEGA64A1U            0x2FE8
#define  USB_PID_ATMEL_DFU_ATUC3D                  0x2FE9
#define  USB_PID_ATMEL_DFU_ATXMEGA128B1            0x2FEA
#define  USB_PID_ATMEL_DFU_AT32UC3C                0x2FEB
#define  USB_PID_ATMEL_DFU_ATXMEGA256A3U           0x2FEC
#define  USB_PID_ATMEL_DFU_ATXMEGA128A1U           0x2FED
#define  USB_PID_ATMEL_DFU_ATMEGA8U2               0x2FEE
#define  USB_PID_ATMEL_DFU_ATMEGA16U2              0x2FEF
#define  USB_PID_ATMEL_DFU_ATMEGA32U2              0x2FF0
#define  USB_PID_ATMEL_DFU_AT32UC3A3               0x2FF1
#define  USB_PID_ATMEL_DFU_ATMEGA32U6              0x2FF2
#define  USB_PID_ATMEL_DFU_ATMEGA16U4              0x2FF3
#define  USB_PID_ATMEL_DFU_ATMEGA32U4              0x2FF4
#define  USB_PID_ATMEL_DFU_AT32AP7200              0x2FF5
#define  USB_PID_ATMEL_DFU_AT32UC3B                0x2FF6
#define  USB_PID_ATMEL_DFU_AT90USB82               0x2FF7
#define  USB_PID_ATMEL_DFU_AT32UC3A                0x2FF8
#define  USB_PID_ATMEL_DFU_AT90USB64               0x2FF9
#define  USB_PID_ATMEL_DFU_AT90USB162              0x2FFA
#define  USB_PID_ATMEL_DFU_AT90USB128              0x2FFB
// 2FFCh to 2FFFh used by C51 family products


/**
 * USB Device Configuration
 * @{
 */

//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID			USB_VID_ATMEL
#define  USB_DEVICE_PRODUCT_ID			USB_PID_ATMEL_ASF_MSC
#define  USB_DEVICE_MAJOR_VERSION		1
#define  USB_DEVICE_MINOR_VERSION		0
#define  USB_DEVICE_POWER				100 // Consumption on Vbus line (mA)

#define  USB_DEVICE_ATTR                \
	(USB_CONFIG_ATTR_SELF_POWERED)
//	(USB_CONFIG_ATTR_BUS_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
#define  USB_DEVICE_MANUFACTURE_NAME		"ATMEL ASF"
#define  USB_DEVICE_PRODUCT_NAME			"MSC"
#define  USB_DEVICE_SERIAL_NAME			"123123123123"	// Disk SN for MSC

/**
 * Device speeds support
 * Low speed not supported by MSC
 * @{
 */
//! To authorize the High speed
#if (UC3A3||UC3A4)
#  define  USB_DEVICE_HS_SUPPORT
#elif (SAM3XA||SAM3U)
#  define  USB_DEVICE_HS_SUPPORT
#endif
//@}


/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)
#define  UDC_SOF_EVENT()                  main_sof_action()
#define  UDC_SUSPEND_EVENT()              main_suspend_action()
#define  UDC_RESUME_EVENT()               main_resume_action()
//! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
// #define  UDC_REMOTEWAKEUP_ENABLE()        user_callback_remotewakeup_enable()
// extern void user_callback_remotewakeup_enable(void);
// #define  UDC_REMOTEWAKEUP_DISABLE()       user_callback_remotewakeup_disable()
// extern void user_callback_remotewakeup_disable(void);
//! When a extra string descriptor must be supported
//! other than manufacturer, product and serial string
// #define  UDC_GET_EXTRA_STRING()
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */

/**
 * Configuration of MSC interface
 * @{
 */

//! Vendor name and Product version of MSC interface
#define UDI_MSC_GLOBAL_VENDOR_ID            \
   'A', 'T', 'M', 'E', 'L', ' ', ' ', ' '
#define UDI_MSC_GLOBAL_PRODUCT_VERSION            \
   '1', '.', '0', '0'

//! Interface callback definition
#define  UDI_MSC_ENABLE_EXT()          main_msc_enable()
#define  UDI_MSC_DISABLE_EXT()         main_msc_disable()
//@}

//@}


/*
 * USB Device Driver Configuration
 */

//! The includes of classes and other headers must be done at the end of this file to avoid compile error

//! Control endpoint size
#define  USB_DEVICE_EP_CTRL_SIZE       64

//! Endpoint numbers used by MSC interface
#define  UDI_MSC_EP_IN                 (1 | USB_EP_DIR_IN)
#define  UDI_MSC_EP_OUT                (2 | USB_EP_DIR_OUT)

//! Interface number is 0 because it is the unique interface
#define  UDI_MSC_IFACE_NUMBER          0

/**
 * \name UDD Configuration
 */
//@{
//! 2 endpoints used by MSC interface
#undef USB_DEVICE_MAX_EP    // undefine this definition in header file
#define  USB_DEVICE_MAX_EP             2
//@}

#if 0
#include "usb/usbUdiMsc.h"
#endif

/*! \brief Called by MSC interface
 * Callback running when USB Host enable MSC interface
 *
 * \retval true if MSC startup is ok
 */
bool main_msc_enable(void);

/*! \brief Called by MSC interface
 * Callback running when USB Host disable MSC interface
 */
void main_msc_disable(void);

/*! \brief Called when a start of frame is received on USB line
 */
void main_sof_action(void);

/*! \brief Enters the application in low power mode
 * Callback called when USB host sets USB line in suspend state
 */
void main_suspend_action(void);

/*! \brief Called by UDD when the USB line exit of suspend state
 */
void main_resume_action(void);

/*! \brief Initialize the memories used by examples
 */
void memories_initialization(void);




/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)
#define  UDC_SOF_EVENT()                  main_sof_action()
#define  UDC_SUSPEND_EVENT()              main_suspend_action()
#define  UDC_RESUME_EVENT()               main_resume_action()
//! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
// #define  UDC_REMOTEWAKEUP_ENABLE()        user_callback_remotewakeup_enable()
// extern void user_callback_remotewakeup_enable(void);
// #define  UDC_REMOTEWAKEUP_DISABLE()       user_callback_remotewakeup_disable()
// extern void user_callback_remotewakeup_disable(void);
//! When a extra string descriptor must be supported
//! other than manufacturer, product and serial string
// #define  UDC_GET_EXTRA_STRING()
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */

/**
 * Configuration of vendor interface
 * @{
 */
//! Interface callback definition
#define UDI_VENDOR_ENABLE_EXT()           main_vendor_enable()
#define UDI_VENDOR_DISABLE_EXT()          main_vendor_disable()
#define UDI_VENDOR_SETUP_OUT_RECEIVED()   main_setup_out_received()
#define UDI_VENDOR_SETUP_IN_RECEIVED()    main_setup_in_received()

//! endpoints size for full speed
//! Note: Disable the endpoints of a type, if size equal 0
#define UDI_VENDOR_EPS_SIZE_INT_FS    64
#define UDI_VENDOR_EPS_SIZE_BULK_FS   64
#if SAMG55
#define UDI_VENDOR_EPS_SIZE_ISO_FS   0
#else
#define UDI_VENDOR_EPS_SIZE_ISO_FS   256
#endif

//! endpoints size for high speed
#define UDI_VENDOR_EPS_SIZE_INT_HS    64
#define UDI_VENDOR_EPS_SIZE_BULK_HS  512
#define UDI_VENDOR_EPS_SIZE_ISO_HS    64

//@}

//@}


#define  USB_DEVICE_VENDOR_ID             USB_VID_ATMEL
#if BOARD == UC3B_BOARD_CONTROLLER
# define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_UC3_CDC_DEBUG
#else
# define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_ASF_CDC
#endif
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 100 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR                  \
	(USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_BUS_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
//	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
#define  USB_DEVICE_MANUFACTURE_NAME      "ATMEL ASF"
#define  USB_DEVICE_PRODUCT_NAME          "CDC Virtual Com"
// #define  USB_DEVICE_SERIAL_NAME           "12...EF"


/**
 * Device speeds support
 * Low speed not supported by CDC
 * @{
 */
//! To authorize the High speed
#if (UC3A3||UC3A4)
#define  USB_DEVICE_HS_SUPPORT
#elif (SAM3XA||SAM3U)
#define  USB_DEVICE_HS_SUPPORT
#endif
//@}


/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)
#define  UDC_SOF_EVENT()                  main_sof_action()
#define  UDC_SUSPEND_EVENT()              main_suspend_action()
#define  UDC_RESUME_EVENT()               main_resume_action()
//! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
// #define  UDC_REMOTEWAKEUP_ENABLE()        user_callback_remotewakeup_enable()
// extern void user_callback_remotewakeup_enable(void);
// #define  UDC_REMOTEWAKEUP_DISABLE()       user_callback_remotewakeup_disable()
// extern void user_callback_remotewakeup_disable(void);
#ifdef USB_DEVICE_LPM_SUPPORT
#define  UDC_SUSPEND_LPM_EVENT()          main_suspend_lpm_action()
#define  UDC_REMOTEWAKEUP_LPM_ENABLE()    main_remotewakeup_lpm_enable()
#define  UDC_REMOTEWAKEUP_LPM_DISABLE()   main_remotewakeup_lpm_disable()
#endif
//! When a extra string descriptor must be supported
//! other than manufacturer, product and serial string
// #define  UDC_GET_EXTRA_STRING()
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */
/**
 * Configuration of CDC interface
 * @{
 */

//! Define two USB communication ports
#define  UDI_CDC_PORT_NB 1

//! Interface callback definition
#define  UDI_CDC_ENABLE_EXT(port)         main_cdc_enable(port)
#define  UDI_CDC_DISABLE_EXT(port)        main_cdc_disable(port)
#define  UDI_CDC_RX_NOTIFY(port)          uart_rx_notify(port)
#define  UDI_CDC_TX_EMPTY_NOTIFY(port)
#define  UDI_CDC_SET_CODING_EXT(port,cfg) uart_config(port,cfg)
#define  UDI_CDC_SET_DTR_EXT(port,set)    main_cdc_set_dtr(port,set)
#define  UDI_CDC_SET_RTS_EXT(port,set)

//! Define it when the transfer CDC Device to Host is a low rate (<512000 bauds)
//! to reduce CDC buffers size
#define  UDI_CDC_LOW_RATE

//! Default configuration of communication port
#define  UDI_CDC_DEFAULT_RATE             115200
#define  UDI_CDC_DEFAULT_STOPBITS         CDC_STOP_BITS_1
#define  UDI_CDC_DEFAULT_PARITY           CDC_PAR_NONE
#define  UDI_CDC_DEFAULT_DATABITS         8


#define USART_ENABLE()
#define USART_DISABLE()
#define USART_BASE       ((Usart*)USART1)
#define USART_ID         ID_USART1

#define USART_HANDLER    USART1_Handler
#define USART_INT_IRQn   USART1_IRQn
#define USART_INT_LEVEL  3

/**
 * USB Device Driver Configuration
 * @{
 */
//! Limit the isochronous endpoint in singe bank mode for USBB driver
//! to avoid exceeding USB DPRAM.
#define UDD_ISOCHRONOUS_NB_BANK(ep) 1
//@}

//! The includes of classes and other headers must be done
//! at the end of this file to avoid compile error

/*! \brief Called by CDC interface
 * Callback running when CDC device have received data
 */
void uart_rx_notify(uint8_t port);

/*! \brief Configures communication line
 *
 * \param cfg      line configuration
 */
//void uart_config(uint8_t port, usb_cdc_line_coding_t * cfg);

/*! \brief Opens communication line
 */
void uart_open(uint8_t port);

/*! \brief Closes communication line
 */
void uart_close(uint8_t port);

#endif

