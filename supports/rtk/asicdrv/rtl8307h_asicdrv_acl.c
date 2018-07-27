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
 * Purpose : RTL8307H switch low-level API for ACL module
 * Feature : 
 *
 */

//FRANK ADDED include string for memset
#include <string.h> /* memset */


#include <rtk_types.h>
#include <rtk_error.h>
#include <rtk_api.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>
#include <rtl8307h_table_struct.h>

#include <rtl8307h_asicdrv_acl.h>


int32 rtl8307h_acl_actTblEntry_set(uint32 index, rtl8307h_aclAct_entry_t * actEntry_p)
{
    rtl8307h_tblasic_act_entry_t entry;
    uint32 infonum = 0;

    if (NULL == actEntry_p)
    {
        return RT_ERR_NULL_POINTER;
    }
    
    if (index >= RTL8307H_ACLACT_ENTRYNUM)
    {
        return RT_ERR_INPUT;
    }

    memset(&entry, 0, sizeof(entry));
    entry.val[0] = ((uint32)actEntry_p->fno << 31)          |           \
                    ((uint32)actEntry_p->drop << 29)        |           \
                    ((uint32)actEntry_p->copytocpu << 28)   |           \
                    ((uint32)actEntry_p->mirror << 27)      |           \
                    ((uint32)actEntry_p->otag << 26)        |           \
                    ((uint32)actEntry_p->itag << 25)        |           \
                    ((uint32)actEntry_p->priormk << 24)     |           \
                    ((uint32)actEntry_p->redir << 23)       |           \
                    ((uint32)actEntry_p->dscprmk << 22)     |           \
                    ((uint32)actEntry_p->prioasn << 21)     |           \
                    ((uint32)actEntry_p->byratelmt << 20);

    infonum++;

    if (1 == actEntry_p->otag)
    {
        entry.val[infonum] = ((uint32)actEntry_p->outTagOpInfo.withdraw << 31)       |           \
                                    ((uint32)actEntry_p->outTagOpInfo.outVidCtl << 29)      |           \
                                    ((uint32)actEntry_p->outTagOpInfo.outVidInfo << 17)      |           \
                                    ((uint32)actEntry_p->outTagOpInfo.outTagOp << 15);
        infonum++;
    }
    
    if (1 == actEntry_p->itag)
    {
        entry.val[infonum] = ((uint32)actEntry_p->inTagOpInfo.withdraw << 31)       |           \
                                    ((uint32)actEntry_p->inTagOpInfo.inVidCtl << 29)      |           \
                                    ((uint32)actEntry_p->inTagOpInfo.inVidInfo << 17)      |           \
                                    ((uint32)actEntry_p->inTagOpInfo.inTagOp << 15);
        infonum ++;
    }
    
    if (1 == actEntry_p->priormk)
    {
        entry.val[infonum] = ((uint32)actEntry_p->PriRmkInfo.withdraw << 31)       |           \
                                    ((uint32)actEntry_p->PriRmkInfo.tagSel << 28)      |           \
                                    ((uint32)actEntry_p->PriRmkInfo.inPri << 25)      |           \
                                    ((uint32)actEntry_p->PriRmkInfo.outPri << 22)    |           \
                                    ((uint32)actEntry_p->PriRmkInfo.dei << 21);
        infonum++;
    }

    if ((1 == actEntry_p->redir) && (infonum < 4))
    {
        entry.val[infonum] = ((uint32)actEntry_p->redirInfo.withdraw << 31)       |           \
                                    ((uint32)actEntry_p->redirInfo.opcode << 30)      |           \
                                    ((uint32)actEntry_p->redirInfo.crsvlan << 29)      |           \
                                    ((uint32)actEntry_p->redirInfo.dpm << 21);
        infonum++;
    }

    if ((1 == actEntry_p->dscprmk) && (infonum < 4))
    {
        if (0 == actEntry_p->dscpRmkInfo.opcode)
        {
            entry.val[infonum] = ((uint32)actEntry_p->dscpRmkInfo.withdraw << 31)       |           \
                                    ((uint32)actEntry_p->redirInfo.opcode << 29)      |           \
                                    ((uint32)actEntry_p->dscpRmkInfo.dscpRmk.dscp.acldscp << 23);

        }
        else if (1 == actEntry_p->dscpRmkInfo.opcode)
        {
            entry.val[infonum] = ((uint32)actEntry_p->dscpRmkInfo.withdraw << 31)       |           \
                                    ((uint32)actEntry_p->redirInfo.opcode << 29)      |           \
                                    ((uint32)actEntry_p->dscpRmkInfo.dscpRmk.ipPrec.ipPrece << 26);
        }
        else if (2 == actEntry_p->dscpRmkInfo.opcode)
        {
            entry.val[infonum] = ((uint32)actEntry_p->dscpRmkInfo.withdraw << 31)       |           \
                                    ((uint32)actEntry_p->redirInfo.opcode << 29)      |           \
                                    ((uint32)actEntry_p->dscpRmkInfo.dscpRmk.dtr.dtr << 23);
        }
        else
        {
            return RT_ERR_INPUT;
        }

        infonum++;
    }

    if ((1 == actEntry_p->prioasn) && (infonum < 4))
    {
        entry.val[infonum] = ((uint32)actEntry_p->dscpRmkInfo.withdraw << 31)       |           \
                                ((uint32)actEntry_p->prioInfo.priority << 28);
        infonum++;
    }

    CHK_FUN_RETVAL(table_write(RTL8307H_UNIT, ACTION_TABLE, index, (uint32 *)entry.val));

    return RT_ERR_OK;
}

