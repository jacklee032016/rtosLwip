/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition function prototype of RTK API.
 *
 * Feature : Function prototype definition
 *
 */

#ifndef __RTK_API_H__
#define __RTK_API_H__

/*
 * Include Files
 */
#include <rtk_types.h>
#include <rtk_error.h>

/*
 * Data Type Declaration
 */
#define ENABLE                                      1
#define DISABLE                                     0

#define PHY_CONTROL_REG                             0
#define PHY_STATUS_REG                              1
#define PHY_AN_ADVERTISEMENT_REG                    4
#define PHY_AN_LINKPARTNER_REG                      5
#define PHY_1000_BASET_CONTROL_REG                  9
#define PHY_1000_BASET_STATUS_REG                   10
#define PHY_RESOLVED_REG                            17
#define PHY_POWERSAVING_REG                         21
#define PHY_POWERSAVING_OFFSET                      12
#define PHY_POWERSAVING_MASK                        0x1000

#define RTK_MAX_NUM_OF_PORT                         8
#define RTK_PORT_ID_MAX                             (RTK_MAX_NUM_OF_PORT-1)
#define RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST    1
#define RTK_MAX_NUM_OF_PROTO_TYPE                   0xFFFF
#define RTK_MAX_PORT_MASK                           0xFF

#define RTK_PRIORITY_MAX                     7

#define RTK_ACL_BLOCK_MAX                           4
#define RTK_ACL_BLOCKRULE_MAX                  16
#define RTK_ACL_RULE_MAX                            (RTK_ACL_BLOCK_MAX * RTK_ACL_BLOCKRULE_MAX)
#define RTK_ACL_TEMPLFIELD_MAX                      9

#define FILTER_POLICING_MAX                        RTK_ACL_RULE_MAX

/*range checking tables*/
#define FILTER_L4PORTRANGE_MAX                   4
#define FILTER_VIDRANGE_MAX                         8
#define FILTER_IPRANGE_MAX                          8
#define FILTER_SRCPORTMASK_MAX                4
#define FILTER_PKTLENRANGE_MAX                4

/* wol event */
#define WOL_EVENT_LINKUP        0x1
#define WOL_EVENT_MAGIC_PKT     0x2
#define WOL_EVENT_WAKEUP_PKT    0x4
#define WOL_EVENT_ANY_PKT_PORT5 0x8   /* Valid only when port 5 is configured as cpu port */
#define WOL_EVENT_ANY_PKT_PORT6 0x10  /* Valid only when port 6 is configured as cpu port */


#ifndef MAC_ADDR_LEN
#define MAC_ADDR_LEN                                6
#endif

#define IPV6_ADDR_LEN                               16
#define IPV4_ADDR_LEN                               4

#define ALLPORT 0xFF

typedef uint32  rtk_dscp_t;         /* dscp vlaue */

typedef enum rtk_enable_e
{
    DISABLED = 0,
    ENABLED,
    RTK_ENABLE_END
} rtk_enable_t;

typedef uint32  rtk_fid_t;        /* filter id type */

typedef uint32  rtk_grp_time_t;       /*group member aging time type*/

typedef uint32  rtk_last_member_time_t;        /*last member aging time type*/

/* ethernet address type */
typedef struct  rtk_mac_s
{
    uint8 octet[ETHER_ADDR_LEN];
} rtk_mac_t;

/*type of template field*/
typedef enum rtk_filter_field_type_e 
{
    FILTER_FIELD_FMT = 0,
    FILTER_FIELD_DMAC0,
    FILTER_FIELD_DMAC1,
    FILTER_FIELD_DMAC2,
    FILTER_FIELD_SMAC0,
    FILTER_FIELD_SMAC1,
    FILTER_FIELD_SMAC2,
    FILTER_FIELD_ETHERTYPE,
    FILTER_FIELD_OTAG,
    FILTER_FIELD_ITAG,
    FILTER_FIELD_PRIORITY,
    FILTER_FIELD_L2PROTOCOL,
    FILTER_FIELD_SIP0,
    FILTER_FIELD_SIP1,
    FILTER_FIELD_DIP0,
    FILTER_FIELD_DIP1,
    FILTER_FIELD_IPHDR,
    FILTER_FIELD_GIP0,
    FILTER_FIELD_GIP1,
    FILTER_FIELD_L4SPORT,
    FILTER_FIELD_L4DPORT,
    FILTER_FIELD_L4HDR,
    FILTER_FIELD_ICMPCODETYPE,
    FILTER_FIELD_SPM1,
    FILTER_FIELD_RANGE0,
    FILTER_FIELD_RANGE1,
    FILTER_FIELD_PAYLOAD,
    FILTER_FIELD_UNUSED,
    FILTER_FIELD_END,
}rtk_filter_field_type_t;

