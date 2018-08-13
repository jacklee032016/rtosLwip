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
 * Purpose : RTL8307H switch low-level API for CVLAN module
 * Feature : 
 *
 */
//frank stdlib string (memset)
#include <stdlib.h>
#include <string.h>

#include <rtk_types.h>
#include <rtk_error.h>
#include <rtk_api.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>
#include <rtl8307h_table_struct.h>
#include <rtl8307h_debug.h>

#include <rtl8307h_asicdrv_vlan.h>

static int32  _rtl8307h_vlanTbl_param2entry(rtl8307h_tblasic_vlan_entry_t *pEntry,  rtl8307h_vlanTbl_param_t* pVlanParam)
{
    memset(pEntry, 0, sizeof(rtl8307h_tblasic_vlan_entry_t));
    
    pVlanParam->untag &= 0xFFUL;
    pVlanParam->fid &= 0xF;
    pVlanParam->valid &= 0x1;
    pVlanParam->vbfwd &= 0x1;
    pVlanParam->ucslkfid &= 0x1;
    pVlanParam->member &= 0xFFUL;
    pVlanParam->vid &= 0xFFF;
 
    pEntry->val[0] = ((uint32)pVlanParam->member << 22)       |           \
                    ((uint32)pVlanParam->untag << 14)      |           \
                    ((uint32)pVlanParam->fid << 10)      |           \
                    ((uint32)pVlanParam->ucslkfid << 9)      |           \
                    ((uint32)pVlanParam->vbfwd << 8)      |           \
                    ((((uint32)pVlanParam->vid >> 5) & 0x7F) << 1)     |           \
                    ((uint32)pVlanParam->valid << 0);
    
    return SUCCESS;
}

static int32 _rtl8307h_vlanTbl_entry2Param(const rtl8307h_tblasic_vlan_entry_t *pEntry, rtl8307h_vlanTbl_param_t *pVlanParam, uint16 index)
{
    memset(pVlanParam, 0, sizeof(rtl8307h_vlanTbl_param_t)); 

    pVlanParam->member = (uint8)((pEntry->val[0] >> 22) & 0xFFUL);
    pVlanParam->untag = (uint8)((pEntry->val[0] >> 14) & 0xFF);
    pVlanParam->fid = (uint8)((pEntry->val[0] >> 10) & 0xF);
    pVlanParam->ucslkfid = (uint8)((pEntry->val[0] >> 9) & 0x1);
    pVlanParam->vbfwd = (uint8)((pEntry->val[0] >> 8) & 0x1); 
    pVlanParam->vid = ((uint16)((pEntry->val[0] >> 1) & 0x7F) << 5) | (index >> 2); 
    pVlanParam->valid = (uint8)(pEntry->val[0] & 0x1);

    return SUCCESS;
}

static int32 _rtl8307h_vlanCam_param2entry(rtl8307h_tblasic_vlanCam_entry_t *pEntry,  rtl8307h_vlanTbl_param_t *pVlanParam)
{
    memset(pEntry, 0, sizeof(rtl8307h_tblasic_vlanCam_entry_t));
    
    pVlanParam->untag &= 0xFFUL;
    pVlanParam->fid &= 0xF;
    pVlanParam->valid &= 0x1;
    pVlanParam->vbfwd &= 0x1;
    pVlanParam->ucslkfid &= 0x1;
    pVlanParam->member &= 0xFFUL;
    pVlanParam->vid &= 0xFFF;
    
    pEntry->val[0] = ((uint32)pVlanParam->untag << 19)       |           \
                    ((uint32)pVlanParam->fid << 15)      |           \
                    ((uint32)pVlanParam->ucslkfid << 14)      |           \
                    ((uint32)pVlanParam->vbfwd << 13)      |           \
                    (((uint32)pVlanParam->vid) << 1)      |           \
                    ((uint32)pVlanParam->valid << 0);
    
    pEntry->val[1] =  (uint32)pVlanParam->member;

    return SUCCESS;
}

