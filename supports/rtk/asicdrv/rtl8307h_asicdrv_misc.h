#ifndef _RTL8307H_ASICDRV_MISC_H_
#define _RTL8307H_ASICDRV_MISC_H_

#include <rtk_types.h>
#include <rtk_error.h>

extern ret_t rtl8307h_arp_asicDefPri_set( uint32 enabled);
extern ret_t rtl8307h_arp_asicDefPri_get( uint32* enabled);
extern ret_t rtl8307h_arp_asicPri_set( uint32 arpPri);
extern ret_t rtl8307h_arp_asicPri_get( uint32* arpPri);
extern ret_t rtl8307h_arp_asicArpCpuPortMask_set( uint32 portMsk);
extern ret_t rtl8307h_arp_asicArpCpuPortMask_get( uint32* portMsk);
extern ret_t rtl8307h_arp_asicArpTrap_set( rtk_trap_misc_action_t action);
extern ret_t rtl8307h_arp_asicArpTrap_get( rtk_trap_misc_action_t* action);
extern ret_t rtl8307h_optionHeader_asicIpv6TrapControl_set( uint32 action);
extern ret_t rtl8307h_optionHeader_asicIpv6TrapControl_get( uint32* action);
extern ret_t rtl8307h_optionHeader_asicIpv4TrapControl_set( uint32 action);
extern ret_t rtl8307h_optionHeader_asicIpv4TrapControl_get( uint32* action);
extern ret_t rtl8307h_optionHeader_asicIpHdrCpuMsk_set( uint32 portMsk);
extern ret_t rtl8307h_optionHeader_asicIpHdrCpuMsk_get( uint32* portMsk);
extern ret_t rtl8307h_optionHeader_asicDefOpPri_set( uint32 enabled);
extern ret_t rtl8307h_optionHeader_asicDefOpPri_get( uint32* enabled);
extern ret_t rtl8307h_optionHeader_asicOpPri_set( uint32 opPri);
extern ret_t rtl8307h_optionHeader_asicOpPri_get( uint32* opPri);
extern ret_t rtl8307h_cfi_asicCfiTrap_set( rtk_trap_misc_action_t action);
extern ret_t rtl8307h_cfi_asiCfiTrap_get( rtk_trap_misc_action_t* action);
extern ret_t rtl8307h_cfi_asicCpuMsk_set( uint32 portMsk);
extern ret_t rtl8307h_cfi_asicCpuMsk_get( uint32* portMsk);
extern ret_t rtl8307h_cfi_asicDefCfiPri_set( uint32 enabled);
extern ret_t rtl8307h_cfi_asicDefCfiPri_get( uint32* enabled);
extern ret_t rtl8307h_cfi_asicCfiPri_set( uint32 cfiPri);
extern ret_t rtl8307h_cfi_asicCfiPri_get( uint32* cfiPri);

    

#endif /*_RTL8307H_ASICDRV_MISC_H_*/