typedef uint8 rtk_filter_templField_id_t;    /* template field ID*/
typedef uint8  rtk_aclBlock_id_t;        /* ACL block ID type */
typedef uint8  rtk_aclRule_id_t;        /* rule ID type */


typedef struct rtk_filter_pieTempl_grpIp1_s
{
    uint16 reserv  : 4;
    uint16 grpIp28_16 : 12;
}rtk_filter_pieTempl_grpIp1_t;

typedef struct rtk_filter_pieTempl_l4Hdr_s
{
    uint16 tcpFlags  : 6;
    uint16 reserv    : 2;
    uint16 igmpType  : 8;
}rtk_filter_pieTempl_l4Hdr_t;

typedef struct rtk_filter_pieTempl_otag_s
{
    uint16 opri : 3;
    uint16 dei    : 1;
    uint16 ovid     : 12;
}rtk_filter_pieTempl_otag_t;

typedef struct rtk_filter_pieTempl_fmt_s
{
    uint16 swip : 1;
    uint16 noneZeroOff : 1;
    uint16 recvPort    : 4;
    uint16 tgL2Fmt     : 2;
    uint16 itagExist   : 1;
    uint16 otagExist   : 1;
    uint16 tgL23Fmt    : 2;
    uint16 tgL4Fmt     : 3;
    uint16 ispppoe     : 1;
}rtk_filter_pieTempl_fmt_t;

typedef struct rtk_filter_pieTempl_itag_s
{
    uint16 ipri : 3;
    uint16 itagExist    : 1;
    uint16 ivid     : 12;
}rtk_filter_pieTempl_itag_t;

typedef struct rtk_filter_pieTempl_prio_s
{
    uint16 itagPri  : 3;
    uint16 itagExist : 1;
    uint16 otagPri : 3;
    uint16 dei : 1;
    uint16 otagExist : 1;
    uint16 fwdPri    : 3;
    uint16 rsved : 3;
    uint16 cfi    : 1;    

}rtk_filter_pieTempl_prio_t;

typedef struct rtk_filter_pieTempl_ipHdr_s
{
    uint16 dscp  : 6;
    uint16 morefrag : 1;
    uint16 noneZeroOff : 1;
    uint16 l4Proto : 8;
}rtk_filter_pieTempl_ipHdr_t;

typedef struct rtk_filter_pieTempl_icmpHdr_s
{
    uint16 icmpCode  : 8;
    uint16 icmpType  : 8;
}rtk_filter_pieTempl_icmpHdr_t;

typedef struct rtk_filter_pieTempl_rcvPortMask1_s
{
    uint16 rcvPortMask  : 8;
    uint16 dmacType  : 2;
    uint16 doNotFrag  : 1;
    uint16 ttlType  : 2;
    uint16 rtkpp  : 1;
    uint16 rsved : 2;
}rtk_filter_pieTempl_rcvPortMask1_t;

typedef struct rtk_filter_pieTempl_rangeTbl0_s
{
    uint16 op  : 4;
    uint16 vid  : 8;
    uint16 len  : 4;
}rtk_filter_pieTempl_rangeTbl0_t;

typedef struct rtk_filter_pieTempl_rangeTbl1_s
{
    uint16 l4Port  : 4;
    uint16 rcvPort  : 4;
    uint16 ipRange  : 8;
}rtk_filter_pieTempl_rangeTbl1_t;

typedef struct rtk_filter_pieTempl_field_s
{
    union {
        rtk_filter_pieTempl_fmt_t format;
        uint16                      dmac0;
        uint16                      dmac1;
        uint16                      dmac2;
        uint16                      smac0;
        uint16                      smac1;
        uint16                      smac2;
        uint16                      ethType;
        rtk_filter_pieTempl_otag_t otag;
        rtk_filter_pieTempl_itag_t itag;
        rtk_filter_pieTempl_prio_t prio;
        uint16                      l2Proto;
        uint16                       sip15_0;
        uint16                       sip31_16;
        uint16                       dip15_0;
        uint16                       dip31_16;
        rtk_filter_pieTempl_ipHdr_t ipHdr;
        uint16                       grpIp15_0;        
        rtk_filter_pieTempl_grpIp1_t grpIph;
        uint16                        l4SrcPort;
        uint16                        l4DstPort;      
        rtk_filter_pieTempl_l4Hdr_t l4Hdr;
        rtk_filter_pieTempl_icmpHdr_t icmpHdr;
        rtk_filter_pieTempl_rcvPortMask1_t rcvSrcPm1;
        rtk_filter_pieTempl_rangeTbl0_t rangeTbl0;
        rtk_filter_pieTempl_rangeTbl1_t rangeTbl1;
        uint16                        payload;               
    }un;
}rtk_filter_pieTempl_field_t;


