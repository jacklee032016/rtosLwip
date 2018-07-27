#ifndef _RTL8307H_ASICDRV_ACL_H_
#define _RTL8307H_ASICDRV_ACL_H_

#include <rtk_types.h>
#include <rtk_api.h>

#define RTL8307H_ACLFIELD_MAX   9
#define RTL8307H_PIEPHY_BLKNUM          2
#define RTL8307H_PIEPHY_BLKENTRY        32
#define RTL8307H_PIELOGIC_BLKNUM        4
#define RTL8307H_PIELOGIC_BLKENTRY      16
#define RTL8307H_ACLACT_ENTRYNUM        (RTL8307H_PIEPHY_BLKNUM * RTL8307H_PIEPHY_BLKENTRY)
#define RTL8307H_PIE_ENTRYNUM           (RTL8307H_PIEPHY_BLKNUM * RTL8307H_PIEPHY_BLKENTRY)
#define RTL8307H_ACLRULEID_AUTO     0xFF
#define RTL8307H_ACLACTINFO_LENGTH         0x3

#define RTL8307H_L4PORTRANGETBLENTRY_NUM 4
#define RTL8307H_VIDRANGETBLENTRY_NUM   8
#define RTL8307H_IPRANGETBLENTRY_NUM    8
#define RTL8307H_SRCPORTMASKTBLENTRY_NUM 4
#define RTL8307H_PKTLENTBLENTRY_NUM     4

typedef struct rtl8307h_asicPieTempl_fmt_s
{
#ifndef _LITTLE_ENDIAN
    uint16 swip : 1;
    uint16 noneZeroOff : 1;
    uint16 recvPort    : 4;
    uint16 tgL2Fmt     : 2;
    uint16 itagExist   : 1;
    uint16 otagExist   : 1;
    uint16 tgL23Fmt    : 2;
    uint16 tgL4Fmt     : 3;
    uint16 ispppoe     : 1;
#else
    uint16 ispppoe     : 1;
    uint16 tgL4Fmt     : 3;
    uint16 tgL23Fmt    : 2;
    uint16 otagExist   : 1;
    uint16 itagExist   : 1;
    uint16 tgL2Fmt     : 2;
    uint16 recvPort    : 5;
    uint16 noneZeroOff : 1;
    uint16 swip : 1;
#endif
}rtl8307h_asicPieTempl_fmt_t;


typedef struct rtl8307h_asicPieTempl_otag_s
{
#ifndef _LITTLE_ENDIAN
    uint16 opri : 3;
    uint16 dei    : 1;
    uint16 ovid     : 12;
#else
    uint16 ovid     : 12;
    uint16 dei    : 1;
    uint16 opri : 3;
#endif
}rtl8307h_asicPieTempl_otag_t;


typedef struct rtl8307h_asicPieTempl_itag_s
{
#ifndef _LITTLE_ENDIAN
    uint16 ipri : 3;
    uint16 itagExist    : 1;
    uint16 ivid     : 12;
#else
    uint16 ivid     : 12;
    uint16 itagExist    : 1;
    uint16 ipri : 3;
#endif
}rtl8307h_asicPieTempl_itag_t;


typedef struct rtl8307h_asicPieTempl_prio_s
{
#ifndef _LITTLE_ENDIAN
    uint16 itagPri  : 3;
    uint16 itagExist : 1;
    uint16 otagPri : 3;
    uint16 dei : 1;
    uint16 otagExist : 1;
    uint16 fwdPri   : 3;
    uint16 rsved : 3;
    uint16 cfi    : 1;    
#else
    uint16 cfi    : 1;    
    uint16 rsved : 3;
    uint16 fwdPri    : 3;
    uint16 otagExist : 1;
    uint16 dei : 1;
    uint16 otagPri : 3;
    uint16 itagExist : 1;
    uint16 itagPri  : 3;
#endif
}rtl8307h_asicPieTempl_prio_t;


