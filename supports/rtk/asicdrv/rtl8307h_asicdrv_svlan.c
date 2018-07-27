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
 * $Revision: 6658 $
 * $Date: 2009-10-30 14:49:58 +0800 (Fri, 30 Oct 2009) $
 *
* Purpose : RTL8307H switch low-level API for SVLAN module
 * Feature : 
 *
 */
#include <rtk_types.h>
#include <rtk_error.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>
#include <rtl8307h_debug.h>

#include <rtl8307h_asicdrv_svlan.h>

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
int32 rtl8307h_svlan_portAcceptFrameType_set(uint32 port, rtk_vlan_acceptFrameType_t accept_frame_type)
{
    uint32 regVal;

    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, &regVal));    
    switch(accept_frame_type)
    {
        case ACCEPT_FRAME_TYPE_ALL:
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTOTAG, 1, &regVal);
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTOUTAG, 1, &regVal);                       
            break;
        case ACCEPT_FRAME_TYPE_TAG_ONLY:
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTOTAG, 1, &regVal);
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTOUTAG, 0, &regVal);                                   
            break;
        case ACCEPT_FRAME_TYPE_UNTAG_ONLY:
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTOTAG, 0, &regVal);
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTOUTAG, 1, &regVal);                                               
            break;
        case ACCEPT_FRAME_TYPE_NONE:
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTOTAG, 0, &regVal);
            reg_field_set(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTOUTAG, 0, &regVal);                                               
            break;
        default:
            return RT_ERR_INPUT;            
    }
    CHK_FUN_RETVAL(reg_write(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, regVal));        

    return RT_ERR_OK;
}

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
int32 rtl8307h_svlan_portAcceptFrameType_get(uint32 port, rtk_vlan_acceptFrameType_t *pAccept_frame_type)
{
    uint32 regVal, acctag, accutag;

    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, &regVal)); 
    reg_field_get(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTOTAG, &acctag, regVal);
    reg_field_get(RTL8307H_UNIT, PORT0_ACCEPT_FRAME_TYPE_CONTROL + port, ACPTOUTAG, &accutag, regVal);    
    *pAccept_frame_type = acctag ? 0: 0x2;
    *pAccept_frame_type +=  accutag ? 0: 0x1;

    return RT_ERR_OK;
}

int32 rtl8307h_svlan_portTagAware_set(uint32 port, uint32 enabled)
{
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, PORT0_IGNORE_VLAN_TAG_CONTROL + port, IGNOREOTAGVID, enabled ? 0 : 1));
    
    return RT_ERR_OK;
}
    
int32 rtl8307h_svlan_portTagAware_get(uint32 port, uint32 *pEnabled)
{
    uint32 fieldVal;
    
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if (NULL == pEnabled)
        return RT_ERR_NULL_POINTER;
        
    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_IGNORE_VLAN_TAG_CONTROL + port, IGNOREOTAGVID, &fieldVal));
    *pEnabled = fieldVal ? 0 : 1;
    
    return RT_ERR_OK;
}

int32 rtl8307h_svlan_portPktParserAbility_set(uint32 port,  uint32 enabled)
{
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, PORT0_PACKET_PARSER_CONTROL + port, OTAGEN, enabled ? 1 : 0));

    return RT_ERR_OK;
}