typedef struct rtk_filter_aclField_s
{
    rtk_filter_field_type_t type;
    rtk_filter_pieTempl_field_t fieldData;
    rtk_filter_pieTempl_field_t fieldMask;
    struct rtk_filter_aclField_s* next;
}rtk_filter_aclField_t;

typedef struct rtk_filter_aclRule_field_s
{
    rtk_filter_field_type_t fieldType;
    rtk_filter_pieTempl_field_t data;
    rtk_filter_pieTempl_field_t mask;
}rtk_filter_aclRule_field_t;


typedef struct rtk_filter_aclEntry_s
{   
    rtk_aclBlock_id_t blockId;  
    rtk_aclRule_id_t ruleId;  
    uint8 reverse;                  /*result reversed*/
    rtk_filter_aclField_t* pField;
    uint32* pAction; /* ACL action*/
}rtk_filter_aclEntry_t;

typedef enum rtk_filter_actionType_e
{
    ACLACTTYPE_FNO = 0,      /*flow number*/
    ACLACTTYPE_DROP,     
    ACLACTTYPE_CP2CPU,   /*copy to CPU*/
    ACLACTTYPE_MIRROR,   /*mirror*/
    ACLACTTYPE_OTAG,     /*outer tag*/
    ACLACTTYPE_ITAG,     /*inner tag*/
    ACLACTTYPE_PRIRMK,   /*priority remarking*/
    ACLACTTYPE_REDIR,    /*redirection*/
    ACLACTTYPE_DSCPRMK,  /*DSCP/TOS remarking*/
    ACLACTTYPE_PRIORITY, /*priority assignment*/
    ACLACTTYPE_BYRATELMT, /*bypass rate limitt*/
    ACLACTTYPE_END,
}rtk_filter_actionType_t;


typedef uint8 rtk_filter_aclAct_drop_t;/*1:drop, 2:withdraw drop*/
typedef uint8 rtk_filter_aclAct_fno_t;
typedef uint8 rtk_filter_aclAct_cp2cpu_t;
typedef uint8 rtk_filter_aclAct_mirror_t;
typedef uint8 rtk_filter_aclAct_byratelmt_t;

typedef struct rtk_filter_aclAct_itagop_s
{
    uint32 withdraw:1;
    uint32 inVidCtl:2;
    uint32 inVidInfo:12;
    uint32 inTagOp:2;
    uint32 reserved:15;
} rtk_filter_aclAct_itagop_t;


typedef struct rtk_filter_aclAct_redirect_s
{
    uint32 withdraw:1;  /*1: withdraw */
    uint32 opcode:1;	/*1: trap 0:normal forwarding  */
    uint32 crsvlan:1;	/*1: cross vlan 0: forward in VLAN*/
    uint32 dpm:8;	/*destination port mask */
    uint32 reserved:21;
} rtk_filter_aclAct_redirect_t;

typedef struct rtk_filter_aclAct_prioRmk_s
{
    uint32 withdraw:1;
    uint32 tagSel:3;      /*xx1:inner priority, x1x:outer priority, 1xx:dei */
    uint32 inPri:3;         /*1: remark inner priority */
    uint32 outPri:3;       /*1: remark outer priority */
    uint32 dei:1;           /*DEI field of outer VLAN tag, used only when rmkDei is 1. */
    uint32 reserved:21;
} rtk_filter_aclAct_prioRmk_t;


typedef struct rtk_filter_aclAct_otagop_s
{
    uint32 withdraw:1;

    /*   0: reserved, 
      *   1: provide new outer VID as OutVIDInfo. 
      *   2: packet's new outer VID = packet's outer tag VID - OutVIDInfo
      *   3: packet's new outer VID = packet's outer tag VID + OutVIDInfo
      */
    uint32 outVidCtl:2; 
    uint32 outVidInfo:12;
    /*  0: packet sent without outer VLAN tag, 1: packet sent with outer VLAN tag
      *  2: keep outer VLAN tag and outer VID,  3: Nop */
    uint32 outTagOp:2; 
    uint32 reserved:15;
} rtk_filter_aclAct_otagop_t;

typedef struct rtk_filter_aclAct_dscpRmk_s
{
    uint32 withdraw;
    uint32 opcode:2;
    /*0: DSCP remarking, 1: modify IP precedence, 2: modify DTR */
    union
    {
        struct
        {
            uint32 acldscp:6;
        } dscp;
        struct
        {
            uint32 ipPrece:3;
        } ipPrec;
        struct
        {
            uint32 dtr:3;
        } dtr;
    } dscpRmk;

} rtk_filter_aclAct_dscpRmk_t;;

typedef struct rtk_filter_aclAct_priority_s
{
    uint32 withdraw: 1;
    uint32 priority: 3;
    uint32 reserved: 28;
} rtk_filter_aclAct_priority_t;

