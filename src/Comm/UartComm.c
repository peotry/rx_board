/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     UartComm.c
* Description:  serial communications SubBoard with BackBoard
* Author    :   ruibin.zhang
* Modified  :
* Reviewer  :
* Date      :   2017-03-03
* Record    :
*
**********************************************************************/
#include <pthread.h>
#include <errno.h>
#include <stdbool.h>
#include <termios.h>
#include "UartComm.h"
#include "wv_err.h"
#include "WV_log.h"
#include "uart.h"
#include "FPGA.h"
#include "UART/uart.h"
#include "status.h"
#include "tools.h"
#include "phy.h"
#include "resource.h"


/*****************************************************************************
  Function:     UartComm_init
  Description:  uart init
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
void UartComm_init()
{
     if(uart1_Init() != WV_UART_SUCCESS)
     {
          return;
     }

}


/*****************************************************************************
  Function:     UartComm_sendData
  Description:  send uart date
  Input:        U8 u8SlotID  -- self slot id
                U8 u8cmd  -- cmd 
                U8 *u8Data -- data point
                U16 u16DataLen  -- data len
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
void UartComm_sendData(U8 u8SlotID, U8 u8cmd, U8 *u8Data, U16 u16DataLen)
{
    ST_WV_UARTCOMM stUartComm;
    U8 * pWriteData =   NULL;

    if( (0 != u16DataLen)&&(NULL == u8Data))
    {
       LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "param err , dataLen > 0 while data point null\r\n");
    }

    stUartComm.u32Header = UARTCOMM_ST_HEADER;
    stUartComm.u8SlotID  = u8SlotID;
    stUartComm.u8CMD     = u8cmd;
    
    stUartComm.u16Length = u16DataLen;
    if(0 != u16DataLen)
    {
        memcpy(&stUartComm.pu8Data[0], u8Data, u16DataLen);
    }
    
    stUartComm.u32StructCRC = Common_CRCCalculate(&stUartComm.u8SlotID, (4+stUartComm.u16Length), UARTCOMM_CRC_32);


    pWriteData =  malloc(stUartComm.u16Length + UARTCOMM_ST_MIN_LEN);
    memset(pWriteData, 0, (stUartComm.u16Length + UARTCOMM_ST_MIN_LEN));
    memcpy(pWriteData, &stUartComm, (stUartComm.u16Length + UARTCOMM_ST_MIN_LEN - 4));
    memcpy((pWriteData + stUartComm.u16Length + UARTCOMM_ST_MIN_LEN - 4), &stUartComm.u32StructCRC, sizeof(U32));
    
    uart1_Write(pWriteData, (stUartComm.u16Length + UARTCOMM_ST_MIN_LEN));
    
    free(pWriteData);
}




/*****************************************************************************
  Function:     UartComm_ResponseBreak
  Description:  串口通信回复挂断信号请求
  Input:        U8 u8SlotID     -   当前槽号
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void UartComm_RequestBreak(U8 u8SlotID)
{
    UartComm_sendData(u8SlotID, UARTCOMM_REQ_BREAK_CMD, NULL, 0);
}

/*****************************************************************************
  Function:     UartComm_ResponseIP
  Description:  串口通信回复IP请求
  Input:        U8 u8SlotID     -   当前槽号
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void UartComm_RequestIP(U8 u8SlotID)
{
    UartComm_sendData(u8SlotID, UARTCOMM_REQ_IP_CMD, NULL, 0);
}


/*****************************************************************************
  Function:     UartComm_HeaderCheck
  Description:  接收数据头检测
  Input:        U32 u32Header   -   头数据
  Output:       none
  Return:       true    -   头正确
                false   -   头错误
  Author:       ruibin.zhang
*****************************************************************************/
static bool UartComm_HeaderCheck(U32 u32Header)
{
    if(UARTCOMM_ST_HEADER == u32Header)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*****************************************************************************
  Function:     UartComm_ParseData
  Description:  解析验证Uart comm的数据
  Input:        U8 *pDataBuf    -   解析数据的句柄
                S32 s32Datalen  -   解析的数据长度
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
wvErrCode UartComm_ParseData(U8 * pDataBuf, S32 s32Datalen, U8 u8slot, ST_WV_UARTCOMM  * pstUartComm)
{
    if((NULL == pDataBuf) || (NULL == pstUartComm))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Param NULL");
        return WV_ERR_PARAMS;
    }
    if(s32Datalen < UARTCOMM_ST_MIN_LEN)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Receive serial data too less, %d", s32Datalen);
        return WV_ERR_COMM_UART_RECV_MISS;
    }

    U32     u32RecvDataCRC  = 0;

    memset(pstUartComm, 0, sizeof(ST_WV_UARTCOMM));

    //header check
    pstUartComm->u32Header = *((U32 *)pDataBuf);
    if(false == UartComm_HeaderCheck(pstUartComm->u32Header))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Receive serial header error, %x", pstUartComm->u32Header);
        return WV_ERR_COMM_UART_RECV_HEADER;
    }

    //Slot check
    pstUartComm->u8SlotID = *(pDataBuf + sizeof(pstUartComm->u32Header));
    if(u8slot != pstUartComm->u8SlotID)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Receive serial slotID error [%d:%d]", pstUartComm->u8SlotID,u8slot);
        return WV_ERR_COMM_UART_RECV_SLOT;
    }

    //len Check
    pstUartComm->u16Length = (U16)(*(pDataBuf + 6));
    if(s32Datalen != (pstUartComm->u16Length + UARTCOMM_ST_MIN_LEN))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Receive serial data len error, %d", s32Datalen);
        return WV_ERR_COMM_UART_RECV_MISS;
    }

    //CRC Check
    //pstUartComm->u32StructCRC = (U32)(*(pDataBuf + 8 + pstUartComm->u16Length));
    memcpy(&(pstUartComm->u32StructCRC), (pDataBuf + 8 + pstUartComm->u16Length),4);
    u32RecvDataCRC = Common_CRCCalculate(pDataBuf + 4, 4 + pstUartComm->u16Length, UARTCOMM_CRC_32);
    if(pstUartComm->u32StructCRC != u32RecvDataCRC)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Receive serial data CRC error [%d:%d]", pstUartComm->u32StructCRC,u32RecvDataCRC);
        return WV_ERR_COMM_UART_RECV_MISS;
    }

    //CMD Parse
    pstUartComm->u8CMD = *(pDataBuf + 5);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "Receive serial CMD : 0x%x", pstUartComm->u8CMD);

    //DATA Parse
    memcpy(pstUartComm->pu8Data, (pDataBuf + 8), pstUartComm->u16Length);

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     UartComm_RecvData
  Description:  串口通信接收数据，接收最多尝试5次。每次接收完解析数据，数据正确，停止接收。
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void UartComm_RecvData(U8 u8slot, ST_WV_UARTCOMM  * pstUartComm)
{
    S32         cnt             =   0;
    wvErrCode   ret             =   WV_SUCCESS;
    S32 s32UartComm_Readlen     =   0;
    U8  pUartComm_ReadBuf[UART_RD_LENGTH];

    for(cnt = 0; cnt < UARTCOMM_RECV_DELAY_NUM; cnt++ )
    {
        s32UartComm_Readlen = 0;
        memset(pUartComm_ReadBuf, 0, UART_RD_LENGTH);

        uart1_Read(pUartComm_ReadBuf, &s32UartComm_Readlen);
        if(s32UartComm_Readlen > 0)
        {
            //parse pUartComm_ReadBuf
            ret = UartComm_ParseData(pUartComm_ReadBuf, s32UartComm_Readlen, u8slot, pstUartComm);
            if(WV_SUCCESS == ret)
            {
                break;
            }
        }
        usleep(UARTCOMM_RECV_DELAY);
    }
}

