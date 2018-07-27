#ifndef _RTL8307H_ASICDRV_VLAN_H_
#define _RTL8307H_ASICDRV_VLAN_H_

#include <rtk_types.h>
#include <rtk_error.h>

#include <rtk_api.h>

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/


/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/

typedef enum  rtl8307h_vlan_pktParserType_e
{
    RTL8307H_VLAN_PKT_PARSER_OUTERTAG,
    RTL8307H_VLAN_PKT_PARSER_INNERTAG, 
    RTL8307H_VLAN_PKT_PARSER_END
}rtl8307h_vlan_pktParserType_t;

typedef enum rtl8307h_portRole_e
{
   RTL8307H_PORTROLE_UNI = 0,
   RTL8307H_PORTROLE_NNI,
   RTL8307H_PORTROLE_END
} rtl8307h_vlan_portRole_t;

typedef struct rtl8307h_vlanTbl_param_s
{
    uint8 untag ;  /*untag set*/
    uint8 fid;     /*filtering database ID*/
    uint8 valid;
    uint8 vbfwd;  /*VLAN based forwarding. 0: based on inner VID, 1: based on outer VID*/
    uint8 ucslkfid;   /*unicast DA lookup use FID or VID, 0: VID, 1: FID*/
    uint8 member;   /*member port*/
    uint16 vid;
}rtl8307h_vlanTbl_param_t;

typedef struct rtl8307h_tblasic_vlan_entry_s
{
    uint32 val[1];   
}rtl8307h_tblasic_vlan_entry_t;

typedef struct rtl8307h_tblasic_vlanCam_entry_s
{
    uint32 val[2];   
}rtl8307h_tblasic_vlanCam_entry_t;


/****************************************************************/
/* Type Definition                                              */
/****************************************************************/


/****************************************************************/
/* Driver Proto Type Definition                                 */
/****************************************************************/

extern int32 rtl8307h_vlanTbl_entry_set(uint32 index, rtl8307h_vlanTbl_param_t* pParam);

extern int32 rtl8307h_vlanTbl_entry_get(uint32 index, rtl8307h_vlanTbl_param_t  *pParam);

extern int32 rtl8307h_vlanCam_entry_set(uint32 index, rtl8307h_vlanTbl_param_t *pParam);

extern int32 rtl8307h_vlanCam_entry_get(uint32 index, rtl8307h_vlanTbl_param_t *pParam);


/* Function Name:
 *      rtl8307h_vlan_portPvid_set
 * Description:
 *      Set VLAN port-based VID and priority
 * Input:
 *      port                          - Port id
 *      Vid                           -  Port-based vid
 *      Pri                           -   Port-base vlan priority
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_SMI
 *      
 * Note:
 *
 */
extern int32 rtl8307h_vlan_portPvid_set(uint32 port, uint32 vid, uint32 pri);


/* Function Name:
 *      rtl8307h_vlan_portPvid_get
 * Description:
 *      Get VLAN port-based VID and priority
 * Input:
 *      port                           - Port id
 * Output:
 *      pVid                          - the pointer of port-based vid
 *      pPri                           - the pointer of port-base vlan priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_SMI
 *      
 * Note:
 *
 */
extern int32 rtl8307h_vlan_portPvid_get(uint32 port, uint32 *pVid, uint32 *pPri);

/* Function Name:
 *      rtl8307h_vlan_portAcceptFrameType_set
 * Description:
 *      Set VLAN support frame type
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
 *     The API can set the port accept frame type.
 *     The accept frame type as following:
 *         FRAME_TYPE_BOTH
 *         FRAME_TYPE_TAGGED_ONLY
 *         FRAME_TYPE_UNTAGGED_ONLY
 *         FRAME_TYPE_NONE
 */
extern int32 rtl8307h_vlan_portAcceptFrameType_set(uint32 port, rtk_vlan_acceptFrameType_t accept_frame_type);

/* Function Name:
 *      rtl8307h_vlan_portAcceptFrameType_get
 * Description:
 *      Get VLAN support frame type
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
 *     The API can get the port accept frame type.
 *     The accept frame type as following:
 *         FRAME_TYPE_BOTH
 *         FRAME_TYPE_TAGGED_ONLY
 *         FRAME_TYPE_UNTAGGED_ONLY
 *         FRAME_TYPE_NONE
 */
extern int32 rtl8307h_vlan_portAcceptFrameType_get(uint32 port, rtk_vlan_acceptFrameType_t *pAccept_frame_type);

