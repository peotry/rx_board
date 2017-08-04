/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     IPComm.c
* Description:  IP communication
* Author    :   ruibin.zhang
* Modified  :
* Reviewer  :
* Date      :   2017-03-06
* Record    :
*
**********************************************************************/
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "IPComm.h"
#include "WV_log.h"
#include "Socket/socket_server.h"
#include "Socket/socket_client.h"
#include "tools.h"
#include "resource.h"
#include "status.h"
#include "TSConstruct.h"
#include "wv_err.h"
#include "upgrade.h"
#include "phy.h"


extern volatile int socketSfd;
extern BOOL g_isUpgrading;

U8 u8recData[2800] = {0};
U32 u32RecLen = 0;

U8 *p_u8Data = NULL;
U32 u32shouldRecPackIndex = 0;




void debgPrint()
{
    int i = 0;
    for(i=0; i<2800; i++)
    {
        printf(" %d ", u8recData[i]);
       if( ((i+1)%20)==0)
       {
            printf("\r\n");
       }
    }

    printf("\r\n");
}


/*****************************************************************************
  Function:     IPComm_LoadData
  Description:  ���ṹ������װ�ص�����ָ��
  Input:        ST_WV_IPCOMM stIPComm     -   ����ͨ�Žṹ��
  Output:       none
  Return:       U8 * pu8Data                  -   ���ڴ���������
  Author:       ruibin.zhang
*****************************************************************************/
void IPComm_LoadData(ST_WV_IPCOMM stIPComm, U8 * pWriteData, U32 * pu32Writelen)
{
    if(NULL == pWriteData)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Param NULL");
        return ;
    }

    U32  u32datalen     =   0;              //����� u8Data ����
    U32  u32totallen    =   0;              //�ܴ��䳤��
    
    U32  u32remainder   =   0;

    // ����ṹ��
    u32remainder = stIPComm.u32Length % 4;
    if(u32remainder != 0)
    {
        u32remainder = 4 - u32remainder;
    }
    u32datalen  = stIPComm.u32Length + u32remainder;
    u32totallen = u32datalen + IP_ST_MIN_LEN;

    memset(pWriteData, 0, (u32totallen));
    memcpy(pWriteData, &stIPComm, (u32totallen - 4));
    memcpy((pWriteData + u32totallen - 4), &stIPComm.u32StructCRC, sizeof(U32));

    * pu32Writelen = u32totallen;
}

