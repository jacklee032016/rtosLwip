
/* come from toolchain's C lib */
#include <string.h>

#include "muxOs.h"

#if MUXLAB_BOARD
#include <rtk_types.h>
#include <rtk_error.h>

#include <rtk_i2c.h>
#include <rtk_api.h>
#include <rtk_api_ext.h>
#include <rtk_error.h>
#include <rtl8307h_reg_struct.h>
#include <rtl8307h_table_struct.h>
#include <rtl8307h_types.h>

#include <rtl8307h_debug.h>

#define	RTK_DEBUG_IF			0


void RTL8307H_I2C_init(void)
{ 
	//i2cbitbang_init();
}
 

int32 RTL8307H_I2C_READ(uint32 switch_addr, uint32 *reg_val)
{    
	unsigned char data[4];
	int i;
	memset(data, 0, sizeof(data));
	if(muxI2CRead(MUX_I2C_PCA9554_CS_BOOTROM, MUX_I2C_ADDRESS_RTL8035, switch_addr, 3, data, 4) == EXIT_FAILURE)
	{
		return RT_ERR_NO_ACK;
	}

	*reg_val = 0;
	for (i = 0; i < 4; i++)
	{
		*reg_val = (data[i] << (i*8) ) | *reg_val ; 
	}

	return RT_ERR_OK;
}


int32 RTL8307H_I2C_WRITE(uint32 switch_addr, uint32 reg_val)
{
	unsigned char data[4];
	data[0] = reg_val & 0xFF;
	data[1] = (reg_val >> 8) & 0xFF;
	data[2] = (reg_val >> 16) & 0xFF;
	data[3] = (reg_val >> 24) & 0xFF;

	if(muxI2CWrite(MUX_I2C_PCA9554_CS_BOOTROM, MUX_I2C_ADDRESS_RTL8035, switch_addr, 3, data, 4) == EXIT_FAILURE)
	{
		return RT_ERR_NO_ACK;
	}
	return RT_ERR_OK;
}

#if RTK_DEBUG_IF
static void _printPortAbility(rtk_port_link_ability_t *ability)
{
	switch(ability->speed)
	{
		case PORT_SPEED_10M:
			printf("10M\t");
			break;
		case PORT_SPEED_100M:
			printf("100M\t");
			break;
		case PORT_SPEED_1000M:
			printf("1000M\t");
			break;
		default:
			printf("Invalidate Speed\t");
			break;
	}
	
	switch(ability->duplex)
	{
		case PORT_HALF_DUPLEX:
			printf("HALF_DUPLEX\t");
			break;
		case PORT_FULL_DUPLEX:
			printf("FULL_DUPLEX\t");
			break;
		default:
			printf("Invalidate Duplex\t");
			break;
	}
	
	switch(ability->link)
	{
		case PORT_LINKDOWN:
			printf("LINK_DOWN\t");
			break;
		case PORT_LINKUP:
			printf("LINK_UP\t");
			break;
		default:
			printf("Invalidate Link Status\t");
			break;
	}  

	printf("NWAY=%i\r\n", ability->nway);
}
#endif

static char _muxConfigRtkOnePort(uint8_t port)
{
	rtk_port_link_ability_t	linkAbility={0};
	int ret;

//	MUX_DEBUGF("Configure port : %d, read status:\t", port);
	ret = rtk_port_linkAbility_get(port, &linkAbility ); 
	if(ret != RT_ERR_OK)
	{
		MUX_ERRORF(("port %d ERROR, return : %s, \t", port, rtk_errMsg_get(ret)));
#if RTK_DEBUG_IF
		MUX_ABORT("Get port status");
#else
		return EXIT_FAILURE;
#endif
	}

//	MUX_DEBUGF("\tset status");
	linkAbility.link   = PORT_LINKUP;
	ret = rtk_port_linkAbility_set(port, &linkAbility);
	if(ret != RT_ERR_OK)
	{
		MUX_ERRORF(("port %d ERROR, return : %s, \t", port, rtk_errMsg_get(ret)));
#if RTK_DEBUG_IF
		MUX_ABORT("Set port status");
#else
		return EXIT_FAILURE;
#endif
	}
	
//	MUX_DEBUGF("\tread status again");
	ret = rtk_port_linkAbility_get(port, &linkAbility );
	if(ret != RT_ERR_OK)
	{
		MUX_ERRORF(("port %d ERROR, return : %s, \t", port, rtk_errMsg_get(ret)));
#if RTK_DEBUG_IF
		MUX_ABORT("Get port status after set");
#else
		return EXIT_FAILURE;
#endif
	}

#if RTK_DEBUG_IF
   	_printPortAbility(&linkAbility);
#endif
	return EXIT_SUCCESS;
}


