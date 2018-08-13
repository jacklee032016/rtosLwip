/*
 * Copyright (C) 2009 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 10275 $
 * $Date: 2010-06-15 15:37:41 +0800 (Tue, 15 Jun 2010) $
 *
 * Purpose : RTK switch high-level API for RTL8307H/RTL8367
 * Feature : Here is a list of all functions and variables in this module.
 *
 */
//frank stdlib string (memset)
#include <stdlib.h>
#include <string.h>

#include "rtk_i2c.h"

#include <rtk_types.h>
#include <rtk_api.h>
#include <rtk_api_ext.h>
#include <rtk_error.h>

#include <rtl8307h_reg_struct.h>
#include <rtl8307h_table_struct.h>
#include <rtl8307h_types.h>

#include <rtl8307h_debug.h>

#include <rtl8307h_asicdrv_vlan.h>
#include <rtl8307h_asicdrv_svlan.h>
#include <rtl8307h_asicdrv_acl.h>
#include <rtl8307h_asicdrv_rma.h>
#include <rtl8307h_asicdrv_port.h>
#include <rtl8307h_asicdrv_dot1x.h>
#include <rtl8307h_asicdrv_lut.h>

#include <rtl8307h_asicdrv_lock.h>

#include <rtl8307h_asicdrv_mib.h>
#include <rtl8307h_asicdrv_wol.h>
#include <rtl8307h_asicdrv_igmp.h>
#include <rtl8307h_asicdrv_misc.h>
#include <rtl8307h_asicdrv_phy.h>



/* Function Name:
 *      rtk_port_mode_get
 * Description:
 *      Get port type. 
 * Input:
 *      port                    -  port id (0~6)
 * Output:     
 *      mode                    -  the pointer of port mode
 * Return: 
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_PORT_ID
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_FAILED  
 *     
 * Note:
 *      Port 0~3 are HEAC ports so the mode is MODE_EXT_HEAC
 *      Port 4 is giga port, so the mode is MODE_EXT_GIGA 
 *      Port 5 supports following modes:
 *           MODE_EXT_RGMII_NODELAY
 *           MODE_EXT_RGMII_RXDELAY_ONLY
 *           MODE_EXT_RGMII_TXDELAY_ONLY
 *           MODE_EXT_RGMII_RXTXDELAY
 *           MODE_EXT_TMII_MII_MAC
 *           MODE_EXT_MII_PHY
 *           MODE_EXT_TMII_PHY
 *           MODE_EXT_RMII_INPUT
 *           MODE_EXT_RMII_OUTPUT
 *      Port 6 supports following modes:
 *           MODE_EXT_TMII_MII_MAC
 *           MODE_EXT_MII_PHY
 *           MODE_EXT_TMII_PHY
 *           MODE_EXT_RMII_INPUT
 *           MODE_EXT_RMII_OUTPUT 
 *           MODE_EXT_HEAC,
 *           MODE_EXT_FAST_ETHERNET,
 */
rtk_api_ret_t rtk_port_mode_get(rtk_port_t port, rtk_mode_ext_t* mode)
{            
    if (port < PN_PORT4)
    {
        *mode = MODE_EXT_HEAC;
    }
    else if (port == 4)
    {
        *mode = MODE_EXT_GIGA;     
    }
    else if (port == 5)
        return rtl8307h_mac5_miiMode_get(mode);  
    else if (port == PN_PORT6)
        return rtl8307h_mac6_miiMode_get(mode);        
    else
        return RT_ERR_PORT_ID;
#ifdef	EXT_LAB
	return RT_ERR_OK;
#endif
}

/* Function Name:
 *      rtk_port_mii_clk_set
 * Description:
 *      The pins of (R)MII/TMII/RGMII interface can be 
 *      put to high-impendence state when port is link down. 
 *      This function is used to set the configuration.
 * Input:
 *      port              -  Port id (PN_PORT5,PN_PORT6)
 *      rtk_enable_t      -  DISABLED/ENABLED
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 *     
 * Note:
 *      
 */
rtk_api_ret_t rtk_port_mii_clk_set(rtk_port_t port, rtk_enable_t enable)
{
    if ((port != PN_PORT5) && (port != PN_PORT6))
        return RT_ERR_PORT_ID;

    if (port == PN_PORT5)
        reg_field_write(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, ENMAC5MIISTOP, enable);
    else
        reg_field_write(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, ENMAC6MIISTOP, enable);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_mii_clk_get
 * Description:
 *      The pins of (R)MII/TMII/RGMII interface can be 
 *      put to high-impendence state when port is link down. 
 *      This function is used to get the configuration.
 * Input:
 *      port              -  Port id (PN_PORT5,PN_PORT6)
 *      pEnable           -  pointer of the ability: DISABLED/ENABLED
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 *     
 * Note:
 *      
 */
rtk_api_ret_t rtk_port_mii_clk_get(rtk_port_t port, rtk_enable_t* pEnable)
{
    uint32 regVal;
    
    if ((port != PN_PORT5) && (port != PN_PORT6))
        return RT_ERR_PORT_ID;

    if (port == PN_PORT5)
        reg_field_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, ENMAC5MIISTOP, &regVal);
    else
        reg_field_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, ENMAC6MIISTOP, &regVal);

    *pEnable = regVal;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      _port_macForceLink_set
 * Description:
 *       Set port force linking configuration
 * Input:
 *      port               -  Port id
 *      pPortability      -  the pointer of port ability configuration 
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_FAILED  
*       RT_ERR_PORT_ID
 *      RT_ERR_SMI       
 *      RT_ERR_NULL_POINTER
 *     
 * Note:
 *      This API can set Port MAC force mode properties. 
 */
static rtk_api_ret_t _port_macForceLink_set(rtk_port_t port, rtk_port_mac_ability_t *pPortability)
{
    uint32 regval;
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID; 
    
    if(NULL == pPortability)
        return RT_ERR_NULL_POINTER;

    if ((5 == port) && (0 == pPortability->forcemode))
        return RT_ERR_INPUT;  
    
    /*set port in mac force mode*/
    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_PROPERTY_CONFIGURE + port, &regval));
    regval &= (~0x3FFUL);
    regval |= (pPortability->forcemode) ? (0x1 << 0) : 0;    

    if((PORT_SPEED_1000M == pPortability->speed) && (PN_PORT4 != port) && (PN_PORT5 != port))
    {
        return RT_ERR_INPUT;  
    }
    else if(PORT_SPEED_1000M == pPortability->speed)
    {
        regval |= (0x1 << 7);
        pPortability->duplex = PORT_FULL_DUPLEX;      
    }
    else if ((PORT_SPEED_100M == pPortability->speed) && ( PORT_FULL_DUPLEX == pPortability->duplex))
    {
        regval |= (0x1 << 6);
    }
    else if ((PORT_SPEED_100M == pPortability->speed) && ( PORT_HALF_DUPLEX == pPortability->duplex))
    {
        regval |= (0x1 << 5);
    }
    else if ((PORT_SPEED_10M == pPortability->speed) && ( PORT_FULL_DUPLEX == pPortability->duplex))
    {
        regval |= (0x1 << 4);
    }
    else if ((PORT_SPEED_10M == pPortability->speed) && ( PORT_HALF_DUPLEX == pPortability->duplex))
    {
        regval |= (0x1 << 3);
    }
       
    regval |= pPortability->link ? (0x1 << 1) : 0;
    regval |= pPortability->nway ? (0x1 << 2) : 0; 
    regval |= pPortability->txpause ? (0x1 << 8) : 0;
    regval |= pPortability->rxpause ? (0x1 << 9) : 0;
    CHK_FUN_RETVAL(reg_write(RTL8307H_UNIT, PORT0_PROPERTY_CONFIGURE + port, regval));    

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_linkAbility_set
 * Description:
 *      Set port link ability
 * Input:
 *      port              -  Port id
 *      pPortability      -  port link ability
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_FAILED  
 *      RT_ERR_PORT_ID
 *      RT_ERR_SMI       
 *      RT_ERR_NULL_POINTER
 *     
 * Note:
 *      When port 5 is bonding as (R)MII/TMII port, 10M/100Mbps speed configuration
 *      is supported but 1000Mbps is not supported. 1000Mbps is supported when port 5 is bonding as RGMII port  
 *      with mode MODE_EXT_RGMII_NODELAY or MODE_EXT_RGMII_RXDELAY_ONLY or MODE_EXT_RGMII_TXDELAY_ONLY
 *          or MODE_EXT_RGMII_RXTXDELAY. *
 *      Error RT_ERR_MAC_FORCE_1000 is returned when configure 1000Mbps to (R)MII/TMII port.
 *
 *      When port 6 is configurated as (R)MII/TMII port, 10M/100Mbps speed configuration
 *      is supported but 1000Mbps is not supported.
 */
rtk_api_ret_t rtk_port_linkAbility_set(rtk_port_t port, rtk_port_link_ability_t *pPortability)
{
    rtk_api_ret_t ret;
    uint32 regval;
    rtk_port_mac_ability_t ability;
    rtk_mode_ext_t mode;

    uint32 chipType;
    uint32 ifType;
 
    uint32 pollcnt, finished; 
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID; 
    
    if(NULL == pPortability)
        return RT_ERR_NULL_POINTER;

    if (pPortability->nway == 0)
    {
        if (pPortability->speed != PORT_SPEED_1000M &&  \
            pPortability->speed != PORT_SPEED_100M  &&  \
            pPortability->speed != PORT_SPEED_10M
            )    
        return RT_ERR_PHY_SPEED;
    }

    reg_read(RTL8307H_UNIT, PORT0_PROPERTY_CONFIGURE + port, &regval);
    
    if (port < PN_PORT4)
    {
        ability.forcemode = 0;          /* 0 : polling phy register; 1 : use mac configuration register */
        ability.nway      = 0;          /* Disable NWAY */ 
        ability.speed     = PORT_SPEED_100M;
        ability.duplex    = PORT_FULL_DUPLEX;
        ability.rxpause   = pPortability->rxpause;
        ability.txpause   = pPortability->txpause; 
        ability.link      = regval & 0x1;
    }
    else if (port == 4)
    {
        /* Gigibit              */
        ability.forcemode = 0;          /* 0 : polling phy register; 1 : use mac configuration register */
        ability.nway      = pPortability->nway;         
        ability.speed     = pPortability->speed;
        ability.duplex    = pPortability->duplex;
        ability.rxpause   = pPortability->rxpause;
        ability.txpause   = pPortability->txpause; 
        ability.link      = regval & 0x1;        
    }
    else if (port == 5)
    {   
        /* judge whether the chip type is RTL8307H or RTL8305H */    
        CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, CHIP_TYPE_2_0, &chipType));

        if (0x7 != chipType)
            return RT_ERR_CHIP_NOT_SUPPORTED;  
            
        /* Verify Speed */
        if ((ret = rtl8307h_mac5_miiMode_get(&mode)) != RT_ERR_OK)
            return ret;   

        if (PORT_SPEED_1000M == pPortability->speed)
        {
            if (mode != MODE_EXT_RGMII_NODELAY && mode != MODE_EXT_RGMII_RXDELAY_ONLY \
                && mode != MODE_EXT_RGMII_TXDELAY_ONLY && mode != MODE_EXT_RGMII_RXTXDELAY)
                return RT_ERR_MAC_FORCE_1000; /* MII not support 1000M */ 
        }      
        
        /* (R)MII/TMII/RGMII    */
        ability.forcemode = 1;        /* link has meaning only when forcemode equals 1 */
        ability.nway      = 0;
        ability.speed     = pPortability->speed;
        ability.duplex    = pPortability->duplex;
        ability.rxpause   = pPortability->rxpause;
        ability.txpause   = pPortability->txpause; 
        ability.link      = pPortability->link;     /* force link up or link down */   

        return _port_macForceLink_set(port, &ability); 
    }
    else if (port == 6)
    {            
        /* judge whether the chip type is RTL8307H or RTL8305H */
        CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, MAC6_MII_2_0, &ifType));

        switch (ifType)
        {
            case 7: /* Fast Ethernet port */
                if (PORT_SPEED_1000M == pPortability->speed)
                    return RT_ERR_PHY_SPEED;     
                ability.forcemode = 0;          /* 0 : polling phy register; 1 : use mac configuration register */
                ability.nway      = pPortability->nway;         
                ability.speed     = pPortability->speed;
                ability.duplex    = pPortability->duplex;
                ability.rxpause   = pPortability->rxpause;
                ability.txpause   = pPortability->txpause; 
                ability.link      = regval & 0x1;  
                break;
            case 6: /* HEAC port          */
                ability.forcemode = 0;          /* 0 : polling phy register; 1 : use mac configuration register */
                ability.nway      = 0;          /* Disable NWAY */ 
                ability.speed     = PORT_SPEED_100M;
                ability.duplex    = PORT_FULL_DUPLEX;
                ability.rxpause   = pPortability->rxpause;
                ability.txpause   = pPortability->txpause; 
                ability.link      = regval & 0x1;
                break;
            case 5: /* (R)MII/TMII       */
            case 4:
            case 3:
            case 2:
            case 1:
                if (PORT_SPEED_1000M == pPortability->speed)
                    return RT_ERR_MAC_FORCE_1000; /* MII not support 1000M */

                ability.forcemode = 1;        /* link has meaning only when forcemode equals 1 */
                ability.nway      = 0;
                ability.speed     = pPortability->speed;
                ability.duplex    = pPortability->duplex;
                ability.rxpause   = pPortability->rxpause;
                ability.txpause   = pPortability->txpause; 
                ability.link      = pPortability->link;     /* force link up or link down */        

                return _port_macForceLink_set(port, &ability); 
                break;
            default:
                return RT_ERR_INPUT;    
        }
    }
    else 
        return RT_ERR_PORT_ID;

    /*Write into the mac configuration register and trigger phy access */  
   
    /*configure port property*/
    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_PROPERTY_CONFIGURE + port, &regval));                       
    regval &= (~0x3FFUL);
    regval |= ability.forcemode;
    regval |= ability.link << 1;
    regval |= ability.nway << 2;
    if ((PORT_SPEED_10M & ability.speed) && ( PORT_HALF_DUPLEX & ability.duplex))
        regval |= (0x1 << 3);
   
    if ((PORT_SPEED_10M & ability.speed) && ( PORT_FULL_DUPLEX & ability.duplex))
        regval |= (0x1 << 4);
   
    if ((PORT_SPEED_100M & ability.speed) && ( PORT_HALF_DUPLEX & ability.duplex))
        regval |= (0x1 << 5);

    if ((PORT_SPEED_100M & ability.speed) && ( PORT_FULL_DUPLEX & ability.duplex))   
        regval |= (0x1 << 6);
        
    if (PORT_SPEED_1000M & ability.speed)   
        regval |= (0x1 << 7);

    regval |= ability.txpause ? (0x1 << 8) : 0;
    regval |= ability.rxpause ? (0x1 << 9) : 0;    
 
    CHK_FUN_RETVAL(reg_write(RTL8307H_UNIT, PORT0_PROPERTY_CONFIGURE + port, regval));                        

    /*switch phy to page 0          */
    phy_reg_write(port, 0, 31, 0); 
    
    /*triger, write configure to phy*/
    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PHY_AUTO_ACCESS_MASK, &regval));
    regval &= ~0x7FUL;
    regval |= (1UL << 16) | (1UL << port);
    CHK_FUN_RETVAL(reg_write(RTL8307H_UNIT, PHY_AUTO_ACCESS_MASK, regval));
    
    for (pollcnt = 0; pollcnt < RTL8307H_MAX_POLLCNT; pollcnt ++)    
    {
        CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PHY_AUTO_ACCESS_MASK, PHYCFG_CMD, &finished));
        if(!finished)
            break;
    }

    if(RTL8307H_MAX_POLLCNT == pollcnt)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_macForceLink_get
 * Description:
 *      Get port link ability
 * Input:
 *      port              -  Port id
 * Output:
 *      pPortability      -  the pointer of port link ability 
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_FAILED  
*       RT_ERR_PORT_ID
 *      RT_ERR_SMI       
 *      RT_ERR_NULL_POINTER
 *     
 * Note:
 *      structure member link means only when the port is (R)MII/TMII/GMII port.
 */
rtk_api_ret_t rtk_port_linkAbility_get(rtk_port_t port, rtk_port_link_ability_t *pPortability)
{
    uint32 regval, media_cap;
    uint32 bitpos;
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID; 

    if(NULL == pPortability)
        return RT_ERR_NULL_POINTER;
    
    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_PROPERTY_CONFIGURE + port, &regval));            

    media_cap = 3;
    for(bitpos = 7; bitpos >= 3; bitpos --)
    {
        if(regval & (1 << bitpos) )
        {
            if ((7 == bitpos) && (4 != port) && (5 != port))
            {
                continue;
            }
            else
            {
                media_cap = bitpos;
                break;
            }
        }
    }
    
    switch(media_cap)
    {
        case 7:
             pPortability->speed = PORT_SPEED_1000M;
             pPortability->duplex = PORT_FULL_DUPLEX;
             break;
        case 6:
             pPortability->speed = PORT_SPEED_100M;
             pPortability->duplex = PORT_FULL_DUPLEX;
             break;
        case 5:
             pPortability->speed = PORT_SPEED_100M;
             pPortability->duplex = PORT_HALF_DUPLEX;
             break;
        case 4:
             pPortability->speed = PORT_SPEED_10M;
             pPortability->duplex = PORT_FULL_DUPLEX;
             break;
        case 3:
             pPortability->speed = PORT_SPEED_10M;
             pPortability->duplex = PORT_HALF_DUPLEX;
             break;
        default:
             break;                        
    }
    
    pPortability ->link = (regval & (0x1 << 1)) ?  PORT_LINKUP : PORT_LINKDOWN;
    pPortability->nway = (regval & 0x1 << 2) ? 1 : 0;
    pPortability->txpause = (regval & (0x1 << 8)) ? 1 : 0;
    pPortability->rxpause = (regval & (0x1 << 9)) ? 1 : 0;    
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_linkStatus_get
 * Description:
 *      Get port mac linking status
 * Input:
 *      port             -  port id
 * Output:
 *      pStatus          -  port link status
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_FAILED                  
 *      RT_ERR_SMI       
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *     
 * Note:
 *      the API could get port status(link/duplex/speed/nway/txpause/rxpause/forcemode).
 */
rtk_api_ret_t rtk_port_linkStatus_get(rtk_port_t port, rtk_port_link_status_t *pStatus)
{
    uint32 regval;
#ifndef	EXT_LAB
    rtk_port_phy_ability_t ability;
#endif

    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID; 

    if(NULL == pStatus)
        return RT_ERR_NULL_POINTER;
    
    /*twice read will be stable*/
    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_LINK_STATUS + port, &regval));

    DEBUG_INFO(DBG_PORT, "regval: %08x\n", regval);
    pStatus->link = (regval & 0x1) ? PORT_LINKUP : PORT_LINKDOWN;
    switch ((regval >> 1) & 0x3)
    {
        case 0:
           pStatus->speed = PORT_SPEED_10M;
           break;
        case 1:
           pStatus->speed = PORT_SPEED_100M;
           break; 
        case 2:
           pStatus->speed = PORT_SPEED_1000M;
           break;  
        default:
            break;
    }    
    pStatus->duplex = (regval & (0x1 << 3)) ?  PORT_FULL_DUPLEX : PORT_HALF_DUPLEX;
    pStatus->txpause = (regval & (0x1 << 4)) ? 1 : 0;
    pStatus->rxpause = (regval & (0x1 << 5)) ? 1 : 0; 

    return RT_ERR_OK;     
}


/* Function Name:
 *      rtk_hec_mode_set
 * Description:
 *      This function is used to set the HEC mode of HEAC interface. 
 * Input:
 *      heac_id           -  HEAC interface id (0~4)
 *      mode              -  HEC operation mode of HEAC interface
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 *      RT_ERR_NOT_ALLOWED
 * Note:
 *      For source device, it is not allowed to enable HEC when ARC is in
 *   TX Single mode. In this case, the function returns RT_ERR_NOT_ALLOWED
 */