typedef struct rtk_filter_aclAction_s
{    
    union{
        rtk_filter_aclAct_fno_t fno;
        rtk_filter_aclAct_drop_t dropInfo;
        rtk_filter_aclAct_cp2cpu_t cp2cpu;
        rtk_filter_aclAct_mirror_t mirror;
        rtk_filter_aclAct_otagop_t outTagOpInfo;        
        rtk_filter_aclAct_itagop_t inTagOpInfo;
        rtk_filter_aclAct_prioRmk_t priRmkInfo;
        rtk_filter_aclAct_redirect_t redirInfo;
        rtk_filter_aclAct_dscpRmk_t dscpRmkInfo;
        rtk_filter_aclAct_priority_t prioInfo;
        rtk_filter_aclAct_byratelmt_t byRateLmt;
    }un;
}rtk_filter_aclAction_t;


typedef struct rtk_filter_policer_s
{
    uint32 rate;        /*in the unit of 16Kbps*/
    uint8 threshold;    /*in the unit of kB*/
} rtk_filter_policer_t;

/*type of template field*/
typedef enum rtk_filter_cnt_type_e 
{
    FILTERCNT_PACKET = 0,
    FILTERCNT_BYTE,
    FILTERCNT_END,
}rtk_filter_cnt_type_t;

typedef struct rtk_filter_counter_s
{
    uint32 counterH;
    uint32 counterL; 
    rtk_filter_cnt_type_t cntType;
} rtk_filter_counter_t;


typedef enum rtk_range_l4portType_e
{
    L4PORT_RANGE_INVALID = 0,                 /*invalid entry*/
    L4PORT_RANGE_TCP_SOURCE_PORT = 1,      /*TCP source port*/
    L4PORT_RANGE_UDP_SOURCE_PORT = 2,   /*UDP source port*/
    L4PORT_RANGE_TCP_OR_UDP_SOURCE_PORT = 3 ,    /*TCP or UDP source port*/
    L4PORT_RANGE_TCP_DEST_PORT = 5,          /*TCP destination port*/
    L4PORT_RANGE_UDP_DEST_PORT = 6,        /*UDP destination port*/
    L4PORT_RANGE_TCP_OR_UDP_DEST_PORT = 7 ,     /*TCP or UDP destination port*/
    L4PORT_RANGE_END,
}rtk_range_l4portType_t;

typedef enum rtk_range_vidType_e
{
    VID_RANGE_INVALID = 0,      /*invalid entry*/
    VID_RANGE_IVID_ONLY,       /*compare inner VID only*/
    VID_RANGE_OVID_ONLY,       /*compare outer VID only*/
    VID_RANGE_IVID_OR_OVID,  /* compare either inner VID or outer VID*/
    VID_RANGE_END,
}rtk_range_vidType_t;

typedef enum rtk_range_ipType_e
{
    IP_RANGE_INVALID = 0,      /*invalid entry*/
    IP_RANGE_SOURCE_IP,       /*compare IPv4 source IP*/
    IP_RANGE_DEST_IP,           /*compare IPv4 destination IPnly*/
    IP_RANGE_SOURCE_OR_DEST_IP,  /*either source IP or destination IP */
    IP_RANGE_END,
}rtk_range_ipType_t;


typedef uint32 rtk_filter_port_t;
typedef uint32 rtk_filter_index_t;
typedef uint16 rtk_filter_bound_t;
typedef uint32 rtk_filter_unmatch_action_t;

typedef enum rtk_filter_unmatch_action_e
{
    FILTER_UNMATCH_DROP = 0,
    FILTER_UNMATCH_PERMIT,
    FILTER_UNMATCH_END,
} rtk_filter_unmatch_action_type_t;

typedef enum rtk_leaky_type_e
{
    LEAKY_GRP = 0,
    LEAKY_STATIC_LUT,
    LEAKY_END,
}rtk_leaky_type_t;

typedef enum rtk_attack_type_s
{
    ATTKTYPE_MACLAND,
    ATTKTYPE_IPLAND,
    ATTKTYPE_UDPBLAT,
    ATTKTYPE_TCPBLAT,
    ATTKTYPE_POD,
    ATTKTYPE_ICMPFRAG,
    ATTKTYPE_V4PINGMAX,
    ATTKTYPE_V6PINGMAX,
    ATTKTYPE_SMURF,
    ATTKTYPE_MINITCPHDR,    
    ATTKTYPE_SYNSPORTL1024,
    ATTKTYPE_NULLSCAN,
    ATTKTYPE_XMAS,
    ATTKTYPE_SYNFIN,        
    ATTKTYPE_END
} rtk_attack_type_t;

typedef enum rtk_attkPrev_action_s
{
    ATTKPREV_PERMIT,
    ATTKPREV_DROP,
    ATTKPREV_END
} rtk_attkPrev_action_t;

typedef uint32  rtk_length_t;        /* filter id type */

