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
 * $Revision: 8194 $
 * $Date: 2010-01-21 20:28:36 +0800 (Thu, 21 Jan 2010) $
 *
 * Purpose : RTL8307H switch high-level API for RTL8367B
 * Feature : 
 *
 */
#include <string.h>
#include <rtk_types.h>
#include <rtk_error.h>
#include <rtk_api.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>
#include <rtl8307h_table_struct.h>
#include <rtl8307h_debug.h>

#include <rtl8307h_asicdrv_lut.h>

/* calculate index using hash algorithm */
static int32 _rtl8307h_lut_hash0_unicast_index_get(const rtl8307h_lut_uni_param_t* param, rtl8307h_lut_4way_idx_t idx)
{
    uint16 index;
    uint16 key[7];
    uint16 tmp;

    key[0] = ((uint16)param->mac[4] & 0x1) << 8 | param->mac[5];
    key[1] = ((uint16)param->mac[3] & 0x3) << 7 | (param->mac[4] >> 1);
    key[2] = ((uint16)param->mac[2] & 0x7) << 6 | (param->mac[3] >> 2);
    key[3] = ((uint16)param->mac[1] & 0xf) << 5 | (param->mac[2] >> 3);
    key[4] = ((uint16)param->mac[0]& 0x1f) << 4 | (param->mac[1] >> 4);

    tmp = (param->ulfid == HASH_WITH_FID) ? (uint16)param->fid : ((uint16)param->cvid & 0x3f);
    key[5] = tmp << 3 | (param->mac[0] >> 5);
    key[6] = (param->ulfid == HASH_WITH_FID) ? 0 : (param->cvid >> 6);        

    
    index = key[0] ^ key[1] ^ key[2] ^ key[3] ^ key[4] ^ key[5] ^ key[6];

    index = index << 2 | idx;
    
    DEBUG_INFO(DBG_LUT,  "The index is 0x%x\n", index);           
    return index;
}

static int32 _rtl8307h_lut_hash0_multicast_index_get(const rtl8307h_lut_multi_param_t* param, rtl8307h_lut_4way_idx_t idx)
{
    uint16 index;
    uint16 key[7];

    key[0] = ((uint16)param->mac[4] & 0x1) << 8 | param->mac[5];
    key[1] = ((uint16)param->mac[3] & 0x3) << 7 | (param->mac[4] >> 1);
    key[2] = ((uint16)param->mac[2] & 0x7) << 6 | (param->mac[3] >> 2);
    key[3] = ((uint16)param->mac[1] & 0xf) << 5 | (param->mac[2] >> 3);
    key[4] = ((uint16)param->mac[0]& 0x1f) << 4 | (param->mac[1] >> 4);

    key[5] = ((uint16)param->ppi_fid & 0x3f) << 3 | (param->mac[0] >> 5);
    key[6] = param->ppi_fid >> 6;      
    
    index = key[0] ^ key[1] ^ key[2] ^ key[3] ^ key[4] ^ key[5] ^ key[6];

    index = index << 2 | idx;
    
    DEBUG_INFO(DBG_LUT, "The index is 0x%x\n", index);           
    return index;
}

static int32 _rtl8307h_lut_hash0_ip_index_get(const rtl8307h_lut_ipMulti_param_t* param, rtl8307h_lut_4way_idx_t idx)
{
    uint16 index;
    uint16 key[7];

    key[0] = ((uint16)param->sip[2] & 0x1) << 8 | param->sip[3]; 
    key[1] = ((uint16)param->sip[1] & 0x3) << 7 | (param->sip[2] >> 1);
    key[2] = ((uint16)param->sip[0] & 0x7) << 6 | (param->sip[1] >> 2);
    key[3] = ((uint16)param->dip[3] & 0xf) << 5 | (param->sip[0] >> 3);
    key[4] = ((uint16)param->dip[2]& 0x1f) << 4 | (param->dip[3] >> 4);
    key[5] = ((uint16)param->dip[1]& 0x3f) << 3 | (param->dip[2] >> 5);
    key[6] = ((uint16)param->dip[0]& 0x0f) << 2 | (param->dip[1] >> 6); 
    
    index = key[0] ^ key[1] ^ key[2] ^ key[3] ^ key[4] ^ key[5] ^ key[6];

    index = index << 2 | idx;
    
    DEBUG_INFO(DBG_LUT, "The index is 0x%x\n", index);           
    return index;
}

static int32 _rtl8307h_lut_hash_group_index_get(const rtl8307h_lut_group_param_t* param, rtl8307h_lut_8way_idx_t idx)
{
    uint16 index;
    uint16 key[7];

    key[0] = param->dip[3] & 0xf; 
    key[1] = (param->dip[3] >> 4) & 0xf; 
    key[2] = param->dip[2] & 0xf;
    key[3] = (param->dip[2] >> 4) & 0xf; 
    key[4] = param->dip[1] & 0xf;
    key[5] = (param->dip[1] >> 4) & 0xf; 
    key[6] = param->dip[0] & 0xf;
    
    index = key[0] ^ key[1] ^ key[2] ^ key[3] ^ key[4] ^ key[5] ^ key[6];

    index = index << 3 | idx;
    
    DEBUG_INFO(DBG_LUT, "The index is 0x%x\n", index);           
    return index;
}

#ifndef	MUX_LAB
static int32 _rtl8307h_lut_hash0_index_get(const rtl8307h_lut_param_t* param, rtl8307h_lut_4way_idx_t idx)
{
    if (param->type == LUT_UNICAST)
    {
        return _rtl8307h_lut_hash0_unicast_index_get(&param->val.uni, idx);    
    }
    else if (param->type == LUT_MULTI)
    {
        return _rtl8307h_lut_hash0_multicast_index_get(&param->val.multi, idx);     
    }
    else
    {
        return _rtl8307h_lut_hash0_ip_index_get(&param->val.ip, idx);     
    }
}
#endif

/* restore the bits in LUT using reverse hash algorithm */
static void _rtl8307h_addrTbl_hash0_unicast_restore(uint16 index, rtl8307h_lut_uni_param_t* param)
{
    uint16 restore_bits;
    uint16 key[7];
    uint16 tmp;
        
    key[0] = (index >> 2);
    key[1] = ((uint16)param->mac[3] & 0x3) << 7 | (param->mac[4] >> 1);
    key[2] = ((uint16)param->mac[2] & 0x7) << 6 | (param->mac[3] >> 2);
    key[3] = ((uint16)param->mac[1] & 0xf) << 5 | (param->mac[2] >> 3);
    key[4] = ((uint16)param->mac[0]& 0x1f) << 4 | (param->mac[1] >> 4);
       
    tmp = (param->ulfid == 1) ? (uint16)param->fid : ((uint16)param->cvid & 0x3f);
    key[5] = tmp << 3 | (param->mac[0] >> 5);
    key[6] = param->ulfid == 1 ? 0 : (param->cvid >> 6);        

//    DEBUG_INFO(DBG_LUT, "key[0]: %d\n", key[0]);
//    DEBUG_INFO(DBG_LUT, "key[1]: %d\n", key[1]);
//    DEBUG_INFO(DBG_LUT, "key[2]: %d\n", key[2]);
//    DEBUG_INFO(DBG_LUT, "key[3]: %d\n", key[3]);
//    DEBUG_INFO(DBG_LUT, "key[4]: %d\n", key[4]); 
//    DEBUG_INFO(DBG_LUT, "key[5]: %d\n", key[5]);
//    DEBUG_INFO(DBG_LUT, "key[6]: %d\n", key[6]);
    
    restore_bits = key[0] ^ key[1] ^ key[2] ^ key[3] ^ key[4] ^ key[5] ^ key[6];

    param->mac[4] &= 0xfe;
    param->mac[4] |= (restore_bits >> 8) & 0x1;

    param->mac[5] = restore_bits & 0xff;
}