rtk_api_ret_t rtk_hec_mode_set(int heac_id, rtk_hec_mode_t mode)
{
    uint8 port_id;
    uint16 value;

    if (heac_id < 4 && heac_id >= 0)
        port_id = heac_id;
    else if (heac_id == 4)
        port_id = 6;
    else
        return RT_ERR_PORT_ID;

    phy_reg_read(port_id, 1, 29, &value);

    if ((mode == HEC_MODE_ENABLE) && ((value & 0x0018) == 0x0010))
    {  /* ARC in TX Single mode */
        return RT_ERR_NOT_ALLOWED;
    }

    if (mode == HEC_MODE_ENABLE)
        value = value | 0x0020;        /* bit 5 = 1 */
    else
        value = value & 0xFFDF;        /* bit 5 = 0 */

    phy_reg_write(port_id, 1, 29, value);

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_hec_mode_get
 * Description:
 *      This function is used to get the HEC mode of HEAC interface. 
 * Input:
 *      heac_id           -  heac id (0~4)
 * Output:
 *      mode              -  HEC operation mode of HEAC interface
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 */
rtk_api_ret_t rtk_hec_mode_get(int heac_id, rtk_hec_mode_t* mode)
{
    uint8 port_id;
    uint16 value;

    if (heac_id < 4 && heac_id >= 0)
        port_id = heac_id;
    else if (heac_id == 4)
        port_id = 6;
    else
        return RT_ERR_PORT_ID;

    phy_reg_read(port_id, 1, 29, &value);

    if (value & 0x0020)
        *mode = HEC_MODE_ENABLE;       /* bit 5 = 1 */
    else
        *mode = HEC_MODE_DISABLE;      /* bit 5 = 0 */

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_arc_mode_set
 * Description:
 *      This function is used to set the ARC mode of HEAC interface. 
 * Input:
 *      heac_id           -  HEAC interface id (0~4)
 *      mode              -  ARC operation mode of HEAC interface
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 *      RT_ERR_NOT_ALLOWED
 * Note:
 *      For source device, it is not allowed to enable ARC TX Single mode
 *   when HEC is enabled. In this case, the function returns RT_ERR_NOT_ALLOWED
 */
rtk_api_ret_t rtk_arc_mode_set(int heac_id, rtk_arc_mode_t mode)
{
    uint8 port_id;
    uint16 value;

    if (heac_id < 4 && heac_id >= 0)
        port_id = heac_id;
    else if (heac_id == 4)
        port_id = 6;
    else
        return RT_ERR_PORT_ID;

    phy_reg_read(port_id, 1, 29, &value);

    if ((mode == ARC_MODE_TX_SINGLE) && ((value & 0x0020) == 0x0020))
    {  /* HEC enabled */
        return RT_ERR_NOT_ALLOWED;
    }

    switch (mode)
    {
        case ARC_MODE_DISABLE:
            value = value & 0xffe7;    /* bits [4..3] = 0 */
            break;

        case ARC_MODE_TX_COMMON:
            value = value & 0xffe7;    /* bits [4..3] = 1 */
            value = value | 0x0008;
            break;

        case ARC_MODE_TX_SINGLE:
            value = value & 0xffe7;    /* bits [4..3] = 2 */
            value = value | 0x0010;
            break;

        case ARC_MODE_RX_BOTH:
            value = value | 0x0018;    /* bits [4..3] = 3 */
            break;

        default:
            break;
    }
    phy_reg_write(port_id, 1, 29, value);

    if (mode == ARC_MODE_TX_SINGLE)
    {     
        phy_reg_read(port_id, 1, 17, &value);
        value |= 0x20;
        phy_reg_write(port_id, 1, 17, value); 
    }
    else
    {
        phy_reg_read(port_id, 1, 17, &value);
        value &= ~0x20;
        phy_reg_write(port_id, 1, 17, value); 
    }
    
    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_arc_mode_get
 * Description:
 *      This function is used to get the ARC mode of HEAC interface. 
 * Input:
 *      heac_id           -  heac id (0~4)
 * Output:
 *      mode              -  ARC operation mode of HEAC interface
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 */
rtk_api_ret_t rtk_arc_mode_get(int heac_id, rtk_arc_mode_t* mode)
{
    uint8 port_id;
    uint16 value;

    if (heac_id < 4 && heac_id >= 0)
        port_id = heac_id;
    else if (heac_id == 4)
        port_id = 6;
    else
        return RT_ERR_PORT_ID;

    phy_reg_read(port_id, 1, 29, &value);

    value = (value >> 3) & 0x3;

    switch (value)
    {
        case 0:
            *mode = ARC_MODE_DISABLE;       /* bits [4..3] = 0 */
            break;

        case 1:
            *mode = ARC_MODE_TX_COMMON;     /* bits [4..3] = 1 */
            break;

        case 2:
            *mode = ARC_MODE_TX_SINGLE;     /* bits [4..3] = 2 */
            break;

        case 3:
            *mode = ARC_MODE_RX_BOTH;       /* bits [4..3] = 3 */
            break;

        default:
            *mode = ARC_MODE_END;
            break;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_spdif_set
 * Description:
 *      This function is used to connect the spdif with heac.
 * Input:
 *      spdif_id          -  spdif id(0~1)
 *      heac_id           -  heac id (0~4)
 * Output:
 *      direct            -  direction of SPDIF. 
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 * Note:
 *      This function will adjust the direction of SPDIF according ARC role.
 *
 *      For source devices with mode:
 *          ARC_MODE_RX_BOTH,
 *      RTL8307H will send out SPDIF signal.
 *
 *      For sink devices with mode:
 *          ARC_MODE_TX_COMMON,  
 *       or ARC_MODE_TX_SINGLE,
 *      RTL8307H will receive SPDIF signal.
 */
rtk_api_ret_t rtk_spdif_set(int spdif_id, int heac_id)
{
    uint32 reg_val;
    rtk_spdif_dir_t direct;
    rtk_arc_mode_t mode;
    
    if (heac_id < 0 || heac_id > 4)
        return RT_ERR_PORT_ID;

    if (spdif_id != 0 && spdif_id != 1)        
        return RT_ERR_INPUT;


    rtk_arc_mode_get(heac_id, &mode);

    switch (mode)
    {
        case ARC_MODE_TX_COMMON: 
        case ARC_MODE_TX_SINGLE:
            direct = SPDIF_SPDIF_INPUT;
            break;

        case ARC_MODE_RX_BOTH: 
            direct = SPDIF_SPDIF_OUTPUT;
            break;

        case ARC_MODE_DISABLE:
        default:
            direct = SPDIF_SPDIF_INPUT;
            break;
    }        

    reg_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, &reg_val);

    /* connect the spdif with heac */
    if (spdif_id == 0)
    {
        reg_val &= 0xffe2ffff; /* mask bits 16, 18-20 SEL_HEAC_SPDIF_IO0 and DIR_SPDIF_IO_0*/
        reg_val |= (uint32)heac_id << 18;
        if (direct == SPDIF_SPDIF_OUTPUT)
        {
           reg_val |= (uint32)1 << 16; 
        } 
    }               
    else
    {
        reg_val &= 0xff1dffff; /* mask bits 17, 21-23 SEL_HEAC_SPDIF_IO1 and DIR_SPDIF_IO_1*/
        reg_val |= (uint32)heac_id << 21;
        if (direct == SPDIF_SPDIF_OUTPUT)
        {
           reg_val |= (uint32)1 << 17; 
        } 
    }

    reg_write(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, reg_val);

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_spdif_get
 * Description:
 *      This function is used to get the status of SPDIF IO, including the connected
 *      HEAC Port and the direction of SPDIF.
 * Input:
 *      spdif_id          -  spdif id(0~1)
 * Output:
 *      heac_id           -  heac id (0~4)
 *      direct            -  direction of SPDIF. 
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 * Note:
 *      For source devices, RTL8307H will send out SPDIF signal(SPDIF_SPDIF_OUTPUT). 
 *      For sink devices, RTL8307H will receive SPDIF signal(SPDIF_SPDIF_INPUT). 
 */
rtk_api_ret_t rtk_spdif_get(int spdif_id, int* heac_id, rtk_spdif_dir_t* spdif_dir)
{
    uint32 reg_val;
    uint8 direct;
    
    reg_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, &reg_val);

    /* connect the spdif with heac */
    if (spdif_id == 0)
    {
        *heac_id = (reg_val>> 18) & 0x7;
        direct  = (reg_val>> 16) & 0x1;
    }               
    else
    {
        *heac_id = (reg_val>> 21) & 0x7;
        direct  = (reg_val>> 17) & 0x1;
    }

    if (direct)
        *spdif_dir = SPDIF_SPDIF_OUTPUT;
    else
        *spdif_dir = SPDIF_SPDIF_INPUT;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_mac6_interface_set
 * Description:
 *      This function is used to set port interface type. 
 * Input:
 *      mode           -  Specify whether MAC6 is Fast Ethernet port or HEAC port
 * Output:
 *      None
 * Return:
 *      None                        
 * Note:
 *      
 */            
void rtk_mac6_interface_set(rtk_port_interface_t mode)
{
    uint8 port = 6;
    rtk_port_link_ability_t ability;
    uint32 regval;

    memset(&ability, 0, sizeof(rtk_port_link_ability_t));

    reg_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, &regval);

    regval &= ~(0x7UL << 3);
    if (mode == RTK_PORT_INTERFACE_FE) 
        regval |= 7 << 3;
    else if (mode == RTK_PORT_INTERFACE_HEAC) 
        regval |= 6 << 3;
        
    reg_write(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, regval);
    
    if (mode == RTK_PORT_INTERFACE_HEAC)   
    {
        /* Disable HEAC */
        phy_reg_write(port, 1, 29, 0x5); 

        /* Adjust Parameter of HEAC phy */
        phy_reg_write(port, 24, 29, 0x121f);  
        phy_reg_write(port, 25, 23, 0x021f);  
        phy_reg_write(port, 1, 0x1c, 0x3088);  
        phy_reg_write(port, 0, 28, 0x40c2);         
        phy_reg_write(port, 1, 17, 0x5a10); 

        /* Enable HEAC */
        phy_reg_write(port, 1, 29, 0x25); 

        /* Sync MAC & PHY */
        ability.nway      = 0;          /* Disable NWAY */ 
        ability.speed     = PORT_SPEED_100M;
        ability.duplex    = PORT_FULL_DUPLEX;
        ability.rxpause   = 1;
        ability.txpause   = 1; 
        ability.link      = 1;
        rtk_port_linkAbility_set(port, &ability);
        
    }
    else if(mode == RTK_PORT_INTERFACE_FE)
    {
        /* Adjust Parameter of FE phy */
        phy_reg_write(6, 1, 29, 0x5); 
        phy_reg_write(6, 1, 28, 0x8388); 

        /* waveform parameter */
        phy_reg_write(6, 1, 19, 0x8800); 

        /* Sync MAC & PHY */
        ability.nway      = 1;         
        ability.speed     = PORT_SPEED_100M | PORT_SPEED_10M;
        ability.duplex    = PORT_FULL_DUPLEX | PORT_HALF_DUPLEX;
        ability.rxpause   = 1;
        ability.txpause   = 1; 
        ability.link      = 1;  
        
        rtk_port_linkAbility_set(port, &ability);
    }    
}

/* Function Name:
 *      rtk_mac6_interface_get
 * Description:
 *      This function is used to get the interface type of MAC 6. 
 * Input:
 *      mode           -  pointer to rtk_port_interface_t
 * Output:
 *      None
 * Return:
 *      None                        
 * Note:
 *      
 */            
void rtk_mac6_interface_get(rtk_port_interface_t *mode)
{
    uint32 regval;
    uint8 ifType;
    
    reg_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, &regval);
    ifType = (regval >> 3) & 0x7;

    switch (ifType)
    {
        case 7: /* Fast Ethernet port */
            *mode = RTK_PORT_INTERFACE_FE;
            break;
        case 6: /* HEAC port          */
            *mode = RTK_PORT_INTERFACE_HEAC;
            break;  
        default:
            *mode = RTK_PORT_INTERFACE_END;
            break;
    }
}


/* Function Name:
 *      rtk_wol_event_set
 * Description:
 *      When one of interested events occurs, a wake-up signal will be sent
 *      out to external CPU.
 *      The interested event will be:
 *          WOL_EVENT_LINKUP        -   port link up
 *          WOL_EVENT_MAGIC_PKT     -   AMD Magic packet
 *          WOL_EVENT_WAKEUP_PKT    -   Wake up frame 
 *          WOL_EVENT_ANY_PKT_PORT5 -   Any packet transmitting to port 5
 *          WOL_EVENT_ANY_PKT_PORT6 -   Any packet transmitting to port 6
 * Input:
 *      event_mask                   -  event mask. 
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      These events can be bitwised OR. 
 */
rtk_api_ret_t rtk_wol_event_set(uint8 event_mask)
{
    uint32 reg_val;
#ifndef	EXT_LAB
    uint32 mask;
#endif

    /* clear bit WFEn,MPEn,LUEn and keep WI_flag (clear Interrupt pending flag to avoid write 1 to clear interrupt pending)*/
    reg_read(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, &reg_val);
    reg_val &= 0xffffff8d;
    
    if (event_mask & WOL_EVENT_LINKUP)
        reg_val |= 0x40;
        
    if (event_mask & WOL_EVENT_MAGIC_PKT)
        reg_val |= 0x20;
        
    if (event_mask & WOL_EVENT_WAKEUP_PKT)
        reg_val |= 0x10;

    reg_write(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, reg_val);  

    if (event_mask & WOL_EVENT_ANY_PKT_PORT5)
    {
        reg_field_read(RTL8307H_UNIT, CPU_TAG_CONTROL, CPU0ENABLE, &reg_val);
        if (reg_val == 0)
            return RT_ERR_WOL_CPU_PORT_NOT_EN;
        else            
            reg_field_write(RTL8307H_UNIT, WOL_CONTROL, WAKEUP_CPUEN0, 1);
    }

    if (event_mask & WOL_EVENT_ANY_PKT_PORT6)
    {
        reg_field_read(RTL8307H_UNIT, CPU_TAG_CONTROL, CPU1ENABLE, &reg_val);
        if (reg_val == 0)
            return RT_ERR_WOL_CPU_PORT_NOT_EN;
        else            
            reg_field_write(RTL8307H_UNIT, WOL_CONTROL, WAKEUP_CPUEN1, 1);
    }
    
    return RT_ERR_OK;    
}

/* Function Name:
 *      rtk_wol_event_get
 * Description:
 *      When one of interested events occurs, a wake-up signal will be sent
 *      out to external CPU.
 *      The interested event will be:
 *          WOL_EVENT_LINKUP        -   port link up
 *          WOL_EVENT_MAGIC_PKT     -   AMD Magic packet
 *          WOL_EVENT_WAKEUP_PKT    -   Wake up frame 
 *          WOL_EVENT_ANY_PKT_PORT5 -   Any packet transmitting to port 5
 *          WOL_EVENT_ANY_PKT_PORT6 -   Any packet transmitting to port 6
 *      This function gets the interested events list. 
 * Input:
 *      event_mask                   -  event mask. 
 * Return: 
 *      RT_ERR_OK
 */
rtk_api_ret_t rtk_wol_event_get(uint8* event_mask)
{
    uint32 reg_val;

    *event_mask = 0;
    
    reg_field_read(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, LUEN, &reg_val);
    if (reg_val == 1)
        *event_mask |= WOL_EVENT_LINKUP;

    reg_field_read(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, MPEN, &reg_val);
    if (reg_val == 1)
        *event_mask |= WOL_EVENT_MAGIC_PKT;     

    reg_field_read(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, WFEN, &reg_val);
    if (reg_val == 1)
        *event_mask |= WOL_EVENT_WAKEUP_PKT;   

    reg_field_read(RTL8307H_UNIT, WOL_CONTROL, WAKEUP_CPUEN0, &reg_val);
    if (reg_val == 1)
        *event_mask |= WOL_EVENT_ANY_PKT_PORT5;  

    reg_field_read(RTL8307H_UNIT, WOL_CONTROL, WAKEUP_CPUEN1, &reg_val);
    if (reg_val == 1)
        *event_mask |= WOL_EVENT_ANY_PKT_PORT6; 
        
    return RT_ERR_OK;         
}

/* Function Name:
 *      rtk_wol_nodeid_set
 * Description:
 *      This function is called when AMD Magic Packet is one of the interested events. 
 *      see function rtk_wol_event_set(). 
 *      Once AMD Magic Packet is selected as one of the interested events, node id should 
 *      be initialized.
 * Input:
 *      node_group              -  Group number which can be either 1, 2 or 3. 
 *      node_id                 -  A six byte array, usually switch own MAC.
 * Output:
 *      None
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      
 */
rtk_api_ret_t rtk_wol_nodeid_set(uint8 node_group, uint8* node_id)
{
    uint32 reg_lsb;
    uint32 reg_msb;

    if (node_group != 1 && node_group != 2 && node_group != 3)
        return RT_ERR_WOL_INVALID_NODEID;

    reg_msb = 0x10000 | ((uint32)node_id[0] << 8) | node_id[1];
    reg_lsb = ((uint32)node_id[2] << 24) | ((uint32)node_id[3] << 16) | \
               ((uint32)node_id[4] << 8) | node_id[5];

    if (node_group == 1)
    {
        reg_write(RTL8307H_UNIT, LSB_NODE_ID_1, reg_lsb);
        reg_write(RTL8307H_UNIT, HSB_NODE_ID_1, reg_msb);
    }
    else if (node_group == 2)
    {
        reg_write(RTL8307H_UNIT, LSB_NODE_ID_2, reg_lsb);
        reg_write(RTL8307H_UNIT, HSB_NODE_ID_2, reg_msb);
    }
    else if (node_group == 3)
    {
        reg_write(RTL8307H_UNIT, LSB_NODE_ID_3, reg_lsb);
        reg_write(RTL8307H_UNIT, HSB_NODE_ID_3, reg_msb);
    }        

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_wol_nodeid_get
 * Description:
 *      This function is called when AMD Magic Packet is one of the interested events. 
 *      see function rtk_wol_event_set(). 
 *      Once AMD Magic Packet is selected as one of the interested events, node id should 
 *      be initialized. This function is used to get the contents of node id for specific group.
 * Input:
 *      node_group              -  Group number which can be either 1, 2 or 3. 
 * Output:
 *      valid                   -  Whether the content of node_id is valid: 1 - valid; 0 - invalid
 *      node_id                 -  The content of node_id, a six byte array.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      
 */
rtk_api_ret_t rtk_wol_nodeid_get(uint8 node_group, uint8* valid, uint8* node_id)
{
    uint32 reg_lsb;
    uint32 reg_msb;

    if (node_group != 1 && node_group != 2 && node_group != 3)
        return RT_ERR_WOL_INVALID_NODEID;
        
    if (node_group == 1)
    {
        reg_read(RTL8307H_UNIT, LSB_NODE_ID_1, &reg_lsb);
        reg_read(RTL8307H_UNIT, HSB_NODE_ID_1, &reg_msb);
    }
    else if (node_group == 2)
    {
        reg_read(RTL8307H_UNIT, LSB_NODE_ID_2, &reg_lsb);
        reg_read(RTL8307H_UNIT, HSB_NODE_ID_2, &reg_msb);
    }
    else if (node_group == 3)
    {
        reg_read(RTL8307H_UNIT, LSB_NODE_ID_3, &reg_lsb);
        reg_read(RTL8307H_UNIT, HSB_NODE_ID_3, &reg_msb);
    }        

    if (reg_lsb & 0x10000)
    {
        *valid = 1;

        node_id[0] = (reg_lsb >> 8) & 0xff;
        node_id[1] = reg_lsb & 0xff;

        node_id[2] = (reg_msb >> 24) & 0xff;
        node_id[3] = (reg_msb >> 16) & 0xff;
        node_id[4] = (reg_msb >> 8) & 0xff;
        node_id[5] = reg_msb & 0xff;
    }
    else
    {
        *valid = 0;
        memset(node_id, 0, 6);
    }
               
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_wol_wakup_sample_set
 * Description:
 *      This function is called when wake up frame is one of the interested events. 
 *      see function rtk_wol_event_set(). 
 *      Once wake up frame is selected as one of the interested events, wake up sample frame
 *      should be initialized.
 * Input:
 *      group                   -  Group number which can be either 1, 2. 
 *      frame                   -  A sample packet, starting from DMAC. The length of the packet should be no more than 128 bytes. 
 *      len                     -  the length of the packet. It should be no more than 128 bytes.
 *      mask                    -  A 16-byte array, used for masking at most 128-byte sample packet. 
 * Output:
 *      None
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      When bit n in mask is 1, the corresponding byte n in the frame is valid and will be compared
 *      Otherwise, the byte n in the frame is ignored.
 *      The length of the sample packet should be no more than 128 bytes. The bytes above 128 bytes are 
 *      ignored.
 */
void rtk_wol_wakup_sample_set(uint8 group, uint8* frame, uint16 len, uint8* mask)
{
    uint16 crc;
    uint8 message[128];
#ifndef	EXT_LAB
    uint8 i;
#endif
    uint32 regVal;

    if (len >= 128)
        len = 128;
        
    memcpy(message, frame, len);
 
    crc = rtl8307h_wol_ccitt_crc16(message, mask, len);

    /* fill the mask */
    if (group == 1)
    {
        regVal = ((uint32)mask[15] << 24) | ((uint32)mask[14] << 16) | ((uint32)mask[13] << 8) | mask[12];
        reg_write(RTL8307H_UNIT, WAKEUP_FRAME_MASK0_0, regVal);

        regVal = ((uint32)mask[11] << 24) | ((uint32)mask[10] << 16) | ((uint32)mask[9] << 8) | mask[8];
        reg_write(RTL8307H_UNIT, WAKEUP_FRAME_MASK0_1, regVal);

        regVal = ((uint32)mask[7] << 24) | ((uint32)mask[6] << 16) | ((uint32)mask[5] << 8) | mask[4];
        reg_write(RTL8307H_UNIT, WAKEUP_FRAME_MASK0_2, regVal);
        regVal = ((uint32)mask[3] << 24) | ((uint32)mask[2] << 16) | ((uint32)mask[1] << 8) | mask[0];
        reg_write(RTL8307H_UNIT, WAKEUP_FRAME_MASK0_3, regVal);

        regVal = crc & 0xff;
        reg_field_write(RTL8307H_UNIT, WPF_CRC_MATCH0, LSBCRC0, regVal);

        regVal = (crc >> 8) & 0xff;
        reg_field_write(RTL8307H_UNIT, WPF_CRC_MATCH0, HSBCRC0, regVal);
    }
    else
    {
        regVal = ((uint32)mask[15] << 24) | ((uint32)mask[14] << 16) | ((uint32)mask[13] << 8) | mask[12];
        reg_write(RTL8307H_UNIT, WAKEUP_FRAME_MASK1_0, regVal);

        regVal = ((uint32)mask[11] << 24) | ((uint32)mask[10] << 16) | ((uint32)mask[9] << 8) | mask[8];
        reg_write(RTL8307H_UNIT, WAKEUP_FRAME_MASK1_1, regVal);

        regVal = ((uint32)mask[7] << 24) | ((uint32)mask[6] << 16) | ((uint32)mask[5] << 8) | mask[4];
        reg_write(RTL8307H_UNIT, WAKEUP_FRAME_MASK1_2, regVal);
        regVal = ((uint32)mask[3] << 24) | ((uint32)mask[2] << 16) | ((uint32)mask[1] << 8) | mask[0];
        reg_write(RTL8307H_UNIT, WAKEUP_FRAME_MASK1_3, regVal);

        regVal = crc & 0xff;
        reg_field_write(RTL8307H_UNIT, WPF_CRC_MATCH0, LSBCRC1, regVal);

        regVal = (crc >> 8) & 0xff;
        reg_field_write(RTL8307H_UNIT, WPF_CRC_MATCH0, HSBCRC1, regVal);        
    }
}

/* Function Name:
 *      rtk_wol_wakup_sample_get
 * Description:
 *      This function is called when wake up frame is one of the interested events. 
 *      see function rtk_wol_event_set(). 
 *      Once wake up frame is selected as one of the interested events, wake up sample frame
 *      should be initialized.
 *      This function is used to get the contents of node id for specific group.
 * Input:
 *      group                   -  Group number which can be either 1, 2. 
 * Output:
 *      mask                    -  A 16-byte array, used for masking the 128-byte sample packet. 
 *      crc                     -  Crc value.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      When bit n in mask is 1, the corresponding byte n in the frame is valid and will be compared
 *      Otherwise, the byte n in the frame is ignored.    
 */
void rtk_wol_wakup_sample_get(uint8 group, uint8* mask, uint16* crc)
{
    uint32 regVal;

    /* fill the mask */
    if (group == 1)
    {
        reg_read(RTL8307H_UNIT, WAKEUP_FRAME_MASK0_0, &regVal);
        mask[12] = regVal & 0xff;
        mask[13] = (regVal >> 8) & 0xff;
        mask[14] = (regVal >> 16)& 0xff;
        mask[15] = (regVal >> 24)& 0xff;

        reg_read(RTL8307H_UNIT, WAKEUP_FRAME_MASK0_1, &regVal);
        mask[8] = regVal & 0xff;
        mask[9] = (regVal >> 8) & 0xff;
        mask[10] = (regVal >> 16)& 0xff;
        mask[11] = (regVal >> 24)& 0xff;

        reg_read(RTL8307H_UNIT, WAKEUP_FRAME_MASK0_2, &regVal);
        mask[4] = regVal & 0xff;
        mask[5] = (regVal >> 8) & 0xff;
        mask[6] = (regVal >> 16)& 0xff;
        mask[7] = (regVal >> 24)& 0xff;

        reg_read(RTL8307H_UNIT, WAKEUP_FRAME_MASK0_3, &regVal);
        mask[0] = regVal & 0xff;
        mask[1] = (regVal >> 8) & 0xff;
        mask[2] = (regVal >> 16)& 0xff;
        mask[3] = (regVal >> 24)& 0xff;
        
        reg_field_read(RTL8307H_UNIT, WPF_CRC_MATCH0, HSBCRC0, &regVal);
        *crc = regVal;

        reg_field_read(RTL8307H_UNIT, WPF_CRC_MATCH0, LSBCRC0, &regVal);
        *crc = (*crc << 8) | regVal;        
    }
    else
    {
        reg_read(RTL8307H_UNIT, WAKEUP_FRAME_MASK1_0, &regVal);
        mask[12] = regVal & 0xff;
        mask[13] = (regVal >> 8) & 0xff;
        mask[14] = (regVal >> 16)& 0xff;
        mask[15] = (regVal >> 24)& 0xff;

        reg_read(RTL8307H_UNIT, WAKEUP_FRAME_MASK1_1, &regVal);
        mask[8] = regVal & 0xff;
        mask[9] = (regVal >> 8) & 0xff;
        mask[10] = (regVal >> 16)& 0xff;
        mask[11] = (regVal >> 24)& 0xff;

        reg_read(RTL8307H_UNIT, WAKEUP_FRAME_MASK1_2, &regVal);
        mask[4] = regVal & 0xff;
        mask[5] = (regVal >> 8) & 0xff;
        mask[6] = (regVal >> 16)& 0xff;
        mask[7] = (regVal >> 24)& 0xff;

        reg_read(RTL8307H_UNIT, WAKEUP_FRAME_MASK1_3, &regVal);
        mask[0] = regVal & 0xff;
        mask[1] = (regVal >> 8) & 0xff;
        mask[2] = (regVal >> 16)& 0xff;
        mask[3] = (regVal >> 24)& 0xff;
        
        reg_field_read(RTL8307H_UNIT, WPF_CRC_MATCH0, HSBCRC1, &regVal);
        *crc = regVal;

        reg_field_read(RTL8307H_UNIT, WPF_CRC_MATCH0, LSBCRC1, &regVal);
        *crc = (*crc << 8) | regVal;  
    }    
}


/* Function Name:
 *      rtk_wol_sig_type_set
 * Description:
 *      When one of interested events occurs, a wake-up signal will be sent
 *      out to external CPU via WOL pin. 
 *      This function is used to set wake-up signal type.
 *      There are four types we can choose:
 *          LEVEL_HIGH              -   high voltage level
 *          LEVEL_LOW               -   low voltage level
 *          PULSE_POSITIVE          -   positive pulse with duration 150ms
 *          PULSE_NEG               -   negative pulse with duration 150ms
 *      When PULSE_POSITIVE or PULSE_NEG option is selected, there are four different time intervals between pulse.
 *              WOL_16NS            -   16   ns
 *              WOL_128NS           -   128  ns
 *              WOL_2048NS          -   2048 ns
 *              WOL_65536NS         -   65536ns
 * Input:
 *      type                    -  Wake-up signal type.
 *      interval                -  Time intervals between pulse when the signal type is either PULSE_POSITIVE or PULSE_NEG.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *     This wake-up signal is sent out via WOL pin. It is irrelative with switch interrupt registers(WOLINT_IE_CPU, WOLPKT_DROP_IE_CPU, WOLPKT_DROP_IP_CPU).
 */
rtk_api_ret_t rtk_wol_sig_type_set(rtk_wol_intr_type_t type, rtk_wol_intr_pulse_interval_t interval)
{
    uint32 reg_val;
    
    reg_read(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, &reg_val);
    reg_val &= 0xfffffffd;   /* keep interrupt flag */

    reg_val &= 0xfffffff3;   /* clear wake up signal*/
    if (type == LEVEL_LOW)
        reg_val |= 0x1 << 2;
    else if (type == PULSE_POSITIVE)
        reg_val |= 0x2 << 2;
    else if (type == PULSE_NEG)
        reg_val |= 0x3 << 2;        

    reg_write(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, reg_val);

    if (type == PULSE_POSITIVE || type == PULSE_NEG)
    {
        reg_field_write(RTL8307H_UNIT, WOL_CONTROL, SW_WOL_PULSE_INTERVAL, interval);    
    }
    
    return RT_ERR_OK; 
}

/* Function Name:
 *      rtk_wol_sig_type_get
 * Description:
 *      When one of interested events occurs, a wake-up signal will be sent
 *      out to external CPU via WOL pin.
 *      This function is used to get wake-up signal type.
 *      There are four types for the wake-up signal:
 *          LEVEL_HIGH              -   high voltage level
 *          LEVEL_LOW               -   low voltage level
 *          PULSE_POSITIVE          -   positive pulse with duration 150ms
 *          PULSE_NEG               -   negative pulse with duration 150ms
 *      When PULSE_POSITIVE or PULSE_NEG option is selected, there are four different time intervals between pulse.
 *              WOL_16NS            -   16   ns
 *              WOL_128NS           -   128  ns
 *              WOL_2048NS          -   2048 ns
 *              WOL_65536NS         -   65536ns
 * Input:
 *      None
 * Output:
 *      type                    -  Wake-up signal type.
 *      interval                -  Time intervals between pulse when the signal type is either PULSE_POSITIVE or PULSE_NEG.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *     This wake-up signal is sent out via WOL pin. It is irrelative with switch interrupt registers(WOLINT_IE_CPU, WOLPKT_DROP_IE_CPU, WOLPKT_DROP_IP_CPU). 
 */
rtk_api_ret_t rtk_wol_sig_type_get(rtk_wol_intr_type_t* type, rtk_wol_intr_pulse_interval_t* interval)
{
    uint32 reg_val;
    
    reg_read(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, &reg_val);
    reg_val = (reg_val >> 2) & 0x3;

    if (reg_val == 0)
        *type = LEVEL_HIGH;
    else if (reg_val == 1)
        *type = LEVEL_LOW;
    else if (reg_val == 2)
        *type = PULSE_POSITIVE;
    else
        *type = PULSE_NEG;
     
    if (*type == PULSE_POSITIVE || *type == PULSE_NEG)
    {
        reg_field_read(RTL8307H_UNIT, WOL_CONTROL, SW_WOL_PULSE_INTERVAL, &reg_val);
        *interval = reg_val;
    }
    else
        *interval = 0;
        
    return RT_ERR_OK; 
}

/* Function Name:
 *      rtk_wol_sig_status_get
 * Description:
 *      When a wake-up signal is sent, pending flag will be set to 1.
 *      This flag can be cleared by external CPU using rtk_wol_sig_status_clear().
 * Input:
 *      pending                 -  Flag specify whether the a wake-up signal is sent.
 * Output:
 *      None
 * Return: 
 *      None     
 * Note:
 *     This wake-up signal is sent out via WOL pin. It is irrelative with switch interrupt registers(WOLINT_IE_CPU, WOLPKT_DROP_IE_CPU, WOLPKT_DROP_IP_CPU).    
 */
void rtk_wol_sig_status_get(uint8* pending)
{
    uint32 reg_val;
    reg_field_read(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, WI_FLAG, &reg_val);

    *pending = reg_val;
}

/* Function Name:
 *      rtk_wol_sig_status_clear
 * Description:
 *      When a wake-up signal is sent, pending flag will be set to 1.
 *      This function is used to clear the pending flag.
 * Input:
 *      None
 * Output:
 *      None
 * Return: 
 *      None     
 * Note:
 *     This wake-up signal is sent out via WOL pin. It is irrelative with switch interrupt registers(WOLINT_IE_CPU, WOLPKT_DROP_IE_CPU, WOLPKT_DROP_IP_CPU).    
 */
void rtk_wol_sig_status_clear(void)
{
    reg_field_write(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, WI_FLAG, 1);
}

/* Function Name:
 *      rtk_wol_enter_sleep
 * Description:
 *      After external CPU has initialized WOL function, please see the programming guide for initialization.
 *      Function rtk_wol_enter_sleep() will be called when external CPU wants to sleep.
 *      Once this function is called and one of interested events occurs,
 *      a wake-up signal will be sent out from RTL8307H to external CPU.
 * Input:
 *      port                    -  port id
 * Output:
 *      None
 * Return: 
 *      RT_ERR_PORT_ID
 *      RT_ERR_OK
 *      RT_ERR_FAILED     
 * Note:
 */
rtk_api_ret_t rtk_wol_enter_sleep(uint32 port)
{
    uint32 reg_val;
    uint32 cpu0valid;
    uint32 cpu1valid;    

    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;  
        
    /* Set GPIOB as WOL */   
    reg_field_write(RTL8307H_UNIT, GPIO_FUNCSELECT_DIRECTR_DATA_REG, SEL_WOL_GPIO, 0);
            
    /* release the packets which is queued in the cpu ports */  
    reg_field_read(RTL8307H_UNIT, CPU_TAG_CONTROL, CPU0ENABLE, &cpu0valid);
    reg_field_read(RTL8307H_UNIT, CPU_TAG_CONTROL, CPU1ENABLE, &cpu1valid);
        
    if ((port == 5 && cpu0valid) || (port == 6 && cpu1valid))
        reg_field_write(RTL8307H_UNIT, WOL_CONTROL, WAKEUP_CPUPKTEN, 0);  
        
    /* set WOLEn bit and WOLDetPortn bit */
    reg_read(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, &reg_val);
    reg_val &= 0xfffffffd;   /* keep interrupt flag */ 
    
    reg_val |= 0x80 | (1UL << (port + 8));
    reg_write(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, reg_val);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_wol_exit_sleep
 * Description:
 *      After rtk_wol_enter_sleep() is called and one of interested events occurs,
 *      a wake-up signal will be sent out from RTL8307H to external CPU.
 *      This function should be called to put switch back to original state.
 * Input:
 *      port                    -  port id
 * Output:
 *      None
 * Return: 
 *      RT_ERR_PORT_ID
 *      RT_ERR_OK
 *      RT_ERR_FAILED     
 * Note:
 *      After calling this function, interrupt pending flag at external CPU should be cleared.
 */
rtk_api_ret_t rtk_wol_exit_sleep(uint32 port)
{
    uint32 reg_val;
    uint32 cpu0valid;
    uint32 cpu1valid;    

    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;  
    
    /* release the packets which is queued in the cpu ports */  
    reg_field_read(RTL8307H_UNIT, CPU_TAG_CONTROL, CPU0ENABLE, &cpu0valid);
    reg_field_read(RTL8307H_UNIT, CPU_TAG_CONTROL, CPU1ENABLE, &cpu1valid);
        
    if ((port == 5 && cpu0valid) || (port == 6 && cpu1valid))
        reg_field_write(RTL8307H_UNIT, WOL_CONTROL, WAKEUP_CPUPKTEN, 1);  

    /* clear interrupt flag and exit wol function */  
    reg_read(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, &reg_val);   
        
    reg_val &= ~(1UL << (port + 8));
    if (!(reg_val & 0x7f00)) /* no port is enabled, disable wol function globally */
        reg_val &= ~0x80UL;
    
    reg_write(RTL8307H_UNIT, WOL_GLOBAL_CONTROL, reg_val);   
      
    return RT_ERR_OK; 
}


/* Function Name:
 *      rtk_wol_magicpacket_behaviour_set
 * Description:
 *      This function is used to set the behaviour of switch when receiving a 
 *      AMD Magic packet. There are two behaviour can selected: Drop or forward the packet.
 *          
 * Input:
 *      type                    -  Drop or forward the AMD Magic packet.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      
 */
void rtk_wol_magicpacket_behaviour_set(rtk_wol_pkt_behaviour_t type)
{
    reg_field_write(RTL8307H_UNIT, WOL_CONTROL, FWD_MP, type);
}

/* Function Name:
 *      rtk_wol_magicpacket_behaviour_get
 * Description:
 *      This function is used to get the behaviour of switch when receiving a 
 *      AMD Magic packet. There may be two behaviour: Drop or forward the packet.  
 * Input:
 *      None
 * Output:
 *      type                    -  Drop or forward the AMD Magic packet.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      
 */
void rtk_wol_magicpacket_behaviour_get(rtk_wol_pkt_behaviour_t* type)
{
    uint32 reg_val;
    reg_field_read(RTL8307H_UNIT, WOL_CONTROL, FWD_MP, &reg_val);

    *type = reg_val;
}

/* Function Name:
 *      rtk_wol_wakuppacket_behaviour_set
 * Description:
 *      This function is used to set the behaviour of switch when receiving a 
 *      wake up packet. There are two behaviour can selected: Drop or forward the packet.
 *          
 * Input:
 *      type                    -  Drop or forward the wake up.
 * Output:
 *      None
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      
 */
void rtk_wol_wakuppacket_behaviour_set(rtk_wol_pkt_behaviour_t type)
{
    reg_field_write(RTL8307H_UNIT, WOL_CONTROL, FWD_WF, type);
}

/* Function Name:
 *      rtk_wol_wakuppacket_behaviour_get
 * Description:
 *      This function is used to get the behaviour of switch when receiving a 
 *      wake up packet. There may be two behaviour: Drop or forward the packet.  
 * Input:
 *      None
 * Output:
 *      type                    -  Drop or forward the wake up.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      
 */
void rtk_wol_wakuppacket_behaviour_get(rtk_wol_pkt_behaviour_t* type)
{
    uint32 reg_val;
    reg_field_read(RTL8307H_UNIT, WOL_CONTROL, FWD_WF, &reg_val);

    *type = reg_val;

}


/* Function Name:
 *      rtk_igmp_init
 * Description:
 *      initialize igmp function  
 * Input:
 *      igmpEnable  -  enable or disable igmp function, the value should be 0x0 or 0x1
 *      mldEnable  -  enable or disable mld function, the value should be 0x0 or 0x1
 *      vlanLeakyEnable  -  enable or disable cross vlan function, the value should be 0x0 or 0x1
 *      fastLeaveEnable  -  enable or disable fast leave function, the value should be 0x0 or 0x1
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can initialize igmp parameters, including igmp/mld enable, cross_vlan, fast leave enable. mld enable should base on fixed fid. 
 */
rtk_api_ret_t rtk_igmp_init(rtk_enable_t igmpEnable, rtk_enable_t mldEnable, rtk_enable_t vlanLeakyEnable, rtk_enable_t fastLeaveEnable)
{
    uint32 regval;
    ret_t retVal;
    if((retVal = rtl8307h_igmp_asicIgmpEnable_set(igmpEnable)) != SUCCESS)
        return retVal;
    if((retVal = rtl8307h_igmp_asicMldEnable_set(mldEnable)) != SUCCESS)
        return retVal;   
    if((retVal = rtl8307h_igmp_asicIpMulticastVlanLeaky_set(vlanLeakyEnable)) != SUCCESS)
        return retVal;
    if((retVal = rtl8307h_igmp_asicFastLeaveEnable_set(fastLeaveEnable)) != SUCCESS)
        return retVal;

    /* Trap IGMP/MLD packets trap to cpu*/
    RTL8307H_I2C_READ(0xbb510000, &regval);

     if (mldEnable == 1)
     {
        regval  |= 1UL << 9;  
        regval  |= 4UL << 4;  
     }
     else
        regval &=  ~(3UL << 9);

     if (igmpEnable == 1)
     {
        regval  |= 1UL << 7;  
        regval  |= 4UL << 4;  
     }
     else
        regval &=  ~(3UL << 7);
    
    RTL8307H_I2C_WRITE(0xbb510000, regval);

    /*enable group table look up, ip look up, vlan leaky*/
    RTL8307H_I2C_READ(0xbb110000, &regval);
    if (mldEnable == 1 || igmpEnable == 1)
    {
        regval |= ((1UL  << 14) |1);                 
    }
    else
        regval &=  ~((1UL  << 14) |1);

    if(vlanLeakyEnable == 1)
    {
        regval |= 1UL  << 23; 
    }
    else
    {
        regval &=  ~(1UL << 23);
    }

    RTL8307H_I2C_WRITE(0xbb110000, regval);

    
    RTL8307H_I2C_WRITE(0xbb130004, 0);
    RTL8307H_I2C_WRITE(0xbb130008, 1);
    while(1)
    {
        RTL8307H_I2C_READ(0xbb130008, &regval);   
        if (!(regval & 0x1))
            break;
    }
    
    if((retVal = rtl8307h_igmp_paraChanged_reset()) != SUCCESS)
        return retVal;

    return RT_ERR_OK;   
}
/* Function Name:
 *      rtk_igmp_getInitConfig
 * Description:
 *      get igmp function configuration    
 * Input:
 *      none
 * Output:
 *      igmpEnable  -  igmp function enabled or disabled, the value should be 0x0 or 0x1
 *      mldEnable  -  mld function enabled or disabled, the value should be 0x0 or 0x1
 *      vlanLeakyEnable  -  cross vlan function enabled or disabled, the value should be 0x0 or 0x1
 *      fastLeaveEnable  -   fast leave function enabled or disabled, the value should be 0x0 or 0x1
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can get igmp function configuration 
 */
rtk_api_ret_t rtk_igmp_getInitConfig(rtk_enable_t* igmpEnable, rtk_enable_t* mldEnable, rtk_enable_t* vlanLeakyEnable, rtk_enable_t* fastLeaveEnable)
{
    ret_t retVal;
#ifdef	EXT_LAB
	uint32 regVal;

	*igmpEnable = DISABLED;
	if((retVal = rtl8307h_igmp_asicIgmpEnable_get(&regVal)) != SUCCESS)
		return retVal;
	*igmpEnable = (rtk_enable_t)regVal;

	*mldEnable = DISABLED;
	if((retVal = rtl8307h_igmp_asicMldEnable_get( &regVal)) != SUCCESS)
		return retVal;
	*mldEnable = (rtk_enable_t)regVal;

	*vlanLeakyEnable = DISABLED;
	if((retVal = rtl8307h_igmp_asicIpMulticastVlanLeaky_get( &regVal)) != SUCCESS)
		return retVal;
	*vlanLeakyEnable = (rtk_enable_t)regVal;

	*fastLeaveEnable = DISABLED;
	if((retVal = rtl8307h_igmp_asicFastLeaveEnable_get(&regVal)) != SUCCESS)
		return retVal;
	*fastLeaveEnable = (rtk_enable_t)regVal;
	
#else		
    if((retVal = rtl8307h_igmp_asicIgmpEnable_get(igmpEnable)) != SUCCESS)
        return retVal;
    if((retVal = rtl8307h_igmp_asicMldEnable_get(mldEnable)) != SUCCESS)
        return retVal;   
    if((retVal = rtl8307h_igmp_asicIpMulticastVlanLeaky_get(vlanLeakyEnable)) != SUCCESS)
        return retVal;
    if((retVal = rtl8307h_igmp_asicFastLeaveEnable_get(fastLeaveEnable)) != SUCCESS)
        return retVal;
#endif		

    return RT_ERR_OK;   
}
/* Function Name:
 *      rtk_igmp_cpuPortMsk_set
 * Description:
 *      set igmp cpu port mask  
 * Input:
 *      igmpCpuPortMsk  -  igmp cpu port mask, the value should be 0x0~0x7
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can set the cpu port mask when igmp packet trapped to cpu
 */
rtk_api_ret_t rtk_igmp_cpuPortMsk_set(rtk_portmask_t* igmpCpuPortMsk)
{
    ret_t retVal;
    if((retVal = rtl8307h_igmp_asicIgmpCpuPortMask_set(igmpCpuPortMsk->bits[0])) != SUCCESS)
        return retVal;

    return RT_ERR_OK;   
}
/* Function Name:
 *      rtk_igmp_cpuPortMsk_get
 * Description:
 *      get igmp cpu port mask     
 * Input:
 *      none
 * Output:
 *      igmpCpuPortMsk  -  igmp cpu port mask, the value should be 0x0~0x7
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can get the cpu port mask when igmp packet trapped to cpu
 */
rtk_api_ret_t rtk_igmp_cpuPortMsk_get(rtk_portmask_t* igmpCpuPortMsk)
{
    ret_t retVal;
    if((retVal = rtl8307h_igmp_asicIgmpCpuPortMask_get(&igmpCpuPortMsk->bits[0])) != SUCCESS)
        return retVal;

    return RT_ERR_OK;   
}
/* Function Name:
 *      rtk_igmp_pktTrap_set
 * Description:
 *      set the action on igmp/mld pkt
 * Input:
 *      igmpTrap  - the  action on igmp pkt
 *      mldTrap  -  the  action on mld pkt
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can set the action on igmp/mld pkt;
 *      Input should be:
 *      0x0       Forward
 *      0x1       Trap(to IGMP cpu port mask )
 *      0x2       Copy to cpu
 *      0x3       Drop
 */
rtk_api_ret_t rtk_igmp_pktTrap_set(rtk_trap_misc_action_t igmpTrap, rtk_trap_misc_action_t mldTrap)
{
    ret_t retVal;
    if((retVal = rtl8307h_igmp_asicIgmpTrap_set(igmpTrap)) != SUCCESS)
        return retVal;
    if((retVal = rtl8307h_igmp_asicMldTrap_set(mldTrap)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;   
}
/* Function Name:
 *      rtk_igmp_pktTrap_get
 * Description:
 *      get the action on igmp/mld pkt    
 * Input:
 *      none
 * Output:
 *      igmpTrap  - the  action on igmp pkt
 *      mldTrap  -  the  action on mld pkt
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can get the action on igmp/mld pkt
 *      Output should be:
 *      0x0       Forward
 *      0x1       Trap(to IGMP cpu port mask )
 *      0x2       Copy to cpu
 *      0x3       Drop
 */
rtk_api_ret_t rtk_igmp_pktTrap_get(rtk_trap_misc_action_t* igmpTrap, rtk_trap_misc_action_t* mldTrap)
{
    ret_t retVal;
    if((retVal = rtl8307h_igmp_asicIgmpTrap_get(igmpTrap)) != SUCCESS)
        return retVal;
    if((retVal = rtl8307h_igmp_asicMldTrap_get(mldTrap)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;   
}
/* Function Name:
 *      rtk_igmp_defPri_set
 * Description:
 *      Define Priority when IGMP/MLD packets trapped to CPU    
 * Input:
 *      defIgmpPri  -  define priority or not, the value should be 0x0 or 0x1
 *      igmpPri  -  priority defined, the value should be 0x0~0x7
 * Output:
 *     none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can define priority  when IGMP/MLD packets trapped to CPU
 */
rtk_api_ret_t rtk_igmp_defPri_set(rtk_enable_t defIgmpPri, rtk_pri_t igmpPri)
{
    ret_t retVal;
    if((retVal = rtl8307h_igmp_asicDefPri_set(defIgmpPri)) != SUCCESS)
        return retVal;
    if((retVal = rtl8307h_igmp_asicPri_set(igmpPri)) != SUCCESS)
        return retVal;

    return RT_ERR_OK; 
}
/* Function Name:
 *      rtk_igmp_defPri_set
 * Description:
 *      Check the priority when IGMP/MLD packets trapped to CPU    
 * Input:
 *      none
 * Output:
 *      defIgmpPri  -  define priority or not,  the value should be 0x0 or 0x1
 *      igmpPri  -  priority defined, the value should be 0x0~0x7
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can check the  priority  when IGMP/MLD packets trapped to CPU
 */
rtk_api_ret_t rtk_igmp_defPri_get(rtk_enable_t* defIgmpPri, rtk_pri_t* igmpPri)
{
    ret_t retVal;
#ifdef	EXT_LAB
	uint32 regVal;

	*defIgmpPri = DISABLED;
	if((retVal = rtl8307h_igmp_asicDefPri_get(&regVal)) != SUCCESS)
		return retVal;
	
	*defIgmpPri = (rtk_enable_t)regVal;
#else
    if((retVal = rtl8307h_igmp_asicDefPri_get(defIgmpPri)) != SUCCESS)
        return retVal;
#endif		
    if((retVal = rtl8307h_igmp_asicPri_get(igmpPri)) != SUCCESS)
        return retVal;

    return RT_ERR_OK; 
}

/* Function Name:
 *      rtk_stp_mstpState_set
 * Description:
 *       This API is used to set spanning tree port states in some instance
 * Input:
 *      msti                 - spanning tree instance id, from 0 to 14.
 *      portId              - port id, from 0 to 7.
 *      stp_state         -  port states, from STP_DISCARDING to STP_FORWARDING.
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID
 *     RT_ERR_MSTI
 *     RT_ERR_MSTP_STATE
 *     RT_ERR_OK                           
 * Note:
 *      None.
 */
rtk_api_ret_t rtk_stp_mstpState_set(rtk_stp_msti_id_t msti, rtk_port_t portId, rtk_stp_state_t stp_state)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(msti >= RTL8307H_MSTIMAX)
        return RT_ERR_MSTI;

    if(stp_state >= STP_END)
        return RT_ERR_MSTP_STATE;
  
    if ((retVal = reg_field_write(RTL8307H_UNIT, SPANNING_TREE_PORT_STATE_CONTROL0+msti, PORT0_STATE-portId, stp_state)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;

}

/* Function Name:
 *      rtk_stp_mstpState_get
 * Description:
 *       This API is used to get spanning tree port states in some instance
 * Input:
 *      msti                - spanning tree instance id, from 0 to 14.
 *      portId              - port id, from 0 to 7.
 * Output:
 *      pStp_state      - the point to port states.
 * Return:
 *     RT_ERR_PORT_ID
 *     RT_ERR_MSTI
 *     RT_ERR_NULL_POINTER
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_stp_mstpState_get(rtk_stp_msti_id_t msti, rtk_port_t portId, rtk_stp_state_t *pStp_state)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(msti >= RTL8307H_MSTIMAX)
        return RT_ERR_MSTI;

    if(NULL == pStp_state)
        return RT_ERR_NULL_POINTER;
    
#ifdef	EXT_LAB
	uint32 regVal;
	*pStp_state = STP_DISCARDING;
	if ((retVal = reg_field_read(RTL8307H_UNIT, SPANNING_TREE_PORT_STATE_CONTROL0+msti, PORT0_STATE-portId, &regVal)) != SUCCESS)
		return retVal;

	*pStp_state = (rtk_stp_state_t)regVal;
#else
    if ((retVal = reg_field_read(RTL8307H_UNIT, SPANNING_TREE_PORT_STATE_CONTROL0+msti, PORT0_STATE-portId, pStp_state)) != SUCCESS)
        return retVal;
#endif

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_stp_ability_set
 * Description:
 *       This API is used to enable or disable 8051 STP
 * Input:
 *      stpAbility         - 8051 stp ability, from DISABLED to ENABLED 
 * Output:
 *      none
 * Return:
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
rtk_api_ret_t rtk_stp_ability_set(rtk_enable_t stpAbility)
{
    uint32 fieldVal;
    int32 retVal;

    if(stpAbility>= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    fieldVal = (ENABLED == stpAbility) ? 0:1;

    if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_GLOBAL, RSTP_DIS, fieldVal)) != SUCCESS)
        return retVal;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_ability_get
 * Description:
 *       This API is used to get spanning tree ability
 * Input:
 *      none
 * Output:
 *      pStpAbility         -the pointer to  8051 stp ability 
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_stp_ability_get(rtk_enable_t* pStpAbility)
{
    uint32 fieldVal;
    int32 retVal;

    if(NULL == pStpAbility)
        return RT_ERR_NULL_POINTER;
        
    if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_GLOBAL, RSTP_DIS, &fieldVal)) != SUCCESS)
        return retVal;

    *pStpAbility = (1==fieldVal) ? DISABLE : ENABLE;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_type_set
 * Description:
 *       This API is used to switch 8051 STP protocol between STP and RSTP
 * Input:
 *      stpType         -STP type, from STP_TYPE_STP to STP_TYPE_RSTP 
 * Output:
 *      none
 * Return:
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
rtk_api_ret_t rtk_stp_type_set(rtk_stp_type_t stpType)
{
    int32 retVal;

    if(stpType >= STP_TYPE_END)
        return RT_ERR_INPUT;

    if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_GLOBAL, RSTP_TYPE, stpType)) != SUCCESS)
        return retVal;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_type_get
 * Description:
 *       This API is used to get spanning tree type
 * Input:
 *      none
 * Output:
 *      pStpType         -the pointer to  spanning tree type
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_stp_type_get(rtk_stp_type_t* pStpType)
{
    int32 retVal;

    if(NULL == pStpType)
        return RT_ERR_NULL_POINTER;

#ifdef	EXT_LAB
	uint32 regVal;

	*pStpType = STP_TYPE_STP;
	if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_GLOBAL, RSTP_TYPE, &regVal)) != SUCCESS)
		return retVal;
	*pStpType = (rtk_stp_type_t)regVal;
#else
    if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_GLOBAL, RSTP_TYPE, pStpType)) != SUCCESS)
        return retVal;
#endif		
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_bridgePriority_set
 * Description:
 *       This API is used to set switch priority in step of 4096(0x1000)
 * Input:
 *      stpBridgePriority         - the switch priority in step of 4096, from 0 to 15 
 * Output:
 *      none
 * Return:
 *     RT_ERR_STP_BRIDGE_PRIORITY 
 *     RT_ERR_OK                           
 * Note:
*      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
rtk_api_ret_t rtk_stp_bridgePriority_set(rtk_stp_bdg_pri_t stpBridgePriority)
{
    int32 retVal;

    if(stpBridgePriority > RTL8307H_STP_BRIDGE_PORT_PRIORITYMAX)
        return RT_ERR_STP_BRIDGE_PRIORITY;

    if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_GLOBAL, BRIDGE_PRIORITY, stpBridgePriority)) != SUCCESS)
        return retVal;
  
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_bridgePriority_get
 * Description:
 *       This API is used to get switch prioriy
 * Input:
 *      none
 * Output:
 *      pStpBridgePriority         -the pointer to  spanning tree switch prioriy
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_stp_bridgePriority_get(rtk_stp_bdg_pri_t* pStpBridgePriority)
{
    int32 retVal;

    if(NULL == pStpBridgePriority)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_GLOBAL, BRIDGE_PRIORITY, pStpBridgePriority)) != SUCCESS)
        return retVal;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_forwardDelay_set
 * Description:
 *       Set forward delay
 * Input:
 *      stpForwardDelay         - spanning tree forward delay
 * Output:
 *      none
 * Return:
 *     RT_ERR_STP_FORWARDDELAY 
 *     RT_ERR_OK                           
 * Note:
 *      This API can set STP forward delay from 4s to 30s
 */
rtk_api_ret_t rtk_stp_forwardDelay_set(rtk_stp_fowardDelay_t stpForwardDelay)
{
    int32 retVal;

    if (stpForwardDelay > RTL8307H_STP_FORWARD_DELAYMAX ||stpForwardDelay <RTL8307H_STP_FORWARD_DELAYMIN)
        return RT_ERR_STP_FORWARDDELAY;

    if((retVal = reg_field_write(RTL8307H_UNIT, RSTP_GLOBAL, FORWARD_DELAY, stpForwardDelay)) != SUCCESS )
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_forwardDelay_get
 * Description:
 *       Get forward delay
 * Input:
 *      none
 * Output:
 *      pStpForwardDelay         -the pointer to  spanning tree forward delay
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      This API can get  STP forward delay from 4s to 30s
 */
rtk_api_ret_t rtk_stp_forwardDelay_get(rtk_stp_fowardDelay_t* pStpForwardDelay)
{
    int32 retVal;

    if(NULL == pStpForwardDelay)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_GLOBAL, FORWARD_DELAY, pStpForwardDelay)) != SUCCESS)
        return retVal;

    if(*pStpForwardDelay > RTL8307H_STP_FORWARD_DELAYMAX  ||*pStpForwardDelay < RTL8307H_STP_FORWARD_DELAYMIN)
        return RT_ERR_FAILED;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_maxAge_set
 * Description:
 *       This API is used to set max age time, from 6s to 40s
 * Input:
 *      stpMaxAge         - spanning tree max age time, from 6 to 40
 * Output:
 *      none
 * Return:
 *     RT_ERR_STP_MAXAGE 
 *     RT_ERR_OK                           
 * Note:
*      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
rtk_api_ret_t rtk_stp_maxAge_set(rtk_stp_maxAdge_t stpMaxAge)
{
    int32 retVal;

    if (stpMaxAge > RTL8307H_STP_AGEMAX ||stpMaxAge <RTL8307H_STP_AGEMIN)
        return RT_ERR_STP_MAXAGE;

    if((retVal = reg_field_write(RTL8307H_UNIT, RSTP_GLOBAL, MAX_AGE, stpMaxAge)) != SUCCESS )
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_maxAge_get
 * Description:
 *       This API is used to get max age
 * Input:
 *      none
 * Output:
 *      pStpMaxAge         - the pointer to spanning tree max age
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_stp_maxAge_get(rtk_stp_maxAdge_t* pStpMaxAge)
{
    int32 retVal;

    if(NULL == pStpMaxAge)
        return RT_ERR_NULL_POINTER;
    
    if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_GLOBAL, MAX_AGE, pStpMaxAge)) != SUCCESS)
        return retVal;

    if(*pStpMaxAge > RTL8307H_STP_AGEMAX || *pStpMaxAge < RTL8307H_STP_AGEMIN)
        return RT_ERR_FAILED;
    
    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_stp_helloTime_set
 * Description:
 *       This API is used to set hello time from 1s to 2s
 * Input:
 *      stpHelloTime         - spanning tree hello time, from 1 to 2
 * Output:
 *      none
 * Return:
 *     RT_ERR_STP_HELLOTIME 
 *     RT_ERR_OK                           
 * Note:
*      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
rtk_api_ret_t rtk_stp_helloTime_set(rtk_stp_helloTime_t stpHelloTime)
{
    uint32 fieldVal;
    int32 retVal;

    if(stpHelloTime > RTL8307H_STP_HELLOTIMEMAX || stpHelloTime < RTL8307H_STP_HELLOTIMEMIN)
        return RT_ERR_STP_HELLOTIME;

    fieldVal = stpHelloTime-1;

    if((retVal = reg_field_write(RTL8307H_UNIT, RSTP_GLOBAL, HELLO_TIME, fieldVal)) != SUCCESS )
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *       rtk_stp_helloTime_get
 * Description:
 *       This API is used to get hello time
 * Input:
 *      none
 * Output:
 *      pStpHelloTime         - the pointer to spanning tree hello time
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_stp_helloTime_get(rtk_stp_helloTime_t* pStpHelloTime)
{
    uint32 fieldVal;
    int32 retVal;

    if(NULL == pStpHelloTime)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_GLOBAL, HELLO_TIME, &fieldVal)) != SUCCESS)
        return retVal;

    *pStpHelloTime = fieldVal+1;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_portPriority_set
 * Description:
 *      This API is used to set port priority in step of 16(0x10)
 * Input:
 *      portId                         - port id, from 0 to 7
 *      stpPortPriority         - spanning tree port priority, from 0 to 15
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_STP_PORT_PRIORITY 
 *     RT_ERR_OK                           
 * Note:
*      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
rtk_api_ret_t rtk_stp_portPriority_set(rtk_port_t portId, rtk_stp_port_pri_t stpPortPriority)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(stpPortPriority > RTL8307H_STP_BRIDGE_PORT_PRIORITYMAX)
        return RT_ERR_STP_PORT_PRIORITY;

    if(portId == 6)
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT3, P6_PORT_PRIORITY, stpPortPriority)) != SUCCESS)
            return retVal;
    }
    else if(portId%2 == 0)
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT0+portId/2, P0_PORT_PRIORITY+portId*5, stpPortPriority)) != SUCCESS)
            return retVal;
    }
    else
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT0+portId/2, P1_PORT_PRIORITY+(portId-1)*5, stpPortPriority)) != SUCCESS)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_portPriority_get
 * Description:
 *      This API is used to get port priority
 * Input:
 *      portId                            - pport id, from 0 to 7
 * Output:
 *      pStpPortPriority         - the pointer to spanning tree port prioriy
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_stp_portPriority_get(rtk_port_t portId, rtk_stp_port_pri_t* pStpPortPriority)
{
    int32 retVal;
    
    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;
    
    if(NULL == pStpPortPriority)
        return RT_ERR_NULL_POINTER;

    if(portId == 6)
    {
        if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT3, P6_PORT_PRIORITY, pStpPortPriority)) != SUCCESS)
            return retVal;
    }
    else if(portId%2 == 0)
    {
        if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT0+portId/2, P0_PORT_PRIORITY+portId*5, pStpPortPriority)) != SUCCESS)
            return retVal;
    }
    else
    {
        if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT0+portId/2, P1_PORT_PRIORITY+(portId-1)*5, pStpPortPriority)) != SUCCESS)
            return retVal;
    }
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_portPathCost_set
 * Description:
 *       This API is used to set port path cost
 * Input:
 *      portId                           - port id, from 0 to 7
 *      stpPortPathCost         - spanning tree port path cost, from 0 to 15
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_STP_PORT_PATH_COST 
 *     RT_ERR_OK                           
 * Note:
 *     port path cost value:
 *           0: stands for auto, STP will decide the path cost according to port's speed and duplex.
 *           1: stands for port speed 100Kbps, path cost=200000000
 *           2: stands for port speed 1Mbps, path cost=20000000
 *           3: stands for port speed 10Mbps Half duplex, path cost=2000000
 *           4: stands for port speed 10Mbps Full duplex, path cost=1999999
 *           5: stands for port speed 10Mbps Link aggregation, path cost=1000000
 *           6: stands for port speed 100Mbps Half duplex, path cost=200000
 *           7: stands for port speed 100Mbps Full duplex, path cost=199999
 *           8: stands for port speed 100Mbps Link aggregation, path cost=100000
 *           9: stands for port speed 1Gbps Full duplex, path cost=20000
 *           10: stands for port speed 1Gbps, path cost=19999
 *           11: stands for port speed 1Gbps Link aggregation, path cost=10000
 *           12: stands for port speed 10Gbps, path cost=2000
 *           13: stands for port speed 100Gbps, path cost=200
 *           14: stands for port speed 1Tbps, path cost=20
 *           15: stands for port speed 10Tbps, path cost=2
 *
 *      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
