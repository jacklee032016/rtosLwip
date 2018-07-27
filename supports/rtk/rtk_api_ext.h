/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition the error number in the SDK.
 *
 * Feature : error definition
 *
 */

#ifndef __RTK_API_EXT_H__
#define __RTK_API_EXT_H__

/*
 * Include Files
 */
#include <rtk_types.h>
#include <rtk_api.h>


/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_hec_mode_set
 * Description:
 *      This function is used to set the HEC mode of HEAC interface. 
 * Input:
 *      heac_id           -  HEAC interface id (0~4)
 *      mode              -  HEC operation mode of HEAC interface
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 *      RT_ERR_NOT_ALLOWED
 * Note:
 *      For source device, it is not allowed to enable HEC when ARC is in
 *   TX Single mode. In this case, the function returns RT_ERR_NOT_ALLOWED
 */
extern rtk_api_ret_t rtk_hec_mode_set(int heac_id, rtk_hec_mode_t mode);


/* Function Name:
 *      rtk_hec_mode_get
 * Description:
 *      This function is used to get the HEC mode of HEAC interface. 
 * Input:
 *      heac_id           -  heac id (0~4)
 * Output:
 *      mode              -  HEC operation mode of HEAC interface
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 */
extern rtk_api_ret_t rtk_hec_mode_get(int heac_id, rtk_hec_mode_t* mode);


/* Function Name:
 *      rtk_arc_mode_set
 * Description:
 *      This function is used to set the ARC mode of HEAC interface. 
 * Input:
 *      heac_id           -  HEAC interface id (0~4)
 *      mode              -  ARC operation mode of HEAC interface
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 *      RT_ERR_NOT_ALLOWED
 * Note:
 *      For source device, it is not allowed to enable ARC TX Single mode
 *   when HEC is enabled. In this case, the function returns RT_ERR_NOT_ALLOWED
 */
extern rtk_api_ret_t rtk_arc_mode_set(int heac_id, rtk_arc_mode_t mode);


/* Function Name:
 *      rtk_arc_mode_get
 * Description:
 *      This function is used to get the ARC mode of HEAC interface. 
 * Input:
 *      heac_id           -  heac id (0~4)
 * Output:
 *      mode              -  ARC operation mode of HEAC interface
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 */
extern rtk_api_ret_t rtk_arc_mode_get(int heac_id, rtk_arc_mode_t* mode);


/* Function Name:
 *      rtk_spdif_set
 * Description:
 *      This function is used to connect the spdif with heac.
 * Input:
 *      spdif_id          -  spdif id(0~1)
 *      heac_id           -  heac id (0~4)
 * Output:
 *      direct            -  direction of SPDIF. 
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 * Note:
 *      This function will adjust the direction of SPDIF according ARC role.
 *
 *      For source devices with mode:
 *          ARC_MODE_RX_BOTH,
 *      RTL8307H will send out SPDIF signal.
 *
 *      For sink devices with mode:
 *          ARC_MODE_TX_COMMON,  
 *       or ARC_MODE_TX_SINGLE,
 *      RTL8307H will receive SPDIF signal.
 */
extern rtk_api_ret_t rtk_spdif_set(int spdif_id, int heac_id);


/* Function Name:
 *      rtk_spdif_get
 * Description:
 *      This function is used to get the status of SPDIF IO, including the connected
 *      HEAC Port and the direction of SPDIF.
 * Input:
 *      spdif_id          -  spdif id(0~1)
 * Output:
 *      heac_id           -  heac id (0~4)
 *      direct            -  direction of SPDIF. 
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 * Note:
 *      For source devices, RTL8307H will send out SPDIF signal(SPDIF_SPDIF_OUTPUT). 
 *      For sink devices, RTL8307H will receive SPDIF signal(SPDIF_SPDIF_INPUT). 
 */
extern rtk_api_ret_t rtk_spdif_get(int spdif_id, int* heac_id, rtk_spdif_dir_t* spdif_dir);

 
/* Function Name:
 *      rtk_port_mode_get
 * Description:
 *      Get port type. 
 * Input:
 *      port                    -  port id (0~6)
 * Output:     
 *      mode                    -  the pointer of port mode
 * Return: 
 *      RT_ERR_CHIP_NOT_SUPPORTED
 *      RT_ERR_PORT_ID
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      RT_ERR_FAILED  
 *     
 * Note:
 *      Port 0~3 are HEAC ports so the mode is MODE_EXT_HEAC
 *      Port 4 is giga port, so the mode is MODE_EXT_GIGA 
 *      Port 5 supports following modes:
 *           MODE_EXT_RGMII_NODELAY
 *           MODE_EXT_RGMII_RXDELAY_ONLY
 *           MODE_EXT_RGMII_TXDELAY_ONLY
 *           MODE_EXT_RGMII_RXTXDELAY
 *           MODE_EXT_TMII_MII_MAC
 *           MODE_EXT_MII_PHY
 *           MODE_EXT_TMII_PHY
 *           MODE_EXT_RMII_INPUT
 *           MODE_EXT_RMII_OUTPUT
 *      Port 6 supports following modes:
 *           MODE_EXT_TMII_MII_MAC
 *           MODE_EXT_MII_PHY
 *           MODE_EXT_TMII_PHY
 *           MODE_EXT_RMII_INPUT
 *           MODE_EXT_RMII_OUTPUT 
 *           MODE_EXT_HEAC,
 *           MODE_EXT_FAST_ETHERNET,
 */
extern rtk_api_ret_t rtk_port_mode_get(rtk_port_t port, rtk_mode_ext_t* mode);

/* Function Name:
 *      rtk_port_mii_clk_set
 * Description:
 *      The pins of (R)MII/TMII/RGMII interface can be 
 *      put to high-impendence state when port is link down. 
 *      This function is used to set the configuration.
 * Input:
 *      port              -  Port id (PN_PORT5,PN_PORT6)
 *      rtk_enable_t      -  DISABLED/ENABLED
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 *     
 * Note:
 *      
 */
extern rtk_api_ret_t rtk_port_mii_clk_set(rtk_port_t port, rtk_enable_t enable);


/* Function Name:
 *      rtk_port_mii_clk_get
 * Description:
 *      The pins of (R)MII/TMII/RGMII interface can be 
 *      put to high-impendence state when port is link down. 
 *      This function is used to get the configuration.
 * Input:
 *      port              -  Port id (PN_PORT5,PN_PORT6)
 *      pEnable           -  pointer of the ability: DISABLED/ENABLED
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_PORT_ID
 *     
 * Note:
 *      
 */
extern rtk_api_ret_t rtk_port_mii_clk_get(rtk_port_t port, rtk_enable_t* pEnable);


/* Function Name:
 *      rtk_port_linkAbility_set
 * Description:
 *      Set port link ability
 * Input:
 *      port              -  Port id
 *      pPortability      -  port link ability
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_FAILED  
 *      RT_ERR_PORT_ID
 *      RT_ERR_SMI       
 *      RT_ERR_NULL_POINTER
 *     
 * Note:
 *      When port 5 is bonding as (R)MII/TMII port, 10M/100Mbps speed configuration
 *      is supported but 1000Mbps is not supported. 1000Mbps is supported when port 5 is bonding as RGMII port  
 *      with mode MODE_EXT_RGMII_NODELAY or MODE_EXT_RGMII_RXDELAY_ONLY or MODE_EXT_RGMII_TXDELAY_ONLY
 *          or MODE_EXT_RGMII_RXTXDELAY. *
 *      Error RT_ERR_MAC_FORCE_1000 is returned when configure 1000Mbps to (R)MII/TMII port.
 *
 *      When port 6 is configurated as (R)MII/TMII port, 10M/100Mbps speed configuration
 *      is supported but 1000Mbps is not supported.
 */
extern rtk_api_ret_t rtk_port_linkAbility_set(rtk_port_t port, rtk_port_link_ability_t *pPortability);


/* Function Name:
 *      rtk_port_macForceLink_get
 * Description:
 *      Get port link ability
 * Input:
 *      port              -  Port id
 * Output:
 *      pPortability      -  the pointer of port link ability 
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_FAILED  
 *       RT_ERR_PORT_ID
 *      RT_ERR_SMI       
 *      RT_ERR_NULL_POINTER
 *     
 * Note:
 *      structure member link means only when the port is (R)MII/TMII/GMII port.
 */
extern rtk_api_ret_t rtk_port_linkAbility_get(rtk_port_t port, rtk_port_link_ability_t *pPortability);


/* Function Name:
 *      rtk_port_linkStatus_get
 * Description:
 *      Get port mac linking status
 * Input:
 *      port             -  port id
 * Output:
 *      pStatus          -  port link status
 * Return:
 *      RT_ERR_OK                        
 *      RT_ERR_FAILED                  
 *      RT_ERR_SMI       
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *     
 * Note:
 *      the API could get port status(link/duplex/speed/nway/txpause/rxpause/forcemode).
 */
extern rtk_api_ret_t rtk_port_linkStatus_get(rtk_port_t port, rtk_port_link_status_t *pStatus);


/* Function Name:
 *      rtk_wol_event_set
 * Description:
 *      When one of interested events occurs, a wake-up signal will be sent
 *      out to external CPU.
 *      The interested event will be:
 *          WOL_EVENT_LINKUP        -   port link up
 *          WOL_EVENT_MAGIC_PKT     -   AMD Magic packet
 *          WOL_EVENT_WAKEUP_PKT    -   Wake up frame 
 *          WOL_EVENT_ANY_PKT_PORT5 -   Any packet transmitting to port 5
 *          WOL_EVENT_ANY_PKT_PORT6 -   Any packet transmitting to port 6
 * Input:
 *      event_mask                   -  event mask. 
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      These events can be bitwised OR. 
 */
extern rtk_api_ret_t rtk_wol_event_set(uint8 event_mask);


/* Function Name:
 *      rtk_wol_event_get
 * Description:
 *      When one of interested events occurs, a wake-up signal will be sent
 *      out to external CPU.
 *      The interested event will be:
 *          WOL_EVENT_LINKUP        -   port link up
 *          WOL_EVENT_MAGIC_PKT     -   AMD Magic packet
 *          WOL_EVENT_WAKEUP_PKT    -   Wake up frame 
 *          WOL_EVENT_ANY_PKT_PORT5 -   Any packet transmitting to port 5
 *          WOL_EVENT_ANY_PKT_PORT6 -   Any packet transmitting to port 6
 *      This function gets the interested events list. 
 * Input:
 *      event_mask                   -  event mask. 
 * Return: 
 *      RT_ERR_OK
 */
extern rtk_api_ret_t rtk_wol_event_get(uint8* event_mask);


/* Function Name:
 *      rtk_wol_nodeid_set
 * Description:
 *      This function is called when AMD Magic Packet is one of the interested events. 
 *      see function rtk_wol_event_set(). 
 *      Once AMD Magic Packet is selected as one of the interested events, node id should 
 *      be initialized.
 * Input:
 *      node_group              -  Group number which can be either 1, 2 or 3. 
 *      node_id                 -  A six byte array, usually switch own MAC.
 * Output:
 *      None
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      
 */
extern rtk_api_ret_t rtk_wol_nodeid_set(uint8 node_group, uint8* node_id);


/* Function Name:
 *      rtk_wol_nodeid_get
 * Description:
 *      This function is called when AMD Magic Packet is one of the interested events. 
 *      see function rtk_wol_event_set(). 
 *      Once AMD Magic Packet is selected as one of the interested events, node id should 
 *      be initialized. This function is used to get the contents of node id for specific group.
 * Input:
 *      node_group              -  Group number which can be either 1, 2 or 3. 
 * Output:
 *      valid                   -  Whether the content of node_id is valid: 1 - valid; 0 - invalid
 *      node_id                 -  The content of node_id, a six byte array.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      
 */
extern rtk_api_ret_t rtk_wol_nodeid_get(uint8 node_group, uint8* valid, uint8* node_id);