/*****************************************************************************
  Function:     UartComm_GetTime
  Description:  1.get ipaddr from back board
                2.set self ip to the system
                3.update related resources info
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.haung
*****************************************************************************/
void UartComm_GetIpAddr(U8 u8slot)
{
    ST_WV_UARTCOMM  stUartComm;
    U32 *pu32Data;

    while(1)
    {
        if(!Status_GetIPReqflag())
        {
             UartComm_RequestIP(u8slot);
             UartComm_RecvData(u8slot, &stUartComm);
             
             if((UARTCOMM_REP_IP_CMD == stUartComm.u8CMD))
             {
                 pu32Data = (U32 *)stUartComm.pu8Data;
                 
                 LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "BackBoard ip: %d.%d.%d.%d \r\n"
                                     , (*pu32Data>>24)&0xff
                                     , (*pu32Data>>16)&0xff
                                     , (*pu32Data>>8)&0xff
                                     , (*pu32Data)&0xff);

                 Resource_setBackBoardIpAddr(*pu32Data);
                 pu32Data++;
                 LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "self ip: %d.%d.%d.%d \r\n"
                                      , (*pu32Data>>24)&0xff
                                      , (*pu32Data>>16)&0xff
                                      , (*pu32Data>>8)&0xff
                                      , (*pu32Data)&0xff);

                 PHY_SetIP(*pu32Data);
                 Resource_setSelfIpAddr(*pu32Data);
                 
                 Status_SetIPReqflag(true);
             }

        }