typedef enum rtk_trap_rma_action_e
{
    RMA_ACTION_FORWARD = 0,
    RMA_ACTION_TRAP2CPU,        
    RMA_ACTION_DROP,
    RMA_ACTION_FORWARD_COPY2_CPU,
    RMA_ACTION_END
} rtk_trap_rma_action_t;

typedef uint32  rtk_stp_msti_id_t;
typedef uint32  rtk_port_t;        /* port is type */

typedef enum rtk_port_linkStatus_e
{
    PORT_LINKDOWN = 0,
    PORT_LINKUP,
    PORT_LINKSTATUS_END
} rtk_port_linkStatus_t;

typedef struct  rtk_port_link_ability_s
{
    uint8 speed;
    uint8 duplex;
    uint8 link;    
    uint8 nway;    
    uint8 txpause;
    uint8 rxpause;     
}rtk_port_link_ability_t;

typedef struct  rtk_port_link_status_s
{
    uint8 speed;
    uint8 duplex;
    uint8 link;    
    uint8 txpause;
    uint8 rxpause;     
}rtk_port_link_status_t;


typedef struct  rtk_port_mac_ability_s
{
    uint32 forcemode;
    uint32 speed;
    uint32 duplex;
    uint32 link;    
    uint32 nway;    
    uint32 txpause;
    uint32 rxpause;     
}rtk_port_mac_ability_t;

typedef struct rtk_port_phy_ability_s
{   
    uint32    AutoNegotiation;  /*PHY register 0.12 setting for auto-negotiation process*/
    uint32    Half_10;              /*PHY register 4.5 setting for 10BASE-TX half duplex capable*/
    uint32    Full_10;              /*PHY register 4.6 setting for 10BASE-TX full duplex capable*/
    uint32    Half_100;           /*PHY register 4.7 setting for 100BASE-TX half duplex capable*/
    uint32    Full_100;            /*PHY register 4.8 setting for 100BASE-TX full duplex capable*/
    uint32    Full_1000;         /*PHY register 9.9 setting for 1000BASE-T full duplex capable*/
    uint32    FC;                   /*PHY register 4.10 setting for flow control capability*/
    uint32    AsyFC;             /*PHY register 4.11 setting for  asymmetric flow control capability*/
} rtk_port_phy_ability_t;

typedef uint16  rtk_port_phy_data_t;     /* phy data */

typedef uint8  rtk_port_phy_page_t;     /* phy page  */

typedef enum rtk_port_phy_reg_e  
{
    PHY_REG_CONTROL             = 0,
    PHY_REG_STATUS,
    PHY_REG_IDENTIFIER_1,
    PHY_REG_IDENTIFIER_2,
    PHY_REG_AN_ADVERTISEMENT,
    PHY_REG_AN_LINKPARTNER,
    PHY_REG_1000_BASET_CONTROL  = 9,
    PHY_REG_1000_BASET_STATUS,
    PHY_REG_END                 = 32
} rtk_port_phy_reg_t;

typedef enum rtk_port_speed_e
{
    PORT_SPEED_10M  = 0x1,
    PORT_SPEED_100M = 0x2,
    PORT_SPEED_1000M= 0x4,
    PORT_SPEED_END
} rtk_port_speed_t;

typedef enum rtk_port_duplex_e
{
    PORT_HALF_DUPLEX = 0x1,
    PORT_FULL_DUPLEX = 0x2,
    PORT_DUPLEX_END
} rtk_port_duplex_t;

typedef struct rtk_portmask_s
{
    uint32  bits[RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST];
} rtk_portmask_t;

typedef enum rtk_vlan_acceptFrameType_e
{
    ACCEPT_FRAME_TYPE_ALL = 0,             /* untagged, priority-tagged and tagged */
    ACCEPT_FRAME_TYPE_TAG_ONLY,         /* tagged */
    ACCEPT_FRAME_TYPE_UNTAG_ONLY,     /* untagged and priority-tagged */
    ACCEPT_FRAME_TYPE_NONE = 3,     /* accept none packet*/
    ACCEPT_FRAME_TYPE_END
} rtk_vlan_acceptFrameType_t;

typedef uint32  rtk_vlan_t;        /* vlan id type */


/* tagged mode of VLAN - reference realtek private specification */
typedef enum rtk_vlan_tagMode_e
{
    VLAN_TAG_MODE_ORIGINAL = 0,
    VLAN_TAG_MODE_KEEP_FORMAT,
    VLAN_TAG_MODE_PRI,
    VLAN_TAG_MODE_REAL_KEEP_FORMAT,
    VLAN_TAG_MODE_END
} rtk_vlan_tagMode_t;

typedef enum  rtk_vlan_txTagStatus_e
{
    TAG_STATUS_WITH_TAG = 0,        
    TAG_STATUS_WITHOUT_TAG,
    TAG_STATUS_END
}rtk_vlan_txTagStatus_t;

