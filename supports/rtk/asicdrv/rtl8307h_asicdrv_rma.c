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
* $Revision: 9040 $
* $Date: 2010-04-14 19:40:10 +0800 (星期三, 14 四月 2010) $
*
* Purpose : ASIC-level driver implementation for Reserved Multicast Address.
*
*  Feature :  This file consists of following modules:
*             1) 
*
*/

#include <rtk_types.h>
#include <rtk_api_ext.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>

#include <rtl8307h_asicdrv_rma.h>
#include <rtl8307h_asicdrv_lock.h>

int32  rtl8307h_rmaAction_set(uint32 index, rtk_trap_rma_action_t rmaOp)
{
    int32 retVal;
    
    if (index == 0)
    {
        rtl8307h_reg_lock();
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL0, ACT0, rmaOp);
        rtl8307h_reg_unlock();
    } 
    else if (index == 1) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL0, ACT1, rmaOp);
    } else if (index == 2)
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL0, ACT2, rmaOp);
    } 
    else if (index == 3) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL1, ACT3, rmaOp);
    } 
    else if ((index >= 4 && index <= 0xD) || (index == 0xF))
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL1, ACT4, rmaOp);
    } 
    else if (index == 0xE) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL1, ACT5, rmaOp);
    } 
    else if (index == 0x10) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL2, ACT6, rmaOp);
    } 
    else if (index == 0x20) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL2, ACT7, rmaOp);
    }
    else if (index == 0x21) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL2, ACT8, rmaOp);
    }
    else if (index >= 0x22 && index <= 0x2F)
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL3, ACT9, rmaOp);
    }
    else if (index >= 0x31 && index <= 0x3F) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL3, ACT10, rmaOp);
    }
    else if (index == RMA_EXTEND_PTP)
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL4, PTP_ACT, rmaOp);
    }
    else if (index == RMA_EXTEND_MMRP)
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL4, MMRP_ACT, rmaOp);
    }
    else if (index == RMA_EXTEND_MVRP)
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL4, MVRP_ACT, rmaOp);
    }
    else  
    {
        return RT_ERR_RMA_ADDR;
    } 

    if (SUCCESS != retVal)
        return retVal;

    return RT_ERR_OK;
}

int32  rtl8307h_rmaAction_get(uint32 index, rtk_trap_rma_action_t* pRmaOp)
{
    int32 retVal;
    
#ifdef	EXT_LAB
	uint32 regVal;

	*pRmaOp = RMA_ACTION_FORWARD;
	if (index == 0)
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL0, ACT0, &regVal);
	} 
	else if (index == 1) 
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL0, ACT1, &regVal);
	}
	else if (index == 2)
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL0, ACT2, &regVal);
	} 
	else if (index == 3) 
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL1, ACT3, &regVal);
	} 
	else if ((index >= 4 && index <= 0xD) || (index == 0xF))
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL1, ACT4, &regVal);
	} 
	else if (index == 0xE) 
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL1, ACT5, &regVal);
	} 
	else if (index == 0x10) 
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL2, ACT6, &regVal);
	} 
	else if (index == 0x20) 
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL2, ACT7, &regVal);
	}
	else if (index == 0x21) 
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL2, ACT8, &regVal);
	}
	else if (index >= 0x22 && index <= 0x2F)
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL3, ACT9, &regVal);
	}
	else if (index >= 0x31 && index <= 0x3F) 
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL3, ACT10, &regVal);
	}
	else if (index == RMA_EXTEND_PTP)
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL4, PTP_ACT, &regVal);
	}
	else if (index == RMA_EXTEND_MMRP)
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL4, MMRP_ACT, &regVal);
	}
	else if (index == RMA_EXTEND_MVRP)
	{
		retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL4, MVRP_ACT, &regVal);
	}
	else
	{
		return RT_ERR_RMA_ADDR;
	} 

	*pRmaOp = (rtk_trap_rma_action_t)regVal;
#else
    if (index == 0)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL0, ACT0, pRmaOp);
    } 
    else if (index == 1) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL0, ACT1, pRmaOp);
    } else if (index == 2)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL0, ACT2, pRmaOp);
    } 
    else if (index == 3) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL1, ACT3, pRmaOp);
    } 
    else if ((index >= 4 && index <= 0xD) || (index == 0xF))
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL1, ACT4, pRmaOp);
    } 
    else if (index == 0xE) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL1, ACT5, pRmaOp);
    } 
    else if (index == 0x10) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL2, ACT6, pRmaOp);
    } 
    else if (index == 0x20) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL2, ACT7, pRmaOp);
    }
    else if (index == 0x21) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL2, ACT8, pRmaOp);
    }
    else if (index >= 0x22 && index <= 0x2F)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL3, ACT9, pRmaOp);
    }
    else if (index >= 0x31 && index <= 0x3F) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL3, ACT10, pRmaOp);
    }
    else if (index == RMA_EXTEND_PTP)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL4, PTP_ACT, pRmaOp);
    }
    else if (index == RMA_EXTEND_MMRP)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL4, MMRP_ACT, pRmaOp);
    }
    else if (index == RMA_EXTEND_MVRP)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL4, MVRP_ACT, pRmaOp);
    }
    else
    {
        return RT_ERR_RMA_ADDR;
    } 
