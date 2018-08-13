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
 * $Revision: 6658 $
 * $Date: 2009-10-30 14:49:58 +0800 (Fri, 30 Oct 2009) $
 *
 * Purpose : RTL8307H switch low-level API for dot1x
 * Feature : 
 *
 */
#include <rtk_types.h>
#include <rtk_error.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>

#include <rtl8307h_asicdrv_dot1x.h>


int32 rtl8307h_dot1x_unauthPktAct_set(rtl8307h_dot1xPktType_t pktType, rtk_dot1x_unauth_action_t action)
{
    if ((pktType >= RTL8307H_DOT1X_PKT_END) || (action >= DOT1X_ACTION_END))
        return RT_ERR_INPUT;

    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, GUEST_VLAN_GLOBAL_CONTROL, INTAGDOT1XUNAUTHBH + pktType, action));

    return RT_ERR_OK;
}

int32 rtl8307h_dot1x_unauthPktAct_get(rtl8307h_dot1xPktType_t pktType, rtk_dot1x_unauth_action_t *pAction)
{
#ifdef	EXT_LAB
	uint32 regVal;

	if (pktType >= RTL8307H_DOT1X_PKT_END)
		return RT_ERR_INPUT;

	if (NULL == pAction)
		return RT_ERR_NULL_POINTER;

	*pAction = DOT1X_ACTION_DROP;
	CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, GUEST_VLAN_GLOBAL_CONTROL, INTAGDOT1XUNAUTHBH + pktType, &regVal));
	*pAction = (rtk_dot1x_unauth_action_t)regVal;
	
#else
    uint32 fieldVal;
    
    if (pktType >= RTL8307H_DOT1X_PKT_END)
        return RT_ERR_INPUT;

    if (NULL == pAction)
        return RT_ERR_NULL_POINTER;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, GUEST_VLAN_GLOBAL_CONTROL, INTAGDOT1XUNAUTHBH + pktType, (uint32 *)pAction));
#endif

    return RT_ERR_OK;
}

