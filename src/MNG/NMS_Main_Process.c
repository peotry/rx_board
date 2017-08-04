/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     NMS_Main_Process.c
* Description:  NMS Function Interface
* Author    :   jie.zhan
* Modified  :
* Reviewer  :
* Date      :   2017-03-28
* Record    :
*
**********************************************************************/
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "NMS_Main_Process.h"
#include "PubServerToClient.h"
#include "WV_log.h"

static COMM_HEAD_t s_stCOMMHeadR = {0,0,0,0,0,0,0,0};
static NMS_Payload_t s_Payload   = {NULL, 0};
static S32 s_nError              = NMS_SUCCESS;
static S8  s_ars8NMSStream[NMS_RX_BUF_SIZE + 1] = {0};

static void NMS_SaveCOMMHead(void* pCommHead);
static COMM_HEAD_t* NMS_GetCOMMHead(void);
static void NMS_RecordErrorCode(S32 nErrorCode);
static S32 NMS_ReadErrorCode(void);
static S32 NMS_TXD(NMSConn *pstConn, const COMM_HEAD_t *pstSCHead, const char* pcData, U32 u32DataLen);
static S32 NMS_RXD(NMSConn *pstConn);
static S32 NMS_ParsePayloadParameters(const char *pcBuf, U32 u32DataLen);
static S32 NMS_ParsePayload(const char *pcBuf, U32 u32DataLen);
static S32 NMS_ExecuteCommands(NMSConn *pstConn);
static S32 NMS_Quit(NMSConn *pstConn);
static S32 NMS_Test(NMSConn *pstConn);
static S32 NMS_CommRsp(NMSConn *pstConn, U32 u32Code);
static S32 NMS_SetAuthorization(NMSConn *pstConn);
static S32 NMS_GetAuthorization(NMSConn *pstConn);
static S32 NMS_Response(NMSConn *pstConn);

/* MGS 命令与执行函数 */
static MgsComm_t s_stMgsComm[] = {

    {NMS_DMP_GET_AUTHORIZATION,     NULL,                   NMS_GetAuthorization},
    {NMS_DMP_SET_AUTHORIZATION,     NMS_SetAuthorization,   NMS_Response},

    {NMS_DMP_TEST,                  NMS_Test,               NMS_Response },
    {NMS_DMP_QUIT,                  NMS_Quit,               NULL }
};

/*****************************************************************************
  Function:     NMS_RecordErrorCode
  Description:  RecordErrorCode
  Input:        nErrorCode
  Output:		none
  Return:		none
  Author:       C.C 2014.11.17
*****************************************************************************/
static void NMS_RecordErrorCode(S32 nErrorCode)
{
    s_nError = nErrorCode;
}

/*****************************************************************************
  Function:     NMS_ReadErrorCode
  Description:  ReadErrorCode
  Input:		none
  Output:		none
  Return:       Error Code.
  Author:       C.C 2014.11.17
*****************************************************************************/
static S32 NMS_ReadErrorCode(void)
{
    return s_nError;
}

/*****************************************************************************
  Function:     NMS_SaveCOMMHead
  Description:  NMS_SaveCOMMHead
  Input:        pCommHead
  Output:		none
  Return:		none
  Author:       C.C 2014.11.17
*****************************************************************************/
static void NMS_SaveCOMMHead(void* pCommHead)
{
    memcpy(&s_stCOMMHeadR, pCommHead, sizeof(COMM_HEAD_t));
}

/*****************************************************************************
  Function:     NMS_GetCOMMHead
  Description:  GetCOMMHead
  Input:		none
  Output:		none
  Return:       &s_stCOMMHeadR
  Author:       C.C 2014.11.17
*****************************************************************************/
static COMM_HEAD_t* NMS_GetCOMMHead(void)
{
    return &s_stCOMMHeadR;
}

/*****************************************************************************
  Function:     NMS_GetPayload
  Description:  GetPayload
  Input:		none
  Output:		none
  Return:       true or false
  Author:       C.C 2014.11.17
*****************************************************************************/
static NMS_Payload_t* NMS_GetPayload(void)
{
    return &s_Payload;
}

