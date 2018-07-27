#ifndef _RTL8307H_ASICDRV_LOCK_H_
#define _RTL8307H_ASICDRV_LOCK_H_

#include <rtk_types.h>
#include <rtk_error.h>

extern int32 rtl8307h_table_lock(void);
extern int32 rtl8307h_table_unlock(void);
extern void rtl8307h_reg_lock(void);
extern void rtl8307h_reg_unlock(void);

#endif /*_RTL8307H_ASICDRV_WOL_H_*/

