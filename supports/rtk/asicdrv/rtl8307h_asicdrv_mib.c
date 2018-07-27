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
 * $Revision: 8218 $
 * $Date: 2010-01-25 19:04:57 +0800 (Mon, 25 Jan 2010) $
 *
 * Purpose : RTL8307H switch low-level API for MIB counter module
 * Feature : 
 *
 */

#include <rtk_types.h>
#include <rtk_error.h>
#include <rtk_api.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>
#include <rtl8307h_table_struct.h>
#include <rtl8307h_debug.h>


#include <rtl8307h_asicdrv_mib.h>

/* Function Name:
 *      rtl8307h_mib_portCntAddr_get
 * Description:
 *     Get 07H's per-port MIB counter address by rtk_stat_port_type_t.
 * Input:
 *      cntr_idx  - the MIB counter Index
 *      port -  port ID
 * Output:
 *      pMibAddr   - MIB address returned 
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_INPUT - Invalid input parameter, maybe specific MIB counter not supported by RTL8307H.
 * Note:
 *      This function works as a MIB counter address converter. It returns the specific RTL8307H's MIB counter address.
 */
int32 rtl8307h_mib_portCntAddr_get(rtk_stat_port_type_t cntr_idx, uint8 port, uint16* pMibAddr)
{
    /*map rtk_stat_port_type_t cntr_idx to RTL8307H's internal MIB counter address*/
    uint8 addrMap[STAT_MIB_PORT_CNTR_END] = {
       /*0~9*/
       14, 27, 29, 19, 30, 23, 25, 20, 31, 16,
       /*10~19*/
       41, 40, 34, 35, 36, 37, 38, 39, 21, 22,
       /*20~29*/
       0, 7, 8, 10, 11, 9, 12, 3, 0xFF, 32,
       /*30~39*/
       4, 5, 6, 0xFF, 0xFF, 0xFF, 2, 18, 26, 28
    };

    if (cntr_idx >= STAT_MIB_PORT_CNTR_END)
    {
        return RT_ERR_INPUT;
    }
    
    *pMibAddr = addrMap[cntr_idx];

    /*MIB counter retrived not supported by RTL8307H*/
    if (*pMibAddr == 0xFF)
        return RT_ERR_CHIP_NOT_SUPPORTED;
    
     *pMibAddr = *pMibAddr + (port * RTL8307H_PORTMIB_OFFSET);

    return RT_ERR_OK ;
}

/* Function Name:
 *      rtl8307h_mib_counter_reset
 * Description:
 *     Set MIBs global/queue manage reset or per-port reset.
 * Input:
 *      greset  - Global reset 
 *      qmreset  - Queue maganement reset
 *      pmask  - Port reset mask  
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_INPUT - Invalid input parameter.
 * Note:
 *      1. ASIC will clear all MIBs counter by global resetting and clear counters associated with a particular port by mapped port resetting. 
 *      2. RTL8307H does not support queue maganement reset.
 */
int32 rtl8307h_mib_counter_reset(uint32 greset, uint32 qmreset, uint32 pmask)
{
    uint32 regVal;
    uint32 port;
    uint32 cnt;

    if (pmask > PM_PORT_ALL)
        return RT_ERR_PORT_MASK;
    
    regVal = 0;

    reg_field_set(RTL8307H_UNIT, MIB_COUNTER_CONTROL1, SYSCOUNTERRESET, greset, &regVal);
    regVal |= pmask;

    reg_write(RTL8307H_UNIT, MIB_COUNTER_CONTROL1, regVal);

    cnt = 0;
    while(cnt++ < 0x100)
    {
        reg_read(RTL8307H_UNIT, MIB_COUNTER_CONTROL1, &regVal);
        if (regVal == 0)
            break;
    }

    if (cnt == 0x100)
    {
        return RT_ERR_FAILED;
    }
    
    /*clear overflags */
    if (greset)
    {
       reg_field_write(RTL8307H_UNIT, SYSTEM_MIB_COUNTER_OVERFLOW_FLAG, DOT1DTPLEARNEDENTRYDISCARDSOF_FLG, 1);
    }
    
    for (port = 0; port < PN_PORT_END; port++)
    {
        if (pmask & ((uint32)1 << port))
        {
            reg_write(RTL8307H_UNIT, PORT0_MIBCOUNTER_OVERFLOW_FLAG0 + port, 0xFFFFFFFF);
            reg_write(RTL8307H_UNIT, PORT0_MIBCOUNTER_OVERFLOW_FLAG1 + port, 0xFFFFFFFF);
        }
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      rtl8307h_mib_counter_get
 * Description:
 *     Get specificed MIB counter.
 * Input:
 *      mibAddr  - mib counter address
 * Output:
 *      counterH  - higher 32-bit of the counter returned.
 *      counterL  - lower 32-bit of the counter returned.
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_NULL_POINTER - Invalid input parameter.
 * Note:
 *       This API gets specificed MIB counter. 
 */
int32 rtl8307h_mib_counter_get(uint16 mibAddr, uint32* counterH, uint32* counterL)
{
    uint32 regVal;
    uint32 cnt;

    if (counterH == NULL || counterL == NULL)
        return RT_ERR_NULL_POINTER;
    
    reg_read(RTL8307H_UNIT, INDIRECT_ACCESS_MIB_COUNTER_CONTROL, &regVal);
    reg_field_set(RTL8307H_UNIT, INDIRECT_ACCESS_MIB_COUNTER_CONTROL, READCOUNTERTRIG, 1, &regVal);
    reg_field_set(RTL8307H_UNIT, INDIRECT_ACCESS_MIB_COUNTER_CONTROL, MIBCOUNTERADDR, mibAddr, &regVal);
    reg_write(RTL8307H_UNIT, INDIRECT_ACCESS_MIB_COUNTER_CONTROL, regVal);

    cnt = 0;
    do
    {
        reg_field_read(RTL8307H_UNIT, INDIRECT_ACCESS_MIB_COUNTER_CONTROL, READCOUNTERTRIG, &regVal);
        if (regVal == 0)
            break;
    }while(cnt++ < 0x100);

    if (cnt == 0x100)
    {
        return RT_ERR_FAILED;
    }
    
    reg_read(RTL8307H_UNIT, INDIRECT_ACCESS_MIB_COUNTER_DATA0, counterL);
    reg_read(RTL8307H_UNIT, INDIRECT_ACCESS_MIB_COUNTER_DATA1, counterH);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8307h_mib_counter_enable
 * Description:
 *     Enable or disable MIB counter.
 * Input:
 *      enable  - TRUE or FALSE
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_NULL_POINTER - Invalid input parameter.
 * Note:
 */
int32 rtl8307h_mib_counter_enable(uint32 enable)
{
    int32 retVal;

    if ((retVal = reg_field_write(RTL8307H_UNIT, MIB_COUNTER_CONTROL0, ENMIBCOUNTER, enable ? 1 :0)) != SUCCESS)
        return retVal;

    return RT_ERR_OK;

}

