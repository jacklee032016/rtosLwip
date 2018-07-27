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
 * $Revision: 8188 $
 * $Date: 2010-01-21 15:11:17 +0800 (Thu, 21 Jan 2010) $
 *
 * Purpose : RTL8307H switch high-level API for RTL8307H
 * Feature : 
 *
 */

#include <rtk_types.h>
#include <rtk_error.h>
#include <rtk_api.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>
#include <rtl8307h_debug.h>

#include <rtl8307h_asicdrv_phy.h>

#define PHY_ACCESS  (0)
#define MMD_ACCESS  (1)


static rtk_api_ret_t wait_op_finished(void)
{
    uint32 reg_val;
    uint16 cnt;
    
    cnt = 0;
    while(cnt++ < RTL8307H_MAX_POLLCNT)
    {
        reg_read(RTL8307H_UNIT, PHY_REG_ACCESS_CONTROL0, &reg_val);
        
        if ((reg_val & 0x01)== 0)
            break;
    }

    if(cnt == RTL8307H_MAX_POLLCNT)
    {
        DEBUG_INFO(DBG_PHY, "Time out: phy access\n");
        return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    return RT_ERR_OK;
}

rtk_api_ret_t phy_reg_read(uint8 phyaddr, uint8 page, uint8 regaddr, uint16* reg_data)
{
    rtk_api_ret_t ret;
    uint32 reg_val;
    uint8 phy_type = PHY_ACCESS;

    *reg_data = 0;        
    if ((ret = wait_op_finished()) != RT_ERR_OK)
        return ret;
    
    reg_val = ((uint16)phyaddr << 13) | ((uint16)regaddr << 8) | (page << 3) | (phy_type << 2) | 0x3;
    reg_write(RTL8307H_UNIT, PHY_REG_ACCESS_CONTROL0, reg_val);
                
    if ((ret = wait_op_finished()) != RT_ERR_OK)
        return ret;
   
    reg_read(RTL8307H_UNIT, PHY_REG_ACCESS_CONTROL0, &reg_val);
        
    *reg_data = (reg_val >> 16) & 0xffff;
    
     return RT_ERR_OK;
}

rtk_api_ret_t phy_reg_write(uint8 phyaddr, uint8 page, uint8 regaddr, uint16 reg_data)
{
    rtk_api_ret_t ret;
    uint32 reg_val;
    uint8 phy_type = PHY_ACCESS;
    
    if ((ret = wait_op_finished()) != RT_ERR_OK)
        return ret;

    reg_val = ((uint32)reg_data << 16) | ((uint16)phyaddr << 13) | ((uint16)regaddr << 8) | (page << 3) | (phy_type << 2) | 0x1;
    reg_write(RTL8307H_UNIT, PHY_REG_ACCESS_CONTROL0, reg_val);
     
    if ((ret = wait_op_finished()) != RT_ERR_OK)
        return ret;
        
     return RT_ERR_OK;
}

rtk_api_ret_t mmd_reg_read(uint8 phyaddr, uint8 device_id, uint16 regaddr, uint16* reg_data)
{
    rtk_api_ret_t ret;
    uint32 reg_val;
    uint8 phy_type = MMD_ACCESS;
    
    *reg_data = 0;        
    if ((ret = wait_op_finished()) != RT_ERR_OK)
        return ret;

    reg_write(RTL8307H_UNIT, PHY_REG_ACCESS_CONTROL1, regaddr);

    reg_val = ((uint16)phyaddr << 13) | ((uint16)device_id << 8) | (phy_type << 2) | (0x1f << 3) |  0x3;
    reg_write(RTL8307H_UNIT, PHY_REG_ACCESS_CONTROL0, reg_val);
                
    if ((ret = wait_op_finished()) != RT_ERR_OK)
        return ret;
   
    reg_read(RTL8307H_UNIT, PHY_REG_ACCESS_CONTROL0, &reg_val);
        
    *reg_data = (reg_val >> 16) & 0xffff;
    
     return RT_ERR_OK;
}

rtk_api_ret_t mmd_reg_write(uint8 phyaddr, uint8 device_id, uint16 regaddr, uint16 reg_data)
{
    rtk_api_ret_t ret;
    uint32 reg_val;
    uint8 phy_type = MMD_ACCESS;
    
    if ((ret = wait_op_finished()) != RT_ERR_OK)
        return ret;

    reg_write(RTL8307H_UNIT, PHY_REG_ACCESS_CONTROL1, regaddr);
    
    reg_val = ((uint32)reg_data << 16) | ((uint16)phyaddr << 13) | ((uint16)device_id << 8) | (phy_type << 2) | (0x1f << 3) | 0x1;
    reg_write(RTL8307H_UNIT, PHY_REG_ACCESS_CONTROL0, reg_val);
     
    if ((ret = wait_op_finished()) != RT_ERR_OK)
        return ret;
        
     return RT_ERR_OK;
}

rtk_api_ret_t giga_phy_ext_read(uint8 page, uint8 regaddr, uint16* reg_data)
{
    uint16 pagesel;   

    /* save old page sel */
    phy_reg_read(PN_PORT4, 0, 31, &pagesel);

    phy_reg_write(PN_PORT4, 0, 31, 7);
    phy_reg_write(PN_PORT4, 31, 30, page);

    phy_reg_read(PN_PORT4, 31, regaddr, reg_data);

    /* restore page sel */
    phy_reg_write(PN_PORT4, 0, 31, pagesel);
	return 0;
}

rtk_api_ret_t giga_phy_ext_write(uint8 page, uint8 regaddr, uint16 reg_data)
{
    uint16 pagesel; 

    /* save old page sel */    
    phy_reg_read(PN_PORT4, 0, 31, &pagesel);

    phy_reg_write(PN_PORT4, 0, 31, 7);
    phy_reg_write(PN_PORT4, 31, 30, page);

    phy_reg_write(PN_PORT4, 31, regaddr, reg_data); 

    /* restore page sel */
    phy_reg_write(PN_PORT4, 0, 31, pagesel);

	return 0;
}



