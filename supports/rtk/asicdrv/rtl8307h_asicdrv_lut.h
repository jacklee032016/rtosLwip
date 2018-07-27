#ifndef _RTL8307H_ASICDRV_LUT_H_
#define _RTL8307H_ASICDRV_LUT_H_

#include <rtk_types.h>
#include <rtk_error.h>

#define RTL8307H_LUT_INVALID_FID        0xf
#define RTL8307H_LUT_FID_MASK           0xf
#define RTL8307H_LUT_UNTAG_CVID         0xfff

#define RTL8307H_LUT_DATA0_MASK         0x3ffff
#define RTL8307H_LUT_DATA1_MASK         0xffffffff
#define RTL8307H_LUT_DATA2_MASK         0x7ffff

#define RTL8307H_LUTCAM_DATA0_MASK         0x3ffff
#define RTL8307H_LUTCAM_DATA1_MASK         0xffffffff
#define RTL8307H_LUTCAM_DATA2_MASK         0xfffffff

#define RTL8307H_LUTCAM_INDEX_START     0
#define RTL8307H_LUTCAM_INDEX_END       15

#define RTL8307H_TBL_GROUP_DATA0_MASK   0xff
#define RTL8307H_TBL_GROUP_DATA1_MASK   0xfffffff

typedef enum hash_method_e {
    HASH_WITH_CVID = 0,
    HASH_WITH_FID  = 1,
}rtl8307h_lut_hash_method_t;

typedef enum rtl8307h_lut_4way_idx_e
{
    LUT_4WAY_FIRST = 0,
    LUT_4WAY_SECOND= 1,
    LUT_4WAY_THIRD = 2,
    LUT_4WAY_FOURTH= 3,
    LUT_4WAY_END,
}rtl8307h_lut_4way_idx_t;

typedef enum rtl8307h_lut_8way_idx_e
{
    LUT_8WAY_FIRST = 0,
    LUT_8WAY_SECOND,
    LUT_8WAY_THIRD,
    LUT_8WAY_FOURTH,
    LUT_8WAY_FIFTH,
    LUT_8WAY_SIXTH,
    LUT_8WAY_SEVENTH,
    LUT_8WAY_EIGHTH,
    LUT_8WAY_END,
}rtl8307h_lut_8way_idx_t;

typedef enum rtl8307h_lut_entry_type_e
{
    LUT_UNICAST,
    LUT_MULTI,
    LUT_IPMULTI,
    LUT_INVALID_ENTRY,
    LUT_ENTRY_TYPE_END
}rtl8307h_lut_entry_type_t;

typedef enum rtl8307h_lut_flush_mode_e
{
    LUT_FLUSH_ALL = 0,
    LUT_FLUSH_PORT_ONLY,
    LUT_FLUSH_PORT_CVID,
    LUT_FLUSH_PORT_FID,
    LUT_FLUSH_END
}rtl8307h_lut_flush_mode_t;

/*address table lookup entry*/
typedef struct rtl8307h_lut_uni_param_s
{
    uint16 cvid;
    uint8  fid;
    uint8  mac[6];
    uint8  spa;    
    uint8  age;
    uint8  bstatic;
    uint8  auth;
    
    uint8  sablk;
    uint8  dablk;
    
    uint8  eav_pri_en;
    uint8  eav_pri;    

    uint8  ulfid;   /* 1: hash with fid, 0: hash with cvid */
}rtl8307h_lut_uni_param_t;

typedef struct rtl8307h_lut_multi_param_s
{
    uint16 ppi_fid;
    uint8  mac[6];
    uint8  crsvlan;    
    uint8  portmask;
    
    uint8  eav_pri_en;
    uint8  eav_pri;    
}rtl8307h_lut_multi_param_t;

typedef struct rtl8307h_lut_ipMulti_param_s
{
    uint8 dip[4];
    uint8 sip[4];
    
    uint8  crsvlan;    
    uint8  portmask;
    
    uint8  eav_pri_en;
    uint8  eav_pri;
}rtl8307h_lut_ipMulti_param_t;

typedef struct rtl8307h_lut_group_param_s
{
    uint8 dip[4];        
    uint8  portmask;
}rtl8307h_lut_group_param_t;

typedef struct rtl8307h_lut_prarm_s{
    rtl8307h_lut_entry_type_t type; 
    union {
        rtl8307h_lut_uni_param_t     uni;       /* unicast parameter        */
        rtl8307h_lut_multi_param_t   multi;     /* multicast parameter      */
        rtl8307h_lut_ipMulti_param_t ip;        /* ip multicast parameter   */
    }val;    

    /* entry number in lut */
    uint16 index; 
}rtl8307h_lut_param_t;