#endif

    if (SUCCESS != retVal)
        return retVal;

    return RT_ERR_OK;
}

int32  rtl8307h_rmaTrapPri_set(uint32 index, uint32 priority)
{
#ifdef	EXT_LAB
    uint32 regVal;
#else	
    int32 retVal;
    int32 regVal;
#endif	
    
    if (index == 0)
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL0, &regVal);
        regVal |= 1 << 6;
        regVal &= ~(0x7 << 3);
        regVal |= priority << 3;
        reg_write(RTL8307H_UNIT, RMA_CONTROL0, regVal);
    } 
    else if (index == 1) 
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL0, &regVal);
        regVal |= 1 << 15;
        regVal &= ~(0x7 << 12);
        regVal |= priority << 12;
        reg_write(RTL8307H_UNIT, RMA_CONTROL0, regVal);
    } 
    else if (index == 2) 
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL0, &regVal);
        regVal |= 1 << 24;
        regVal &= ~(0x7 << 21);
        regVal |= priority << 21;
        reg_write(RTL8307H_UNIT, RMA_CONTROL0, regVal);
    } 
    else if (index == 3) 
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL1, &regVal);
        regVal |= 1 << 6;
        regVal &= ~(0x7 << 3);
        regVal |= priority << 3;
        reg_write(RTL8307H_UNIT, RMA_CONTROL1, regVal);
    } 
    else if ((index >= 4 && index <= 0xD) || index == 0xF) 
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL1, &regVal);
        regVal |= 1 << 15;
        regVal &= ~(0x7 << 12);
        regVal |= priority << 12;
        reg_write(RTL8307H_UNIT, RMA_CONTROL1, regVal);
    } 
    else if (index == 0xE) 
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL1, &regVal);;
        regVal |= 1 << 24;
        regVal &= ~(0x7 << 21);
        regVal |= priority << 21;
        reg_write(RTL8307H_UNIT, RMA_CONTROL1, regVal);
    } 
    else if (index == 0x10) 
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL2, &regVal);
        regVal |= 1 << 6;
        regVal &= ~(0x7 << 3);
        regVal |= priority << 3;
        reg_write(RTL8307H_UNIT, RMA_CONTROL2, regVal);
    }
    else if (index == 0x20)
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL2, &regVal);
        regVal |= 1 << 15;
        regVal &= ~(0x7 << 12);
        regVal |= priority << 12;
        reg_write(RTL8307H_UNIT, RMA_CONTROL2, regVal);
    } 
    else if (index == 0x21)
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL2, &regVal);
        regVal |= 1 << 24;
        regVal &= ~(0x7 << 21);
        regVal |= priority << 21;
        reg_write(RTL8307H_UNIT, RMA_CONTROL2, regVal);
    } 
    else if (index >= 0x22 && index <= 0x2F)
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL3, &regVal);
        regVal |= 1 << 6;
        regVal &= ~(0x7 << 3);
        regVal |= priority << 3;
        reg_write(RTL8307H_UNIT, RMA_CONTROL3, regVal);
    } 
    else if (index >= 0x31 && index <= 0x3F) 
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL3, &regVal);
        regVal |= 1 << 15;
        regVal &= ~(0x7 << 12);
        regVal |= priority << 12;
        reg_write(RTL8307H_UNIT, RMA_CONTROL3, regVal);
    }
    else if (index == RMA_EXTEND_PTP)
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL4, &regVal);
        regVal |= 1 << 6;
        regVal &= ~(0x7 << 3);
        regVal |= priority << 3;
        reg_write(RTL8307H_UNIT, RMA_CONTROL4, regVal);
    }
    else if (index == RMA_EXTEND_MMRP)
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL4, &regVal);
        regVal |= 1 << 15;
        regVal &= ~(0x7 << 12);
        regVal |= priority << 12;
        reg_write(RTL8307H_UNIT, RMA_CONTROL4, regVal);
    }
    else if (index == RMA_EXTEND_MVRP)
    {
        reg_read(RTL8307H_UNIT, RMA_CONTROL4, &regVal);
        regVal |= 1 << 24;
        regVal &= ~(0x7 << 21);
        regVal |= priority << 21;
        reg_write(RTL8307H_UNIT, RMA_CONTROL4, regVal);
    }
    else  
    {
        return RT_ERR_RMA_ADDR;
    } 

    return RT_ERR_OK;
}