/*****************************************************************************
  Function:     NMS_DestroyPayload
  Description:  DestroyPayload
  Input:		none
  Output:		none
  Return:		none
  Author:       C.C 2014.11.17
*****************************************************************************/
static void NMS_DestroyPayload(void)
{
    if (NULL != s_Payload.pu8Data)
    {
        free(s_Payload.pu8Data);
        s_Payload.pu8Data = NULL;
        s_Payload.u32Len  = 0;
    }
}

/*****************************************************************************
  Function:     NMS_Quit
  Description:  Quit
  Input:        pstConn
  Output:		none
  Return:       ERROR_NMG_PARAM		--- 入参错误
  				NMS_SUCCESS			--- 成功
  Author:       C.C 2014.11.17
*****************************************************************************/
static S32 NMS_Quit(NMSConn *pstConn)
{
    if (NULL == pstConn)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Invalid input, NULL == pstConn");
        return ERROR_NMG_PARAM;
    }

    pstConn->nClose = NMS_DISCONNECT;

    LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Quit.");

    return NMS_SUCCESS;
}

/*****************************************************************************
  Function:     NMS_Test
  Description:  Test
  Input:        pstConn
  Output:		none
  Return:       ERROR_NMG_PARAM		--- 入参错误
  				NMS_SUCCESS			--- 成功
  Author:       C.C 2014.11.17
*****************************************************************************/
static S32 NMS_Test(NMSConn *pstConn)
{
    if (NULL == pstConn)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Invalid input.NULL == pstConn");
        return ERROR_NMG_PARAM;
    }

    LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Test.");

    return NMS_SUCCESS;
}

/*****************************************************************************
  Function:     NMS_Response
  Description:  CommRsp
  Input:        pstConn
  Output:		none
  Return:       success or error code.
  Author:       C.C 2014.11.17
*****************************************************************************/
static S32 NMS_Response(NMSConn *pstConn)
{
    U32 u32Code = (U32)NMS_ReadErrorCode();

    if (NULL == pstConn)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Invalid input.NULL == pstConn");
        return ERROR_NMG_PARAM;
    }

    return NMS_CommRsp(pstConn, u32Code);
}

/*****************************************************************************
  Function:     NMS_CommRsp
  Description:  CommRsp
  Input:        pstConn
  				u32Code
  Output:		none
  Return:       ERROR_NMG_PARAM		--- 入参错误
  				NMS_SUCCESS			--- 成功
  Author:       C.C 2014.11.17
*****************************************************************************/
static S32 NMS_CommRsp(NMSConn *pstConn, U32 u32Code)
{
    S32 nRet  = 0;
    COMM_HEAD_t stHead        = {0,0,0,0,0,0,0,0};
    Srv2ClientCommonRsp stRsp = {0};

    if ((NULL == pstConn) || (pstConn->nSocket < 0))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "invalid input");
        return ERROR_NMG_PARAM;
    }

    stHead.dwDataType      = NMS_DMP_COMMON_RSP;
    stHead.dwDataLength    = sizeof(Srv2ClientCommonRsp);
    stHead.dwInallFrameNum = 1;

    nRet = NMS_SendData(pstConn->nSocket, (char*)&stHead, sizeof(COMM_HEAD_t));
    if (NMS_SUCCESS != nRet)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "NMS_SendData Failed.");
        return nRet;
    }

    stRsp.dwRslt = u32Code;

    nRet = NMS_SendData(pstConn->nSocket, (char*)&stRsp, sizeof(Srv2ClientCommonRsp));
    if (NMS_SUCCESS != nRet)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "NMS_SendData Failed.");
        return nRet;
    }

    return NMS_SUCCESS;
}

