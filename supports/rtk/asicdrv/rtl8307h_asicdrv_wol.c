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
 * $Revision: 8252 $
 * $Date: 2010-01-29 14:04:02 +0800 (Fri, 29 Jan 2010) $
 *
 * Purpose : RTL8307H switch high-level API for RTL8307H
 * Feature : 
 *
 */

#include <rtk_types.h>
#include <rtk_error.h>

#include <rtl8307h_types.h>
#include <rtl8307h_reg_struct.h>
#include <rtl8307h_debug.h>

#include <rtl8307h_asicdrv_wol.h>

/* Update the CRC for transmitted and received data using
   the CCITT 16bit algorithm (X^16 + X^12 + X^5 + 1).
   */
uint16 rtl8307h_wol_ccitt_crc16(uint8 *message, uint8* mask, uint16 len) 
{
    uint16 crc = 0xffff;
    uint8 crc_old[16];
    uint8 crc_new[16];
    uint8 rxd[8];
    uint8 tmp[15];
        
    int16 i, j;
        
    for (i = 0; i < len; i++) 
    { 
        if((mask[i/8] & (1 << (i % 8))) == 0)
            continue;
            
        for (j = 0; j<16; j++)
            crc_old[j] = (crc >> j) & 0x1;
            
        for (j = 0; j< 8; j++)
            rxd[j] = (message[i] >> j) & 0x1;
            
        tmp[15] = rxd[0] ^ crc_old[15];
        tmp[14] = rxd[1] ^ crc_old[14];
        tmp[13] = rxd[2] ^ crc_old[13];
        tmp[12] = rxd[3] ^ crc_old[12];
        tmp[11] = rxd[4] ^ crc_old[11];
        tmp[10] = rxd[5] ^ crc_old[10];
        tmp[9]  = rxd[6] ^ crc_old[9];
        tmp[8]  = rxd[7] ^ crc_old[8];
  
        crc_new[15] = crc_old[7] ^                       tmp[11] ^                      tmp[15];
        crc_new[14] = crc_old[6] ^               tmp[10] ^                       tmp[14]       ;
        crc_new[13] = crc_old[5] ^        tmp[9] ^                       tmp[13]               ;
        crc_new[12] = crc_old[4] ^ tmp[8] ^                      tmp[12] ^              tmp[15];
        crc_new[11] = crc_old[3] ^                                               tmp[14]       ;
        crc_new[10] = crc_old[2] ^                                       tmp[13]               ;
        crc_new[9]  = crc_old[1] ^                               tmp[12]                       ;
        crc_new[8]  = crc_old[0] ^                       tmp[11] ^                      tmp[15];
        crc_new[7]  =                            tmp[10] ^                      tmp[14]^tmp[15];
        crc_new[6]  =                     tmp[9] ^                       tmp[13]^tmp[14]       ;
        crc_new[5]  =              tmp[8] ^                      tmp[12]^tmp[13]               ;
        crc_new[4]  =                                            tmp[12]                       ;
        crc_new[3]  =                                    tmp[11] ^                      tmp[15];
        crc_new[2]  =                            tmp[10] ^                       tmp[14]       ;
        crc_new[1]  =                     tmp[9] ^                        tmp[13]              ;
        crc_new[0]  =              tmp[8] ^                        tmp[12]                     ;
            
        crc = 0;
        for (j = 0; j<16; j++)
            crc |= crc_new[j] << j;
    } 
    return crc; 
}