rtk_api_ret_t rtk_stp_portPathCost_set(rtk_port_t portId, rtk_stp_portPathCost_t stpPortPathCost)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(stpPortPathCost > RTL8307H_STP_PATHCOSTMAX)
        return RT_ERR_STP_PORT_PATH_COST;

    if(portId == 6)
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT3, P6_PORT_PATH_COST, stpPortPathCost)) != SUCCESS)
            return retVal;
    }
    else if(portId%2 == 0)
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT0+portId/2, P0_PORT_PATH_COST+portId*5, stpPortPathCost)) != SUCCESS)
            return retVal;
    }
    else
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT0+portId/2, P1_PORT_PATH_COST+(portId-1)*5, stpPortPathCost)) != SUCCESS)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_portPathCost_get
 * Description:
 *       This API is used to get port path cost
 * Input:
 *      portId                               - port id, from 0 to 7
 * Output:
 *      pStpPortPathCost         - the pointer to spanning tree port path cost
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_stp_portPathCost_get(rtk_port_t portId, rtk_stp_portPathCost_t* pStpPortPathCost)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pStpPortPathCost)
        return RT_ERR_NULL_POINTER;

    if(portId == 6)
    {
        if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT3, P6_PORT_PATH_COST, pStpPortPathCost)) != SUCCESS)
            return retVal;
    }
    else if(portId%2 == 0)
    {
        if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT0+portId/2, P0_PORT_PATH_COST+portId*5, pStpPortPathCost)) != SUCCESS)
            return retVal;
    }
    else
    {
        if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT0+portId/2, P1_PORT_PATH_COST+(portId-1)*5, pStpPortPathCost)) != SUCCESS)
            return retVal;
    }
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_portEdge_set
 * Description:
 *       This API is used to set port edge attribution
 * Input:
 *      portId                    - port id, from 0 to 7
 *      stpPortEdge         - spanning tree port edge attribution, from STP_EDGE_AUTO to STP_EDGE_ADMIN
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
rtk_api_ret_t rtk_stp_portEdge_set(rtk_port_t portId, rtk_stp_portEdge_t  stpPortEdge)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(stpPortEdge >= STP_EDGE_END)
        return RT_ERR_INPUT;

    if(portId == 6)
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT3, P6_EDGE_PORT, stpPortEdge)) != SUCCESS)
            return retVal;
    }
    else if(portId%2 == 0)
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT0+portId/2, P0_EDGE_PORT+portId*5, stpPortEdge)) != SUCCESS)
            return retVal;
    }
    else
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT0+portId/2, P1_EDGE_PORT+(portId-1)*5, stpPortEdge)) != SUCCESS)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_portEdge_get
 * Description:
 *       This API is used to get port edge attribution
 * Input:
 *      portId                          - port id, from 0 to 7
 * Output:
 *      pStpPortEdge             - the pointer to spanning tree port edge attribution
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_stp_portEdge_get(rtk_port_t portId, rtk_stp_portEdge_t*pStpPortEdge)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pStpPortEdge)
        return RT_ERR_NULL_POINTER;

