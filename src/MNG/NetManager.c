/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     NetManager.c
* Description:  Net Manage Function
* Author    :   jie.zhan
* Modified  :
* Reviewer  :
* Date      :   2017-03-28
* Record    :
*
**********************************************************************/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include "socket_server.h"
#include "socket_client.h"
#include "NMS_Main_Process.h"
#include "NetManager.h"
#include "LicenseParse.h"
#include "DS2432.h"
#include "WV_log.h"

static NMSConn s_stNMSConn;

/*****************************************************************************
  Function:     NMS_ResetConnection
  Description:  重置
  Input:        pstHttpConn             --- 通信句柄
  Output:       none
  Return:       none
  Modify:       jie.zhan
*****************************************************************************/
void NMS_ResetConnection(NMSConn *pstHttpConn)
{
    if (NULL == pstHttpConn)
    {
        return;
    }

    memset(pstHttpConn, 0, sizeof(NMSConn));

    pstHttpConn->nSocket     = INVALID_SOCKET;
    pstHttpConn->eState      = READY;
}

/*****************************************************************************
  Function:     HandleAcceptConnection
  Description:  接受链接
  Input:        nSocket             
                pstConn         
                nType
  Output:       none
  Return:       ERROR_NMG_ACCEPT
                NMS_SUCCESS
  Modify:       jie.zhan
*****************************************************************************/
static S32 HandleAcceptConnection(int nSocket, NMSConn *pstConn, S32 nType)
{
    S32                 nTmpSocket = 0;
    S32                 nAddrLen   = 0;
    struct sockaddr_in  stSrcAddr;

    nAddrLen = sizeof(stSrcAddr);

    if (pstConn->nSocket != INVALID_SOCKET)
    {
        close(pstConn->nSocket);
        NMS_ResetConnection(pstConn);
    }

    if ((nTmpSocket = accept(nSocket, (struct sockaddr *)&stSrcAddr, &nAddrLen)) < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "accept failed.");
        return ERROR_NMG_ACCEPT;
    }

    pstConn->nSocket = nTmpSocket;

#ifdef WEB_SESSION_MANAGEMENT
    if (CONNECTION_TYPE_HTTP == nType)
    {
        (void)snprintf((char*)s_aru8CurrentIp, sizeof(s_aru8CurrentIp), "%s", inet_ntoa(stSrcAddr.sin_addr));
    }
#endif

#ifdef ENABLE_PC_NMS
    if (CONNECTION_TYPE_WVNM == nType)
    {
        memcpy(&gNMSAddr, &stSrcAddr, sizeof(stSrcAddr));
    }
#endif

    return NMS_SUCCESS;
}

/*****************************************************************************
  Function:     SendDataPerPacket
  Description:  封装按包发送数据的函数
  Input:        nSocket             
                pcDataBuff           --- 数据buffer 
                nDataLen             --- 数据长度
  Output:       none
  Return:       ERROR_NMG_PARAM      --- 入参错误
                ERROR_NMG_SEND       --- 发数据错误
                NMS_SUCCESS          --- 发数据成功
  Modify:       jie.zhan
*****************************************************************************/
static S32 SendDataPerPacket(S32 nSocket, const char *pcDataBuff, S32 nDataLen)
{
    S32  nRet           = 0;
    S32  nHadSend       = 0;
    S32  nTryTime       = 0;
    const S32  nTimeout = 5000; //5 锟斤拷

    // CHECK PARAMS
    if ((pcDataBuff == NULL) || (nDataLen < 0) || (nSocket < 0))
    {
        return ERROR_NMG_PARAM;
    }
    while((nHadSend < nDataLen) && (nTryTime < nTimeout))
    {
        nRet = send(nSocket, (char *)(pcDataBuff + nHadSend), (nDataLen - nHadSend), 0);
        if(nRet == SOCKET_ERROR)
        {
            usleep(64 * 1000);
            nTryTime += 64;
        }
        else if(nRet > 0)
        {
            nHadSend += nRet;
        }
        else
        {
            usleep(32 * 1000);
            nTryTime += 32;
        }
        
        usleep(2 * 1000);
        nTryTime += 2;
    }

    if (nHadSend < nDataLen)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "ERROR_NMG_SEND.");
        return ERROR_NMG_SEND;
    }

    return NMS_SUCCESS;
}