/* Function Name:
 *      rtk_wol_wakup_sample_set
 * Description:
 *      This function is called when wake up frame is one of the interested events. 
 *      see function rtk_wol_event_set(). 
 *      Once wake up frame is selected as one of the interested events, wake up sample frame
 *      should be initialized.
 * Input:
 *      group                   -  Group number which can be either 1, 2. 
 *      frame                   -  A sample packet, starting from DMAC. The length of the packet should be no more than 128 bytes. 
 *      len                     -  the length of the packet. It should be no more than 128 bytes.
 *      mask                    -  A 16-byte array, used for masking at most 128-byte sample packet. 
 * Output:
 *      None
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      When bit n in mask is 1, the corresponding byte n in the frame is valid and will be compared
 *      Otherwise, the byte n in the frame is ignored.
 *      The length of the sample packet should be no more than 128 bytes. The bytes above 128 bytes are 
 *      ignored.
 */
extern void rtk_wol_wakup_sample_set(uint8 group, uint8* frame, uint16 len, uint8* mask);


/* Function Name:
 *      rtk_wol_wakup_sample_get
 * Description:
 *      This function is called when wake up frame is one of the interested events. 
 *      see function rtk_wol_event_set(). 
 *      Once wake up frame is selected as one of the interested events, wake up sample frame
 *      should be initialized.
 *      This function is used to get the contents of node id for specific group.
 * Input:
 *      group                   -  Group number which can be either 1, 2. 
 * Output:
 *      mask                    -  A 16-byte array, used for masking the 128-byte sample packet. 
 *      crc                     -  Crc value.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      When bit n in mask is 1, the corresponding byte n in the frame is valid and will be compared
 *      Otherwise, the byte n in the frame is ignored.    
 */
extern void rtk_wol_wakup_sample_get(uint8 group, uint8* mask, uint16* crc);


/* Function Name:
 *      rtk_wol_sig_type_set
 * Description:
 *      When one of interested events occurs, a wake-up signal will be sent
 *      out to external CPU via WOL pin. 
 *      This function is used to set wake-up signal type.
 *      There are four types we can choose:
 *          LEVEL_HIGH              -   high voltage level
 *          LEVEL_LOW               -   low voltage level
 *          PULSE_POSITIVE          -   positive pulse with duration 150ms
 *          PULSE_NEG               -   negative pulse with duration 150ms
 *      When PULSE_POSITIVE or PULSE_NEG option is selected, there are four different time intervals between pulse.
 *              WOL_16NS            -   16   ns
 *              WOL_128NS           -   128  ns
 *              WOL_2048NS          -   2048 ns
 *              WOL_65536NS         -   65536ns
 * Input:
 *      type                    -  Wake-up signal type.
 *      interval                -  Time intervals between pulse when the signal type is either PULSE_POSITIVE or PULSE_NEG.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *     This wake-up signal is sent out via WOL pin. It is irrelative with switch interrupt registers(WOLINT_IE_CPU, WOLPKT_DROP_IE_CPU, WOLPKT_DROP_IP_CPU).
 */
extern rtk_api_ret_t rtk_wol_sig_type_set(rtk_wol_intr_type_t type, rtk_wol_intr_pulse_interval_t interval);


/* Function Name:
 *      rtk_wol_sig_type_get
 * Description:
 *      When one of interested events occurs, a wake-up signal will be sent
 *      out to external CPU via WOL pin.
 *      This function is used to get wake-up signal type.
 *      There are four types for the wake-up signal:
 *          LEVEL_HIGH              -   high voltage level
 *          LEVEL_LOW               -   low voltage level
 *          PULSE_POSITIVE          -   positive pulse with duration 150ms
 *          PULSE_NEG               -   negative pulse with duration 150ms
 *      When PULSE_POSITIVE or PULSE_NEG option is selected, there are four different time intervals between pulse.
 *              WOL_16NS            -   16   ns
 *              WOL_128NS           -   128  ns
 *              WOL_2048NS          -   2048 ns
 *              WOL_65536NS         -   65536ns
 * Input:
 *      None
 * Output:
 *      type                    -  Wake-up signal type.
 *      interval                -  Time intervals between pulse when the signal type is either PULSE_POSITIVE or PULSE_NEG.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *     This wake-up signal is sent out via WOL pin. It is irrelative with switch interrupt registers(WOLINT_IE_CPU, WOLPKT_DROP_IE_CPU, WOLPKT_DROP_IP_CPU). 
 */
extern rtk_api_ret_t rtk_wol_sig_type_get(rtk_wol_intr_type_t* type, rtk_wol_intr_pulse_interval_t* interval);


/* Function Name:
 *      rtk_wol_sig_status_get
 * Description:
 *      When a wake-up signal is sent, pending flag will be set to 1.
 *      This flag can be cleared by external CPU using rtk_wol_sig_status_clear().
 * Input:
 *      pending                 -  Flag specify whether the a wake-up signal is sent.
 * Output:
 *      None
 * Return: 
 *      None     
 * Note:
 *     This wake-up signal is sent out via WOL pin. It is irrelative with switch interrupt registers(WOLINT_IE_CPU, WOLPKT_DROP_IE_CPU, WOLPKT_DROP_IP_CPU).    
 */
extern void rtk_wol_sig_status_get(uint8* pending);


/* Function Name:
 *      rtk_wol_sig_status_clear
 * Description:
 *      When a wake-up signal is sent, pending flag will be set to 1.
 *      This function is used to clear the pending flag.
 * Input:
 *      None
 * Output:
 *      None
 * Return: 
 *      None     
 * Note:
 *     This wake-up signal is sent out via WOL pin. It is irrelative with switch interrupt registers(WOLINT_IE_CPU, WOLPKT_DROP_IE_CPU, WOLPKT_DROP_IP_CPU).    
 */
extern void rtk_wol_sig_status_clear(void);


/* Function Name:
 *      rtk_wol_enter_sleep
 * Description:
 *      After external CPU has initialized WOL function, please see the programming guide for initialization.
 *      Function rtk_wol_enter_sleep() will be called when external CPU wants to sleep.
 *      Once this function is called and one of interested events occurs,
 *      a wake-up signal will be sent out from RTL8307H to external CPU.
 * Input:
 *      port                    -  port id
 * Output:
 *      None
 * Return: 
 *      RT_ERR_PORT_ID
 *      RT_ERR_OK
 *      RT_ERR_FAILED     
 * Note:
 */
extern rtk_api_ret_t rtk_wol_enter_sleep(uint32 port);


/* Function Name:
 *      rtk_wol_exit_sleep
 * Description:
 *      After rtk_wol_enter_sleep() is called and one of interested events occurs,
 *      a wake-up signal will be sent out from RTL8307H to external CPU.
 *      This function should be called to put switch back to original state.
 * Input:
 *      port                    -  port id
 * Output:
 *      None
 * Return: 
 *      RT_ERR_PORT_ID
 *      RT_ERR_OK
 *      RT_ERR_FAILED     
 * Note:
 *      After calling this function, interrupt pending flag at external CPU should be cleared.
 */
extern rtk_api_ret_t rtk_wol_exit_sleep(uint32 port);


/* Function Name:
 *      rtk_wol_magicpacket_behaviour_set
 * Description:
 *      This function is used to set the behaviour of switch when receiving a 
 *      AMD Magic packet. There are two behaviour can selected: Drop or forward the packet.
 *          
 * Input:
 *      type                    -  Drop or forward the AMD Magic packet.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      
 */
extern void rtk_wol_magicpacket_behaviour_set(rtk_wol_pkt_behaviour_t type);


/* Function Name:
 *      rtk_wol_magicpacket_behaviour_get
 * Description:
 *      This function is used to get the behaviour of switch when receiving a 
 *      AMD Magic packet. There may be two behaviour: Drop or forward the packet.  
 * Input:
 *      None
 * Output:
 *      type                    -  Drop or forward the AMD Magic packet.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      
 */
extern void rtk_wol_magicpacket_behaviour_get(rtk_wol_pkt_behaviour_t* type);

/* Function Name:
 *      rtk_wol_wakuppacket_behaviour_set
 * Description:
 *      This function is used to set the behaviour of switch when receiving a 
 *      wake up packet. There are two behaviour can selected: Drop or forward the packet.
 *          
 * Input:
 *      type                    -  Drop or forward the wake up.
 * Output:
 *      None
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      
 */
extern void rtk_wol_wakuppacket_behaviour_set(rtk_wol_pkt_behaviour_t type);

/* Function Name:
 *      rtk_wol_wakuppacket_behaviour_get
 * Description:
 *      This function is used to get the behaviour of switch when receiving a 
 *      wake up packet. There may be two behaviour: Drop or forward the packet.  
 * Input:
 *      None
 * Output:
 *      type                    -  Drop or forward the wake up.
 * Return: 
 *      RT_ERR_OK     
 * Note:
 *      
 */
extern void rtk_wol_wakuppacket_behaviour_get(rtk_wol_pkt_behaviour_t* type);

/* Function Name:
 *      rtk_igmp_init
 * Description:
 *      initialize igmp function  
 * Input:
 *      igmpEnable  -  enable or disable igmp function, the value should be 0x0 or 0x1
 *      mldEnable  -  enable or disable mld function, the value should be 0x0 or 0x1
 *      vlanLeakyEnable  -  enable or disable cross vlan function, the value should be 0x0 or 0x1
 *      fastLeaveEnable  -  enable or disable fast leave function, the value should be 0x0 or 0x1
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can initialize igmp parameters, including igmp/mld enable, cross_vlan, fast leave enable. mld enable should base on fixed fid. 
 */
extern rtk_api_ret_t rtk_igmp_init(rtk_enable_t igmpEnable, rtk_enable_t mldEnable, rtk_enable_t vlanLeakyEnable, rtk_enable_t fastLeaveEnable);

/* Function Name:
 *      rtk_igmp_getInitConfig
 * Description:
 *      get igmp function configuration    
 * Input:
 *      none
 * Output:
 *      igmpEnable  -  igmp function enabled or disabled, the value should be 0x0 or 0x1
 *      mldEnable  -  mld function enabled or disabled, the value should be 0x0 or 0x1
 *      vlanLeakyEnable  -  cross vlan function enabled or disabled, the value should be 0x0 or 0x1
 *      fastLeaveEnable  -   fast leave function enabled or disabled, the value should be 0x0 or 0x1
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can get igmp function configuration 
 */
extern rtk_api_ret_t rtk_igmp_getInitConfig(rtk_enable_t *igmpEnable, rtk_enable_t *mldEnable, rtk_enable_t *vlanLeakyEnable, rtk_enable_t *fastLeaveEnable);
/* Function Name:
 *      rtk_igmp_cpuPortMsk_set
 * Description:
 *      set igmp cpu port mask  
 * Input:
 *      igmpCpuPortMsk  -  igmp cpu port mask, the value should be 0x0~0x7
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can set the cpu port mask when igmp packet trapped to cpu
 */
extern rtk_api_ret_t rtk_igmp_cpuPortMsk_set(rtk_portmask_t *igmpCpuPortMsk);

/* Function Name:
 *      rtk_igmp_cpuPortMsk_get
 * Description:
 *      get igmp cpu port mask     
 * Input:
 *      none
 * Output:
 *      igmpCpuPortMsk  -  igmp cpu port mask, the value should be 0x0~0x7
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can get the cpu port mask when igmp packet trapped to cpu
 */
extern rtk_api_ret_t rtk_igmp_cpuPortMsk_get(rtk_portmask_t *igmpCpuPortMsk);

/* Function Name:
 *      rtk_igmp_pktTrap_set
 * Description:
 *      set the action on igmp/mld pkt
 * Input:
 *      igmpTrap  - the  action on igmp pkt
 *      mldTrap  -  the  action on mld pkt
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE  
 *      RT_ERR_FAILED
 * Note:
 *      This API can set the action on igmp/mld pkt;
 *      Input should be:
 *      0x0       Forward
 *      0x1       Trap(to IGMP cpu port mask )
 *      0x2       Copy to cpu
 *      0x3       Drop
 */
extern rtk_api_ret_t rtk_igmp_pktTrap_set(rtk_trap_misc_action_t igmpTrap, rtk_trap_misc_action_t mldTrap);

/* Function Name:
 *      rtk_igmp_pktTrap_get
 * Description:
 *      get the action on igmp/mld pkt    
 * Input:
 *      none
 * Output:
 *      igmpTrap  - the  action on igmp pkt
 *      mldTrap  -  the  action on mld pkt
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can get the action on igmp/mld pkt
 *      Output should be:
 *      0x0       Forward
 *      0x1       Trap(to IGMP cpu port mask )
 *      0x2       Copy to cpu
 *      0x3       Drop
 */