int32 rtl8307h_svlan_portPktParserAbility_get(uint32 port,  uint32 *pEnabled)
{
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if (NULL == pEnabled)
        return RT_ERR_NULL_POINTER;
        
    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_PACKET_PARSER_CONTROL + port, OTAGEN, pEnabled));

    return RT_ERR_OK;
}

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
int32 rtl8307h_svlan_portEgressTagMode_set(uint32 port, rtk_vlan_tagMode_t tag_rxu, rtk_vlan_tagMode_t tag_rxn)
{
    uint32 fieldVal1, fieldVal2;
    
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    if ((tag_rxu >= VLAN_TAG_MODE_END) || (tag_rxn >= VLAN_TAG_MODE_END))
        return RT_ERR_INPUT;
        
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
    
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, PORT0_TX_TAG_CONTROL0 + port, RXUTX_OTAG_KEEP, fieldVal1));     
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, PORT0_TX_TAG_CONTROL0 + port, RXNTX_OTAG_KEEP, fieldVal2));

    return RT_ERR_OK;
}


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
int32 rtl8307h_svlan_portEgressTagMode_get(uint32 port, rtk_vlan_tagMode_t *pTag_rxu, rtk_vlan_tagMode_t *pTag_rxn)
{
    uint32 fieldVal;
    
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    if ((NULL == pTag_rxu ) || ( NULL == pTag_rxn))
        return RT_ERR_NULL_POINTER;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_TX_TAG_CONTROL0 + port, RXUTX_OTAG_KEEP, &fieldVal));
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

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_TX_TAG_CONTROL0 + port, RXNTX_OTAG_KEEP, &fieldVal));   
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
int32 rtl8307h_svlan_portEgressTagStatus_set(uint32 port, rtk_vlan_txTagStatus_t tag_rxu, rtk_vlan_txTagStatus_t tag_rxn)
{
    uint32 regVal;
    uint32 rxuFileld;
    uint32 rxnFileld;
    
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_EGRESS_TAG_STATUS_CONTROL + port, &regVal));
    reg_field_get(RTL8307H_UNIT,  PORT0_EGRESS_TAG_STATUS_CONTROL + port, RXU_TAGSTAT, &rxuFileld, regVal);
    reg_field_get(RTL8307H_UNIT,  PORT0_EGRESS_TAG_STATUS_CONTROL + port, RXN_TAGSTAT, &rxnFileld, regVal); 

    reg_field_set(RTL8307H_UNIT,  PORT0_EGRESS_TAG_STATUS_CONTROL + port, RXU_TAGSTAT, (TAG_STATUS_WITH_TAG == tag_rxu) ? (rxuFileld & (~0x2)) : (rxuFileld | 0x2), &regVal);
    reg_field_set(RTL8307H_UNIT,  PORT0_EGRESS_TAG_STATUS_CONTROL + port, RXN_TAGSTAT, (TAG_STATUS_WITH_TAG == tag_rxn) ? (rxnFileld & (~0x2)) : (rxnFileld | 0x2), &regVal);
            
    CHK_FUN_RETVAL(reg_write(RTL8307H_UNIT, PORT0_EGRESS_TAG_STATUS_CONTROL + port, regVal));
    
    return RT_ERR_OK;
}


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
int32 rtl8307h_svlan_portEgressTagStatus_get(uint32 port,  rtk_vlan_txTagStatus_t  *pTag_rxu,  rtk_vlan_txTagStatus_t  *pTag_rxn)
{
    uint32 filedVal;
    
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_EGRESS_TAG_STATUS_CONTROL + port, RXU_TAGSTAT,  &filedVal) );
    *pTag_rxu = (filedVal & 0x2) ?  TAG_STATUS_WITHOUT_TAG : TAG_STATUS_WITH_TAG;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PORT0_EGRESS_TAG_STATUS_CONTROL + port, RXN_TAGSTAT,  &filedVal) );
    *pTag_rxn = (filedVal & 0x2) ? TAG_STATUS_WITHOUT_TAG : TAG_STATUS_WITH_TAG;

    return RT_ERR_OK;
}

int32 rtl8307h_svlan_portFwdBaseOvid_set(uint32 port, uint32 enabled)
{
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, port + PORT0_FORWARDING_TAG_SELECT_CONTROL, FWDBASE_OVID, enabled));           

    return RT_ERR_OK;
}

int32 rtl8307h_svlan_portFwdBaseOvid_get(uint32 port, uint32 *pEnabled)
{
    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if (NULL ==  pEnabled)
        return RT_ERR_NULL_POINTER;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, port + PORT0_FORWARDING_TAG_SELECT_CONTROL, FWDBASE_OVID, pEnabled));           

    return RT_ERR_OK;
}

int32 rtl8307h_svlan_portPvid_set(uint32 port, uint32 svid, uint32 pri, uint32 dei)
{
    uint32 regVal;

    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;

    if (svid > RTL8307H_VIDMAX)
        return RT_ERR_SVLAN_VID;

    if(pri > RTL8307H_PRIMAX) 
        return RT_ERR_VLAN_PRIORITY;

    if (dei > 1)
        return RT_ERR_INPUT;

    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, &regVal));
    reg_field_set(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, POVID, svid, &regVal);
    reg_field_set(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, POPRI, pri, &regVal);
    reg_field_set(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, PODEI, dei, &regVal);
    CHK_FUN_RETVAL(reg_write(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, regVal));
   
    return RT_ERR_OK;
}


int32 rtl8307h_svlan_portPvid_get(uint32 port, uint32 *pSvid, uint32 *pPri, uint32 *pDei)
{
    uint32 regVal;

    if (!IS_VALID_PORT_ID(port))
        return RT_ERR_PORT_ID;
    
    if ((NULL == pSvid) || (NULL == pPri) || (NULL == pDei))
        return RT_ERR_NULL_POINTER;

    CHK_FUN_RETVAL(reg_read(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, &regVal));
    reg_field_get(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, POVID, pSvid, regVal);
    reg_field_get(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, POPRI, pPri, regVal);
    reg_field_get(RTL8307H_UNIT, PORT0_VLAN_CONTROL + port, PODEI, pDei, regVal);
   
    return RT_ERR_OK;
}

 int32 rtl8307h_svlan_tpid_set(uint32 tpid)
{
    CHK_FUN_RETVAL(reg_field_write(RTL8307H_UNIT, PACKET_PARSER_PID_CONTROL_REGIST, SPID, tpid & 0xffff));                       
    
    return RT_ERR_OK;
}


int32 rtl8307h_svlan_tpid_get(uint32* pTpid)
{
    uint32 fieldval;

    CHK_FUN_RETVAL(reg_field_read(RTL8307H_UNIT, PACKET_PARSER_PID_CONTROL_REGIST, SPID, &fieldval));                       

    *pTpid = fieldval & 0xffff;
    
    return RT_ERR_OK;
}


