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
 * $Revision: 8184 $
 * $Date: 2010-01-21 11:07:42 +0800 (Thu, 21 Jan 2010) $
 *
 * Purpose : RTL8307H switch high-level API for RTL8367B
 * Feature : 
 *
 */
#include <rtk_types.h>
#include <rtk_api.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>

#include <rtl8307h_asicdrv_igmp.h>

/* Function Name:
 *      rtl8307h_igmp_asicIgmpEnable_set
 * Description:
 *      set igmp function enabled or disabled    
 * Input:
 *      enabled -the value should be 0x0 or 0x1
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can enable or disable igmp function. 
 */
ret_t rtl8307h_igmp_asicIgmpEnable_set( uint32 enabled)
{
    if(enabled > 0x1)
        return RT_ERR_ENABLE;
    enabled = (enabled) ? 0 : 1;
    
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, IGMP_CONTROL, IGMP_DIS, enabled))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicIgmpEnable_get
 * Description:
 *      check igmp function enabled or disabled    
 * Input:
 *      none
 * Output:
 *      enabled -the value should be 0x0 or 0x1
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED
 * Note:
 *      This API can check igmp function enabled or disabled.
 */
ret_t rtl8307h_igmp_asicIgmpEnable_get( uint32* enabled)
{
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, IGMP_CONTROL, IGMP_DIS, enabled))
    {
        *enabled = (*enabled) ? 0 : 1;
        return RT_ERR_OK;
    }
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicMldEnable_set
 * Description:
 *      set mld function enabled or disabled    
 * Input:
 *      enabled -the value should be 0x0 or 0x1
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can enable or disable mld function. 
 */
ret_t rtl8307h_igmp_asicMldEnable_set( uint32 enabled)
{
    if(enabled > 0x1)
        return RT_ERR_ENABLE;
    enabled = (enabled)?0:1;
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, IGMP_CONTROL, MLD_DIS, enabled))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicMldEnable_get
 * Description:
 *      check mld function enabled or disabled    
 * Input:
 *      none
 * Output:
 *      enabled -the value should be 0x0 or 0x1
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED
 * Note:
 *      This API can check mld function enabled or disabled.
 */
ret_t rtl8307h_igmp_asicMldEnable_get( uint32* enabled)
{
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, IGMP_CONTROL, MLD_DIS, enabled))
    {
        *enabled = (*enabled) ? 0 : 1;
        return RT_ERR_OK;
    }
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicIpMulticastVlanLeaky_set
 * Description:
 *      set cross vlan function enabled or disabled    
 * Input:
 *      enabled -the value should be 0x0 or 0x1
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE  
 *      RT_ERR_FAILED
 * Note:
 *      The API can set the IP multicast VLAN Leaky function. When enabling this function, 
 *      if the lookup result(forwarding portmap) of IP Multicast packet is over VLAN boundary, 
 *      the packet can be forwarded across VLAN.
*/
ret_t rtl8307h_igmp_asicIpMulticastVlanLeaky_set( uint32 enabled)
{
    if(enabled > 0x1)
        return RT_ERR_ENABLE;   
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, IGMP_CONTROL, CROSS_VLAN, enabled))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicIpMulticastVlanLeaky_get
 * Description:
 *      check cross vlan function enabled or disabled    
 * Input:
 *      none
 * Output:
 *      enabled -the value should be 0x0 or 0x1
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED
 * Note:
 *      The API can check  IP multicast VLAN Leaky function.
*/
ret_t rtl8307h_igmp_asicIpMulticastVlanLeaky_get( uint32* enabled)
{
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, IGMP_CONTROL, CROSS_VLAN, enabled))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicFastLeaveEnable_set
 * Description:
 *      set fast leave function enabled or disabled    
 * Input:
 *      enabled -the value should be 0x0 or 0x1
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can set fast leave enabled or disabled. 
*/
ret_t rtl8307h_igmp_asicFastLeaveEnable_set( uint32 enabled)
{
    if(enabled > 0x1)
        return RT_ERR_ENABLE;
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, IGMP_CONTROL, FAST_LEAVE_EN, enabled))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicFastLeaveEnable_get
 * Description:
 *      check fast leave function enabled or disabled    
 * Input:
 *      none
 * Output:
 *      enabled -the value should be 0x0 or 0x1
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED
 * Note:
 *      This API can check fast leave enabled or disabled. 
