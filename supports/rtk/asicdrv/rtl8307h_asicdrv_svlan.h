#ifndef _RTL8307H_ASICDRV_SVLAN_H_
#define _RTL8307H_ASICDRV_SVLAN_H_

#include <rtk_types.h>
#include <rtk_error.h>

#include <rtl8307h_asicdrv_vlan.h>

/* Function Name:
 *      rtl83107h_svlan_portAcceptFrameType_set
 * Description:
 *      Set SVLAN support frame type
 * Input:
 *      port                           - Port id
 *      accept_frame_type      - accept frame type 
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 *      
 * Note:
 *     The API can set the port SVLAN accept frame type.
 *     The accept frame type as following:
 *         ACCEPT_FRAME_TYPE_ALL
 *         ACCEPT_FRAME_TYPE_TAG_ONLY
 *         ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *         ACCEPT_FRAME_TYPE_NONE
 */
extern int32 rtl8307h_svlan_portAcceptFrameType_set(uint32 port, rtk_vlan_acceptFrameType_t accept_frame_type);


/* Function Name:
 *      rtl8307h_svlan_portAcceptFrameType_get
 * Description:
 *      Get SVLAN support frame type
 * Input:
 *      port                           - Port id
 * Output:
 *      pAccept_frame_type     - accept frame type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_SMI
 *      
 * Note:
 *     The API can get the port SVLAN accept frame type.
 *     The accept frame type as following:
 *         ACCEPT_FRAME_TYPE_ALL
 *         ACCEPT_FRAME_TYPE_TAG_ONLY
 *         ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *         ACCEPT_FRAME_TYPE_NONE
 */
extern int32 rtl8307h_svlan_portAcceptFrameType_get(uint32 port, rtk_vlan_acceptFrameType_t *pAccept_frame_type);

extern int32 rtl8307h_svlan_portTagAware_set(uint32 port, uint32 enabled);
    
extern int32 rtl8307h_svlan_portTagAware_get(uint32 port, uint32 *pEnabled);

extern int32 rtl8307h_svlan_portPktParserAbility_set(uint32 port,  uint32 enabled);

extern int32 rtl8307h_svlan_portPktParserAbility_get(uint32 port,  uint32 *pEnabled);

/* Function Name:
 *      rtl8307h_svlan_portTagMode_set
 * Description:
 *      Set SVLAN egress tag mode
 * Input:
 *      port           - Port id
 *      tag_rxu     - the pkts whose souce port is UNI port tx tag status
 *      tag_rxn     - the pkts whose souce port is NNI port tx tag status
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_INPUT
 *      RT_ERR_SMI
 *      
 * Note:
 *    The API is used for setting SVLAN egress tag mode
 *    The accept frame type as following:
 *    VLAN_TAG_MODE_ORIGINAL
 *    VLAN_TAG_MODE_KEEP_FORMAT
 *    VLAN_TAG_MODE_REAL_KEEP_FORMAT
 */
extern int32 rtl8307h_svlan_portEgressTagMode_set(uint32 port, rtk_vlan_tagMode_t tag_rxu, rtk_vlan_tagMode_t tag_rxn);


/* Function Name:
 *      rtl8307h_svlan_portTagMode_get
 * Description:
 *      Get SVLAN egress tag mode
 * Input:
 *      port            - Port id
 * Output:
 *      pTag_rxu     - the pkts whose souce port is UNI port tx tag status
 *      pTag_rxn     - the pkts whose souce port is NNI port tx tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_SMI
 *      
 * Note:
 *    The API is used for setting SVLAN egress tag mode
 *    The accept frame type as following:
 *    VLAN_TAG_MODE_ORIGINAL
 *    VLAN_TAG_MODE_KEEP_FORMAT
 *    VLAN_TAG_MODE_REAL_KEEP_FORMAT
 */
extern int32 rtl8307h_svlan_portEgressTagMode_get(uint32 port, rtk_vlan_tagMode_t *pTag_rxu, rtk_vlan_tagMode_t *pTag_rxn);


/* Function Name:
 *      rtl8307h_svlan_portEgressTagStatus_set
 * Description:
 *      Get egress port SVLAN tag status
 * Input:
 *      port      -  port id
 *      tag_rxu     - tx SVLAN tag status for the pkts received from UNI port
 *      tag_rxn     - tx SVLAN tag status for the pkts received from NNI port
 * Output:
*       none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      The API sets egree port transimiting pakcet with or without
 *      SVLAN tag. Its priority is lower than ACL, and equial to VLAN
 *      untag set. It means the setting will be ignored if tag status
 *      has been determined by ACL. One of VLAN untag set and 
 *      egress port tag status is untag, the packet will be untag.
 */
extern int32 rtl8307h_svlan_portEgressTagStatus_set(uint32 port, rtk_vlan_txTagStatus_t tag_rxu, rtk_vlan_txTagStatus_t tag_rxn);

 /* Function Name:
 *      rtl8307h_svlan_portEgressTagStatus_get
 * Description:
 *      Get egress port SVLAN tag status
 * Input:
 *      port      -  port id
 * Output:
 *      pTag_rxu     - the pointer of tx SVLAN tag status for the pkts received from UNI port
 *      pTag_rxn     - the pointer of tx SVLAN tag status for the pkts received from NNI port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      The API gets egree port transimiting pakcet with or without
 *      SVLAN tags. Its priority is lower than ACL, and equial to VLAN
 *      untag set. It means the setting will be ignored if tag status
 *      has been determined by ACL. One of VLAN untag set and 
 *      egress port tag status is untag, the packet will be untag.
 */
extern int32 rtl8307h_svlan_portEgressTagStatus_get(uint32 port,  rtk_vlan_txTagStatus_t  *pTag_rxu,  rtk_vlan_txTagStatus_t  *pTag_rxn);

extern int32 rtl8307h_svlan_portPvid_set(uint32 port, uint32 svid, uint32 pri, uint32 dei);

extern int32 rtl8307h_svlan_portPvid_get(uint32 port, uint32 *pSvid, uint32 *pPri, uint32 *pDei);

extern int32 rtl8307h_svlan_portFwdBaseOvid_set(uint32 port, uint32 enabled);

extern int32 rtl8307h_svlan_portFwdBaseOvid_get(uint32 port, uint32 *pEnabled);

extern int32 rtl8307h_svlan_tpid_set(uint32 tpid);

extern int32 rtl8307h_svlan_tpid_get(uint32* pTpid);

#endif /*#ifndef _RTL8307H_ASICDRV_SVLAN_H_*/

