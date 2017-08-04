/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     NMS_Main_Process.h
* Description:  NMS Function Interface
* Author    :   jie.zhan
* Modified  :
* Reviewer  :
* Date      :   2017-03-28
* Record    :
*
**********************************************************************/

#ifndef _NMS_MAIN_PROCESS_H_
#define _NMS_MAIN_PROCESS_H_

#include "appGlobal.h"
#include "NetManager.h"

#define NMS_INVALID_COMMAND             (0xFFFFFFFF)
#define NMS_MAX_RETRY_TIMES             (6)

/********************************************************************/

#define NMS_SOCKET_ERROR                  (1)
#define NMS_NO_DATA_RECEIVED              (2)
#define NMS_OPEN_FILE_FAILED              (3)
#define NMS_NO_PROGRAM                    (4)
#define NMS_READ_ONLY                     (5)
#define NMS_OVERFLOW                      (6)
#define NMS_INVALID_FORMAT                (7)
#define NMS_INVALID_PARAMETER             (8)
#define NMS_SEND_FAILED                   (9)
#define NMS_CONNECT_FAILED                (10)
#define NMS_ILLEAGE_PID                   (11)
#define NMS_ILLEAGE_SERVICEID             (12)
#define NMS_ILLEAGE_PMTPID                (13)
#define NMS_ILLEAGE_PCRPID                (14)
#define NMS_REPEAT_ESPID                  (15)
#define NMS_REPEAT_ESECMPID               (16)
#define NMS_REPEAT_PSECMPID               (17)
#define NMS_REPEAT_EMMPID                 (18)
#define NMS_REPEAT_OTHERPID               (19)
#define NMS_REPEAT_SERVICEID              (20)
#define NMS_REPEAT_PMTPID                 (21)
#define NMS_REPEAT_PCRPID                 (22)
#define NMS_RECVD_FAILED                  (23)
#define NMS_MALLOC_FAILED                 (24)
#define NMS_WRONG_INDEX                   (25)
#define NMS_ERR_PARAMETER_SIZE_MISMATCH   (26)
#define NMS_ERR_WRONG_DATA                (27)
#define NMS_OTHER_ERR                     (28)
#define NMS_SYNC_ERR                      (29)
#define NMS_CRC_ERR                       (30)
#define NMS_WRITE_FLASH_ERR               (31)
#define NMS_UPGRADE_FAILED_ERR            (32)
#define NMS_WRONG_ARGUMENT                (33)
#define NMS_HAD_SELECT_THIS_PROGRAM       (34)
#define NMS_SET_FAILED                    (35)
#define NMS_TIME_OUT                      (36)
#define NMS_DELETE_INPUT_TS               (37)
#define NMS_HAD_SUPPORT_ONE_PROGRAM_ONLY  (38)
#define NMS_BYPASSTS                      (39)
#define NMS_WRONG_MACHINE_TYPE            (40)
#define NMS_WRONG_HARDWARE_VERSION        (41)
#define NMS_ACCEPT_ERR                    (42)
#define NMS_PASSWORD_ERR                  (43)
#define NMS_UPGRADE_LICENSE_ERR           (44)
#define NMS_PID_CONFLICT                  (45)
#define NMS_PSISI_TABLE_TYPE_ERR          (46)

/*******************************************************************************/

typedef int (*HandleExecute)(NMSConn *pstConn);
typedef int (*HandleResponse)(NMSConn *pstConn);

typedef struct
{
    U32             u32CMD;
    HandleExecute   pExe;
    HandleResponse  pRes;
}MgsComm_t;

typedef struct
{
    U8  *pu8Data;
    U32 u32Len;
}NMS_Payload_t;

typedef struct COMM_HEAD_T
{
    U32   dwDataType;
    U32   dwDataLength;
    U32   dwInallFrameNum;
    U32   dwFrameNo;
    U8    ucDataRet;
    U8    ucDataExt1;
    U8    ucDataExt2;
    U8    ucDataExt3;
}COMM_HEAD_t;

typedef struct
{
    U32   dwSyncFlag;
    U32   dwImageNun;
    U32   dwEraseNum;
    U8    ucHadMainBoardFlag;
    U8    ucDataExt1;
    U8    ucDataExt2;
    U8    ucDataExt3;
}DMPUpgrade_t;

/*******************************************************************
*Function:   NMS_HandleReceive
*Description:NMS总处理函数
*Input:      pstHttpConn 连接
*Output:     -
*Return:     sucess or error code
********************************************************************/
S32 NMS_HandleReceive(NMSConn *pstConn);

#endif/*_NMS_MAIN_PROCESS_H_*/


