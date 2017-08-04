/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     NetManager.h
* Description:  Net Manage
* Author    :   jie.zhan
* Modified  :
* Reviewer  :
* Date      :   2017-03-28
* Record    :
*
**********************************************************************/

#ifndef _NETMANAGER_H
#define _NETMANAGER_H

#include "appGlobal.h"
#include "DS2432.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#define NMS_PORT   			(30)
#define INVALID_SOCKET      0xFFFFFFFF  
#define SOCKET_ERROR        -1

/* success */
#define NMS_SUCCESS 								DS2432_SUCCESS

/* NMG 错误码定义 */
#define ERROR_NMG_START                             (0x001C0000)
#define ERROR_NMG_ACCEPT                            (ERROR_NMG_START)
#define ERROR_NMG_RECV                              (ERROR_NMG_START + 1)
#define ERROR_NMG_SEND                              (ERROR_NMG_START + 2)
#define ERROR_NMG_MALLOC                            (ERROR_NMG_START + 3)
#define ERROR_NMG_PARAM                             (ERROR_NMG_START + 4)
#define ERROR_NMG_USERNAME                          (ERROR_NMG_START + 5)
#define ERROR_NMG_PASSWORD                          (ERROR_NMG_START + 6)
#define ERROR_NMG_BOUNDARY                          (ERROR_NMG_START + 7)
#define ERROR_NMG_FILE                              (ERROR_NMG_START + 8)
#define ERROR_NMG_SYNC                              (ERROR_NMG_START + 9)
#define ERROR_NMG_CRC                               (ERROR_NMG_START + 10)
#define ERROR_MNG_MACHINE_TYPE                      (ERROR_NMG_START + 11)
#define ERROR_NMG_HW_VER                            (ERROR_NMG_START + 12)
#define ERROR_NMG_SW_FUN_VER                        (ERROR_NMG_START + 13)
#define ERROR_NMG_LOADER                            (ERROR_NMG_START + 14)
#define ERROR_NMG_FLASH_ADDR                        (ERROR_NMG_START + 15)
#define ERROR_NMG_SB_USE_FLAG                       (ERROR_NMG_START + 16)
#define ERROR_NMG_SB_CONFIG                         (ERROR_NMG_START + 17)
#define ERROR_NMG_SB_TYPE                           (ERROR_NMG_START + 18)
#define ERROR_NMG_REBOOTING                         (ERROR_NMG_START + 19)
#define ERROR_NMG_FACTORYSETING                     (ERROR_NMG_START + 20)
#define ERROR_NMG_PARAMETER_SIZE_MISMATCH           (ERROR_NMG_START + 21)
#define ERROR_NMG_WRONG_INDEX                       (ERROR_NMG_START + 22)
#define ERROR_NMG_PID_CONFLICT                      (ERROR_NMG_START + 23)
#define ERROR_NMG_CHIP_ID                           (ERROR_NMG_START + 24)
#define ERROR_NMG_EMPTY_BYPASS_TS_TO_CI             (ERROR_NMG_START + 25)
#define ERROR_NMG_WAITING_READY                     (ERROR_NMG_START + 26)
#define ERROR_NMG_EIT_CHANNEL_OVERFLOW              (ERROR_NMG_START + 27)
#define ERROR_NMG_PARAM_CONFIC_FAIL                 (ERROR_NMG_START + 28)
#define ERROR_NMG_BITRATE_EXCEPTION                 (ERROR_NMG_START + 29)
#define ERROR_NMG_PROGRAM_HAS_DEST                  (ERROR_NMG_START + 30)
#define ERROR_NMG_NO_LOGGED_IN                      (ERROR_NMG_START + 31)
#define ERROR_NMG_PARAM_CONFIC_OVER                 (ERROR_NMG_START + 32)
#define ERROR_NMG_UPLOADLOGO_SUC                    (ERROR_NMG_START + 33)
#define ERROR_NMG_UPLOADLOGO_FAIL                   (ERROR_NMG_START + 34)
#define ERROR_NMG_ERASELOGO_SUC                     (ERROR_NMG_START + 35)

#define CONNECTION_TYPE_HTTP   1 // 连接类型 http
#define CONNECTION_TYPE_WVNM   2 // 连接类型 wellav PC 网管

/* TX & RX buffer sizes for all socket sends & receives in our sss app*/
#define NMS_RX_BUF_SIZE         (10240)
#define NMS_PER_PACKET          (4096)
#define NMS_TMPE_BUFFER_LEN     (1500)
#define MAX_TRY_READ_TIMES      (100) // 连续100次接收数据失败,返回失败

/*****************************************************************************
    以下用于描述连接
 ******************************************************************************/
#define NMS_DISCONNECT (8)

typedef enum{
    READY, COMPLETE, CLOSE,
}eConnState;

/* Here we structure to manage sss communication for a single connection */
typedef struct NMS_SOCKET
{
  eConnState eState;
  int        nSocket;
  int        nClose;
  U32        u32ReadLen;
  U32        u32WriteLen;
  S8        *ps8ReadPos; /* position we've read up to */
  S8        *ps8WritePos; /* position we've written up to */
} NMSConn;

S32 NonblockingReceive(S32 nSocketFd, char *pcBuf, unsigned int u32MaxLen);
void NMS_ResetConnection(NMSConn *pstHttpConn);
S32 NMS_SendData(S32 nSocket, const char *pcDataBuff, S32 nDataLen);
void NMS_SocketServerTask(void);
U32 NMG_ReceiveData(int nSocketFd, char *pcDataBuff, int nDataLen);
S32 NMGServer_Start(void);

#endif /*_NETMANAGER_H*/