int32  rtl8307h_rmaTrapPri_get(uint32 index, uint32* pPriority)
{
    int32 retVal;
    
    if (index == 0)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL0, RMAPRI0, pPriority);
    } 
    else if (index == 1)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL0, RMAPRI1, pPriority);
    } 
    else if (index == 2)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL0, RMAPRI2, pPriority);
    } 
    else if (index == 3) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL1, RMAPRI3, pPriority);
    } 
    else if ((index >= 4 && index <= 0xD) || index == 0xF) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL1, RMAPRI4, pPriority);
    } 
    else if (index == 0xE) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL1, RMAPRI5, pPriority);
    } 
    else if (index == 0x10) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL2, RMAPRI6, pPriority);
    }
    else if (index == 0x20)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL2, RMAPRI7, pPriority);
    } 
    else if (index == 0x21)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL2, RMAPRI8, pPriority);
    } 
    else if (index >= 0x22 && index <= 0x2F)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL3, RMAPRI9, pPriority);
    } 
    else if (index >= 0x31 && index <= 0x3F)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL3, RMAPRI10, pPriority);
    }
    else if (index == RMA_EXTEND_PTP)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL4, PTP_RMAPRI, pPriority);
    }
    else if (index == RMA_EXTEND_MMRP)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL4, MMRP_RMAPRI, pPriority);
    }
    else if (index == RMA_EXTEND_MVRP)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL4, MVRP_RMAPRI, pPriority);
    }
    else
    {
        return RT_ERR_RMA_ADDR;
    } 

#ifdef	EXT_LAB
	return retVal;
#else
    return RT_ERR_OK;
#endif
}

#ifndef	EXT_LAB
int32  rtl8307h_rmaCpuMsk_set(uint32 index, uint32 portMsk)
{
    int32 retVal;

    if(portMsk > 0x7)
        return RT_ERR_OUT_OF_RANGE;

    if (index == 0)
    {
        rtl8307h_reg_lock();
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL0, CPUMASK0, portMsk);
        rtl8307h_reg_unlock();
    } 
    else if (index == 1) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL0, CPUMASK1, portMsk);
    } else if (index == 2)
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL0, CPUMASK2, portMsk);
    } 
    else if (index == 3) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL1, CPUMASK3, portMsk);
    } 
    else if ((index >= 4 && index <= 0xD) || (index == 0xF))
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL1, CPUMASK4, portMsk);
    } 
    else if (index == 0xE) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL1, CPUMASK5, portMsk);
    } 
    else if (index == 0x10) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL2, CPUMASK6, portMsk);
    } 
    else if (index == 0x20) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL2, CPUMASK7, portMsk);
    }
    else if (index == 0x21) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL2, CPUMASK8, portMsk);
    }
    else if (index >= 0x22 && index <= 0x2F)
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL3, CPUMASK9, portMsk);
    }
    else if (index >= 0x31 && index <= 0x3F) 
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL3, CPUMASK10, portMsk);
    }
    else if (index == RMA_EXTEND_PTP)
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL4, PTP_CPUMASK, portMsk);
    }
    else if (index == RMA_EXTEND_MMRP)
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL4, MMRP_CPUMASK, portMsk);
    }
    else if (index == RMA_EXTEND_MVRP)
    {
        retVal = reg_field_write(RTL8307H_UNIT, RMA_CONTROL4, MVRP_CPUMASK, portMsk);
    }
    else  
    {
        return RT_ERR_RMA_ADDR;
    } 

    if (SUCCESS != retVal)
        return retVal;

    return RT_ERR_OK;
}


int32  rtl8307h_rmaCpuMsk_get(uint32 index, uint32 *portMsk)
{
    int32 retVal;
    
    if (index == 0)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL0, CPUMASK0, portMsk);
    } 
    else if (index == 1) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL0, CPUMASK1, portMsk);
    } else if (index == 2)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL0, CPUMASK2, portMsk);
    } 
    else if (index == 3) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL1, CPUMASK3, portMsk);
    } 
    else if ((index >= 4 && index <= 0xD) || (index == 0xF))
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL1, CPUMASK4, portMsk);
    } 
    else if (index == 0xE) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL1, CPUMASK5, portMsk);
    } 
    else if (index == 0x10) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL2, CPUMASK6, portMsk);
    } 
    else if (index == 0x20) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL2, CPUMASK7, portMsk);
    }
    else if (index == 0x21) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL2, CPUMASK8, portMsk);
    }
    else if (index >= 0x22 && index <= 0x2F)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL3, CPUMASK9, portMsk);
    }
    else if (index >= 0x31 && index <= 0x3F) 
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL3, CPUMASK10, portMsk);
    }
    else if (index == RMA_EXTEND_PTP)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL4, PTP_CPUMASK, portMsk);
    }
    else if (index == RMA_EXTEND_MMRP)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL4, MMRP_CPUMASK, portMsk);
    }
    else if (index == RMA_EXTEND_MVRP)
    {
        retVal = reg_field_read(RTL8307H_UNIT, RMA_CONTROL4, MVRP_CPUMASK, portMsk);
    }
    else
    {
        return RT_ERR_RMA_ADDR;
    } 

    if (SUCCESS != retVal)
        return retVal;

    return RT_ERR_OK;
}
#endif

