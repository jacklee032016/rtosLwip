#ifndef _RTL8307H_ASICDRV_PHY_H_
#define _RTL8307H_ASICDRV_PHY_H_

#include <rtk_types.h>
#include <rtk_error.h>

#define RTL8307H_MAX_POLLCNT  1000

extern rtk_api_ret_t phy_reg_read(uint8 phyaddr, uint8 page, uint8 regaddr, uint16* reg_data);
extern rtk_api_ret_t phy_reg_write(uint8 phyaddr, uint8 page, uint8 regaddr, uint16 reg_data);

extern rtk_api_ret_t mmd_reg_read(uint8 phyaddr, uint8 device_id, uint16 regaddr, uint16* reg_data);
extern rtk_api_ret_t mmd_reg_write(uint8 phyaddr, uint8 device_id, uint16 regaddr, uint16 reg_data);

extern rtk_api_ret_t giga_phy_ext_read(uint8 page, uint8 regaddr, uint16* reg_data);
extern rtk_api_ret_t giga_phy_ext_write(uint8 page, uint8 regaddr, uint16 reg_data);

#endif /*#ifndef _RTL8307H_ASICDRV_PHY_H_*/