static int32 _rtl8307h_vlanCam_entry2Param(const rtl8307h_tblasic_vlanCam_entry_t *pEntry, rtl8307h_vlanTbl_param_t *pVlanParam)
{
    memset(pVlanParam, 0, sizeof(rtl8307h_vlanTbl_param_t));
    
    pVlanParam->untag = (uint8)((pEntry->val[0] >> 19) & 0xFF);
    pVlanParam->fid = (uint8)((pEntry->val[0] >> 15) & 0xF);
    pVlanParam->ucslkfid = (uint8)((pEntry->val[0] >> 14) & 0x1);
    pVlanParam->vbfwd = (uint8)((pEntry->val[0] >> 13) & 0x1); 
    pVlanParam->vid = (uint16)((pEntry->val[0] >> 1) & 0xFFF)  ; 
    pVlanParam->valid = (uint8)(pEntry->val[0] & 0x1);
    pVlanParam->member = (uint8)(pEntry->val[1] & 0xFFUL);

    return SUCCESS;
}


int32 rtl8307h_vlanTbl_entry_set(uint32 index, rtl8307h_vlanTbl_param_t* pParam)
{
    uint32 retvalue;
    rtl8307h_tblasic_vlan_entry_t entry;

    if (pParam == NULL)
        return FAILED;

     _rtl8307h_vlanTbl_param2entry(&entry, pParam);
    retvalue = table_write(RTL8307H_UNIT, VLAN, index, (uint32 *)entry.val);
    
    return retvalue;   
}
  

int32 rtl8307h_vlanTbl_entry_get(uint32 index, rtl8307h_vlanTbl_param_t *pParam)
{
    rtl8307h_tblasic_vlan_entry_t entry; 

    if (pParam == NULL)
        return FAILED;
    
     CHK_FUN_RETVAL(table_read(RTL8307H_UNIT, VLAN, index, (uint32 *)entry.val));

    _rtl8307h_vlanTbl_entry2Param(&entry, pParam, (uint16)index);

    return SUCCESS;
}


int32 rtl8307h_vlanCam_entry_set(uint32 index, rtl8307h_vlanTbl_param_t *pParam)
{
    uint32 retvalue;
    rtl8307h_tblasic_vlanCam_entry_t entry;

    if (pParam == NULL)
        return FAILED;
     
    _rtl8307h_vlanCam_param2entry(&entry, pParam);
    
    retvalue = table_write(RTL8307H_UNIT, VLAN_CAM, index, (uint32 *)entry.val);
    
    return retvalue;
}

