/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2009 
* All rights reserved.
*
* Abstract: this header file defines DBUG message for RTL8307H.
*
* $Author:  $
*
* ---------------------------------------------------------------
*/

#ifndef RTL8307H_ASIC_DEBUG_H
#define RTL8307H_ASIC_DEBUG_H

#define DBG_MSG      /* DEFINED: enable debugging message */

/*=========== global debug message ==========*/
/* global debug level */
#define DBG_MASK_NULL        0
#define DBG_VERIF            (1 << 0)
#define DBG_INFO             (1 << 1)
#define DBG_WARN             (1 << 2)
#define DBG_ERR              (1 << 3)
#define DBG_MEMDUMP          (1 << 4)

/* Individual module debug mask */
#define DBG_NULL                 0
#define DBG_VLAN             (1 << 0)        /* vlan debug mask                     */
#define DBG_PRI              (1 << 1)        /* priority debug mask                 */
#define DBG_802DOT1X         (1 << 2)        /*802.1X network access control        */
#define DBG_RANGECHECK       (1 << 3)        /*TCP/UDP port range checking          */
#define DBG_PKTTRANSLATOR    (1 << 4)        /*packet translator debug              */
#define DBG_TRAFFIC          (1 << 5)        /*traffic isolation filter debug       */
#define DBG_PIE              (1 << 6)        /*PIE look up                          */
#define DBG_FLOWCLF          (1 << 7)        /*flow classification                  */
#define DBG_ACL              (1 << 8)        /*ACL debug                            */
#define DBG_PARSER           (1 << 9)        /*packet parser debug                  */
#define DBG_LEARNING         (1 << 10)       /*source mac learning debug            */
#define DBG_LUT              (1 << 11)       /*L2 table and foward table lookup     */
#define DBG_RMA              (1 << 12)       /*RMA                                  */
#define DBG_OAM              (1 << 13)       /*OAM Parser decision& oam multiplexer */
#define DBG_LINKAGGR         (1 << 14)       /*Link Aggregation                     */
#define DBG_LOOPDETECTION    (1 << 15)       /*Loop detection                       */
#define DBG_PKTGEN           (1 << 16)       /*packet generator                     */
#define DBG_TEMPLGEN         (1 << 17)
#define DBG_MIRROR           (1 << 18)       /*Mirror, sFlow and RSPAN              */
#define DBG_ATTACKPREV       (1 << 19)       /*Attack prevention                    */
#define DBG_ICMODEL          (1 << 20)       /*IC Model                             */
#define DBG_SPT              (1 << 21)       /*Spanning tree protocol               */
#define DBG_LOCK             (1 << 22)       /*lock mechanism                       */
#define DBG_MEMCTRL          (1 << 23)       /*memory controller                    */
#define DBG_PHY              (1 << 24)       /*phy access                           */
#define DBG_TESTCASE         (1 << 25)       /*for testing cases                    */
#define DBG_REGVERIFY        (1 << 26)       /*for testing cases                    */
#define DBG_TBLACCESS        (1 << 27)       /*for testing cases                    */
#define DBG_REG              (1 << 28)       /*register access                      */
#define DBG_PORT             (1 << 29)       /*port                                 */

/*to add your module debug mask here.....*/
#define DBG_MISC             (1 << 30)           /*for all trivial things*/

/* define debug level                   */
//#define DBG_MASK           DBG_MASK_NULL
#define DBG_MASK				DBG_ERR|DBG_WARN|DBG_INFO|DBG_VERIF
#define DBG_MODULE_MASK		DBG_PKTTRANSLATOR|DBG_PRI|DBG_MIRROR|DBG_PARSER
//|DBG_REG|DBG_PORT|DBG_PHY

//#define DBG_MODULE_MASK		0xFFFFFFFF



    /* global debug message define */
#if ((DBG_MASK) & DBG_VERIF)
#define DEBUG_VERIF(type,fmt, args...) \
	do {if(type & (DBG_MODULE_MASK)) rtlglue_printf("[%s-%d]-verif-: " fmt "\n", __FUNCTION__, __LINE__, ## args);} while (0)
#else
#define DEBUG_VERIF(type, fmt, args...) do {} while(0)
#endif

#if ((DBG_MASK) & DBG_INFO)
#define DEBUG_INFO(type,fmt, args...) \
	do {if(type & (DBG_MODULE_MASK)) printf("[%s-%d]-info-: " fmt "\n", __FUNCTION__, __LINE__, ## args);} while (0)
#else
#define DEBUG_INFO(type,fmt, args...) do {} while(0)
#endif

#if ((DBG_MASK) & DBG_WARN)
#define DEBUG_WARN(type,fmt, args...) \
	do {if(type & (DBG_MODULE_MASK)) rtlglue_printf("[%s-%d]-warn-: " fmt "\n", __FUNCTION__, __LINE__, ## args);} while (0)
#else
#define DEBUG_WARN(type,fmt, args...) do {} while(0)
#endif


#if ((DBG_MASK) & DBG_ERR)
#define DEBUG_ERR(type,fmt, args...) \
	do {if(type & (DBG_MODULE_MASK)) rtlglue_printf("[%s-%d]-error-: " fmt "\n", __FUNCTION__, __LINE__, ## args);} while (0)
#else
#define DEBUG_ERR(type,fmt, args...) do {} while(0)
#endif

#if ((DBG_MASK) & DBG_MEMDUMP)
#define DEBUG_MEMDUMP(type, start, size, strHeader) \
    do {if(type & (DBG_MODULE_MASK))   \
		rtlglue_printf("[%s-%d]-memdump-:\n", __FUNCTION__, __LINE__);  /  memDump(start,size,strHeader);} while (0)
#else
#define DEBUG_MEMDUMP(type, start, size, strHeader) do {} while(0)
#endif

#ifndef RTL8307H_DEBUG
#define ASSERT(expr) do {if (expr); } while (0)
#else
#define ASSERT(expr) \
        if((expr) == 0) {                   \
            rtlglue_printf( "\033[33;41m%s:%d: assert(%s)\033[m\n",           __FILE__,__LINE__,#expr);       \
        }
#endif
#ifndef RTL8307H_DEBUG
#define PRINT_ERROR(fmt, args...) do {} while (0)
#else
#define PRINT_ERROR(fmt, args...) \
	do {rtlglue_printf("[%s-%d]-ERROR-: " fmt "\n", __FUNCTION__, __LINE__, ## args);} while (0)
#endif    


#endif /*header file*/