#ifdef	EXT_LAB
	uint32 regVal;

	if(portId == 6)
	{
		*pStpPortEdge = STP_EDGE_AUTO;
		if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT3, P6_EDGE_PORT, &regVal)) != SUCCESS)
			return retVal;
		*pStpPortEdge = (rtk_stp_portEdge_t)regVal;
	}
	else if(portId%2 == 0)
	{
		*pStpPortEdge = STP_EDGE_AUTO;
		if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT0+portId/2, P0_EDGE_PORT+portId*5, &regVal)) != SUCCESS)
			return retVal;
		*pStpPortEdge = (rtk_stp_portEdge_t)regVal;
	}
	else
	{
		*pStpPortEdge = STP_EDGE_AUTO;
		if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT0+portId/2, P1_EDGE_PORT+(portId-1)*5, &regVal)) != SUCCESS)
			return retVal;
		*pStpPortEdge = (rtk_stp_portEdge_t)regVal;
	}
#else
    if(portId == 6)
    {
        if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT3, P6_EDGE_PORT, pStpPortEdge)) != SUCCESS)
            return retVal;
    }
    else if(portId%2 == 0)
    {
        if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT0+portId/2, P0_EDGE_PORT+portId*5, pStpPortEdge)) != SUCCESS)
            return retVal;
    }
    else
    {
        if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT0+portId/2, P1_EDGE_PORT+(portId-1)*5, pStpPortEdge)) != SUCCESS)
            return retVal;
    }
#endif			
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_portP2P_set
 * Description:
 *       This API is used to set port p2p attribution
 * Input:
 *      portId                       - port id, from 0 to 7
 *      stpPortP2P              - spanning tree port P2P attribution, from STP_P2P_AUTO to STP_P2P_FALSE
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
rtk_api_ret_t rtk_stp_portP2P_set(rtk_port_t portId, rtk_stp_portP2P_t stpPortP2P)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(stpPortP2P >= STP_P2P_END)
        return RT_ERR_INPUT;
 
    if(portId == 6)
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT3, P6_P2P_PROT, stpPortP2P)) != SUCCESS)
            return retVal;
    }
    else if(portId%2 == 0)
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT0+portId/2, P0_P2P_PROT+portId*5, stpPortP2P)) != SUCCESS)
            return retVal;
    }
    else
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT0+portId/2, P1_P2P_PROT+(portId-1)*5, stpPortP2P)) != SUCCESS)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_portP2P_get
 * Description:
 *       This API is used to get port p2p attribution
 * Input:
 *      portId                        - port id, from 0 to 7
 * Output:
 *      pStpPortP2P             - the pointer to spanning tree port p2p attribution
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_stp_portP2P_get(rtk_port_t portId, rtk_stp_portP2P_t* pStpPortP2P)
{
    int32 retVal;
    
    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;
    
    if(NULL == pStpPortP2P)
        return RT_ERR_NULL_POINTER;

#ifdef	EXT_LAB
	uint32 regVal;

	if(portId == 6)
	{
		*pStpPortP2P = STP_P2P_AUTO;
		if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT3, P6_P2P_PROT, &regVal)) != SUCCESS)
			return retVal;
		*pStpPortP2P = (rtk_stp_portP2P_t)regVal;
	}
	else if(portId%2 == 0)
	{
		*pStpPortP2P = STP_P2P_AUTO;
		if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT0+portId/2, P0_P2P_PROT+portId*5, &regVal)) != SUCCESS)
			return retVal;
		*pStpPortP2P = (rtk_stp_portP2P_t)regVal;
	}
	else
	{
		*pStpPortP2P = STP_P2P_AUTO;
		if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT0+portId/2, P1_P2P_PROT+(portId-1)*5, &regVal)) != SUCCESS)
			return retVal;
		*pStpPortP2P = (rtk_stp_portP2P_t)regVal;
	}
#else
    if(portId == 6)
    {
        if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT3, P6_P2P_PROT, pStpPortP2P)) != SUCCESS)
            return retVal;
    }
    else if(portId%2 == 0)
    {
        if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT0+portId/2, P0_P2P_PROT+portId*5, pStpPortP2P)) != SUCCESS)
            return retVal;
    }
    else
    {
        if ((retVal = reg_field_read(RTL8307H_UNIT, RSTP_PORT0+portId/2, P1_P2P_PROT+(portId-1)*5, pStpPortP2P)) != SUCCESS)
            return retVal;
    }
#endif

    if(*pStpPortP2P>=STP_P2P_END)
        return RT_ERR_FAILED;
 
    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_stp_portMigrationCheck_set
 * Description:
 *       This API is used to trigger port migration check
 * Input:
 *      portId                 - port id, from 0 to 7
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_OK                           
 * Note:
 *      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 *      When migration check is done, the related register bit will be cleared to 0 automatically.   
 */
