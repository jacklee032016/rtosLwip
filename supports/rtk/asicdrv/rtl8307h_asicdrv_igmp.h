#ifndef _RTL8307H_ASICDRV_IGMP_H_
#define _RTL8307H_ASICDRV_IGMP_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <rtk_types.h>
#include <rtk_error.h>

/* Function Name:
 *      rtl8307h_igmp_asicIgmpEnable_set
 * Description:
 *      set igmp function enabled or disabled    
 * Input:
 *      enabled -the value should be 0x0 or 0x1
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can enable or disable igmp function. 
 */
extern ret_t rtl8307h_igmp_asicIgmpEnable_set( uint32 disabled);
/* Function Name:
 *      rtl8307h_igmp_asicIgmpEnable_get
 * Description:
 *      check igmp function enabled or disabled    
 * Input:
 *      none
 * Output:
 *      enabled -the value should be 0x0 or 0x1
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED
 * Note:
 *      This API can check igmp function enabled or disabled.
 */
extern ret_t rtl8307h_igmp_asicIgmpEnable_get( uint32* disabled);
/* Function Name:
 *      rtl8307h_igmp_asicMldEnable_set
 * Description:
 *      set mld function enabled or disabled    
 * Input:
 *      enabled -the value should be 0x0 or 0x1
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can enable or disable mld function. 
 */
extern ret_t rtl8307h_igmp_asicMldEnable_set( uint32 disabled);
/* Function Name:
 *      rtl8307h_igmp_asicMldEnable_get
 * Description:
 *      check mld function enabled or disabled    
 * Input:
 *      none
 * Output:
 *      enabled -the value should be 0x0 or 0x1
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED
 * Note:
 *      This API can check mld function enabled or disabled.
 */
extern ret_t rtl8307h_igmp_asicMldEnable_get( uint32* disabled);
/* Function Name:
 *      rtl8307h_igmp_asicIpMulticastVlanLeaky_set
 * Description:
 *      set cross vlan function enabled or disabled    
 * Input:
 *      enabled -the value should be 0x0 or 0x1
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE  
 *      RT_ERR_FAILED
 * Note:
 *      The API can set the IP multicast VLAN Leaky function. When enabling this function, 
 *      if the lookup result(forwarding portmap) of IP Multicast packet is over VLAN boundary, 
 *      the packet can be forwarded across VLAN.
*/
extern ret_t rtl8307h_igmp_asicIpMulticastVlanLeaky_set( uint32 enabled);
/* Function Name:
 *      rtl8307h_igmp_asicIpMulticastVlanLeaky_get
 * Description:
 *      check cross vlan function enabled or disabled    
 * Input:
 *      none
 * Output:
 *      enabled -the value should be 0x0 or 0x1
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED
 * Note:
 *      The API can check  IP multicast VLAN Leaky function.
*/
extern ret_t rtl8307h_igmp_asicIpMulticastVlanLeaky_get( uint32* enabled);
/* Function Name:
 *      rtl8307h_igmp_asicFastLeaveEnable_set
 * Description:
 *      set fast leave function enabled or disabled    
 * Input:
 *      enabled -the value should be 0x0 or 0x1
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can set fast leave enabled or disabled. 
*/
extern ret_t rtl8307h_igmp_asicFastLeaveEnable_set( uint32 enabled);
/* Function Name:
 *      rtl8307h_igmp_asicFastLeaveEnable_get
 * Description:
 *      check fast leave function enabled or disabled    
 * Input:
 *      none
 * Output:
 *      enabled -the value should be 0x0 or 0x1
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED
 * Note:
 *      This API can check fast leave enabled or disabled. 
*/
extern ret_t rtl8307h_igmp_asicFastLeaveEnable_get( uint32* enabled);
/* Function Name:
 *      rtl8307h_igmp_asicIgmpCpuPortMask_set
 * Description:
 *      set igmp cpu port mask  
 * Input:
 *      portMsk  -  igmp cpu port mask, the value should be 0x0~0x7
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can set the cpu port mask when igmp packet trapped to cpu
 */
extern ret_t rtl8307h_igmp_asicIgmpCpuPortMask_set( uint32 portMsk);
/* Function Name:
 *      rtl8307h_igmp_asicIgmpCpuPortMask_get
 * Description:
 *      get igmp cpu port mask     
 * Input:
 *      none
 * Output:
 *      portMsk  -  igmp cpu port mask, the value should be 0x0~0x7
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can get the cpu port mask when igmp packet trapped to cpu
 */