static void _rtl8307h_addrTbl_hash0_multi_restore(uint16 index, rtl8307h_lut_multi_param_t* param)
{
    uint16 restore_bits;
    uint16 key[7];
#ifndef	MUX_LAB
    uint16 tmp;
#endif

    key[0] = (index >> 2);
    key[1] = ((uint16)param->mac[3] & 0x3) << 7 | (param->mac[4] >> 1);
    key[2] = ((uint16)param->mac[2] & 0x7) << 6 | (param->mac[3] >> 2);
    key[3] = ((uint16)param->mac[1] & 0xf) << 5 | (param->mac[2] >> 3);
    key[4] = ((uint16)param->mac[0]& 0x1f) << 4 | (param->mac[1] >> 4);
    key[5] = (param->ppi_fid & 0x3f) << 3 | (param->mac[0] >> 5);
    key[6] = param->ppi_fid >> 6;        

    DEBUG_INFO(DBG_LUT, "key[0]: %d\n", key[0]);
    DEBUG_INFO(DBG_LUT, "key[1]: %d\n", key[1]);
    DEBUG_INFO(DBG_LUT, "key[2]: %d\n", key[2]);
    DEBUG_INFO(DBG_LUT, "key[3]: %d\n", key[3]);
    DEBUG_INFO(DBG_LUT, "key[4]: %d\n", key[4]); 
    DEBUG_INFO(DBG_LUT, "key[5]: %d\n", key[5]);
    DEBUG_INFO(DBG_LUT, "key[6]: %d\n", key[6]);
    
    restore_bits = key[0] ^ key[1] ^ key[2] ^ key[3] ^ key[4] ^ key[5] ^ key[6];
    DEBUG_INFO(DBG_LUT, "restore_bits: %x\n", restore_bits);
    
    param->mac[4] &= 0xfe;
    param->mac[4] |= (restore_bits >> 8) & 0x1;

    param->mac[5] = restore_bits & 0xff;
}

static void _rtl8307h_addrTbl_hash0_ip_restore(uint16 index, rtl8307h_lut_ipMulti_param_t* param)
{
    uint16 restore_bits;
    uint16 key[7];
//    uint16 tmp;
        
    key[0] = (index >> 2);
    key[1] = ((uint16)param->sip[1] & 0x3) << 7 | (param->sip[2] >> 1);
    key[2] = ((uint16)param->sip[0] & 0x7) << 6 | (param->sip[1] >> 2);
    key[3] = ((uint16)param->dip[3] & 0xf) << 5 | (param->sip[0] >> 3);
    key[4] = ((uint16)param->dip[2]& 0x1f) << 4 | (param->dip[3] >> 4);
    key[5] = ((uint16)param->dip[1]& 0x3f) << 3 | (param->dip[2] >> 5);
    key[6] = ((uint16)param->dip[0]& 0x0f) << 2 | (param->dip[1] >> 6); 
    
    DEBUG_INFO(DBG_LUT, "key[0]: %d\n", key[0]);
    DEBUG_INFO(DBG_LUT, "key[1]: %d\n", key[1]);
    DEBUG_INFO(DBG_LUT, "key[2]: %d\n", key[2]);
    DEBUG_INFO(DBG_LUT, "key[3]: %d\n", key[3]);
    DEBUG_INFO(DBG_LUT, "key[4]: %d\n", key[4]); 
    DEBUG_INFO(DBG_LUT, "key[5]: %d\n", key[5]);
    DEBUG_INFO(DBG_LUT, "key[6]: %d\n", key[6]);
    
    restore_bits = key[0] ^ key[1] ^ key[2] ^ key[3] ^ key[4] ^ key[5] ^ key[6];
    DEBUG_INFO(DBG_LUT, "restore_bits: %x\n", restore_bits);
    
    param->sip[2] &= 0xfe;
    param->sip[2] |= (restore_bits >> 8) & 0x1;

    param->sip[3] = restore_bits & 0xff;
}

/* calculate index using hash algorithm */
static int32 _rtl8307h_lut_hash1_unicast_index_get(const rtl8307h_lut_uni_param_t* param, rtl8307h_lut_4way_idx_t idx)
{
    uint16 index;
    uint16 key[7];
    uint16 tmp;

    key[0] = param->mac[5] | ((uint16)param->mac[4] & 0x1) << 8;
    key[1] = ((param->mac[4] >> 3) & 0x1f) |(((uint16)param->mac[3] & 0x3) << 5) | ((((uint16)param->mac[4] >> 1) & 0x3) << 7);
    key[2] = ((param->mac[3] >> 5) & 0x7) |(((uint16)param->mac[2] & 0x7) << 3) | ((((uint16)param->mac[3] >> 2) & 0x7) << 6);
    key[3] = ((param->mac[2] >> 7) & 0x1) |(((uint16)param->mac[1] & 0xf) << 1) | ((((uint16)param->mac[2] >> 3) & 0xf) << 5);
    key[4] = ((param->mac[0] >> 1) & 0xf) |((uint16)param->mac[1] & 0xf0) | ((param->mac[0] & 0x1) << 8);

    tmp = (param->ulfid == HASH_WITH_FID) ? (uint16)param->fid : ((uint16)param->cvid & 0x3f);
    key[5] = tmp >> 3 | (((param->mac[0] >> 5) & 0x7) << 3) | ((tmp & 0x7) << 6);
    key[6] = (param->ulfid == HASH_WITH_FID) ? 0 : ((param->cvid >> 11) | ((param->cvid >> 6) & 0x1f) << 1);        
    
    index = key[0] ^ key[1] ^ key[2] ^ key[3] ^ key[4] ^ key[5] ^ key[6];

    index = index << 2 | idx;
    
    DEBUG_INFO(DBG_LUT, "The index is 0x%x\n", index);           
    return index;
}

static int32 _rtl8307h_lut_hash1_multicast_index_get(const rtl8307h_lut_multi_param_t* param, rtl8307h_lut_4way_idx_t idx)
{
    uint16 index;
    uint16 key[7];

    key[0] = param->mac[5] | ((uint16)param->mac[4] & 0x1) << 8;
    key[1] = ((param->mac[4] >> 3) & 0x1f) |(((uint16)param->mac[3] & 0x3) << 5) | ((((uint16)param->mac[4] >> 1) & 0x3) << 7);
    key[2] = ((param->mac[3] >> 5) & 0x7) |(((uint16)param->mac[2] & 0x7) << 3) | ((((uint16)param->mac[3] >> 2) & 0x7) << 6);
    key[3] = ((param->mac[2] >> 7) & 0x1) |(((uint16)param->mac[1] & 0xf) << 1) | ((((uint16)param->mac[2] >> 3) & 0xf) << 5);
    key[4] = ((param->mac[0] >> 1) & 0xf) |((uint16)param->mac[1] & 0xf0) | ((param->mac[0] & 0x1) << 8);

    key[5] = ((param->ppi_fid >> 3) & 0x7) | (((param->mac[0] >> 5) & 0x7) << 3) | ((param->ppi_fid & 0x7) << 6);
    key[6] = ((param->ppi_fid >> 11) | (((param->ppi_fid >> 6)) & 0x1f) << 1);              
    
    index = key[0] ^ key[1] ^ key[2] ^ key[3] ^ key[4] ^ key[5] ^ key[6];

    index = index << 2 | idx;
    
    DEBUG_INFO(DBG_LUT, "The index is 0x%x\n", index);           
    return index;
}

static int32 _rtl8307h_lut_hash1_ip_index_get(const rtl8307h_lut_ipMulti_param_t* param, rtl8307h_lut_4way_idx_t idx)
{
    uint16 index;
    uint16 key[7];

    key[0] = param->sip[3] | ((uint16)param->sip[2] & 0x1) << 8;
    key[1] = ((param->sip[2] >> 3) & 0x1f) |(((uint16)param->sip[1] & 0x3) << 5) | ((((uint16)param->sip[2] >> 1) & 0x3) << 7);
    key[2] = ((param->sip[1] >> 5) & 0x7) |(((uint16)param->sip[0] & 0x7) << 3) | ((((uint16)param->sip[1] >> 2) & 0x7) << 6);
    key[3] = ((param->sip[0] >> 7) & 0x1) |(((uint16)param->dip[3] & 0xf) << 1) | ((((uint16)param->sip[0] >> 3) & 0xf) << 5);
    key[4] = ((param->dip[2] >> 1) & 0xf) |((uint16)param->dip[3] & 0xf0) | ((param->dip[2] & 0x1) << 8);
    key[5] = ((param->dip[1] >> 3) & 0x7) | (((param->dip[2] >> 5) & 0x7) << 3) | ((param->dip[1] & 0x7) << 6);
    key[6] = ((param->dip[0] >> 3) & 0x1) | (((param->dip[1] >> 6) & 0x3) << 1) | ((param->dip[0] & 0x7) << 3);             
    
    index = key[0] ^ key[1] ^ key[2] ^ key[3] ^ key[4] ^ key[5] ^ key[6];

    index = index << 2 | idx;
    
    DEBUG_INFO(DBG_LUT, "The index is 0x%x\n", index);           
    return index;
}