int32 rtl8307h_vlanCam_entry_get(uint32 index, rtl8307h_vlanTbl_param_t *pParam)
{
    rtl8307h_tblasic_vlanCam_entry_t entry;  

    if (pParam == NULL)
        return FAILED;
    
    CHK_FUN_RETVAL(table_read(RTL8307H_UNIT, VLAN_CAM, index, entry.val));
    _rtl8307h_vlanCam_entry2Param(&entry, pParam);

    return SUCCESS;
}


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
int32 rtl8307h_vlan_portPvid_set(uint32 port, uint32 vid, uint32 pri)
{
    uint32 regVal;

    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    if (vid >  RTL8307H_VIDMAX)
        return RT_ERR_VLAN_VID;

    if(pri > RTL8307H_PRIMAX) 
        return RT_ERR_VLAN_PRIORITY;

    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, &regVal));
    reg_field_set(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, PIPRI, pri, &regVal);
    reg_field_set(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, PIVID, vid, &regVal);
    CHK_FUN_RETVAL(reg_write(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, regVal));
   
    return RT_ERR_OK;
}


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
int32 rtl8307h_vlan_portPvid_get(uint32 port, uint32 *pVid, uint32 *pPri)
{
    uint32 regVal;

    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    if(pVid == NULL)
        return RT_ERR_INPUT;

    if(pPri == NULL) 
        return RT_ERR_INPUT;

    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, &regVal));
    reg_field_get(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, PIVID, pVid, regVal);
    reg_field_get(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, PIPRI, pPri, regVal);    

    return RT_ERR_OK;
}

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
int32 rtl8307h_vlan_portAcceptFrameType_set(uint32 port, rtk_vlan_acceptFrameType_t accept_frame_type)
{
    uint32 regVal;

    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, &regVal));    
    switch(accept_frame_type)
    {
        case ACCEPT_FRAME_TYPE_ALL:
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTITAG, 1, &regVal);
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTIUTAG, 1, &regVal);                       
            break;
        case ACCEPT_FRAME_TYPE_TAG_ONLY:
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTITAG, 1, &regVal);
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTIUTAG, 0, &regVal);                                   
            break;
        case ACCEPT_FRAME_TYPE_UNTAG_ONLY:
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTITAG, 0, &regVal);
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTIUTAG, 1, &regVal);                                               
            break;
        case ACCEPT_FRAME_TYPE_NONE:
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTITAG, 0, &regVal);
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTIUTAG, 0, &regVal);                                               
            break;
        default:
            return RT_ERR_INPUT;     
    }
    CHK_FUN_RETVAL(reg_write(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, regVal));        

    return RT_ERR_OK;
}

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
int32 rtl8307h_vlan_portAcceptFrameType_get(uint32 port, rtk_vlan_acceptFrameType_t *pAccept_frame_type)
{
    uint32 regVal;
    uint32 acctag;
    uint32 accutag;
    
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, &regVal)); 
    reg_field_get(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTITAG, &acctag, regVal);
    reg_field_get(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTIUTAG, &accutag, regVal);    
    *pAccept_frame_type = acctag ? 0: 0x2;
    *pAccept_frame_type +=  accutag ? 0: 0x1;

    return RT_ERR_OK;
} 

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
int32 rtl8307h_vlan_portEgressTagMode_set(uint32 port, rtk_vlan_tagMode_t tag_rxu, rtk_vlan_tagMode_t tag_rxn )
{
    uint32 fieldVal1, fieldVal2;
    
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    switch (tag_rxu)
    {
        case VLAN_TAG_MODE_ORIGINAL:
            fieldVal1 = 0;
            break;
            
        case VLAN_TAG_MODE_KEEP_FORMAT:
            fieldVal1 = 1;
            break;
            
        case VLAN_TAG_MODE_REAL_KEEP_FORMAT:
            fieldVal1 = 2;
            break;
            
        default:
            return RT_ERR_INPUT;
    }
    
    switch (tag_rxn)
    {
        case VLAN_TAG_MODE_ORIGINAL:
            fieldVal2 = 0;
            break;
            
        case VLAN_TAG_MODE_KEEP_FORMAT:
            fieldVal2 = 1;
            break;
            
        case VLAN_TAG_MODE_REAL_KEEP_FORMAT:
            fieldVal2 = 2;
            break;
            
        default:
            return RT_ERR_INPUT;
    }
    
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, PORT0_TX_TAG_CONTROL0 + port, RXUTX_ITAG_KEEP, fieldVal1));     
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, PORT0_TX_TAG_CONTROL0 + port, RXNTX_ITAG_KEEP, fieldVal2));

    return RT_ERR_OK;
}

/* Function Name:
 *      rtl8307h_vlan_portEgressTagMode_get
 * Description:
 *      Get CVLAN egress tag mode
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
 *    The API is used for setting CVLAN egress tag mode
 *    The accept frame type as following:
 *    VLAN_TAG_MODE_ORIGINAL
 *    VLAN_TAG_MODE_KEEP_FORMAT
 *    VLAN_TAG_MODE_REAL_KEEP_FORMAT
 */