extern ret_t rtl8307h_igmp_asicIgmpCpuPortMask_get( uint32* portMsk);
/* Function Name:
 *      rtl8307h_igmp_asicIgmpTrap_set
 * Description:
 *      set the action on igmp pkt
 * Input:
 *      action  - the  action on igmp pkt
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can set the action on igmp pkt;
 *      Input should be:
 *      0x0       Forward
 *      0x1       Trap(to IGMP cpu port mask )
 *      0x2       Copy to cpu
 *      0x3       Drop
 */
extern ret_t rtl8307h_igmp_asicIgmpTrap_set( rtk_trap_misc_action_t action);
/* Function Name:
 *      rtl8307h_igmp_asicIgmpTrap_get
 * Description:
 *      get the action on igmp pkt    
 * Input:
 *      none
 * Output:
 *      action  - the  action on igmp pkt
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can get the action on igmp pkt
 *      Output should be:
 *      0x0       Forward
 *      0x1       Trap(to IGMP cpu port mask )
 *      0x2       Copy to cpu
 *      0x3       Drop
 */
extern ret_t rtl8307h_igmp_asicIgmpTrap_get( rtk_trap_misc_action_t* action);
/* Function Name:
 *      rtl8307h_igmp_asicMldTrap_set
 * Description:
 *      set the action on mld pkt
 * Input:
 *      action  - the  action on mld pkt
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can set the action on mld pkt;
 *      Input should be:
 *      0x0       Forward
 *      0x1       Trap(to IGMP cpu port mask )
 *      0x2       Copy to cpu
 *      0x3       Drop
 */
extern ret_t rtl8307h_igmp_asicMldTrap_set( rtk_trap_misc_action_t action);
/* Function Name:
 *      rtl8307h_igmp_asicMldTrap_get
 * Description:
 *      get the action on mld pkt    
 * Input:
 *      none
 * Output:
 *      action  - the  action on mld pkt
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can get the action on mld pkt
 *      Output should be:
 *      0x0       Forward
 *      0x1       Trap(to IGMP cpu port mask )
 *      0x2       Copy to cpu
 *      0x3       Drop
 */
extern ret_t rtl8307h_igmp_asicMldTrap_get( rtk_trap_misc_action_t* action);
/* Function Name:
 *      rtl8307h_igmp_asicDefPri_set
 * Description:
 *      Define priority or not when IGMP/MLD packets trapped to CPU    
 * Input:
 *      enabled  -  define priority or not, the value should be 0x0 or 0x1
 * Output:
 *     none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can decide that define priority or not when IGMP/MLD packets trapped to CPU
 */
extern ret_t rtl8307h_igmp_asicDefPri_set( uint32 enabled);
/* Function Name:
 *      rtl8307h_igmp_asicDefPri_get
 * Description:
 *      check the priority defined or not when IGMP/MLD packets trapped to CPU    
 * Input:
 *      none
 * Output:
 *      enabled  -  define priority or not,  the value should be 0x0 or 0x1
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can check that priority defined or not  when IGMP/MLD packets trapped to CPU
 */
extern ret_t rtl8307h_igmp_asicDefPri_get( uint32* enabled);
/* Function Name:
 *      rtl8307h_igmp_asicPri_set
 * Description:
 *      Define the priority  when IGMP/MLD packets trapped to CPU    
 * Input:
 *      igmpPri  -  priority defined, the value should be 0x0~0x7
 * Output:
 *     none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can define the priority  when IGMP/MLD packets trapped to CPU
 */
extern ret_t rtl8307h_igmp_asicPri_set( uint32 igmpPri);
/* Function Name:
 *      rtl8307h_igmp_asicPri_get
 * Description:
 *      get the priority  when IGMP/MLD packets trapped to CPU    
 * Input:
 *      none
 * Output:
 *      igmpPri  -  priority defined, the value should be 0x0~0x7
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can get the priority  when IGMP/MLD packets trapped to CPU
 */
extern ret_t rtl8307h_igmp_asicPri_get( uint32* igmpPri);
/* Function Name:
 *      rtl8307h_igmp_paraChanged_reset
 * Description:
 *      hold cpu, reset nic, release cpu   
 * Input:
 *      none
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK  
 *      RT_ERR_FAILED
 * Note:
 *      This API can reset 8051 code when igmp parameters changed.
 */
#ifdef	MUX_LAB
extern ret_t rtl8307h_igmp_paraChanged_reset(void);
#else
extern ret_t rtl8307h_igmp_paraChanged_reset();
#endif

#endif /*#ifndef _RTL8307H_ASICDRV_IGMP_H_*/