int32 rtl8307h_acl_actTblEntry_get(uint32 index, rtl8307h_aclAct_entry_t* actEntry_p)
{
    rtl8307h_tblasic_act_entry_t entry;
    uint32 infonum = 0;

    if (NULL == actEntry_p)
    {
        return RT_ERR_NULL_POINTER;
    }
    
    if (index >= RTL8307H_ACLACT_ENTRYNUM)
    {
        return RT_ERR_INPUT;
    }

    memset(&entry, 0, sizeof(entry));
    CHK_FUN_RETVAL(table_read(RTL8307H_UNIT, ACTION_TABLE, index, (uint32 *)entry.val));
    
    memset(actEntry_p, 0, sizeof(rtl8307h_aclAct_entry_t)); 
    actEntry_p->fno = (uint8)((entry.val[0] >> 31) & 0x1);
    actEntry_p->drop = (uint8)((entry.val[0] >> 29) & 0x1);
    actEntry_p->copytocpu = (uint8)((entry.val[0] >> 28) & 0x1);
    actEntry_p->mirror = (uint8)((entry.val[0] >> 27) & 0x1);
    actEntry_p->otag = (uint8)((entry.val[0] >> 26) & 0x1);
    actEntry_p->itag = (uint8)((entry.val[0] >> 25) & 0x1);
    actEntry_p->itag = (uint8)((entry.val[0] >> 25) & 0x1);
    actEntry_p->priormk = (uint8)((entry.val[0] >> 24) & 0x1);
    actEntry_p->redir = (uint8)((entry.val[0] >> 23) & 0x1);
    actEntry_p->dscprmk = (uint8)((entry.val[0] >> 22) & 0x1);
    actEntry_p->prioasn = (uint8)((entry.val[0] >> 21) & 0x1);
    actEntry_p->byratelmt = (uint8)((entry.val[0] >> 20) & 0x1);
    
    infonum++;
        
    if (1 == actEntry_p->otag)
    {
        actEntry_p->outTagOpInfo.withdraw = (uint8)((entry.val[infonum] >> 31) & 0x1);
        actEntry_p->outTagOpInfo.outVidCtl = (uint8)((entry.val[infonum] >> 29) & 0x3);
        actEntry_p->outTagOpInfo.outVidInfo = (uint16)((entry.val[infonum] >> 17) & 0xFFFUL);
        actEntry_p->outTagOpInfo.outTagOp = (uint8)((entry.val[infonum] >> 15) & 0x3);
        
        infonum++;
    }

    if (1 == actEntry_p->itag)
    {
        actEntry_p->inTagOpInfo.withdraw = (uint8)((entry.val[infonum] >> 31) & 0x1);
        actEntry_p->inTagOpInfo.inVidCtl = (uint8)((entry.val[infonum] >> 29) & 0x3);
        actEntry_p->inTagOpInfo.inVidInfo = (uint16)((entry.val[infonum] >> 17) & 0xFFFUL);
        actEntry_p->inTagOpInfo.inTagOp = (uint8)((entry.val[infonum] >> 15) & 0x3);
        
        infonum++;
    }

    if (1 == actEntry_p->priormk)
    {
        actEntry_p->PriRmkInfo.withdraw = (uint8)((entry.val[infonum] >> 31) & 0x1);
        actEntry_p->PriRmkInfo.tagSel = (uint8)((entry.val[infonum] >> 28) & 0xF);
        actEntry_p->PriRmkInfo.inPri = (uint8)((entry.val[infonum] >> 25) & 0xF);
        actEntry_p->PriRmkInfo.outPri = (uint8)((entry.val[infonum] >> 22) & 0xF);
        actEntry_p->PriRmkInfo.dei = (uint8)((entry.val[infonum] >> 21) & 0x1);
                
        infonum++;
    }

    if((1 == actEntry_p->redir) && (infonum < 4))
    {
        actEntry_p->redirInfo.withdraw = (uint8)((entry.val[infonum] >> 31) & 0x1);
        actEntry_p->redirInfo.opcode = (uint8)((entry.val[infonum] >> 30) & 0x1);
        actEntry_p->redirInfo.crsvlan = (uint8)((entry.val[infonum] >> 29) & 0x1);
        actEntry_p->redirInfo.dpm = (uint8)((entry.val[infonum] >> 21) & 0xFFUL);
        
        infonum++;
    }

    if ((1 == actEntry_p->dscprmk) && (infonum < 4))
    {
        actEntry_p->dscpRmkInfo.withdraw = (uint8)((entry.val[infonum] >> 31) & 0x1);
        actEntry_p->dscpRmkInfo.opcode = (uint8)((entry.val[infonum] >> 29) & 0x3);
        if (0 == actEntry_p->dscpRmkInfo.opcode)
        {
            actEntry_p->dscpRmkInfo.dscpRmk.dscp.acldscp = (uint8)((entry.val[infonum] >> 23) & 0x3F);
        }
        else if (1 == actEntry_p->dscpRmkInfo.opcode)
        {
            actEntry_p->dscpRmkInfo.dscpRmk.ipPrec.ipPrece =  (uint8)((entry.val[infonum] >> 26) & 0x7);
        }
        else if (2 == actEntry_p->dscpRmkInfo.opcode)
        {
            actEntry_p->dscpRmkInfo.dscpRmk.dtr.dtr = (uint8)((entry.val[infonum] >> 23) & 0x7);
        }
        else
        {
            ;
        }

        infonum++;
    }

    if ((1 == actEntry_p->prioasn) && (infonum < 4))
    {
        actEntry_p->prioInfo.withdraw = (uint8)((entry.val[infonum] >> 31) & 0x1);
        actEntry_p->prioInfo.priority = (uint8)((entry.val[infonum] >> 28) & 0x7);
        infonum++;
    }
    
    return RT_ERR_OK;
    
}