#ifdef MAINTIAN_UART_COMM
        if (Status_GetIPReqflag())
        {
            break;
        }
#else
        if(!Status_GetCommFinishflag() && Status_GetIPReqflag())
        {
            UartComm_RequestBreak(u8slot);
            
            UartComm_RecvData(u8slot, &stUartComm);
            if(UARTCOMM_REP_BREAK_CMD == stUartComm.u8CMD)
            {
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "Response break up!");
                Status_SetCommFinishflag(true);
   
                break;
            }
        }
#endif
    }
  
}



/*****************************************************************************
  Function:     UartComm_GetTime
  Description:  get time from back board through uart and set to the system
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.haung
*****************************************************************************/
void UartComm_GetTime()
{
    U32 *pu32Data;
    ST_WV_UARTCOMM  stUartComm;

    U8 u8Slot = Status_GetSlotID();

    U32 timeoutCnt = 0;

    while(1)
    {
        UartComm_sendData(u8Slot, UARTCOMM_REQ_TIME_CMD, NULL, 0);
        UartComm_RecvData(u8Slot, &stUartComm);

        if((UARTCOMM_REP_TIME_CMD == stUartComm.u8CMD))
        {
            pu32Data = (U32 *)stUartComm.pu8Data;

            LOG_PRINTF(LOG_LEVEL_ALL, LOG_MODULE_COMM,"get time %d\r\n", pu32Data);

            Status_SetSystemTime(*pu32Data);
            break;
        }

        if (timeoutCnt > 40)
        {
            LOG_PRINTF(LOG_LEVEL_ALL, LOG_MODULE_COMM,"get time failed, time out cnt %d\r\n", timeoutCnt);
            break;
        }
        uart1_Flush();
        timeoutCnt++;
    }
}


/*****************************************************************************
  Function:     UartComm_Service
  Description:  串口通信线程 对已插入的子板 循环通信
                1 判断是否需要对已插入子板进行串口通信
                2 切换相应槽号的串口
                3 等待接收串口数据，等待0.5s
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void UartComm_Service()
{
    U8 u8slot                   =   0;

    ST_WV_UARTCOMM  stUartComm;                 //解析后的结构体数据存储在这里


    //init uart1
    if(uart1_Init() != WV_UART_SUCCESS)
    {
        return;
    }
    u8slot = Status_GetSlotID();

    while(1)
    {
        //TODO may can use state machine
        if(!Status_GetIPReqflag())
        {
            UartComm_RequestIP(u8slot);
            UartComm_RecvData(u8slot, &stUartComm);
            if((4 == stUartComm.u16Length) && (UARTCOMM_REP_IP_CMD == stUartComm.u8CMD))
            {
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM,
                            "BackBoard response ip : 0x%x", stUartComm.pu8Data);
                PHY_SetIP( *((U32 *)stUartComm.pu8Data) );
                Status_SetIPReqflag(true);
            }
            uart1_Flush();
        }

        if(!Status_GetCommFinishflag() && Status_GetIPReqflag())
        {
            UartComm_RequestBreak(u8slot);
            UartComm_RecvData(u8slot, &stUartComm);
            if(UARTCOMM_REP_BREAK_CMD == stUartComm.u8CMD)
            {
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "Response break up!");
                Status_SetCommFinishflag(true);

                break;//exit thread
            }
            uart1_Flush();
        }
    }

}

/*****************************************************************************
  Function:     UartComm_Start
  Description:  启动串口通信
  Input:        none
  Output:       none
  Return:       WV_ERR_FAILURE         --  失败
                WV_SUCCESS             --  成功
  Author:       ruibin.zhang
*****************************************************************************/
wvErrCode UartComm_Start(void)
{
    int res = 0;
    pthread_t UartComm_thread;

    res = pthread_create(&UartComm_thread, NULL, (void*)UartComm_Service, NULL);
    if(-1 == res)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "serial communication thread create error :%s(errno: %d)",strerror(errno),errno);
        return WV_ERR_COMM_PTHREAD_CREAT;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "serial communication create successfully!");
    }

    res = pthread_detach(UartComm_thread);
    if(-1 == res)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "serial communication thread detach error :%s(errno: %d)",strerror(errno),errno);
        return WV_ERR_COMM_PTHREAD_CREAT;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "serial communication pthread detach successfully!");
    }

    return WV_SUCCESS;
}