typedef struct rtl8307h_asicPieTempl_ipHdr_s
{
#ifndef _LITTLE_ENDIAN
    uint16 dscp  : 6;
    uint16 morefrag : 1;
    uint16 noneZeroOff : 1;
    uint16 l4Proto : 8;
#else
    uint16 l4Proto : 8;
    uint16 noneZeroOff : 1;
    uint16 morefrag : 1;
    uint16 dscp  : 6;
#endif
}rtl8307h_asicPieTempl_ipHdr_t;


typedef struct rtl8307h_asicPieTempl_grpIp1_s
{
#ifndef _LITTLE_ENDIAN
        uint16 reserv  : 4;
        uint16 grpIp28_16 : 12;
#else
        uint16 grpIp28_16 : 12;
        uint16 reserv  : 4;
#endif
}rtl8307h_asicPieTempl_grpIp1_t;


typedef struct rtl8307h_asicPieTempl_l4Hdr_s
{
#ifndef _LITTLE_ENDIAN
        uint16 tcpFlags  : 6;
        uint16 reserv    : 2;
        uint16 igmpType  : 8;
#else
        uint16 igmpType  : 8;
        uint16 reserv    : 2;
        uint16 tcpFlags  : 6;
#endif
}rtl8307h_asicPieTempl_l4Hdr_t;


typedef struct rtl8307h_asicPieTempl_icmpHdr_s
{
#ifndef _LITTLE_ENDIAN
        uint16 icmpCode  : 8;
        uint16 icmpType  : 8;
#else
        uint16 icmpType  : 8;
        uint16 icmpCode  : 8;
#endif
}rtl8307h_asicPieTempl_icmpHdr_t;


typedef struct rtl8307h_asicPieTempl_rangeTbl0_s
{
#ifndef _LITTLE_ENDIAN
    uint16 op  : 4;
    uint16 vid  : 8;
    uint16 len  : 4;
#else
    uint16 len  : 4;
    uint16 vid  : 8;
    uint16 op  : 4;
#endif
}rtl8307h_asicPieTempl_rangeTbl0_t;


typedef struct rtl8307h_asicPieTempl_rangeTbl1_s
{
#ifndef _LITTLE_ENDIAN
    uint16 l4Port  : 4;
    uint16 rcvPort  : 4;
    uint16 ipRange  : 8;
#else
    uint16 ipRange  : 8;
    uint16 rcvPort  : 4;
    uint16 l4Port  : 4;
#endif
}rtl8307h_asicPieTempl_rangeTbl1_t;

typedef struct rtl8307h_asicPieTempl_rcvPortMask1_s
{
#ifndef _LITTLE_ENDIAN
    uint16 rcvPortMask  : 8;
    uint16 dmacType  : 2;
    uint16 doNotFrag  : 1;
    uint16 ttlType  : 2;
    uint16 rtkpp  : 1;
    uint16 reserv  : 2;
#else
    uint16 reserv  : 2;
    uint16 rtkpp  : 1;
    uint16 ttlType  : 2;
    uint16 doNotFrag  : 1;
    uint16 dmacType  : 2;
    uint16 rcvPortMask  : 8;
#endif
}rtl8307h_asicPieTempl_rcvPortMask1_t;


typedef struct rtl8307h_asicPieTempl_valid_s
{
#ifndef _LITTLE_ENDIAN
        uint16 reserved  : 15;
        uint16 valid  : 1;
#else
        uint16 valid  : 1;
        uint16 reserved  : 15;
#endif
}rtl8307h_asicPieTempl_valid_t;

typedef struct rtl8307h_asicPieTempl_field_s
{
    union {
        rtl8307h_asicPieTempl_fmt_t format;
        uint16                      dmac0;
        uint16                      dmac1;
        uint16                      dmac2;
        uint16                      smac0;
        uint16                      smac1;
        uint16                      smac2;
        uint16                      ethType;
        rtl8307h_asicPieTempl_otag_t otag;
        rtl8307h_asicPieTempl_itag_t itag;
        rtl8307h_asicPieTempl_prio_t prio;
        uint16                      l2Proto;
        uint16                       sip15_0;
        uint16                       sip31_16;
        uint16                       dip15_0;
        uint16                       dip31_16;
        rtl8307h_asicPieTempl_ipHdr_t ipHdr;
        uint16                       grpIp15_0;        
        rtl8307h_asicPieTempl_grpIp1_t grpIph;
        uint16                        l4SrcPort;
        uint16                        l4DstPort;      
        rtl8307h_asicPieTempl_l4Hdr_t l4Hdr;
        rtl8307h_asicPieTempl_icmpHdr_t icmpHdr;
        rtl8307h_asicPieTempl_rcvPortMask1_t rcvSrcPm1;
        rtl8307h_asicPieTempl_rangeTbl0_t rangeTbl0;
        rtl8307h_asicPieTempl_rangeTbl1_t rangeTbl1;
        uint16                        payload;               
        rtl8307h_asicPieTempl_valid_t valid;
    }un;
}rtl8307h_asicPieTempl_field_t;