#ifndef	MUX_LAB
static int32 _rtl8307h_lut_hash1_index_get(const rtl8307h_lut_param_t* param, rtl8307h_lut_4way_idx_t idx)
{
    if (param->type == LUT_UNICAST)
    {
        return _rtl8307h_lut_hash1_unicast_index_get(&param->val.uni, idx);    
    }
    else if (param->type == LUT_MULTI)
    {
        return _rtl8307h_lut_hash1_multicast_index_get(&param->val.multi, idx);     
    }
    else
    {
        return _rtl8307h_lut_hash1_ip_index_get(&param->val.ip, idx);     
    }
}
#endif

/* restore the bits in LUT using reverse hash algorithm */
static void _rtl8307h_addrTbl_hash1_unicast_restore(uint16 index, rtl8307h_lut_uni_param_t* param)
{
    uint16 restore_bits;
    uint16 key[7];
    uint16 tmp;
        
    key[0] = (index >> 2);
    key[1] = ((param->mac[4] >> 3) & 0x1f) |(((uint16)param->mac[3] & 0x3) << 5) | ((((uint16)param->mac[4] >> 1) & 0x3) << 7);
    key[2] = ((param->mac[3] >> 5) & 0x7) |(((uint16)param->mac[2] & 0x7) << 3) | ((((uint16)param->mac[3] >> 2) & 0x7) << 6);
    key[3] = ((param->mac[2] >> 7) & 0x1) |(((uint16)param->mac[1] & 0xf) << 1) | ((((uint16)param->mac[2] >> 3) & 0xf) << 5);
    key[4] = ((param->mac[0] >> 1) & 0xf) |((uint16)param->mac[1] & 0xf0) | ((param->mac[0] & 0x1) << 8);

    tmp = (param->ulfid == HASH_WITH_FID) ? (uint16)param->fid : ((uint16)param->cvid & 0x3f);
    key[5] = tmp >> 3 | (((param->mac[0] >> 5) & 0x7) << 3) | ((tmp & 0x7) << 6);
    key[6] = (param->ulfid == HASH_WITH_FID) ? 0 : ((param->cvid >> 11) | (((param->cvid >> 6)) & 0x1f) << 1);        
   

//    DEBUG_INFO(DBG_LUT, "key[0]: %d\n", key[0]);
//    DEBUG_INFO(DBG_LUT, "key[1]: %d\n", key[1]);
//    DEBUG_INFO(DBG_LUT, "key[2]: %d\n", key[2]);
//    DEBUG_INFO(DBG_LUT, "key[3]: %d\n", key[3]);
//    DEBUG_INFO(DBG_LUT, "key[4]: %d\n", key[4]); 
//    DEBUG_INFO(DBG_LUT, "key[5]: %d\n", key[5]);
//    DEBUG_INFO(DBG_LUT, "key[6]: %d\n", key[6]);
    
    restore_bits = key[0] ^ key[1] ^ key[2] ^ key[3] ^ key[4] ^ key[5] ^ key[6];

    param->mac[4] &= 0xfe;
    param->mac[4] |= (restore_bits >> 8) & 0x1;

    param->mac[5] = restore_bits & 0xff;
}

static void _rtl8307h_addrTbl_hash1_multi_restore(uint16 index, rtl8307h_lut_multi_param_t* param)
{
    uint16 restore_bits;
    uint16 key[7];
//    uint16 tmp;
        
    key[0] = (index >> 2);
    key[1] = ((param->mac[4] >> 3) & 0x1f) |(((uint16)param->mac[3] & 0x3) << 5) | ((((uint16)param->mac[4] >> 1) & 0x3) << 7);
    key[2] = ((param->mac[3] >> 5) & 0x7) |(((uint16)param->mac[2] & 0x7) << 3) | ((((uint16)param->mac[3] >> 2) & 0x7) << 6);
    key[3] = ((param->mac[2] >> 7) & 0x1) |(((uint16)param->mac[1] & 0xf) << 1) | ((((uint16)param->mac[2] >> 3) & 0xf) << 5);
    key[4] = ((param->mac[0] >> 1) & 0xf) |((uint16)param->mac[1] & 0xf0) | ((param->mac[0] & 0x1) << 8);

    key[5] = ((param->ppi_fid >> 3) & 0x7)| (((param->mac[0] >> 5) & 0x7) << 3) | ((param->ppi_fid & 0x7) << 6);
    key[6] = (param->ppi_fid >> 11) | ((param->ppi_fid >> 6) & 0x1f) << 1;              
   
    DEBUG_INFO(DBG_LUT, "key[0]: %d\n", key[0]);
    DEBUG_INFO(DBG_LUT, "key[1]: %d\n", key[1]);
    DEBUG_INFO(DBG_LUT, "key[2]: %d\n", key[2]);
    DEBUG_INFO(DBG_LUT, "key[3]: %d\n", key[3]);
    DEBUG_INFO(DBG_LUT, "key[4]: %d\n", key[4]); 
    DEBUG_INFO(DBG_LUT, "key[5]: %d\n", key[5]);
    DEBUG_INFO(DBG_LUT, "key[6]: %d\n", key[6]);
    
    restore_bits = key[0] ^ key[1] ^ key[2] ^ key[3] ^ key[4] ^ key[5] ^ key[6];
    DEBUG_INFO(DBG_LUT, "restore_bits: %x\n", restore_bits);
    
    param->mac[4] &= 0xfe;
    param->mac[4] |= (restore_bits >> 8) & 0x1;

    param->mac[5] = restore_bits & 0xff;
}

static void _rtl8307h_addrTbl_hash1_ip_restore(uint16 index, rtl8307h_lut_ipMulti_param_t* param)
{
    uint16 restore_bits;
    uint16 key[7];
//    uint16 tmp;
        
    key[0] = (index >> 2);
    key[1] = ((param->sip[2] >> 3) & 0x1f) |(((uint16)param->sip[1] & 0x3) << 5) | ((((uint16)param->sip[2] >> 1) & 0x3) << 7);
    key[2] = ((param->sip[1] >> 5) & 0x7) |(((uint16)param->sip[0] & 0x7) << 3) | ((((uint16)param->sip[1] >> 2) & 0x7) << 6);
    key[3] = ((param->sip[0] >> 7) & 0x1) |(((uint16)param->dip[3] & 0xf) << 1) | ((((uint16)param->sip[0] >> 3) & 0xf) << 5);
    key[4] = ((param->dip[2] >> 1) & 0xf) |((uint16)param->dip[3] & 0xf0) | ((param->dip[2] & 0x1) << 8);
    key[5] = ((param->dip[1] >> 3) & 0x7) | (((param->dip[2] >> 5) & 0x7) << 3) | ((param->dip[1] & 0x7) << 6);
    key[6] = ((param->dip[0] >> 3) & 0x1) | (((param->dip[1] >> 6) & 0x3) << 1) | ((param->dip[0] & 0x7) << 3);             
      
    DEBUG_INFO(DBG_LUT, "key[0]: %d\n", key[0]);
    DEBUG_INFO(DBG_LUT, "key[1]: %d\n", key[1]);
    DEBUG_INFO(DBG_LUT, "key[2]: %d\n", key[2]);
    DEBUG_INFO(DBG_LUT, "key[3]: %d\n", key[3]);
    DEBUG_INFO(DBG_LUT, "key[4]: %d\n", key[4]); 
    DEBUG_INFO(DBG_LUT, "key[5]: %d\n", key[5]);
    DEBUG_INFO(DBG_LUT, "key[6]: %d\n", key[6]);
    
    restore_bits = key[0] ^ key[1] ^ key[2] ^ key[3] ^ key[4] ^ key[5] ^ key[6];
    DEBUG_INFO(DBG_LUT, "restore_bits: %x\n", restore_bits);
    
    param->sip[2] &= 0xfe;
    param->sip[2] |= (restore_bits >> 8) & 0x1;

    param->sip[3] = restore_bits & 0xff;
}

