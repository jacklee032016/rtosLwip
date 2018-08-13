
/*
* header for FPGA firmware
* J.L. 02.08.2018
*/

#ifndef __EXT_FPGA_H__
#define __EXT_FPGA_H__


#define I2C_ADDR_FPGA_A	(0x60 >> 1)		// 8 bits addr format

#define I2C_ADDR_FPGA_B	(0x62 >> 1)		// 8 bits addr format
#define I2C_FPGA_MUX		0


#define FPGA_CTRL0			0x00
#define FPGA_CTRL1			0x01
#define FPGA_CTRL_RST		0x02


//#define FPGA_STATUS3	0x03
#define FPGA_HDMI_STAT	0x04
#define FPGA_STATUS5	0x05
#define FPGA_STATUS6	0x06
#define FPGA_STATUS7	0x07

#define FPGA_DETECT_A	0x08
#define FPGA_DETECT_B	0x09
#define FPGA_DETECT_C	0x0A
#define FPGA_DETECT_D	0x0B

#define FPGA_LED_STATUS	0x20

#define FPGA_VERSION	0x1D
#define FPGA_REVISIONN	0x1E
#define FPGA_DEVICE		0x1F

#define FPGA_LED_B_STAT	0x20
#define FPGA_LED_R_STAT	0x21

#define LED_XOR			0x22
#define FPGA_RST_CH		0x23

#define FPGA_VCXO_A		0x24
#define FPGA_VCXO_B		0x26
#define FPGA_VCXO_C		0x28
#define FPGA_VCXO_D		0x2A


// 8 bit register for fifio level
// range from empty: 0  to full : 255
//
#define FPGA_FIFO_LEVEL_RX_A	12
#define FPGA_FIFO_LEVEL_RX_B	14
#define FPGA_FIFO_LEVEL_RX_C	16
#define FPGA_FIFO_LEVEL_RX_D	18

// ####################################################
#define FPGA_MEASURE_FREQ	0x16		// page 8, 9 ...

#define SDI_IN_FREQ_INFO_A	8
#define SDI_IN_FREQ_INFO_B	9
#define SDI_IN_FREQ_INFO_C	10
#define SDI_IN_FREQ_INFO_D  11

#define SDI_OUT_FREQ_INFO_A	12
#define SDI_OUT_FREQ_INFO_B	13
#define SDI_OUT_FREQ_INFO_C	14
#define SDI_OUT_FREQ_INFO_D	15


#define FPGA_MEASURE_TIMING	0x16		// page 0, 1 ...

#define TIMING_H_LEN		0
#define TIMING_H_BLANK		1
#define TIMING_V_LEN		2
#define TIMING_V_SYNC		3



#define RESET_PCS_TX		BIT_7
#define RESET_PCS_RX		BIT_6
#define ENA_TEST_PATTERN	BIT_5 // 0x20
#define RESET_ETH_PHY		BIT_4


//#define VALENS_RESET		0x80
//#define VALENS_HPD_OUT	0x40
//#define HDMI_RESET	0x40

#define RESET_ALL	BIT_7 //0x80
#define RESET_TX	BIT_6 //0x40
#define RESET_RX	BIT_5 //0x20
#define RESET_XAUI_CORE		BIT_4 //0x10


// channel detection registers

#define SDI_RATE_MASK	0x03
#define SDI_MODE_MASK	0x04		//  rx_m or tx_m
#define SDI_LOCK_MASK	0x08





//#define AUTORATE		0
//#define RATE6G		1
//#define RATE3G		2
//#define RATEHD		3
//#define RATESD		4

#define RX_MODE_6G		3
#define RX_MODE_3G		2
#define RX_MODE_HD		0
#define RX_MODE_NONE	1

#define TX_MODE_6G		3
#define TX_MODE_3G		2
#define TX_MODE_HD		0
#define TX_MODE_NONE	1



#define	FPGA_LED_ENABLE	0x80