rtk_api_ret_t rtk_stp_portMigrationCheck_set(rtk_port_t portId)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(portId == 6)
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT3, P6_MIGRATION_CHECK, 1)) != SUCCESS)
            return retVal;
    }
    else if(portId%2 == 0)
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT0+portId/2, P0_MIGRATION_CHECK+portId*5, 1)) != SUCCESS)
            return retVal;
    }
    else
    {
        if ((retVal = reg_field_write(RTL8307H_UNIT, RSTP_PORT0+portId/2, P1_MIGRATION_CHECK+(portId-1)*5, 1)) != SUCCESS)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stp_parameterChangeIntTrigger
 * Description:
 *       This API is used to trigger parameter change interrupt to announce 8051 to reconfigure its STP
 * Input:
 *      none
 * Output:
 *      none
 * Return:
 *     RT_ERR_OK                    
 *     RT_ERR_STP_PARAMETER_CHANGE_INT_DONE         
 *     RT_ERR_STP_PARAMETER_CHANGE_INT_CLEAR     
 * Note:
 *     RT_ERR_STP_PARAMETER_CHANGE_INT_CLEAR  means time out of interrupt clear.
 *     RT_ERR_STP_PARAMETER_CHANGE_INT_DONE  means time out of 8051 reconfiguration STP
 */
rtk_api_ret_t rtk_stp_parameterChangeIntTrigger(void )
{
    int32 retVal;
    uint32 fieldVal;

    uint32 int_Polling_Time = 1000;
    uint32 C8051_event_pollingTime = 10000;

    rtl8307h_reg_lock();

    if ((retVal = reg_field_write(RTL8307H_UNIT, SWITCH_INTERRUPT_GLOBAL_SOURCE_STATUS, RSTPPAMTERCGE_IP, 1)) != SUCCESS)
    {
        rtl8307h_reg_unlock();
        return retVal;   
    }

    rtl8307h_reg_unlock();

    while(int_Polling_Time)
    {
        int_Polling_Time--;
    
        if((retVal = reg_field_read(RTL8307H_UNIT, SWITCH_INTERRUPT_GLOBAL_SOURCE_STATUS, RSTPPAMTERCGE_IP, &fieldVal))!=SUCCESS)          
            return retVal;


        if(fieldVal == 0)
        {
            while(C8051_event_pollingTime)
            {
                C8051_event_pollingTime--;
            
                if((retVal = reg_field_read(RTL8307H_UNIT, SWITCH_INTERRUPT_MODE_8051EVENT, CPU8051_ENVENT_NUM, &fieldVal))!=SUCCESS)
                    return retVal;

                if(fieldVal == STP_PARAM_CHANGE_FINISH)
                {
               
                    rtl8307h_reg_lock();

                    if((retVal = reg_field_write(RTL8307H_UNIT, SWITCH_INTERRUPT_MODE_8051EVENT, CPU8051_ENVENT_NUM, 0))!=SUCCESS)
                        return retVal;

                    rtl8307h_reg_unlock();
                
                    return RT_ERR_OK;
                }
                
                int_Polling_Time--;
            }

            return RT_ERR_STP_PARAMETER_CHANGE_INT_DONE;
        }
        
        int_Polling_Time--;
    }

    return RT_ERR_STP_PARAMETER_CHANGE_INT_CLEAR;
}

/* Function Name:
 *      rtk_qos_portPri_set
 * Description:
 *       This API is used to set port-based priority
 * Input:
 *      portId                                -port id, from 0 to 7
 *      int_pri                              -port-based priority, from 0 to 7
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_portPri_set(rtk_port_t portId, rtk_pri_t int_pri) 
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(int_pri> RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_PRIORITY_CONTROL+portId, PORTPRIORITY, int_pri)) != SUCCESS)
        return retVal;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_portPri_get
 * Description:
 *       This API is used to get port-based priority
 * Input:
 *      portId                                  -port id, from 0 to 7
 * Output:
 *      pInt_pri                              -pointer to port base priority
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_portPri_get(rtk_port_t portId, rtk_pri_t *pInt_pri) 
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pInt_pri)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_CONTROL+portId, PORTPRIORITY, pInt_pri)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_priSel_set
 * Description:
 *       This API is used to configure the priority extraction arbitration table
 * Input:
 *      pPriDec                 -the pointer to priority weight configuration
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_SOURCE 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      pPriDec->group stands for the abitration table index, from 0 to 1.
 *      pPriDec->port_pri stands for the weight of port-based priority, from 0 to 7.
 *      pPriDec->dot1q_pri  stands for the weight of  Dot1q port-base priority, from 0 to 7.
 *      pPriDec->dscp_pri stands for the weight of  Dscp priority, from 0 to 7.
 *      pPriDec->cvlan_pri stands for the weight of  Cvlan priority, from 0 to 7.
 *      pPriDec->svlan_pri stands for the weight of  Svlan priority, from 0 to 7
 */
rtk_api_ret_t rtk_qos_priSel_set(rtk_priority_select_t *pPriDec)
{
    int32 retVal;
    
    if  (
            pPriDec->port_pri > RTL8307H_PRIMAX || 
            pPriDec->dot1q_pri > RTL8307H_PRIMAX || 
            pPriDec->dscp_pri > RTL8307H_PRIMAX ||
            pPriDec->cvlan_pri >  RTL8307H_PRIMAX || 
            pPriDec->svlan_pri > RTL8307H_PRIMAX
        )
        return RT_ERR_QOS_SEL_PRI_SOURCE;

    if (pPriDec->group > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if ((retVal = reg_field_write(RTL8307H_UNIT, PRIORITY_ARBITRATION_CONTROL0+pPriDec->group, OTAG_PRIO, pPriDec->svlan_pri)) != SUCCESS)
        return retVal;
    if ((retVal = reg_field_write(RTL8307H_UNIT, PRIORITY_ARBITRATION_CONTROL0+pPriDec->group, ITAG_PRIO, pPriDec->cvlan_pri)) != SUCCESS)
        return retVal;
    if ((retVal = reg_field_write(RTL8307H_UNIT, PRIORITY_ARBITRATION_CONTROL0+pPriDec->group, DSCP_PRIO, pPriDec->dscp_pri)) != SUCCESS)
        return retVal;
    if ((retVal = reg_field_write(RTL8307H_UNIT, PRIORITY_ARBITRATION_CONTROL0+pPriDec->group, DOT1QBP_PRIO, pPriDec->dot1q_pri)) != SUCCESS)
        return retVal;
    if ((retVal = reg_field_write(RTL8307H_UNIT, PRIORITY_ARBITRATION_CONTROL0+pPriDec->group, PBP_PRIO, pPriDec->port_pri)) != SUCCESS)
        return retVal;    

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_priSel_get
 * Description:
 *       This API is used to get configuration of  priority extraction arbitration table
 * Input:
 *      pPriDec                 -the pointer to priority weight configuration, set pPriDec->group to choose the group which you want to get 
 * Output:
 *      pPriDec                 -the pointer to priority weight configuration
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      pPriDec->group stands for the abitration table index, from 0 to 1.
 *      pPriDec->port_pri stands for the weight of port-based priority, from 0 to 7.
 *      pPriDec->dot1q_pri  stands for the weight of  Dot1q port-base priority, from 0 to 7.
 *      pPriDec->dscp_pri stands for the weight of  Dscp priority, from 0 to 7.
 *      pPriDec->cvlan_pri stands for the weight of  Cvlan priority, from 0 to 7.
 *      pPriDec->svlan_pri stands for the weight of  Svlan priority, from 0 to 7
 */
rtk_api_ret_t rtk_qos_priSel_get(rtk_priority_select_t *pPriDec)
{
    int32 retVal;

    if (pPriDec == NULL)
        return RT_ERR_NULL_POINTER;

    if (pPriDec->group > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if ((retVal = reg_field_read(RTL8307H_UNIT, PRIORITY_ARBITRATION_CONTROL0+pPriDec->group, OTAG_PRIO, &pPriDec->svlan_pri)) != SUCCESS)
        return retVal;
    if ((retVal = reg_field_read(RTL8307H_UNIT, PRIORITY_ARBITRATION_CONTROL0+pPriDec->group, ITAG_PRIO, &pPriDec->cvlan_pri)) != SUCCESS)
        return retVal;
    if ((retVal = reg_field_read(RTL8307H_UNIT, PRIORITY_ARBITRATION_CONTROL0+pPriDec->group, DSCP_PRIO, &pPriDec->dscp_pri)) != SUCCESS)
        return retVal;
    if ((retVal = reg_field_read(RTL8307H_UNIT, PRIORITY_ARBITRATION_CONTROL0+pPriDec->group, DOT1QBP_PRIO, &pPriDec->dot1q_pri)) != SUCCESS)
        return retVal;
    if ((retVal = reg_field_read(RTL8307H_UNIT, PRIORITY_ARBITRATION_CONTROL0+pPriDec->group, PBP_PRIO, &pPriDec->port_pri)) != SUCCESS)
        return retVal;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_PriWeightTable_set
 * Description:
 *       This API is used for port to select priority extraction arbitration table
 * Input:
 *      portId                                      -port id, from 0 to 7
 *      index                                       -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_priWeightTable_set(rtk_port_t portId, rtk_index_t index)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_PRIORITY_CONTROL+portId, PRIWGTTBLIDX, index)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_PriWeightTable_get
 * Description:
 *      This API is used to get  port  priority extraction arbitration table index
 * Input:
 *      portId                                   -port id, from 0 to 7
 * Output:
 *      pIndex                                  -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_priWeightTable_get(rtk_port_t portId, rtk_index_t* pIndex)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pIndex)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_CONTROL+portId, PRIWGTTBLIDX, pIndex)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_cvlanPriRemap_set
 * Description:
 *       This API is used to configure cvlan priority mapping table
 * Input:
 *      index                                        - mapping table index, from 0 to 1
 *      innerPriority                           -original priority in ctag, from 0 to 7
 *      mappingPrioirty                      -the mapping priority, from 0 to 7
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_cvlanPriRemap_set(rtk_index_t index, rtk_pri_t innerPriority, rtk_pri_t mappingPrioirty)
{
    int32 retVal;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;
    
    if(innerPriority> RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY; 

    if(mappingPrioirty> RTL8307H_PRIMAX)
        return RT_ERR_INPUT; 

    if((retVal=reg_field_write(RTL8307H_UNIT, INNER_TAG_PRIORITY_MAP_CONTROL0+index, PRIORITYMAPTO0-innerPriority, mappingPrioirty)) != SUCCESS)
        return retVal;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_cvlanPriRemap_get
 * Description:
 *       This API is used to get cvlan priority mapping table configuration
 * Input:
 *      index                                        - mapping table index, from 0 to 1
 *      innerPriority                           -original priority in ctag, from 0 to 7
 * Output:
 *      pMappingPrioirty                      -the pointer to  mapping priority
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_cvlanPriRemap_get(rtk_index_t index, rtk_pri_t innerPriority, rtk_pri_t* pMappingPrioirty)
{
    int32 retVal;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;
    
    if(innerPriority> RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY; 

    if(NULL == pMappingPrioirty)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, INNER_TAG_PRIORITY_MAP_CONTROL0+index, PRIORITYMAPTO0-innerPriority, pMappingPrioirty)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_cvlanRemapTable_set
 * Description:
 *       This API is used for port to select cvlan priority mapping table
 * Input:
 *      portId                                       -port id, from 0 to 7
 *      index                                        -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_cvlanRemapTable_set(rtk_port_t portId, rtk_index_t index)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_PRIORITY_CONTROL+portId, IPRIDPTBLIDX, index)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_cvlanRemapTable_get
 * Description:
 *      This API is used to get  port  cvlan mapping table index
 * Input:
 *      portId                                         -port id, from 0 to 7
 * Output:
 *      pIndex                                       -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_cvlanRemapTable_get(rtk_port_t portId, rtk_index_t* pIndex)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pIndex)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_CONTROL+portId, IPRIDPTBLIDX, pIndex)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_svlanPriRemap_set
 * Description:
 *       This API is used to configure svlan priority mapping table
 * Input:
 *      index                                        - mapping table index, from 0 to 1
 *      outerPriority                           -original priority in stag, from 0 to 7
 *      dei                                             -original dei in stag, from 0 to 1
 *      mappingPrioirty                      -the mapping priority, from 0 to 7
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_QOS_1P_DEI 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_svlanPriRemap_set(rtk_index_t index, rtk_pri_t outerPriority, rtk_dei_t  dei, rtk_pri_t mappingPrioirty)
{
    int32 retVal;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;
    
    if(outerPriority> RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY; 

    if(dei > RTL8307H_DEIMAX)
        return RT_ERR_QOS_1P_DEI; 

    if(mappingPrioirty> RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY; 

    if((retVal=reg_field_write(RTL8307H_UNIT, OUTER_TAG_PRIORITY_MAP_CONTROL0+index*2+dei, PRIORITYMAPTO0-outerPriority, mappingPrioirty)) != SUCCESS)
        return retVal;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_svlanPriRemap_get
 * Description:
 *       This API is used to get svlan priority mapping table configuration
 * Input:
 *      index                                        -mapping table index, from 0 to 1
 *      outerPriority                           -original priority in stag, from 0 to 7
 *      dei                                             -original dei in stag, from 0 to 1
 * Output:
 *      pMappingPrioirty                      -the pointer to  mapping priority
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_QOS_1P_DEI 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_svlanPriRemap_get(rtk_index_t index, rtk_pri_t outerPriority, rtk_dei_t dei, rtk_pri_t* pMappingPrioirty)
{
    int32 retVal;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;
    
    if(outerPriority> RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY; 

    if(dei > RTL8307H_DEIMAX)
        return RT_ERR_QOS_1P_DEI; 

    if(NULL == pMappingPrioirty)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, OUTER_TAG_PRIORITY_MAP_CONTROL0+index*2+dei, PRIORITYMAPTO0-outerPriority, pMappingPrioirty)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_dscpPriRemap_set
 * Description:
 *       This API is used to configure dscp priority mapping table
 * Input:
 *      index                                        - mapping table index, from 0 to 1
 *      dscp                                            -dscp value in IP header, from 0 to 63
 *      mappingPrioirty                      -the mapping priority, from 0 to 7
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_DSCP_VALUE 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_dscpPriRemap_set(rtk_index_t index, rtk_dscp_t dscp, rtk_pri_t mappingPrioirty)
{
    int32 retVal;

    if(index >RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;
    
    if(dscp> RTL8307H_DSCPMAX)
        return RT_ERR_QOS_DSCP_VALUE; 

    if(mappingPrioirty> RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY; 

    if((retVal=reg_field_write(RTL8307H_UNIT, DSCP_PRIORITY_MAP_CONTROL0+index*8+dscp/8, PRIORITYMAPTO0+(dscp/8)*8-dscp%8, mappingPrioirty)) != SUCCESS)
        return retVal;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_dscpPriRemap_get
 * Description:
 *       This API is used to get dscp priority mapping table configuration
 * Input:
 *      index                                        - mapping table index, from 0 to 1
 *      dscp                                            -dscp value in IP header, from 0 to 63
 * Output:
 *      pMappingPrioirty                      -the pionter  to mapping prioirty
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_DSCP_VALUE 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                             
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_dscpPriRemap_get(rtk_index_t index, rtk_dscp_t dscp, rtk_pri_t* pMappingPrioirty)
{
    int32 retVal;

    if(index >RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;
    
    if(dscp> RTL8307H_DSCPMAX)
        return RT_ERR_QOS_DSCP_VALUE; 

    if(NULL == pMappingPrioirty)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, DSCP_PRIORITY_MAP_CONTROL0+index*8+dscp/8, PRIORITYMAPTO0+(dscp/8)*8-dscp%8, pMappingPrioirty)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 



/* Function Name:
 *      rtk_qos_svlanRemapTable_set
 * Description:
 *       This API is used for port to select cvlan priority mapping table
 * Input:
 *      portId                                       -port id, from 0 to 7
 *      index                                        -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_svlanRemapTable_set(rtk_port_t portId, rtk_index_t index)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_PRIORITY_CONTROL+portId, OPRIDPTBLIDX, index)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_svlanRemapTable_get
 * Description:
 *      This API is used to get  port  svlan mapping table index
 * Input:
 *      portId                                       -port id, from 0 to 7
 * Output:
 *      pIndex                                      -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_svlanRemapTable_get(rtk_port_t portId, rtk_index_t* pIndex)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pIndex)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_CONTROL+portId, OPRIDPTBLIDX, pIndex)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_dscpRemapTable_set
 * Description:
 *       This API is used for port to select dscp priority mapping table
 * Input:
 *      portId                                       -port id, from 0 to 7
 *      index                                        -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_dscpRemapTable_set(rtk_port_t portId, rtk_index_t index)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_PRIORITY_CONTROL+portId, DSCPPRIDPTBLIDX, index)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_dscpRemapTable_get
 * Description:
 *      This API is used to get  port  dscp mapping table index
 * Input:
 *      portId                                       -port id, from 0 to 7
 * Output:
 *      pIndex                                      -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_dscpRemapTable_get(rtk_port_t portId, rtk_index_t* pIndex)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pIndex)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_CONTROL+portId, DSCPPRIDPTBLIDX, pIndex)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 



 

/* Function Name:
 *      rtk_qos_cvlanRemarkEnable_set
 * Description:
 *       This API is used to set egress port ctag priority remark ablity
 * Input:
 *      portId                                      -port id, from 0 to 7
 *      remarkAbility                       -ctag priority remark ability, from DISABLED to ENABLED
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_cvlanRemarkEnable_set(rtk_port_t portId , rtk_enable_t remarkAbility)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(remarkAbility>= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, IPRIRMK, remarkAbility)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_cvlanRemarkEnable_get
 * Description:
 *       This API is used to get egress port ctag priority remark ablity
 * Input:
 *      portId                                      -port id, from 0 to 7
 * Output:
 *      pRemarkAbility                     -the pointer to remark ability
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_cvlanRemarkEnable_get(rtk_port_t portId , rtk_enable_t* pRemarkAbility)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pRemarkAbility)
        return RT_ERR_NULL_POINTER;

#ifdef	EXT_LAB
	uint32 regVal;

	*pRemarkAbility = DISABLED;
	if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, IPRIRMK, &regVal)) != SUCCESS)
		return retVal;
	*pRemarkAbility = (rtk_enable_t)regVal;
#else
    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, IPRIRMK, pRemarkAbility)) != SUCCESS)
        return retVal;
#endif		

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_svlanRemarkEnable_set
 * Description:
 *       This API is used to set egress port stag priority remark ablity
 * Input:
 *      portId                                      -port id, from 0 to 7
 *      remarkAbility                       -stag priority remark ability, from DISABLED to ENABLED
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_svlanRemarkEnable_set(rtk_port_t portId , rtk_enable_t remarkAbility)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(remarkAbility >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, OPRIRMK, remarkAbility)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_svlanRemarkEnable_get
 * Description:
 *       This API is used to get egress port stag priority remark ablity
 * Input:
 *      portId                                      -port id, from 0 to 7
 * Output:
 *      pRemarkAbility                     -the pointer to remark ability
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_svlanRemarkEnable_get(rtk_port_t portId , rtk_enable_t* pRemarkAbility)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pRemarkAbility)
        return RT_ERR_NULL_POINTER;

#ifdef	EXT_LAB
	uint32 regVal;

	*pRemarkAbility = DISABLED;
	if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, OPRIRMK, &regVal)) != SUCCESS)
		return retVal;
	*pRemarkAbility = (rtk_enable_t)regVal;
	
#else
    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, OPRIRMK, pRemarkAbility)) != SUCCESS)
        return retVal;
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_dscpRemarkEnable_set
 * Description:
 *       This API is used to set egress port dscp remark ablity
 * Input:
 *      portId                                      -port id, from 0 to 7
 *      remarkAbility                       -dscp remark ability, from DISABLED to ENABLED
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_dscpRemarkEnable_set(rtk_port_t portId, rtk_enable_t remarkAbility)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(remarkAbility >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, DSCPRMK, remarkAbility)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_dscpRemarkEnable_get
 * Description:
 *       This API is used to get egress port dscp remark ablity
 * Input:
 *      portId                                      -port id, from 0 to 7
 * Output:
 *      pRemarkAbility                     -the pointer to remark ability
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_dscpRemarkEnable_get(rtk_port_t portId, rtk_enable_t *pRemarkAbility)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pRemarkAbility)
        return RT_ERR_NULL_POINTER;

#ifdef	EXT_LAB
	uint32 regVal;

	*pRemarkAbility = DISABLED;
	if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, DSCPRMK, &regVal)) != SUCCESS)
		return retVal;
	*pRemarkAbility = (rtk_enable_t)regVal;
#else
   if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, DSCPRMK, pRemarkAbility)) != SUCCESS)
        return retVal;
#endif   	

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_cvlanRemark_set
 * Description:
 *       This API is used to configure egress port ctag priority remark table
 * Input:
 *      index                                       -table index, from 0 to 1
 *      originPriority                        - internal priority, from 0 to 7
 *      remarkPriority                      - new priority which will be remarked to ctag priority, from 0 to 7
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_cvlanRemark_set(rtk_index_t index, rtk_pri_t originPriority, rtk_pri_t remarkPriority)
{
    int32 retVal;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if(originPriority > RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY;

    if(remarkPriority > RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY;

    if((retVal=reg_field_write(RTL8307H_UNIT, INNER_TAG_PRIORITY_REMARKING_TABLE_CONTROL0+index, PRITOPRI0-originPriority, remarkPriority)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_cvlanRemark_get
 * Description:
 *       This API is used to get egress port ctag priority remark table configuration
 * Input:
 *      index                                       -table index, from 0 to 1
 *      originPriority                        - internal priority, from 0 to 7
 * Output:
 *      pRemarkPriority                      -the pointer to remark priority
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_cvlanRemark_get(rtk_index_t index, rtk_pri_t originPriority, rtk_pri_t* pRemarkPriority)
{
    int32 retVal;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if(originPriority > RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY;

    if(NULL == pRemarkPriority)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, INNER_TAG_PRIORITY_REMARKING_TABLE_CONTROL0+index, PRITOPRI0-originPriority, pRemarkPriority)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_svlanRemark_set
 * Description:
 *       This API is used to configure egress port stag priority&dei remark table
 * Input:
 *      index                                       -table index, from 0 to 1
 *      originPriority                        - internal priority, from 0 to 7
 *      remarkPriority                      - new priority which will be remarked to stag priority, from 0 to 7
 *      remarkDei                              - new dei which will be remarked to stag dei, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_QOS_1P_DEI 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_svlanRemark_set(rtk_index_t index, rtk_pri_t originPriority,  rtk_pri_t remarkPriority, rtk_dei_t remarkDei)
{
    int32 retVal;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if(originPriority > RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY;

    if(remarkPriority > RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY;

    if(remarkDei>RTL8307H_DEIMAX)
        return RT_ERR_QOS_1P_DEI;
    
    if((retVal=reg_field_write(RTL8307H_UNIT, OUTER_TAG_PRIORITY_AND_DEI_REMARKING_TABLE_CONTROL0+index, PRITOPRI0-originPriority, remarkPriority)) != SUCCESS)
        return retVal;

    if((retVal=reg_field_write(RTL8307H_UNIT, OUTER_TAG_PRIORITY_AND_DEI_REMARKING_TABLE_CONTROL0+index, DEITOPRI0-originPriority, remarkDei)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_svlanRemark_get
 * Description:
 *       This API is used to get egress port stag priority&dei remark table configuration
 * Input:
 *      index                                       -table index, from 0 to 1
 *      originPriority                        - internal priority, from 0 to 7
 * Output:
 *      pRemarkPriority                      -the pointer to remark priority
  *      pRemarkDei                             -the pointer to remark dei
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_svlanRemark_get(rtk_index_t index, rtk_pri_t originPriority, rtk_pri_t* pRemarkPriority, rtk_dei_t* pRemarkDei)
{
    int32 retVal;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if(originPriority > RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY;

    if(NULL == pRemarkPriority || NULL ==pRemarkDei)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, OUTER_TAG_PRIORITY_AND_DEI_REMARKING_TABLE_CONTROL0+index, PRITOPRI0-originPriority, pRemarkPriority)) != SUCCESS)
        return retVal;
    
    if((retVal=reg_field_read(RTL8307H_UNIT, OUTER_TAG_PRIORITY_AND_DEI_REMARKING_TABLE_CONTROL0+index, DEITOPRI0-originPriority, pRemarkDei)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_dscpRemark_set
 * Description:
 *       This API is used to configure egress port dscp remark table
 * Input:
 *      index                                       -table index, from 0 to 1
 *      originPriority                        - internal priority, from 0 to 7
 *      remarkDscp                             -remark dscp, from 0 to 63
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_QOS_DSCP_VALUE 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_dscpRemark_set(rtk_index_t index, rtk_pri_t originPriority, rtk_dscp_t remarkDscp)
{
    int32 retVal;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if(originPriority > RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY;

    if(remarkDscp > RTL8307H_DSCPMAX)
        return RT_ERR_QOS_DSCP_VALUE;

    if((retVal=reg_field_write(RTL8307H_UNIT, DSCP_REMARKING_TABLE_CONTROL0+index*2+(originPriority/5), DSCPTOPRI0+(originPriority/5)*3-(originPriority%5), remarkDscp)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_dscpRemark_get
 * Description:
 *       This API is used to get egress port dscp remark table configuration
 * Input:
 *      index                                       -table index, from 0 to 1
 *      originPriority                        - internal priority, from 0 to 7
 * Output:
 *      pRemarkDscp                         -the pointer to remark dscp
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      noen
 */
rtk_api_ret_t rtk_qos_dscpRemark_get(rtk_index_t index, rtk_pri_t originPriority, rtk_dscp_t* pRemarkDscp)
{
    int32 retVal;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if(originPriority > RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY;

    if(NULL == pRemarkDscp)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, DSCP_REMARKING_TABLE_CONTROL0+index*2+(originPriority/5), DSCPTOPRI0+(originPriority/5)*3-(originPriority%5), pRemarkDscp)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_cvlanRemarkTable_set
 * Description:
 *       This API is used for port to select cvlan priority remarking table
 * Input:
 *      portId                                 -port id, from 0 to 7
 *      index                                  -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_cvlanRemarkTable_set(rtk_port_t portId, rtk_index_t index)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, IPRIRMKTBLIDX, index)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_cvlanRemarkTable_get
 * Description:
 *       This API is used  to get port cvlan priority remarking table index
 * Input:
 *      portId                                 -port id, from 0 to 7
 * Output:
 *      pIndex                                -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_cvlanRemarkTable_get(rtk_port_t portId, rtk_index_t* pIndex)
{
#ifndef	EXT_LAB
    uint32 fieldVal;
#endif
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pIndex)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, IPRIRMKTBLIDX, pIndex)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_svlanRemarkTable_set
 * Description:
 *       This API is used for port to select svlan priority remarking table
 * Input:
 *      portId                                 -port id, from 0 to 7
 *      index                                  -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_svlanRemarkTable_set(rtk_port_t portId, rtk_index_t index)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, OPRIRMKTBLIDX, index)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_svlanRemarkTable_get
 * Description:
 *       This API is used  to get port svlan priority remarking table index
 * Input:
 *      portId                                 -port id, from 0 to 7
 * Output:
 *      pIndex                                -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_svlanRemarkTable_get(rtk_port_t portId, rtk_index_t* pIndex)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pIndex)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, OPRIRMKTBLIDX, pIndex)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_dscpRemarkTable_set
 * Description:
 *       This API is used for port to select dscp remarking table
 * Input:
 *      portId                                 -port id, from 0 to 7
 *      index                                  -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_dscpRemarkTable_set(rtk_port_t portId, rtk_index_t index)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(index > RTL8307H_INDEXMAX)
        return RT_ERR_QOS_SEL_PRI_GROUP;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, DSCPRMKTBLIDX, index)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_dscpRemarkTable_get
 * Description:
 *       This API is used  to get port dscp remarking table index
 * Input:
 *      portId                                 -port id, from 0 to 7
 * Output:
 *      pIndex                                -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_dscpRemarkTable_get(rtk_port_t portId, rtk_index_t* pIndex)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pIndex)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_REMARKING_CONTROL+portId, DSCPRMKTBLIDX, pIndex)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_priQueueMapping_set
 * Description:
 *       This API is used for port to configure internal priority to queue mapping table
 * Input:
 *      portId                                 -port id, from 0 to 7
 *      priority                              - internal priority, from 0 to 7
 *      queueId                             - queue id, from 0 to 3
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_priQueueMapping_set(rtk_port_t portId, rtk_pri_t priority,  rtk_queue_num_t queueId)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(priority> RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY;

    if(queueId > RTL8307H_QIDMAX)
        return RT_ERR_QUEUE_ID;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_PRIORITY_MAP_TO_QUEUENUMBER_CONTROL+portId, PRI0QNUM-priority, queueId)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_qos_priQueueMapping_get
 * Description:
 *       This API is used get port  internal priority to queue mapping table configuration
 * Input:
 *      portId                                 - port id, from 0 to 7
 *      priority                              - internal priority, from 0 to 7
 * Output:
 *      pQueueId                             - pointer to queue id
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      This API can get port priority to queue mapping table
 */
rtk_api_ret_t rtk_qos_priQueueMapping_get(rtk_port_t portId, rtk_pri_t priority, rtk_queue_num_t* pQueueId)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(priority> RTL8307H_PRIMAX)
        return RT_ERR_QOS_1P_PRIORITY;

    if(NULL == pQueueId)
        return RT_ERR_NULL_POINTER;

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_PRIORITY_MAP_TO_QUEUENUMBER_CONTROL+portId, PRI0QNUM-priority, pQueueId)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_portScheduler_set
 * Description:
 *       Set port schedule
 * Input:
 *      portId                                       -port id
 *      portSchedule                         - schedule type
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SCHEDULE 
 *     RT_ERR_OK                           
 * Note:
 *      This API can set port schedule
 */
rtk_api_ret_t rtk_qos_portScheduler_set(rtk_port_t portId, rtk_queue_scheduleType_t portSchedule)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(portSchedule >= QOS_STRICT_PRIORITY)
        return RT_ERR_QOS_SCHEDULE;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_QUEUE_SCHEDULE_CONTROL+portId, SCHEDULER_TYPE_SEL, portSchedule)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_portScheduler_get
 * Description:
 *       Get port schedule
 * Input:
 *      portId                                 -port id
 * Output:
 *      pPortSchedule                         - schedule type
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_FAILED 
 *     RT_ERR_OK                           
 * Note:
 *      This API can get port schedule
 */
rtk_api_ret_t rtk_qos_portScheduler_get(rtk_port_t portId, rtk_queue_scheduleType_t* pPortSchedule)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pPortSchedule)
        return RT_ERR_NULL_POINTER;

#ifdef	EXT_LAB
	uint32 regVal;
	*pPortSchedule = QOS_WFQ;	
	if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_QUEUE_SCHEDULE_CONTROL+portId, SCHEDULER_TYPE_SEL, &regVal)) != SUCCESS)
		return retVal;
	*pPortSchedule = (rtk_queue_scheduleType_t)regVal;	
#else
    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_QUEUE_SCHEDULE_CONTROL+portId, SCHEDULER_TYPE_SEL, pPortSchedule)) != SUCCESS)
        return retVal;
#endif		

    if(*pPortSchedule >= QOS_STRICT_PRIORITY)
        return RT_ERR_FAILED;
   
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_queueScheduler_set
 * Description:
 *       This API is used to configure queue scheduling algorithm
 * Input:
 *      portId                                 - port id, from 0 to 7
 *      queueId                              -queue id, from 0 to 3
 *      queueSchedule                 - schedule algorithm, from QOS_WFQ to QOS_STRICT_PRIORITY
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_QOS_SCHEDULE 
 *     RT_ERR_OK                           
 * Note:
 *      For a port, WFQ/WRR/CBS can not exist at the same time.
 *      Queue 0 can not be set to CBS.
 */
rtk_api_ret_t rtk_qos_queueScheduler_set(rtk_port_t portId, rtk_queue_num_t queueId, rtk_queue_scheduleType_t queueSchedule)
{
    int32 fieldVal;
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(queueId > RTL8307H_QIDMAX)
        return RT_ERR_QUEUE_ID;

    if(queueSchedule >= QOS_SCHEDULE_END)
        return RT_ERR_INPUT;

    /* Q0 is not allowed to be set as Credit Based Shaper*/
    if(0 == queueId && QOS_CREDIT_BASED == queueSchedule)    
        return RT_ERR_QOS_SCHEDULE;

    if(QOS_STRICT_PRIORITY == queueSchedule)
        fieldVal = 1;
    else
    {
        if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_QUEUE_SCHEDULE_CONTROL+portId, SCHEDULER_TYPE_SEL, queueSchedule)) != SUCCESS)
            return retVal;

        if(QOS_CREDIT_BASED == queueSchedule)
        {
            if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_QUEUE_SCHEDULE_CONTROL+portId, Q0MAXLB_EN-queueId, ENABLED)) != SUCCESS)
                return retVal;
        }
        else
        {
            if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_QUEUE_SCHEDULE_CONTROL+portId, Q0MAXLB_EN-queueId, DISABLED)) != SUCCESS)
                return retVal;
        }
        
        fieldVal = 0;
    }

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_Q0_SCHEDULER_BUCKET_CONTROL2+portId+queueId*24, MINBKTSCKTYPE, fieldVal)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_queueScheduler_set
 * Description:
 *       This API is used to get queue scheduling algorithm
 * Input:
 *      portId                                 - port id, from 0 to 7
 *      queueId                              -queue id, from 0 to 3
 * Output:
 *      pQueueSchedule              -the pointer to  schedule algorithm
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_RT_ERR_FAILED
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_queueScheduler_get(rtk_port_t portId, rtk_queue_num_t queueId, rtk_queue_scheduleType_t* pQueueSchedule)
{
    uint32 fieldVal;
    int32 retVal;
#ifdef	EXT_LAB
	uint32 tempVal;
#else
    int32 tempVal;
#endif

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(queueId > RTL8307H_QIDMAX)
        return RT_ERR_QUEUE_ID;

    if(NULL == pQueueSchedule)
        return RT_ERR_NULL_POINTER;
    
    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_Q0_SCHEDULER_BUCKET_CONTROL2+portId+queueId*24, MINBKTSCKTYPE, &fieldVal)) != SUCCESS)
        return retVal;

    if(1 == fieldVal)
        *pQueueSchedule = QOS_STRICT_PRIORITY;
    else
    {
        if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_QUEUE_SCHEDULE_CONTROL+portId, SCHEDULER_TYPE_SEL, &tempVal)) != SUCCESS)
            return retVal;

        if(tempVal > QOS_CREDIT_BASED)
            return RT_ERR_FAILED;
        
        *pQueueSchedule = tempVal;
    }
    
    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_qos_queueWFQWRRWeight_set
 * Description:
 *       This API is used to configure queue weight
 * Input:
 *      portId                                 - port id, from 0 to 7
 *      queueId                              -queue id, from 0 to 3
 *      queueWeight                    - queue weight of a queue, from 0 to 63
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_QOS_QUEUE_WEIGHT 
 *     RT_ERR_OK                           
 * Note:
 *      This API is valid only if the queue algorithm is WFQ or WRR
 */
