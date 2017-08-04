// ********************************************************
//
//  $Workfile: $ 
//
//   Author/Copyright   Gero Kuehn / GkWare e.K.
//                      Humboldtstrasse 177
//                      45149 Essen
//                      GERMANY 
//                      Tel: +49 174 520 8026
//                      Email: support@gkware.com
//                      Web: http://www.gkware.com
//
//
// ********************************************************
//
//  This file contains the portable CI Lib
// 

/****************************************************************************
 * Include files
 ****************************************************************************/

#include <assert.h>
#include "all.h"

#ifdef ENABLE_CI


/////////////////////////////////
// Config


/////////////////////////////////
// Code
//#define CI_DEBUG_ENABLE
//#define DUMP_TX_TPDUS
//#define DUMP_RX_TPDUS
//#define DUMP_TX_SPDUS
//#define DUMP_RX_SPDUS


#ifdef CI_DEBUG_ENABLE 
#define CI_DEBUG(x)   printf x
#else
#define CI_DEBUG(x)
#endif


/****************************************************************************
 * vars
 ****************************************************************************/

CI_HANDLESTRUCT             CiSlot[MAX_CI_SLOTS];
RESOURCE_INFO const        *g_CiResources[MAX_CI_RESOURCES];
const char                 *g_pszCiStackVersion = "GKSVN070810";

///////////////////////////////////////////////////////////////////
// Transport Layer
///////////////////////////////////////////////////////////////////

void CI_ModuleInserted(PCMCIA_HANDLE hModule, BYTE bSlotNum)
{
    BYTE msg[3];
    CI_HANDLE hCI;

    hCI = &CiSlot[bSlotNum];

    memset(hCI,0x00,sizeof(CI_HANDLESTRUCT));
    hCI->hSlotHandle = hModule;
    hCI->bSlotNum = bSlotNum;

  //  printf ("##DEBUG##[CI_ModuleRemoved] hCI->hSlotHandle(0x%08X), hCI(0x%08X) %d \r\n", 
  //            hCI->hSlotHandle, 
  //            hModule, 
  //            bSlotNum);

    msg[0] = CI_MSG_CI_INFO;
    msg[1] = CI_SLOT_MODULE_INSERTED;
    msg[2] = bSlotNum;
    SendCIMessage(hCI, msg, 3);

}

// -------------------------------------------------------------
void CI_ModuleRemoved(PCMCIA_HANDLE hModule, BYTE bSlotNum)
{
    CI_HANDLE hCI;
    BYTE msg[3];
    WORD i;

    hCI = &CiSlot[bSlotNum];

    // the PCMCIA handle <-> slot number assignment should not change
    CI_LOG_DEBG("[CI_ModuleRemoved] hCI->hSlotHandle(0x%08X) hCI(0x%08X) %d \r\n", 
            hCI->hSlotHandle, 
            hModule, 
            bSlotNum);
    assert(hCI->hSlotHandle == hModule);
    

    msg[0] = CI_MSG_CI_INFO;
    msg[1] = CI_SLOT_EMPTY;
    msg[2] = bSlotNum;
    SendCIMessage(hCI,msg,3);
        
    // close all opened resource connections
    for(i=0;i<MAX_CI_SESSIONS;i++)
    {
        if(hCI->CiSessions[i].fSlotInUse)
        {
            hCI->CiSessions[i].pResource->Close(hCI,i);
            hCI->CiSessions[i].fSlotInUse = FALSE;
        }
    }

    //Buffer reset  
    memset(hCI,0x00,sizeof(CI_HANDLESTRUCT));
}


/******************************************************************************
 * CI_IsProcessCAPMTPacket() - 
 * DESCRIPTION: - 
 *     根据自定义添加的结束标志，判断数据是否是CAPMT，是返回TRUE，否则返回FALSE
 * Input  : pucData         数据指针
 *          nDataLen        数据长度
 * Output : 
 * Returns: TRUE            是CAPMT
 *          FALSE           非CAPMT
 ******************************************************************************/