int32 _rtl8307h_inAclRule_sw2asic(rtk_filter_field_type_t fieldType, uint16* asicData, rtk_filter_pieTempl_field_t swData)
{
    rtl8307h_asicPieTempl_field_t *pData;
    
    if (asicData == NULL)
    {
        return RT_ERR_NULL_POINTER;
    }
    
    pData = (rtl8307h_asicPieTempl_field_t*)asicData;
    
    switch (fieldType)
    {
        case FILTER_FIELD_FMT:
            pData->un.format.swip = swData.un.format.swip;
            pData->un.format.noneZeroOff = swData.un.format.noneZeroOff;
            pData->un.format.recvPort = swData.un.format.recvPort;
            pData->un.format.tgL2Fmt = swData.un.format.tgL2Fmt;
            pData->un.format.itagExist = swData.un.format.itagExist;
            pData->un.format.otagExist = swData.un.format.otagExist;
            pData->un.format.tgL23Fmt = swData.un.format.tgL23Fmt;
            pData->un.format.tgL4Fmt = swData.un.format.tgL4Fmt;
            pData->un.format.ispppoe = swData.un.format.ispppoe;
            break;
            
        case FILTER_FIELD_DMAC0:
            pData->un.dmac0 = swData.un.dmac0;  
            break;
            
        case FILTER_FIELD_DMAC1:                
            pData->un.dmac1 = swData.un.dmac1;
            break;
            
        case FILTER_FIELD_DMAC2:
            pData->un.dmac2 = swData.un.dmac2;
            break;
            
        case FILTER_FIELD_SMAC0:
            pData->un.smac0 = swData.un.smac0;
            break;
            
        case FILTER_FIELD_SMAC1:
            pData->un.smac1 = swData.un.smac1;
            break;
            
        case FILTER_FIELD_SMAC2:
            pData->un.smac2 = swData.un.smac2;
            break;
            
        case FILTER_FIELD_ETHERTYPE:
            pData->un.ethType = swData.un.ethType;
            break;
            
        case FILTER_FIELD_OTAG:
            pData->un.otag.opri = swData.un.otag.opri;
            pData->un.otag.dei = swData.un.otag.dei;
            pData->un.otag.ovid = swData.un.otag.ovid;
            break;
            
        case FILTER_FIELD_ITAG:
            pData->un.itag.ipri = swData.un.itag.ipri;
            pData->un.itag.itagExist = swData.un.itag.itagExist;
            pData->un.itag.ivid = swData.un.itag.ivid;
            break;
            
        case FILTER_FIELD_PRIORITY:
            pData->un.prio.itagPri = swData.un.prio.itagPri;
            pData->un.prio.itagExist = swData.un.prio.itagExist;
            pData->un.prio.otagPri = swData.un.prio.otagPri;
            pData->un.prio.dei = swData.un.prio.dei;
            pData->un.prio.otagExist = swData.un.prio.otagExist;
            pData->un.prio.fwdPri = swData.un.prio.fwdPri;
            pData->un.prio.cfi = swData.un.prio.cfi;
            break;
    
        case FILTER_FIELD_L2PROTOCOL:                
            pData->un.l2Proto = swData.un.l2Proto;
            break;
    
        case FILTER_FIELD_SIP0:
            pData->un.sip15_0 = swData.un.sip15_0;    
            break;
            
        case FILTER_FIELD_SIP1:
            pData->un.sip31_16 = swData.un.sip31_16;
            break;
    
        case FILTER_FIELD_DIP0:
            pData->un.dip15_0 = swData.un.dip15_0;
            break;
    
        case FILTER_FIELD_DIP1:
            pData->un.dip31_16 = swData.un.dip31_16;
            break;
    
        case FILTER_FIELD_IPHDR:
            pData->un.ipHdr.dscp = swData.un.ipHdr.dscp;
            pData->un.ipHdr.morefrag = swData.un.ipHdr.morefrag;
            pData->un.ipHdr.noneZeroOff = swData.un.ipHdr.noneZeroOff;
            pData->un.ipHdr.l4Proto = swData.un.ipHdr.l4Proto;
            break;
    
        case FILTER_FIELD_GIP0:
            pData->un.grpIp15_0 = swData.un.grpIp15_0;    
            break;
            
        case FILTER_FIELD_GIP1:
            pData->un.grpIph.grpIp28_16 = swData.un.grpIph.grpIp28_16;
            break;
    
        case FILTER_FIELD_L4SPORT:
            pData->un.l4SrcPort = swData.un.l4SrcPort;    
            break;
    
        case FILTER_FIELD_L4DPORT:
            pData->un.l4DstPort = swData.un.l4DstPort ;
            break;
    
        case FILTER_FIELD_L4HDR:
            pData->un.l4Hdr.tcpFlags = swData.un.l4Hdr.tcpFlags;                
            pData->un.l4Hdr.igmpType = swData.un.l4Hdr.igmpType;    
            break;
    
        case FILTER_FIELD_ICMPCODETYPE:
            pData->un.icmpHdr.icmpCode = swData.un.icmpHdr.icmpCode;
            pData->un.icmpHdr.icmpType = swData.un.icmpHdr.icmpType;
            break;
            
        case FILTER_FIELD_SPM1:
            pData->un.rcvSrcPm1.rcvPortMask = swData.un.rcvSrcPm1.rcvPortMask;
            pData->un.rcvSrcPm1.dmacType = swData.un.rcvSrcPm1.dmacType;
            pData->un.rcvSrcPm1.doNotFrag = swData.un.rcvSrcPm1.doNotFrag;
            pData->un.rcvSrcPm1.ttlType = swData.un.rcvSrcPm1.ttlType;
            pData->un.rcvSrcPm1.rtkpp = swData.un.rcvSrcPm1.rtkpp;
            break;
    
        case FILTER_FIELD_RANGE0:
            pData->un.rangeTbl0.op = swData.un.rangeTbl0.op;
            pData->un.rangeTbl0.vid = swData.un.rangeTbl0.vid;
            pData->un.rangeTbl0.len = swData.un.rangeTbl0.len;
            break;
            
        case FILTER_FIELD_RANGE1:
            pData->un.rangeTbl1.l4Port = swData.un.rangeTbl1.l4Port;
            pData->un.rangeTbl1.rcvPort = swData.un.rangeTbl1.rcvPort;
            pData->un.rangeTbl1.ipRange = swData.un.rangeTbl1.ipRange;    
            break;
    
        case FILTER_FIELD_PAYLOAD:
            pData->un.payload = swData.un.payload;
            break;

        case FILTER_FIELD_UNUSED:
            pData->un.payload = 0;
            break;

        default:                
            return RT_ERR_FAILED;
    
    }

    return RT_ERR_OK;
}