static void _muxRtl8305DebugOneport(rtk_port_t port)
{
#if MUXLAB_BOARD
	rtk_stat_counter_t cnt;
	int count;
	rtk_stat_port_get(port, STAT_IfInOctets, &cnt);
	count = (int)cnt;
	printf("Port:%d: InfCount:%d ", port, count);

	rtk_stat_port_get(port, STAT_IfOutOctets, &cnt);
	count = (int)cnt;
	printf("OutfCount:%d "MUX_NEW_LINE, count);
#endif
}

void muxEtherDebug(void)
{
	_muxRtl8305DebugOneport(PN_PORT4);
	_muxRtl8305DebugOneport(PN_PORT5);
	_muxRtl8305DebugOneport(PN_PORT6);
}

#define	__RTK_CHECK_PORT_START(port, retVal, action)		\
		{if( (retVal) != RT_ERR_OK ){ MUX_ERRORF(("Port %d %s failed: %d", (port),(action), (retVal))); \
			return EXIT_FAILURE;	}}


char muxBspRtl8305Config(void)
{
	rtk_api_ret_t retVal;
#if 0
	rtk_wol_exit_sleep(PN_PORT0);
	rtk_wol_exit_sleep(PN_PORT1);
//	rtk_wol_exit_sleep(PN_PORT2);
//	rtk_wol_exit_sleep(PN_PORT3);
	rtk_wol_exit_sleep(PN_PORT4);
	rtk_wol_exit_sleep(PN_PORT5);
	rtk_wol_exit_sleep(PN_PORT6);
#endif

#if 1
	if(_muxConfigRtkOnePort(PN_PORT0) != EXIT_SUCCESS)
		return EXIT_FAILURE;/* timeout */
	if(_muxConfigRtkOnePort(PN_PORT1) != EXIT_SUCCESS)
		return EXIT_FAILURE;
	if(_muxConfigRtkOnePort(PN_PORT2) != EXIT_SUCCESS)
		return EXIT_FAILURE;
	if(_muxConfigRtkOnePort(PN_PORT3) != EXIT_SUCCESS)
		return EXIT_FAILURE;
#endif

	if(_muxConfigRtkOnePort(PN_PORT4) != EXIT_SUCCESS)/* port to RJ45. eg. GBE PHY port */
		return EXIT_FAILURE;

	if(_muxConfigRtkOnePort(PN_PORT5) != EXIT_SUCCESS) /*port to FPGA, eg. RGMII  port */
		return EXIT_FAILURE;
	
	if(_muxConfigRtkOnePort(PN_PORT6) != EXIT_SUCCESS) /* port to E70, eg. FE/RMII port */
		return EXIT_FAILURE;
//	_muxConfigRtkOnePort(PN_PORT7); /* RT_ERR_PORT_ID invalid port id, this port to 8051 MCU */


//	rtk_hec_mode_set(PN_PORT0, HEC_MODE_ENABLE);
//	rtk_hec_mode_set(PN_PORT1, HEC_MODE_ENABLE);

	retVal = rtk_stat_port_reset(PN_PORT4);
	__RTK_CHECK_PORT_START(PN_PORT4, retVal, "reset");

	retVal = rtk_stat_port_reset(PN_PORT5);
	__RTK_CHECK_PORT_START(PN_PORT5, retVal, "reset");
	retVal = rtk_stat_port_reset(PN_PORT6);
	__RTK_CHECK_PORT_START(PN_PORT6, retVal, "reset");

	retVal = rtk_stat_port_start(PN_PORT4);
	__RTK_CHECK_PORT_START(PN_PORT4, retVal, "start");
	retVal = rtk_stat_port_start(PN_PORT5);
	__RTK_CHECK_PORT_START(PN_PORT5, retVal, "start");
	retVal = rtk_stat_port_start(PN_PORT6);
	__RTK_CHECK_PORT_START(PN_PORT6, retVal, "start");

	return EXIT_SUCCESS;
}

#endif

