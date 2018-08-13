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
 * $Revision: 8252 $
 * $Date: 2010-01-29 14:04:02 +0800 (Fri, 29 Jan 2010) $
 *
 * Purpose : RTL8307H switch high-level API for RTL8367B
 * Feature : 
 *
 */

#include <rtk_types.h>
#include <rtk_error.h>
#include <rtk_api.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>

#include <rtl8307h_asicdrv_misc.h>

/*
@func ret_t | rtl8307h_arp_asicDefPri_set | Define Priority or not when ARP packets trapped to CPU
@parm uint32 | enabled | enable Define Priority or not
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_ENABLE | Invalid enable parameter.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can define priority or not when ARP packets trapped to CPU. 
*/
ret_t rtl8307h_arp_asicDefPri_set( uint32 enabled)
{
    if(enabled > 0x1)
        return RT_ERR_ENABLE;
    
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, ARP_CONTROL, DFARPPRI, enabled))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_arp_asicDefPri_get | Define Priority or not when ARP packets trapped to CPU
@parm uint32* | enabled | check Define Priority enabled or not
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can check define priority enabled or not when ARP packets trapped to CPU. 
*/
ret_t rtl8307h_arp_asicDefPri_get( uint32* enabled)
{    
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, ARP_CONTROL, DFARPPRI, enabled))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_arp_asicPri_set | Define Priority
@parm uint32 | arpPri |  Define Priority
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_OUT_OF_RANGE | Parameter out of range.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can define the  packet's priority when ARP packets trapped to CPU. 
*/
ret_t rtl8307h_arp_asicPri_set( uint32 arpPri)
{
    if(arpPri > 0x7)
        return RT_ERR_OUT_OF_RANGE;
    
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, ARP_CONTROL, ARPPRI, arpPri))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_arp_asicPri_get | get the defined priority
@parm uint32* | arpPri |the defined priority
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can get ARP packets' priority. 
*/
ret_t rtl8307h_arp_asicPri_get( uint32* arpPri)
{    
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, ARP_CONTROL, ARPPRI, arpPri))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}