rtk_api_ret_t rtk_qos_queueWFQWRRWeight_set(rtk_port_t portId, rtk_queue_num_t queueId, rtk_weight_t queueWeight)
{
    int32 retVal;

    if(portId >= PN_PORT_END)        
        return RT_ERR_PORT_ID;

    if(queueId > RTL8307H_QIDMAX)
        return RT_ERR_QUEUE_ID;

    if(queueWeight > RTL8307H_WEIGHTMAX)
        return RT_ERR_QOS_QUEUE_WEIGHT;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_Q0_SCHEDULER_BUCKET_CONTROL2+portId+queueId*24, MINBKTWEIGHT, queueWeight)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_qos_queueWFQWRRWeight_get
 * Description:
 *       This API is used to get queue weight
 * Input:
 *      portId                                 - port id, from 0 to 7
 *      queueId                              -queue id, from 0 to 3
 * Output:
 *      pQueueWeight                 -the pointer to  queue weight
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_qos_queueWFQWRRWeight_get(rtk_port_t portId, rtk_queue_num_t queueId,  rtk_weight_t* pQueueWeight)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(queueId > RTL8307H_QIDMAX)
        return RT_ERR_QUEUE_ID;

    if(NULL == pQueueWeight)
        return RT_ERR_NULL_POINTER;
     
    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_Q0_SCHEDULER_BUCKET_CONTROL2+portId+queueId*24, MINBKTWEIGHT, pQueueWeight)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_rate_ingressBWCtrl_set
 * Description:
 *      This API is used to configure port ingress bandwidth and flow control ability.
 * Input:
 *      portId                                        -port id, from 0 to 7.
 *      pIngressBwCtrl                       -the pointer to bandwidth control configuration
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_INPUT 
 *     RT_ERR_QOS_RATE 
 *     RT_ERR_OK                           
 * Note:
 *     pIngressBwCtrl->bwCtrlEnable stands for ingress bandwidth control ability, from DISABLED to ENABLED.
 *     pIngressBwCtrl->ingressRate0 stands for the rate of LB0, in unit of 8Kbps, from 0 to 0x1ffff.
 *     pIngressBwCtrl->ingressRate1 stands for the rate of LB0, in unit of 8Kbps, from 0 to 0x1ffff.
 *     pIngressBwCtrl->flowCtrlEnable0 stands for the flow control ability of LB0.
 *     pIngressBwCtrl->flowCtrlEnable1 stands for the flow control ability of LB1.
 */
rtk_api_ret_t rtk_rate_ingressBWCtrl_set(rtk_port_t portId, rtk_ingress_bwCtrl_t* pIngressBwCtrl)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID; 

    if(NULL == pIngressBwCtrl )
        return RT_ERR_NULL_POINTER; 

    if(pIngressBwCtrl->bwCtrlEnable>= RTK_ENABLE_END || pIngressBwCtrl->flowCtrlEnable0 >= RTK_ENABLE_END || pIngressBwCtrl->flowCtrlEnable1 >= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if(pIngressBwCtrl->ingressRate0>RTL8307H_PORTRATEMAX || pIngressBwCtrl->ingressRate1>RTL8307H_PORTRATEMAX )
        return RT_ERR_QOS_RATE;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL0+portId, INBW0_EN, pIngressBwCtrl->bwCtrlEnable)) != SUCCESS)
        return retVal;  

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL1+portId, INBW1_EN, pIngressBwCtrl->bwCtrlEnable)) != SUCCESS)
        return retVal;  

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL0+portId, INBW0_RATE, pIngressBwCtrl->ingressRate0)) != SUCCESS)
        return retVal;  

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL1+portId, INBW1_RATE, pIngressBwCtrl->ingressRate1)) != SUCCESS)
        return retVal;  

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL0+portId, INBW0_FC_EN, pIngressBwCtrl->flowCtrlEnable0)) != SUCCESS)
        return retVal;  

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL1+portId, INBW1_FC_EN, pIngressBwCtrl->flowCtrlEnable1)) != SUCCESS)
        return retVal;  

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_rate_ingressBWCtrl_get
 * Description:
 *      This API is used to get  port ingress bandwidth and flow control ability.
 * Input:
 *      portId                                        -port id, from 0 to 7.
 * Output:
 *      pIngressBwCtrl                       -the pointer to bandwidth control configuration
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_rate_ingressBWCtrl_get(rtk_port_t portId, rtk_ingress_bwCtrl_t* pIngressBwCtrl)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID; 

    if(NULL == pIngressBwCtrl )
        return RT_ERR_NULL_POINTER; 

#ifdef	EXT_LAB
	uint32 regVal;
	pIngressBwCtrl->bwCtrlEnable = DISABLED;
	if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL0+portId, INBW0_EN, &regVal)) != SUCCESS)
		return retVal;  
	pIngressBwCtrl->bwCtrlEnable = (rtk_enable_t)regVal;

	pIngressBwCtrl->bwCtrlEnable = DISABLED;
	if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL1+portId, INBW1_EN, &regVal)) != SUCCESS)
		return retVal;  
	pIngressBwCtrl->bwCtrlEnable = (rtk_enable_t)regVal;
#else
    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL0+portId, INBW0_EN, &pIngressBwCtrl->bwCtrlEnable)) != SUCCESS)
        return retVal;  

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL1+portId, INBW1_EN, &pIngressBwCtrl->bwCtrlEnable)) != SUCCESS)
        return retVal;  
#endif

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL0+portId, INBW0_RATE, &pIngressBwCtrl->ingressRate0)) != SUCCESS)
        return retVal;  

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL1+portId, INBW1_RATE, &pIngressBwCtrl->ingressRate1)) != SUCCESS)
        return retVal;  

#ifdef	EXT_LAB
	pIngressBwCtrl->flowCtrlEnable0 = DISABLED;
	if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL0+portId, INBW0_FC_EN, &regVal)) != SUCCESS)
		return retVal;  
	pIngressBwCtrl->flowCtrlEnable0 = (rtk_enable_t)regVal;

	pIngressBwCtrl->flowCtrlEnable1 = DISABLED;
	if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL1+portId, INBW1_FC_EN, &regVal)) != SUCCESS)
		return retVal;  
	pIngressBwCtrl->flowCtrlEnable1 = (rtk_enable_t)regVal;
#else
    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL0+portId, INBW0_FC_EN, &pIngressBwCtrl->flowCtrlEnable0)) != SUCCESS)
        return retVal;  

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_INGRESS_BANDWIDTH_RATE_CONTROL1+portId, INBW1_FC_EN, &pIngressBwCtrl->flowCtrlEnable1)) != SUCCESS)
        return retVal;  
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_rate_egressPortBWCtrl_set
 * Description:
 *       This API is used to configure port egress bandwidth control
 * Input:
 *      portId                                       -port id, from 0 to 7
 *      enable                                      -bandwidth control ability, from DISABLED to ENABLED
 *      portRate                                   -rate of egress port, in unit of 8Kbps, from 0 to 0x1ffff
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_QOS_RATE 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_rate_egressPortBWCtrl_set(rtk_port_t portId, rtk_enable_t enable, rtk_rate_t portRate)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(enable>= RTK_ENABLE_END)
        return RT_ERR_INPUT;

    if(portRate > RTL8307H_PORTRATEMAX)
        return RT_ERR_QOS_RATE;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_QUEUE_SCHEDULE_CONTROL+portId, MINLB_EN, enable)) != SUCCESS)
        return retVal;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_BANDWITH_CONTROL+portId, BWRATE, portRate)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_rate_egressPortBWCtrl_get
 * Description:
 *       This API is used to get  port egress bandwidth control
 * Input:
 *      portId                                       -port id, from 0 to 7
 * Output:
 *      pEnable                                    -the pointer to bandwidth control ability
 *      pPortRate                                 -the pointer toegress port rate in step of 8Kbps
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_rate_egressPortBWCtrl_get(rtk_port_t portId, rtk_enable_t* pEnable, rtk_rate_t* pPortRate)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(NULL == pEnable ||NULL == pPortRate)
        return RT_ERR_NULL_POINTER;

#ifdef	EXT_LAB
	uint32 regVal;
	*pEnable = DISABLED;
	if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_QUEUE_SCHEDULE_CONTROL+portId, MINLB_EN, &regVal)) != SUCCESS)
		return retVal;
	*pEnable = (rtk_enable_t)regVal;
#else
    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_QUEUE_SCHEDULE_CONTROL+portId, MINLB_EN, pEnable)) != SUCCESS)
        return retVal;
#endif		
    
    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_BANDWITH_CONTROL+portId, BWRATE, pPortRate)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_rate_egressQueueBWCtlrl_set
 * Description:
 *       This API is used to configure egress queue bandwidth control
 * Input:
 *      portId                                 -port id, from 0 to 7            
 *      queueId                              -queue id, from 0 to 3
 *      enable                                -egress queue bandwidth control ability, from DISABLED to ENABLEDy
 *      queueRate                         -rate of egress queue, in unit of 8Kbps, from 0 to 0x1ffff
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_QOS_RATE 
 *     RT_ERR_OK                           
 * Note:
 *      This API is valid only if the queue algorithm is WRR or Strict Priotiy
 */
rtk_api_ret_t rtk_rate_egressQueueBWCtlrl_set(rtk_port_t portId, rtk_queue_num_t queueId, rtk_enable_t enable, rtk_rate_t queueRate)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(queueId > RTL8307H_QIDMAX)
        return RT_ERR_QUEUE_ID;

#ifdef	EXT_LAB
    if(((rtk_queue_scheduleType_t)enable>= QOS_SCHEDULE_END))
#else
    if(enable>= QOS_SCHEDULE_END)
#endif
        return RT_ERR_INPUT;
    
    if(queueRate > RTL8307H_PORTRATEMAX)
        return RT_ERR_QOS_RATE;
    
    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_QUEUE_SCHEDULE_CONTROL+portId, Q0MAXLB_EN-queueId, enable)) != SUCCESS)
        return retVal;

    if((retVal=reg_field_write(RTL8307H_UNIT, PORT0_Q0_SCHEDULER_BUCKET_CONTROL1+portId+queueId*24, MAXBKTAVERAGERATE, queueRate)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_rate_egressQueueBWCtlrl_get
 * Description:
 *       This API is used to get  queue egress bandwidth control
 * Input:
 *      portId                                 -port id, from 0 to 7            
 *      queueId                              -queue id, from 0 to 3
 * Output:
 *      pEnable                              -the pointer to bandwidth control ability
 *      pQueueRate                      -the pointer to egress queue rate in step of 8Kbps
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
rtk_api_ret_t rtk_rate_egressQueueBWCtlrl_get(rtk_port_t portId, rtk_queue_num_t queueId, rtk_enable_t* pEnable, rtk_rate_t* pQueueRate)
{
    int32 retVal;

    if(portId >= PN_PORT_END)
        return RT_ERR_PORT_ID;

    if(queueId > RTL8307H_QIDMAX)
        return RT_ERR_QUEUE_ID;

    if(NULL == pEnable || NULL == pQueueRate)
        return RT_ERR_NULL_POINTER;

#ifdef	EXT_LAB
	uint32 regVal;
	*pEnable = DISABLED;
	if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_QUEUE_SCHEDULE_CONTROL+portId, Q0MAXLB_EN-queueId, &regVal)) != SUCCESS)
		return retVal;
	*pEnable = (rtk_enable_t)regVal;
#else
    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_QUEUE_SCHEDULE_CONTROL+portId, Q0MAXLB_EN-queueId, pEnable)) != SUCCESS)
        return retVal;