extern rtk_api_ret_t rtk_igmp_pktTrap_get(rtk_trap_misc_action_t *igmpTrap, rtk_trap_misc_action_t *mldTrap);

/* Function Name:
 *      rtk_igmp_defPri_set
 * Description:
 *      Define Priority or not when IGMP/MLD packets trapped to CPU    
 * Input:
 *      defIgmpPri  -  define priority or not, the value should be 0x0 or 0x1
 *      igmpPri  -  priority defined, the value should be 0x0~0x7
 * Output:
 *     none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can define Priority  when IGMP/MLD packets trapped to CPU
 */
rtk_api_ret_t rtk_igmp_defPri_set(rtk_enable_t defIgmpPri, rtk_pri_t igmpPri);
/* Function Name:
 *      rtk_igmp_defPri_set
 * Description:
 *      Define Priority or not when IGMP/MLD packets trapped to CPU    
 * Input:
 *      none
 * Output:
 *      defIgmpPri  -  define priority or not,  the value should be 0x0 or 0x1
 *      igmpPri  -  priority defined, the value should be 0x0~0x7
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENABLE    
 *      RT_ERR_FAILED
 * Note:
 *      This API can define Priority  when IGMP/MLD packets trapped to CPU
 */
rtk_api_ret_t rtk_igmp_defPri_get(rtk_enable_t* defIgmpPri, rtk_pri_t* igmpPri);


/* Function Name:
 *      rtk_stp_mstpState_set
 * Description:
 *       This API is used to set spanning tree port states in some instance
 * Input:
 *      msti                 - spanning tree instance id, from 0 to 14.
 *      portId              - port id, from 0 to 7.
 *      stp_state         -  port states, from STP_DISCARDING to STP_FORWARDING.
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID
 *     RT_ERR_MSTI
 *     RT_ERR_MSTP_STATE
 *     RT_ERR_OK                           
 * Note:
 *      None.
 */
extern rtk_api_ret_t rtk_stp_mstpState_set(rtk_stp_msti_id_t msti, rtk_port_t portId, rtk_stp_state_t stp_state);

/* Function Name:
 *      rtk_stp_mstpState_get
 * Description:
 *       This API is used to get spanning tree port states in some instance
 * Input:
 *      msti                - spanning tree instance id, from 0 to 14.
 *      portId              - port id, from 0 to 7.
 * Output:
 *      pStp_state      - the point to port states.
 * Return:
 *     RT_ERR_PORT_ID
 *     RT_ERR_MSTI
 *     RT_ERR_NULL_POINTER
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_stp_mstpState_get(rtk_stp_msti_id_t msti, rtk_port_t portId, rtk_stp_state_t *pStp_state);

/* Function Name:
 *      rtk_stp_ability_set
 * Description:
 *       This API is used to enable or disable 8051 STP
 * Input:
 *      stpAbility         - 8051 stp ability, from DISABLED to ENABLED 
 * Output:
 *      none
 * Return:
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
extern rtk_api_ret_t rtk_stp_ability_set(rtk_enable_t stpAbility);

/* Function Name:
 *      rtk_stp_ability_get
 * Description:
 *       This API is used to get spanning tree ability
 * Input:
 *      none
 * Output:
 *      pStpAbility         -the pointer to  8051 stp ability 
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_stp_ability_get(rtk_enable_t* pStpAbility);

/* Function Name:
 *      rtk_stp_type_set
 * Description:
 *       This API is used to switch 8051 STP protocol between STP and RSTP
 * Input:
 *      stpType         -STP type, from STP_TYPE_STP to STP_TYPE_RSTP 
 * Output:
 *      none
 * Return:
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
extern rtk_api_ret_t rtk_stp_type_set(rtk_stp_type_t stpType);

/* Function Name:
 *      rtk_stp_type_get
 * Description:
 *       This API is used to get spanning tree type
 * Input:
 *      none
 * Output:
 *      pStpType         -the pointer to  spanning tree type
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_stp_type_get(rtk_stp_type_t* pStpType);

/* Function Name:
 *      rtk_stp_bridgePriority_set
 * Description:
 *       This API is used to set switch priority in step of 4096(0x1000)
 * Input:
 *      stpBridgePriority         - the switch priority in step of 4096, from 0 to 15 
 * Output:
 *      none
 * Return:
 *     RT_ERR_STP_BRIDGE_PRIORITY 
 *     RT_ERR_OK                           
 * Note:
*      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
extern rtk_api_ret_t rtk_stp_bridgePriority_set(rtk_stp_bdg_pri_t stpBridgePriority);

/* Function Name:
 *      rtk_stp_bridgePriority_get
 * Description:
 *       This API is used to get switch prioriy
 * Input:
 *      none
 * Output:
 *      pStpBridgePriority         -the pointer to  spanning tree switch prioriy
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_stp_bridgePriority_get(rtk_stp_bdg_pri_t* pStpBridgePriority);

/* Function Name:
 *      rtk_stp_maxAge_set
 * Description:
 *       This API is used to set max age time, from 6s to 40s
 * Input:
 *      stpMaxAge         - spanning tree max age time, from 6 to 40
 * Output:
 *      none
 * Return:
 *     RT_ERR_STP_MAXAGE 
 *     RT_ERR_OK                           
 * Note:
*      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
extern rtk_api_ret_t rtk_stp_maxAge_set(rtk_stp_maxAdge_t stpMaxAge);

/* Function Name:
 *      rtk_stp_maxAge_get
 * Description:
 *       This API is used to get max age
 * Input:
 *      none
 * Output:
 *      pStpMaxAge         - the pointer to spanning tree max age
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_stp_maxAge_get(rtk_stp_maxAdge_t* pStpMaxAge);

/* Function Name:
 *      rtk_stp_forwardDelay_set
 * Description:
 *       Set forward delay
 * Input:
 *      stpForwardDelay         - spanning tree forward delay
 * Output:
 *      none
 * Return:
 *     RT_ERR_STP_FORWARDDELAY 
 *     RT_ERR_OK                           
 * Note:
 *      This API can set STP forward delay from 4s to 30s
 */
extern rtk_api_ret_t rtk_stp_forwardDelay_set(rtk_stp_fowardDelay_t stpForwardDelay);

/* Function Name:
 *      rtk_stp_forwardDelay_get
 * Description:
 *       Get forward delay
 * Input:
 *      none
 * Output:
 *      pStpForwardDelay         -the pointer to  spanning tree forward delay
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      This API can get  STP forward delay from 4s to 30s
 */
extern rtk_api_ret_t rtk_stp_forwardDelay_get(rtk_stp_fowardDelay_t* pStpForwardDelay);

/* Function Name:
 *      rtk_stp_helloTime_set
 * Description:
 *       This API is used to set hello time from 1s to 2s
 * Input:
 *      stpHelloTime         - spanning tree hello time, from 1 to 2
 * Output:
 *      none
 * Return:
 *     RT_ERR_STP_HELLOTIME 
 *     RT_ERR_OK                           
 * Note:
*      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
extern rtk_api_ret_t rtk_stp_helloTime_set(rtk_stp_helloTime_t stpHelloTime);

/* Function Name:
 *       rtk_stp_helloTime_get
 * Description:
 *       This API is used to get hello time
 * Input:
 *      none
 * Output:
 *      pStpHelloTime         - the pointer to spanning tree hello time
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_stp_helloTime_get(rtk_stp_helloTime_t* pStpHelloTime);

/* Function Name:
 *      rtk_stp_portPriority_set
 * Description:
 *      This API is used to set port priority in step of 16(0x10)
 * Input:
 *      portId                         - port id, from 0 to 7
 *      stpPortPriority         - spanning tree port priority, from 0 to 15
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_STP_PORT_PRIORITY 
 *     RT_ERR_OK                           
 * Note:
*      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
extern rtk_api_ret_t rtk_stp_portPriority_set(rtk_port_t portId, rtk_stp_port_pri_t stpPortPriority);

/* Function Name:
 *      rtk_stp_portPriority_get
 * Description:
 *      This API is used to get port priority
 * Input:
 *      portId                            - pport id, from 0 to 7
 * Output:
 *      pStpPortPriority         - the pointer to spanning tree port prioriy
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_stp_portPriority_get(rtk_port_t portId, rtk_stp_port_pri_t* pStpPortPriority);

/* Function Name:
 *      rtk_stp_portPathCost_set
 * Description:
 *       This API is used to set port path cost
 * Input:
 *      portId                           - port id, from 0 to 7
 *      stpPortPathCost         - spanning tree port path cost, from 0 to 15
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_STP_PORT_PATH_COST 
 *     RT_ERR_OK                           
 * Note:
 *     port path cost value:
 *           0: stands for auto, STP will decide the path cost according to port's speed and duplex.
 *           1: stands for port speed 100Kbps, path cost=200000000
 *           2: stands for port speed 1Mbps, path cost=20000000
 *           3: stands for port speed 10Mbps Half duplex, path cost=2000000
 *           4: stands for port speed 10Mbps Full duplex, path cost=1999999
 *           5: stands for port speed 10Mbps Link aggregation, path cost=1000000
 *           6: stands for port speed 100Mbps Half duplex, path cost=200000
 *           7: stands for port speed 100Mbps Full duplex, path cost=199999
 *           8: stands for port speed 100Mbps Link aggregation, path cost=100000
 *           9: stands for port speed 1Gbps Full duplex, path cost=20000
 *           10: stands for port speed 1Gbps, path cost=19999
 *           11: stands for port speed 1Gbps Link aggregation, path cost=10000
 *           12: stands for port speed 10Gbps, path cost=2000
 *           13: stands for port speed 100Gbps, path cost=200
 *           14: stands for port speed 1Tbps, path cost=20
 *           15: stands for port speed 10Tbps, path cost=2
 *
 *      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
extern rtk_api_ret_t rtk_stp_portPathCost_set(rtk_port_t portId, rtk_stp_portPathCost_t stpPortPathCost);

/* Function Name:
 *      rtk_stp_portPathCost_get
 * Description:
 *       This API is used to get port path cost
 * Input:
 *      portId                               - port id, from 0 to 7
 * Output:
 *      pStpPortPathCost         - the pointer to spanning tree port path cost
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_stp_portPathCost_get(rtk_port_t portId, rtk_stp_portPathCost_t* pStpPortPathCost);

/* Function Name:
 *      rtk_stp_portEdge_set
 * Description:
 *       This API is used to set port edge attribution
 * Input:
 *      portId                    - port id, from 0 to 7
 *      stpPortEdge         - spanning tree port edge attribution, from STP_EDGE_AUTO to STP_EDGE_ADMIN
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
extern rtk_api_ret_t rtk_stp_portEdge_set(rtk_port_t portId, rtk_stp_portEdge_t  stpPortEdge);

/* Function Name:
 *      rtk_stp_portEdge_get
 * Description:
 *       This API is used to get port edge attribution
 * Input:
 *      portId                          - port id, from 0 to 7
 * Output:
 *      pStpPortEdge             - the pointer to spanning tree port edge attribution
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_stp_portEdge_get(rtk_port_t portId, rtk_stp_portEdge_t*pStpPortEdge);

/* Function Name:
 *      rtk_stp_portP2P_set
 * Description:
 *       This API is used to set port p2p attribution
 * Input:
 *      portId                       - port id, from 0 to 7
 *      stpPortP2P              - spanning tree port P2P attribution, from STP_P2P_AUTO to STP_P2P_FALSE
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 */
extern rtk_api_ret_t rtk_stp_portP2P_set(rtk_port_t portId, rtk_stp_portP2P_t stpPortP2P);

/* Function Name:
 *      rtk_stp_portP2P_get
 * Description:
 *       This API is used to get port p2p attribution
 * Input:
 *      portId                        - port id, from 0 to 7
 * Output:
 *      pStpPortP2P             - the pointer to spanning tree port p2p attribution
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_stp_portP2P_get(rtk_port_t portId, rtk_stp_portP2P_t* pStpPortP2P);

/* Function Name:
 *      rtk_stp_portMigrationCheck_set
 * Description:
 *       This API is used to trigger port migration check
 * Input:
 *      portId                 - port id, from 0 to 7
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_OK                           
 * Note:
 *      After using this API, user should call API-rtk_stp_parameterChangeIntTrigger to announce 8051 to reconfigure its STP
 *      When migration check is done, the related register bit will be cleared to 0 automatically.   
 */
