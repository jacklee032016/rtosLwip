#ifndef _RTL8307H_ASICDRV_RMA_H_
#define _RTL8307H_ASICDRV_RMA_H_

#include <rtk_types.h>
#include <rtk_error.h>


typedef enum rtl8307h_rma_extend_s
{
    RMA_EXTEND_PTP = 0x40,
    RMA_EXTEND_MMRP,
    RMA_EXTEND_MVRP,
    RMA_EXTEND_END
}rtl8307h_rma_extend_t;

extern int32  rtl8307h_rmaAction_set(uint32 index, rtk_trap_rma_action_t rmaOp);
extern int32  rtl8307h_rmaAction_get(uint32 index, rtk_trap_rma_action_t* pRmaOp);
extern int32  rtl8307h_rmaTrapPri_set(uint32 index, uint32 priority);
extern int32  rtl8307h_rmaTrapPri_get(uint32 index, uint32* pPriority);
#endif /*#ifndef _RTL8307H_ASICDRV_RMA_H_*/