#define	FPGA_LED_SDI_A	0x01
#define	FPGA_LED_SDI_B	0x02
#define	FPGA_LED_SDI_C	0x04
#define	FPGA_LED_SDI_D	0x08

//#define	FPGA_LED_SDI_6G		0x10
#define	FPGA_LED_SDI_ACT	BIT_4 // 0x10
#define	FPGA_LED_POWER		BIT_6 // 0x40


#define	FPGA_SDI_ENA_A	0x01
#define	FPGA_SDI_ENA_B	0x02
#define	FPGA_SDI_ENA_C	0x04
#define	FPGA_SDI_ENA_D	0x08

#define	FPGA_SDI_ENA_ALL	(FPGA_SDI_ENA_A | FPGA_SDI_ENA_B | FPGA_SDI_ENA_C | FPGA_SDI_ENA_D)
#define	FPGA_SDI_ENA_A_C	(FPGA_SDI_ENA_A | FPGA_SDI_ENA_C)


#define	FPGA_SYNC_LOCK_A	0x10
#define	FPGA_SYNC_LOCK_B	0x20
#define	FPGA_SYNC_LOCK_C	0x40
#define	FPGA_SYNC_LOCK_D	0x80



// ###################################################
// FPGA register section B

#define	FPGA_B_SDI_PORT_A	0x00
#define	FPGA_B_SDI_PORT_B	0x01
#define	FPGA_B_SDI_PORT_C	0x02
#define	FPGA_B_SDI_PORT_D	0x03

#define	SDI_PORT0_SD_HD_N		BIT_0 // 0x01
#define	SDI_PORT0_GAIN_SEL		BIT_1
#define	SDI_PORT0_EQ_EN			BIT_2
#define	SDI_PORT0_DIS_N			BIT_3

// SDI port bits, (B,C,D)
#define	SDI_PORT_SD_EN		BIT_0 // 0x01
#define	SDI_PORT_GAIN_SEL	BIT_1 // 0x02
#define	SDI_PORT_SQ_ADJ		BIT_2 // 0x04
#define	SDI_PORT_OP_CTL		BIT_3 // 0x08

#define	SDI_PORT_EQ_BYPASS	BIT_4 // 0x10
#define	SDI_PORT_EQ_DIS		BIT_5 // 0x20
#define	SDI_PORT_CD_ENA		BIT_6 // 0x40
#define	SDI_PORT_LED_DIR	BIT_7 // 0x40


#define	FPGA_B_XAUI_PORT	0x04	// control bits to XAUI port

#define	XAUI_PORT_PDTRXA	BIT_0 // 0x01
#define	XAUI_PORT_LS_OK		BIT_1 // 0x02
#define	XAUI_PORT_PRBSEN	BIT_2 // 0x04
#define	XAUI_PORT_ST		BIT_3 // 0x08
#define	XAUI_PORT_MODE_SEL	BIT_4 // 0x10

#define	FPGA_B_XAUI_CONFIG	0x05	// control bits to XAUI port

#define	XAUI_CONFIG_LOOPBACK	0x01
#define	XAUI_CONFIG_PDOWN		0x02
#define	XAUI_CONFIG_RESET_FAULT	0x04
#define	XAUI_CONFIG_RESET_RX_STAT	0x08
#define	XAUI_CONFIG_TEST_ENA	0x10
#define	XAUI_CONFIG_TEST_SEL0	0x20
#define	XAUI_CONFIG_TEST_SEL1	0x40

#define	FPGA_B_SDI_RATES	6
#define	FPGA_B_SDI_MODE		7

#define FPGA_FIFO_LEVEL_TX_A	12
#define FPGA_FIFO_LEVEL_TX_B	14
#define FPGA_FIFO_LEVEL_TX_C	16
#define FPGA_FIFO_LEVEL_TX_D	18

#define FPGA_DETECT_TX			20

#define SDI_MODE_MSK		0x03




#define	FPGA_B_STAT_SERDES		30	// control bits to XAUI port

#define	STAT_SERDES_LOSA		0x01
#define	STAT_SERDES_LS_OK		0x02
#define	STAT_SERDES_PRBS_OK		0x04