extern rtk_api_ret_t rtk_stp_portMigrationCheck_set(rtk_port_t portId);

/* Function Name:
 *      rtk_stp_parameterChangeIntTrigger
 * Description:
 *       This API is used to trigger parameter change interrupt to announce 8051 to reconfigure its STP
 * Input:
 *      none
 * Output:
 *      none
 * Return:
 *     RT_ERR_OK                    
 *     RT_ERR_STP_PARAMETER_CHANGE_INT_DONE         
 *     RT_ERR_STP_PARAMETER_CHANGE_INT_CLEAR     
 * Note:
 *     RT_ERR_STP_PARAMETER_CHANGE_INT_CLEAR  means time out of interrupt clear.
 *     RT_ERR_STP_PARAMETER_CHANGE_INT_DONE  means time out of 8051 reconfiguration STP
 */
extern rtk_api_ret_t rtk_stp_parameterChangeIntTrigger(void);

/* Function Name:
 *      rtk_qos_priSel_set
 * Description:
 *       This API is used to configure the priority extraction arbitration table
 * Input:
 *      pPriDec                 -the pointer to priority weight configuration
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_SOURCE 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      pPriDec->group stands for the abitration table index, from 0 to 1.
 *      pPriDec->port_pri stands for the weight of port-based priority, from 0 to 7.
 *      pPriDec->dot1q_pri  stands for the weight of  Dot1q port-base priority, from 0 to 7.
 *      pPriDec->dscp_pri stands for the weight of  Dscp priority, from 0 to 7.
 *      pPriDec->cvlan_pri stands for the weight of  Cvlan priority, from 0 to 7.
 *      pPriDec->svlan_pri stands for the weight of  Svlan priority, from 0 to 7
 */
extern rtk_api_ret_t rtk_qos_priSel_set(rtk_priority_select_t *pPriDec);

/* Function Name:
 *      rtk_qos_priSel_get
 * Description:
 *       This API is used to get configuration of  priority extraction arbitration table
 * Input:
 *      pPriDec                 -the pointer to priority weight configuration, set pPriDec->group to choose the group which you want to get 
 * Output:
 *      pPriDec                 -the pointer to priority weight configuration
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      pPriDec->group stands for the abitration table index, from 0 to 1.
 *      pPriDec->port_pri stands for the weight of port-based priority, from 0 to 7.
 *      pPriDec->dot1q_pri  stands for the weight of  Dot1q port-base priority, from 0 to 7.
 *      pPriDec->dscp_pri stands for the weight of  Dscp priority, from 0 to 7.
 *      pPriDec->cvlan_pri stands for the weight of  Cvlan priority, from 0 to 7.
 *      pPriDec->svlan_pri stands for the weight of  Svlan priority, from 0 to 7
 */
extern rtk_api_ret_t rtk_qos_priSel_get(rtk_priority_select_t *pPriDec);

/* Function Name:
 *      rtk_qos_portCvlanPriCopy_set
 * Description:
 *       Set port cvlan priority source
 * Input:
 *      portId                           -port id, from 0 to 7
 *      priorirtyCopySwap    -the priority source
 * Output:
 *      pPriDec                 -the pointer to priority weight configuration
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      This API can set  port cvlan priority source
 */
extern rtk_api_ret_t rtk_qos_portCvlanPriCopy_set(rtk_port_t portId, rtk_rx_priority_copySwap_t priorirtyCopySwap);

/* Function Name:
 *      rtk_qos_portCvlanPriCopy_get
 * Description:
 *       Get port cvlan priority source
 * Input:
 *      portId                                  - port id
 * Output:
 *      pPriorirtyCopySwap              -the priority source
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_FAILED 
 *     RT_ERR_OK                           
 * Note:
 *      This API can get  port cvlan priority source
 */
extern rtk_api_ret_t rtk_qos_portCvlanPriCopy_get(rtk_port_t portId, rtk_rx_priority_copySwap_t* pPriorirtyCopySwap);

/* Function Name:
 *      rtk_qos_portSvlanPriCopy_set
 * Description:
 *       Set port svlan priority source
 * Input:
 *      portId                                  - port id
 *      priorirtyCopySwap                 -the priority source
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      This API can set  port svlan priority source
 */
extern rtk_api_ret_t rtk_qos_portSvlanPriCopy_set(rtk_port_t portId, rtk_rx_priority_copySwap_t priorirtyCopySwap);

/* Function Name:
 *      rtk_qos_portSvlanPriCopy_get
 * Description:
 *       Get port svlan priority source
 * Input:
 *      portId                                  - port id
 * Output:
 *      pPriorirtyCopySwap              -the priority source
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_FAILED 
 *     RT_ERR_OK                           
 * Note:
 *      This API can get  port svlan priority source
 */
extern rtk_api_ret_t rtk_qos_portSvlanPriCopy_get(rtk_port_t portId, rtk_rx_priority_copySwap_t* pPriorirtyCopySwap);

/* Function Name:
 *      rtk_qos_cvlanPriRemap_set
 * Description:
 *       This API is used to configure cvlan priority mapping table
 * Input:
 *      index                                        - mapping table index, from 0 to 1
 *      innerPriority                           -original priority in ctag, from 0 to 7
 *      mappingPrioirty                      -the mapping priority, from 0 to 7
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_cvlanPriRemap_set(rtk_index_t index, rtk_pri_t innerPriority, rtk_pri_t mappingPrioirty);

/* Function Name:
 *      rtk_qos_cvlanPriRemap_get
 * Description:
 *       This API is used to get cvlan priority mapping table configuration
 * Input:
 *      index                                        - mapping table index, from 0 to 1
 *      innerPriority                           -original priority in ctag, from 0 to 7
 * Output:
 *      pMappingPrioirty                      -the pointer to  mapping priority
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_cvlanPriRemap_get(rtk_index_t index, rtk_pri_t innerPriority, rtk_pri_t* pMappingPrioirty);

/* Function Name:
 *      rtk_qos_svlanPriRemap_set
 * Description:
 *       This API is used to configure svlan priority mapping table
 * Input:
 *      index                                        - mapping table index, from 0 to 1
 *      outerPriority                           -original priority in stag, from 0 to 7
 *      dei                                             -original dei in stag, from 0 to 1
 *      mappingPrioirty                      -the mapping priority, from 0 to 7
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_QOS_1P_DEI 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_svlanPriRemap_set(rtk_index_t index, rtk_pri_t outerPriority, rtk_dei_t  dei, rtk_pri_t mappingPrioirty);

/* Function Name:
 *      rtk_qos_svlanPriRemap_get
 * Description:
 *       This API is used to get svlan priority mapping table configuration
 * Input:
 *      index                                        -mapping table index, from 0 to 1
 *      outerPriority                           -original priority in stag, from 0 to 7
 *      dei                                             -original dei in stag, from 0 to 1
 * Output:
 *      pMappingPrioirty                      -the pointer to  mapping priority
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_QOS_1P_DEI 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_svlanPriRemap_get(rtk_index_t index, rtk_pri_t outerPriority, rtk_dei_t dei, rtk_pri_t* pMappingPrioirty);

/* Function Name:
 *      rtk_qos_dscpPriRemap_set
 * Description:
 *       This API is used to configure dscp priority mapping table
 * Input:
 *      index                                        - mapping table index, from 0 to 1
 *      dscp                                            -dscp value in IP header, from 0 to 63
 *      mappingPrioirty                      -the mapping priority, from 0 to 7
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_DSCP_VALUE 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_dscpPriRemap_set(rtk_index_t index, rtk_dscp_t dscp, rtk_pri_t mappingPrioirty);

/* Function Name:
 *      rtk_qos_dscpPriRemap_get
 * Description:
 *       This API is used to get dscp priority mapping table configuration
 * Input:
 *      index                                        - mapping table index, from 0 to 1
 *      dscp                                            -dscp value in IP header, from 0 to 63
 * Output:
 *      pMappingPrioirty                      -the pionter  to mapping prioirty
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_DSCP_VALUE 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                             
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_dscpPriRemap_get(rtk_index_t index, rtk_dscp_t dscp, rtk_pri_t* pMappingPrioirty);

/* Function Name:
 *      rtk_qos_cvlanRemapTable_set
 * Description:
 *       This API is used for port to select cvlan priority mapping table
 * Input:
 *      portId                                       -port id, from 0 to 7
 *      index                                        -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_cvlanRemapTable_set(rtk_port_t portId, rtk_index_t index);

/* Function Name:
 *      rtk_qos_cvlanRemapTable_get
 * Description:
 *      This API is used to get  port  cvlan mapping table index
 * Input:
 *      portId                                       -port id, from 0 to 7
 * Output:
 *      pIndex                                       -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_cvlanRemapTable_get(rtk_port_t portId, rtk_index_t* pIndex);

/* Function Name:
 *      rtk_qos_svlanRemapTable_set
 * Description:
 *       This API is used for port to select cvlan priority mapping table
 * Input:
 *      portId                                       -port id, from 0 to 7
 *      index                                        -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_svlanRemapTable_set(rtk_port_t portId, rtk_index_t index);

/* Function Name:
 *      rtk_qos_svlanRemapTable_get
 * Description:
 *      This API is used to get  port  svlan mapping table index
 * Input:
 *      portId                                       -port id, from 0 to 7
 * Output:
 *      pIndex                                      -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_svlanRemapTable_get(rtk_port_t portId, rtk_index_t* pIndex);

/* Function Name:
 *      rtk_qos_dscpRemapTable_set
 * Description:
 *       This API is used for port to select dscp priority mapping table
 * Input:
 *      portId                                       -port id, from 0 to 7
 *      index                                        -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_dscpRemapTable_set(rtk_port_t portId, rtk_index_t index);

/* Function Name:
 *      rtk_qos_dscpRemapTable_get
 * Description:
 *      This API is used to get  port  dscp mapping table index
 * Input:
 *      portId                                       -port id, from 0 to 7
 * Output:
 *      pIndex                                      -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_dscpRemapTable_get(rtk_port_t portId, rtk_index_t* pIndex);

/* Function Name:
 *      rtk_qos_PriWeightTable_set
 * Description:
 *       This API is used for port to select priority extraction arbitration table
 * Input:
 *      portId                                      -port id, from 0 to 7
 *      index                                       -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_priWeightTable_set(rtk_port_t portId, rtk_index_t index);

/* Function Name:
 *      rtk_qos_PriWeightTable_get
 * Description:
 *      This API is used to get  port  priority extraction arbitration table index
 * Input:
 *      portId                                   -port id, from 0 to 7
 * Output:
 *      pIndex                                  -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_priWeightTable_get(rtk_port_t portId, rtk_index_t* pIndex);

/* Function Name:
 *      rtk_qos_portPri_set
 * Description:
 *       This API is used to set port-based priority
 * Input:
 *      portId                                -port id, from 0 to 7
 *      int_pri                              -port-based priority, from 0 to 7
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_portPri_set(rtk_port_t portId, rtk_pri_t int_pri) ;

/* Function Name:
 *      rtk_qos_portPri_get
 * Description:
 *       This API is used to get port-based priority
 * Input:
 *      portId                                  -port id, from 0 to 7
 * Output:
 *      pInt_pri                              -pointer to port base priority
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_portPri_get(rtk_port_t portId, rtk_pri_t *pInt_pri) ;

/* Function Name:
 *      rtk_qos_cvlanRemarkEnable_set
 * Description:
 *       This API is used to set egress port ctag priority remark ablity
 * Input:
 *      portId                                      -port id, from 0 to 7
 *      remarkAbility                       -ctag priority remark ability, from DISABLED to ENABLED
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_cvlanRemarkEnable_set(rtk_port_t portId , rtk_enable_t remarkAbility); 

/* Function Name:
 *      rtk_qos_cvlanRemarkEnable_get
 * Description:
 *       This API is used to get egress port ctag priority remark ablity
 * Input:
 *      portId                                      -port id, from 0 to 7
 * Output:
 *      pRemarkAbility                     -the pointer to remark ability
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_cvlanRemarkEnable_get(rtk_port_t portId , rtk_enable_t* pRemarkAbility);

/* Function Name:
 *      rtk_qos_svlanRemarkEnable_set
 * Description:
 *       This API is used to set egress port stag priority remark ablity
 * Input:
 *      portId                                      -port id, from 0 to 7
 *      remarkAbility                       -stag priority remark ability, from DISABLED to ENABLED
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_svlanRemarkEnable_set(rtk_port_t portId , rtk_enable_t remarkAbility); 

/* Function Name:
 *      rtk_qos_svlanRemarkEnable_get
 * Description:
 *       This API is used to get egress port stag priority remark ablity
 * Input:
 *      portId                                      -port id, from 0 to 7
 * Output:
 *      pRemarkAbility                     -the pointer to remark ability
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_svlanRemarkEnable_get(rtk_port_t portId , rtk_enable_t* pRemarkAbility);

/* Function Name:
 *      rtk_qos_dscpRemarkEnable_set
 * Description:
 *       This API is used to set egress port dscp remark ablity
 * Input:
 *      portId                                      -port id, from 0 to 7
 *      remarkAbility                       -dscp remark ability, from DISABLED to ENABLED
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_dscpRemarkEnable_set(rtk_port_t portId, rtk_enable_t remarkAbility);

/* Function Name:
 *      rtk_qos_dscpRemarkEnable_get
 * Description:
 *       This API is used to get egress port dscp remark ablity
 * Input:
 *      portId                                      -port id, from 0 to 7
 * Output:
 *      pRemarkAbility                     -the pointer to remark ability
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_dscpRemarkEnable_get(rtk_port_t portId, rtk_enable_t *pRemarkAbility);

/* Function Name:
 *      rtk_qos_cvlanRemark_set
 * Description:
 *       This API is used to configure egress port ctag priority remark table
 * Input:
 *      index                                       -table index, from 0 to 1
 *      originPriority                        - internal priority, from 0 to 7
 *      remarkPriority                      - new priority which will be remarked to ctag priority, from 0 to 7
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_cvlanRemark_set(rtk_index_t index, rtk_pri_t originPriority, rtk_pri_t remarkPriority);

/* Function Name:
 *      rtk_qos_cvlanRemark_get
 * Description:
 *       This API is used to get egress port ctag priority remark table configuration
 * Input:
 *      index                                       -table index, from 0 to 1
 *      originPriority                        - internal priority, from 0 to 7
 * Output:
 *      pRemarkPriority                      -the pointer to remark priority
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_cvlanRemark_get(rtk_index_t index, rtk_pri_t originPriority, rtk_pri_t* pRemarkPriority);

/* Function Name:
 *      rtk_qos_svlanRemark_set
 * Description:
 *       This API is used to configure egress port stag priority&dei remark table
 * Input:
 *      index                                       -table index, from 0 to 1
 *      originPriority                        - internal priority, from 0 to 7
 *      remarkPriority                      - new priority which will be remarked to stag priority, from 0 to 7
 *      remarkDei                              - new dei which will be remarked to stag dei, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_QOS_1P_DEI 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_svlanRemark_set(rtk_index_t index, rtk_pri_t originPriority,  rtk_pri_t remarkPriority, rtk_dei_t remarkDei);

/* Function Name:
 *      rtk_qos_svlanRemark_get
 * Description:
 *       This API is used to get egress port stag priority&dei remark table configuration
 * Input:
 *      index                                       -table index, from 0 to 1
 *      originPriority                        - internal priority, from 0 to 7
 * Output:
 *      pRemarkPriority                      -the pointer to remark priority
  *      pRemarkDei                             -the pointer to remark dei
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_svlanRemark_get(rtk_index_t index, rtk_pri_t originPriority, rtk_pri_t* pRemarkPriority, rtk_dei_t* pRemarkDei);

/* Function Name:
 *      rtk_qos_dscpRemark_set
 * Description:
 *       This API is used to configure egress port dscp remark table
 * Input:
 *      index                                       -table index, from 0 to 1
 *      originPriority                        - internal priority, from 0 to 7
 *      remarkDscp                             -remark dscp, from 0 to 63
 * Output:
 *      none
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_QOS_DSCP_VALUE 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_dscpRemark_set(rtk_index_t index, rtk_pri_t originPriority, rtk_dscp_t remarkDscp);

/* Function Name:
 *      rtk_qos_dscpRemark_get
 * Description:
 *       This API is used to get egress port dscp remark table configuration
 * Input:
 *      index                                       -table index, from 0 to 1
 *      originPriority                        - internal priority, from 0 to 7
 * Output:
 *      pRemarkDscp                         -the pointer to remark dscp
 * Return:
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      noen
 */