static int32 _rtl8307h_addrTbl_unicast_param2entry(rtk_table_list_t table, rtl8307h_tblasic_entry_t *entry, const rtl8307h_lut_uni_param_t* param)
{
    uint8 fid;

    if (param->ulfid == 0)
        fid = RTL8307H_LUT_INVALID_FID;
    else
        fid = param->fid;
        
    memset(entry, 0, sizeof(rtl8307h_tblasic_entry_t));
    
    entry->val[0] = (uint32)param->eav_pri_en       |           \
                    (uint32)param->eav_pri<< 1      |           \
                    (uint32)fid           << 4      |           \
                    (uint32)param->bstatic<< 8      |           \
                    (uint32)param->auth   << 9      |           \
                    (uint32)param->dablk  << 10     |           \
                    (uint32)param->sablk  << 11     |           \
                    (uint32)param->spa    << 12     |           \
                    (uint32)param->age    << 15     |           \
                    (uint32)0             << 17;    /* L3Entry = 0 */ 

    if (table == L2_TABLE)
    {
        entry->val[1] = ((uint32)param->mac[4] & 0xfe) >> 1 |       \
                        (uint32)param->mac[3] << 7      |           \
                        (uint32)param->mac[2] << 15     |           \
                        (uint32)param->mac[1] << 23     |           \
                        (uint32)(param->mac[0] & 0x1) << 31; /* should be 0 in bit 31 */

        entry->val[2] = ((uint32)param->mac[0] & 0xfe) >> 1 |      \
                        (uint32)param->cvid << 7;  
    }
    else
    {
        entry->val[1] = ((uint32)param->mac[5] & 0xff)  |           \
                        (uint32)param->mac[4] << 8      |           \
                        (uint32)param->mac[3] << 16     |           \
                        (uint32)param->mac[2] << 24;
                        
        entry->val[2] = ((uint32)param->mac[1] & 0xff)  |           \
                        (uint32)param->mac[0] << 8      |           \
                        (uint32)param->cvid << 16; 
    }
#if 0
    DEBUG_INFO(DBG_LUT, "print _rtl8307h_addrTbl_unicast_param2entry parameter:\n");
    DEBUG_INFO(DBG_LUT, "eav_pri_en : %d\n", param->eav_pri_en);
    DEBUG_INFO(DBG_LUT, "eav_pri    : %d\n", param->eav_pri);
    DEBUG_INFO(DBG_LUT, "cvid       : %d\n", param->cvid);  
    if (param->fid == RTL8307H_LUT_INVALID_FID)
        DEBUG_INFO(DBG_LUT, "fid        : Invalid(15)\n"); 
    else
        DEBUG_INFO(DBG_LUT, "fid        : %d\n", param->fid);   
    DEBUG_INFO(DBG_LUT, "bstatic    : %d\n", param->bstatic);
    DEBUG_INFO(DBG_LUT, "auth       : %d\n", param->auth);
    DEBUG_INFO(DBG_LUT, "dablk      : %d\n", param->dablk);
    DEBUG_INFO(DBG_LUT, "sablk      : %d\n", param->sablk);
    DEBUG_INFO(DBG_LUT, "spa        : %d\n", param->spa);
    DEBUG_INFO(DBG_LUT, "age        : %d\n", param->age);
    DEBUG_INFO(DBG_LUT, "mac is 0x %x-%x-%x-%x-%x-%x \n", 
                    param->mac[0], param->mac[1], param->mac[2], 
                    param->mac[3], param->mac[4], param->mac[5]);     

    DEBUG_INFO(DBG_LUT, "entry val[0]: 0x%08x\n", entry->val[0]);
    DEBUG_INFO(DBG_LUT, "entry val[1]: 0x%08x\n", entry->val[1]);
    DEBUG_INFO(DBG_LUT, "entry val[2]: 0x%08x\n", entry->val[2]);
#endif    
    return SUCCESS;
}



static int32 _rtl8307h_addrTbl_unicast_entry2param(uint32 hashAlg, rtk_table_list_t table, uint32 index, rtl8307h_lut_uni_param_t* param, const rtl8307h_tblasic_entry_t *entry)
{
#ifndef	MUX_LAB
    uint8 fid; 
#endif

    memset(param, 0, sizeof(rtl8307h_lut_uni_param_t)); 
    
    param->eav_pri_en = (entry->val[0] >> 0) & 0x1;       
    param->eav_pri    = (entry->val[0] >> 1) & 0x7;  
    
    param->fid        = (entry->val[0] >> 4) & 0xf;  
    if (param->fid == RTL8307H_LUT_INVALID_FID)  /* hash with CVID */
    {
        param->ulfid = 0;
        //DEBUG_INFO(DBG_LUT, "hash with CVID\n");
    }
    else
        param->ulfid = 1;        
        
    param->bstatic    = (entry->val[0] >> 8) & 0x1;
    param->auth       = (entry->val[0] >> 9) & 0x1;
    param->dablk      = (entry->val[0] >> 10) & 0x1;                       
    param->sablk      = (entry->val[0] >> 11) & 0x1;  
    param->spa        = (entry->val[0] >> 12) & 0x7;
    param->age        = (entry->val[0] >> 15) & 0x3; 

    if (table == L2_TABLE)
    {
        param->mac[4]     = (entry->val[1] & 0x7f) << 1;     /* bit 15:9*/  
        param->mac[3]     = (entry->val[1] >> 7) & 0xff;
        param->mac[2]     = (entry->val[1] >> 15) & 0xff;
        param->mac[1]     = (entry->val[1] >> 23) & 0xff;  
        
        param->mac[0]     = ((entry->val[1] >> 31) & 0x1) |  \
                            ((entry->val[2] & 0x7f) << 1);      
        param->cvid       = (entry->val[2] >> 7) & 0xfff; 

        if (hashAlg == 0)
            _rtl8307h_addrTbl_hash0_unicast_restore(index, param);      /* restore bit 8:0*/
        else
            _rtl8307h_addrTbl_hash1_unicast_restore(index, param);
    }
    else /* CAM */
    {
        param->mac[5]     = entry->val[1] & 0xff;     
        param->mac[4]     = (entry->val[1] >> 8) & 0xff;
        param->mac[3]     = (entry->val[1] >> 16) & 0xff;
        param->mac[2]     = (entry->val[1] >> 24) & 0xff;  

        param->mac[1]     = entry->val[2] & 0xff;     
        param->mac[0]     = (entry->val[2] >> 8) & 0xff;  
        param->cvid       = (entry->val[2] >> 16) & 0xfff;     
    }
#if 0    
    DEBUG_INFO(DBG_LUT, "print _rtl8307h_addrTbl_unicast_entry2param() parameter:\n");
    DEBUG_INFO(DBG_LUT, "eav_pri_en : %d\n", param->eav_pri_en);
    DEBUG_INFO(DBG_LUT, "eav_pri    : %d\n", param->eav_pri);
    DEBUG_INFO(DBG_LUT, "cvid       : %d\n", param->cvid);  
    if (param->fid == RTL8307H_LUT_INVALID_FID)
        DEBUG_INFO(DBG_LUT, "fid        : Invalid(15)\n"); 
    else
        DEBUG_INFO(DBG_LUT, "fid        : %d\n", param->fid);   
    DEBUG_INFO(DBG_LUT, "bstatic    : %d\n", param->bstatic);
    DEBUG_INFO(DBG_LUT, "auth       : %d\n", param->auth);
    DEBUG_INFO(DBG_LUT, "dablk      : %d\n", param->dablk);
    DEBUG_INFO(DBG_LUT, "sablk      : %d\n", param->sablk);
    DEBUG_INFO(DBG_LUT, "spa        : %d\n", param->spa);
    DEBUG_INFO(DBG_LUT, "age        : %d\n", param->age);
    DEBUG_INFO(DBG_LUT, "mac is 0x %x-%x-%x-%x-%x-%x \n", 
                    param->mac[0], param->mac[1], param->mac[2], 
                    param->mac[3], param->mac[4], param->mac[5]);      
#endif    
    return SUCCESS;
}