/*
@func ret_t | rtl8307h_arp_asicArpCpuPortMask_set | Set ARP CPU PortMask function
@parm uint32 | portMsk | ARP  CPU PortMask
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can set ARP CPU PortMask. 
*/
ret_t rtl8307h_arp_asicArpCpuPortMask_set( uint32 portMsk)
{
    if(portMsk > 0x7)
        return RT_ERR_OUT_OF_RANGE;
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, ARP_CONTROL, ARPCPUMSK, portMsk))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_arp_asicArpCpuPortMask_get | Get ARP CPU PortMask
@parm uint32* | portMsk |  ARP CPU PortMask
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can get  ARP CPU PortMask. 
*/
ret_t rtl8307h_arp_asicArpCpuPortMask_get( uint32* portMsk)
{
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, ARP_CONTROL, ARPCPUMSK, portMsk))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_arp_asicArpTrap_set | Set ARP Packets trapped to CPU function
@parm rtl8307h_miscTrap_t | action | ARP Packets trapped to CPU or not
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can set ARP Packets trapped to CPU or not
*/
ret_t rtl8307h_arp_asicArpTrap_set( rtk_trap_misc_action_t action)
{
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, ARP_CONTROL, ARPTRAP, action))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_arp_asicArpTrap_get | Check ARP Packets trapped to CPU function
@parm rtl8307h_miscTrap_t* | action |  ARP Packets trapped to CPU or not
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can check  ARP Packets trapped to CPU or not.
*/
ret_t rtl8307h_arp_asicArpTrap_get( rtk_trap_misc_action_t* action)
{
#ifdef	EXT_LAB
	uint32 regVal;

	*action = MISC_ACTION_FORWARD;
	if( SUCCESS == reg_field_read(RTL8307H_UNIT, ARP_CONTROL, ARPTRAP, &regVal))
	{
		*action = (rtk_trap_misc_action_t)regVal;
		return RT_ERR_OK;
	}
	else
		return RT_ERR_FAILED;
#else
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, ARP_CONTROL, ARPTRAP, action))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
#endif		
}
/*
@func ret_t | rtl8307h_optionHeader_asicIpv6TrapControl_set |Ipv6 packet with specified extension header trap to cpu or not
@parm uint32 | action | trap or normal forward
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_OUT_OF_RANGE | Parameter out of range.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can set ipv6 packet with specified extension header trap to cpu or not. 
*/
ret_t rtl8307h_optionHeader_asicIpv6TrapControl_set( uint32 action)
{
    if(action > 0x1)
        return RT_ERR_OUT_OF_RANGE;
    
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, OPTION_HEADER_TRAP_CONTROL, IPV6OPHDRTRAP, action))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_optionHeader_asicIpv6TrapControl_get | Ipv6 packet with specified extension header trap to cpu or not
@parm uint32* | action |trap or normal forward
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can check  ipv6 packet with specified extension header trap to cpu or not. 
*/
ret_t rtl8307h_optionHeader_asicIpv6TrapControl_get( uint32* action)
{    
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, OPTION_HEADER_TRAP_CONTROL, IPV6OPHDRTRAP, action))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_optionHeader_asicIpv4TrapControl_set |IPv4 packet with any option header  trap to cpu or not
@parm uint32 | action | trap or normal forward
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_OUT_OF_RANGE | Parameter out of range.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can set IPv4 packet with any option header trap to cpu or not. 
*/
ret_t rtl8307h_optionHeader_asicIpv4TrapControl_set( uint32 action)
{
    if(action > 0x1)
        return RT_ERR_OUT_OF_RANGE;
    
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, OPTION_HEADER_TRAP_CONTROL, IPV4OPHDRTRAP, action))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_optionHeader_asicIpv4TrapControl_get | IPv4 packet with any option header trap to cpu or not
@parm uint32* | action |trap or normal forward
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can check  IPv4 packet with any option header trap to cpu or not. 
*/
ret_t rtl8307h_optionHeader_asicIpv4TrapControl_get( uint32* action)
{    
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, OPTION_HEADER_TRAP_CONTROL, IPV4OPHDRTRAP, action))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_optionHeader_asicIpHdrCpuMsk_set | Set CPU PortMask
@parm uint32 | portMsk | CPU PortMask
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can set packets with option header CPU PortMask. 
*/
ret_t rtl8307h_optionHeader_asicIpHdrCpuMsk_set( uint32 portMsk)
{
    if(portMsk > 0x7)
        return RT_ERR_OUT_OF_RANGE;
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, OPTION_HEADER_TRAP_CONTROL, IPHDRCPUMSK, portMsk))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_arp_asicArpCpuPortMask_get | Get  CPU PortMask
@parm uint32* | portMsk |  CPU PortMask
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can set packets with option header CPU PortMask. 
*/
ret_t rtl8307h_optionHeader_asicIpHdrCpuMsk_get( uint32* portMsk)
{
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, OPTION_HEADER_TRAP_CONTROL, IPHDRCPUMSK, portMsk))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_optionHeader_asicDefOpPri_set | Define packet with option header trap to cpu priority
@parm uint32 | enabled | enable Define Priority or not
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_ENABLE | Invalid enable parameter.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can Define packet with option header trap to cpu priority. 
*/
ret_t rtl8307h_optionHeader_asicDefOpPri_set( uint32 enabled)
{
    if(enabled > 0x1)
        return RT_ERR_ENABLE;
    
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, OPTION_HEADER_TRAP_CONTROL, DFOPPRI, enabled))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_optionHeader_asicDefOpPri_get | Define packet with option header trap to cpu priority
@parm uint32* | enabled | check Define Priority enabled or not
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can check Define packet with option header trap to cpu priority. 
*/
ret_t rtl8307h_optionHeader_asicDefOpPri_get( uint32* enabled)
{    
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, OPTION_HEADER_TRAP_CONTROL, DFOPPRI, enabled))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_optionHeader_asicOpPri_set | Define Priority
@parm uint32 | opPri |  Define Priority
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_OUT_OF_RANGE | Parameter out of range.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can define packet with option header trap to cpu priority. 
*/
ret_t rtl8307h_optionHeader_asicOpPri_set( uint32 opPri)
{
    if(opPri > 0x7)
        return RT_ERR_OUT_OF_RANGE;
    
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, OPTION_HEADER_TRAP_CONTROL, OPPRI, opPri))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_optionHeader_asicOpPri_get | get the defined priority
@parm uint32* | opPri |the defined priority
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can get packet with option header trap to cpu priority. 
*/
ret_t rtl8307h_optionHeader_asicOpPri_get( uint32* opPri)
{    
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, OPTION_HEADER_TRAP_CONTROL, OPPRI, opPri))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_cfi_asicCfiTrap_set | Set packets with inner tag and cfi == 1 trapped to CPU or not
@parm rtl8307h_miscTrap_t | action |  trapped to CPU or not
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can set packets with inner tag and cfi == 1 trapped to CPU or not
*/
ret_t rtl8307h_cfi_asicCfiTrap_set( rtk_trap_misc_action_t action)
{
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, CFI_CONTROL, CFITRAP, action))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_cfi_asiCfiTrap_get | Check packets with inner tag and cfi == 1 trapped to CPU or not
@parm rtl8307h_miscTrap_t* | action |  packets trapped to CPU or not
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can check  packets with inner tag and cfi == 1 trapped to CPU or not
*/
#ifndef	EXT_LAB
ret_t rtl8307h_cfi_asicCfiTrap_get( rtk_trap_misc_action_t* action)
{
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, CFI_CONTROL, CFITRAP, action))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}
#endif