extern rtk_api_ret_t rtk_qos_dscpRemark_get(rtk_index_t index, rtk_pri_t originPriority, rtk_dscp_t* pRemarkDscp);

/* Function Name:
 *      rtk_qos_cvlanRemarkTable_set
 * Description:
 *       This API is used for port to select cvlan priority remarking table
 * Input:
 *      portId                                 -port id, from 0 to 7
 *      index                                  -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_cvlanRemarkTable_set(rtk_port_t portId, rtk_index_t index);

/* Function Name:
 *      rtk_qos_cvlanRemarkTable_get
 * Description:
 *       This API is used  to get port cvlan priority remarking table index
 * Input:
 *      portId                                 -port id, from 0 to 7
 * Output:
 *      pIndex                                -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_cvlanRemarkTable_get(rtk_port_t portId, rtk_index_t* pIndex);

/* Function Name:
 *      rtk_qos_svlanRemarkTable_set
 * Description:
 *       This API is used for port to select svlan priority remarking table
 * Input:
 *      portId                                 -port id, from 0 to 7
 *      index                                  -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_svlanRemarkTable_set(rtk_port_t portId, rtk_index_t index);

/* Function Name:
 *      rtk_qos_svlanRemarkTable_get
 * Description:
 *       This API is used  to get port svlan priority remarking table index
 * Input:
 *      portId                                 -port id, from 0 to 7
 * Output:
 *      pIndex                                -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_svlanRemarkTable_get(rtk_port_t portId, rtk_index_t* pIndex);

/* Function Name:
 *      rtk_qos_dscpRemarkTable_set
 * Description:
 *       This API is used for port to select dscp remarking table
 * Input:
 *      portId                                 -port id, from 0 to 7
 *      index                                  -table index, from 0 to 1
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SEL_PRI_GROUP 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_dscpRemarkTable_set(rtk_port_t portId, rtk_index_t index);

/* Function Name:
 *      rtk_qos_dscpRemarkTable_get
 * Description:
 *       This API is used  to get port dscp remarking table index
 * Input:
 *      portId                                 -port id, from 0 to 7
 * Output:
 *      pIndex                                -the pointer to table index
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_dscpRemarkTable_get(rtk_port_t portId, rtk_index_t* pIndex);

/* Function Name:
 *      rtk_qos_priQueueMapping_set
 * Description:
 *       This API is used for port to configure internal priority to queue mapping table
 * Input:
 *      portId                                 -port id, from 0 to 7
 *      priority                              - internal priority, from 0 to 7
 *      queueId                             - queue id, from 0 to 3
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_priQueueMapping_set(rtk_port_t portId, rtk_pri_t priority,  rtk_queue_num_t queueId);

/* Function Name:
 *      rtk_qos_priQueueMapping_get
 * Description:
 *       This API is used get port  internal priority to queue mapping table configuration
 * Input:
 *      portId                                 - port id, from 0 to 7
 *      priority                              - internal priority, from 0 to 7
 * Output:
 *      pQueueId                             - pointer to queue id
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_1P_PRIORITY 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      This API can get port priority to queue mapping table
 */
extern rtk_api_ret_t rtk_qos_priQueueMapping_get(rtk_port_t portId, rtk_pri_t priority, rtk_queue_num_t* pQueueId);

/* Function Name:
 *      rtk_qos_portScheduler_set
 * Description:
 *       Set port schedule
 * Input:
 *      portId                                       -port id
 *      portSchedule                         - schedule type
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QOS_SCHEDULE 
 *     RT_ERR_OK                           
 * Note:
 *      This API can set port schedule
 */
extern rtk_api_ret_t rtk_qos_portScheduler_set(rtk_port_t portId, rtk_queue_scheduleType_t portSchedule);

/* Function Name:
 *      rtk_qos_portScheduler_get
 * Description:
 *       Get port schedule
 * Input:
 *      portId                                 -port id
 * Output:
 *      pPortSchedule                         - schedule type
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_FAILED 
 *     RT_ERR_OK                           
 * Note:
 *      This API can get port schedule
 */
extern rtk_api_ret_t rtk_qos_portScheduler_get(rtk_port_t portId, rtk_queue_scheduleType_t* pPortSchedule);

/* Function Name:
 *      rtk_qos_queueScheduler_set
 * Description:
 *       This API is used to configure queue scheduling algorithm
 * Input:
 *      portId                                 - port id, from 0 to 7
 *      queueId                              -queue id, from 0 to 3
 *      queueSchedule                 - schedule algorithm, from QOS_WFQ to QOS_STRICT_PRIORITY
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_QOS_SCHEDULE 
 *     RT_ERR_OK                           
 * Note:
 *      For a port, WFQ/WRR/CBS can not exist at the same time.
 *      Queue 0 can not be set to CBS.
 */
extern rtk_api_ret_t rtk_qos_queueScheduler_set(rtk_port_t portId, rtk_queue_num_t queueId, rtk_queue_scheduleType_t queueSchedule);

/* Function Name:
 *      rtk_qos_queueScheduler_set
 * Description:
 *       This API is used to get queue scheduling algorithm
 * Input:
 *      portId                                 - port id, from 0 to 7
 *      queueId                              -queue id, from 0 to 3
 * Output:
 *      pQueueSchedule              -the pointer to  schedule algorithm
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_RT_ERR_FAILED
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_queueScheduler_get(rtk_port_t portId, rtk_queue_num_t queueId, rtk_queue_scheduleType_t* pQueueSchedule);

/* Function Name:
 *      rtk_qos_queueWFQWRRWeight_set
 * Description:
 *       This API is used to configure queue weight
 * Input:
 *      portId                                 - port id, from 0 to 7
 *      queueId                              -queue id, from 0 to 3
 *      queueWeight                    - queue weight of a queue, from 0 to 63
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_QOS_QUEUE_WEIGHT 
 *     RT_ERR_OK                           
 * Note:
 *      This API is valid only if the queue algorithm is WFQ or WRR
 */
extern rtk_api_ret_t rtk_qos_queueWFQWRRWeight_set(rtk_port_t portId, rtk_queue_num_t queueId, rtk_weight_t queueWeight);

/* Function Name:
 *      rtk_qos_queueWFQWRRWeight_get
 * Description:
 *       This API is used to get queue weight
 * Input:
 *      portId                                 - port id, from 0 to 7
 *      queueId                              -queue id, from 0 to 3
 * Output:
 *      pQueueWeight                 -the pointer to  queue weight
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_qos_queueWFQWRRWeight_get(rtk_port_t portId, rtk_queue_num_t queueId,  rtk_weight_t* pQueueWeight);

/* Function Name:
 *      rtk_rate_ingressBWPreIFG_set
 * Description:
 *       Set ingress bandwidth control include Preamble & IFG or not
 * Input:
 *      preIfg                                 -include Preamble & IFG or not
 * Output:
 *      none
 * Return:
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      This API can set ingress bandwidth control include Preamble & IFG or not
 */
extern rtk_api_ret_t rtk_rate_ingressBWPreIFG_set(rtk_pktLen_with_preIfg_t preIfg);

/* Function Name:
 *      rtk_rate_ingressBWPreIFG_get
 * Description:
 *       Get ingress bandwidth control include Preamble & IFG or not
 * Input:
 *      none
 * Output:
 *      pPreIfg                                 -include Preamble & IFG or not
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      This API can get ingress bandwidth control include Preamble & IFG or not
 */
extern rtk_api_ret_t rtk_rate_ingressBWPreIFG_get(rtk_pktLen_with_preIfg_t* pPreIfg);