/*****************************************************************************
  Function:     IPComm_SetCommST
  Description:  ���ݲ�ͬ������ýṹ��
  Input:        U8 u8SlotID                 -       �ۺ�
                IPCOMM_CMD u8CMD            -       ����
  Output:       ST_WV_IPCOMM stIPComm     -   ����ͨ�Žṹ��
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void IPComm_SetCommST(ST_WV_IPCOMM * pstIPComm, U8 u8SlotID, IPCOMM_CMD u16CMD)
{
    if(NULL == pstIPComm)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Param NULL");
        return ;
    }

    pstIPComm->u8SlotID  = u8SlotID;
    pstIPComm->u16CMD    = u16CMD;

    switch(u16CMD)
    {
        case IPCOMM_SEND_RESOUR:
            pstIPComm->u32Length = 5;
            //TODO send resource st
            memcpy(pstIPComm->pu8Data, "text", 4);
            break;
        default:
            pstIPComm->u32Length = 0;
            break;
    }

    pstIPComm->u32StructCRC = Common_CRCCalculate(&(pstIPComm->u8SlotID), (16 + pstIPComm->u32Length), IP_CRC_32);

}

void IPComm_setStruct(ST_WV_IPCOMM * pstIPComm, IPCOMM_CMD u16CMD,U32 u32CurTansNum,U32 u32SumTansNum)
{
    pstIPComm->u8SlotID = Status_GetSlotID();
    pstIPComm->u16CMD = u16CMD;
    pstIPComm->u32CurTansNum = u32CurTansNum;
    pstIPComm->u32SumTansNum =u32SumTansNum;
}



void IPComm_SendTo(S32 connect_fd, IPCOMM_CMD ipCmd, U8 *u8Data, U32 u32DataLen, U32 u32CurTansNum, U32 u32SumTansNum)
{
    ST_WV_IPCOMM stIPCommRep;
    
    U8  *   pWriteData  =   NULL;
    U32     u32WriteLen =   0;
   

    if(u32DataLen > IPCOMM_DATA_MAX_LEN)
    {
        printf("*** over flow datalen > 1400\r\n");
        return;
    }

    if((0 != u32DataLen) && (NULL == u8Data))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "bad param, u32DataLen>0 while data point null\n");
    }

    pWriteData = malloc(IPCOMM_DATA_MAX_LEN + IP_ST_MIN_LEN);
    
    if(NULL == pWriteData)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "malloc error: %s(errno: %d)\n",strerror(errno),errno);
        return;
    }

    memset(&stIPCommRep, 0, sizeof(ST_WV_IPCOMM));
    

    IPComm_setStruct(&stIPCommRep,ipCmd,u32CurTansNum, u32SumTansNum);


    stIPCommRep.u32Length = u32DataLen;


    if(u32DataLen > 0)
    {
         memcpy(&stIPCommRep.pu8Data[0], u8Data, u32DataLen);
    }
    
    stIPCommRep.u32StructCRC = Common_CRCCalculate(&(stIPCommRep.u8SlotID), (IP_ST_MIN_LEN - IPCOMM_CRC_LEN + stIPCommRep.u32Length), IP_CRC_32);
    
    IPComm_LoadData(stIPCommRep, pWriteData, &u32WriteLen);


    if(-1 == send(connect_fd, pWriteData, u32WriteLen,0))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "send to client error: %s(errno: %d)\n",strerror(errno),errno);
        return ;
    }

    if(NULL != pWriteData)
    {
        free(pWriteData);
    }

}




/*****************************************************************************
  Function:     IPComm_Request_SendResource
  Description:  IPͨ����������Դ��Ϣ
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void IPComm_SendResource(S32 connect_fd)
{
    IPComm_SendTo(connect_fd, IPCOMM_SEND_RESOUR, NULL, 0, 0, 0);
}


/*****************************************************************************
  Function:     IPComm_ResponseSuccess
  Description:  IPͨ�Ŷ�����Ļ�Ӧ��ȷ����
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void IPComm_ResponseSuccess(S32 connect_fd)
{
    IPComm_SendTo(connect_fd, IPCOMM_SUCCESS_CMD, NULL, 0, 0, 1);
}


/*****************************************************************************
  Function:     IPComm_ResponseError
  Description:  IPͨ�Ŷ�����Ļ�Ӧ���ݴ���
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void IPComm_ResponseError(S32 connect_fd)
{
    IPComm_SendTo(connect_fd, IPCOMM_DATA_ERROR, NULL, 0, 0, 1);
}


/*****************************************************************************
  Function:     IPComm_Response
  Description:  IPͨ�Ŷ�����Ļ�Ӧ
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void IPComm_Response(S32 connect_fd, ST_WV_IPCOMM * pstIPComm)
{
    
    if((NULL == pstIPComm))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Param NULL");
        return ;
    }

    switch(pstIPComm->u16CMD)
    {
        case IPCOMM_NONE_CMD:
        {
            IPComm_ResponseError(connect_fd);
            break;
        }
        case IPCOMM_SEND_RESOUR:
        {
            //FIX ruibin.zhang Need check
            Resource_setSlotResource(pstIPComm->u8SlotID, &pstIPComm->pu8Data[0]);
            IPComm_ResponseSuccess(connect_fd);
            break;
        }

        case IPCOMM_SEND_CLEAR_OUTPUT_TS:
        {
            IPComm_ResponseSuccess(connect_fd);
            TSP_ProcessClearDestSlot(pstIPComm->u8SlotID);
            break;
        }
        
        case IPCOMM_SEND_DATA:
        {
            memcpy(&u8recData[u32RecLen], &pstIPComm->pu8Data[0], pstIPComm->u32Length);
            
            IPComm_ResponseSuccess(connect_fd);
            u32RecLen += pstIPComm->u32Length;
            if(pstIPComm->u32CurTansNum == (pstIPComm->u32SumTansNum-1) )
            {
               debgPrint();
            }

            break;
         } 
         case IPCOMM_SEND_UPGRADE:
         {
            if(pstIPComm->u32CurTansNum == 0)
            {
                g_isUpgrading = true;
                printf("#####receive upgrade total packet %d\r\n", pstIPComm->u32SumTansNum);
                u32RecLen = 0;

                p_u8Data = malloc(IPCOMM_DATA_MAX_LEN*pstIPComm->u32SumTansNum);
                if(p_u8Data == NULL)
                {
                    printf("***IPCOMM UPGRADE CMD malloc fail\r\n");
                }
            }
            if(p_u8Data != NULL)
            {
                memcpy(p_u8Data+u32RecLen, &pstIPComm->pu8Data[0], pstIPComm->u32Length);
            }

            u32RecLen += pstIPComm->u32Length;
            if(pstIPComm->u32CurTansNum != u32shouldRecPackIndex)
            {
                g_isUpgrading = false;
                printf("##debug## IPCOMM_SEND_UPGRADE receive packet index error sould rec %d, rec %d\r\n", u32shouldRecPackIndex,  pstIPComm->u32CurTansNum);
                free(p_u8Data);
                p_u8Data = NULL;
            }

            u32shouldRecPackIndex++;
            IPComm_ResponseSuccess(connect_fd);
            
            if(pstIPComm->u32CurTansNum == (pstIPComm->u32SumTansNum-1) )
            {

                g_isUpgrading = false;
                printf("###receive upgrade file finish\r\n");
                upgrade_RecieveFile(p_u8Data, u32RecLen);

                upgrade_UpgradeSystem((const U8*)"/var/volatile/upgradefile");

                /*�ͷ���Դ*/
                free(p_u8Data);
                p_u8Data = NULL;

            }    
            
            break;  
         }
         case IPCOMM_SEND_SUBPLUGFLAG:
         {
             U16 u16SubBoardplug = 0;

             memcpy(&u16SubBoardplug, pstIPComm->pu8Data, 2);
             LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "Get subplugflag %x", u16SubBoardplug);

             Status_SetSubBoardPlugStatus(u16SubBoardplug);
             Resource_Refresh();
             IPComm_ResponseSuccess(connect_fd);
             break;
         }

        case IPCOMM_SEND_IP:
        {
            IPComm_ResponseSuccess(connect_fd);
            IPComm_ChangeIP(pstIPComm->u8SlotID, &pstIPComm->pu8Data[0]);
            break;
        }
         
        default:
        {
            //LOG_PRINTF
            IPComm_ResponseError(connect_fd);
            break;
        }
    }
}