typedef struct rtl8307h_aclRule_param_s
{
    uint16 field0;
    uint16 field1;

    uint16 field2;
    uint16 field3;

    uint16 field4;
    uint16 field5;

    uint16 field6;
    uint16 field7;
    
    uint16 field8;
    uint8 valid;

    uint16 mask0;
    uint16 mask1;

    uint16 mask2;
    uint16 mask3;

    uint16 mask4;
    uint16 mask5;

    uint16 mask6;
    uint16 mask7;
    
    uint16 mask8;
}rtl8307h_aclRule_param_t;

typedef struct rtl8307h_tblasic_acl_entry_s
{
    uint32 val[10];
}rtl8307h_tblasic_acl_entry_t;

typedef struct rtl8307h_tblasic_act_entry_s
{
    uint32 val[4];
}rtl8307h_tblasic_act_entry_t;

typedef struct rtl8307h_aclAct_otagopParam_s
{
    uint8 withdraw;

    uint8 outVidCtl; /*   0: reserved, 
                   1: provide new outer VID as OutVIDInfo. 
                   2: packet's new outer VID = packet's outer tag VID - OutVIDInfo
                   3: packet's new outer VID = packet's outer tag VID + OutVIDInfo
                 */
    uint16 outVidInfo;
    uint8 outTagOp; /*0: packet sent without outer VLAN tag, 1: packet sent with outer VLAN tag
                      *2: keep outer VLAN tag and outer VID, 3: Nop */
} rtl8307h_aclAct_otagopParam_t;

typedef struct  rtl8307h_aclAct_itagopParam_s
{
    uint8 withdraw;
    uint8 inVidCtl;
    uint16 inVidInfo;
    uint8 inTagOp;
} rtl8307h_aclAct_itagopParam_t;

typedef struct rtl8307h_aclAct_redirectParam_s
{
    uint8 withdraw; /*1: withdraw */
    uint8 opcode;   /*1: trap 0:normal forwarding  */
    uint8 crsvlan;  /*1: cross vlan 0: forward in VLAN*/
    uint8 dpm;  /*destination port mask */
} rtl8307h_aclAct_redirectParam_t;


typedef struct rtl8307h_aclAct_prioRmkParam_s
{
    uint8 withdraw;
    uint8 tagSel;       /*xx1:inner priority, x1x:outer priority, 1xx:dei */
    uint8 inPri;        /*1: remark inner priority */
    uint8 outPri;       /*1: remark outer priority */
    uint8 dei;      /*DEI field of outer VLAN tag, used only when rmkDei is 1. */
} rtl8307h_aclAct_prioRmkParam_t;


typedef struct rtl8307h_aclAct_dscpRmkParam_s
{
    uint8 withdraw;
    uint8 opcode;       /*0: DSCP remarking, 1: modify IP precedence, 2: modify DTR */
    union
    {
    struct
    {
        uint8 acldscp;
    } dscp;
    struct
    {
        uint8 ipPrece;
    } ipPrec;
    struct
    {
        uint8 dtr;
    } dtr;
    } dscpRmk;
}rtl8307h_aclAct_dscpRmkParam_t;

typedef struct rtl8307h_aclAct_prioParam_s
{
    uint8 withdraw;
    uint8 priority;
} rtl8307h_aclAct_prioParam_t;