/* Function Name:
 *      rtk_rate_ingressBWCtrl_set
 * Description:
 *      This API is used to configure port ingress bandwidth and flow control ability.
 * Input:
 *      portId                                        -port id, from 0 to 7.
 *      pIngressBwCtrl                       -the pointer to bandwidth control configuration
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_INPUT 
 *     RT_ERR_QOS_RATE 
 *     RT_ERR_OK                           
 * Note:
 *     pIngressBwCtrl->bwCtrlEnable stands for ingress bandwidth control ability, from DISABLED to ENABLED.
 *     pIngressBwCtrl->ingressRate0 stands for the rate of LB0, in unit of 8Kbps, from 0 to 0x1ffff.
 *     pIngressBwCtrl->ingressRate1 stands for the rate of LB0, in unit of 8Kbps, from 0 to 0x1ffff.
 *     pIngressBwCtrl->flowCtrlEnable0 stands for the flow control ability of LB0.
 *     pIngressBwCtrl->flowCtrlEnable1 stands for the flow control ability of LB1.
 */
extern rtk_api_ret_t rtk_rate_ingressBWCtrl_set(rtk_port_t portId, rtk_ingress_bwCtrl_t* pIngressBwCtrl);

/* Function Name:
 *      rtk_rate_ingressBWCtrl_get
 * Description:
 *      This API is used to get  port ingress bandwidth and flow control ability.
 * Input:
 *      portId                                        -port id, from 0 to 7.
 * Output:
 *      pIngressBwCtrl                       -the pointer to bandwidth control configuration
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_rate_ingressBWCtrl_get(rtk_port_t portId, rtk_ingress_bwCtrl_t* pIngressBwCtrl);

/* Function Name:
 *      rtk_rate_egressBWPreIFG_set
 * Description:
 *       Set egress bandwidth control include Preamble & IFG or not
 * Input:
 *      preIfg                                 -include Preamble & IFG or not
 * Output:
 *      none
 * Return:
 *     RT_ERR_INPUT 
 *     RT_ERR_OK                           
 * Note:
 *      This API can set egress bandwidth control include Preamble & IFG or not
 */
extern rtk_api_ret_t rtk_rate_egressBWPreIFG_set(rtk_pktLen_with_preIfg_t preIfg);

/* Function Name:
 *      rtk_rate_egressBWPreIFG_get
 * Description:
 *       Get egress bandwidth control include Preamble & IFG or not
 * Input:
 *      none
 * Output:
 *      pPreIfg                                 -include Preamble & IFG or not
 * Return:
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      This API can get egress bandwidth control include Preamble & IFG or not
 */
extern rtk_api_ret_t rtk_rate_egressBWPreIFG_get(rtk_pktLen_with_preIfg_t* pPreIfg);

/* Function Name:
 *      rtk_rate_egressPortBWCtrl_set
 * Description:
 *       This API is used to configure port egress bandwidth control
 * Input:
 *      portId                                       -port id, from 0 to 7
 *      enable                                      -bandwidth control ability, from DISABLED to ENABLED
 *      portRate                                   -rate of egress port, in unit of 8Kbps, from 0 to 0x1ffff
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_QOS_RATE 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_rate_egressPortBWCtrl_set(rtk_port_t portId, rtk_enable_t enable, rtk_rate_t portRate);

/* Function Name:
 *      rtk_rate_egressPortBWCtrl_get
 * Description:
 *       This API is used to get  port egress bandwidth control
 * Input:
 *      portId                                       -port id, from 0 to 7
 * Output:
 *      pEnable                                    -the pointer to bandwidth control ability
 *      pPortRate                                 -the pointer toegress port rate in step of 8Kbps
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_rate_egressPortBWCtrl_get(rtk_port_t portId, rtk_enable_t* pEnable, rtk_rate_t* pPortRate);

/* Function Name:
 *      rtk_rate_egressQueueBWCtlrl_set
 * Description:
 *       This API is used to configure egress queue bandwidth control
 * Input:
 *      portId                                 -port id, from 0 to 7            
 *      queueId                              -queue id, from 0 to 3
 *      enable                                -egress queue bandwidth control ability, from DISABLED to ENABLEDy
 *      queueRate                         -rate of egress queue, in unit of 8Kbps, from 0 to 0x1ffff
 * Output:
 *      none
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_INPUT 
 *     RT_ERR_QOS_RATE 
 *     RT_ERR_OK                           
 * Note:
 *      This API is valid only if the queue algorithm is WRR or Strict Priotiy
 */
extern rtk_api_ret_t rtk_rate_egressQueueBWCtlrl_set(rtk_port_t portId, rtk_queue_num_t queueId, rtk_enable_t enable, rtk_rate_t queueRate);

/* Function Name:
 *      rtk_rate_egressQueueBWCtlrl_get
 * Description:
 *       This API is used to get  queue egress bandwidth control
 * Input:
 *      portId                                 -port id, from 0 to 7            
 *      queueId                              -queue id, from 0 to 3
 * Output:
 *      pEnable                              -the pointer to bandwidth control ability
 *      pQueueRate                      -the pointer to egress queue rate in step of 8Kbps
 * Return:
 *     RT_ERR_PORT_ID 
 *     RT_ERR_QUEUE_ID 
 *     RT_ERR_NULL_POINTER 
 *     RT_ERR_OK                           
 * Note:
 *      none
 */
extern rtk_api_ret_t rtk_rate_egressQueueBWCtlrl_get(rtk_port_t portId, rtk_queue_num_t queueId, rtk_enable_t* pEnable, rtk_rate_t* pQueueRate);

/* Function Name:
 *      rtk_port_isolation_set
 * Description:
 *      Set permitted port isolation port mask
 * Input:
 *      port - port id.
 *      portmask - permit port mask
 * Output:
 *      None 
 * Return:
 *      RT_ERR_OK              - set shared meter successfully
 *      RT_ERR_FAILED          - FAILED to iset shared meter
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_ID - Invalid port number.
 *      RT_ERR_PORT_MASK - Invalid portmask.
 * Note:
 *      This API sets the ports that a port can forward packets to.
 *      A port can only forward packets to the ports included in permitted portmask
 */
extern rtk_api_ret_t rtk_port_isolation_set(rtk_port_t port, rtk_portmask_t portmask);

/* Function Name:
 *      rtk_port_isolation_get
 * Description:
 *      Get permitted port isolation portmask
 * Input:
 *      port - Port id.
 * Output:
 *      pPortmask - permit port mask
 * Return:
 *      RT_ERR_OK              - success
 *      RT_ERR_FAILED         - FAILED
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_ID - Invalid port number.
 * Note:
 *      This API gets the ports that a port can forward packets to.
 *      A port can only forward packets to the ports included in permitted portmask
 */
extern rtk_api_ret_t rtk_port_isolation_get(rtk_port_t port, rtk_portmask_t *pPortmask);


/* Function Name:
 *      rtk_vlan_init
 * Description:
 *      Initialize VLAN
 * Input:
 *      void
 * Output:
 *      void
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI 
 * Note:
 *     VLAN is disabled by default. User has to call this API to enable VLAN before
 *     using it. And It will set a default VLAN(vid 1) including all ports and set 
 *     all ports PVID to the default VLAN.
 */
extern rtk_api_ret_t rtk_vlan_init(void);

/* Function Name:
 *      rtk_vlan_set
 * Description:
 *      Set a VLAN entry
 * Input:
 *      vid           - VLAN ID to configure
 *      mbrmsk        - VLAN member set portmask
 *      untagmsk      - VLAN untag set portmask
 *      fid           -  filtering database id(0 ~ 14)
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_VLAN_VID
 *      RT_ERR_VLAN_PORT_MBR_EXIST
 *      RT_ERR_L2_FID
 *      RT_ERR_TBL_FULL
 * Note:
 *     There are 4K VLAN entry supported. User could configure the member set and untag set
 *     for specified vid through this API. The portmask's bit N means port N.
 *     For example, mbrmask 23=0x17=010111 means port 0,1,2,4 in the member set.
 *     For this chip, FID range is 0~14, it is used as spanning tree instance ID, if VLAN is set as 
 *     SVL mode, FID is also used as filtering database id. 
 *     Because SVLAN and CVLAN share the same vlan table, so SVLAN also use this API to set 
 *      vlan information.
 */
 extern rtk_api_ret_t rtk_vlan_set(rtk_vlan_t vid, rtk_portmask_t mbrmsk, rtk_portmask_t untagmsk, rtk_fid_t fid);


/* Function Name:
 *      rtk_vlan_get
 * Description:
 *      Get a VLAN entry
 * Input:
 *      vid             - VLAN ID to configure
 * Output:
 *      pMbrmsk         - VLAN member set portmask
 *      pUntagmsk       - VLAN untag set portmask
 *      pFid            -  filtering database id (0 ~ 14)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_VLAN_VID
 *      RT_ERR_NULL_POINTER 
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 *     There are 4K VLAN entry supported. User could configure the member set and untag set
 *     for specified vid through this API. The portmask's bit N means port N.
 *     For example, mbrmask 23=0x17=010111 means port 0,1,2,4 in the member set.
 *     For this chip, FID range is 0~14, it is used as spanning tree instance ID, if VLAN is set as 
 *     SVL mode, FID is also used as filtering database id. 
 *     Because SVLAN and CVLAN share the same vlan table, so SVLAN also use this API to set 
 *      vlan information.
 */
extern rtk_api_ret_t rtk_vlan_get(rtk_vlan_t vid, rtk_portmask_t *pMbrmsk, rtk_portmask_t *pUntagmsk, rtk_fid_t *pFid);

/* Function Name:
 *      rtk_vlan_destroy
 * Description:
 *      remove a VLAN entry
 * Input:
 *      vid             - VLAN ID to configure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_VLAN_VID
 *      RT_ERR_NULL_POINTER 
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 */
extern rtk_api_ret_t rtk_vlan_destroy(rtk_vlan_t vid);

/* Function Name:
 *      rtk_vlan_portPvid_set
 * Description:
 *      Set port to specified VLAN ID(PVID)
 * Input:
 *      port             - Port id
 *      pvid             - Specified VLAN ID
 *      priority         - 802.1p priority for the PVID
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_VLAN_VID
 *      RT_ERR_VLAN_PRIORITY 
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 *    The API is used for Port-based VLAN. The untagged frame received from the
 *    port will be classified to the specified VLAN and assigned to the specified priority.
 */
extern int32 rtk_vlan_portPvid_set(rtk_port_t port, rtk_vlan_t pvid, rtk_pri_t priority);

/* Function Name:
 *      rtk_vlan_portPvid_get
 * Description:
 *      Get VLAN ID(PVID) on specified port
 * Input:
 *      port             - Port id
 *      pPvid            - Specified VLAN ID
 *      pPriority        - 802.1p priority for the PVID
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER 
 * Note:
 *    The API is used for Port-based VLAN. The untagged frame received from the
 *    port will be classified to the specified VLAN and assigned to the specified priority.
 */
extern int32 rtk_vlan_portPvid_get(rtk_port_t port, rtk_vlan_t *pPvid, rtk_pri_t *pPriority);

/* Function Name:
 *      rtk_vlan_portAcceptFrameType_set
 * Description:
 *      Set VLAN support frame type
 * Input:
 *      port                                 - Port id
 *      accept_frame_type             - accept frame type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_PORT_ID
 * Note:
 *    The API is used for checking 802.1Q tagged frames.
 *    The accept frame type as following:
 *    ACCEPT_FRAME_TYPE_ALL
 *    ACCEPT_FRAME_TYPE_TAG_ONLY
 *    ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
extern rtk_api_ret_t rtk_vlan_portAcceptFrameType_set(rtk_port_t port, rtk_vlan_acceptFrameType_t accept_frame_type);

/* Function Name:
 *      rtk_vlan_portAcceptFrameType_get
 * Description:
 *      Get VLAN support frame type
 * Input:
 *      port                                 - Port id
 *      accept_frame_type             - accept frame type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_PORT_ID
 * Note:
 *    The API is used for checking 802.1Q tagged frames.
 *    The accept frame type as following:
 *    ACCEPT_FRAME_TYPE_ALL
 *    ACCEPT_FRAME_TYPE_TAG_ONLY
 *    ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
extern rtk_api_ret_t rtk_vlan_portAcceptFrameType_get(rtk_port_t port, rtk_vlan_acceptFrameType_t *pAccept_frame_type);

/* Function Name:
 *      rtk_vlan_portEgressTagStatus_set
 * Description:
 *      Set egress port VLAN tag status
 * Input:
 *      port      -  port id
 *      tag_status     - tx with or without CVLAN tag
 * Output:
*       none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      The API sets egree port transimiting pakcet with or without CVLAN tag.
 *      The tag status could be set as following:
 *      TAG_STATUS_WITH_TAG
 *      TAG_STATUS_WITHOUT_TAG
 */