BOOL CI_IsProcessCAPMTPacket(const BYTE *pucData, int nDataLen)
{
    // vars
    BYTE    ucCheckEndFlag[4] = {0};
    //int     kk = 0;

    // check params
    if (pucData == NULL     || nDataLen < 4)
    {
        return FALSE;
    }
    
    // 如果帧0结尾包含有结束标志
    ucCheckEndFlag[0] = pucData[nDataLen-4];
    ucCheckEndFlag[1] = pucData[nDataLen-3];
    ucCheckEndFlag[2] = pucData[nDataLen-2];
    ucCheckEndFlag[3] = pucData[nDataLen-1];
    if ((ucCheckEndFlag[0] == WAV_CI_PMT_END_FLAG)      &&
        (ucCheckEndFlag[1] == WAV_CI_PMT_END_FLAG)      &&
        (ucCheckEndFlag[2] == WAV_CI_PMT_END_FLAG)      &&
        (ucCheckEndFlag[3] == WAV_CI_PMT_END_FLAG))
    {      
        //printf ("capmt(%d) end flag!! \r\n", nDataLen);
        return TRUE;
    }
    
    // 其它情况认为是正常包    
    return FALSE;
}


// -------------------------------------------------------------
void SendTransportPacket(CI_TPDUBUFHANDLE pBuf, BYTE ConnId, const BYTE *pData, int nLen)
{
    // vars 
    BYTE    Frame[512];
    int     nMaxtrytimes = 10;
    BOOL    bIsCAPMTPacket = 0;
    
    Frame[0]=ConnId;
    
    if(pBuf->wBufferSize < 16)
        return;    

#if defined(ENABLE_CI_SEMPHORE)
    while(WVCI_SUCCESS != WVCI_GetSemphore(50)) //Time out is 50*10 ms.
    {
        nMaxtrytimes--;
        CI_LOG_DEBG("[%s] Get semphore fail!!, retry(%d)... \r\n", __FUNCTION__, nMaxtrytimes);
        GURU_DelayMS(200);
        if (nMaxtrytimes < 0)
        {
            CI_LOG_ERR("[%s] Get semphore fail!! skip this packet\r\n", __FUNCTION__);
            return;
        }
    }   

    // check is pmt packet or not
    bIsCAPMTPacket = CI_IsProcessCAPMTPacket(pData, nLen);
    if (bIsCAPMTPacket)
    {
        nLen -= 4; // skip the capmt end flag (4 bytes)        
    }

#endif

    while(nLen > (pBuf->wBufferSize-2))
    {
        Frame[1]=0x80;
        memcpy(Frame+2, pData, pBuf->wBufferSize-2);
        CI_WriteData(pBuf, pBuf->wBufferSize, Frame, 0);
        pData += pBuf->wBufferSize-2;
        nLen -= pBuf->wBufferSize-2;
    }
    Frame[1]=0;
    memcpy(Frame+2, pData, nLen);
    CI_WriteData(pBuf, (WORD)(nLen+2), Frame, bIsCAPMTPacket);
    
#if defined(ENABLE_CI_SEMPHORE)
    WVCI_ReleaseSemphore();
#endif
}

// -------------------------------------------------------------
void Send_T_CREATE(CI_HANDLE hCI, BYTE bConnId)
{
    BYTE Msg[3];
    Msg[0]=CI_T_CREATE;
    Msg[1]=0x01;
    Msg[2]=bConnId;

    SendTransportPacket(&hCI->IOBuffer_Host2CI,bConnId,Msg,3);
}

// -------------------------------------------------------------
void Send_T_RCV(CI_HANDLE hCI, BYTE bConnId)
{
    BYTE Msg[3];
    Msg[0]=CI_T_RCV;
    Msg[1]=0x01;
    Msg[2]=bConnId;

    SendTransportPacket(&hCI->IOBuffer_Host2CI,bConnId,Msg,3);
}