int32 _rtl8307h_inAclRule_asic2sw(rtk_filter_field_type_t fieldType, uint16 asicData, rtk_filter_pieTempl_field_t* swData)
{
    rtl8307h_asicPieTempl_field_t *pData;

    if (NULL == swData)
    {
        return RT_ERR_NULL_POINTER;
    }
    
    pData = (rtl8307h_asicPieTempl_field_t*)&asicData;

    switch (fieldType)
    {
        case FILTER_FIELD_FMT:
            swData->un.format.swip = pData->un.format.swip;
            swData->un.format.noneZeroOff = pData->un.format.noneZeroOff;
            swData->un.format.recvPort = pData->un.format.recvPort;
            swData->un.format.tgL2Fmt = pData->un.format.tgL2Fmt;
            swData->un.format.itagExist = pData->un.format.itagExist;
            swData->un.format.otagExist = pData->un.format.otagExist;
            swData->un.format.tgL23Fmt = pData->un.format.tgL23Fmt;
            swData->un.format.tgL4Fmt = pData->un.format.tgL4Fmt;
            swData->un.format.ispppoe = pData->un.format.ispppoe;
            break;
            
        case FILTER_FIELD_DMAC0:
            swData->un.dmac0 = pData->un.dmac0;
            break;
            
        case FILTER_FIELD_DMAC1:
            swData->un.dmac1 = pData->un.dmac1;
            break;
            
        case FILTER_FIELD_DMAC2:
            swData->un.dmac2 = pData->un.dmac2;
            break;
            
        case FILTER_FIELD_SMAC0:
            swData->un.smac0 = pData->un.smac0;
            break;
            
        case FILTER_FIELD_SMAC1:
            swData->un.smac1 = pData->un.smac1;
            break;
            
        case FILTER_FIELD_SMAC2:
            swData->un.smac2 = pData->un.smac2;
            break;
            
        case FILTER_FIELD_ETHERTYPE:
            swData->un.ethType = pData->un.ethType;
            break;
            
        case FILTER_FIELD_OTAG:
            swData->un.otag.opri = pData->un.otag.opri;
            swData->un.otag.dei = pData->un.otag.dei;
            swData->un.otag.ovid = pData->un.otag.ovid;
            break;
            
        case FILTER_FIELD_ITAG:
            swData->un.itag.ipri = pData->un.itag.ipri;
            swData->un.itag.itagExist = pData->un.itag.itagExist;
            swData->un.itag.ivid = pData->un.itag.ivid;
            break;
            
        case FILTER_FIELD_PRIORITY:
            swData->un.prio.itagPri = pData->un.prio.itagPri;
            swData->un.prio.itagExist = pData->un.prio.itagExist;
            swData->un.prio.otagPri = pData->un.prio.otagPri;
            swData->un.prio.dei = pData->un.prio.dei;
            swData->un.prio.otagExist = pData->un.prio.otagExist;
            swData->un.prio.fwdPri = pData->un.prio.fwdPri;
            swData->un.prio.cfi = pData->un.prio.cfi;
            break;
    
        case FILTER_FIELD_L2PROTOCOL:
            swData->un.l2Proto = pData->un.l2Proto;
            break;
    
        case FILTER_FIELD_SIP0:
            swData->un.sip15_0 = pData->un.sip15_0;   
            break;
            
        case FILTER_FIELD_SIP1:
            swData->un.sip31_16 = pData->un.sip31_16;
            break;
    
        case FILTER_FIELD_DIP0:
            swData->un.dip15_0 = pData->un.dip15_0;
            break;
    
        case FILTER_FIELD_DIP1:
            swData->un.dip31_16 = pData->un.dip31_16;
            break;
    
        case FILTER_FIELD_IPHDR:
            swData->un.ipHdr.dscp = pData->un.ipHdr.dscp;
            swData->un.ipHdr.morefrag = pData->un.ipHdr.morefrag;
            swData->un.ipHdr.noneZeroOff = pData->un.ipHdr.noneZeroOff;
            swData->un.ipHdr.l4Proto = pData->un.ipHdr.l4Proto;
            break;
    
        case FILTER_FIELD_GIP0:
            swData->un.grpIp15_0 = pData->un.grpIp15_0;
            break;
            
        case FILTER_FIELD_GIP1:
            swData->un.grpIph.grpIp28_16 = pData->un.grpIph.grpIp28_16;    
            break;
    
        case FILTER_FIELD_L4SPORT:
            swData->un.l4SrcPort = pData->un.l4SrcPort;    
            break;
    
        case FILTER_FIELD_L4DPORT:
            swData->un.l4DstPort = pData->un.l4DstPort;
            break;
    
        case FILTER_FIELD_L4HDR:
            swData->un.l4Hdr.tcpFlags = pData->un.l4Hdr.tcpFlags;
            swData->un.l4Hdr.igmpType = pData->un.l4Hdr.igmpType;
            break;
    
        case FILTER_FIELD_ICMPCODETYPE:
            swData->un.icmpHdr.icmpCode = pData->un.icmpHdr.icmpCode;
            swData->un.icmpHdr.icmpType = pData->un.icmpHdr.icmpType;
            break;
    
        case FILTER_FIELD_SPM1:
            swData->un.rcvSrcPm1.rcvPortMask= pData->un.rcvSrcPm1.rcvPortMask;
            swData->un.rcvSrcPm1.dmacType = pData->un.rcvSrcPm1.dmacType;
            swData->un.rcvSrcPm1.doNotFrag = pData->un.rcvSrcPm1.doNotFrag;
            swData->un.rcvSrcPm1.ttlType = pData->un.rcvSrcPm1.ttlType;
            swData->un.rcvSrcPm1.rtkpp = pData->un.rcvSrcPm1.rtkpp;
            break;
    
        case FILTER_FIELD_RANGE0:
            swData->un.rangeTbl0.op = pData->un.rangeTbl0.op;
            swData->un.rangeTbl0.vid = pData->un.rangeTbl0.vid;
            swData->un.rangeTbl0.len = pData->un.rangeTbl0.len;
            break;
            
        case FILTER_FIELD_RANGE1:
            swData->un.rangeTbl1.l4Port = pData->un.rangeTbl1.l4Port;
            swData->un.rangeTbl1.rcvPort = pData->un.rangeTbl1.rcvPort;
            swData->un.rangeTbl1.ipRange = pData->un.rangeTbl1.ipRange;
            break;
    
        case FILTER_FIELD_PAYLOAD:
            swData->un.payload = pData->un.payload;
            break;

        case FILTER_FIELD_UNUSED:
            swData->un.payload = 0;
            break;
    
        default:                
            return RT_ERR_FAILED;    
    }

    return RT_ERR_OK;
}