typedef struct rtl8307h_aclAct_entry_s
{
    uint8 fno;            /*flow number*/
    uint8 drop;         /*00b:permit; 01b:drop; 10b: withdraw drop; 11: reserved*/
    uint8 copytocpu;
    uint8 mirror; 
    uint8 otag;       /*outer tag operation*/
    uint8 itag;       /*inner tag operation*/
    uint8 priormk;     /*priority/DEI remarking*/
    uint8 redir;    /*redirection*/
    uint8 dscprmk;    /*DSCP remarking*/
    uint8 prioasn;     /*forwarding priority (tx queue) assignment*/
    uint8 byratelmt;    /*bypass ingress rate limit*/
    uint8 actNum;
    
    rtl8307h_aclAct_otagopParam_t outTagOpInfo;
    rtl8307h_aclAct_redirectParam_t redirInfo;
    rtl8307h_aclAct_itagopParam_t inTagOpInfo;
    rtl8307h_aclAct_prioParam_t prioInfo;
    rtl8307h_aclAct_dscpRmkParam_t dscpRmkInfo;
    rtl8307h_aclAct_prioRmkParam_t PriRmkInfo;
}rtl8307h_aclAct_entry_t; 


typedef struct rtl8307h_aclEntry_s
{
    rtk_filter_aclRule_field_t ruleField[RTL8307H_ACLFIELD_MAX];
    rtl8307h_aclAct_entry_t* pAction;
    uint16 valid;
}rtl8307h_aclEntry_t;


typedef struct rtl8307h_tblasic_policer_entry_s
{
    uint32 val[2];
}rtl8307h_tblasic_policer_entry_t;

typedef struct rtl8307h_policerTbl_param_s
{
    uint8 type ;            /*0: counter; 1: policer*/
    uint32 tokencnt;     /*policer token counter*/
    uint8 threshold;
    uint32 rate;;
}rtl8307h_policerTbl_param_t;

typedef struct rtl8307h_tblasic_counter_entry_s
{
    uint32 val[2];
}rtl8307h_tblasic_counter_entry_t;

typedef struct rtl8307h_counterTbl_param_s
{
    uint8 type ;            /* 0: counter; 1: policer */
    uint8 cntMode;       /* 0: packet-based counter; 1: byte-based counter */
    uint16 counterH;           /* MSB 10-bit */
    uint32 counterL;             /* LSB 32-bit */
}rtl8307h_counterTbl_param_t;

extern int32 rtl8307h_acl_actTblEntry_set(uint32 index, rtl8307h_aclAct_entry_t * actEntry_p);

extern int32 rtl8307h_acl_actTblEntry_get(uint32 index, rtl8307h_aclAct_entry_t* actEntry_p);

extern int32 _rtl8307h_inAclRule_sw2asic(rtk_filter_field_type_t fieldType, uint16* asicData, rtk_filter_pieTempl_field_t swData);

extern int32 _rtl8307h_inAclRule_asic2sw(rtk_filter_field_type_t fieldType, uint16 asicData, rtk_filter_pieTempl_field_t* swData);

/*convert raw ASIC data to readable ACL rule entry, should fill pAclEntry->rule[n].fieldType first*/
extern int32 rtl8307h_acl_ruleEntry_get(uint32 index, rtl8307h_aclEntry_t* pAclEntry);

/*convert raw ASIC data to readable ACL rule entry*/
extern int32 rtl8307h_acl_ruleEntry_set(uint32 index, rtl8307h_aclEntry_t* pAclEntry);

extern int32 rtl8307h_acl_ruleEntry_del(uint32 index);


extern int32 rtl8307h_acl_policerTblEntry_set(uint32 index, rtl8307h_policerTbl_param_t* pParam);

extern int32 rtl8307h_acl_policerTblEntry_get(uint32 index, rtl8307h_policerTbl_param_t *pParam);

extern int32 rtl8307h_acl_counterTblEntry_set(uint32 index, rtl8307h_counterTbl_param_t* pParam);

extern int32 rtl8307h_acl_counterTblEntry_get(uint32 index, rtl8307h_counterTbl_param_t* pParam);

#endif /*_RTL8307H_ASICDRV_ACL_H_*/

