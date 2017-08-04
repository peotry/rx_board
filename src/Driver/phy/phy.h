/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     IPconfig.h
* Description:  IP manage and configure
* Author    :   ruibin.zhang
* Modified  :
* Reviewer  :
* Date      :   2017-03-04
* Record    :
*
**********************************************************************/
#ifndef INCLUDE_DRIVERS_PHY
#define INCLUDE_DRIVERS_PHY

#include "appGlobal.h"
#include "err/wv_err.h"

#define GEM1_NET_CONFIG_BASEADRESS  0xE000B000
#define GEM2_NET_CONFIG_BASEADRESS  0xE000C000

#define MAC0REG_HIGHT_ADDR          0x109
#define MAC0REG_LOWER_ADDR          0x10a
#define MAC1REG_HIGHT_ADDR          0x209
#define MAC1REG_LOWER_ADDR          0x20a

#define XST_EMAC_MEMORY_SIZE_ERROR  1001L   /* Memory space is not big enough
                                                * to hold the minimum number of
                                                * buffers or descriptors */
#define XST_EMAC_MEMORY_ALLOC_ERROR 1002L   /* Memory allocation failed */
#define XST_EMAC_MII_READ_ERROR     1003L   /* MII read error */
#define XST_EMAC_MII_BUSY           1004L   /* An MII operation is in progress */
#define XST_EMAC_OUT_OF_BUFFERS     1005L   /* Driver is out of buffers */
#define XST_EMAC_PARSE_ERROR        1006L   /* Invalid driver init string */
#define XST_EMAC_COLLISION_ERROR    1007L   /* Excess deferral or late*/

#define XST_SUCCESS                     0L
#define XST_FAILURE                     1L

#define XEMACPS_NWSR_MDIOIDLE_MASK     0x00000004 /**< PHY management idle */
#define XEMACPS_NWCTRL_OFFSET          0x00000000 /**< Network Control reg */
#define XEMACPS_NWCFG_OFFSET           0x00000004 /**< Network Config reg */
#define XEMACPS_NWSR_OFFSET            0x00000008 /**< Network Status reg */

#define XEMACPS_PHYMNTNC_OP_MASK       0x40020000  /**< operation mask bits */
#define XEMACPS_PHYMNTNC_OP_R_MASK     0x20000000  /**< read operation */
#define XEMACPS_PHYMNTNC_OP_W_MASK     0x10000000  /**< write operation */
#define XEMACPS_PHYMNTNC_ADDR_MASK     0x0F800000  /**< Address bits */
#define XEMACPS_PHYMNTNC_REG_MASK      0x007C0000  /**< register bits */
#define XEMACPS_PHYMNTNC_DATA_MASK     0x00000FFF  /**< data bits */
#define XEMACPS_PHYMNTNC_PHYAD_SHIFT_MASK   23  /**< Shift bits for PHYAD */
#define XEMACPS_PHYMNTNC_PHREG_SHIFT_MASK   18  /**< Shift bits for PHREG */

#define XEMACPS_PHYMNTNC_OFFSET        0x00000034 /**< Phy Maintaince reg */

typedef enum {
    ETH0,
    ETH1
} EthType_enum;

void PHY_RegMap(void);
wvErrCode PHY_SetLocalMac(U8 * pu8MAC, EthType_enum emEthType);
BOOL PHY_SetEth(EthType_enum emEthType);
void PHY_Init(void);
void PHY_SetIP(U32 u32IP);
void PHY_GetFreeIP(U32 * pu32Ip);

#endif 