typedef struct rtl8307h_tblasic_entry_s
{
    uint32 val[3];   
}rtl8307h_tblasic_entry_t;

typedef struct rtl8307h_tblgroup_entry_s
{
    uint32 val[2];   
}rtl8307h_tblgroup_entry_t;

typedef struct rtl8307h_flush_param_s
{
    uint16 cvid;
    uint8 portmask; 
    uint8 fid;
}rtl8307h_flush_param_t;

#define lutEther   key.ether
#define lutIp       key.ip
#define lutInfo     info.mac

typedef struct lut_search_param_s{
    uint32 defPortNum;
    uint32 portNum ;
    uint32 defMac;
    ether_addr_t mac;
    uint32 defFid ;
    uint8  ulfid; /* 0: hash with vid, 1: hash with fid */
    uint32 fid;
    uint32 vid;
    uint32 startIdx; 
}lut_search_param_t;

extern int32 rtl8307h_addrTbl_entry_delete(uint32 index);
extern int32 rtl8307h_addrTbl_entry_get(uint32 hashAlg, uint32 index, rtl8307h_lut_param_t* param);

extern int32 rtl8307h_addrTbl_uni_entry_set(uint32 hashAlg, const rtl8307h_lut_uni_param_t* param, rtl8307h_lut_4way_idx_t idx);
extern int32 rtl8307h_addrTbl_uni_entry_get(uint32 hashAlg, uint32 index, rtl8307h_lut_uni_param_t* param);
extern int32 rtl8307h_addrTbl_uni_entry_force_set(uint32 hashAlg, uint32 index, const rtl8307h_lut_uni_param_t* param);

extern int32 rtl8307h_addrTbl_multicast_entry_set(uint32 hashAlg, const rtl8307h_lut_multi_param_t* param, rtl8307h_lut_4way_idx_t idx);
extern int32 rtl8307h_addrTbl_multicast_entry_get(uint32 hashAlg, uint32 index, rtl8307h_lut_multi_param_t* param);

extern int32 rtl8307h_addrTbl_ip_entry_set(uint32 hashAlg, const rtl8307h_lut_ipMulti_param_t* param, rtl8307h_lut_4way_idx_t idx);
extern int32 rtl8307h_addrTbl_ip_entry_get(uint32 hashAlg, uint32 index, rtl8307h_lut_ipMulti_param_t* param);

extern int32 rtl8307h_addrTbl_group_entry_set(const rtl8307h_lut_group_param_t* param, rtl8307h_lut_8way_idx_t idx);
extern int32 rtl8307h_addrTbl_group_entry_get(uint32 index, rtl8307h_lut_group_param_t* param);

extern int32 rtl8307h_addrTbl_cam_entry_get(uint32 index, rtl8307h_lut_param_t* param);
extern int32 rtl8307h_addrTbl_cam_entry_set(uint32 index, rtl8307h_lut_param_t* param);

extern int32 rtl8307h_addrTbl_flush(rtl8307h_lut_flush_mode_t mode, rtl8307h_flush_param_t* param);
extern int32 rtl8307h_addrTbl_search(lut_search_param_t *param, uint32 *found, uint32* hitaddr);

extern uint32 rtl8307h_addrTbl_hashAlg_get(void);
extern int32 rtl8307h_addrTbl_hashAlg_set(uint32 hashAlg);

extern int32 rtl8307h_addrTbl_bktFull_trap(uint32 enable);
extern int32 rtl8307h_addrTbl_lru_enable(uint32 enable);


extern int32 rtl8307h_maclrnlimit_enable(uint32 port);
extern int32 rtl8307h_maclrnlimit_disable(uint32 port);
extern int32 rtl8307h_maclrnlimit_maxnum_set(uint32 port, uint32 maxNum);
extern int32 rtl8307h_maclrnlimit_maxnum_get(uint32 port, uint32* maxNum);
extern int32 rtl8307h_maclrnlimit_curnum_get(uint32 port, uint32* curNum);
extern int32 rtl8307h_maclrnlimit_action_set(uint32 port, uint32 action);
extern int32 rtl8307h_maclrnlimit_action_get(uint32 port, uint32* action);
extern int32 rtl8307h_maclrnlimit_cpuport_set(uint32 port);
extern int32 rtl8307h_maclrnlimit_cpuport_get(uint32* port);

#endif /*_RTL8307H_ASICDRV_LUT_H_*/