/*****************************************************************************
  Function:     NMS_ParsePayloadParameters
  Description:  ParsePayload
  Input:        pcBuf
  				u32DataLen
  Output:		none
  Return:       success or error code.
  Author:       C.C 2014.11.17
*****************************************************************************/
static S32 NMS_ParsePayloadParameters(const char *pcBuf, U32 u32DataLen)
{
    S32 nRet 		  = 0;
    U16 i             = 0;
    U16 u16Packets    = 0;
    U32 u32PayloadLen = 0;
    char *pcPayload   = NULL;
    const char *p     = NULL;

    Srv2ClientParameterRsp_t stCSRsp = {0, 0, 0, 0, {0}};

    if (NULL == pcBuf)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS,"invalid input.");
        return ERROR_NMG_PARAM;
    }

    if (u32DataLen < sizeof(Srv2ClientParameterRsp_t) + sizeof(COMM_HEAD_t))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS,"data len(%u) error.", u32DataLen);
        return ERROR_NMG_PARAM;
    }

    memcpy(&stCSRsp, pcBuf + sizeof(COMM_HEAD_t), sizeof(Srv2ClientParameterRsp_t));

    if (0 != stCSRsp.wParameterSendIndex)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS,"0 != stCSRsp.wParameterSendIndex");
        return ERROR_NMG_RECV;
    }

    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "stCSRsp.dwParameterTotelLen = %u", stCSRsp.dwParameterTotelLen);

    pcPayload = (char*)malloc(stCSRsp.dwParameterTotelLen);
    if (NULL == pcPayload)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "malloc failed.");
        return ERROR_NMG_MALLOC;
    }

    memcpy(pcPayload, stCSRsp.arucParameterBuf, stCSRsp.wParameterSendLen);
    u32PayloadLen = stCSRsp.wParameterSendLen;

    u16Packets = stCSRsp.wParameterSendTotelCnt;

    for (i = 1; i < u16Packets; i++)
    {
        p = pcBuf + sizeof(COMM_HEAD_t) + i * (sizeof(Srv2ClientParameterRsp_t) + sizeof(COMM_HEAD_t));

        memcpy(&stCSRsp, p, sizeof(Srv2ClientParameterRsp_t));

        if (i != stCSRsp.wParameterSendIndex)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "index error %u != %u", i, stCSRsp.wParameterSendIndex);
            nRet = ERROR_NMG_RECV;

            goto error;
        }

        memcpy(pcPayload + u32PayloadLen, stCSRsp.arucParameterBuf, stCSRsp.wParameterSendLen);
        u32PayloadLen += stCSRsp.wParameterSendLen;
    }

    if (NULL != s_Payload.pu8Data)
    {
        free(s_Payload.pu8Data);
        s_Payload.pu8Data = NULL;
    }

    s_Payload.pu8Data = (U8*)pcPayload;
    s_Payload.u32Len  = u32PayloadLen;

    return NMS_SUCCESS;

error:

    if (NULL != pcPayload)
    {
        free(pcPayload);
        pcPayload = NULL;
        (void)pcPayload; // pc-lint
    }

    return nRet;
}

/*****************************************************************************
  Function:     NMS_TXD
  Description:	TX Send Data
  Input:        pstConn 
  				pstSCHead
  				pcData
  				u32DataLen
  Output:		none
  Return:       success or error code.
  Author:       C.C 2014.11.17
*****************************************************************************/
static S32 NMS_TXD(NMSConn *pstConn, const COMM_HEAD_t *pstSCHead, const char* pcData, U32 u32DataLen)
{
    S32 nRet 		 = 0;
    U16 u16Packets   = 0;
    U16 u16PacketIdx = 0;
    U32 u32TxdLen    = 0;
    U32 u32Len       = 0;
    const U32 u32MsgHeadLen = sizeof(COMM_HEAD_t);
    const U32 u32MsgLen = sizeof(Srv2ClientParameterRsp_t);
    Srv2ClientParameterRsp_t stS2CRsp;

    char *p = (char*)s_ars8NMSStream;

    if ((NULL == pstConn) || (pstConn->nSocket < 0) || (NULL == pstSCHead) || (NULL == pcData))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "invalid input");
        return ERROR_NMG_PARAM;
    }

    u16Packets = (U16)((u32DataLen + MAX_PARAMETER_PER_PACKET_BUF_LEN - 1) / MAX_PARAMETER_PER_PACKET_BUF_LEN);

    while (u32TxdLen < u32DataLen)
    {
        u32Len = ((u32DataLen - u32TxdLen) > MAX_PARAMETER_PER_PACKET_BUF_LEN)
               ? MAX_PARAMETER_PER_PACKET_BUF_LEN : (u32DataLen - u32TxdLen);

        memset(&stS2CRsp, 0, u32MsgLen);

        stS2CRsp.dwParameterTotelLen    = u32DataLen;
        stS2CRsp.wParameterSendTotelCnt = u16Packets;
        stS2CRsp.wParameterSendIndex    = u16PacketIdx;
        stS2CRsp.wParameterSendLen      = (U16)u32Len;

        memcpy(stS2CRsp.arucParameterBuf, pcData + u32TxdLen, u32Len);

        memcpy(p, pstSCHead, u32MsgHeadLen);

        memcpy(p + u32MsgHeadLen, &stS2CRsp, u32MsgLen);

        nRet = NMS_SendData(pstConn->nSocket, p, u32MsgHeadLen + u32MsgLen);
        if (NMS_SUCCESS != nRet)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "NMS_SendData Failed.");
            return nRet;
        }

        u16PacketIdx++;
        u32TxdLen += u32Len;
    }

    return NMS_SUCCESS;
}