// -------------------------------------------------------------
void Send_T_DataLast(CI_HANDLE hCI, BYTE bConnId, const BYTE *pData, WORD wLen)
{
    BYTE Msg[1024]; 
#ifdef DUMP_TX_TPDUS    
    CI_LOG_DEBG("Send_T_DataLast\r\n");
    if(wLen)
    {
        int i;
        CI_LOG_DEBG("Send TPDU DataLast: ");
        for(i=0;i<wLen;i++)
            CI_LOG_DEBG("%02X ",pData[i]);
        CI_LOG_DEBG("\r\n");
    }
#endif
    Msg[0]=CI_T_DATALAST;
    Msg[1]=(BYTE)(wLen+1);
    Msg[2]=bConnId;
    if(wLen)
        memcpy(Msg+3,pData,wLen);

    SendTransportPacket(&hCI->IOBuffer_Host2CI, bConnId, Msg, (WORD)(3+wLen));
}

// -------------------------------------------------------------
void Send_T_DataLong(CI_HANDLE hCI, BYTE bConnId, const BYTE *pData, WORD wLen)
{
    BOOL bIsCAPMTPacket = FALSE; 
    WORD wRealyMsgLen = 0;
    BYTE Msg[1024]; 
#ifdef DUMP_TX_TPDUS
    CI_LOG_DEBG("[Send_T_DataLong]Send_T_DataLong\r\n");
    if(wLen)
    {
        int i;
        CI_LOG_DEBG("[Send_T_DataLong]Send TPDU DataMore: ");
        for(i=0;i<wLen;i++)
            CI_LOG_DEBG("%02X ",pData[i]);
        CI_LOG_DEBG("\r\n");
    }
#endif

    // 针对NEOTION CAM不解扰问题修正:
    // 如果是PMT数据包，需要把上层添加的4个同步检查字节(0x47, 0x47, 0x47, 0x47)的长度去掉，再赋给Msg.Len．
    // 上层添加4个检查字节主要目的:在底层分Frame时，用于判断是否是PMT包．(如果是PMT包，增加发给CAM的时间间隔)
    // 实际与CAM的通信数据包是去掉了这4个同步字节的原始数据．　(0x47, 0x47, 0x47, 0x47)
    // 原则上不影响正常协议栈运行．
    bIsCAPMTPacket = CI_IsProcessCAPMTPacket(pData, wLen);
    if (bIsCAPMTPacket)
    {
        wRealyMsgLen = wLen - 4; 
    }
    else
    {
        wRealyMsgLen = wLen;
    }

    if(wLen+3>=0x80)
    {
        Msg[0]=CI_T_DATALAST;
        Msg[1]=0x82;
        Msg[2]=(BYTE)((wRealyMsgLen+1)>>8);
        Msg[3]=(BYTE)((wRealyMsgLen+1)&0xFF);
        Msg[4]=bConnId;
        memcpy(Msg+5, pData, wLen);
        SendTransportPacket(&hCI->IOBuffer_Host2CI, bConnId, Msg, (WORD)(wLen+5));
    }
    else
    {
        // small 
        Msg[0]=CI_T_DATALAST;
        Msg[1]=(BYTE)(wRealyMsgLen+1);
        Msg[2]=bConnId;
        if(wLen)
            memcpy(Msg+3, pData, wLen);
        SendTransportPacket(&hCI->IOBuffer_Host2CI, bConnId, Msg, (WORD)(3+wLen));
    }
}


///////////////////////////////////////////////////////////////////
// Session Layer
///////////////////////////////////////////////////////////////////

void Send_SPDU_OpenResponse(CI_HANDLE hCI, BYTE bConnID, BYTE bStatus, DWORD dwResource, CISESSIONHANDLE wSessionNumber)
{
    BYTE pFrame[9];
    pFrame[0]=CI_SESSION_OPEN_RESPONSE;
    pFrame[1]=7;
    pFrame[2]=bStatus;
    pFrame[3]=(BYTE)(dwResource>>24);
    pFrame[4]=(BYTE)(dwResource>>16);
    pFrame[5]=(BYTE)(dwResource>>8);
    pFrame[6]=(BYTE)(dwResource);
    pFrame[7]=(BYTE)(wSessionNumber>>8);
    pFrame[8]=(BYTE)(wSessionNumber);
    Send_T_DataLast(hCI,bConnID,pFrame,9);
}

