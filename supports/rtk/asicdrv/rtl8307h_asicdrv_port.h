#ifndef _RTL8307H_ASICDRV_PORTSECURITY_H_
#define _RTL8307H_ASICDRV_PORTSECURITY_H_

#include <rtk_types.h>
#include <rtk_error.h>
#include <rtk_api.h>

extern int32 rtl8307h_iso_mask_get(uint32 port, uint32 *pMask);
extern int32 rtl8307h_iso_mask_set(uint32 port, uint32 mask);

extern rtk_api_ret_t rtl8307h_mac6_miiMode_set(rtk_mode_ext_t mode);
extern rtk_api_ret_t rtl8307h_mac6_miiMode_get(rtk_mode_ext_t* mode);
extern rtk_api_ret_t rtl8307h_mac5_miiMode_set(rtk_mode_ext_t mode);
extern rtk_api_ret_t rtl8307h_mac5_miiMode_get(rtk_mode_ext_t* mode);

#endif /*_RTL8307H_ASICDRV_PORTSECURITY_H_*/

