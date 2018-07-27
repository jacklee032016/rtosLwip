/*
* Copyright (C) 2009 Realtek Semiconductor Corp.
* All Rights Reserved.
*
* This program is the proprietary software of Realtek Semiconductor
* Corporation and/or its licensors, and only be used, duplicated,
* modified or distributed under the authorized license from Realtek.
*
* ANY USE OF THE SOFTWARE OTEHR THAN AS AUTHORIZED UNDER 
* THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
* 
* $Revision: 12570 $
* $Date: 2010-09-08 14:14:31 +0800 (星期三, 08 九月 2010) $
*
* Purpose :  ASIC-level driver implementation for port property..
*
*  Feature :  This file consists of following modules:
*             1) 
*
*/
#include <rtk_types.h>
#include <rtk_error.h>
#include <rtk_api.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>

#include <rtl8307h_asicdrv_port.h>

int32 rtl8307h_iso_mask_get(uint32 port, uint32 *pMask)
{
    uint32 retVal;

    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
     
    if (NULL == pMask)
        return RT_ERR_NULL_POINTER;

    if((retVal=  reg_field_read(RTL8307H_UNIT, port + PORT0_PORT_ISOLATION_CONTROL, TISO, pMask)) != RT_ERR_OK)
        return retVal;
    
    return RT_ERR_OK;
}

int32 rtl8307h_iso_mask_set(uint32 port, uint32 mask)
{
    uint32 retVal;

    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if((retVal = reg_field_write(RTL8307H_UNIT, port + PORT0_PORT_ISOLATION_CONTROL, TISO, mask)) != RT_ERR_OK)
        return retVal;                     
    
    return RT_ERR_OK;
}

rtk_api_ret_t rtl8307h_mac6_miiMode_set(rtk_mode_ext_t mode)
{
#if 0
    uint32 regVal;
    uint32 chipType;
    uint32 ifType;
        
    if(mode >= MODE_EXT_END)
        return RT_ERR_INPUT;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, CHIP_TYPE_2_0, &chipType));
    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, MAC6_MII_2_0, &ifType));


    if ((0x7 != chipType) || (0x7 == ifType) || (0x6 == ifType))
        return RT_ERR_CHIP_NOT_SUPPORTED;
    
    switch (mode)
    {
        case MODE_EXT_TMII_MII_MAC:
            regVal = 5;
            break;

        case MODE_EXT_MII_PHY:
            regVal = 4;
            break;
            
        case MODE_EXT_TMII_PHY:
            regVal = 3;
            break;

        case MODE_EXT_RMII_INPUT:
            regVal = 2;
            break;

        case MODE_EXT_RMII_OUTPUT:
            regVal = 1;
            break;
        default:
            return RT_ERR_INPUT;    
    }

    reg_field_write(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, MAC6_MII_2_0, regVal);
#endif
    return RT_ERR_OK;    
}

rtk_api_ret_t rtl8307h_mac6_miiMode_get(rtk_mode_ext_t* mode)
{
    uint32 ifType;
        
    if(NULL == mode)
        return RT_ERR_NULL_POINTER;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, MAC6_MII_2_0, &ifType));

    switch (ifType)
    {
        case 7:
            *mode = MODE_EXT_FAST_ETHERNET;
            break;
        case 6:
            *mode = MODE_EXT_HEAC;
            break;
        case 5:
            *mode = MODE_EXT_TMII_MII_MAC;
            break;
        case 4:
            *mode = MODE_EXT_MII_PHY;
            break;
        case 3:
            *mode = MODE_EXT_TMII_PHY;
            break;
        case 2:
            *mode = MODE_EXT_RMII_INPUT;
            break;
        case 1:
            *mode = MODE_EXT_RMII_OUTPUT;
            break;
        default:
            *mode = MODE_EXT_END;
            return RT_ERR_INPUT;    
    }

    return RT_ERR_OK;    
}

rtk_api_ret_t rtl8307h_mac5_miiMode_set(rtk_mode_ext_t mode)
{
#if 0        
    uint32 regVal;
    uint32 chipType;
    uint32 ifType;
    if(mode >= MODE_EXT_END)
        return RT_ERR_INPUT;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, CHIP_TYPE_2_0, &chipType));
    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, MAC5_RGMII_2_0, &ifType));

    if (0x7 != chipType)
        return RT_ERR_CHIP_NOT_SUPPORTED;
    
    switch (mode)
    {
        case MODE_EXT_RGMII_NODELAY:
            regVal = 0;
            break;

        case MODE_EXT_RGMII_RXDELAY_ONLY:
            regVal = 1;
            break;
            
        case MODE_EXT_RGMII_TXDELAY_ONLY:
            regVal = 2;
            break;

        case MODE_EXT_RGMII_RXTXDELAY:
            regVal = 3;
            break;
            
        case MODE_EXT_TMII_MII_MAC:
            regVal = 4;
            break;

        case MODE_EXT_MII_PHY:
            regVal = 5;
            break;
            
        case MODE_EXT_TMII_PHY:
            regVal = 6;
            break;

        case MODE_EXT_RMII_INPUT:
            regVal = 7;
            reg_field_write(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, MAC5_DIRREFCLK, 1);
            break;

        case MODE_EXT_RMII_OUTPUT:
            regVal = 7;
            reg_field_write(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, MAC5_DIRREFCLK, 0);
            break;
            
        default:
            return RT_ERR_INPUT;    
    }

    reg_field_write(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, MAC5_RGMII_2_0, regVal);
#endif
    return RT_ERR_OK;    
}

rtk_api_ret_t rtl8307h_mac5_miiMode_get(rtk_mode_ext_t* mode)
{
    uint32 chipType;
    uint32 ifType;
    uint32 regVal;
        
    if(NULL == mode)
        return RT_ERR_NULL_POINTER;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, CHIP_TYPE_2_0, &chipType));
    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, MAC5_RGMII_2_0, &ifType));

    if (0x7 != chipType)
    {
    	printf("chipType:%x, not suport", chipType );
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    
    switch (ifType)
    {    
        case 0:
            *mode = MODE_EXT_RGMII_NODELAY;
            break;
        case 1:
            *mode = MODE_EXT_RGMII_RXDELAY_ONLY;
            break;
        case 2:
            *mode = MODE_EXT_RGMII_TXDELAY_ONLY;
            break;
        case 3:
            *mode = MODE_EXT_RGMII_RXTXDELAY;
            break;
        case 4:
            *mode = MODE_EXT_TMII_MII_MAC;
            break;
        case 5:
            *mode = MODE_EXT_MII_PHY;
            break;
        case 6:
            *mode = MODE_EXT_TMII_PHY;
            break;
        case 7:
            reg_field_read(RTL8307H_UNIT, GLOBAL_MAC_INTERFACE_CONTROL0, MAC5_DIRREFCLK, &regVal);
            if (regVal == 1)
                *mode = MODE_EXT_RMII_INPUT;
            else
                *mode = MODE_EXT_RMII_OUTPUT; 
            break;
        default:
            *mode = MODE_EXT_END;
            return RT_ERR_INPUT;    
    }

    return RT_ERR_OK;    
}
