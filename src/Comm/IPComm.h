/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     IPComm.h
* Description:  IP communication
* Author    :   ruibin.zhang
* Modified  :
* Reviewer  :
* Date      :   2017-03-06
* Record    :
*
**********************************************************************/
#ifndef __IPCOMM_H__
#define __IPCOMM_H__

#include "appGlobal.h"

#define IP_CRC_32                   0x04C11DB7  //CRC生成多项式

#define IPCOMM_DATA_MAX_LEN        (1400)      //IP通信传输最大长度
#define IP_ST_MIN_LEN               20          //IP数据最短长度
#define IPCOMM_CRC_LEN               4




typedef struct
{
    U8      u8SlotID;
    U8      u8Res;
    U16     u16CMD;
    U32     u32CurTansNum;
    U32     u32SumTansNum;
    U32     u32Length;
    U8      pu8Data[IPCOMM_DATA_MAX_LEN];
    U32     u32StructCRC;
}ST_WV_IPCOMM;



typedef enum{
    IPCOMM_NONE_CMD             =   0x00,
    IPCOMM_SUCCESS_CMD          =   0x01,
    IPCOMM_DATA_ERROR           =   0x02,

    //BB & Sub
    IPCOMM_SEND_RESOUR          =   0x101,
    IPCOMM_SEND_DATA            =   0x102,

    //TSP
    IPCOMM_SEND_ADD_TS          =   0x103,
    IPCOMM_SEND_ADD_PROG        =   0x104,
    IPCOMM_SEND_ADD_PID         =   0x105,
    IPCOMM_SEND_DEL_TS          =   0x106,
    IPCOMM_SEND_DEL_PROG        =   0x107,
    IPCOMM_SEND_DONE_FLAG       =   0x108,
    IPCOMM_SEND_CLEAR_INPUT_TS  =   0x109,
    IPCOMM_SEND_CLEAR_OUTPUT_TS =   0x10A,
    IPCOMM_CHANGE_PROGRAM       =   0x10B,//编码板用
    IPCOMM_CHANGE_PID           =   0x10C,//编码板用
    IPCOMM_REPLACE_PID_SRC_INFO =   0x10C,
    IPCOMM_PROG_SET_DESCRAMBLE  =   0x10D,
    IPCOMM_PROG_UNSET_DESCRAMBLE=   0x10E,
    
    IPCOMM_SEND_SUBPLUGFLAG     =   0x201,
    IPCOMM_SEND_IP              =   0x202,

    IPCOMM_SEND_UPGRADE         =   0xf01,
    IPCOMM_REQ_REBOOT           =   0Xf02,

}IPCOMM_CMD;




wvErrCode IPServer_Start(void);
wvErrCode IPComm_SendCmd2Slot(U32 u32Slot, U32 u32cmd);
wvErrCode IPComm_SendData2Slot(U32 u32Slot, U32 u32cmd, U8 *u8Data, U32 u32DataLen);
wvErrCode IPComm_SendCmd(U32 u32IpAddr, U32 u32cmd);
wvErrCode IPComm_SendData(U32 u32IpAddr, U32 u32cmd, U8 *u8Data, U32 u32DataLen);
wvErrCode IPComm_ChangeIP(U8 u8Slot, U8 *pu8Data);

#endif