/*
@func ret_t | rtl8307h_cfi_asicCpuMsk_set | Set CPU PortMask
@parm uint32 | portMsk | CPU PortMask
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can set packets with inner tag and cfi == 1 option header CPU PortMask. 
*/
ret_t rtl8307h_cfi_asicCpuMsk_set( uint32 portMsk)
{
    if(portMsk > 0x7)
        return RT_ERR_OUT_OF_RANGE;
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, CFI_CONTROL, CPUMASK, portMsk))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_cfi_asicCpuMsk_get | Get  CPU PortMask
@parm uint32* | portMsk |  CPU PortMask
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can set packets with with inner tag and cfi == 1  CPU PortMask. 
*/
ret_t rtl8307h_cfi_asicCpuMsk_get( uint32* portMsk)
{
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, CFI_CONTROL, CPUMASK, portMsk))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_cfi_asicDefCfiPri_set | Define packet with CFI trap to cpu priority
@parm uint32 | enabled | enable Define Priority or not
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_ENABLE | Invalid enable parameter.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can Define packet with CFI trap to cpu priority. 
*/
ret_t rtl8307h_cfi_asicDefCfiPri_set( uint32 enabled)
{
    if(enabled > 0x1)
        return RT_ERR_ENABLE;
    
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, CFI_CONTROL, DFCFIPRI, enabled))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_optionHeader_asicDefOpPri_get | Define packet with CFI trap to cpu priority
@parm uint32* | enabled | check Define Priority enabled or not
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can check Define packet with CFI trap to cpu priority. 
*/
ret_t rtl8307h_cfi_asicDefCfiPri_get( uint32* enabled)
{    
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, CFI_CONTROL, DFCFIPRI, enabled))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_cfi_asicCfiPri_set | Define packet with CFI trap to cpu priority
@parm uint32 | cfiPri |  Define Priority
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_OUT_OF_RANGE | Parameter out of range.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can Define packet with CFI trap to cpu priority. 
*/
ret_t rtl8307h_cfi_asicCfiPri_set( uint32 cfiPri)
{
    if(cfiPri > 0x7)
        return RT_ERR_OUT_OF_RANGE;
    
    if( SUCCESS == reg_field_write(RTL8307H_UNIT, CFI_CONTROL, CFIPRI, cfiPri))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
/*
@func ret_t | rtl8307h_optionHeader_asicOpPri_get | get the defined priority
@parm uint32* | cfiPri |the defined priority
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_FAILED | General error, including register read and write. 
@comm
    This API can get packet with CFI trap to cpu priority. 
*/
ret_t rtl8307h_cfi_asicCfiPri_get( uint32* cfiPri)
{    
    if( SUCCESS == reg_field_read(RTL8307H_UNIT, CFI_CONTROL, CFIPRI, cfiPri))
        return RT_ERR_OK;       
    else
        return RT_ERR_FAILED;
}