#define	FPGA_B_STAT_XAUI		31	// control bits to XAUI port

#define	STAT_XAUI_TX_FAULT		0x01
#define	STAT_XAUI_RX_FAULT		0x02
#define	STAT_XAUI_SYNC_LINK1	0x04
#define	STAT_XAUI_SYNC_LINK2	0x08
#define	STAT_XAUI_SYNC_LINK3	0x10
#define	STAT_XAUI_SYNC_LINK4	0x20
#define	STAT_XAUI_SYNC_LINK_ALL		(STAT_XAUI_SYNC_LINK1 | STAT_XAUI_SYNC_LINK2 | STAT_XAUI_SYNC_LINK3 | STAT_XAUI_SYNC_LINK4)
#define	STAT_XAUI_ALIGN			0x40
#define	STAT_XAUI_RX_LINK_OK	0x80


// ---------------------------------------------
//
#define MODE_6G_1CH		1
#define MODE_6G_2CH		2
#define MODE_6G_4CH		3

#define MODE_3G_1CH		4
#define MODE_3G_2CH		5

#define MODE_HD_1CH		7

#define COMPRESS_MODE_1CH		0
#define COMPRESS_MODE_2CH		1
#define COMPRESS_MODE_3CH		2
#define COMPRESS_MODE_4CH		3


#define FIFO_HALF_LEVEL_RX			700 // 122		// Fifo range is 0 - 255 ( 8 bit count )

#define FIFO_THRESHOLD_RX			25 //16
#define FIFO_THRESHOLD2_RX			50 //40
#define FIFO_THRESHOLD_MAX_RX		100
#define FIFO_HALF_LEVEL_TX		(2047)	// with 4K fifos

#define FIFO_THRESHOLD_MAX_TX		1900



#define  GET_JUMPER_SELECT_SIG()	((gpio_get_pin_value(PIN_JUMPER_SELECT) == 0) ? TRUE : FALSE)	// return tru when jumper in place
#define  GET_POWER_1V_OK_SIG()		((gpio_get_pin_value(PIN_POWER_1V_OK) != 0) ? TRUE : FALSE)


#define SCORE_6G	5940
#define SCORE_3G	2970
#define SCORE_HD	1485

#define SCORE_MAX	(SCORE_6G + SCORE_3G + 10)


COMPILER_PACK_SET(1);

struct _EXT_FPGA_VER
{
	char		rx_tx : 1;
	char		version : 7;
	char		revision;
	char		fpgaDevice;
}__attribute__((packed));

typedef	struct _EXT_FPGA_VER		EXT_FPGA_VER;

typedef	struct {
	uint8_t		Ctrl0;
	uint8_t		Ctrl1;
	uint8_t		Ctrl_Rst;
/*
	U8		Status3_obsolete;
	U8		HDMI_Stat;
	U8		Status5;
	U8		Status6;
	U8		Status7;

	U8		Detect_A;	// r8
	U8		Detect_B;
	U8		Detect_C;
	U8		Detect_D;

	U8		nu_0[4];
	U8		nu_1[13];	// 16 - 28

	U8		Version;	// 29
	U8		Revision;	// 30
	U8		HW_device;	// 31
	
	U8		Led_status1;
	U8		Led_status2;
	U8		nu3;
	U8		nu4;
*/
} FPGA_A_regs, *pFPGA_A_reg;


typedef	struct {
	volatile uint8_t		SDI_Ctrl_A;
	volatile uint8_t		SDI_Ctrl_B;
	volatile uint8_t		SDI_Ctrl_C;
	volatile uint8_t		SDI_Ctrl_D;

	volatile uint8_t		XAUI_Ctrl;
	volatile uint8_t		XAUI_config;
	
	volatile uint8_t		SDI_Rates;	// keep next to SDI_modes
	volatile uint8_t		SDI_modes;	// **********************

} FPGA_B_regs, *pFPGA_B_reg;


#endif