#endif		

    if((retVal=reg_field_read(RTL8307H_UNIT, PORT0_Q0_SCHEDULER_BUCKET_CONTROL1+portId+queueId*24, MAXBKTAVERAGERATE, pQueueRate)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_isolation_set
 * Description:
 *      Set permitted port isolation port mask
 * Input:
 *      port - port id.
 *      portmask - permit port mask
 * Output:
 *      None 
 * Return:
 *      RT_ERR_OK              - set shared meter successfully
 *      RT_ERR_FAILED          - FAILED to iset shared meter
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_ID - Invalid port number.
 *      RT_ERR_PORT_MASK - Invalid portmask.
 * Note:
 *      This API sets the ports that a port can forward packets to.
 *      A port can only forward packets to the ports included in permitted portmask
 */
rtk_api_ret_t rtk_port_isolation_set(rtk_port_t port, rtk_portmask_t portmask)
{
    rtk_api_ret_t retVal;

    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID; 

    if (portmask.bits[0] > RTK_MAX_PORT_MASK)
        return RT_ERR_PORT_MASK;
    
    if ((retVal = rtl8307h_iso_mask_set(port, ((~portmask.bits[0]) |(1 << port)) & RTK_MAX_PORT_MASK)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_isolation_get
 * Description:
 *      Get permitted port isolation portmask
 * Input:
 *      port - Port id.
 * Output:
 *      pPortmask - permit port mask
 * Return:
 *      RT_ERR_OK              - success
 *      RT_ERR_FAILED         - FAILED
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_ID - Invalid port number.
 * Note:
 *      This API gets the ports that a port can forward packets to.
 *      A port can only forward packets to the ports included in permitted portmask
 */
rtk_api_ret_t rtk_port_isolation_get(rtk_port_t port, rtk_portmask_t *pPortmask)
{
    rtk_api_ret_t retVal;
    uint32 isomask;
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    if ((retVal = rtl8307h_iso_mask_get(port, &isomask)) != RT_ERR_OK)
        return retVal;

   pPortmask->bits[0] = (~isomask) & (~(1 << port)) & RTK_MAX_PORT_MASK;
    
    return RT_ERR_OK;
}

rtk_ivlsvl_t IvlSvlVlan;


/* Function Name:
 *      rtk_vlan_init
 * Description:
 *      Initialize VLAN
 * Input:
 *      void
 * Output:
 *      void
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI 
 * Note:
 *     VLAN is disabled by default. User has to call this API to enable VLAN before
 *     using it. And it will set a default VLAN(vid 1) including all ports and set 
 *     all ports PVID to the default VLAN.
 */
rtk_api_ret_t rtk_vlan_init(void)
{       
    rtl8307h_vlanTbl_param_t vlanEntry;
    rtk_api_ret_t retVal;
    rtk_vlan_ivlsvl_t ivlsvl;    
    uint16 index;
    uint16 port;

    /*clear vlan table*/
    if ((retVal = rtl8307h_vlan_flush_set()) != RT_ERR_OK)
        return RT_ERR_FAILED;
    
    /*default set VLAN as IVL mode*/
    ivlsvl = VLAN_IVL;
    if ((retVal = rtk_vlan_ivlsvlMode_set(ivlsvl)) != RT_ERR_OK)
        return RT_ERR_FAILED;
    
    /*set default vlan vid = 1*/
    vlanEntry.vid = 1;
    vlanEntry.member = PM_PORT_ALL;
    vlanEntry.untag = PM_PORT_ALL;
    vlanEntry.valid = 1;
    vlanEntry.fid = 0;
    vlanEntry.ucslkfid = (VLAN_IVL == ivlsvl) ? 0 : 1;
    vlanEntry.vbfwd = 0;
    retVal = rtl8307h_vlanTbl_entry_set(4, &vlanEntry);
    if (RT_ERR_OK != retVal)
        return retVal;                       

    memset(&vlanEntry, 0, sizeof(vlanEntry));   
    for (index = 5; index < 8; index ++ )
    {
        retVal = rtl8307h_vlanTbl_entry_set(index, &vlanEntry);
        if (RT_ERR_OK != retVal)
            return retVal;                                        
    }

    /*enable all ports tag aware, accept both untagged and tagged packets, 
      *set all ports PVID to the default VLAN, and set tag-mode to original,
      *disable VLAN ingress filter*/
    for (port = 0 ; port < PN_PORT_END; port ++)
    {
        retVal =  rtl8307h_vlan_portTagAware_set(port, TRUE);
        if (RT_ERR_OK != retVal)
            return retVal;

        retVal = rtl8307h_vlan_portAcceptFrameType_set(port, ACCEPT_FRAME_TYPE_ALL);
        if (RT_ERR_OK != retVal)
            return retVal;
        
        retVal =  rtl8307h_vlan_portPvid_set(port, 1, 0);
        if (RT_ERR_OK != retVal)
            return retVal;
                
        retVal = rtl8307h_vlan_portEgressTagStatus_set(port, TAG_STATUS_WITHOUT_TAG, TAG_STATUS_WITHOUT_TAG);
        if (RT_ERR_OK != retVal)
            return retVal;

        retVal = rtl8307h_vlan_portEgressTagMode_set(port, VLAN_TAG_MODE_ORIGINAL, VLAN_TAG_MODE_ORIGINAL);
        if (RT_ERR_OK != retVal)
            return retVal;
        
       retVal = rtl8307h_vlan_portIgrFilterEnable_set(port, FALSE);
        if (RT_ERR_OK != retVal)
            return retVal; 
    }       

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_vlan_set
 * Description:
 *      Set a VLAN entry
 * Input:
 *      vid           - VLAN ID to configure
 *      mbrmsk        - VLAN member set portmask
 *      untagmsk      - VLAN untag set portmask
 *      fid           -  filtering database id(0 ~ 14)
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_VLAN_VID
 *      RT_ERR_VLAN_PORT_MBR_EXIST
 *      RT_ERR_L2_FID
 *      RT_ERR_TBL_FULL
 * Note:
 *     There are 4K VLAN entry supported. User could configure the member set and untag set
 *     for specified vid through this API. The portmask's bit N means port N.
 *     For example, mbrmask 23=0x17=010111 means port 0,1,2,4 in the member set.
 *     For this chip, FID range is 0~14, it is used as spanning tree instance ID, if VLAN is set as 
 *     SVL mode, FID is also used as filtering database id. 
 *     Because SVLAN and CVLAN share the same vlan table, so SVLAN also use this API to set 
 *      vlan information.
 */
rtk_api_ret_t rtk_vlan_set(rtk_vlan_t vid, rtk_portmask_t mbrmsk, rtk_portmask_t untagmsk, rtk_fid_t fid)
{
    rtl8307h_vlanTbl_param_t vlanEntry;
    uint32 index, hit_index;
    uint16 hit_flag;                                /*0:not hit,  1: hit vlan table,  2: hit vlan cam table */
    uint32 invalid_index;
    uint32 cam_invalid_index;   
    rtk_api_ret_t retVal;
    rtk_vlan_ivlsvl_t ivlsvl;  
    
    /* vid must be 0~4095 */
    if (vid > RTL8307H_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* fid must be 0~14 */
    if (fid > RTL8307H_FIDMAX)
        return RT_ERR_L2_FID;  

    /*get vlan IVL or SVL mode*/
    if ((retVal = rtk_vlan_ivlsvlMode_get(&ivlsvl)) != RT_ERR_OK)
        return RT_ERR_FAILED;
    
    /*search 128-entry vlan table and 16-entry vlan cam table to find the vid*/
    hit_flag = 0;
    hit_index = 0xffff;
    invalid_index = 0xffff;
    cam_invalid_index = 0xffff;  
    memset(&vlanEntry, 0, sizeof(vlanEntry));
    for (index = (vid & 0x1F) << 2; index < ((vid & 0x1F) << 2) + 4; index ++)
    {
        if ((retVal = rtl8307h_vlanTbl_entry_get(index, &vlanEntry)) != RT_ERR_OK)
            return retVal;

        if (!vlanEntry.valid)
            invalid_index = index;
        
        if (vlanEntry.valid && (vlanEntry.vid == vid))
        {
            hit_flag = 1;
            hit_index = index;
            break;
        }
    }
    
    if (!hit_flag)
    {
        for (index = 0; index < RTL8307H_VLANCAMMAX; index ++)
        {
            if ((retVal = rtl8307h_vlanCam_entry_get(index, &vlanEntry)) != RT_ERR_OK)
                return retVal;
            
            if (!vlanEntry.valid)
                cam_invalid_index = index;
            if (vlanEntry.valid && (vlanEntry.vid == vid))
            {
                hit_flag = 2;
                hit_index = index;
                break;
            }                                                           
        }        
    }

    memset(&vlanEntry, 0, sizeof(vlanEntry));
    vlanEntry.vid = (uint16)vid;
    vlanEntry.member = mbrmsk.bits[0];
    vlanEntry.untag = untagmsk.bits[0];
    vlanEntry.valid = 1;
    vlanEntry.fid = (uint16)fid;
    vlanEntry.ucslkfid = (VLAN_IVL == ivlsvl) ? 0 : 1;
    vlanEntry.vbfwd = 0;
    
    if((vlanEntry.member == 0) && (vlanEntry.untag == 0))
    {
        /*delete the vlan*/
        vlanEntry.valid = 0;
    }

    if (hit_flag == 1)
    {       
        if ((retVal = rtl8307h_vlanTbl_entry_set(hit_index, &vlanEntry)) != RT_ERR_OK)
            return retVal;
    }
    else if (hit_flag == 2)
    {   
        if ((retVal = rtl8307h_vlanCam_entry_set(hit_index, &vlanEntry)) != RT_ERR_OK)
            return retVal;
    }
    else if (invalid_index != 0xffff)
    {
        if ((retVal = rtl8307h_vlanTbl_entry_set(invalid_index, &vlanEntry)) != RT_ERR_OK)
            return retVal;             
    }
    else if (cam_invalid_index != 0xffff)
    {
        if ((retVal = rtl8307h_vlanCam_entry_set(cam_invalid_index, &vlanEntry)) != RT_ERR_OK)
            return retVal;                   
    }
    else 
    {
        return RT_ERR_TBL_FULL;
    }
        
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_vlan_get
 * Description:
 *      Get a VLAN entry
 * Input:
 *      vid             - VLAN ID to configure
 * Output:
 *      pMbrmsk         - VLAN member set portmask
 *      pUntagmsk       - VLAN untag set portmask
 *      pFid            -  filtering database id (0 ~ 14)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_VLAN_VID
 *      RT_ERR_NULL_POINTER 
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 *     There are 4K VLAN entry supported. User could configure the member set and untag set
 *     for specified vid through this API. The portmask's bit N means port N.
 *     For example, mbrmask 23=0x17=010111 means port 0,1,2,4 in the member set.
 *     For this chip, FID range is 0~14, it is used as spanning tree instance ID, if VLAN is set as 
 *     SVL mode, FID is also used as filtering database id. 
 *     Because SVLAN and CVLAN share the same vlan table, so SVLAN also use this API to set 
 *      vlan information.
 */
rtk_api_ret_t rtk_vlan_get(rtk_vlan_t vid, rtk_portmask_t *pMbrmsk, rtk_portmask_t *pUntagmsk, rtk_fid_t *pFid)
{
    rtl8307h_vlanTbl_param_t vlanEntry;
    uint32 index, hit_index;
    uint16 hit_flag;        /*0:not hit  1: hit vlan table, 2: hit vlan cam table */
    rtk_api_ret_t retVal;
        
    /* vid must be 0~4095 */
    if(vid > RTL8307H_VIDMAX)
        return RT_ERR_VLAN_VID;

    if ((NULL == pMbrmsk) || (NULL == pUntagmsk) || (NULL == pFid))
        return RT_ERR_NULL_POINTER;

    /*search 128-entry vlan table and 16-entry vlan cam table to find the vid*/
    hit_flag = 0;
    hit_index = 0xffff;
    memset(&vlanEntry, 0, sizeof(vlanEntry));
    for (index = (vid & 0x1F) << 2; index < ((vid & 0x1F) << 2) + 4; index ++)
    {
        if ((retVal = rtl8307h_vlanTbl_entry_get(index, &vlanEntry)) != RT_ERR_OK)
            return retVal;
        
        if (vlanEntry.valid && (vlanEntry.vid == vid))
        {
            hit_flag = 1;
            hit_index = index;            
            pMbrmsk->bits[0] = vlanEntry.member;
            pUntagmsk->bits[0] = vlanEntry.untag;
            *pFid = (rtk_fid_t)vlanEntry.fid;
            break;
        }
    }
    
    if (!hit_flag)
    {
        for (index = 0; index < RTL8307H_VLANCAMMAX; index ++)
        {
            if ((retVal = rtl8307h_vlanCam_entry_get(index, &vlanEntry)) != RT_ERR_OK)
                return retVal;
                    
            if (vlanEntry.valid && (vlanEntry.vid == vid))
            {
                hit_flag = 2;
                hit_index = index;
                pMbrmsk->bits[0] = vlanEntry.member;
                pUntagmsk->bits[0] = vlanEntry.untag;
                *pFid = (rtk_fid_t)vlanEntry.fid;                
                break;
            }                                                           
        }        
    }

#ifdef	EXT_LAB
	hit_index = hit_index;
#endif

    if (!hit_flag)
        return RT_ERR_VLAN_ENTRY_NOT_FOUND;
            
    return RT_ERR_OK;

}


/* Function Name:
 *      rtk_vlan_portPvid_set
 * Description:
 *      Set port to specified VLAN ID(PVID)
 * Input:
 *      port             - Port id
 *      pvid             - Specified VLAN ID
 *      priority         - 802.1p priority for the PVID
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_VLAN_VID
 *      RT_ERR_VLAN_PRIORITY 
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 *    The API is used for Port-based VLAN. The untagged frame received from the
 *    port will be classified to the specified VLAN and assigned to the specified priority.
 */
rtk_api_ret_t rtk_vlan_portPvid_set(rtk_port_t port, rtk_vlan_t pvid, rtk_pri_t priority)
{
    rtk_api_ret_t retVal;

    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    /* vid must be 0~4095 */
    if (pvid > RTL8307H_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* priority must be 0~7 */
    if (priority > RTL8307H_PRIMAX)
        return RT_ERR_VLAN_PRIORITY;

    retVal = rtl8307h_vlan_portPvid_set(port, pvid, priority);
    
    
    return retVal;
}

/* Function Name:
 *      rtk_vlan_portPvid_get
 * Description:
 *      Get VLAN ID(PVID) on specified port
 * Input:
 *      port             - Port id
 *      pPvid            - Specified VLAN ID
 *      pPriority        - 802.1p priority for the PVID
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER 
 * Note:
 *    The API is used for Port-based VLAN. The untagged frame received from the
 *    port will be classified to the specified VLAN and assigned to the specified priority.
 */
rtk_api_ret_t rtk_vlan_portPvid_get(rtk_port_t port, rtk_vlan_t *pPvid, rtk_pri_t *pPriority)
{
    rtk_api_ret_t retVal;
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    if (pPvid == NULL)
        return RT_ERR_NULL_POINTER;

    if (pPriority == NULL) 
        return RT_ERR_NULL_POINTER;
    
    retVal = rtl8307h_vlan_portPvid_get(port, pPvid, pPriority);
    return retVal;
}

/* Function Name:
 *      rtk_vlan_destroy
 * Description:
 *      remove a VLAN entry
 * Input:
 *      vid             - VLAN ID to configure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_VLAN_VID
 *      RT_ERR_NULL_POINTER 
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 */
rtk_api_ret_t rtk_vlan_destroy(rtk_vlan_t vid)
{
    rtl8307h_vlanTbl_param_t vlanEntry;
    uint16 index;
    uint16 hit_flag;        /*0:not hit  1: hit vlan table, 2: hit vlan cam table */
    rtk_api_ret_t retVal;
        
    /* vid must be 0~4095 */
    if(vid > RTL8307H_VIDMAX)
        return RT_ERR_VLAN_VID;

    /*search 128-entry vlan table and 16-entry vlan cam table to find the vid*/
    hit_flag = 0;
    memset(&vlanEntry, 0, sizeof(vlanEntry));
    for (index = (vid & 0x1F) << 2; index < ((vid & 0x1F) << 2) + 4; index ++)
    {
        if ((retVal = rtl8307h_vlanTbl_entry_get(index, &vlanEntry)) != RT_ERR_OK)
            return retVal;
        
        if (vlanEntry.valid && (vlanEntry.vid == vid))
        {
            hit_flag = 1;         
            memset(&vlanEntry, 0, sizeof(vlanEntry));
            if ((retVal = rtl8307h_vlanTbl_entry_set(index, &vlanEntry)) != RT_ERR_OK)
                return retVal;
            break;
        }
    }
    
    if (!hit_flag)
    {
        for (index = 0; index < RTL8307H_VLANCAMMAX; index ++)
        {
            if ((retVal = rtl8307h_vlanCam_entry_get(index, &vlanEntry)) != RT_ERR_OK)
                return retVal;
            
            if (vlanEntry.valid && (vlanEntry.vid == vid))
            {
                hit_flag = 2;
                memset(&vlanEntry, 0, sizeof(vlanEntry)); 
                if ((retVal = rtl8307h_vlanCam_entry_set(index, &vlanEntry)) != RT_ERR_OK)
                    return retVal;
                break;
            }                                                           
        }        
    }

    if (!hit_flag)
        return RT_ERR_VLAN_ENTRY_NOT_FOUND;


    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_vlan_portAcceptFrameType_set
 * Description:
 *      Set VLAN support frame type
 * Input:
 *      port                                 - Port id
 *      accept_frame_type             - accept frame type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_PORT_ID
 * Note:
 *    The API is used for checking 802.1Q tagged frames.
 *    The accept frame type as following:
 *    ACCEPT_FRAME_TYPE_ALL
 *    ACCEPT_FRAME_TYPE_TAG_ONLY
 *    ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
rtk_api_ret_t rtk_vlan_portAcceptFrameType_set(rtk_port_t port, rtk_vlan_acceptFrameType_t accept_frame_type)
{
    rtk_api_ret_t retVal;

    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if (accept_frame_type >= ACCEPT_FRAME_TYPE_END)
        return RT_ERR_VLAN_ACCEPT_FRAME_TYPE;
    
    retVal = rtl8307h_vlan_portAcceptFrameType_set(port, accept_frame_type);

    return retVal;
}


/* Function Name:
 *      rtk_vlan_portAcceptFrameType_get
 * Description:
 *      Get VLAN support frame type
 * Input:
 *      port                                 - Port id
 *      accept_frame_type             - accept frame type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_PORT_ID
 * Note:
 *    The API is used for checking 802.1Q tagged frames.
 *    The accept frame type as following:
 *    ACCEPT_FRAME_TYPE_ALL
 *    ACCEPT_FRAME_TYPE_TAG_ONLY
 *    ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
rtk_api_ret_t rtk_vlan_portAcceptFrameType_get(rtk_port_t port, rtk_vlan_acceptFrameType_t *pAccept_frame_type)
{
    uint32 retVal;
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    if (pAccept_frame_type == NULL)
        return RT_ERR_NULL_POINTER;

    if((retVal = rtl8307h_vlan_portAcceptFrameType_get(port, pAccept_frame_type)) != RT_ERR_OK)
        return retVal; 
    
    return RT_ERR_OK;
}    

/* Function Name:
 *      rtk_vlan_portEgressTagStatus_set
 * Description:
 *      Set egress port VLAN tag status
 * Input:
 *      port      -  port id
 *      tag_rxu     - tx with or without CVLAN tag for the pkts received from UNI port
 *      tag_rxn     - tx with or without CVLAN tag for the pkts received from NNI port
 * Output:
*       none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      The API sets egress port transimiting pakcet with or without CVLAN tag.
 *      The tag status could be set as following:
 *      TAG_STATUS_WITH_TAG
 *      TAG_STATUS_WITHOUT_TAG
 */
rtk_api_ret_t rtk_vlan_portEgressTagStatus_set(rtk_port_t port, rtk_vlan_txTagStatus_t tag_status)
{
    rtk_api_ret_t retVal;
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if (tag_status > TAG_STATUS_END)
        return RT_ERR_ENABLE;

    retVal = rtl8307h_vlan_portEgressTagStatus_set(port, tag_status, tag_status);    
    
    return retVal;
}

/* Function Name:
 *      rtk_vlan_portEgressTagStatus_set
 * Description:
 *      Get egress port VLAN tag status
 * Input:
 *      port       -  port id
 * Output:
 *      pTag_status -  the pointer of egress port tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      The API gets egree port transimiting pakcet with or without CVLAN tag.
 *      The tag status could be following values:
 *      TAG_STATUS_WITH_TAG
 *      TAG_STATUS_WITHOUT_TAG
 */
rtk_api_ret_t rtk_vlan_portEgressTagStatus_get(rtk_port_t port, rtk_vlan_txTagStatus_t* pTag_status)
{
    rtk_api_ret_t retVal;
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if(NULL == pTag_status)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8307h_vlan_portEgressTagStatus_get(port, pTag_status, pTag_status)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_vlan_portIgrFilterEnable_set
 * Description:
 *      Set VLAN ingress for each port
 * Input:
 *      port         - Port id
 *      igr_filter    - VLAN ingress function enable status
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_ENABLE
 * Note:
 *    The status of vlan ingress filter is as following:
 *      DISABLED
 *      ENABLED
 *   While VLAN function is enabled, ASIC will decide VLAN ID for each received frame and get belonged member
 *   ports from VLAN table. If received port is not belonged to VLAN member ports, ASIC will drop received frame if VLAN ingress function is enabled.
 */
rtk_api_ret_t rtk_vlan_portIgrFilterEnable_set(rtk_port_t port, rtk_enable_t igr_filter)
{
    rtk_api_ret_t retVal;

    if(port >= RTK_MAX_NUM_OF_PORT)
        return RT_ERR_PORT_ID;

    if(igr_filter >= RTK_ENABLE_END)
        return RT_ERR_ENABLE; 

    retVal = rtl8307h_vlan_portIgrFilterEnable_set(port, igr_filter);

    return retVal;
}

/* Function Name:
 *      rtk_vlan_portIgrFilterEnable_get
 * Description:
 *      Get VLAN Ingress Filter
 * Input:
 *      port            - Port id
 * Output:
 *      pIgr_filter    -  the pointer of VLAN ingress function enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *     The API can Get the VLAN ingress filter status.
 *     The status of vlan ingress filter is as following:
 *     DISABLED
 *     ENABLED   
 */
rtk_api_ret_t rtk_vlan_portIgrFilterEnable_get(rtk_port_t port, rtk_enable_t *pIgr_filter)
{
    rtk_api_ret_t retVal;
    uint32        enabled;
    
    if(port >= RTK_MAX_NUM_OF_PORT)
        return RT_ERR_PORT_ID;

    if (NULL == pIgr_filter)
        return RT_ERR_NULL_POINTER;

    if((retVal = rtl8307h_vlan_portIgrFilterEnable_get(port, &enabled)) != RT_ERR_OK)
        return retVal; 
    
    *pIgr_filter = enabled ? ENABLED : DISABLED;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_vlan_tagAware_set
 * Description:
 *      Set ingress port aware tag VID  or not
 * Input:
 *      port       -  port id
 *      enabled  -  aware or unware
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      (1) If ingress port awares tag VID, for tagged packet, its VLAN is VID 
 *           in VLAN tag, for untagged packet, its VLAN is PVID
 *      (2) If ingress port unawares tag VID, for both tagged packet and untagged packet, 
 *           its VLAN is PVID.
 */
rtk_api_ret_t rtk_vlan_tagAware_set(rtk_port_t port, rtk_enable_t enabled)
{
    rtk_api_ret_t retVal;    
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if (enabled >= RTK_ENABLE_END)
        return RT_ERR_ENABLE;


    if ((retVal = rtl8307h_vlan_portTagAware_set(port, enabled)) != RT_ERR_OK)
        return retVal;
    if ((retVal = rtl8307h_vlan_portParserTagAbility_set(port, enabled)) != RT_ERR_OK)
        return retVal;
    
    return RT_ERR_OK;    
}

/* Function Name:
 *      rtk_vlan_tagAware_get
 * Description:
 *      Get ingress port aware tag VID  or not
 * Input:
 *      port        -  port id
 * Output:
 *      pEnabled  -  the pointer of aware or unware
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      (1) If ingress port awares tag VID, for tagged packet, its VLAN is VID 
 *           in VLAN tag, for untagged packet, its VLAN is PVID
 *      (2) If ingress port unawares tag VID, for both tagged packet and untagged packet, 
 *           its VLAN is PVID.
 */
rtk_api_ret_t rtk_vlan_tagAware_get(rtk_port_t port, rtk_enable_t *pEnabled)
{
#ifdef	EXT_LAB
	rtk_api_ret_t retVal = RT_ERR_OK;
#else
    rtk_api_ret_t retVal;    
#endif
    uint32 enabled;
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    if(NULL == pEnabled)
        return RT_ERR_NULL_POINTER;

    if((rtl8307h_vlan_portTagAware_get(port, &enabled) != RT_ERR_OK))
        return retVal;
    
    *pEnabled = enabled ? ENABLED : DISABLED;
    
    return RT_ERR_OK;    
}

/* Function Name:
 *      rtk_leaky_vlan_set
 * Description:
 *      Set VLAN leaky
 * Input:
 *      type             - Packet type for VLAN leaky
 *      enable           - Leaky status
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_INPUT
 *      RT_ERR_ENABLE 
 * Note:
 *    This API can set VLAN leaky.
 *    The leaky frame types are as following:
 *    LEAKY_GRP
 *    LEAKY_STATIC_LUT
 *    LEAKY_RLDP.
 */
rtk_api_ret_t rtk_leaky_vlan_set(rtk_leaky_type_t type, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;

    if (type >= LEAKY_END)
        return RT_ERR_INPUT;

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_ENABLE;
    
    switch (type)
    {
            case LEAKY_GRP:
            retVal = reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_LOOKUP_CONTROL, GRPVLEAKY, enable);
            break;
        case LEAKY_STATIC_LUT:
            retVal = reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_LOOKUP_CONTROL, STATICVLKY, enable);
            break;
        default:
            return RT_ERR_INPUT;
    }

    return retVal;
}


/* Function Name:
 *      rtk_leaky_vlan_get
 * Description:
 *      Get VLAN leaky setting
 * Input:
 *      type                - Packet type for VLAN leaky
 * Output:
 *      rtk_enable_t     - Leaky status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER 
 * Note:
 *    This API can get VLAN leaky setting
 *    The leaky frame types are as following:
 *    LEAKY_GRP
 *    LEAKY_STATIC_LUT
 *    LEAKY_RLDP.
 */
 rtk_api_ret_t rtk_leaky_vlan_get(rtk_leaky_type_t type, rtk_enable_t *pEnable)
{
    rtk_api_ret_t retVal;
    uint32        feildVal;
    
    if (pEnable == NULL)
    {
        return RT_ERR_NULL_POINTER;
    }
    
    if (type >= LEAKY_END)
        return RT_ERR_INPUT;    

    switch (type)
    {
        case LEAKY_GRP:
            if((retVal = reg_field_read(RTL8307H_UNIT, ADDRESS_TABLE_LOOKUP_CONTROL, GRPVLEAKY, &feildVal)) != RT_ERR_OK)
                return retVal; 
            break;
        case LEAKY_STATIC_LUT:
            if((retVal = reg_field_read(RTL8307H_UNIT, ADDRESS_TABLE_LOOKUP_CONTROL, STATICVLKY, &feildVal)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_INPUT;
    }

    *pEnable = feildVal ? ENABLED : DISABLED;
    
    return retVal;
}

/* Function Name:
 *      rtk_vlan_ivlsvlMode_set
 * Description:
 *      Set VLAN IVL or SVL mode
 * Input:
 *      ivlsvl           - IVL or SVL mode
 * Output:
*       none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      this API can set VLAN as IVL(Independent VLAN Learning) or SVL(shared VLAN learning) mode. 
 *      for IVL mode, if a given individual MAC Address is learned in one VLAN, that learned information 
 *      is not used in forwarding decisions taken for that address relative to any other VLAN.
 *       
 */
rtk_api_ret_t rtk_vlan_ivlsvlMode_set(rtk_vlan_ivlsvl_t ivlsvl)
{
    if (ivlsvl >= VLAN_IVLSVL_END)
        return RT_ERR_FAILED;

    IvlSvlVlan = ivlsvl;    
                
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_vlan_ivlsvlMode_get
 * Description:
 *      Get VLAN IVL or SVL mode
 * Input:
 *      pIvlsvl       -  the pointer of VLAN IVL or SVL mode
 * Output:
*       none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      this API can get VLAN as IVL(Independent VLAN Learning) or SVL(shared VLAN learning) mode. 
 *      for IVL mode, if a given individual MAC Address is learned in one VLAN, that learned information 
 *      is not used in forwarding decisions taken for that address relative to any other VLAN, its FID
 *      is VLAN ID.
 *       
 */
rtk_api_ret_t rtk_vlan_ivlsvlMode_get(rtk_vlan_ivlsvl_t *pIvlsvl)
{
    if (NULL == pIvlsvl)
        return RT_ERR_NULL_POINTER;

    *pIvlsvl = IvlSvlVlan;
                
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_vlan_stg_set
 * Description:
 *      Set spanning tree group instance of the vlan to the specified device
 * Input:
 *      vid                -  specified VLAN ID
 *      stg                -  spanning tree group instance
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_SMI
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 *     The API can set spanning tree group instance of the vlan to the specified device.
 *     The spanning tree group number is from 0 to 14.  
 */
rtk_api_ret_t rtk_vlan_stg_set(rtk_vlan_t vid, rtk_stg_t stg)
{
    rtk_api_ret_t retVal;
    rtk_portmask_t mbrmsk;
    rtk_portmask_t untagmsk;
    rtk_fid_t fid;

    /* vid must be 0~4095 */
    if( vid > RTL8307H_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* stg must be 0~14 */
    if( stg > RTL8307H_FIDMAX)
        return RT_ERR_MSTI;
    
    if ((retVal = rtk_vlan_get(vid, &mbrmsk, &untagmsk, &fid)) != RT_ERR_OK )
        return retVal;

    fid = (rtk_fid_t)stg;

    retVal = rtk_vlan_set(vid, mbrmsk, untagmsk, fid);
       
    return retVal;    
}

/* Function Name:
 *      rtk_vlan_stg_get
 * Description:
 *      Set spanning tree group instance of the vlan to the specified device
 * Input:
 *      vid                -  specified VLAN ID
 * Output:
 *      pStg             -   spanning tree group instance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 *     The API can set spanning tree group instance of the vlan to the specified device.
 *     The spanning tree group number is from 0 to 15.  
 */
rtk_api_ret_t rtk_vlan_stg_get(rtk_vlan_t vid, rtk_stg_t *pStg)
{
    rtk_api_ret_t retVal;
    rtk_portmask_t mbrmsk;
    rtk_portmask_t untagmsk;
    rtk_fid_t fid;

    /* vid must be 0~4095 */
    if( vid > RTL8307H_VIDMAX)
        return RT_ERR_VLAN_VID;
    
    if ( NULL == pStg )
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtk_vlan_get(vid, &mbrmsk, &untagmsk, &fid)) != RT_ERR_OK )
        return retVal;
    
    *pStg = (rtk_stg_t) (fid);

    return RT_ERR_OK;
    
}


/* Function Name:
 *      rtk_svlan_init
 * Description:
 *      Initialize SVLAN Configuration
 * Input:
 *      none
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 *      RT_ERR_INPUT
 * Note:
 *    Ether type of S-tag in 802.1ad is 0x88a8 and there are existed ether type 0x9100 and 0x9200 for Q-in-Q SLAN design. 
 *    User can set mathced ether type as service provider supported protocol. After call this API, 
 *    all ports are set as CVLAN port. you can use rtk_svlan_servicePort_add to add SVLAN port. 
 *    
 */
rtk_api_ret_t rtk_svlan_init(void)
{
    rtk_api_ret_t retVal;
    rtk_port_t port;

    /*set default SVLAN SPID 0x88a8*/
    if ((retVal = rtl8307h_svlan_tpid_set(0x88a8)) != RT_ERR_OK)
        return retVal;

    /*set all port as UNI port, forward base cvlan*/
    for ( port = 0 ; port < PN_PORT_END; port ++ )
    {
        if ((retVal = rtl8307h_vlan_portRole_set(port, RTL8307H_PORTROLE_UNI)) != RT_ERR_OK)
        {
            return retVal;
        }    

        if ((retVal = rtl8307h_svlan_portFwdBaseOvid_set(port, FALSE)) != RT_ERR_OK)
        {
            return retVal;            
        }

    }

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_svlan_servicePort_add
 * Description:
 *      Enable one service port in the specified device
 * Input:
 *      port     -  Port id
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 * Note:
 *    This API is setting which port is connected to provider switch. All frames receiving from
 *     this port will recognize Service Vlan Tag. 
 */
rtk_api_ret_t rtk_svlan_servicePort_add(rtk_port_t port)
{
    rtk_api_ret_t retVal;

    /*set port as Service Provider Port, forward base OVID, */
    if ((retVal = rtl8307h_vlan_portRole_set(port, RTL8307H_PORTROLE_NNI)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8307h_svlan_portFwdBaseOvid_set(port, TRUE)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8307h_vlan_portIgrFilterEnable_set(port, TRUE)) != RT_ERR_OK)
        return retVal;

    if ((retVal =  rtk_svlan_tagAware_set(port, ENABLED) ) != RT_ERR_OK) 
        return retVal;

    if ((retVal = rtl8307h_svlan_portAcceptFrameType_set(port, ACCEPT_FRAME_TYPE_TAG_ONLY)) != RT_ERR_OK)
        return retVal;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_svlan_servicePort_del
 * Description:
 *      Disable one service port in the specified device
 * Input:
 *      port     -  Port id
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 * Note:
 */
rtk_api_ret_t rtk_svlan_servicePort_del(rtk_port_t port)
{
    rtk_api_ret_t retVal;

    /*set port as Custermer Port, forward base IVID, */
    if ((retVal = rtl8307h_vlan_portRole_set(port, RTL8307H_PORTROLE_UNI)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8307h_svlan_portFwdBaseOvid_set(port, FALSE)) != RT_ERR_OK)
        return retVal;

    if ((retVal =  rtk_svlan_tagAware_set(port, DISABLED )) != RT_ERR_OK) 
        return retVal;

    if ((retVal = rtl8307h_svlan_portAcceptFrameType_set(port, ACCEPT_FRAME_TYPE_ALL)) != RT_ERR_OK)
        return retVal;
        
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_svlan_servicePort_get
 * Description:
 *      Get all the service ports in the specified device
 * Input:
 *      none
 * Output:
 *      pSvlan_portmask  - svlan ports mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
rtk_api_ret_t rtk_svlan_servicePort_get(rtk_portmask_t *pSvlan_portmask)
{
    rtk_port_t port;    
    rtk_api_ret_t retVal;
    rtl8307h_vlan_portRole_t portRole;

    if (NULL == pSvlan_portmask)
        return RT_ERR_NULL_POINTER;

    pSvlan_portmask->bits[0] = 0;
    for (port = 0; port < PN_PORT_END; port ++)
    {
        if ((retVal = rtl8307h_vlan_portRole_get(port, &portRole)) != RT_ERR_OK)
            return retVal;
        if (RTL8307H_PORTROLE_NNI == portRole)
        {
            pSvlan_portmask->bits[0] |=  (1 << (uint32)port);        
        }
    }
    
    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_svlan_tpidEntry_set
 * Description:
 *      Configure accepted S-VLAN ether type. The default ether type of S-VLAN is 0x88a8
 * Input:
 *      svlan_tag_id  - Ether type of S-tag frame parsing in uplink ports
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 * Note:
 *      Ether type of S-tag in 802.1ad is 0x88a8 and there are existed ether type 0x9100 
 *      and 0x9200 for Q-in-Q SLAN design. User can set mathced ether type as service 
 *      provider supported protocol. 
 */
rtk_api_ret_t rtk_svlan_tpidEntry_set(rtk_svlan_tpid_t svlan_tag_id)
{
    rtk_api_ret_t retVal;
    
    if (svlan_tag_id > RTK_MAX_NUM_OF_PROTO_TYPE)
        return RT_ERR_INPUT;
    
    if ((retVal = rtl8307h_svlan_tpid_set(svlan_tag_id)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_svlan_tpidEntry_get
 * Description:
 *      Get accepted S-VLAN ether type. The default ether type of S-VLAN is 0x88a8
 * Input:
 *      pSvlan_tag_id       - Ether type of S-tag frame parsing in uplink ports
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Ether type of S-tag in 802.1ad is 0x88a8 and there are existed ether type 0x9100 
 *      and 0x9200 for Q-in-Q SLAN design. User can set mathced ether type as service 
 *      provider supported protocol. 
 */
rtk_api_ret_t rtk_svlan_tpidEntry_get(rtk_svlan_tpid_t *pSvlan_tag_id)
{

    rtk_api_ret_t retVal;    

    if ((retVal = rtl8307h_svlan_tpid_get(pSvlan_tag_id)) !=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_svlan_portPvid_set
 * Description:
 *      Set port to specified VLAN ID(PVID) for Service Provider Port
 * Input:
 *      port             - Port id
 *      pvid             - Specified Service VLAN ID
 *      priority         - 802.1p priority for the PVID
 *      dei               - Service VLAN tag DEI bit
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SMI 
 *      RT_ERR_VLAN_PRIORITY 
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 *    The API is used for Port-based VLAN. The untagged frame received from the
 *    port will be classified to the specified VLAN and assigned to the specified priority.
 */
rtk_api_ret_t rtk_svlan_portPvid_set(rtk_port_t port, rtk_vlan_t pvid, rtk_pri_t priority, rtk_dei_t dei)
{
    rtk_api_ret_t retVal;

    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    /* vid must be 0~4095 */
    if(pvid > RTL8307H_VIDMAX)
        return RT_ERR_VLAN_VID;

    /* priority must be 0~7 */
    if(priority > RTL8307H_PRIMAX)
        return RT_ERR_VLAN_PRIORITY;

    retVal = rtl8307h_svlan_portPvid_set(port, pvid, priority, dei);
        
    return retVal;
}

/* Function Name:
 *      rtk_svlan_portPvid_get
 * Description:
 *      Get Service VLAN ID(PVID) on specified port
 * Input:
 *      port             - Port id
 *      pPvid            - Specified VLAN ID
 *      pPriority        - 802.1p priority for the PVID
 *      pDei             - DEI bit
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER 
 * Note:
 *    The API is used for Port-based VLAN. The untagged frame received from the
 *    port will be classified to the specified VLAN and assigned to the specified priority.
 */
rtk_api_ret_t rtk_svlan_portPvid_get(rtk_port_t port, rtk_vlan_t *pPvid, rtk_pri_t* pPriority, rtk_dei_t *pDei)
{
    rtk_api_ret_t retVal;

    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if ((NULL == pPvid) || (NULL == pPriority) || (NULL == pDei))
        return RT_ERR_NULL_POINTER;

    retVal = rtl8307h_svlan_portPvid_get(port, pPvid, pPriority, pDei);
        
    return retVal;
}

/* Function Name:
 *      rtk_svlan_portAcceptFrameType_set
 * Description:
 *      Set Service VLAN support frame type
 * Input:
 *      port                                 - Port id
 *      accept_frame_type             - accept frame type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI 
 *      RT_ERR_PORT_ID
 * Note:
 *    The API is used for checking Service VLAN tagged frames.
 *    The accept frame type as following:
 *    ACCEPT_FRAME_TYPE_ALL
 *    ACCEPT_FRAME_TYPE_TAG_ONLY
 *    ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
rtk_api_ret_t rtk_svlan_portAcceptFrameType_set(rtk_port_t port, rtk_vlan_acceptFrameType_t accept_frame_type)
{
    rtk_api_ret_t retVal;

    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if (accept_frame_type >= ACCEPT_FRAME_TYPE_END)
        return RT_ERR_VLAN_ACCEPT_FRAME_TYPE;
    
    retVal = rtl8307h_svlan_portAcceptFrameType_set(port, accept_frame_type);

    return retVal;
}

/* Function Name:
 *      rtk_svlan_portAcceptFrameType_get
 * Description:
 *      Get Service VLAN support frame type
 * Input:
 *      port                                 - Port id
 *      accept_frame_type             - accept frame type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI 
 *      RT_ERR_PORT_ID
 * Note:
 *    The API is used for checking svlan tagged frames.
 *    The accept frame type as following:
 *    ACCEPT_FRAME_TYPE_ALL
 *    ACCEPT_FRAME_TYPE_TAG_ONLY
 *    ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
rtk_api_ret_t rtk_svlan_portAcceptFrameType_get(rtk_port_t port, rtk_vlan_acceptFrameType_t *pAccept_frame_type)
{
    uint32 retVal;
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    if ( NULL == pAccept_frame_type)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8307h_svlan_portAcceptFrameType_get(port, pAccept_frame_type)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_vlan_stagMode_set
 * Description:
 *      Set SVLAN egress tag mode
 * Input:
 *      port                - Port id
 *      tag_mode        - Egress tag mode
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI 
 *      RT_ERR_INPUT
 * Note:
 *    The API is used for setting port vlan egress tag mode
 *    The tag mode as following:
 *    VLAN_TAG_MODE_ORIGINAL
 *    VLAN_TAG_MODE_KEEP_FORMAT
 *    VLAN_TAG_MODE_REAL_KEEP_FORMAT
 */
rtk_api_ret_t rtk_svlan_tagMode_set(rtk_port_t port, rtk_vlan_tagMode_t tag_mode)
{
    rtk_api_ret_t retVal;
#ifndef	EXT_LAB
    rtk_vlan_tagMode_t mode;
#endif

    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if (tag_mode > VLAN_TAG_MODE_END)
        return RT_ERR_INPUT;
    
    retVal = rtl8307h_svlan_portEgressTagMode_set(port, tag_mode, tag_mode);

    return retVal;
}


/* Function Name:
 *      rtk_svlan_tagMode_get
 * Description:
 *      Get SVLAN egress tag mode
 * Input:
 *      port                - Port id
 *      pTag_mode      - Egress tag mode
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI 
 *      RT_ERR_INPUT
 * Note:
 *    The API is used for setting port vlan egress tag mode
 *    The tag mode as following:
 *    VLAN_TAG_MODE_ORIGINAL
 *    VLAN_TAG_MODE_KEEP_FORMAT
 *    VLAN_TAG_MODE_REAL_KEEP_FORMAT
 */
rtk_api_ret_t rtk_svlan_tagMode_get(rtk_port_t port, rtk_vlan_tagMode_t *pTag_mode)
{
    rtk_api_ret_t retVal;
    rtk_vlan_tagMode_t rxu_mode, rxn_mode;
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if ( NULL == pTag_mode )
        return RT_ERR_NULL_POINTER;
    
    if ((retVal = rtl8307h_svlan_portEgressTagMode_get(port, &rxu_mode, &rxn_mode)) != RT_ERR_OK)
        return retVal;
 
    *pTag_mode = rxn_mode;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_svlan_portEgressTagStatus_set
 * Description:
 *      Set egress port SVLAN tag status
 * Input:
 *      port      -  port id
 *      tag_status     - tx with or without SVLAN tag
 * Output:
*       none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      The API sets egree port transimiting pakcet with or without SVLAN tag.
 *      The tag status could be set as following:
 *      TAG_STATUS_WITH_TAG
 *      TAG_STATUS_WITHOUT_TAG
 */
rtk_api_ret_t rtk_svlan_portEgressTagStatus_set(rtk_port_t port, rtk_vlan_txTagStatus_t tag_status)
{
    rtk_api_ret_t retVal;
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if (tag_status > TAG_STATUS_END)
        return RT_ERR_ENABLE;

    retVal = rtl8307h_svlan_portEgressTagStatus_set(port, tag_status, tag_status);    
    
    return retVal;
}

/* Function Name:
 *      rtk_svlan_portEgressTagStatus_get
 * Description:
 *      Get egress port SVLAN tag status
 * Input:
 *      port       -  port id
 * Output:
 *      pTag_status -  the pointer of egress port tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      The API gets egree port transimiting pakcet with or without SVLAN tag.
 *      The tag status could be following values:
 *      TAG_STATUS_WITH_TAG
 *      TAG_STATUS_WITHOUT_TAG
 */
rtk_api_ret_t rtk_svlan_portEgressTagStatus_get(rtk_port_t port, rtk_vlan_txTagStatus_t *pTag_status)
{
    rtk_api_ret_t retVal;

    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if(NULL == pTag_status) 
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8307h_svlan_portEgressTagStatus_get(port, pTag_status, pTag_status)) != RT_ERR_OK)
        return retVal;
    
    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_svlan_tagAware_set
 * Description:
 *      Set ingress port aware Stag VID  or not
 * Input:
 *      port       -  port id
 *      enabled  -  aware or unware
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      (1) If ingress port awares tag VID, for tagged packet, its SVLAN is VID 
 *           in SVLAN tag, for untagged packet, its SVLAN is PVID
 *      (2) If ingress port unawares tag VID, for both tagged packet and untagged packet, 
 *           its SVLAN is PVID.
 */
rtk_api_ret_t rtk_svlan_tagAware_set(rtk_port_t port, rtk_enable_t enabled)
{
    rtk_api_ret_t retVal;    
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if (enabled >= RTK_ENABLE_END)
        return RT_ERR_ENABLE;

    if ((retVal = rtl8307h_svlan_portTagAware_set(port, enabled)) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = rtl8307h_svlan_portPktParserAbility_set(port, enabled)) != RT_ERR_OK)
        return retVal;
    
    return RT_ERR_OK;  
}

/* Function Name:
 *      rtk_svlan_tagAware_get
 * Description:
 *      Get ingress port aware Stag VID  or not
 * Input:
 *      port        -  port id
 * Output:
 *      pEnabled  -  the pointer of aware or unware
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      (1) If ingress port awares tag VID, for tagged packet, its SVLAN is VID 
 *           in VLAN tag, for untagged packet, its SVLAN is PVID
 *      (2) If ingress port unawares tag VID, for both tagged packet and untagged packet, 
 *           its SVLAN is PVID.
 */
rtk_api_ret_t rtk_svlan_tagAware_get(rtk_port_t port, rtk_enable_t *pEnabled)
{
    rtk_api_ret_t retVal;    
    uint32 enabled;
    
    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    if(NULL == pEnabled)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8307h_svlan_portTagAware_get(port, &enabled)) != RT_ERR_OK)
        return retVal;

    *pEnabled = enabled ? ENABLED : DISABLED;
    
    return RT_ERR_OK;    
}

/* Function Name:
 *      rtk_svlan_fwdBaseOVid_set
 * Description:
 *      Configure whether forward basd on outer VID or inner VID.
 * Input:
 *      portId  - port Id
 *      enable  - ENABLED: forward based on outer VID, DISABLED: based on inner VID
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE - 
 *      RT_ERR_PORT_ID - port ID out of range
 *      RT_ERR_SMI
 * Note:
 *      This API is used to configure the forwarding VID for frames received from the specified port. 
 */
rtk_api_ret_t rtk_svlan_fwdBaseOVid_set(rtk_port_t portId, rtk_enable_t enable)
{
    rtk_api_ret_t retVal;    

    if(!IS_VALID_PORT_ID(portId))
        return RT_ERR_PORT_ID;

    if (enable >= RTK_ENABLE_END)
        return RT_ERR_ENABLE;
    
    if ((retVal = rtl8307h_svlan_portFwdBaseOvid_set(portId, enable)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}
/* Function Name:
 *      rtk_svlan_fwdBaseOVid_get
 * Description:
 *      Retrieve whether forward basd on outer VID or inner VID.
 * Input:
 *      portId  - port Id
 * Output:
 *      pEnable  - ENABLED: forward based on outer VID, DISABLED: based on inner VID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - 
 *      RT_ERR_PORT_ID - port ID out of range
 *      RT_ERR_SMI
 * Note:
 *      This API is used to retrieve the forwarding VID for frames received from the specified port. 
 */
rtk_api_ret_t rtk_svlan_fwdBaseOVid_get(rtk_port_t portId, rtk_enable_t* pEnable)
{
    rtk_api_ret_t retVal;    
    uint32 enable;

    if(!IS_VALID_PORT_ID(portId))
        return RT_ERR_PORT_ID;

    if (NULL == pEnable)
        return RT_ERR_NULL_POINTER;
    
    if ((retVal = rtl8307h_svlan_portFwdBaseOvid_get(portId, &enable)) != RT_ERR_OK)
        return retVal;

    *pEnable = (rtk_enable_t)enable;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_mirror_portBased_set
 * Description:
 *      Set port mirror function
 * Input:
 *      mirroring_port              - mirroring port
 *      pMirrored_rx_portmask   - Rx mirrored port mask
 *      pMirrored_tx_portmask   - Tx mirrored port mask
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 *      RT_ERR_PORT_ID
 *      RT_ERR_PORT_MASK
 * Note:
 *      The API is to set mirror function of mirrorring port and Rx/Tx mirrorred ports
 *    
 */
rtk_api_ret_t rtk_mirror_portBased_set(rtk_port_t mirroring_port, rtk_portmask_t *pMirrored_rx_portmask, rtk_portmask_t *pMirrored_tx_portmask)
{
    uint32 regVal;

    if (!IS_VALID_PORT_ID(mirroring_port))
        return RT_ERR_PORT_ID;
    
    if((NULL == pMirrored_rx_portmask) || (NULL == pMirrored_tx_portmask))
        return RT_ERR_NULL_POINTER;
         
    if (pMirrored_rx_portmask->bits[0] > RTK_MAX_PORT_MASK)
        return RT_ERR_PORT_MASK; 

    if (pMirrored_tx_portmask->bits[0] > RTK_MAX_PORT_MASK)
        return RT_ERR_PORT_MASK;

    /*mirroring port != mirrored port*/
    if ((pMirrored_rx_portmask->bits[0] & ((uint32)1 << mirroring_port)) || 
        (pMirrored_tx_portmask->bits[0] & ((uint32)1 << mirroring_port)) )
        return RT_ERR_PORT_MASK;   

    regVal = mirroring_port;

    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL1, DP, regVal));


    regVal = pMirrored_rx_portmask->bits[0];
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL0, SPM, regVal));

    
    regVal = pMirrored_tx_portmask->bits[0];
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL1, DPM, regVal));

    regVal = 0;
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL0, SPMDPMOP, regVal));

    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL1, MORG, 1));
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL0, CROSSVLAN, 1));    
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL0, MUA, 1));  
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL0, MMA, 1));  
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL0, MBA, 1));      
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL0, MBPKT, 1));
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL0, MGPKT, 1));
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL2, MPTKPP, 1));    
        
    return RT_ERR_OK;

}