static int32  _rtl8307h_aclRule_param2entry(rtl8307h_tblasic_acl_entry_t *pEntry,  rtl8307h_aclRule_param_t* pAclParam)
{
    memset(pEntry, 0, sizeof(rtl8307h_tblasic_acl_entry_t));
    
    pAclParam->valid &= 0x1;

    pEntry->val[0] = ((uint32)pAclParam->field1 << 16)      |           \
                    ((uint32)pAclParam->field0 << 0);
    
    pEntry->val[1] = ((uint32)pAclParam->field3 << 16)      |           \
                    ((uint32)pAclParam->field2 << 0);
    
    pEntry->val[2] = ((uint32)pAclParam->field5 << 16)      |           \
                    ((uint32)pAclParam->field4 << 0);
    
    pEntry->val[3] = ((uint32)pAclParam->field7 << 16)      |           \
                    ((uint32)pAclParam->field6 << 0);
    
    pEntry->val[4] = ((uint32)pAclParam->valid << 16)       |           \
                    ((uint32)pAclParam->field8 << 0);

    pEntry->val[5] = ((uint32)pAclParam->mask1 << 16)      |           \
                    ((uint32)pAclParam->mask0 << 0);

     pEntry->val[6] = ((uint32)pAclParam->mask3 << 16)      |           \
                    ((uint32)pAclParam->mask2 << 0);

     pEntry->val[7] = ((uint32)pAclParam->mask5 << 16)      |           \
                    ((uint32)pAclParam->mask4 << 0);
     
     pEntry->val[8] = ((uint32)pAclParam->mask7 << 16)      |           \
                    ((uint32)pAclParam->mask6 << 0);

     pEntry->val[9] = ((uint32)pAclParam->mask8 << 0);
    
    return SUCCESS;
}