typedef uint32 rtk_ivlsvl_t;

typedef enum rtk_vlan_ivlsvl_e
{
    VLAN_IVL = 0,
    VLAN_SVL,
    VLAN_IVLSVL_END
} rtk_vlan_ivlsvl_t;

typedef uint32 rtk_svlan_tpid_t;
typedef uint16 rtk_vlan_tpid_t;
typedef uint32  rtk_stg_t;        /* spanning tree instance id type */


typedef enum  rtk_phy_heacOpMode_e
{
    OP_MODE_HEC_DISABLED = 0,
    OP_MODE_ONLY_ARC_TX_COMMON,
    OP_MODE_ONLY_ARC_TX_SINGLE,
    OP_MODE_ONLY_ARC_RX,
    OP_MODE_ONLY_HEC,
    OP_MODE_HEC_WITH_ARC_TX_COMMON = 5,
    OP_MODE_HEC_WITH_ARC_RX_COMMON = 7,
    OP_MODE_END
}rtk_phy_heacOpMode_t;


typedef uint32 rtk_stp_bdg_pri_t;
typedef uint32 rtk_stp_port_pri_t;

typedef uint32 rtk_stp_maxAdge_t;
typedef uint32 rtk_stp_helloTime_t;
typedef uint32 rtk_stp_fowardDelay_t;

typedef uint32 rtk_stp_portPathCost_t;


typedef enum rtk_stp_type_e
{
    STP_TYPE_STP = 0,
    STP_TYPE_RSTP,
    STP_TYPE_END
}rtk_stp_type_t;

typedef enum rtk_stp_state_e
{
    STP_DISCARDING = 0,
    STP_BLOCKING,
    STP_LEARNING,
    STP_FORWARDING,
    STP_END
}rtk_stp_state_t;

typedef enum rtk_stp_portEdge_e
{
    STP_EDGE_AUTO = 0,
    STP_EDGE_ADMIN,
    STP_EDGE_END
}rtk_stp_portEdge_t;


typedef enum rtk_stp_portP2P_e
{
    STP_P2P_AUTO = 0,
    STP_P2P_TURE,
    STP_P2P_FALSE,
    STP_P2P_END
}rtk_stp_portP2P_t;


typedef enum rtk_event_notify_e
{
    STP_PARAM_CHANGE_FINISH = 5,    /* Spanning Tree Param change operation finished    */  
}rtk_event_notify_t;

typedef uint64 rtk_stat_counter_t;

/* global statistic counter structure */
typedef struct rtk_stat_global_cntr_s
{
    uint32 dot1dTpLearnedEntryDiscards;
}rtk_stat_global_cntr_t;

typedef enum rtk_stat_global_type_e
{
    DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX = 36,
    MIB_GLOBAL_CNTR_END
}rtk_stat_global_type_t;

/* port statistic counter structure */
typedef struct rtk_stat_port_cntr_s
{
    uint64 ifOutOctets;
    uint64 ifInOctets;
    uint32 ifInUcastPkts;
    uint32 dot3OutPauseFrames;
    uint32 dot3OutPauseOnFrames;
    uint32 dot3StatsExcessiveCollisions;
    uint32 dot3StatsLateCollisions;
    uint32 dot3StatsDeferredTransmissions;
    uint32 dot3StatsMultipleCollisionFrames;
    uint32 dot3StatsSingleCollisionFrames;
    uint32 dot3CtrlInUnknownOpcodes;
    uint32 dot3InPauseFrames;
    uint32 dot3StatsSymbolErrors;
    uint32 dot3StatsFCSErrors;
    uint32 etherStatsJabbers;
    uint32 etherStatsCollisions;
    uint32 etherStatsMcastPkts;
    uint32 etherStatsBcastPkts;
    uint32 etherStatsFragments;
    uint32 etherStatsPkts64Octets;
    uint32 etherStatsPkts65to127Octets;
    uint32 etherStatsPkts128to255Octets;
    uint32 etherStatsPkts256to511Octets;
    uint32 etherStatsPkts512to1023Octets;
    uint32 etherStatsPkts1024toMaxOctets;
    uint32 etherStatsOversizePkts;
    uint64 etherStatsOctets;
    uint32 etherStatsUndersizePkts;
    uint32 igrLackPktBufDrop;
    uint32 flowCtrlOnDropPktCnt;
    uint32 txCrcCheckFailCnt;
    uint32 smartTriggerHit0;
    uint32 smartTriggerHit1;
    uint32 ifOutUcastPkts;
    uint32 ifOutMulticastPkts;
    uint32 ifOutBrocastPkts;
}rtk_stat_port_cntr_t;