int32 rtl8307h_vlan_portEgressTagMode_get(uint32 port, rtk_vlan_tagMode_t *pTag_rxu, rtk_vlan_tagMode_t *pTag_rxn)
{
    uint32 fieldVal;
    
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    if ((NULL == pTag_rxu ) || ( NULL == pTag_rxn))
        return RT_ERR_NULL_POINTER;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_TX_TAG_CONTROL0 + port, RXUTX_ITAG_KEEP, &fieldVal));
    switch (fieldVal)
    {
        case 0:
            *pTag_rxu = VLAN_TAG_MODE_ORIGINAL;
            break;
            
        case 1:
            *pTag_rxu = VLAN_TAG_MODE_KEEP_FORMAT;
            break;
            
        case 2:
            *pTag_rxu = VLAN_TAG_MODE_REAL_KEEP_FORMAT;
            break;
            
        default:
            return RT_ERR_FAILED;
    }

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_TX_TAG_CONTROL0 + port, RXNTX_ITAG_KEEP, &fieldVal));   
    switch (fieldVal)
    {
        case 0:
            *pTag_rxn = VLAN_TAG_MODE_ORIGINAL;
            break;
            
        case 1:
            *pTag_rxn = VLAN_TAG_MODE_KEEP_FORMAT;
            break;
            
        case 2:
            *pTag_rxn = VLAN_TAG_MODE_REAL_KEEP_FORMAT;
            break;
            
        default:
            return RT_ERR_FAILED;
    }      

    return RT_ERR_OK;
}

int32 rtl8307h_vlan_portTagAware_set(uint32 port, uint32 enabled)
{
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, PORT0_IGNORE_VLAN_TAG_CONTROL + port, IGNOREITAGVID, enabled ? 0 : 1));
    
    return RT_ERR_OK;
}
    
int32 rtl8307h_vlan_portTagAware_get(uint32 port, uint32 *pEnabled)
{
    uint32 fieldVal;
    
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if (NULL == pEnabled)
        return RT_ERR_NULL_POINTER;
        
    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_IGNORE_VLAN_TAG_CONTROL + port, IGNOREITAGVID, &fieldVal));
    *pEnabled = fieldVal ? 0 : 1;
    
    return RT_ERR_OK;
}

int32 rtl8307h_vlan_portParserTagAbility_set(uint32 port,  uint32 enabled)
{
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, PORT0_PACKET_PARSER_CONTROL + port, ITAGEN, enabled ? 1 : 0));

    return RT_ERR_OK;
}

int32 rtl8307h_vlan_portParserTagAbility_get(uint32 port,  uint32 *pEnabled)
{
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if (NULL == pEnabled)
        return RT_ERR_NULL_POINTER;
        
    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_PACKET_PARSER_CONTROL + port, ITAGEN, pEnabled));

    return RT_ERR_OK;
}

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
int32 rtl8307h_vlan_portEgressTagStatus_set(uint32 port, rtk_vlan_txTagStatus_t tag_rxu, rtk_vlan_txTagStatus_t tag_rxn)
{
    uint32 regVal;
    uint32 rxuFileld;
    uint32 rxnFileld;
    
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_EGRESS_TAG_STATUS_CONTROL + port, &regVal));
    reg_field_get(RTL8307H_UNIT,  PORT0_EGRESS_TAG_STATUS_CONTROL + port, RXU_TAGSTAT, &rxuFileld, regVal);
    reg_field_get(RTL8307H_UNIT,  PORT0_EGRESS_TAG_STATUS_CONTROL + port, RXN_TAGSTAT, &rxnFileld, regVal); 

    reg_field_set(RTL8307H_UNIT,  PORT0_EGRESS_TAG_STATUS_CONTROL + port, RXU_TAGSTAT, (TAG_STATUS_WITH_TAG == tag_rxu) ? (rxuFileld & (~0x1)) : (rxuFileld | 0x1), &regVal);
    reg_field_set(RTL8307H_UNIT,  PORT0_EGRESS_TAG_STATUS_CONTROL + port, RXN_TAGSTAT, (TAG_STATUS_WITH_TAG == tag_rxn) ? (rxnFileld & (~0x1)) : (rxnFileld | 0x1), &regVal);
            
    CHK_FUN_RETVAL(reg_write(RTL8307H_UNIT, PORT0_EGRESS_TAG_STATUS_CONTROL + port, regVal));
    
    return RT_ERR_OK;
}

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
int32 rtl8307h_vlan_portEgressTagStatus_get(uint32 port,  rtk_vlan_txTagStatus_t  *pTag_rxu,  rtk_vlan_txTagStatus_t  *pTag_rxn)
{
    uint32 filedVal;
    
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_EGRESS_TAG_STATUS_CONTROL + port, RXU_TAGSTAT,  &filedVal) );
    *pTag_rxu = (filedVal & 0x1) ? TAG_STATUS_WITHOUT_TAG : TAG_STATUS_WITH_TAG;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_EGRESS_TAG_STATUS_CONTROL + port, RXN_TAGSTAT,  &filedVal) );
    *pTag_rxn = (filedVal & 0x1) ? TAG_STATUS_WITHOUT_TAG : TAG_STATUS_WITH_TAG;

    return RT_ERR_OK;
}