static int32 _rtl8307h_aclRule_entry2param(const rtl8307h_tblasic_acl_entry_t *pEntry, rtl8307h_aclRule_param_t *pAclParam)
{
    memset(pAclParam, 0, sizeof(rtl8307h_aclRule_param_t)); 

    pAclParam->field1 = (uint16)((pEntry->val[0] >> 16) & 0xFFFFUL);
    pAclParam->field0 = (uint16)((pEntry->val[0] >> 0) & 0xFFFFUL);
    
    pAclParam->field3 = (uint16)((pEntry->val[1] >> 16) & 0xFFFFUL);
    pAclParam->field2 = (uint16)((pEntry->val[1] >> 0) & 0xFFFFUL);

    pAclParam->field5 = (uint16)((pEntry->val[2] >> 16) & 0xFFFFUL);
    pAclParam->field4 = (uint16)((pEntry->val[2] >> 0) & 0xFFFFUL);
    
    pAclParam->field7 = (uint16)((pEntry->val[3] >> 16) & 0xFFFFUL);
    pAclParam->field6 = (uint16)((pEntry->val[3] >> 0) & 0xFFFFUL);
    
    pAclParam->valid = (uint8)((pEntry->val[4] >> 16) & 0x1);
    pAclParam->field8 = (uint16)((pEntry->val[4] >> 0) & 0xFFFFUL);


    pAclParam->mask1 = (uint16)((pEntry->val[5] >> 16) & 0xFFFFUL);
    pAclParam->mask0 = (uint16)((pEntry->val[5] >> 0) & 0xFFFFUL);
    
    pAclParam->mask3 = (uint16)((pEntry->val[6] >> 16) & 0xFFFFUL);
    pAclParam->mask2 = (uint16)((pEntry->val[6] >> 0) & 0xFFFFUL);
    
    pAclParam->mask5 = (uint16)((pEntry->val[7] >> 16) & 0xFFFFUL);
    pAclParam->mask4 = (uint16)((pEntry->val[7] >> 0) & 0xFFFFUL);

    pAclParam->mask7 = (uint16)((pEntry->val[8] >> 16) & 0xFFFFUL);
    pAclParam->mask6 = (uint16)((pEntry->val[8] >> 0) & 0xFFFFUL);

    pAclParam->mask8 = (uint16)((pEntry->val[9] >> 0) & 0xFFFFUL);
    
    return SUCCESS;
}