/*****************************************************************************
  Function:     NonblockingReceive
  Description:  非阻塞接收数据
  Input:        nSocketFd           
                pcBuf                --- 接收数据buffer 
                u32MaxLen            --- 接收数据最大长度
  Output:       none
  Return:       recv返回值
  Modify:       jie.zhan
*****************************************************************************/
S32 NonblockingReceive(S32 nSocketFd, char *pcBuf, unsigned int u32MaxLen)
{
    return recv(nSocketFd, pcBuf, u32MaxLen, MSG_DONTWAIT);
}

/*****************************************************************************
  Function:     NMG_ReceiveData
  Description:  接收数据
  Input:        nSocketFd           
                pcDataBuff           --- 接收数据buffer 
                nDataLen             --- 长度
  Output:       none
  Return:       u32HadRecvLen        --- 接收长度
  Modify:       jie.zhan
*****************************************************************************/
U32 NMG_ReceiveData(S32 nSocketFd, char *pcDataBuff, S32 nDataLen)
{
    U32 u32TryReadTimes = 0;
    U32 u32HadRecvLen   = 0;
    U32 u32TotalLen     = nDataLen;
    U32 u32DlyMS        = 0;
    S32 nRet            = 0;

    if (0 > nSocketFd || NULL == pcDataBuff || 0 >= nDataLen)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "invalid input");
        return 0;
    }

    while (u32TryReadTimes < MAX_TRY_READ_TIMES && u32HadRecvLen < u32TotalLen)
    {
        nRet = NonblockingReceive(nSocketFd, pcDataBuff + u32HadRecvLen, u32TotalLen - u32HadRecvLen);
        if (nRet > 0)
        {
            u32HadRecvLen += (U32)nRet;
            u32TryReadTimes = 0;
            u32DlyMS = 2;
        }
        else if (SOCKET_ERROR == nRet)
        {
            u32DlyMS = 100;
            u32TryReadTimes++;
        }
        else
        {
            u32DlyMS = 20;
            u32TryReadTimes++;
        }
        
        usleep(u32DlyMS * 1000);
    }

    return u32HadRecvLen;
}

/*****************************************************************************
  Function:     NMS_SendData
  Description:  传输数据
  Input:        nSocket             
                pcDataBuff           --- 传输数据buffer 
                nDataLen             --- 传输数据长度
  Output:       none
  Return:       ERROR_NMG_PARAM      --- 入参错误
                ERROR_NMG_SEND       --- 传输数据失败
                NMS_SUCCESS          --- 成功
  Modify:       jie.zhan
*****************************************************************************/
S32 NMS_SendData(S32 nSocket, const char *pcDataBuff, S32 nDataLen)
{
    // VARS
    S32     nRet     = 0;
    S32     nHadSend = 0;
    U32     u32SendLenPerPacket = 0;

    // CHECK PARAMS
    if ((pcDataBuff == NULL) || (nDataLen < 0) || (nSocket < 0))
    {
        return ERROR_NMG_PARAM;
    }

    while(nHadSend < nDataLen)
    {
        if ((nDataLen - nHadSend) > NMS_PER_PACKET)
        {
            u32SendLenPerPacket = NMS_PER_PACKET;
        }
        else
        {
            u32SendLenPerPacket = (nDataLen - nHadSend);
        }
        nRet = SendDataPerPacket(nSocket, (char *)(pcDataBuff + nHadSend), u32SendLenPerPacket);
        if (nRet == NMS_SUCCESS)
        {
            nHadSend += u32SendLenPerPacket;
        }
        else
        {
            return ERROR_NMG_SEND;
        }
        
        if (nHadSend < nDataLen)
        {
            usleep(8 * 1000);
        }
    }

    if (nHadSend < nDataLen)
    {
        return ERROR_NMG_SEND;
    }

    return NMS_SUCCESS;
}