*/
ret_t rtl8307h_igmp_asicFastLeaveEnable_get( uint32* enabled)
{
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, IGMP_CONTROL, FAST_LEAVE_EN, enabled))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicIgmpCpuPortMask_set
 * Description:
 *      set igmp cpu port mask  
 * Input:
 *      portMsk  -  igmp cpu port mask, the value should be 0x0~0x7
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can set the cpu port mask when igmp packet trapped to cpu
 */
ret_t rtl8307h_igmp_asicIgmpCpuPortMask_set( uint32 portMsk)
{
    if(portMsk > 0x7)
        return RT_ERR_OUT_OF_RANGE;
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, IGMP_MLD_CONTROL, IGMPCPUMSK, portMsk))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicIgmpCpuPortMask_get
 * Description:
 *      get igmp cpu port mask     
 * Input:
 *      none
 * Output:
 *      portMsk  -  igmp cpu port mask, the value should be 0x0~0x7
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can get the cpu port mask when igmp packet trapped to cpu
 */
ret_t rtl8307h_igmp_asicIgmpCpuPortMask_get( uint32* portMsk)
{
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, IGMP_MLD_CONTROL, IGMPCPUMSK, portMsk))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicIgmpTrap_set
 * Description:
 *      set the action on igmp pkt
 * Input:
 *      action  - the  action on igmp pkt
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can set the action on igmp pkt;
 *      Input should be:
 *      0x0       Forward
 *      0x1       Trap(to IGMP cpu port mask )
 *      0x2       Copy to cpu
 *      0x3       Drop
 */
ret_t rtl8307h_igmp_asicIgmpTrap_set( rtk_trap_misc_action_t action)
{
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, IGMP_MLD_CONTROL, IGMPTRAP, action))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicIgmpTrap_get
 * Description:
 *      get the action on igmp pkt    
 * Input:
 *      none
 * Output:
 *      action  - the  action on igmp pkt
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can get the action on igmp pkt
 *      Output should be:
 *      0x0       Forward
 *      0x1       Trap(to IGMP cpu port mask )
 *      0x2       Copy to cpu
 *      0x3       Drop
 */
ret_t rtl8307h_igmp_asicIgmpTrap_get( rtk_trap_misc_action_t* action)
{
#ifdef	MUX_LAB
	uint32 regVal;

	*action = MISC_ACTION_FORWARD;
	if( SUCCESS == reg_field_read(RTL8307H_UNIT, IGMP_MLD_CONTROL, IGMPTRAP, &regVal))
	{
		*action = (rtk_trap_misc_action_t)regVal;
		return RT_ERR_OK;
	}
	else
		return RT_ERR_FAILED;
#else
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, IGMP_MLD_CONTROL, IGMPTRAP, action))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
#endif
}
/* Function Name:
 *      rtl8307h_igmp_asicMldTrap_set
 * Description:
 *      set the action on mld pkt
 * Input:
 *      action  - the  action on mld pkt
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can set the action on mld pkt;
 *      Input should be:
 *      0x0       Forward
 *      0x1       Trap(to IGMP cpu port mask )
 *      0x2       Copy to cpu
 *      0x3       Drop
 */
ret_t rtl8307h_igmp_asicMldTrap_set( rtk_trap_misc_action_t action)
{
#ifdef	MUX_LAB
	if( SUCCESS == reg_field_write(RTL8307H_UNIT, IGMP_MLD_CONTROL, MLDTRAP, (uint32)action))
	{
		return RT_ERR_OK;       
	}
	else
		return RT_ERR_FAILED;
#else
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, IGMP_MLD_CONTROL, MLDTRAP, action))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
#endif		
}
/* Function Name:
 *      rtl8307h_igmp_asicMldTrap_get
 * Description:
 *      get the action on mld pkt    
 * Input:
 *      none
 * Output:
 *      action  - the  action on mld pkt
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can get the action on mld pkt
 *      Output should be:
 *      0x0       Forward
 *      0x1       Trap(to IGMP cpu port mask )
 *      0x2       Copy to cpu
 *      0x3       Drop
 */