/*convert raw ASIC data to readable ACL rule entry*/
int32 rtl8307h_acl_ruleEntry_set(uint32 index, rtl8307h_aclEntry_t* pAclEntry)
{
    rtl8307h_tblasic_acl_entry_t entry;
    rtl8307h_aclRule_param_t param;
    uint32 retvalue;

    if (NULL == pAclEntry)
    {
        return RT_ERR_NULL_POINTER;
    }
    
    if (index < 0 || index >= RTL8307H_PIE_ENTRYNUM)
    {
        return RT_ERR_INPUT;
    }

    if (0 == pAclEntry->valid)
    {
        return RT_ERR_FAILED;
    }
    
    memset(&param, 0, sizeof(param));
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[0].fieldType, &param.field0, pAclEntry->ruleField[0].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[0].fieldType, &param.mask0, pAclEntry->ruleField[0].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[1].fieldType, &param.field1, pAclEntry->ruleField[1].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[1].fieldType, &param.mask1, pAclEntry->ruleField[1].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[2].fieldType, &param.field2, pAclEntry->ruleField[2].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[2].fieldType, &param.mask2, pAclEntry->ruleField[2].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[3].fieldType, &param.field3, pAclEntry->ruleField[3].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[3].fieldType, &param.mask3, pAclEntry->ruleField[3].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[4].fieldType, &param.field4, pAclEntry->ruleField[4].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[4].fieldType, &param.mask4, pAclEntry->ruleField[4].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[5].fieldType, &param.field5, pAclEntry->ruleField[5].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[5].fieldType, &param.mask5, pAclEntry->ruleField[5].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[6].fieldType, &param.field6, pAclEntry->ruleField[6].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[6].fieldType, &param.mask6, pAclEntry->ruleField[6].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[7].fieldType, &param.field7, pAclEntry->ruleField[7].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[7].fieldType, &param.mask7, pAclEntry->ruleField[7].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[8].fieldType, &param.field8, pAclEntry->ruleField[8].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_sw2asic(pAclEntry->ruleField[8].fieldType, &param.mask8, pAclEntry->ruleField[8].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;

    param.valid = 1;

     _rtl8307h_aclRule_param2entry(&entry, &param);

    retvalue = table_write(RTL8307H_UNIT, PIE_TABLE, index, (uint32 *)entry.val);

    return retvalue;
}

/*convert raw ASIC data to readable ACL rule entry, should fill pAclEntry->rule[n].fieldType first*/
int32 rtl8307h_acl_ruleEntry_get(uint32 index, rtl8307h_aclEntry_t* pAclEntry)
{
    rtl8307h_tblasic_acl_entry_t entry;
    rtl8307h_aclRule_param_t param;
    
    if (NULL == pAclEntry)
    {
        return RT_ERR_NULL_POINTER;
    }
    
    if (index < 0 || index >= RTL8307H_PIE_ENTRYNUM)
    {
        return RT_ERR_INPUT;
    }

    memset(&entry, 0, sizeof(entry));
    CHK_FUN_RETVAL(table_read(RTL8307H_UNIT, PIE_TABLE, index, (uint32 *)entry.val));

    _rtl8307h_aclRule_entry2param(&entry, &param);
   
    if (0 == param.valid)
    {
        pAclEntry->valid = 0;
        return RT_ERR_OK;
    }
    
    pAclEntry->valid = 1;
    
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[0].fieldType, param.field0, &pAclEntry->ruleField[0].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[0].fieldType, param.mask0, &pAclEntry->ruleField[0].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[1].fieldType, param.field1, &pAclEntry->ruleField[1].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[1].fieldType, param.mask1, &pAclEntry->ruleField[1].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[2].fieldType, param.field2, &pAclEntry->ruleField[2].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[2].fieldType, param.mask2, &pAclEntry->ruleField[2].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[3].fieldType, param.field3, &pAclEntry->ruleField[3].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[3].fieldType, param.mask3, &pAclEntry->ruleField[3].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[4].fieldType, param.field4, &pAclEntry->ruleField[4].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[4].fieldType, param.mask4, &pAclEntry->ruleField[4].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[5].fieldType, param.field5, &pAclEntry->ruleField[5].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[5].fieldType, param.mask5, &pAclEntry->ruleField[5].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[6].fieldType, param.field6, &pAclEntry->ruleField[6].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[6].fieldType, param.mask6, &pAclEntry->ruleField[6].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[7].fieldType, param.field7, &pAclEntry->ruleField[7].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[7].fieldType, param.mask7, &pAclEntry->ruleField[7].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[8].fieldType, param.field8, &pAclEntry->ruleField[8].data) != RT_ERR_OK)
        return RT_ERR_FAILED;
    if (_rtl8307h_inAclRule_asic2sw(pAclEntry->ruleField[8].fieldType, param.mask8, &pAclEntry->ruleField[8].mask) != RT_ERR_OK)
        return RT_ERR_FAILED;
    
    return RT_ERR_OK;
}

int32 rtl8307h_acl_ruleEntry_del(uint32 index)
{
    rtl8307h_tblasic_acl_entry_t entry;
    rtl8307h_aclRule_param_t param;
    uint32 retvalue;

    if (index < 0 || index >= RTL8307H_PIE_ENTRYNUM)
    {
        return RT_ERR_INPUT;
    }

    param.valid = 0;

     _rtl8307h_aclRule_param2entry(&entry, &param);

    retvalue = table_write(RTL8307H_UNIT, PIE_TABLE, index, (uint32 *)entry.val);

    return retvalue;
}

static int32  _rtl8307h_policerTbl_param2entry(rtl8307h_tblasic_policer_entry_t *pEntry,  rtl8307h_policerTbl_param_t* pParam)
{
    memset(pEntry, 0, sizeof(rtl8307h_tblasic_policer_entry_t));
    
    pParam->type &= 0x1;
    pParam->tokencnt &= 0x3FFFFUL;
    pParam->threshold &= 0xFF;
    pParam->rate &= 0x1FFFFUL;
 
    pEntry->val[0] = ((uint32)pParam->type << 31)       |           \
                        ((uint32)pParam->tokencnt << 8)      |           \
                        ((uint32)pParam->threshold << 0);

    pEntry->val[1] = ((uint32)pParam->rate << 0);
        
    return RT_ERR_OK;
}

