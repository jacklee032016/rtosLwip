#ifndef _RTL8307H_ASICDRV_DOT1X_H_
#define _RTL8307H_ASICDRV_DOT1X_H_

#include <rtk_types.h>
#include <rtk_error.h>
#include <rtk_api.h>

/* unauth pkt action */
typedef enum  rtl8307h_dot1xPktType_e
{
    RTL8307H_DOT1X_PKT_INTAG = 0,
    RTL8307H_DOT1X_PKT_INUNTAG,
    RTL8307H_DOT1X_PKT_OUTTAG,
    RTL8307H_DOT1X_PKT_OUTUNTAG,
    RTL8307H_DOT1X_PKT_END
}  rtl8307h_dot1xPktType_t;

extern int32 rtl8307h_dot1x_unauthPktAct_set(rtl8307h_dot1xPktType_t pktType, rtk_dot1x_unauth_action_t action);

extern int32 rtl8307h_dot1x_unauthPktAct_get(rtl8307h_dot1xPktType_t pktType, rtk_dot1x_unauth_action_t *pAction);


#endif /*_RTL8307H_ASICDRV_DOT1X_H_*/