extern rtk_api_ret_t rtk_vlan_portEgressTagStatus_set(rtk_port_t port, rtk_vlan_txTagStatus_t tag_status);

/* Function Name:
 *      rtk_vlan_portEgressTagStatus_set
 * Description:
 *      Get egress port VLAN tag status
 * Input:
 *      port       -  port id
 * Output:
 *      pTag_status -  the pointer of egress port tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      The API gets egree port transimiting pakcet with or without CVLAN tag.
 *      The tag status could be following values:
 *      TAG_STATUS_WITH_TAG
 *      TAG_STATUS_WITHOUT_TAG
 */
extern rtk_api_ret_t  rtk_vlan_portEgressTagStatus_get(rtk_port_t port, rtk_vlan_txTagStatus_t* pTag_status);

/* Function Name:
 *      rtk_vlan_portIgrFilterEnable_set
 * Description:
 *      Set VLAN ingress for each port
 * Input:
 *      port         - Port id
 *      igr_filter    - VLAN ingress function enable status
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_ENABLE
 * Note:
 *    The status of vlan ingress filter is as following:
 *      DISABLED
 *      ENABLED
 *   While VLAN function is enabled, ASIC will decide VLAN ID for each received frame and get belonged member
 *   ports from VLAN table. If received port is not belonged to VLAN member ports, ASIC will drop received frame if VLAN ingress function is enabled.
 */
extern rtk_api_ret_t rtk_vlan_portIgrFilterEnable_set(rtk_port_t port, rtk_enable_t igr_filter);

/* Function Name:
 *      rtk_vlan_portIgrFilterEnable_get
 * Description:
 *      Get VLAN Ingress Filter
 * Input:
 *      port            - Port id
 * Output:
 *      pIgr_filter    -  the pointer of VLAN ingress function enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_INPUT
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 *     The API can Get the VLAN ingress filter status.
 *     The status of vlan ingress filter is as following:
 *     DISABLED
 *     ENABLED   
 */
extern rtk_api_ret_t rtk_vlan_portIgrFilterEnable_get(rtk_port_t port, rtk_enable_t *pIgr_filter);

/* Function Name:
 *      rtk_vlan_tagAware_set
 * Description:
 *      Set ingress port aware tag VID  or not
 * Input:
 *      port       -  port id
 *      enabled  -  aware or unware
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      (1) If ingress port awares tag VID, for tagged packet, its VLAN is VID 
 *           in VLAN tag, for untagged packet, its VLAN is PVID
 *      (2) If ingress port unawares tag VID, for both tagged packet and untagged packet, 
 *           its VLAN is PVID.
 */
extern rtk_api_ret_t rtk_vlan_tagAware_set(rtk_port_t port, rtk_enable_t enabled);

/* Function Name:
 *      rtk_vlan_tagAware_get
 * Description:
 *      Get ingress port aware tag VID  or not
 * Input:
 *      port        -  port id
 * Output:
 *      pEnabled  -  the pointer of aware or unware
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      (1) If ingress port awares tag VID, for tagged packet, its VLAN is VID 
 *           in VLAN tag, for untagged packet, its VLAN is PVID
 *      (2) If ingress port unawares tag VID, for both tagged packet and untagged packet, 
 *           its VLAN is PVID.
 */
extern rtk_api_ret_t rtk_vlan_tagAware_get(rtk_port_t port, rtk_enable_t *pEnabled);

/* Function Name:
 *      rtk_leaky_vlan_set
 * Description:
 *      Set VLAN leaky
 * Input:
 *      type             - Packet type for VLAN leaky
 *      enable          - Leaky status
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_INPUT
 *      RT_ERR_ENABLE 
 * Note:
 *    This API can set VLAN leaky.
 *    The leaky frame types are as following:
 *    LEAKY_GRP
 *    LEAKY_STATIC_LUT
 *    LEAKY_RLDP.
 */
extern rtk_api_ret_t rtk_leaky_vlan_set(rtk_leaky_type_t type, rtk_enable_t enable);

/* Function Name:
 *      rtk_leaky_vlan_get
 * Description:
 *      Get VLAN leaky setting
 * Input:
 *      type                - Packet type for VLAN leaky
 * Output:
 *      rtk_enable_t     - Leaky status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI  
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER 
 * Note:
 *    This API can get VLAN leaky setting
 *    The leaky frame types are as following:
 *    LEAKY_GRP
 *    LEAKY_STATIC_LUT
 *    LEAKY_RLDP.
 */