void Send_SPDU_CloseResponse(CI_HANDLE hCI, BYTE bConnID, BYTE bStatus, CISESSIONHANDLE wSessionNumber)
{
    BYTE pFrame[9];
    pFrame[0]=CI_SESSION_CLOSE_RESPONSE;
    pFrame[1]=3;
    pFrame[2]=bStatus;
    pFrame[3]=(BYTE)(wSessionNumber>>8);
    pFrame[4]=(BYTE)(wSessionNumber);
    Send_T_DataLast(hCI,bConnID,pFrame,5);
}

void Send_SPDU_CloseRequest(CI_HANDLE hCI, BYTE bConnID, CISESSIONHANDLE wSessionNumber)
{
    BYTE pFrame[4];
    pFrame[0]=CI_SESSION_CLOSE_REQUEST;
    pFrame[1]=2;
    pFrame[2]=(BYTE)(wSessionNumber>>8);
    pFrame[3]=(BYTE)(wSessionNumber);
    Send_T_DataLast(hCI,bConnID,pFrame,4);
}

// -------------------------------------------------------------
void Send_SPDU_CreateSession(CI_HANDLE hCI, BYTE bConnID, DWORD dwResource, CISESSIONHANDLE wSessionNumber)
{
    BYTE pFrame[9];
    pFrame[0]=CI_SESSION_CREATE;
    pFrame[1]=6;
    pFrame[2]=(BYTE)(dwResource>>24);
    pFrame[3]=(BYTE)(dwResource>>16);
    pFrame[4]=(BYTE)(dwResource>>8);
    pFrame[5]=(BYTE)(dwResource);
    pFrame[6]=(BYTE)(wSessionNumber>>8);
    pFrame[7]=(BYTE)(wSessionNumber);
    Send_T_DataLast(hCI,bConnID,pFrame,8);
}

// -------------------------------------------------------------
void Send_SPDU_Data(CI_HANDLE hCI, BYTE bConnID, CISESSIONHANDLE wSessionNumber, const BYTE *pData, WORD wLen)
{
    BYTE pFrame[1024];
    pFrame[0]=CI_SESSION_DATA;
    pFrame[1]=2;
    pFrame[2]=(BYTE)(wSessionNumber>>8);
    pFrame[3]=(BYTE)(wSessionNumber);
    if(wLen)
        memcpy(pFrame+4, pData, wLen);

    Send_T_DataLong(hCI, bConnID, pFrame, (WORD)(wLen+4));
}

///////////////////////////////////////////////////////////////////
// Application Layer
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
// Session RX part

