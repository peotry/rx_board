/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     status.c
* Description:  DEV-4U BackBoard status manage
* Author    :   ruibin.zhang
* Modified  :
* Reviewer  :
* Date      :   2017-03-02
* Record    :
*
**********************************************************************/
#ifndef STATUS_H
#define STATUS_H

#include <stdbool.h>
#include "appGlobal.h"






typedef struct
{
    bool                    IPConifg;                       // 配置完成 1
    bool                    CommFinish;                     // 通信完成 1
}ST_UartCommFlag;

typedef struct
{
    VU8                     u8SlotID;                       //当前槽号
    VU16                    u16SubBoardplug;                //  1表示插入 0表示未插入 0~15位分别对应16个槽
    ST_UartCommFlag         stUartCommflag;                 //当前串口通信标识
}ST_WV_STATUS;



wvErrCode Status_Init(void);
ST_WV_STATUS * Status_GetStatusHandler(void);
VU8 Status_GetSlotID(void);
void Status_SetSlotID(VU8 u8SlotID);
VU16 Status_GetSubBoardPlugStatus(void);
void Status_SetSubBoardPlugStatus(VU16 u16SubBoardplug);
bool Status_CheckSubBoardPlugExist(SLOT_ID u8slotID);
bool Status_GetIPReqflag(void);
void Status_SetIPReqflag(bool IPReqflag);
bool Status_GetCommFinishflag(void);
void Status_SetCommFinishflag(bool CommFinish);

void Status_SetMac(U8 *u8MacAddr);
void Status_setIpAddr(U32 u32Ipaddr);


void Status_SetSystemTime(U32 u32CurrentTime);
#endif
