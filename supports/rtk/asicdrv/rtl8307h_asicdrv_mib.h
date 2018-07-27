#ifndef _RTL8307H_ASICDRV_MIB_H_
#define _RTL8307H_ASICDRV_MIB_H_

#include <rtk_types.h>
#include <rtk_error.h>
#include <rtk_api.h>

#define RTL8307H_PORTMIB_OFFSET     0x3C



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
extern int32 rtl8307h_mib_portCntAddr_get(rtk_stat_port_type_t cntr_idx, uint8 port, uint16* pMibAddr);


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
extern int32 rtl8307h_mib_counter_reset(uint32 greset, uint32 qmreset, uint32 pmask);

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
extern int32 rtl8307h_mib_counter_get(uint16 mibAddr, uint32* counterH, uint32* counterL);


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
extern int32 rtl8307h_mib_counter_enable(uint32 enable);

#endif /*#ifndef _RTL8307H_ASICDRV_MIB_H_*/

