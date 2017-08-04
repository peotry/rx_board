/******************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName uart.h
* Description : UART module
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-28
* Record    :
*
******************************************************************************/
#ifndef _UART_H_
#define _UART_H_

#include "appGlobal.h"

//TODO reference to .dts 根据设备树生成，更改以下
#define UART0_DEV_NAME "/dev/ttyPS0"
#define UART1_DEV_NAME "/dev/ttyPS1"

#define UART_RD_LENGTH  (140)

typedef enum{
    WV_UART_SUCCESS                         =               0X0,

    WV_UART_PARAM_ERR                       =               0X1,

    WV_UART_OPEN_ERR                        =               0X2,
    WV_UART_ALREADY_OPEN_ERR                =               0X3,

    WV_UART_GET_OLDCONFIG_ERR               =               0X4,
    WV_UART_SET_CONFIG_ERR                  =               0x5,

}UARTErrCode;

void uart1_Flush();
int uart_OpenDev(const char *dev_name);
UARTErrCode uart_Set(int fd, int speed, int data_bits, int stop_bits, char parity);
UARTErrCode uart_Read(int fd, U8 *pReadBuf, S32 * ps32Readlen);
UARTErrCode uart_Write(int fd, U8 *pWriteBuf, S32 s32Writelen);

UARTErrCode uart1_Init();
UARTErrCode uart1_Read(U8 *pReadBuf, S32 * ps32Readlen);
UARTErrCode uart1_Write(U8 *pWriteBuf, S32 s32Writelen);


#endif