/*****************************************************************************
  Function:     IPComm_ParseData
  Description:  ������֤IP comm������
  Input:        U8 *pDataBuf    -   �������ݵľ��
                S32 s32Datalen  -   ���������ݳ���
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
wvErrCode IPComm_ParseData(U8 * pDataBuf, S32 s32Datalen, ST_WV_IPCOMM  * pstIPComm)
{
    if((NULL == pDataBuf) || (NULL == pstIPComm))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Param NULL");
        return WV_ERR_PARAMS;
    }
    
    if(s32Datalen < IP_ST_MIN_LEN)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Receive IP data too less, %d", s32Datalen);
        return WV_ERR_COMM_IP_RECV_MISS;
    }

    U32     u32RecvDataCRC  =   0;
    U32     u32remainder    =   0;

    memset(pstIPComm, 0, sizeof(ST_WV_IPCOMM));

    //len Check
    memcpy(&(pstIPComm->u32Length), (pDataBuf + 12), 4);
    // ����ṹ��Ĳ�����
    u32remainder = pstIPComm->u32Length % 4;
    if(u32remainder != 0)
    {
        u32remainder = 4 - u32remainder;
    }
    if(s32Datalen != (pstIPComm->u32Length + IP_ST_MIN_LEN + u32remainder))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Receive ip data len error, %d", s32Datalen);
        return WV_ERR_COMM_IP_RECV_MISS;
    }

    //CRC Check
    memcpy(&(pstIPComm->u32StructCRC), (pDataBuf + s32Datalen - 4),4);
    u32RecvDataCRC = Common_CRCCalculate(pDataBuf, 16 + pstIPComm->u32Length, IP_CRC_32);
    if(pstIPComm->u32StructCRC != u32RecvDataCRC)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Receive IP data CRC error [%d:%d]", pstIPComm->u32StructCRC,u32RecvDataCRC);
        return WV_ERR_CRC;
    }

    //CMD Parse
    memcpy(&(pstIPComm->u16CMD), (pDataBuf + 2), 2);
   // LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "Receive IP CMD : 0x%x", pstIPComm->u16CMD);

    //Slot ID
    pstIPComm->u8SlotID = *pDataBuf;
    //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "Receive IP SlotID : 0x%x", pstIPComm->u8SlotID);

    //DATA Parse
    memcpy(pstIPComm->pu8Data, (pDataBuf + 16), pstIPComm->u32Length);

    //Combine pack
    memcpy(&(pstIPComm->u32CurTansNum), (pDataBuf + 4), sizeof(U32));
    memcpy(&(pstIPComm->u32SumTansNum), (pDataBuf + 8), sizeof(U32));

    return WV_SUCCESS;
}




/*****************************************************************************
  Function:     IPComm_Server_Service
  Description:  IPͨ�ŷ������߳�
                �����ȴ��пͻ�������֮�������������̺߳�����ȴ�
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void IPComm_Server_Service()
{
    //init socket
    socketS_open();

    //bind
    socketS_bind();

    //listen
    socketS_listen();

    U8      pU8RecvBuff[4096];
    S32     s32Recvlen  =   0;
    S32     connect_fd  =   -1;
    ST_WV_IPCOMM  stIPComm;

    while(1)
    {
        //TODO maybe can be used by IPaddr check
        connect_fd = accept(socketSfd, (struct sockaddr*)NULL, NULL);
        if(-1 == connect_fd)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "accept socket error: %s(errno: %d)\n",strerror(errno),errno);
            continue;
        }

   
        s32Recvlen = recv(connect_fd, pU8RecvBuff, 4096, 0);
        IPComm_ParseData(pU8RecvBuff, s32Recvlen, &stIPComm);
        IPComm_Response(connect_fd, &stIPComm);
        
        close(connect_fd);
    }

    close(socketSfd);
}

/*****************************************************************************
  Function:     IPServer_Start
  Description:  ����IPͨ�ŷ�����
  Input:        none
  Output:       none
  Return:       WV_ERR_FAILURE         --  ʧ��
                WV_SUCCESS             --  �ɹ�
  Author:       ruibin.zhang
*****************************************************************************/
wvErrCode IPServer_Start(void)
{
    int res = 0;
    pthread_t IPServer_thread;

    res = pthread_create(&IPServer_thread, NULL, (void*)IPComm_Server_Service, NULL);
    if(-1 == res)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "IP communication thread create error :%s(errno: %d)",strerror(errno),errno);
        return WV_ERR_COMM_PTHREAD_CREAT;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "IP communication create successfully!");
    }

    res = pthread_detach(IPServer_thread);
    if(-1 == res)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "IP communication thread detach error :%s(errno: %d)",strerror(errno),errno);
        return WV_ERR_COMM_PTHREAD_CREAT;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "IP communication pthread detach successfully!");
    }

    return WV_SUCCESS;
}