void Parse_SPDU(CI_HANDLE hCI, const BYTE *pData, DWORD dwLen)
{
    DWORD dwResourceID;
    WORD wSessionNumber;
    WORD wLen;
    int nOffset = 1;
    WORD SessNum=0;
    DWORD i;
    WORD hSession;
#ifdef DUMP_RX_SPDUS
    CI_LOG_DEBG("RX   SPDU: ");
    for(i=0;i<dwLen;i++)
        CI_LOG_DEBG("%02X ",pData[i]);
    CI_LOG_DEBG("\r\n");
#endif

    nOffset += CI_DecodeLengthField(&pData[nOffset],&wLen);
    switch(pData[0])
    {
    case CI_SESSION_OPEN_REQUEST:
        dwResourceID = pData[nOffset]<<24|pData[nOffset+1]<<16|pData[nOffset+2]<<8|pData[nOffset+3];
        CI_LOG_DEBG("[Parse_SPDU]CI_SESSION_OPEN_REQUEST %04X %08X\r\n",wLen,dwResourceID);
        for(i=1;i<MAX_CI_SESSIONS;i++) 
        {
            if(!hCI->CiSessions[i].fSlotInUse)
            {
                SessNum = (WORD)i;
                break;
            }
        }
        if(i==MAX_CI_SESSIONS) {
            CI_LOG_DEBG("[Parse_SPDU]Out of Session handles\r\n");
            Send_SPDU_OpenResponse(hCI,0x01,0xF0,dwResourceID,SessNum);
            break;
        }
        for(i=0;g_CiResources[i];i++) {
            if(g_CiResources[i]->dwResourceID == dwResourceID)
            {
                Send_SPDU_OpenResponse(hCI, 0x01, 0x00, dwResourceID, SessNum);
                hCI->CiSessions[SessNum].fSlotInUse = TRUE;
                hCI->CiSessions[SessNum].bTransportConnID = 0x01;
                hCI->CiSessions[SessNum].pResource = g_CiResources[i];
                hCI->CiSessions[SessNum].wSessionID = SessNum;
                if(hCI->CiSessions[SessNum].pResource->Connect(hCI, SessNum) == FALSE)
                {
                    CI_LOG_DEBG("[Parse_SPDU]Resource refused open request\r\n");
                    hCI->CiSessions[SessNum].fSlotInUse = FALSE;
                    hCI->CiSessions[SessNum].pResource = NULL;
                    Send_SPDU_OpenResponse(hCI, 0x01, 0xF0, dwResourceID, SessNum);
                }
                break;
            }
        }
        if(!g_CiResources[i]){
            CI_LOG_ERR("[Parse_SPDU]CI_SESSION_OPEN_REQUEST failed (unknown/unimplemented resource)\r\n");
            Send_SPDU_OpenResponse(hCI,0x01,0xF0,dwResourceID,SessNum);
        }
        break;
    case CI_SESSION_OPEN_RESPONSE:
        CI_LOG_DEBG("[Parse_SPDU]CI_SESSION_OPEN_RESPONSE\r\n");    
        break;
    case CI_SESSION_CREATE: 
        dwResourceID = pData[nOffset]<<24|pData[nOffset+1]<<16|pData[nOffset+2]<<8|pData[nOffset+3];
        wSessionNumber = (WORD)((pData[nOffset+4]<<8)|pData[nOffset+5]);
        //CI_DEBUG(("CI_SESSION_CREATE %02X %04X %02X\r\n",bLen,dwResourceID,wSessionNumber));  
        break;
    case CI_SESSION_CREATE_RESPONSE:
        CI_LOG_DEBG("[Parse_SPDU]CI_SESSION_CREATE_RESPONSE\r\n");  
        break;
    case CI_SESSION_CLOSE_REQUEST:
        CI_LOG_DEBG("[Parse_SPDU]CI_SESSION_CLOSE_REQUEST\r\n");    
        hSession = (WORD)((pData[nOffset]<<8)|pData[nOffset+1]);
        if(hSession >= MAX_CI_SESSIONS)
        {
            CI_LOG_ERR("[Parse_SPDU]CI is referencing invalid session handle\r\n");
            break;
        }
        if(hCI->CiSessions[hSession].pResource)
            hCI->CiSessions[hSession].pResource->Close(hCI,hSession);
        hCI->CiSessions[hSession].pResource = NULL;
        CI_LOG_DEBG("[Parse_SPDU]Releasing Session handle %d\r\n", (int)hSession);  
        hCI->CiSessions[hSession].fSlotInUse = FALSE;
        Send_SPDU_CloseResponse(hCI,0x01,0x00,hSession);
        break;
    case CI_SESSION_CLOSE_RESPONSE:
        CI_LOG_DEBG("[Parse_SPDU]CI_SESSION_CLOSE_RESPONSE\r\n");   
        hSession = (WORD)((pData[nOffset+1]<<8)|pData[nOffset+2]);
        if(hSession >= MAX_CI_SESSIONS)
        {
            CI_LOG_ERR("[Parse_SPDU]CI is referencing invalid session handle\r\n");
            break;
        }       
        CI_LOG_DEBG("[Parse_SPDU]Releasing Session handle %d\r\n", (int)hSession);  
        if(hCI->CiSessions[hSession].pResource)
            hCI->CiSessions[hSession].pResource->Close(hCI, hSession);
        hCI->CiSessions[hSession].pResource = NULL;

        hCI->CiSessions[hSession].fSlotInUse = FALSE;
        break;
    case CI_SESSION_DATA:           
        CI_LOG_DEBG("[Parse_SPDU]CI_SESSION_DATA:\r\n");
        /*
        for(i=0;i<dwLen;i++)
            LogPrint(LOG_TYPE_DEBUG, LOG_MODULE_CI,"%02X ",pData[i]);
        LogPrint(LOG_TYPE_DEBUG, LOG_MODULE_CI,"\r\n");
        for(i=0;i<dwLen;i++)
            LogPrint(LOG_TYPE_DEBUG, LOG_MODULE_CI,"%c  ",pData[i]);
        LogPrint(LOG_TYPE_DEBUG, LOG_MODULE_CI,"\r\n");
        */
        hSession = (WORD)((pData[nOffset]<<8)|pData[nOffset+1]);
        nOffset+=2;
        if((hSession>=MAX_CI_SESSIONS)||(!hCI->CiSessions[hSession].fSlotInUse))
        {
            CI_LOG_ERR("[Parse_SPDU] hCI=%08X Data4Session %04X\r\n",hCI,hSession);
            CI_LOG_ERR("[Parse_SPDU] Fatal Error, invalid Session handle\r\n");
        } else {
            if(hCI->CiSessions[hSession].pResource)
                hCI->CiSessions[hSession].pResource->Data(hCI,hSession,pData+nOffset,dwLen-nOffset);
            else {
                CI_LOG_ERR("[Parse_SPDU] CI_SESSION_DATA referencing closed session\r\n");           
            }
        }
        break;
    default:
        CI_LOG_ERR("[Parse_SPDU]Unknown SPDU,Tag %02X:\r\n",pData[0]);
        for(i=0;i<dwLen;i++)
            CI_LOG_ERR("%02X ",pData[i]);
        CI_LOG_ERR("\r\n");
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
// Transport RX part

void Parse_TPDU(CI_HANDLE hCI, const BYTE *pData, DWORD dwLen)
{
    DWORD ObjPtr=0;
    int DataPtr=0;
    DWORD nDataLen;

#ifdef DUMP_RX_TPDUS
    {
        DWORD i;
        CI_LOG_DEBG("[Parse_TPDU]RX   TPDU: ");
        for(i=0;i<dwLen;i++)
            CI_LOG_DEBG("%02X ",pData[i]);
        CI_LOG_DEBG("\r\n");
    }
#endif

    while(ObjPtr<dwLen)
    {
        int nLengthBytes=1;
        if(pData[ObjPtr+1]&0x80) {
            nLengthBytes=pData[ObjPtr+1]&0x7F;
            if(nLengthBytes==0 || (nLengthBytes>2)) {
                CI_LOG_DEBG("[Parse_TPDU]Invalid TPDU Length (%d)\r\n",nLengthBytes);
                return;
            }
            if(nLengthBytes == 1)
                nDataLen = pData[ObjPtr+2]-1;
            else
                nDataLen = (pData[ObjPtr+2]<<8|pData[ObjPtr+3])-1;
            DataPtr = ObjPtr+2+nLengthBytes+1;
            nLengthBytes++;
            //CI_DEBUG(("Long Data %04X\r\n",nDataLen));
        } else {
            DataPtr = ObjPtr+2+1;
            nDataLen = pData[ObjPtr+1]-1;
        }
        switch(pData[ObjPtr])
        {
        case CI_T_CREATE_REPLY:
            CI_LOG_DEBG("[Parse_TPDU] Create connection Reply\r\n");
            break;
        case CI_T_RCV:
            CI_LOG_DEBG("[Parse_TPDU] Rcv");
            break;
		case CI_T_DATAMORE:
            CI_LOG_DEBG("[Parse_TPDU] DataMore\r\n");
            if(hCI->dwRXTPDULen+nDataLen < sizeof(hCI->bRXTPDUBuf))
            {
                memcpy(hCI->bRXTPDUBuf+hCI->dwRXTPDULen,pData+DataPtr,nDataLen);
                hCI->dwRXTPDULen += nDataLen;
            } else {
                assert(0);
                CI_LOG_ERR("[Parse_TPDU] DataMore too long\r\n");
            }
            break;
        case CI_T_DATALAST:
            //CI_DEBUG(("Parse_TPDU: DataLast\r\n"));
            // next layer
            if(hCI->dwRXTPDULen)
            {
                if(hCI->dwRXTPDULen+nDataLen < sizeof(hCI->bRXTPDUBuf))
                {
                    memcpy(hCI->bRXTPDUBuf+hCI->dwRXTPDULen,pData+DataPtr,nDataLen);
                    hCI->dwRXTPDULen += nDataLen;
                    Parse_SPDU(hCI,hCI->bRXTPDUBuf,hCI->dwRXTPDULen);
                } else {
                    assert(0);
                    CI_LOG_ERR("[Parse_TPDU] DataLast too long\r\n");
                }
                hCI->dwRXTPDULen = 0;
            } else {
                Parse_SPDU(hCI,pData+DataPtr,nDataLen);
            }
            break;
        case CI_T_SB:
            hCI->fGotPollAck = TRUE;
            //CI_DEBUG(("Parse_TPDU: T_SB ConnId %02X",pData[ObjPtr+2]));
            if(pData[ObjPtr+3]&0x80)
            {
                //CI_DEBUG(("DataReady"));
                Send_T_RCV(hCI,pData[ObjPtr+2]);
            }
            break;
#if 0 // this is no longer necessary with the enforced 1:1 poll:ack
        case CI_SESSION_DATA:
            // this is a workaround for modules that send a SPDU on
            // the wrong layer of the stack.
            if(  (pData[ObjPtr+0] == 0x90)
               &&(pData[ObjPtr+1] == 0x02))
            {
                printf("Bad protocol workaround: Len %08X\r\n",pData[ObjPtr+7]);
                // pass the SPDU up
                // the length byte position is calculated on the assumption
                // that there is a 4 byte T_session_number followed
                // by a 3 byte tag
                Parse_SPDU(hCI,pData+ObjPtr,pData[ObjPtr+7]+3+4);
            }
            break;
#endif
        default:{
            BYTE i;
            CI_LOG_ERR("[Parse_TPDU] Unknown TPDU Code %02X\r\n",pData[ObjPtr]);
            
            for(i=0;i<5;i++)
                if (pData[i] != 0x0A) 
                    break;
            if (i>3)
                hCI->bUnknown_TPDU=3;
            else
                hCI->bUnknown_TPDU++;
            }
            return;
        }
        hCI->bUnknown_TPDU=0;
        ObjPtr+=nDataLen+2+nLengthBytes;
    }
}


// -------------------------------------------------------------
// returns if the tpdu is incomplete
BOOL Parse_CI_T_Packet(CI_HANDLE hCI, const BYTE *pData, WORD wLen)
{
    if(hCI->TPDULen+wLen > MAX_CI_TPDU) {
        hCI->TPDULen = 0;
        return FALSE;
    }
    memcpy(hCI->TPDU+hCI->TPDULen, pData+2, wLen-2);
    hCI->TPDULen += wLen-2;
    if(pData[1]&0x80) {
        // Multipart TPDU

        return TRUE;
    }
    Parse_TPDU(hCI, hCI->TPDU, hCI->TPDULen);
    if (hCI->bUnknown_TPDU > 2)
    {
        CI_LOG_DEBG("[Parse_CI_T_Packet] too many unknown tpdus - Modul Reset\r\n");
        hCI->ModulReset=1;          
    }                   
    hCI->TPDULen=0;
    return FALSE;
}

WORD CI_DecodeLengthField(const BYTE *pData,WORD *pwLength)
{
    if(pData[0]&0x80)   //with size_indicator
    {
        //CI_DEBUG(("Length field %02X\r\n",pData[0]));

        // length_size == 1 byte
        if((pData[0]&0x7F)==0x01)   
        {
            //CI_DEBUG(("Length size %02X\r\n",pData[1]));
            *pwLength = pData[1];
            return 2;
        }
        // length_size == 2 byte
        if((pData[0]&0x7F)==0x02)   
        {
            //CI_DEBUG(("Length field %02X%02X\r\n",pData[1],pData[2]));
            *pwLength = (WORD)((pData[1]<<8)|pData[2]);
            return 3;
        }
        // error
        // this is not defined
        CI_LOG_ERR("[CI_DecodeLengthField] Invalid Length field %02X\r\n",pData[0]);
        *pwLength = (WORD)((pData[1]<<8)|pData[2]);
        return (WORD)((pData[0]&0x7F)+1);
    }
    else // no size_indicator, first byte=length
    {
        *pwLength = pData[0];
        return 1;
    }
}


int CI_SendCAPMT2CAM(BYTE ucSlotIndex, const BYTE *pucCAPMT, WORD wCAPMTLen)
{
    CI_HANDLE   hCI = NULL;

    if (ucSlotIndex > WVCI_MAX_SLOT_NUM ||
        pucCAPMT == NULL                ||
        wCAPMTLen == 0                  ||
        wCAPMTLen >= MAX_LEN_CA_PMT)
    {
        return -1;
    }
    
    hCI = &CiSlot[ucSlotIndex];
    CI_LOG_INF("[CI_SendCAPMT2CAM] ca_pmt Len:%d\r\n", (int)wCAPMTLen);
    CaSupport_SendPMT(hCI, pucCAPMT, wCAPMTLen);
    return 0;
}


BOOL CI_RegisterResource(const RESOURCE_INFO *pRes)
{
    int i;
    CI_LOG_DEBG("[CI_RegisterResource] resource ID:%06X\r\n",pRes->dwResourceID);
    // already registered ?
    for(i=0;i<MAX_CI_RESOURCES;i++)
    {
        if(g_CiResources[i] == pRes)
        {
            CI_LOG_ERR("[CI_RegisterResource]pRes alredy registered!!\r\n");
            return FALSE;
        }
    }
    for(i=0;i<MAX_CI_RESOURCES;i++)
    {
        if(!g_CiResources[i])
        {
            g_CiResources[i] = pRes;
            return TRUE;
        }
    }
    // list full
    CI_LOG_ERR("[CI_RegisterResource]list full!!\r\n");
    return FALSE;
}

BOOL CI_UnregisterResource(const RESOURCE_INFO *pRes)
{
    int i;
    CI_LOG_DEBG("[CI_UnregisterResource] entry!\r\n");
    // search
    for(i=0;i<MAX_CI_RESOURCES;i++)
    {
        if(g_CiResources[i] == pRes) {
            // clear the entry
            g_CiResources[i] = NULL;
            return TRUE;
        }
    }
    // not found
    return FALSE;
}


BOOL CI_Init(void)
{
    CI_LOG_DEBG("[CI_Init] entry!\r\n");
    memset((void*)g_CiResources,0x00,sizeof(g_CiResources));
    memset(CiSlot,0x00,sizeof(CiSlot));
    
    CI_RegisterResource(&ResMgr);
    CI_RegisterResource(&AppInfo);
#ifdef ENABLE_CI_HOSTCONTROL
    CI_RegisterResource(&HostControl);
#endif
    CI_RegisterResource(&CaSupport);
    CI_RegisterResource(&DateTime);
    CI_RegisterResource(&MMI);

#ifndef ENABLE_CIPLUS
    CI_RegisterResource(&AppInfoV2);
#else
    CI_RegisterResource(&AppInfoV3);
#endif // ENABLE_CIPLUS
    
#ifdef ENABLE_CI_PROFILE2
    CI_RegisterResource(&ResMgrV2);
#endif
#ifdef ENABLE_CI_COPYPROTECTION
    CI_RegisterResource(&CopyProtection);
#endif
#ifdef ENABLE_CI_POWERMANAGEMENT
    CI_RegisterResource(&PowerManagement);
#endif
#ifdef ENABLE_CI_STATUSQUERY
    CI_RegisterResource(&StatusQuery1);
#endif
#ifdef ENABLE_CI_EVENTMGMT
    CI_RegisterResource(&EventManagement1);
#endif
#ifdef ENABLE_CI_APPLICATIONMMI
    CI_RegisterResource(&ApplicationMMI);
#endif

#if defined(ENABLE_FINGER_PRINT)
    CI_RegisterResource(&FingerPrint);
#endif

#ifdef ENABLE_CIPLUS
    return CIPlus_Init();
#else
    return TRUE;
#endif
}



#endif // ENABLE_CI