/* port statistic counter index */
typedef enum rtk_stat_port_type_e
{
    STAT_IfInOctets = 0,
    STAT_Dot3StatsFCSErrors,
    STAT_Dot3StatsSymbolErrors,
    STAT_Dot3InPauseFrames,
    STAT_Dot3ControlInUnknownOpcodes,        
    STAT_EtherStatsFragments,
    STAT_EtherStatsJabbers,
    STAT_IfInUcastPkts,
    STAT_EtherStatsDropEvents,
    STAT_EtherStatsOctets,
    
    STAT_EtherStatsUnderSizePkts,
    STAT_EtherOversizeStats,
    STAT_EtherStatsPkts64Octets,
    STAT_EtherStatsPkts65to127Octets,
    STAT_EtherStatsPkts128to255Octets,
    STAT_EtherStatsPkts256to511Octets,
    STAT_EtherStatsPkts512to1023Octets,
    STAT_EtherStatsPkts1024to1518Octets,
    STAT_EtherStatsMulticastPkts,
    STAT_EtherStatsBroadcastPkts,  
    
    STAT_IfOutOctets,
    STAT_Dot3StatsSingleCollisionFrames,
    STAT_Dot3StatMultipleCollisionFrames,
    STAT_Dot3SDeferredTransmissions,
    STAT_Dot3StatsLateCollisions,
    STAT_EtherStatsCollisions,
    STAT_Dot3StatsExcessiveCollisions,
    STAT_Dot3OutPauseFrames,
    STAT_Dot1dBasePortDelayExceededDiscards,
    STAT_Dot1dTpPortInDiscards,
    
    STAT_IfOutUcastPkts,
    STAT_IfOutMulticastPkts,
    STAT_IfOutBroadcastPkts,
    STAT_OutOampduPkts,
    STAT_InOampduPkts,
    STAT_PktgenPkts,
    
    STAT_Dot1dTpPortOutFrames,
    STAT_Dot1dTpPortInFrames,       
    STAT_Dot3StatsFrameTooLongs,
    STAT_Dot3StatsAlignmentErrors,       
    STAT_MIB_PORT_CNTR_END
}rtk_stat_port_type_t;

typedef uint32  rtk_pri_t;         /* priority vlaue */
typedef uint32  rtk_dei_t;        /* dei vlaue */

typedef uint32  rtk_index_t;
typedef uint32  rtk_weight_t;

typedef uint32  rtk_queue_num_t;
typedef uint32  rtk_rate_t;

typedef uint32  rtk_ingressLB_t;


#define RTK_MAX_NUM_OF_PRIORITY     8
#define RTK_MAX_NUM_OF_QUEUE    4

typedef struct rtk_priority_select_s
{   uint32 group;       /*RTL8325D has two groups of priority selection*/
    uint32 port_pri;
    uint32 dot1q_pri;
    uint32 dscp_pri;
    uint32 cvlan_pri;
    uint32 svlan_pri;
} rtk_priority_select_t;

typedef struct rtk_qos_pri2queue_s
{
    uint32 pri2queue[RTK_MAX_NUM_OF_PRIORITY];
} rtk_qos_pri2queue_t;

typedef struct rtk_qos_queue_weights_s
{
    uint32 weights[RTK_MAX_NUM_OF_QUEUE];
} rtk_qos_queue_weights_t;

typedef enum rtk_priority_extraction_source_e
{
    QOS_PORT_BASE_PRIORITY = 0,
    QOS_DOT1Q_PORT_BASE_PRIORITY,  
    QOS_DSCP_BASE_PRIORITY,
    QOS_ITAG_PRIORITY,
    QOS_OTAG_PRIORITY,
    QOS_PRIORITY_END
}rtk_priority_extraction_source_t;


typedef enum rtk_rx_priority_copySwap_e
{
    QOS_PRIORITY_FOLLOW_ALE = 0,
    QOS_PRIORITY_INNER_TAG,
    QOS_PRIORITY_OUTER_TAG,
    QOS_PRIORITY_COPYSWAP_END 
}rtk_rx_priority_copySwap_t;

typedef enum rtk_tx_remark_ability_e
{
    QOS_TX_NO_REMARK = 0,
    QOS_TX_REMARK,
    QOS_TX_REMARK_END
}rtk_tx_remark_ability_t;

typedef enum rtk_pktLen_with_preIfg_e
{
    QOS_PREIFG_EXCLUDE = 0,
    QOS_PREIFG_INCLUDE,
    QOS_PREIFG_END
}rtk_pktLen_with_preIfg_t;

typedef enum rtk_queue_scheduleType_e
{
    QOS_WFQ = 0,
    QOS_WRR,
    QOS_CREDIT_BASED,
    QOS_STRICT_PRIORITY,
    QOS_SCHEDULE_END
}rtk_queue_scheduleType_t;

typedef enum rtk_qos_queueFix_e
{
    QOS_QUEUE_NOFIX = 0,
    QOS_QUEUE_FIX,
    QOS_QUEUE_FIX_END
}rtk_qos_queueFix_t;