/*****************************************************************************
  Function:     NMS_RXD
  Description:	RXD Data
  Input:        pstConn
  Output:		none
  Return:       success or error code.
  Author:       C.C 2014.11.17
*****************************************************************************/
static S32 NMS_RXD(NMSConn *pstConn)
{
    S32 nRet 			= 0;
    U32 u32HadRecvLen   = 0;
    U32 u32PerPacketLen = 0;
    U32 u32TotalLen 	= 0;
    U32 u32Ret 			= 0;
    Srv2ClientParameterRsp_t stCSRsp = {0, 0, 0, 0, {0}};
    COMM_HEAD_t *pstCHead  = NULL;
    char *pcBuf 		   = NULL;
    BOOL bIsParamter       = FALSE;

    if (NULL == pstConn)
    {
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "invalid pstConn.");
        return ERROR_NMG_PARAM;
    }

    if (0 > pstConn->nSocket)
    {
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "invalid socket.");
        return ERROR_NMG_PARAM;
    }

    if (NULL == pstConn->ps8ReadPos)
    {
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "invalid readpos.");
        return ERROR_NMG_PARAM;
    }

    pstCHead = NMS_GetCOMMHead();
    if (NULL == pstCHead)
    {
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "NMS_GetCOMMHead Failed.");
        return ERROR_NMG_PARAM;
    }

    u32HadRecvLen = pstConn->u32ReadLen;
    u32PerPacketLen = pstCHead->dwDataLength + sizeof(COMM_HEAD_t);

    if (u32HadRecvLen < u32PerPacketLen)
    {
        u32Ret = NMG_ReceiveData(pstConn->nSocket, (char*)(pstConn->ps8ReadPos + u32HadRecvLen),
                                u32PerPacketLen - u32HadRecvLen);
        if (u32Ret != u32PerPacketLen - u32HadRecvLen)
        {
			LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "NMG_ReceiveData Failed.");
            return nRet;
        }

        u32HadRecvLen = u32PerPacketLen;
    }

    u32TotalLen = sizeof(COMM_HEAD_t) + pstCHead->dwDataLength;

   LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS,"1. u32TotalLen = %u. pstCHead->dwDataType = %x", u32TotalLen, pstCHead->dwDataType);

    if (NMS_DMP_SET_TS_INFO          == pstCHead->dwDataType
     || NMS_DMP_CLEAR_ALL_TS         == pstCHead->dwDataType
     || NMS_DMP_GET_SINGLE_TS_INFO   == pstCHead->dwDataType
     || NMS_DMP_CLEAR_SINGLE_TS_INFO == pstCHead->dwDataType
     || NMS_DMP_SAVE_TS              == pstCHead->dwDataType
     || NMS_SET_SB_PARAM             == pstCHead->dwDataType
     || NMS_DMP_SET_SYSTEM_INFO      == pstCHead->dwDataType
     || NMS_DMP_SET_AUTHORIZATION    == pstCHead->dwDataType)
    {
        memcpy(&stCSRsp, pstConn->ps8ReadPos + sizeof(COMM_HEAD_t), sizeof(Srv2ClientParameterRsp_t));

        if (0 != stCSRsp.wParameterSendIndex)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "0 != stCSRsp.wParameterSendIndex");
            return ERROR_NMG_RECV;
        }

        if (0 != stCSRsp.wParameterSendTotelCnt)
        {
            u32TotalLen = stCSRsp.wParameterSendTotelCnt * (sizeof(COMM_HEAD_t) + sizeof(Srv2ClientParameterRsp_t));
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "2 u32TotalLen = %u, stCSRsp.wParameterSendTotelCnt = %u", u32TotalLen, stCSRsp.wParameterSendTotelCnt);
        }

        bIsParamter = TRUE;
    }

    LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS,"3 u32TotalLen = %u, u32HadRecvLen = %u", u32TotalLen, u32HadRecvLen);

    pcBuf = (char*)malloc(u32TotalLen);
    if (NULL == pcBuf)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "malloc failed.");
        return ERROR_NMG_MALLOC;
    }

    memset(pcBuf, 0, u32TotalLen);
    memcpy(pcBuf, pstConn->ps8ReadPos, u32HadRecvLen);

    if (u32HadRecvLen < u32TotalLen)
    {
        u32Ret = NMG_ReceiveData(pstConn->nSocket, pcBuf + u32HadRecvLen, u32TotalLen - u32HadRecvLen);
        if (u32Ret != u32TotalLen - u32HadRecvLen)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "NMG_ReceiveData Failed, u32Ret(%u) != u32TotalLen - u32HadRecvLen(%u)", u32Ret, u32TotalLen - u32HadRecvLen);
            nRet = ERROR_NMG_RECV;
            goto error;
        }
    }

    if (bIsParamter)
    {
        nRet = NMS_ParsePayloadParameters(pcBuf, u32HadRecvLen);
        if (NMS_SUCCESS != nRet)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "NMS_ParsePayloadParameters Failed.");
        }
    }
    else
    {
        nRet = NMS_ParsePayload(pcBuf, u32HadRecvLen);
        if (NMS_SUCCESS != nRet)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "NMS_ParsePayload Failed.");
        }
    }