static int32 _rtl8307h_addrTbl_multicast_param2entry(rtk_table_list_t table, rtl8307h_tblasic_entry_t *entry, const rtl8307h_lut_multi_param_t* param)
{       
    memset(entry, 0, sizeof(rtl8307h_tblasic_entry_t));
    
    entry->val[0] = (uint32)param->eav_pri_en       |           \
                    (uint32)param->eav_pri << 1     |           \
                    (uint32)param->portmask<< 8     |           \
                    (uint32)param->crsvlan << 16    |           \
                    (uint32)0              << 17;    /* L3Entry = 0 */ 

    if (table == L2_TABLE)
    {
        entry->val[1] = ((uint32)param->mac[4] & 0xfe) >> 1 |       \
                        (uint32)param->mac[3] << 7      |           \
                        (uint32)param->mac[2] << 15     |           \
                        (uint32)param->mac[1] << 23     |           \
                        (uint32)(param->mac[0] & 0x1) << 31; /* should be 1 in bit 31 */

        entry->val[2] = ((uint32)param->mac[0] & 0xfe) >> 1 |      \
                        (uint32)param->ppi_fid << 7;   
    }
    else
    {
        entry->val[1] = ((uint32)param->mac[5] & 0xff)  |           \
                        (uint32)param->mac[4] << 8      |           \
                        (uint32)param->mac[3] << 16     |           \
                        (uint32)param->mac[2] << 24;

        entry->val[2] = ((uint32)param->mac[1] & 0xff)  |           \
                        (uint32)param->mac[0] << 8      |           \
                        (uint32)param->ppi_fid << 16;         
    }
    
    DEBUG_INFO(DBG_LUT, "entry val[0]: 0x%08x\n", entry->val[0]);
    DEBUG_INFO(DBG_LUT, "entry val[1]: 0x%08x\n", entry->val[1]);
    DEBUG_INFO(DBG_LUT, "entry val[2]: 0x%08x\n", entry->val[2]);
    
    return SUCCESS;
}



static int32 _rtl8307h_addrTbl_multicast_entry2param(uint32 hashAlg, rtk_table_list_t table, uint32 index, rtl8307h_lut_multi_param_t* param, const rtl8307h_tblasic_entry_t *entry)
{    
    memset(param, 0, sizeof(rtl8307h_lut_multi_param_t)); 
    
    param->eav_pri_en = (entry->val[0] >> 0) & 0x1;       
    param->eav_pri    = (entry->val[0] >> 1) & 0x7;  
    param->portmask   = (entry->val[0] >> 8) & 0xff; 
    param->crsvlan    = (entry->val[0] >> 16)& 0x1;        

    if (table == L2_TABLE)
    {
        param->mac[4]     = (entry->val[1] & 0x7f) << 1;     /* bit 15:9*/  
        param->mac[3]     = (entry->val[1] >> 7) & 0xff;
        param->mac[2]     = (entry->val[1] >> 15) & 0xff;
        param->mac[1]     = (entry->val[1] >> 23) & 0xff;  
        
        param->mac[0]     = ((entry->val[1] >> 31) & 0x1) |  \
                            ((entry->val[2] & 0x7f) << 1);      
        param->ppi_fid    = (entry->val[2] >> 7) & 0xfff; 

        if (hashAlg == 0)
            _rtl8307h_addrTbl_hash0_multi_restore(index, param);      /* restore bit 8:0*/
        else
            _rtl8307h_addrTbl_hash1_multi_restore(index, param);
    }
    else
    {
        param->mac[5]     = entry->val[1] & 0xff;     
        param->mac[4]     = (entry->val[1] >> 8) & 0xff;
        param->mac[3]     = (entry->val[1] >> 16) & 0xff;
        param->mac[2]     = (entry->val[1] >> 24) & 0xff;  

        param->mac[1]     = entry->val[2] & 0xff;     
        param->mac[0]     = (entry->val[2] >> 8) & 0xff;  
        param->ppi_fid    = (entry->val[2] >> 16) & 0xfff; 
    }
#if 0    
    DEBUG_INFO(DBG_LUT, "print lut multicast parameter:\n");
    DEBUG_INFO(DBG_LUT, "eav_pri_en : %d\n", param->eav_pri_en);
    DEBUG_INFO(DBG_LUT, "eav_pri    : %d\n", param->eav_pri);
    DEBUG_INFO(DBG_LUT, "portmask   : 0x%x\n", param->portmask);  
    DEBUG_INFO(DBG_LUT, "crsvlan    : %d\n", param->crsvlan);    
    DEBUG_INFO(DBG_LUT, "ppi_fid    : %d\n", param->ppi_fid);
    DEBUG_INFO(DBG_LUT, "mac is 0x %x-%x-%x-%x-%x-%x \n", 
                    param->mac[0], param->mac[1], param->mac[2], 
                    param->mac[3], param->mac[4], param->mac[5]);      
#endif    
    return SUCCESS;    
}

static int32 _rtl8307h_addrTbl_ip_param2entry(rtk_table_list_t table, rtl8307h_tblasic_entry_t *entry, const rtl8307h_lut_ipMulti_param_t* param)
{       
    memset(entry, 0, sizeof(rtl8307h_tblasic_entry_t));
        
    entry->val[0] = (uint32)param->eav_pri_en       |           \
                    (uint32)param->eav_pri << 1     |           \
                    (uint32)param->portmask<< 8     |           \
                    (uint32)param->crsvlan << 16    |           \
                    (uint32)1              << 17;    /* L3Entry = 1 */ 
    if (table == L2_TABLE)
    {
        entry->val[1] = ((uint32)param->sip[2] & 0xfe) >> 1 |       \
                        (uint32)param->sip[1] << 7      |           \
                        (uint32)param->sip[0] << 15     |           \
                        (uint32)param->dip[3] << 23     |           \
                        (uint32)(param->dip[2] & 0x1) << 31; 

        entry->val[2] = ((uint32)param->dip[2] & 0xfe) >> 1 |      \
                        (uint32)param->dip[1] << 7          |      \
                        ((uint32)param->dip[0]&0xf) << 15;  
    }
    else
    {
        entry->val[1] = ((uint32)param->sip[3] & 0xff)  |       \
                        (uint32)param->sip[2] << 8      |           \
                        (uint32)param->sip[1] << 16     |           \
                        (uint32)param->sip[0] << 24;                       

        entry->val[2] = ((uint32)param->dip[3] & 0xff)      |      \
                        (uint32)param->dip[2] << 8          |      \
                        (uint32)param->dip[1] << 16         |      \
                        ((uint32)param->dip[0]&0xf) << 24; 
    }
    
    DEBUG_INFO(DBG_LUT, "entry val[0]: 0x%08x\n", entry->val[0]);
    DEBUG_INFO(DBG_LUT, "entry val[1]: 0x%08x\n", entry->val[1]);
    DEBUG_INFO(DBG_LUT, "entry val[2]: 0x%08x\n", entry->val[2]);
    
    return SUCCESS;
}