extern  rtk_api_ret_t rtk_leaky_vlan_get(rtk_leaky_type_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_ivlsvlMode_set
 * Description:
 *      Set VLAN IVL or SVL mode
 * Input:
 *      ivlsvl           - IVL or SVL mode
 * Output:
*       none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      this API can set VLAN as IVL(Independent VLAN Learning) or SVL(shared VLAN learning) mode. 
 *      for IVL mode, if a given individual MAC Address is learned in one VLAN, that learned information 
 *      is not used in forwarding decisions taken for that address relative to any other VLAN.
 *       
 */
extern rtk_api_ret_t rtk_vlan_ivlsvlMode_set(rtk_vlan_ivlsvl_t ivlsvl);

/* Function Name:
 *      rtk_vlan_ivlsvlMode_get
 * Description:
 *      Get VLAN IVL or SVL mode
 * Input:
 *      pIvlsvl       -  the pointer of VLAN IVL or SVL mode
 * Output:
*       none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      this API can get VLAN as IVL(Independent VLAN Learning) or SVL(shared VLAN learning) mode. 
 *      for IVL mode, if a given individual MAC Address is learned in one VLAN, that learned information 
 *      is not used in forwarding decisions taken for that address relative to any other VLAN, its FID
 *      is VLAN ID.
 *       
 */
extern rtk_api_ret_t rtk_vlan_ivlsvlMode_get(rtk_vlan_ivlsvl_t *pIvlsvl);

/* Function Name:
 *      rtk_vlan_stg_set
 * Description:
 *      Set spanning tree group instance of the vlan to the specified device
 * Input:
 *      vid                -  specified VLAN ID
 *      stg                -  spanning tree group instance
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_SMI
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 *     The API can set spanning tree group instance of the vlan to the specified device.
 *     The spanning tree group number is from 0 to 15.  
 */
extern rtk_api_ret_t rtk_vlan_stg_set(rtk_vlan_t vid, rtk_stg_t stg);

/* Function Name:
 *      rtk_vlan_stg_get
 * Description:
 *      Set spanning tree group instance of the vlan to the specified device
 * Input:
 *      vid                -  specified VLAN ID
 * Output:
 *      pStg             -   spanning tree group instance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 *     The API can set spanning tree group instance of the vlan to the specified device.
 *     The spanning tree group number is from 0 to 15.  
 */
extern rtk_api_ret_t rtk_vlan_stg_get(rtk_vlan_t vid, rtk_stg_t *pStg);

/* Function Name:
 *      rtk_svlan_init
 * Description:
 *      Initialize SVLAN Configuration
 * Input:
 *      none
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 *      RT_ERR_INPUT
 * Note:
 *    Ether type of S-tag in 802.1ad is 0x88a8 and there are existed ether type 0x9100 and 0x9200 for Q-in-Q SLAN design. 
 *    User can set mathced ether type as service provider supported protocol. After call this API, 
 *    all ports are set as CVLAN port. you can use rtk_svlan_servicePort_add to add SVLAN port. 
 *    
 */
extern rtk_api_ret_t rtk_svlan_init(void);

/* Function Name:
 *      rtk_svlan_servicePort_add
 * Description:
 *      Enable one service port in the specified device
 * Input:
 *      port     -  Port id
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 * Note:
 *    This API is setting which port is connected to provider switch. All frames receiving from
 *     this port will recognize Service Vlan Tag. 
 */
extern rtk_api_ret_t rtk_svlan_servicePort_add(rtk_port_t port);

/* Function Name:
 *      rtk_svlan_servicePort_del
 * Description:
 *      Disable one service port in the specified device
 * Input:
 *      port     -  Port id
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 * Note:
 */
extern rtk_api_ret_t rtk_svlan_servicePort_del(rtk_port_t port);

/* Function Name:
 *      rtk_svlan_servicePort_get
 * Description:
 *      Get all the service ports in the specified device
 * Input:
 *      none
 * Output:
 *      pSvlan_portmask  - svlan ports mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern rtk_api_ret_t rtk_svlan_servicePort_get(rtk_portmask_t *pSvlan_portmask);

/* Function Name:
 *      rtk_svlan_tpidEntry_set
 * Description:
 *      Configure accepted S-VLAN ether type. The default ether type of S-VLAN is 0x88a8
 * Input:
 *      svlan_tag_id  - Ether type of S-tag frame parsing in uplink ports
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 * Note:
 *      Ether type of S-tag in 802.1ad is 0x88a8 and there are existed ether type 0x9100 
 *      and 0x9200 for Q-in-Q SLAN design. User can set mathced ether type as service 
 *      provider supported protocol. 
 */
extern rtk_api_ret_t rtk_svlan_tpidEntry_set(rtk_svlan_tpid_t svlan_tag_id);

/* Function Name:
 *      rtk_svlan_tpidEntry_get
 * Description:
 *      Get accepted S-VLAN ether type. The default ether type of S-VLAN is 0x88a8
 * Input:
 *      pSvlan_tag_id       - Ether type of S-tag frame parsing in uplink ports
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Ether type of S-tag in 802.1ad is 0x88a8 and there are existed ether type 0x9100 
 *      and 0x9200 for Q-in-Q SLAN design. User can set mathced ether type as service 
 *      provider supported protocol. 
 */
extern rtk_api_ret_t rtk_svlan_tpidEntry_get(rtk_svlan_tpid_t *pSvlan_tag_id);

/* Function Name:
 *      rtk_svlan_portPvid_set
 * Description:
 *      Set port to specified VLAN ID(PVID) for Service Provider Port
 * Input:
 *      port             - Port id
 *      pvid             - Specified Service VLAN ID
 *      priority         - 802.1p priority for the PVID
 *      dei               - Service VLAN tag DEI bit
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID
 *      RT_ERR_SMI 
 *      RT_ERR_VLAN_PRIORITY 
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND
 * Note:
 *    The API is used for Port-based VLAN. The untagged frame received from the
 *    port will be classified to the specified VLAN and assigned to the specified priority.
 */
extern rtk_api_ret_t rtk_svlan_portPvid_set(rtk_port_t port, rtk_vlan_t pvid, rtk_pri_t priority, rtk_dei_t dei);

/* Function Name:
 *      rtk_svlan_portPvid_get
 * Description:
 *      Get Service VLAN ID(PVID) on specified port
 * Input:
 *      port             - Port id
 *      pPvid            - Specified VLAN ID
 *      pPriority        - 802.1p priority for the PVID
 *      pDei             - DEI bit
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER 
 * Note:
 *    The API is used for Port-based VLAN. The untagged frame received from the
 *    port will be classified to the specified VLAN and assigned to the specified priority.
 */
extern rtk_api_ret_t rtk_svlan_portPvid_get(rtk_port_t port, rtk_vlan_t *pPvid, rtk_pri_t* pPriority, rtk_dei_t *pDei);

/* Function Name:
 *      rtk_svlan_portAcceptFrameType_set
 * Description:
 *      Set Service VLAN support frame type
 * Input:
 *      port                                 - Port id
 *      accept_frame_type             - accept frame type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI 
 *      RT_ERR_PORT_ID
 * Note:
 *    The API is used for checking Service VLAN tagged frames.
 *    The accept frame type as following:
 *    ACCEPT_FRAME_TYPE_ALL
 *    ACCEPT_FRAME_TYPE_TAG_ONLY
 *    ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
extern rtk_api_ret_t rtk_svlan_portAcceptFrameType_set(rtk_port_t port, rtk_vlan_acceptFrameType_t accept_frame_type);

/* Function Name:
 *      rtk_svlan_portAcceptFrameType_get
 * Description:
 *      Get Service VLAN support frame type
 * Input:
 *      port                                 - Port id
 *      accept_frame_type             - accept frame type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI 
 *      RT_ERR_PORT_ID
 * Note:
 *    The API is used for checking svlan tagged frames.
 *    The accept frame type as following:
 *    ACCEPT_FRAME_TYPE_ALL
 *    ACCEPT_FRAME_TYPE_TAG_ONLY
 *    ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
extern rtk_api_ret_t rtk_svlan_portAcceptFrameType_get(rtk_port_t port, rtk_vlan_acceptFrameType_t *pAccept_frame_type);

/* Function Name:
 *      rtk_vlan_stagMode_set
 * Description:
 *      Set SVLAN egress tag mode
 * Input:
 *      port                - Port id
 *      tag_mode        - Egress tag mode
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI 
 *      RT_ERR_INPUT
 * Note:
 *    The API is used for setting port vlan egress tag mode
 *    The tag mode as following:
 *    VLAN_TAG_MODE_ORIGINAL
 *    VLAN_TAG_MODE_KEEP_FORMAT
 *    VLAN_TAG_MODE_REAL_KEEP_FORMAT
 */
extern rtk_api_ret_t rtk_svlan_tagMode_set(rtk_port_t port, rtk_vlan_tagMode_t tag_mode);

/* Function Name:
 *      rtk_svlan_tagMode_get
 * Description:
 *      Get SVLAN egress tag mode
 * Input:
 *      port                - Port id
 *      pTag_mode      - Egress tag mode
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI 
 *      RT_ERR_INPUT
 * Note:
 *    The API is used for setting port vlan egress tag mode
 *    The tag mode as following:
 *    VLAN_TAG_MODE_ORIGINAL
 *    VLAN_TAG_MODE_KEEP_FORMAT
 *    VLAN_TAG_MODE_REAL_KEEP_FORMAT
 */
extern rtk_api_ret_t rtk_svlan_tagMode_get(rtk_port_t port, rtk_vlan_tagMode_t *pTag_mode);

/* Function Name:
 *      rtk_svlan_portEgressTagStatus_set
 * Description:
 *      Set egress port SVLAN tag status
 * Input:
 *      port      -  port id
 *      tag_status     - tx with or without SVLAN tag
 * Output:
*       none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      The API sets egree port transimiting pakcet with or without SVLAN tag.
 *      The tag status could be set as following:
 *      TAG_STATUS_WITH_TAG
 *      TAG_STATUS_WITHOUT_TAG
 */
extern rtk_api_ret_t rtk_svlan_portEgressTagStatus_set(rtk_port_t port, rtk_vlan_txTagStatus_t tag_status);

/* Function Name:
 *      rtk_svlan_portEgressTagStatus_get
 * Description:
 *      Get egress port SVLAN tag status
 * Input:
 *      port       -  port id
 * Output:
 *      pTag_status -  the pointer of egress port tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      The API gets egree port transimiting pakcet with or without SVLAN tag.
 *      The tag status could be following values:
 *      TAG_STATUS_WITH_TAG
 *      TAG_STATUS_WITHOUT_TAG
 */
extern rtk_api_ret_t rtk_svlan_portEgressTagStatus_get(rtk_port_t port, rtk_vlan_txTagStatus_t *pTag_status);

/* Function Name:
 *      rtk_svlan_tagAware_set
 * Description:
 *      Set ingress port aware Stag VID  or not
 * Input:
 *      port       -  port id
 *      enabled  -  aware or unware
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      (1) If ingress port awares tag VID, for tagged packet, its SVLAN is VID 
 *           in SVLAN tag, for untagged packet, its SVLAN is PVID
 *      (2) If ingress port unawares tag VID, for both tagged packet and untagged packet, 
 *           its SVLAN is PVID.
 */
extern rtk_api_ret_t rtk_svlan_tagAware_set(rtk_port_t port, rtk_enable_t enabled);

/* Function Name:
 *      rtk_svlan_tagAware_get
 * Description:
 *      Get ingress port aware Stag VID  or not
 * Input:
 *      port        -  port id
 * Output:
 *      pEnabled  -  the pointer of aware or unware
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      (1) If ingress port awares tag VID, for tagged packet, its SVLAN is VID 
 *           in VLAN tag, for untagged packet, its SVLAN is PVID
 *      (2) If ingress port unawares tag VID, for both tagged packet and untagged packet, 
 *           its SVLAN is PVID.
 */
extern rtk_api_ret_t rtk_svlan_tagAware_get(rtk_port_t port, rtk_enable_t *pEnabled);

/* Function Name:
 *      rtk_svlan_fwdBaseOVid_set
 * Description:
 *      Configure whether forward basd on outer VID or inner VID.
 * Input:
 *      portId  - port Id
 *      enable  - ENABLED: forward based on outer VID, DISABLED: based on inner VID
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE - 
 *      RT_ERR_PORT_ID - port ID out of range
 *      RT_ERR_SMI
 * Note:
 *      This API is used to configure the forwarding VID for frames received from the specified port. 
 */
extern rtk_api_ret_t rtk_svlan_fwdBaseOVid_set(rtk_port_t portId, rtk_enable_t enable);

/* Function Name:
 *      rtk_svlan_fwdBaseOVid_get
 * Description:
 *      Retrieve whether forward basd on outer VID or inner VID.
 * Input:
 *      portId  - port Id
 * Output:
 *      pEnable  - ENABLED: forward based on outer VID, DISABLED: based on inner VID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - 
 *      RT_ERR_PORT_ID - port ID out of range
 *      RT_ERR_SMI
 * Note:
 *      This API is used to retrieve the forwarding VID for frames received from the specified port. 
 */
extern rtk_api_ret_t rtk_svlan_fwdBaseOVid_get(rtk_port_t portId, rtk_enable_t* pEnable);


/* Function Name:
 *      rtk_mirror_portBased_set
 * Description:
 *      Set port mirror function
 * Input:
 *      mirroring_port              - mirroring port
 *      pMirrored_rx_portmask   - Rx mirrored port mask
 *      pMirrored_tx_portmask   - Tx mirrored port mask
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 *      RT_ERR_PORT_ID
 *      RT_ERR_PORT_MASK
 * Note:
 *      The API is to set mirror function of  mirrorring port and Rx/Tx mirrorred ports
 *    
 */
extern rtk_api_ret_t rtk_mirror_portBased_set(rtk_port_t mirroring_port, rtk_portmask_t *pMirrored_rx_portmask, rtk_portmask_t *pMirrored_tx_portmask);

/* Function Name:
 *      rtk_mirror_portBased_get
 * Description:
 *      Get port mirror function
 * Input:
 *      none
 * Output:
 *      pMirroring_port               - mirroring port
 *      pMirrored_rx_portmask   - Rx mirrored port mask
 *      pMirrored_tx_portmask   - Tx mirrored port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI 
 *      RT_ERR_NULL_POINTER
 * Note:
 *      The API is to get mirror function of mirroring port and Rx/Tx mirrored ports
 *    
 */
extern rtk_api_ret_t rtk_mirror_portBased_get(rtk_port_t* pMirroring_port, rtk_portmask_t *pMirrored_rx_portmask, rtk_portmask_t *pMirrored_tx_portmask);

/* Function Name:
 *      rtk_mirror_portIso_set
 * Description:
 *      Set mirror port isolation
 * Input:
 *      enable   - Mirror isolation status
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 *      RT_ERR_ENABLE
 * Note:
 *    The API is to set mirror isolation function that prevent normal forwarding packets to mirorring port.
 *    
 */
extern rtk_api_ret_t rtk_mirror_portIso_set(rtk_enable_t enable);

/* Function Name:
 *      rtk_mirror_portIso_get
 * Description:
 *      Get mirror port isolation
 * Input:
 *      none
 * Output:
 *      pEnable   - Mirror isolation status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_SMI
 *      RT_ERR_ENABLE
 * Note:
 *    The API is to get mirror isolation status.
 *    
 */
extern rtk_api_ret_t rtk_mirror_portIso_get(rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_stat_global_get
 * Description:
 *      get global MIB counter.
 * Input:
 *      cntr_idx -  counter index
 * Output:
 *      pCntr -  counter returned
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid input parameter.
 * Note:
 *     Get global MIB counter.
 */
extern rtk_api_ret_t rtk_stat_global_get(rtk_stat_global_type_t cntr_idx, rtk_stat_counter_t *pCntr);


/* Function Name:
 *      rtk_stat_global_start
 * Description:
 *      Start global MIB counter.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 * Note:
 *     Start global MIB counter.
 */
extern rtk_api_ret_t rtk_stat_global_start(void);

/* Function Name:
 *      rtk_stat_global_stop
 * Description:
 *      Stop global MIB counter.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 * Note:
 *     Stop global MIB counter.
 */
extern rtk_api_ret_t rtk_stat_global_stop(void);

/* Function Name:
 *      rtk_stat_port_start
 * Description:
 *      Start port-based MIB counter.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_PORT_ID   - port ID out of range
 * Note:
 *     Start specified port MIB counter.
 */
extern rtk_api_ret_t rtk_stat_port_start(rtk_port_t port);

rtk_api_ret_t rtk_stat_port_reset(rtk_port_t port);

/* Function Name:
 *      rtk_stat_port_stop
 * Description:
 *      Stop port-based MIB counter.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_PORT_ID   - port ID out of range
 * Note:
 *     Stop specified port MIB counter.
 */
extern rtk_api_ret_t rtk_stat_port_stop(rtk_port_t port);

/* Function Name:
 *      rtk_stat_port_get
 * Description:
 *      Get port MIB counter.
 * Input:
 *      port -  port id.
 *      cntr_idx -  counter index.
 * Output:
 *      pCntr -  counter returned.
 * Return:
 *      RT_ERR_OK  - 
 *      RT_ERR_FAILED   - 
 *      RT_ERR_INPUT - Invalid input parameter.
 * Note:
 *     Get port MIB counter.
 */
extern rtk_api_ret_t rtk_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntr_idx, rtk_stat_counter_t *pCntr);

/* Function Name:
 *      rtk_mib_enable
 * Description:
 *      Enable MIB counter.
 * Input:
 *      None
 * Output:
 *      None.
 * Return:
 *      None
 * Note:
 *     
 */
extern void rtk_mib_enable(void);

/* Function Name:
 *      rtk_port_packet_received_get
 * Description:
 *      Get the number of received packets on port port_id.
 * Input:
 *      port -  port id.
 * Output:
 *      pCntr -  received packets.
 * Return:
 *      RT_ERR_OK  - 
 * Note:
 *     
 */
extern rtk_api_ret_t rtk_port_packet_received_get(int port_id, rtk_stat_counter_t *pCntr);


/* Function Name:
 *      rtk_port_packet_transmitted_get
 * Description:
 *      Get the number of transmitted packets on port port_id.
 * Input:
 *      port -  port id.
 * Output:
 *      pCntr -  transmitted packets.
 * Return:
 *      RT_ERR_OK  - 
 * Note:
 *     
 */
extern rtk_api_ret_t rtk_port_packet_transmitted_get(int port_id, rtk_stat_counter_t *pCntr);
 

/* Function Name:
 *      rtk_port_packet_error_get
 * Description:
 *      Get the number of packets with error on port port_id.
 * Input:
 *      port -  port id.
 * Output:
 *      pCntr -  error packets.
 * Return:
 *      RT_ERR_OK  - 
 * Note:
 *     
 */
extern rtk_api_ret_t rtk_port_packet_error_get(int port_id, rtk_stat_counter_t *pCntr);
 
/* Function Name:
 *      rtk_mac6_interface_set
 * Description:
 *      This function is used to set port interface type. 
 * Input:
 *      mode           -  Specify whether MAC6 is Fast Ethernet port or HEAC port
 * Output:
 *      None
 * Return:
 *      None                        
 * Note:
 *      
 */            
extern void rtk_mac6_interface_set(rtk_port_interface_t mode);

/* Function Name:
 *      rtk_mac6_interface_get
 * Description:
 *      This function is used to get the interface type of MAC 6. 
 * Input:
 *      mode           -  pointer to rtk_port_interface_t
 * Output:
 *      None
 * Return:
 *      None                        
 * Note:
 *      
 */            
extern void rtk_mac6_interface_get(rtk_port_interface_t *mode);

#endif /* __RTK_API_EXT_H__ */