error:

    if (NULL != pcBuf)
    {
        free(pcBuf);
        pcBuf = NULL;
        (void)pcBuf; // pc-lint
    }

    return nRet;
}

/*****************************************************************************
  Function:     NMS_ParsePayload
  Description:  ParsePayload
  Input:        pcBuf
  				u32DataLen
  Output:		none
  Return:       ERROR_NMG_PARAM     --- 入参错误
  				ERROR_NMG_MALLOC    --- Malloc错误
  				NMS_SUCCESS         --- 成功
  Author:       C.C 2014.11.17
*****************************************************************************/
static S32 NMS_ParsePayload(const char *pcBuf, U32 u32DataLen)
{
    U32 u32PayloadLen       = 0;
    char *pcPayload   		= NULL;
    COMM_HEAD_t  *pstCHead  = NULL;

    if (NULL == pcBuf || 0 == u32DataLen)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Invalid input. NULL == pcBuf || 0 == u32DataLen");
        return ERROR_NMG_PARAM;
    }

    pstCHead = NMS_GetCOMMHead();
    if (NULL == pstCHead)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "NMS_GetCOMMHead Failed.");
        return ERROR_NMG_PARAM;
    }

    u32PayloadLen = pstCHead->dwDataLength;

    pcPayload = (char*)malloc(u32PayloadLen);
    if (NULL == pcPayload)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "malloc failed.");
        return ERROR_NMG_MALLOC;
    }

    memcpy(pcPayload, pcBuf + sizeof(COMM_HEAD_t), u32PayloadLen);

    if (NULL != s_Payload.pu8Data)
    {
        free(s_Payload.pu8Data);
        s_Payload.pu8Data = NULL;
    }

    s_Payload.pu8Data = (U8*)pcPayload;
    s_Payload.u32Len  = u32PayloadLen;

    return NMS_SUCCESS;
}