static int32 _rtl8307h_addrTbl_ip_entry2param(uint32 hashAlg, rtk_table_list_t table, uint32 index, rtl8307h_lut_ipMulti_param_t* param, const rtl8307h_tblasic_entry_t *entry)
{                        
    memset(param, 0, sizeof(rtl8307h_lut_ipMulti_param_t)); 
  
    param->eav_pri_en = (entry->val[0] >> 0) & 0x1;       
    param->eav_pri    = (entry->val[0] >> 1) & 0x7;  
    param->portmask   = (entry->val[0] >> 8) & 0xff; 
    param->crsvlan    = (entry->val[0] >> 16)& 0x1;     

    if (table == L2_TABLE)       
    {
        param->sip[2]     = (entry->val[1] & 0x7f) << 1;     /* bit 15:9*/  
        param->sip[1]     = (entry->val[1] >> 7) & 0xff;
        param->sip[0]     = (entry->val[1] >> 15) & 0xff;
        param->dip[3]     = (entry->val[1] >> 23) & 0xff;  
        
        param->dip[2]     = ((entry->val[1] >> 31) & 0x1) |  \
                            ((entry->val[2] & 0x7f) << 1);      
        param->dip[1]     = (entry->val[2] >> 7) & 0xff; 
        param->dip[0]     = ((entry->val[2] >>15) & 0x0f) | 0xe0; 

        if (hashAlg == 0)
            _rtl8307h_addrTbl_hash0_ip_restore(index, param);      /* restore bit 8:0*/
        else  
            _rtl8307h_addrTbl_hash1_ip_restore(index, param);
    }
    else
    {
        param->sip[3]     = entry->val[1] & 0xff;     
        param->sip[2]     = (entry->val[1] >> 8) & 0xff;
        param->sip[1]     = (entry->val[1] >> 16) & 0xff;
        param->sip[0]     = (entry->val[1] >> 24) & 0xff;  

        param->dip[3]     = entry->val[2] & 0xff;    
        param->dip[2]     = (entry->val[2] >> 8) & 0xff;
        param->dip[1]     = (entry->val[2] >> 16) & 0xff;
        param->dip[0]     = ((entry->val[2]>> 24) & 0x0f) | 0xe0; 
    }
#if 0    
    DEBUG_INFO(DBG_LUT, "print ip multicast parameter:\n");
    DEBUG_INFO(DBG_LUT, "eav_pri_en : %d\n", param->eav_pri_en);
    DEBUG_INFO(DBG_LUT, "eav_pri    : %d\n", param->eav_pri);
    DEBUG_INFO(DBG_LUT, "portmask   : 0x%x\n", param->portmask);  
    DEBUG_INFO(DBG_LUT, "crsvlan    : %d\n", param->crsvlan);   
    
    DEBUG_INFO(DBG_LUT, "dip is 0x %x-%x-%x-%x \n", 
                    param->dip[0], param->dip[1], param->dip[2], param->dip[3]); 
    DEBUG_INFO(DBG_LUT, "sip is 0x %x-%x-%x-%x \n", 
                    param->sip[0], param->sip[1], param->sdip[2], param->sip[3]);
#endif    
    return SUCCESS;    
}

static int32 rtl8307h_addrTbl_entry2param(uint32 hashAlg, rtk_table_list_t tbl_type, uint32 index, rtl8307h_lut_param_t* param, const rtl8307h_tblasic_entry_t *entry)
{
    uint8 l3entry;
    uint8 multicast;
    
    l3entry   = (entry->val[0] >> 17) & 0x01;
    
    if (tbl_type == L2_TABLE)
        multicast = (entry->val[1] >> 31) & 0x01;
    else
        multicast = (entry->val[2] >> 8) & 0x01;
    
    memset(param, 0, sizeof(rtl8307h_lut_param_t));
    if (l3entry == 1)
        param->type = LUT_IPMULTI;
    else if (multicast == 1)
        param->type = LUT_MULTI;
    else 
    {    
        if ( ((entry->val[0] >> 8) & 0x1)  || /* static entry    */  \
             ((entry->val[0] >> 9) & 0x1)  || /* auth = 1        */  \
             ((entry->val[0] >> 10)& 0x1)  || /* dablk= 1        */  \
             ((entry->val[0] >> 11)& 0x1)  || /* sablk= 1        */
             ((entry->val[0] >> 15)& 0x3))    /* age != 0        */
        {
             param->type = LUT_UNICAST;
        }
        else
        {
            DEBUG_INFO(DBG_LUT, "Invalid entry\n");
            param->type = LUT_INVALID_ENTRY;
            return SUCCESS;
        }
        
    }
    
    switch (param->type) 
    {
        case LUT_IPMULTI:
            _rtl8307h_addrTbl_ip_entry2param(hashAlg, tbl_type, index, &param->val.ip, entry);
            break;
        case LUT_MULTI:
            _rtl8307h_addrTbl_multicast_entry2param(hashAlg, tbl_type, index, &param->val.multi, entry);
            break;
        case LUT_UNICAST:
            _rtl8307h_addrTbl_unicast_entry2param(hashAlg, tbl_type, index, &param->val.uni, entry);
            break;
        default:
            break;
    }

    param->index = index;
    return SUCCESS;
}

/*This function is used to replace function rtl8307h_addrTbl_setAsicEntry*/
int32 rtl8307h_addrTbl_uni_entry_set(uint32 hashAlg, const rtl8307h_lut_uni_param_t* param, rtl8307h_lut_4way_idx_t idx)
{
    uint32 index;
    rtl8307h_tblasic_entry_t entry;

    if (hashAlg == 0)
        index = _rtl8307h_lut_hash0_unicast_index_get(param, idx);
    else
        index = _rtl8307h_lut_hash1_unicast_index_get(param, idx);
         
    _rtl8307h_addrTbl_unicast_param2entry(L2_TABLE, &entry, param);

    table_write(RTL8307H_UNIT, L2_TABLE, index, entry.val);
          
    return SUCCESS;     
}

/*This function is used to test fpga */
int32 rtl8307h_addrTbl_uni_entry_force_set(uint32 hashAlg, uint32 index, const rtl8307h_lut_uni_param_t* param)
{
    rtl8307h_tblasic_entry_t entry;
         
    _rtl8307h_addrTbl_unicast_param2entry(L2_TABLE, &entry, param);

    table_write(RTL8307H_UNIT, L2_TABLE, index, entry.val);
          
    return SUCCESS;     
}

int32 rtl8307h_addrTbl_entry_delete(uint32 index)
{
    rtl8307h_tblasic_entry_t entry;
    memset(&entry, 0, sizeof(rtl8307h_tblasic_entry_t));
         
    table_write(RTL8307H_UNIT, L2_TABLE, index, entry.val);
          
    return SUCCESS;     
}


/*This function is used to replace function rtl8307h_addrTbl_getAsicEntry*/
int32 rtl8307h_addrTbl_uni_entry_get(uint32 hashAlg, uint32 index, rtl8307h_lut_uni_param_t* param)
{
    rtl8307h_tblasic_entry_t entry;

    table_read(RTL8307H_UNIT, L2_TABLE, index, entry.val);

    _rtl8307h_addrTbl_unicast_entry2param(hashAlg, L2_TABLE, index, param, &entry);
          
    return SUCCESS;     
}

int32 rtl8307h_addrTbl_multicast_entry_set(uint32 hashAlg, const rtl8307h_lut_multi_param_t* param, rtl8307h_lut_4way_idx_t idx)
{
    uint32 index;
    rtl8307h_tblasic_entry_t entry;

    if (hashAlg == 0)
        index = _rtl8307h_lut_hash0_multicast_index_get(param, idx);
    else
        index = _rtl8307h_lut_hash1_multicast_index_get(param, idx);
         
    _rtl8307h_addrTbl_multicast_param2entry(L2_TABLE, &entry, param);

    table_write(RTL8307H_UNIT, L2_TABLE, index, entry.val);
          
    return SUCCESS;     
}

int32 rtl8307h_addrTbl_multicast_entry_get(uint32 hashAlg, uint32 index, rtl8307h_lut_multi_param_t* param)
{
    rtl8307h_tblasic_entry_t entry;

    table_read(RTL8307H_UNIT, L2_TABLE, index, entry.val);

    _rtl8307h_addrTbl_multicast_entry2param(hashAlg, L2_TABLE, index, param, &entry);
          
    return SUCCESS;     
}

int32 rtl8307h_addrTbl_ip_entry_set(uint32 hashAlg, const rtl8307h_lut_ipMulti_param_t* param, rtl8307h_lut_4way_idx_t idx)
{
    uint32 index;
    rtl8307h_tblasic_entry_t entry;

    if (hashAlg == 0)
        index = _rtl8307h_lut_hash0_ip_index_get(param, idx);
    else
        index = _rtl8307h_lut_hash1_ip_index_get(param, idx);
         
    _rtl8307h_addrTbl_ip_param2entry(L2_TABLE, &entry, param);

    table_write(RTL8307H_UNIT, L2_TABLE, index, entry.val);
          
    return SUCCESS;     
}