typedef enum rtk_qos_flowContorolMode_e
{
    QOS_FC_NWAY = 0,
    QOS_FC_FORCE,
    QOS_FC_END
}rtk_qos_flowContorolMode_t;

typedef struct rtk_qos_queueSchedule_e
{
    rtk_queue_scheduleType_t    scheduleType;
    rtk_weight_t    weight;  
}rtk_qos_queueSchedule_t;

typedef struct rtk_ingress_bwCtrl_s
{
    rtk_enable_t bwCtrlEnable;
    rtk_rate_t ingressRate0;
    rtk_enable_t flowCtrlEnable0;
    rtk_rate_t ingressRate1;
    rtk_enable_t flowCtrlEnable1;
}rtk_ingress_bwCtrl_t;

/* unauth pkt action */
typedef enum rtk_dot1x_unauth_action_e
{
    DOT1X_ACTION_DROP = 0,
    DOT1X_ACTION_TRAP2CPU,
    DOT1X_ACTION_GUESTVLAN,
    DOT1X_ACTION_END
} rtk_dot1x_unauth_action_t;

/* Type of port-based dot1x auth/unauth*/
typedef enum rtk_dot1x_auth_status_e
{
    UNAUTH = 0,
    AUTH,
    AUTH_STATUS_END
} rtk_dot1x_auth_status_t;

typedef enum rtk_dot1x_direction_e
{
    BOTH = 0,
    IN,
    DIRECTION_END
} rtk_dot1x_direction_t;


typedef enum rtk_trap_misc_action_e
{
    MISC_ACTION_FORWARD=0,
    MISC_ACTION_TRAP,
    MISC_ACTION_COPY_TO_CPU,
    MISC_ACTION_DROP,
    MISC_ACTION_END
} rtk_trap_misc_action_t;

typedef enum rtk_opHdMisc_action_e
{
    OPHD_MISC_ACTION_NORMAL=0,
    OPHD_MISC_ACTION_TRAP,
    OPHD_MISC_ACTION_END
} rtk_opHdMisc_action_t;

typedef enum rtk_mode_ext_e
{
    MODE_EXT_DISABLE = 0,
    MODE_EXT_RGMII_NODELAY,
    MODE_EXT_RGMII_RXDELAY_ONLY,
    MODE_EXT_RGMII_TXDELAY_ONLY,
    MODE_EXT_RGMII_RXTXDELAY,
    MODE_EXT_TMII_MII_MAC,
    MODE_EXT_MII_PHY,
    MODE_EXT_TMII_PHY,
    MODE_EXT_RMII_INPUT,
    MODE_EXT_RMII_OUTPUT, 
    MODE_EXT_HEAC,
    MODE_EXT_FAST_ETHERNET, 
    MODE_EXT_GIGA,
    MODE_EXT_END
} rtk_mode_ext_t;

/* WOL type */
typedef enum rtk_wol_intr_type_e
{
    LEVEL_HIGH,
    LEVEL_LOW,
    PULSE_POSITIVE,
    PULSE_NEG,
}rtk_wol_intr_type_t;

typedef enum rtk_wol_intr_pulse_interval_e
{
    WOL_16NS = 0,
    WOL_128NS,
    WOL_2048NS,
    WOL_65536NS,
}rtk_wol_intr_pulse_interval_t;

typedef enum rtk_wol_pkt_behaviour_e
{
    WOL_DROP = 0,
    WOL_FORWARD,
}rtk_wol_pkt_behaviour_t;

typedef enum rtk_hec_mode_e
{
    HEC_MODE_DISABLE   = 0,   /* HEC disabled */  
    HEC_MODE_ENABLE    = 1    /* HEC enabled  */
} rtk_hec_mode_t;

typedef enum rtk_arc_mode_e
{
    ARC_MODE_DISABLE   = 0,   /* ARC disabled       */  

    ARC_MODE_TX_COMMON = 1,   /* ARC Tx Common Mode */
    ARC_MODE_TX_SINGLE = 2,   /* ARC Tx Single Mode */

    ARC_MODE_RX_BOTH   = 3,   /* ARC Rx Single & Common Mode */

    ARC_MODE_END
} rtk_arc_mode_t;


typedef enum rtk_spdif_dir_e
{
    SPDIF_SPDIF_INPUT  = 0,   /* RTL8307H receives SPDIF signal       */
    SPDIF_SPDIF_OUTPUT = 1,   /* Send SPDIF signal out from RTL8307H  */  
} rtk_spdif_dir_t;

typedef enum rtk_port_interface_e
{
    RTK_PORT_INTERFACE_FE = 0,/* The port is fast ethernet port       */
    RTK_PORT_INTERFACE_HEAC,  /* The port is HEAC port                */  

    RTK_PORT_INTERFACE_END
} rtk_port_interface_t;

#endif /* __RTK_API_H__ */