/*****************************************************************************
  Function:     NMS_ExecuteCommands
  Description:  ExecuteCommands
  Input:        pstConn
  Output:		none
  Return:       ERROR_NMG_PARAM	    --- 入参错误
  				NMS_SUCCESS			--- 成功
  Author:       C.C 2014.11.17
*****************************************************************************/
static S32 NMS_ExecuteCommands(NMSConn *pstConn)
{
    S32 nRet   = 0;
    U32 i      = 0;
    U32 u32Num = sizeof(s_stMgsComm) / sizeof(MgsComm_t);

    COMM_HEAD_t stCOMMHead = {0,0,0,0,0,0,0,0};

    U32             u32CMD = 0;
    HandleExecute   pExe   = NULL;
    HandleResponse  pRes   = NULL;

    if (NULL == pstConn)
    {
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "invalid conn.");
        return ERROR_NMG_PARAM;
    }

    if (NULL == pstConn->ps8ReadPos)
    {
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "invalid pstConn->ps8ReadPos.");
        return ERROR_NMG_PARAM;
    }

    if (0 == pstConn->u32ReadLen)
    {
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "invalid pstConn->u32ReadLen.");
        return ERROR_NMG_PARAM;
    }

    NMS_RecordErrorCode(NMS_SUCCESS);

    memcpy(&stCOMMHead, pstConn->ps8ReadPos, sizeof(COMM_HEAD_t));
    NMS_SaveCOMMHead(&stCOMMHead);

    for (i = 0; i < u32Num; i++)
    {
        if (s_stMgsComm[i].u32CMD == stCOMMHead.dwDataType)
        {
            u32CMD = stCOMMHead.dwDataType;
            pExe = s_stMgsComm[i].pExe;
            pRes = s_stMgsComm[i].pRes;
			
			LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "u32CMD = 0x%08x", u32CMD);

            break;
        }
    }

    if (i == u32Num)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Unknown CMD 0x%08x.",stCOMMHead.dwDataType);
    }

    if (NULL != pExe)
    {
        nRet = NMS_RXD(pstConn);
        if (NMS_SUCCESS != nRet)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "NMS_RXD Failed(Cmd 0x%08x).", u32CMD);
        }

        nRet = pExe(pstConn);
        if (NMS_SUCCESS != nRet)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Exe Cmd 0x%08x Return 0x%x", u32CMD, nRet);
        }
        NMS_RecordErrorCode(nRet);
    }

    if (NULL != pRes)
    {
        nRet = pRes(pstConn);
        if (NMS_SUCCESS != nRet)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Res Cmd 0x%08x Return 0x%x", u32CMD, nRet);
        }
		
        NMS_RecordErrorCode(nRet);
    }

    NMS_DestroyPayload();

    return NMS_SUCCESS;
}

/*****************************************************************************
  Function:     NMS_SetAuthorization
  Description:  SetAuthorization
  Input:        pstConn
  Output:		none
  Return:       success or error code.
  Author:       C.C 2014.11.17
*****************************************************************************/
static S32 NMS_SetAuthorization(NMSConn *pstConn)
{
    U32 i    = 0;
    S32 nRet = 0;
    NMS_Payload_t* pstPayload = NULL;

    DS2432_AllBoard_t  stOldDS2432AllBoard;
    DS2432_AllBoard_t *pstAllBoardDS2432 = NULL;

    if (NULL == pstConn)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Invalid input.NULL == pstConn");
        return ERROR_NMG_PARAM;
    }

    pstPayload = NMS_GetPayload();
    if (NULL == pstPayload)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "NMS_GetPayload Failed.");
        return ERROR_NMG_PARAM;
    }

    if (NULL == pstPayload->pu8Data)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "invalid payload data pointer.");
        return ERROR_NMG_PARAM;
    }

    if (0 == pstPayload->u32Len)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "payload len = 0.");
        return ERROR_NMG_PARAM;
    }

    pstAllBoardDS2432 = DS2432_GetAllBoardDS2432();
    if (NULL == pstAllBoardDS2432)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "DS2432_GetAllBoardDS2432 failed.");
        return ERROR_NMG_PARAM;
    }

    memcpy(&stOldDS2432AllBoard, pstAllBoardDS2432,   sizeof(DS2432_AllBoard_t));
    memcpy(pstAllBoardDS2432,    pstPayload->pu8Data, sizeof(DS2432_AllBoard_t));

    for (i = 0; i < pstAllBoardDS2432->u32HadBoardNum; i++)
    {
        if (0 != memcmp(&stOldDS2432AllBoard.stDS2432_PerBoard[i],
                        &pstAllBoardDS2432->stDS2432_PerBoard[i], sizeof(DS2432_PerBoard_t)))
        {
            nRet |= DS2432_LoadData(pstAllBoardDS2432->stDS2432_PerBoard[i].u8SlotNumber,
                                    (U8 *)&(pstAllBoardDS2432->stDS2432_PerBoard[i].stDS2432_ChipInfo));

            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "DS2432_LoadData return %d", nRet);
        }
    }

    if (NMS_SUCCESS != nRet)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "DS2432_LoadData Failed.");
        memcpy(pstAllBoardDS2432, &stOldDS2432AllBoard, sizeof(DS2432_AllBoard_t));
    }
	else
	{
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "DS2432_LoadData Sucess.");
	}

    return nRet;
}