int32 rtl8307h_addrTbl_ip_entry_get(uint32 hashAlg, uint32 index, rtl8307h_lut_ipMulti_param_t* param)
{
    rtl8307h_tblasic_entry_t entry;

    table_read(RTL8307H_UNIT, L2_TABLE, index, entry.val);

    _rtl8307h_addrTbl_ip_entry2param(hashAlg, L2_TABLE, index, param, &entry);
          
    return SUCCESS;     
}

int32 rtl8307h_addrTbl_group_entry_set(const rtl8307h_lut_group_param_t* param, rtl8307h_lut_8way_idx_t idx)
{
    uint32 index;
    rtl8307h_tblgroup_entry_t entry;

    index = _rtl8307h_lut_hash_group_index_get(param, idx);

    memset(&entry, 0, sizeof(rtl8307h_tblgroup_entry_t));
        
    entry.val[0] = param->portmask;
    entry.val[1] = param->dip[3]                   |           \
                   (uint32)param->dip[2] << 8      |           \
                   (uint32)param->dip[1] << 16      |           \
                   (uint32)(param->dip[0] & 0x0f) << 24;

    table_write(RTL8307H_UNIT, GROUP, index, entry.val);
          
    return SUCCESS;     
}

int32 rtl8307h_addrTbl_group_entry_get(uint32 index, rtl8307h_lut_group_param_t* param)
{
    rtl8307h_tblgroup_entry_t entry;

    table_read(RTL8307H_UNIT, GROUP, index, entry.val);

    param->portmask = entry.val[0] & 0xff;
    
    param->dip[3]   = entry.val[1] & 0xff;
    param->dip[2]   = (entry.val[1] >> 8) & 0xff;
    param->dip[1]   = (entry.val[1] >> 16) & 0xff;
    param->dip[0]   = ((entry.val[1]>> 24) & 0xf) | 0xe0;            
    
    return SUCCESS;     
}

/*This function is used to replace function rtl8307h_addrTbl_getAsicEntry*/
int32 rtl8307h_addrTbl_cam_entry_get(uint32 index, rtl8307h_lut_param_t* param)
{
    rtl8307h_tblasic_entry_t entry;
    
    memset(&entry, 0, sizeof(rtl8307h_tblasic_entry_t));
    memset(param, 0, sizeof(rtl8307h_lut_param_t));

    table_read(RTL8307H_UNIT, LUT_BCAM, index, entry.val);

    rtl8307h_addrTbl_entry2param(0, LUT_BCAM, index, param, &entry);              
    return SUCCESS;  
}

int32 rtl8307h_addrTbl_cam_entry_set(uint32 index, rtl8307h_lut_param_t* param)
{
    rtl8307h_tblasic_entry_t entry;
    
    memset(&entry, 0, sizeof(rtl8307h_tblasic_entry_t));

    switch (param->type) 
    {
        case LUT_IPMULTI:
            _rtl8307h_addrTbl_ip_param2entry(LUT_BCAM, &entry , &param->val.ip);
            break;
        case LUT_MULTI:
            _rtl8307h_addrTbl_multicast_param2entry(LUT_BCAM, &entry, &param->val.multi);
            break;
        case LUT_UNICAST:
            _rtl8307h_addrTbl_unicast_param2entry(LUT_BCAM, &entry, &param->val.uni);
            break;
        default:
            break;
    }

    table_write(RTL8307H_UNIT, LUT_BCAM, index, entry.val);
                  
    return SUCCESS;  
}

int32 rtl8307h_addrTbl_flush(rtl8307h_lut_flush_mode_t mode, rtl8307h_flush_param_t* param)
{   
    uint32 value;
    reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_FLUSH_CONTROL0, LUT_FLUSH_MODE, mode);
    
    switch (mode) 
    {
        case LUT_FLUSH_PORT_ONLY:   
            reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_FLUSH_CONTROL0, LUT_FLUSH_PMSK, param->portmask);           
            break;
        case LUT_FLUSH_PORT_CVID:
            reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_FLUSH_CONTROL0, LUT_FLUSH_PMSK, param->portmask);           
            reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_FLUSH_CONTROL0, LUT_FLUSH_VID, param->cvid);           
            break;
        case LUT_FLUSH_PORT_FID:
            reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_FLUSH_CONTROL0, LUT_FLUSH_PMSK, param->portmask);           
            reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_FLUSH_CONTROL0, LUT_FLUSH_FID, param->fid);   
            break;
        case LUT_FLUSH_ALL:
        default:
            break;
    }

    reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_FLUSH_CONTROL1, LUT_FLUSH_TRIGGER, 1);     

    do {
        reg_field_read(RTL8307H_UNIT, ADDRESS_TABLE_FLUSH_CONTROL1, LUT_FLUSH_TRIGGER, &value);         
    }while(value);
    
    return SUCCESS;  
}

int32 rtl8307h_addrTbl_search(lut_search_param_t *param, uint32 *found, uint32* hitaddr)
{  
    uint32 mac31_0;
    uint32 mac47_32;
    uint32 reg_val;

    reg_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL0, 0);
    reg_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL1, 0);
    reg_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL2, 0);
    
    if (param->defMac)
    {
        mac47_32= param->mac.octet[0] << 8 | param->mac.octet[1];
        mac31_0 = param->mac.octet[2] << 24 | param->mac.octet[3] << 16 | \
                  param->mac.octet[4] << 8 | param->mac.octet[5];        
                
        reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL0, LUT_SEARCH_MAC_47_32, mac47_32);          
        reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL1, LUT_SEARCH_MAC_31_0, mac31_0);                     
        reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL0, LUT_SEARCH_SPECIFY_MAC, 1);           
    }
    
    if (param->defPortNum)
    {             
        reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL0, LUT_SEARCH_PORT_NUM, param->portNum);                              
        reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL0, LUT_SEARCH_SPECIFY_PORT, 1);           
    }
    
    if (param->defFid)
    {             
        reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL2, LUT_SEARCH_SPECIFY_FID, 1);                              
        if (param->ulfid == 0) /* 0: hash with vid, 1: hash with fid */
        {
            reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL2, LUT_SEARCH_HASH_ALG, 0); 
            reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL2, LUT_SEARCH_VID, param->vid);                                  
            reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL2, LUT_SEARCH_FID, 0); 
        }
        else
        {
            reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL2, LUT_SEARCH_HASH_ALG, 1); 
            reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL2, LUT_SEARCH_VID, 0);                                  
            reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL2, LUT_SEARCH_FID, param->fid); 
        }
    }
    
    reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL2, LUT_SEARCH_IDX_START, param->startIdx);             
    
    reg_write(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL3, 1);

    do {
        reg_read(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_CONTROL3, &reg_val);
    }while(reg_val == 1);

    reg_field_read(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_RESULT, LUT_SEARCH_FOUND, found);  

    if (*found)
        reg_field_read(RTL8307H_UNIT, ADDRESS_TABLE_SEARCH_RESULT, LUT_SEARCH_HIT_ADDR, hitaddr);             
    else
        *hitaddr = 0;
        
    return SUCCESS;  
}

/*This function is used to replace function rtl8307h_addrTbl_getAsicEntry*/
int32 rtl8307h_addrTbl_entry_get(uint32 hashAlg, uint32 index, rtl8307h_lut_param_t* param)
{
    rtl8307h_tblasic_entry_t entry;
    
    memset(&entry, 0, sizeof(rtl8307h_tblasic_entry_t));
    memset(param, 0, sizeof(rtl8307h_lut_param_t));

    table_read(RTL8307H_UNIT, L2_TABLE, index, entry.val);

    rtl8307h_addrTbl_entry2param(hashAlg, L2_TABLE, index, param, &entry);              
    return SUCCESS;     
}

/*
@func uint32 | rtl8307h_addrTbl_hashAlg_get | get address table hash algorithm
@parm  void | 
@rvalue uint32 |hash algorithm  |
@comm 
*/
uint32 rtl8307h_addrTbl_hashAlg_get(void)
{   
    uint32 regVal;

    reg_field_read(RTL8307H_UNIT, ADDRESS_TABLE_LOOKUP_CONTROL, HASHALG, &regVal);

    return regVal;
}