/*****************************************************************************
  Function:     IPComm_Request
  Description:  IPͨ�������������
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
//TODO input Parameters can change
void IPComm_Request_SendResource()
{
    S32 socketC_fd = -1;

    //init socket
    socketC_open(&socketC_fd);
    if(socketC_fd < 0)
    {
        return;
    }

    //connect
    U32 u32IpAddr = 0xc0a8030a; //"192.168.3.10"
    socketC_connect(socketC_fd, u32IpAddr);


    U8      pU8RecvBuff[4096];
    S32     s32Recvlen  =   0;
    ST_WV_IPCOMM  stIPComm;

    U32     cnt         =   0;

    while(1)
    {
        IPComm_SendResource(socketC_fd);

        s32Recvlen = recv(socketC_fd, pU8RecvBuff, 4096, 0);
        
        IPComm_ParseData(pU8RecvBuff, s32Recvlen, &stIPComm);

        if((IPCOMM_SUCCESS_CMD == stIPComm.u16CMD) || cnt >= 3)
        {
            break;
        }
        cnt ++;
    }

    close(socketC_fd);
}


/*****************************************************************************
  Function:     IPComm_SendCmd2Slot
  Description:  Send Command to slot
  Input:        u32Slot  -- dest slot
                u32cmd -- command
  Output:       none
  Return:       WV_ERR_FAILURE         --  ʧ��
                WV_SUCCESS             --  �ɹ�
  Author:       huada.huang
*****************************************************************************/
wvErrCode IPComm_SendCmd2Slot(U32 u32Slot, U32 u32cmd)
{
    U32 u32IpAddr = 0;
    u32IpAddr = Resource_getBoardIPAddress(u32Slot);

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_COMM,
        "[%s:%d]u32IpAddr[%u.%u.%u.%u],u32cmd[%08X]\r\n",
        __FUNCTION__,
        __LINE__,
        (u32IpAddr >> 24) & 0xFF,
        (u32IpAddr >> 16) & 0xFF,
        (u32IpAddr >>  8) & 0xFF,
        (u32IpAddr >>  0) & 0xFF,
        u32cmd);
    
    return IPComm_SendCmd(u32IpAddr, u32cmd);
}