/*****************************************************************************
  Function:     NMS_GetAuthorization
  Description:  GetAuthorization
  Input:        pstConn
  Output:		none
  Return:       success or error code.
  Author:       C.C 2014.11.17
*****************************************************************************/
static S32 NMS_GetAuthorization(NMSConn *pstConn)
{
    S32 nRet = 0;
    char *p  = NULL;
    COMM_HEAD_t  stSCHead = {0,0,0,0,0,0,0,0};

    if (NULL == pstConn || pstConn->nSocket < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "invalid pstConn / pstConn->nSocket.");
        return ERROR_NMG_PARAM;
    }

    p = (char*)DS2432_GetAllBoardDS2432();
    if (NULL == p)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "DS2432_GetAllBoardDS2432 failed.");
        return ERROR_NMG_PARAM;
    }

	LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "u32HadBoardNum = %d", *p);

    stSCHead.dwDataType   = NMS_DMP_GET_AUTHORIZATION_RSP;
    stSCHead.dwDataLength = sizeof(Srv2ClientParameterRsp_t);

    nRet = NMS_TXD(pstConn, &stSCHead, p, sizeof(DS2432_AllBoard_t));
    if (NMS_SUCCESS != nRet)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "NMS_TXD Failed.");
        return nRet;
    }

    return NMS_SUCCESS;
}

/*****************************************************************************
  Function:     NMS_HandleReceive
  Description:	Handle Receive
  Input:        pstConn
  Output:		none
  Return:       ERROR_NMG_PARAM	    --- 入参错误
  				NMS_SUCCESS			--- 成功
  Author:		C.C 2014.11.17
*****************************************************************************/
S32 NMS_HandleReceive(NMSConn *pstConn)
{
    S32 nLen = 0;
    S32 nRet = 0;
    S32 nMaxRegetTimes = 6;
    U32 u32HadRecvdLen = 0;
    U32 u32MsgHeadSize = 0;

    if ((NULL == pstConn) || (pstConn->nSocket < 0))
    {
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Invalid Input...");
        return ERROR_NMG_PARAM;
    }

    memset(s_ars8NMSStream, 0, sizeof(s_ars8NMSStream));
    pstConn->ps8ReadPos  = s_ars8NMSStream;
    pstConn->ps8WritePos = s_ars8NMSStream;
    pstConn->nClose      = 0;

    u32MsgHeadSize = sizeof(COMM_HEAD_t);

    while(CLOSE != pstConn->eState && nMaxRegetTimes > 0)
    {
        // recv
        nLen = NonblockingReceive(pstConn->nSocket,
                                (char*)(pstConn->ps8ReadPos + u32HadRecvdLen),
                                NMS_RX_BUF_SIZE - u32HadRecvdLen);
        if (nLen <= 0)
        {
            nMaxRegetTimes--;
            usleep(50000);
            continue;
        }

        u32HadRecvdLen += (U32)nLen;

        if (u32HadRecvdLen < u32MsgHeadSize)
        {
            continue;
        }

        pstConn->u32ReadLen = u32HadRecvdLen;

        nRet = NMS_ExecuteCommands(pstConn);
        if (NMS_SUCCESS != nRet)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "ExecuteCommands failed.");
        }

        pstConn->eState = CLOSE;
    }

    if (NMS_DISCONNECT == pstConn->nClose)
    {
        close(pstConn->nSocket);
        NMS_ResetConnection(pstConn);
    }
    else
    {
        pstConn->eState = READY;
    }

    return NMS_SUCCESS;
}