/*
@func int32 | rtl8307h_addrTbl_hashAlg_set  | set address table hash algorithm
@parm  uint32 | hashAlg      | algorithm, 0 or 1
@rvalue int32 |FAILED   |
@rvalue int32 |SUCCESS  |
@comm 
*/
int32 rtl8307h_addrTbl_hashAlg_set(uint32 hashAlg)
{   
    return reg_field_write(RTL8307H_UNIT, ADDRESS_TABLE_LOOKUP_CONTROL, HASHALG, hashAlg);     
}

/*
@func int32 | rtl8307h_addrTbl_bktFull_trap | configure action for packets when address table bucket full
@parm  uint32 | enable   | 1: trap, 0: forward
@rvalue int32 |SUCCESS  |
@rvalue int32 |FAILED   |
@comm when four ways bucket full, address will not be auto learned by ASIC
*/
int32 rtl8307h_addrTbl_bktFull_trap(uint32 enable)
{
    return reg_field_write(RTL8307H_UNIT, SOURCE_MAC_LEARNING_CONTROL, BKTFULLACT, enable);
}

/*
@func int32 | rtl8307h_addrTbl_lru_enable   | enable LRU when bucket full
@parm  uint32 | enable   | 1: enable, 0: disable
@rvalue int32 |SUCCESS  |
@rvalue int32 |FAILED   |
@comm L2LRU enable, the least recently used entry would be overworitten by new un-learned source MAC
*/
int32 rtl8307h_addrTbl_lru_enable(uint32 enable)
{
    return reg_field_write(RTL8307H_UNIT, SOURCE_MAC_LEARNING_CONTROL, L2LRU, enable);
}

/*
@func int32 | rtl8307h_maclrnlimit_enable   | enable port MAC learning number limitation
@parm  uint32 | port      | port number [0, 24]
@rvalue int32 |SUCCESS  | 
@rvalue int32 | FAILED
@comm
*/
int32 rtl8307h_maclrnlimit_enable(uint32 port)
{
    int32 retval;

    if (port >= PN_PORT_END)
    {
        return -1;
    }
   
    retval = reg_field_write(RTL8307H_UNIT, PORT0_MAC_LIMIT_CONTROL + port, MACNUMCTL, 1);

    return retval;
}

/*
@func int32 | rtl8307h_maclrnlimit_disable  | disable port MAC learning number limitation
@parm  uint32 | port      | port number [0, 24]
@rvalue int32 |SUCCESS  | 
@rvalue int32 | FAILED
@comm
*/
int32 rtl8307h_maclrnlimit_disable(uint32 port)
{
    int32 retval;

    if (port >= PN_PORT_END)
    {
        return -1;
    }
   
    retval = reg_field_write(RTL8307H_UNIT, PORT0_MAC_LIMIT_CONTROL + port, MACNUMCTL, 0);

    return retval;
}


/*
@func int32 | rtl8307h_maclrnlimit_maxnum_set   | configure port MAC learning maximum number
@parm  uint32 | port      | port number [0, 24]
@parm  uint32 | maxNum      | maximun number
@rvalue int32 |SUCCESS  | 
@rvalue int32 | FAILED
@comm
*/
int32 rtl8307h_maclrnlimit_maxnum_set(uint32 port, uint32 maxNum)
{
    int32 retval;

    if (port >= PN_PORT_END)
    {
        return -1;
    }

    if (maxNum >= (1 << 14))
    {
        return -1;
    }
   
    retval = reg_field_write(RTL8307H_UNIT, PORT0_MAC_LIMIT_CONTROL + port, MAXMACNUM, maxNum);

    return retval;
}


/*
@func int32 | rtl8307h_maclrnlimit_maxnum_get   | get port MAC learning maximum number
@parm  uint32 | port      | port number [0, 24]
@parm  uint32* | maxNum      | maximun number
@rvalue int32 |SUCCESS  | 
@rvalue int32 | FAILED
@comm
*/

int32 rtl8307h_maclrnlimit_maxnum_get(uint32 port, uint32* maxNum)
{
    int32 retval;

    if (port >= PN_PORT_END)
    {
        return -1;
    }

    if (maxNum == NULL)
    {
        return -1;
    }
   
    retval = reg_field_read(RTL8307H_UNIT, PORT0_MAC_LIMIT_CONTROL + port, MAXMACNUM, maxNum);

    return retval;
}


/*
@func int32 | rtl8307h_maclrnlimit_curnum_get   | get port MAC current learning number
@parm  uint32 | port      | port number [0, 24]
@parm  uint32* | curNum      | current number
@rvalue int32 |SUCCESS  | 
@rvalue int32 | FAILED
@comm
*/
int32 rtl8307h_maclrnlimit_curnum_get(uint32 port, uint32* curNum)
{
    int32 retval;

    if (port >= PN_PORT_END)
    {
        return -1;
    }

    if (curNum == NULL)
    {
        return -1;
    }
   
    retval = reg_field_read(RTL8307H_UNIT, PORT0_MAC_LIMIT_CONTROL + port, CURMACNUM, curNum);

    return retval;
}


/*
@func int32 | rtl8307h_maclrnlimit_action_set   | configure action when port MAC learning reaches maximum number
@parm  uint32 | port      | port number [0, 24]
@parm  uint32 | action      | 0: drop packets; 1: forward without learning; 2: trap to CPU
@rvalue int32 |SUCCESS  | 
@rvalue int32 | FAILED
@comm
*/
int32 rtl8307h_maclrnlimit_action_set(uint32 port, uint32 action)
{
    int32 retval;

    if (port >= PN_PORT_END)
    {
        return -1;
    }
   
    retval = reg_field_write(RTL8307H_UNIT, PORT0_MAC_LIMIT_CONTROL + port, L2LIMACT, action);

    return retval;
}


/*
@func int32 | rtl8307h_maclrnlimit_action_get   | get action when port MAC learning reaches maximum number
@parm  uint32 | port      | port number [0, 24]
@parm  uint32* | action      | 0: drop packets; 1: forward without learning; 2: trap to CPU
@rvalue int32 |SUCCESS  | 
@rvalue int32 | FAILED
@comm
*/

int32 rtl8307h_maclrnlimit_action_get(uint32 port, uint32* action)
{
    int32 retval;

    if (port >= PN_PORT_END)
    {
        return -1;
    }

    if (action == NULL)
    {
        return -1;
    }
   
    retval = reg_field_read(RTL8307H_UNIT, PORT0_MAC_LIMIT_CONTROL + port, L2LIMACT, action);

    return retval;
}


/*
@func int32 | rtl8307h_maclrnlimit_cpuport_set  | configure CPU port mask when port MAC learning reaches maximum number and trap to CPU
@parm  uint32 | port      | 0: Reserved, 1: CPU port23; 2: CPU port24; 3: CPU port23 and CPU port24
@rvalue int32 |SUCCESS  | 
@rvalue int32 | FAILED
@comm
*/
int32 rtl8307h_maclrnlimit_cpuport_set(uint32 port)
{
    int32 retval;

    retval = reg_field_write(RTL8307H_UNIT, MAC_ADDRESS_NUMBER_CONSTRAIN_CONTROL, MCONSCPUMSK, port);

    return retval;
}


/*
@func int32 | rtl8307h_maclrnlimit_cpuport_get  | get CPU port mask when port MAC learning reaches maximum number and trap to CPU
@parm  uint32* | port      | 0: Reserved, 1: CPU port23; 2: CPU port24; 3: CPU port23 and CPU port24
@rvalue int32 |SUCCESS  | 
@rvalue int32 | FAILED
@comm
*/
int32 rtl8307h_maclrnlimit_cpuport_get(uint32* port)
{
    int32 retval;

    if (port == NULL)
    {
        return -1;
    }
    
    retval = reg_field_read(RTL8307H_UNIT, MAC_ADDRESS_NUMBER_CONSTRAIN_CONTROL, MCONSCPUMSK, port);

    return retval;
}