/*****************************************************************************
  Function:     NMS_SocketServerTask
  Description:  网管总处理函数完成SOCKET初始化、绑定、监听是否有连接或有数据
  Input:        none
  Output:       none
  Return:       none
  Modify:       jie.zhan
*****************************************************************************/
void NMS_SocketServerTask(void)
{
    S32    nNMSListen   = 0;
    S32    nMaxSocket   = 0;
    S32    nRet         = 0;
    BOOL   bConnected   = FALSE;
    
    struct sockaddr_in stNMSAddr;
    fd_set stReadfds;
    struct timeval stTime;

    while(1)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "[NMS_SocketServerTask]start.");

        if ((nNMSListen = socket(AF_INET, SOCK_STREAM, 0)) < 0)     /* NMS */
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Socket creation failed.");
            usleep(10000);
            continue;
        }

        stNMSAddr.sin_family      = AF_INET;
        stNMSAddr.sin_port        = htons(NMS_PORT);
        stNMSAddr.sin_addr.s_addr = INADDR_ANY;

        if ((bind(nNMSListen, (struct sockaddr *)&stNMSAddr, sizeof(stNMSAddr))) < 0)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Bind failed.");
            usleep(10000);
            continue;
        }

        /*
        * Sockets primer, continued...
        * The listen socket is a socket which is waiting for incoming connections.
        * This call to listen will block (i.e. not return) until someone tries to
        * connect to this port.
        */
        if ((listen(nNMSListen, 1)) < 0)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Listen failed.");
            usleep(10000);
            continue;
        }

        NMS_ResetConnection(&s_stNMSConn);

        while(1)
        {
            /*
            * For those not familiar with sockets programming...
            * The select() call below basically tells the TCPIP stack to return
            * from this call when any of the events I have expressed an interest
            * in happen (it blocks until our call to select() is satisfied).
            *
            * In the call below we're only interested in either someone trying to
            * connect to us, or data being available to read on a socket, both of
            * these are a read event as far as select is called.
            *
            * The sockets we're interested in are passed in in the stReadfds
            * parameter, the format of the stReadfds is implementation dependant
            * Hence there are standard MACROs for setting/reading the values:
            *
            *   FD_ZERO  - Zero's out the sockets we're interested in
            *   FD_SET   - Adds a socket to those we're interested in
            *   FD_ISSET - Tests whether the chosen socket is set
            */
            bConnected = FALSE;
            FD_ZERO(&stReadfds);   
            FD_SET(nNMSListen, &stReadfds);/* 锟斤拷锟侥硷拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷 nHttpListen 锟斤拷锟斤拷锟斤拷一锟斤拷锟铰碉拷锟侥硷拷锟斤拷锟斤拷锟斤拷 */

            if (s_stNMSConn.nSocket != INVALID_SOCKET)
            {
                FD_SET(s_stNMSConn.nSocket, &stReadfds);

                nMaxSocket = max(nMaxSocket, s_stNMSConn.nSocket + 1);
            }

            stTime.tv_sec  = 0;
            stTime.tv_usec = 0;
            nRet = select(nMaxSocket, &stReadfds, NULL, NULL, &stTime);

            /* tcp nms */
            if (FD_ISSET(nNMSListen, &stReadfds))
            {
                bConnected = TRUE;

                nRet = HandleAcceptConnection(nNMSListen, &s_stNMSConn, CONNECTION_TYPE_WVNM);
                if(NMS_SUCCESS != nRet)
                {
                    LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "HandleAcceptConnection WVNM failed");
                }
            }

            if ((s_stNMSConn.nSocket != INVALID_SOCKET) && FD_ISSET(s_stNMSConn.nSocket, &stReadfds))
            {
                bConnected = TRUE;

                //FPGA_DisableOCMMonitor();

                nRet = NMS_HandleReceive(&s_stNMSConn);
                if (NMS_SUCCESS != nRet)
                {
                    LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "NMS_HandleReceive Failed. 0x%08x", nRet);
                }

                //FPGA_EnableOCMMonitor();
            }
            
            if (!bConnected)
            {
                usleep(200000);
            }
            else
            {
                usleep(30000);
            }
        }
    }
}

/*****************************************************************************
  Function:     NMGServer_Start
  Description:  启动授权功能线程
  Input:        none
  Output:       none
  Return:       WV_ERR_COMM_PTHREAD_CREAT       ---  失败
                WV_SUCCESS                      ---  成功
  Author:       jie.zhan
*****************************************************************************/
S32 NMGServer_Start(void)
{
    S32 s32Return = 0;
    pthread_t NMGServer_thread;

    s32Return = pthread_create(&NMGServer_thread, NULL, (void*)NMS_SocketServerTask, NULL);
    if(-1 == s32Return)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "NMGServer communication thread create error :%s(errno: %d)",strerror(errno),errno);
        return WV_ERR_COMM_PTHREAD_CREAT;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "NMGServer communication create successfully!");
    }

    s32Return = pthread_detach(NMGServer_thread);
    if(-1 == s32Return)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "NMGServer communication thread detach error :%s(errno: %d)",strerror(errno),errno);
        return WV_ERR_COMM_PTHREAD_CREAT;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "NMGServer communication pthread detach successfully!");
    }

    return WV_SUCCESS;
}