/* Function Name:
 *      rtl8307h_vlan_portEgressTagMode_set
 * Description:
 *      Set CVLAN egress tag mode
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
 *    The API is used for setting CVLAN egress tag mode
 *    The accept frame type as following:
 *    VLAN_TAG_MODE_ORIGINAL
 *    VLAN_TAG_MODE_KEEP_FORMAT
 *    VLAN_TAG_MODE_REAL_KEEP_FORMAT
 */
extern int32 rtl8307h_vlan_portEgressTagMode_set(uint32 port, rtk_vlan_tagMode_t tag_rxu, rtk_vlan_tagMode_t tag_rxn );

/* Function Name:
 *      rtl8307h_vlan_portEgressTagMode_get
 * Description:
 *      Get CVLAN egress tag mode
 * Input:
 *      pTag_rxu     - the pkts whose souce port is UNI port tx tag status
 *      pTag_rxn     - the pkts whose souce port is NNI port tx tag status
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_SMI
 *      
 * Note:
 *    The API is used for setting CVLAN egress tag mode
 *    The accept frame type as following:
 *    VLAN_TAG_MODE_ORIGINAL
 *    VLAN_TAG_MODE_KEEP_FORMAT
 *    VLAN_TAG_MODE_REAL_KEEP_FORMAT
 */
extern int32 rtl8307h_vlan_portEgressTagMode_get(uint32 port, rtk_vlan_tagMode_t *pTag_rxu, rtk_vlan_tagMode_t *pTag_rxn);

extern int32 rtl8307h_vlan_portTagAware_set(uint32 port, uint32 enabled);

extern int32 rtl8307h_vlan_portTagAware_get(uint32 port, uint32 *pEnabled);

extern int32 rtl8307h_vlan_portParserTagAbility_set(uint32 port, uint32 enabled);
extern int32 rtl8307h_vlan_portParserTagAbility_get(uint32 port,  uint32 *pEnabled);

extern int32 rtl8307h_vlan_portIgrFilterEnable_set(uint32 port, uint32 enabled);

extern int32 rtl8307h_vlan_portIgrFilterEnable_get(uint32 port, uint32 *pEnabled);

extern int32 rtl8307h_vlan_portRole_set(uint32 port, rtl8307h_vlan_portRole_t portrole);

extern int32 rtl8307h_vlan_portRole_get(uint32 port, rtl8307h_vlan_portRole_t *pPortrole);

/* Function Name:
 *      rtl8307h_vlan_portEgressTagStatus_set
 * Description:
 *      Set egress port CVLAN tag status
 * Input:
 *      port      -  port id
 *      tag_rxu     - tx CVLAN tag status for the pkts received from UNI port
 *      tag_rxn     - tx CVLAN tag status for the pkts received from NNI port
 * Output:
*       none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      The API set egree port transimiting pakcet with or without
 *      CVLAN tag. Its priority is lower than ACL, and equial to VLAN
 *      untag set. It means the setting will be ignored if tag status
 *      has been determined by ACL. One of VLAN untag set and 
 *      egress port tag status is untag, the packet will be untag.
 */
extern int32 rtl8307h_vlan_portEgressTagStatus_set(uint32 port, rtk_vlan_txTagStatus_t tag_rxu, rtk_vlan_txTagStatus_t tag_rxn);

/* Function Name:
 *      rtl8307h_vlan_portEgressTagStatus_get
 * Description:
 *      Get egress port CVLAN tag status
 * Input:
 *      port      -  port id
 * Output:
 *      pTag_rxu     - the pointer of tx CVLAN tag status for the pkts received from UNI port
 *      pTag_rxn     - the pointer of tx CVLAN tag status for the pkts received from NNI port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENABLE
 * Note:
 *      The API gets egree port transimiting pakcet with or without
 *      CVLAN tags. Its priority is lower than ACL, and equial to VLAN
 *      untag set. It means the setting will be ignored if tag status
 *      has been determined by ACL. One of VLAN untag set and 
 *      egress port tag status is untag, the packet will be untag.
 */
extern int32 rtl8307h_vlan_portEgressTagStatus_get(uint32 port,  rtk_vlan_txTagStatus_t  *pTag_rxu,  rtk_vlan_txTagStatus_t  *pTag_rxn);

extern int32 rtl8307h_vlan_tpid_set(uint32 tpid);

extern int32 rtl8316d_vlan_tpid_get(uint32* pTpid);

extern int32 rtl8307h_vlan_flush_set(void);

#endif /*#ifndef _RTL8307H_ASICDRV_VLAN_H_*/