/* Function Name:
 *      rtk_mirror_portBased_get
 * Description:
 *      Get port mirror function
 * Input:
 *      none
 * Output:
 *      pMirroring_port               - mirroring port
 *      pMirrored_rx_portmask   - Rx mirrored port mask
 *      pMirrored_tx_portmask   - Tx mirrored port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI 
 *      RT_ERR_NULL_POINTER
 * Note:
 *      The API is to get mirror function of mirroring port and Rx/Tx mirrored ports
 *    
 */
rtk_api_ret_t rtk_mirror_portBased_get(rtk_port_t* pMirroring_port, rtk_portmask_t *pMirrored_rx_portmask, rtk_portmask_t *pMirrored_tx_portmask)
{
    rtk_api_ret_t regVal;

    if ((NULL == pMirroring_port) || (NULL == pMirrored_rx_portmask) || (NULL == pMirrored_tx_portmask))
        return RT_ERR_NULL_POINTER;

    *pMirroring_port = 0;
    memset(pMirrored_rx_portmask, 0, sizeof(rtk_portmask_t));
    memset(pMirrored_tx_portmask, 0, sizeof(rtk_portmask_t));

#ifdef	EXT_LAB
	CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL1, DP, (uint32 *)&regVal));
	*pMirroring_port = regVal;
	      
	CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL0, SPM, (uint32 *)&regVal));
	pMirrored_rx_portmask->bits[0] = regVal;

	CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL1, DPM, (uint32 *)&regVal));
	pMirrored_tx_portmask->bits[0] = regVal;
#else
    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL1, DP, &regVal));
    *pMirroring_port = regVal;
          
    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL0, SPM, &regVal));
    pMirrored_rx_portmask->bits[0] = regVal;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, TRAFFIC_MIRROR_CONTROL1, DPM, &regVal));
    pMirrored_tx_portmask->bits[0] = regVal;
#endif
    
    return RT_ERR_OK;

}

/* Function Name:
 *      rtk_mirror_portIso_set
 * Description:
 *      Set mirror port isolation
 * Input:
 *      enable   - Mirror isolation status
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 *      RT_ERR_ENABLE
 * Note:
 *    The API is to set mirror isolation function that prevent normal forwarding packets to mirorring port.
 *    
 */
rtk_api_ret_t rtk_mirror_portIso_set(rtk_enable_t enable)
{
    if( enable >= RTK_ENABLE_END )
        return RT_ERR_ENABLE;
    
   CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, MIRROR_PORT_EGRESS_FILTER_CONTROL, MIREGFILTER, (uint32)enable));                

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_mirror_portIso_get
 * Description:
 *      Get mirror port isolation
 * Input:
 *      none
 * Output:
 *      pEnable   - Mirror isolation status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 *      RT_ERR_ENABLE
 * Note:
 *    The API is to get mirror isolation status.
 *    
 */
rtk_api_ret_t rtk_mirror_portIso_get(rtk_enable_t *pEnable)
{
    if (NULL == pEnable)
        return RT_ERR_NULL_POINTER;
#ifdef	EXT_LAB
	uint32 regVal;

	*pEnable = DISABLED;
	CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, MIRROR_PORT_EGRESS_FILTER_CONTROL, MIREGFILTER, &regVal));
	*pEnable = (rtk_enable_t )regVal;
#else    
    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, MIRROR_PORT_EGRESS_FILTER_CONTROL, MIREGFILTER, (uint32*)pEnable));           
#endif

    return RT_ERR_OK;
}


/* Function Name:
 *      rtk_stat_global_reset
 * Description:
 *     Reset global MIB counter.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_INPUT - Invalid input parameter.
 * Note:
 *      Reset MIB counter of ports. API will use global reset while port mask is all-ports.
 */
#ifndef	EXT_LAB
rtk_api_ret_t rtk_stat_global_reset(void)
{
    rtk_api_ret_t retVal;

    if((retVal = rtl8307h_mib_counter_reset(TRUE, FALSE, PM_PORT_ALL)) != RT_ERR_OK)
        return retVal; 
        
    return RT_ERR_OK;
}
#endif

/* Function Name:
 *      rtk_stat_port_reset
 * Description:
 *     Reset per port MIB counter by port.
 * Input:
 *      port -  port id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_INPUT - Invalid input parameter.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *     Reset MIB counter of ports. .
 */
rtk_api_ret_t rtk_stat_port_reset(rtk_port_t port)
{
    rtk_api_ret_t retVal;

    if(!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID; 
    
    if((retVal = rtl8307h_mib_counter_reset(FALSE, FALSE, (uint32)1 << port)) != RT_ERR_OK)
        return retVal; 
        
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stat_global_get
 * Description:
 *      get global MIB counter.
 * Input:
 *      cntr_idx -  counter index
 * Output:
 *      pCntr -  counter returned
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid input parameter.
 * Note:
 *     Get global MIB counter.
 */
rtk_api_ret_t rtk_stat_global_get(rtk_stat_global_type_t cntr_idx, rtk_stat_counter_t *pCntr)
{
    rtk_api_ret_t retVal;
    uint32 cntrH, cntrL;

    if(cntr_idx != DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX)
        return RT_ERR_STAT_INVALID_GLOBAL_CNTR;

    /*only one global MIB counter*/
    if((retVal = rtl8307h_mib_counter_get(0x83E, &cntrH, &cntrL)) != RT_ERR_OK)
        return retVal;

     *pCntr = (uint64)cntrH << 32 | cntrL;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stat_global_start
 * Description:
 *      Start global MIB counter.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 * Note:
 *     Start global MIB counter.
 */
rtk_api_ret_t rtk_stat_global_start(void)
{
    rtk_api_ret_t retVal;

    if ((retVal = rtl8307h_mib_counter_enable(TRUE)) != RT_ERR_OK)
        return retVal;
        
    if ((retVal = reg_field_write(RTL8307H_UNIT, MIB_COUNTER_CONTROL2, SYSCOUNTERSTART, 1)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stat_global_stop
 * Description:
 *      Stop global MIB counter.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 * Note:
 *     Stop global MIB counter.
 */
rtk_api_ret_t rtk_stat_global_stop(void)
{
    rtk_api_ret_t retVal;

    if ((retVal = reg_field_write(RTL8307H_UNIT, MIB_COUNTER_CONTROL2, SYSCOUNTERSTART, 0)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stat_port_start
 * Description:
 *      Start port-based MIB counter.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_PORT_ID   - port ID out of range
 * Note:
 *     Start specified port MIB counter.
 */
rtk_api_ret_t rtk_stat_port_start(rtk_port_t port)
{
    rtk_api_ret_t retVal;
    uint32 regVal;

    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if ((retVal = rtl8307h_mib_counter_enable(TRUE)) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg_read(RTL8307H_UNIT, MIB_COUNTER_CONTROL2, &regVal)) != SUCCESS)
        return retVal;

    regVal |= (1 << port);
    
    if ((retVal = reg_write(RTL8307H_UNIT, MIB_COUNTER_CONTROL2, regVal)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stat_port_stop
 * Description:
 *      Stop port-based MIB counter.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_PORT_ID   - port ID out of range
 * Note:
 *     Stop specified port MIB counter.
 */
rtk_api_ret_t rtk_stat_port_stop(rtk_port_t port)
{
    rtk_api_ret_t retVal;
    uint32 regVal;

    if (!IS_VALID_PORT_ID(port))
    	{
        return RT_ERR_PORT_ID;
    	}
	
    if ((retVal = reg_read(RTL8307H_UNIT, MIB_COUNTER_CONTROL2, (uint32 *)&regVal)) != SUCCESS)
    	{
        return retVal;
    	}

        regVal &= ~(1 << port);
    
    if ((retVal = reg_write(RTL8307H_UNIT, MIB_COUNTER_CONTROL2, regVal)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_stat_port_get
 * Description:
 *      Get port MIB counter.
 * Input:
 *      port -  port id.
 *      cntr_idx -  counter index.
 * Output:
 *      pCntr -  counter returned.
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_INPUT - Invalid input parameter.
 * Note:
 *     Get port MIB counter.
 */
rtk_api_ret_t rtk_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntr_idx, rtk_stat_counter_t *pCntr)
{
    rtk_api_ret_t retVal;
    uint32 cntrH, cntrL;
    uint16 mibAddr;

    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID; 
    
    if(cntr_idx>=STAT_MIB_PORT_CNTR_END)
        return RT_ERR_STAT_INVALID_PORT_CNTR;

    if (pCntr == NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtl8307h_mib_portCntAddr_get(cntr_idx, port, &mibAddr)) != RT_ERR_OK)
        return retVal;

    if((retVal=rtl8307h_mib_counter_get(mibAddr, &cntrH, &cntrL)) != RT_ERR_OK)
        return retVal;

    *pCntr = (uint64)cntrH << 32 | cntrL;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_mib_enable
 * Description:
 *      Enable MIB counter.
 * Input:
 *      None
 * Output:
 *      None.
 * Return:
 *      None
 * Note:
 *     
 */
void rtk_mib_enable(void)
{
    uint32 regVal;
 
    reg_read(RTL8307H_UNIT, MIB_COUNTER_CONTROL0, &regVal);
    regVal |= 0x1;
    reg_write(RTL8307H_UNIT, MIB_COUNTER_CONTROL0, regVal);

    reg_read(RTL8307H_UNIT, MIB_COUNTER_CONTROL2, &regVal);
    regVal |= 0xff;
    reg_write(RTL8307H_UNIT, MIB_COUNTER_CONTROL2, regVal);
}

/* Function Name:
 *      rtk_port_packet_received_get
 * Description:
 *      Get the number of received packets on port port_id.
 * Input:
 *      port -  port id.
 * Output:
 *      pCntr -  received packets.
 * Return:
 *      RT_ERR_OK  - 
 * Note:
 *     
 */
rtk_api_ret_t rtk_port_packet_received_get(int port_id, rtk_stat_counter_t *pCntr)
{
    rtk_stat_counter_t total, cnt;

    total = 0;
    
    rtk_stat_port_get(port_id, STAT_EtherStatsUnderSizePkts, &cnt);
    total += cnt;
    
    rtk_stat_port_get(port_id, STAT_EtherOversizeStats, &cnt);
    total += cnt;

    rtk_stat_port_get(port_id, STAT_EtherStatsPkts64Octets, &cnt);
    total += cnt;

    rtk_stat_port_get(port_id, STAT_EtherStatsPkts65to127Octets, &cnt);
    total += cnt;

    rtk_stat_port_get(port_id, STAT_EtherStatsPkts128to255Octets, &cnt);
    total += cnt;

    rtk_stat_port_get(port_id, STAT_EtherStatsPkts256to511Octets, &cnt);
    total += cnt;

    rtk_stat_port_get(port_id, STAT_EtherStatsPkts512to1023Octets, &cnt);
    total += cnt;

    rtk_stat_port_get(port_id, STAT_EtherStatsPkts1024to1518Octets, &cnt);
    total += cnt;

    *pCntr = total;
    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_port_packet_transmitted_get
 * Description:
 *      Get the number of transmitted packets on port port_id.
 * Input:
 *      port -  port id.
 * Output:
 *      pCntr -  transmitted packets.
 * Return:
 *      RT_ERR_OK  - 
 * Note:
 *     
 */
rtk_api_ret_t rtk_port_packet_transmitted_get(int port_id, rtk_stat_counter_t *pCntr)
{
    rtk_stat_counter_t total, cnt;

    total = 0;
    
    rtk_stat_port_get(port_id, STAT_IfOutUcastPkts, &cnt);
    total += cnt;
    
    rtk_stat_port_get(port_id, STAT_IfOutMulticastPkts, &cnt);
    total += cnt;

    rtk_stat_port_get(port_id, STAT_IfOutBroadcastPkts, &cnt);
    total += cnt;

    *pCntr = total;
    return RT_ERR_OK;
} 

/* Function Name:
 *      rtk_port_packet_error_get
 * Description:
 *      Get the number of packets with error on port port_id.
 * Input:
 *      port -  port id.
 * Output:
 *      pCntr -  error packets.
 * Return:
 *      RT_ERR_OK  - 
 * Note:
 *     
 */
rtk_api_ret_t rtk_port_packet_error_get(int port_id, rtk_stat_counter_t *pCntr)
{
    rtk_stat_counter_t total, cnt;

    total = 0;
    
    rtk_stat_port_get(port_id, STAT_Dot3StatsFCSErrors, &cnt);
    total += cnt;
    
    rtk_stat_port_get(port_id, STAT_Dot3StatsSymbolErrors, &cnt);
    total += cnt;

    rtk_stat_port_get(port_id, STAT_EtherStatsFragments, &cnt);
    total += cnt;
    
    rtk_stat_port_get(port_id, STAT_EtherStatsJabbers, &cnt);
    total += cnt;

    *pCntr = total;
    return RT_ERR_OK;
} 

 