int32 rtl8307h_vlan_portIgrFilterEnable_set(uint32 port, uint32 enabled)
{
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, PORT0_VLAN_INGRESS_FILTER_CONTROL + port, IGFILTER, enabled ? 1 : 0));
   
    return RT_ERR_OK;
}

int32 rtl8307h_vlan_portIgrFilterEnable_get(uint32 port, uint32 *pEnabled)
{
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    if (NULL == pEnabled)
        return RT_ERR_NULL_POINTER;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_VLAN_INGRESS_FILTER_CONTROL + port, IGFILTER, pEnabled));
   
    return RT_ERR_OK;
}

int32 rtl8307h_vlan_portRole_set(uint32 port, rtl8307h_vlan_portRole_t portrole)
{
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
   
    if (portrole >= RTL8307H_PORTROLE_END)
        return RT_ERR_INPUT;

    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port,  PORTROLE, (uint32)portrole));

    return RT_ERR_OK;
}

int32 rtl8307h_vlan_portRole_get(uint32 port, rtl8307h_vlan_portRole_t *pPortrole)
{
    uint32 fieldval;
    
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
   
    if (NULL == pPortrole)
        return RT_ERR_NULL_POINTER;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port,  PORTROLE, &fieldval));
    *pPortrole = fieldval ? RTL8307H_PORTROLE_NNI : RTL8307H_PORTROLE_UNI;

    return RT_ERR_OK;
}

int32 rtl8307h_vlan_tpid_set(uint32 tpid)
{
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, PACKET_PARSER_PID_CONTROL_REGIST, TPID, tpid & 0xffff));                       
    
    return RT_ERR_OK;
}

#ifndef	EXT_LAB
int32 rtl83107h_vlan_tpid_get(uint32* pTpid)
{
    uint32 fieldval;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PACKET_PARSER_PID_CONTROL_REGIST, TPID, &fieldval));                       

    *pTpid = fieldval & 0xffff;
    
    return RT_ERR_OK;
}
#endif

int32 rtl8307h_vlan_flush_set(void)
{
    uint32 fieldval;
    uint32 pollcnt;
    
    reg_field_write(RTL8307H_UNIT, VLAN_CONTROL, FLUSHVLANTBL, 1);
    for (pollcnt = 0; pollcnt < RTL8307H_VLAN_FLUSH_TIMEOUT; pollcnt ++)
    {
        reg_field_read(RTL8307H_UNIT, VLAN_CONTROL, FLUSHVLANTBL, &fieldval);
        if (fieldval == 0)
            break;
    }
    if (RTL8307H_VLAN_FLUSH_TIMEOUT == pollcnt )
        return RT_ERR_FAILED;
    
    return RT_ERR_OK;
}