/*****************************************************************************
  Function:     IPComm_SendData2Slot
  Description:  Send data to slot
  Input:        u32Slot  -- dest slot
                u32cmd -- command
                *u8Data -- data point
                u32DataLen -- data len
  Output:       none
  Return:       WV_ERR_FAILURE         --  ʧ��
                WV_SUCCESS             --  �ɹ�
  Author:       huada.huang
*****************************************************************************/
wvErrCode IPComm_SendData2Slot(U32 u32Slot, U32 u32cmd, U8 *u8Data, U32 u32DataLen)
{
    U32 u32IpAddr = 0;
    u32IpAddr = Resource_getBoardIPAddress(u32Slot);

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_COMM,
        "[%s:%d]u32IpAddr[%u.%u.%u.%u],u32cmd[%08X],u32DataLen[%u]\r\n",
        __FUNCTION__,
        __LINE__,
        (u32IpAddr >> 24) & 0xFF,
        (u32IpAddr >> 16) & 0xFF,
        (u32IpAddr >>  8) & 0xFF,
        (u32IpAddr >>  0) & 0xFF,
        u32cmd,
        u32DataLen);

    return IPComm_SendData(u32IpAddr, u32cmd, u8Data, u32DataLen);
}

/*****************************************************************************
  Function:     IPComm_SendCmd
  Description:  Send Command through ip
  Input:        u32IpAddr  -- dest ip addr 
                u32cmd -- command
  Output:       none
  Return:       WV_ERR_FAILURE         --  ʧ��
                WV_SUCCESS             --  �ɹ�
  Author:       huada.huang
*****************************************************************************/
wvErrCode IPComm_SendCmd(U32 u32IpAddr, U32 u32cmd)
{
    S32    socketC_fd  = -1;
    S32    s32Recvlen  =   0;
    U32    cnt         =   0;
    ST_WV_IPCOMM  stIPComm;

    U8   pU8RecvBuff[4096];

    while(1)
    {
        /*init socket*/
        if (WV_SOCKETC_SUCCESS != socketC_open(&socketC_fd))
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "IP communication socket open failed!");
            return WV_ERR_SOCKET_OPEN_FAIL;
        }

        /* connect to dest board */
        if(WV_SOCKETC_SUCCESS != socketC_connect(socketC_fd, u32IpAddr))
        {
            LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "IP communication socket connect failed!");
            return WV_ERR_SOCKET_CONNECT_FAIL;
        }

        IPComm_SendTo(socketC_fd, u32cmd, NULL, 0, 0, 1);

        s32Recvlen = recv(socketC_fd, pU8RecvBuff, 4096, 0);
        IPComm_ParseData(pU8RecvBuff, s32Recvlen, &stIPComm);

        close(socketC_fd);
        socketC_fd = -1;

        if((IPCOMM_SUCCESS_CMD == stIPComm.u16CMD) || (cnt >= 3))
        {
            break;
        }
        cnt ++;
    }

    if(cnt >= 3)
    {
        return WV_ERR_IPCOMM_TIME_OUT;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     IPComm_SendData
  Description:  Send Command through ip
  Input:        u32IpAddr  -- dest ip addr 
                u32cmd -- command
                *u8Data -- data point
                u32DataLen -- data len
  Output:       none
  Return:       WV_ERR_FAILURE         --  ʧ��
                WV_SUCCESS             --  �ɹ�
  Author:       huada.huang
*****************************************************************************/
wvErrCode IPComm_SendData(U32 u32IpAddr, U32 u32cmd, U8 *u8Data, U32 u32DataLen)
{
    S32 socketC_fd = -1;
    
    U8      pU8RecvBuff[4096];
    S32     s32Recvlen  =   0;
    ST_WV_IPCOMM  stIPComm;

    U32 shouldSendLen = 0;
    U32 hadsendLen = 0;
    U32 sendLen = 0;

    U32 SumTansNum = 0;
    U32 TranIndex = 0;
    U8 *pdata;
    
    pdata = u8Data;
    U32     cnt         =   0;

    shouldSendLen = u32DataLen;

    SumTansNum = u32DataLen/IPCOMM_DATA_MAX_LEN + (((u32DataLen%IPCOMM_DATA_MAX_LEN)==0)?0:1);

    for (TranIndex=0; TranIndex < SumTansNum; TranIndex++)
    {
        shouldSendLen = u32DataLen - hadsendLen;
        sendLen = (shouldSendLen > IPCOMM_DATA_MAX_LEN)?IPCOMM_DATA_MAX_LEN:(shouldSendLen);
        
        while(1)
        {
            /*init socket*/
            if (WV_SOCKETC_SUCCESS != socketC_open(&socketC_fd))
            {
                LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "IP communication socket open failed!");
                return WV_ERR_SOCKET_OPEN_FAIL;
            }

            /* connect to dest board */
            if(WV_SOCKETC_SUCCESS != socketC_connect(socketC_fd, u32IpAddr))
            {
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "IP communication socket connect failed!");
                return WV_ERR_SOCKET_CONNECT_FAIL;
            }

            IPComm_SendTo(socketC_fd, u32cmd, pdata, sendLen, TranIndex, SumTansNum);

            s32Recvlen = recv(socketC_fd, pU8RecvBuff, 4096, 0);
            IPComm_ParseData(pU8RecvBuff, s32Recvlen, &stIPComm);

            printf("****** close socket\r\n");
            close(socketC_fd);
            socketC_fd = -1;

            if((IPCOMM_SUCCESS_CMD == stIPComm.u16CMD) || (cnt >= 3))
            {   
                break;
            }
            cnt ++;
        }

        if(IPCOMM_SUCCESS_CMD == stIPComm.u16CMD)
        {
            hadsendLen += sendLen;
            pdata += sendLen;
        }

        if(cnt >= 3)
        {
            return WV_ERR_IPCOMM_TIME_OUT;
        }
    }

    return WV_SUCCESS;
}

