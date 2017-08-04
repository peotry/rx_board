/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     UartComm.h
* Description:  serial communications SubBoard with BackBoard
* Author    :   ruibin.zhang
* Modified  :
* Reviewer  :
* Date      :   2017-03-03
* Record    :
*
**********************************************************************/
#ifndef UARTCOMM_H
#define UARTCOMM_H

#include "appGlobal.h"

#define UARTCOMM_DATA_MAX_LEN       (0x230)
#define UARTCOMM_ST_MIN_LEN         12              //数据传输的最短长度
#define UARTCOMM_RECV_DELAY         100*1000        //串口通信等待接收延时
#define UARTCOMM_RECV_DELAY_NUM     2               //串口通信等待接收等待次数

#define UARTCOMM_IP_DATA_LEN        4               //串口通信IP传输数据长度

#define UARTCOMM_ST_HEADER          0x47474747      //规定头 数据
#define UARTCOMM_CRC_32             0x04C11DB7      //CRC生成多项式

typedef struct
{
    U32     u32Header;                              //传输头验证 0x47474747
    U8      u8SlotID;
    U8      u8CMD;
    U16     u16Length;                              // data 数据长度
    U8      pu8Data[UARTCOMM_DATA_MAX_LEN];
    U32     u32StructCRC;
}ST_WV_UARTCOMM;

typedef enum{
    UARTCOMM_NONE_CMD           =   0x00,
    UARTCOMM_SUCCESS_CMD        =   0x01,
    UARTCOMM_REQ_IP_CMD         =   0x02,
    UARTCOMM_REP_IP_CMD         =   0x03,
    UARTCOMM_REQ_BREAK_CMD      =   0x04,
    UARTCOMM_REP_BREAK_CMD      =   0x05,
    UARTCOMM_REQ_TIME_CMD       =   0x06,
    UARTCOMM_REP_TIME_CMD       =   0x07,

}UARTCOMM_CMD;

void UartComm_init();
void UartComm_GetTime();
void UartComm_GetIpAddr(U8 u8slot);

wvErrCode UartComm_Start(void);

#endif