ret_t rtl8307h_igmp_asicMldTrap_get( rtk_trap_misc_action_t* action)
{
#ifdef	MUX_LAB
	uint32 regVal;

	*action= MISC_ACTION_FORWARD;
	if( SUCCESS == reg_field_read(RTL8307H_UNIT, IGMP_MLD_CONTROL, MLDTRAP, &regVal))
	{
		*action= (rtk_trap_misc_action_t)regVal;
		return RT_ERR_OK;
	}
	else
		return RT_ERR_FAILED;
#else
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, IGMP_MLD_CONTROL, MLDTRAP, action))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
#endif		
}
/* Function Name:
 *      rtl8307h_igmp_asicDefPri_set
 * Description:
 *      Define priority or not when IGMP/MLD packets trapped to CPU    
 * Input:
 *      enabled  -  define priority or not, the value should be 0x0 or 0x1
 * Output:
 *     none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can decide that define priority or not when IGMP/MLD packets trapped to CPU
 */
ret_t rtl8307h_igmp_asicDefPri_set( uint32 enabled)
{
    if(enabled > 0x1)
        return RT_ERR_ENABLE;
    
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, IGMP_MLD_CONTROL, DFIGMPPRI, enabled))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicDefPri_get
 * Description:
 *      check the priority defined or not when IGMP/MLD packets trapped to CPU    
 * Input:
 *      none
 * Output:
 *      enabled  -  define priority or not,  the value should be 0x0 or 0x1
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can check that priority defined or not  when IGMP/MLD packets trapped to CPU
 */
ret_t rtl8307h_igmp_asicDefPri_get( uint32* enabled)
{    
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, IGMP_MLD_CONTROL, DFIGMPPRI, enabled))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicPri_set
 * Description:
 *      Define the priority  when IGMP/MLD packets trapped to CPU    
 * Input:
 *      igmpPri  -  priority defined, the value should be 0x0~0x7
 * Output:
 *     none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can define the priority  when IGMP/MLD packets trapped to CPU
 */
ret_t rtl8307h_igmp_asicPri_set( uint32 igmpPri)
{
    if(igmpPri > 0x7)
        return RT_ERR_OUT_OF_RANGE;
    
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, IGMP_MLD_CONTROL, IGMPPRI, igmpPri))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_asicPri_get
 * Description:
 *      get the priority  when IGMP/MLD packets trapped to CPU    
 * Input:
 *      none
 * Output:
 *      igmpPri  -  priority defined, the value should be 0x0~0x7
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can get the priority  when IGMP/MLD packets trapped to CPU
 */
ret_t rtl8307h_igmp_asicPri_get( uint32* igmpPri)
{    
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, IGMP_MLD_CONTROL, IGMPPRI, igmpPri))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/* Function Name:
 *      rtl8307h_igmp_paraChanged_reset
 * Description:
 *      hold cpu, reset nic, release cpu   
 * Input:
 *      none
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK  
 *      RT_ERR_FAILED
 * Note:
 *      This API can reset 8051 code when igmp parameters changed.
 */
#ifdef	MUX_LAB
ret_t rtl8307h_igmp_paraChanged_reset(void)
#else
ret_t rtl8307h_igmp_paraChanged_reset()
#endif
{
    if( SUCCESS != reg_write(RTL8307H_UNIT, CPU_8051_RESET, 0x2))
        return RT_ERR_FAILED;
    
    if( SUCCESS != reg_write(RTL8307H_UNIT, NIC_GLOBAL_CONTROL, 0x1))
        return RT_ERR_FAILED;
    
    if( SUCCESS != reg_write(RTL8307H_UNIT, CPU_8051_RESET, 0x0))
        return RT_ERR_FAILED;

    return RT_ERR_OK;        
}