wvErrCode IPComm_ChangeIP(U8 u8Slot, U8 *pu8Data)
{
    U32 u32BBIPAddr = 0;
    U32 u32SelfIPAddr = 0;

    if ((!pu8Data) || (SLOT_BB != u8Slot))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM,
                "[%s]:Input Error,pu8Data[%p],u8Slot[%u]\r\n",
                __FUNCTION__, pu8Data, u8Slot);
        return WV_ERR_PARAMS;
    }

    u32BBIPAddr = *((U32 *)(&pu8Data[0]));
    u32SelfIPAddr = *((U32 *)(&pu8Data[4]));
    
    Resource_setBackBoardIpAddr(u32BBIPAddr);
    PHY_SetIP(u32SelfIPAddr);
    Resource_setSelfIpAddr(u32SelfIPAddr);

    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS,
        "u32BBIPAddr[%d.%d.%d.%d],u32SelfIPAddr[%d.%d.%d.%d]\r\n",
        (u32BBIPAddr >> 24) & 0xFF,
        (u32BBIPAddr >> 16) & 0xFF,
        (u32BBIPAddr >> 8) & 0xFF,
        u32BBIPAddr & 0xFF,
        (u32SelfIPAddr >> 24) & 0xFF,
        (u32SelfIPAddr >> 16) & 0xFF,
        (u32SelfIPAddr >> 8) & 0xFF,
        u32SelfIPAddr & 0xFF);

    //������Դ��Ϣ������
    usleep(1000000);

    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Send Resource To BackBoard\r\n");

    IPComm_SendData(u32BBIPAddr, IPCOMM_SEND_RESOUR, (U8*)Resource_getSelfHandle(), sizeof(TResource_TS));

    return WV_SUCCESS;
}