static int32 _rtl8307h_policerTbl_entry2Param(const rtl8307h_tblasic_policer_entry_t *pEntry, rtl8307h_policerTbl_param_t *pParam)
{
    memset(pParam, 0, sizeof(rtl8307h_policerTbl_param_t)); 

    pParam->type = (uint8)((pEntry->val[0] >> 31) & 0x1);
    pParam->tokencnt = (uint32)((pEntry->val[0] >> 8) & 0x3FFFF);
    pParam->threshold = (uint8)((pEntry->val[0] >> 0) & 0xFF); 
    pParam->rate = (uint32)((pEntry->val[1] >> 0) & 0x1FFFF); 
    
    return RT_ERR_OK;
}

int32 rtl8307h_acl_policerTblEntry_set(uint32 index, rtl8307h_policerTbl_param_t* pParam)
{
    rtl8307h_tblasic_policer_entry_t entry;
    uint32 retvalue;

    if ((index < 0) || (index >= RTL8307H_ACLACT_ENTRYNUM ) || (NULL == pParam))
    {
        return RT_ERR_INPUT;
    }
    
    if(pParam->type != 1)
    {
         return RT_ERR_INPUT;
    }

     _rtl8307h_policerTbl_param2entry(&entry, pParam);
    retvalue = table_write(RTL8307H_UNIT, POLICER, index, (uint32 *)entry.val);
    
    return retvalue;   
}


int32 rtl8307h_acl_policerTblEntry_get(uint32 index, rtl8307h_policerTbl_param_t *pParam)
{
    rtl8307h_tblasic_policer_entry_t entry; 

    if ((index < 0) || (index >= RTL8307H_ACLACT_ENTRYNUM ) || (NULL == pParam))
    {
        return RT_ERR_INPUT;
    }
    
     CHK_FUN_RETVAL(table_read(RTL8307H_UNIT, POLICER, index, (uint32 *)entry.val));

    _rtl8307h_policerTbl_entry2Param(&entry, pParam);

    return RT_ERR_OK;
}

static int32  _rtl8307h_counterTbl_param2entry(rtl8307h_tblasic_counter_entry_t *pEntry, rtl8307h_counterTbl_param_t* pParam)
{
    memset(pEntry, 0, sizeof(rtl8307h_tblasic_counter_entry_t));
    
    pParam->type &= 0x1;
    pParam->cntMode &= 0x1;
    pParam->counterH &= 0x3FF;
    pParam->counterL &= 0xFFFFFFFFUL;
 
    pEntry->val[0] = ((uint32)pParam->type << 31)           |           \
                        ((uint32)pParam->cntMode << 10)     |           \
                        ((uint32)pParam->counterH << 0);

    pEntry->val[1] = ((uint32)pParam->counterL << 0);
        
    return RT_ERR_OK;
}

static int32 _rtl8307h_counterTbl_entry2Param(const rtl8307h_tblasic_counter_entry_t *pEntry, rtl8307h_counterTbl_param_t *pParam)
{
    memset(pParam, 0, sizeof(rtl8307h_counterTbl_param_t)); 

    pParam->type = (uint8)((pEntry->val[0] >> 31) & 0x1);
    pParam->cntMode = (uint8)((pEntry->val[0] >> 10) & 0x1);
    pParam->counterH = (uint16)((pEntry->val[0] >> 0) & 0x3FF); 
    pParam->counterL = (uint32)(pEntry->val[1] >> 0); 
    
    return RT_ERR_OK;
}

int32 rtl8307h_acl_counterTblEntry_set(uint32 index, rtl8307h_counterTbl_param_t* pParam)
{
    rtl8307h_tblasic_counter_entry_t entry;
    uint32 retvalue;

    if ((index < 0) || (index >= RTL8307H_ACLACT_ENTRYNUM ) || (NULL == pParam))
    {
        return RT_ERR_INPUT;
    }
    
    if(pParam->type != 0)
    {
         return RT_ERR_INPUT;
    }

     _rtl8307h_counterTbl_param2entry(&entry, pParam);
    retvalue = table_write(RTL8307H_UNIT, ACL_COUNTER, index, (uint32 *)entry.val);
    
    return retvalue;   
}


int32 rtl8307h_acl_counterTblEntry_get(uint32 index, rtl8307h_counterTbl_param_t* pParam)
{
    rtl8307h_tblasic_counter_entry_t entry; 
//    uint32 retvalue;

    if ((index < 0) || (index >= RTL8307H_ACLACT_ENTRYNUM ) || (NULL == pParam))
    {
        return RT_ERR_INPUT;
    }

     CHK_FUN_RETVAL(table_read(RTL8307H_UNIT, ACL_COUNTER, index, (uint32 *)entry.val));

    _rtl8307h_counterTbl_entry2Param(&entry, pParam);

    return RT_ERR_OK;
}

