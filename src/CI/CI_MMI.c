// ********************************************************
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
#include "all.h"
#include "WVCI_PlatformDriver.h"

// @file CI_MMI.c
// This file contains the implementation of the MMI resource.

#define MMI_DEBUG_ENABLE

#ifdef MMI_DEBUG_ENABLE
#define MMI_DEBUG(x) printf(x)
////FAILRELEASEBUILD
#else
#define MMI_DEBUG(x)
#endif



#ifdef ENABLE_CI

static BOOL MMI_Connect(CI_HANDLE hCI, WORD hSession);
static void MMI_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen);
static void MMI_Close(CI_HANDLE hCI, WORD hSession);
static const BYTE MMI_CLOSE[4]={0x9f,0x88,0x00,00};

static const BYTE MMIDispControlResp[6]={0x9f,0x88,0x02,0x02,0x01,0x01};
static const BYTE APPINFO_ENTERMENU[4]={0x9f,0x80,0x22,00};

const RESOURCE_INFO MMI ={ RESOURCEID_MMI,MMI_Connect,MMI_Data,MMI_Close };



BOOL MMI_Connect(CI_HANDLE hCI, WORD hSession)
{
    CI_LOG_DEBG("[MMI_Connect]MMI Connect (Session handle %04X)\r\n", hSession);
    // allow only one session
    if(hCI->dwMMISession) {
        CI_LOG_ERR("[MMI_Connect]MMI ERROR, only one session supported\r\n");
        return FALSE;
    }
    hCI->dwMMISession = hSession;
    return TRUE;
}


void MMI_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen)
{
    BYTE *msg;
    int i;
    int items_length,msgPos;
    DWORD dwTag;
    WORD wLen;
    int itemcount = 0;
    int nOffset = 3;
    BYTE bMmiText[512] = {0};
    dwTag = pData[0]<<16|pData[1]<<8|pData[2];
    CI_LOG_DEBG("[MMI_Data] MMI_Data %06X\r\n",dwTag);
    nOffset += CI_DecodeLengthField(&pData[nOffset],&wLen);

    if(dwTag==TAG_T_CLOSEMMI)
    {
        CI_LOG_DEBG("[MMI_Data] MMI Data CLOSE Cmd:%02X Delay:%02X\r\n",pData[nOffset],pData[nOffset+1]);   
        
        Send_SPDU_CloseRequest(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID);
        
        {
            BYTE msg[4];
            msg[0] = CI_MMI_CLOSE_MMI_IMM;
            msg[1] = hCI->bSlotNum;
            msg[2] = (BYTE)(hSession>>8);
            msg[3] = (BYTE)(hSession&0xFF);
            SendCIMessage(hCI,msg,4);
        }
        hCI->dwMMISession = 0;                      
        
    }
    if(dwTag==TAG_T_DISPLAYCONTROL)
    {
        // Warning: Some modules only send the first display_control_cmd byte
        // and not the MMI_mode byte
    
        CI_LOG_DEBG("[MMI_Data] Display Control Cmd:%02X\r\n",pData[nOffset]);
        if(pData[nOffset]==0x01)
        {
            CI_LOG_DEBG("[MMI_Data] Display Control MMI Mode:%02X\r\n",pData[nOffset+1]);
        }
        Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,MMIDispControlResp,6);
    }

    if(dwTag==TAG_T_ENQ)
    {
        CI_LOG_INF("[MMI_Data] MMI Enquiry: User Input, Choices:%02X\r\n",pData[nOffset]);
        CI_LOG_INF("[MMI_Data] Blind Answer %02X\r\n",pData[nOffset]&0x01);
        items_length = pData[nOffset+1];
        CI_LOG_INF("[MMI_Data] Answer Text Length %02X\r\n",pData[nOffset+1]);
        CI_LOG_INF("[MMI_Data] Text: ");
        for(i=0;i<wLen-2;i++)
            CI_LOG_INF("%c",pData[nOffset+2+i]);
        CI_LOG_INF("\r\n");
        
        msg = (BYTE *)malloc((wLen+6) * (sizeof(BYTE)));
        if( msg != NULL )
        {
            memset(msg, 0x00, wLen+6);
        
            msg[0] = CI_MMI_REQUEST_INPUT;
            msg[1] = hCI->bSlotNum;
            msg[2] = (BYTE)(hSession>>8);
            msg[3] = (BYTE)(hSession&0xFF);
            msg[4] = (BYTE)(pData[nOffset]&0x01);   //PIN BLIND
            msg[5] = pData[nOffset+1];              //Answer Length
            for(i=0;i<wLen-2;i++)
            {
                if(   (pData[nOffset+2+i] != 0x86)
                    &&(pData[nOffset+2+i] != 0x87)
                    &&(pData[nOffset+2+i] != 0x8E)
                    &&(pData[nOffset+2+i] != 0x93)
                    &&(pData[nOffset+2+i] != 0x96)
                    &&(pData[nOffset+2+i] != 0x97)
                    )
                    msg[6+i] = pData[nOffset+2+i];
                else
                    msg[6+i] = 0x20;
            }
            msg[6+i]=0;
        
            SendCIMessage(hCI, msg, wLen+6-1);
            free(msg);
        }   
    }   
    if(dwTag==TAG_T_MENULAST)
    {
        BYTE choice=0;
        CI_LOG_DEBG("[MMI_Data] MMI: Menu Last, Choices:%02X\r\n",pData[nOffset]);
        CI_LOG_DEBG("[MMI_Data] LEN %04X %04X \r\n",dwLen,pData[nOffset+1]);

        choice=pData[nOffset];
        items_length = 0;
        
        nOffset++;
        while(nOffset<(int)dwLen)
        {
            DWORD dwTag;
            //DWORD k;
            WORD wTagLen;
            WORD wLengthFieldLen;
            int i = nOffset;
            dwTag = pData[i]<<16|pData[i+1]<<8|pData[i+2];
            CI_LOG_INF("[MMI_Data] Tag %06X Len %02X\r\n ",dwTag,pData[i+3]);            
            wLengthFieldLen = CI_DecodeLengthField(&pData[i+3],&wTagLen);
            if(dwTag == TAG_T_TEXTLAST)
                items_length+=(wTagLen+wLengthFieldLen);

            memset(bMmiText,0,512);
            if(wTagLen < sizeof(bMmiText))
            {
                memcpy(bMmiText,pData + i + 3 + wLengthFieldLen,wTagLen);
                bMmiText[wTagLen] = '\0';
                CI_LOG_INF("[MMI_Data] mmi text:%s\r\n",bMmiText);
            }
            nOffset+=wTagLen+3+wLengthFieldLen;
        }
        
        nOffset = 3+CI_DecodeLengthField(&pData[3],&wLen);
        
        msg = (BYTE *)malloc((items_length+7) * (sizeof(BYTE)));
        if( msg != NULL )
        {
            memset(msg, 0xFF, items_length+7);
        
            msg[0] = CI_MMI_MENU;
            msg[1] = hCI->bSlotNum;
            msg[2] = (BYTE)(hSession>>8);
            msg[3] = (BYTE)(hSession&0xFF);
            msg[4] = 0x00;
            msg[5] = (BYTE)(pData[nOffset]+3);
            CI_LOG_DEBG("[MMI_Data] MSG[3]: %02X\r\n",msg[3]);
        
            nOffset++;
            msgPos=6;
            while(nOffset<(int)dwLen)
            {
                DWORD dwTag;
                DWORD k;
                int i = nOffset;
                WORD wTagLen;
                int nLengthFieldLen;
                dwTag = pData[i]<<16|pData[i+1]<<8|pData[i+2];
                nLengthFieldLen = CI_DecodeLengthField(&pData[i+3],&wTagLen);
            
                if(dwTag == TAG_T_TEXTLAST)
                {
                    for(k=0;k<wTagLen;k++)
                    {
                        // conax garbage filtering
                        if(   (pData[i+3+k+nLengthFieldLen] != 0x86)
                            &&(pData[i+3+k+nLengthFieldLen] != 0x87)
                            &&(pData[i+3+k+nLengthFieldLen] != 0x8E)
                            &&(pData[i+3+k+nLengthFieldLen] != 0x93)
                            &&(pData[i+3+k+nLengthFieldLen] != 0x96)
                            &&(pData[i+3+k+nLengthFieldLen] != 0x97)
                            )
                        {
                            msg[msgPos] = pData[i+3+k+nLengthFieldLen];
                            msgPos++;
                        }
                        //LogPrint(LOG_TYPE_INF, LOG_MODULE_CI,"%c",pData[i+3+k+nLengthFieldLen]);
                    }
                    //LogPrint(LOG_TYPE_INF, LOG_MODULE_CI,"\r\n");
                    msg[msgPos] = 0x00;
                    msgPos++;
                    itemcount++;
                }
                nOffset+=wTagLen+3+nLengthFieldLen;
            }
            msg[4]=(BYTE)(itemcount);
            msg[msgPos] = 0x00;
            msgPos++;
            
            SendCIMessage(hCI, msg, items_length+7);
    
            if ((choice==0x00) && (strstr((char*)&msg[22],"Receiver nicht f")==((char*)&msg[22])))
            {
                hCI->fHighPrio = FALSE;
                hCI->fIsPremiereModule = 1;
                MMI_DEBUG(("[MMI_Data] Receiver rejected\r\n"));
            }
        
            free(msg);
        }   
    }
    if(dwTag==TAG_T_MENUMORE)
    {
        CI_LOG_DEBG("[MMI_Data] Menu More, Choices:%02X\r\n",pData[nOffset]);
        CI_LOG_DEBG("[MMI_Data] LEN %04X %04X",dwLen,pData[5]);
    }   
    if(dwTag==TAG_T_LISTLAST)
    {
        CI_LOG_INF("[MMI_Data] List Last, Choices:%02X\r\n",pData[nOffset]);
        items_length = 0;
        
        nOffset++;
        while(nOffset<(int)dwLen)
        {
            DWORD dwTag;
            DWORD k;
            WORD wTagLen;
            int nLengthFieldLen;
            int i = nOffset;
            dwTag = pData[i]<<16|pData[i+1]<<8|pData[i+2];
            nLengthFieldLen = CI_DecodeLengthField(&pData[i+3],&wTagLen);
            if(dwTag == TAG_T_TEXTLAST)
                items_length+=(wTagLen+1);
            for(k=0;k<wTagLen;k++) {
                CI_LOG_INF("%c",pData[i+3+k+nLengthFieldLen]);
            }
            CI_LOG_INF("\r\n");
            nOffset+=wTagLen+3+nLengthFieldLen;
        }
        
        nOffset = 3+CI_DecodeLengthField(&pData[3],&wLen);
        
        msg = (BYTE *)malloc((items_length+7) * (sizeof(BYTE)));
        if( msg != NULL )
        {
            memset(msg, 0xFF, items_length+7);
        
            msg[0] = CI_MMI_LIST;
            msg[1] = hCI->bSlotNum;
            msg[2] = (BYTE)(hSession>>8);
            msg[3] = (BYTE)(hSession&0xFF);
            msg[4] = 0x00;
            msg[5] = (BYTE)(pData[nOffset]+3);
            CI_LOG_INF("[MMI_Data] MSG[3]: %.2X%.2X\r\n",msg[2],msg[3]);
        
            nOffset++;
            msgPos=6;
            while(nOffset<(int)dwLen)
            {
                DWORD dwTag;
                DWORD k;
                int i = nOffset;
                WORD wTagLen;
                int nLengthFieldLen;
                dwTag = pData[i]<<16|pData[i+1]<<8|pData[i+2];
                nLengthFieldLen = CI_DecodeLengthField(&pData[i+3],&wTagLen);
                if(dwTag == TAG_T_TEXTLAST)
                {
                    for(k=0;k<wTagLen;k++)
                    {
                        // conax garbage filtering
                        if(   (pData[i+3+k+nLengthFieldLen] != 0x86)
                            &&(pData[i+3+k+nLengthFieldLen] != 0x87)
                            &&(pData[i+3+k+nLengthFieldLen] != 0x8E)
                            &&(pData[i+3+k+nLengthFieldLen] != 0x93)
                            &&(pData[i+3+k+nLengthFieldLen] != 0x96)
                            &&(pData[i+3+k+nLengthFieldLen] != 0x97)
                            )
                        {
                            msg[msgPos] = pData[i+3+k+nLengthFieldLen];
                            msgPos++;
                        }
                        CI_LOG_INF("%c",pData[i+3+k+nLengthFieldLen]);
                    }
                    CI_LOG_INF("\r\n");
                    msg[msgPos] = 0x00;
                    msgPos++;
                    itemcount++;
                }
                nOffset+=wTagLen+3+nLengthFieldLen;     
            }
            msg[4] = (BYTE)(itemcount);
            msg[msgPos] = 0x00;
            msgPos++;
            SendCIMessage(hCI,msg,items_length+7);//3
            free(msg);
        }
    }
    if(dwTag==TAG_T_LISTMORE)
    {
        CI_LOG_DEBG("[MMI_Data] List More, Choices:%02X\r\n",pData[nOffset]);
    }

    switch(dwTag)
    {
    case TAG_T_KEYPADCONTROL:
    case TAG_T_SUBTITLESEGMENTLAST:
    case TAG_T_SUBTITLESEGMENTMORE:
    case TAG_T_SUBTITLEDOWNLOADLAST:
    case TAG_T_SUBTITLEDOWNLOADMORE:
    case TAG_T_DISPLAYMESSAGE:
    case TAG_T_SCENEENDMARK:
    case TAG_T_SCENECONTROL:
    case TAG_T_SCENEDONE:
    case TAG_T_FLUSHDOWNLOAD:
    case TAG_T_DOWNLOADREPLY:
        // LowLevel MMI
        break;
    }

    


}
// -------------------------------------------------------------
void MMI_Close(CI_HANDLE hCI, WORD hSession)
{
    BYTE slot=0;
    BYTE msg[4];
    msg[0] = CI_MMI_CLOSE_MMI_IMM;
    msg[1] = hCI->bSlotNum;
    msg[2] = (BYTE)(hSession>>8);
    msg[3] = (BYTE)(hSession&0xFF);
    SendCIMessage(hCI,msg,4);//3

    CI_LOG_DEBG("[MMI_Close] Session handle %04X\r\n", hSession);
    hCI->dwMMISession = 0;
    
    if (hCI == &CiSlot[1])
        slot=1;

}




void CI_MMI_Close(BYTE bSlotIndex)
{
    WORD hSession;
    CI_HANDLE hCI;
    if(bSlotIndex>MAX_CI_SLOTS)
        return;
    CI_LOG_DEBG("[CI_MMI_Close] slot index: %d\r\n",bSlotIndex);
    hCI = &CiSlot[bSlotIndex];
    hSession = hCI->dwMMISession;
    Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,MMI_CLOSE,4);
}

void CI_MMI_EnterMenu(BYTE bSlotIndex)
{
    WORD hSession;
    CI_HANDLE hCI;  
    if(bSlotIndex>MAX_CI_SLOTS)
        return;
    hCI = &CiSlot[bSlotIndex];
    CI_LOG_DEBG("[CI_EnterMenu] slot index: %d ,AppInfoSession %04X\r\n", bSlotIndex, hCI->wAppInfoSession);

    if (0 == hCI->wAppInfoSession)
    {
        CI_LOG_ERR("[CI_EnterMenu] BAD PARAMS, WAIT FOR INIT SUCCESS!!\r\n");
        return;
    }
    
    hSession = hCI->wAppInfoSession;
    Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,APPINFO_ENTERMENU,4);
}

void CI_MMI_AnswerMenu(BYTE bSlotIndex, BYTE choice)
{
    CI_HANDLE hCI;
    WORD hSession;
    BYTE MMI_MENUANSWER[5]={0x9f,0x88,0x0B,01};
    if(bSlotIndex>MAX_CI_SLOTS)
        return;
    CI_LOG_DEBG("[CI_MMI_AnswerMenu] slot index: %d,choice: %d\r\n",bSlotIndex,choice);
    MMI_MENUANSWER[4]=choice;
    hCI = &CiSlot[bSlotIndex];
    hSession = CiSlot[bSlotIndex].dwMMISession;
    Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,MMI_MENUANSWER,5);       
}

void CI_MMI_AnswerInput(BYTE bSlotIndex, const BYTE *pBuffer, BYTE nLength)
{
    CI_HANDLE hCI;
    BYTE *msg;
    BYTE i;
    WORD hSession;
    if(bSlotIndex>MAX_CI_SLOTS)
        return;
    CI_LOG_DEBG("[CI_MMI_AnswerInput] entry!\r\n");
    
    msg = (BYTE *)malloc((nLength+5) * (sizeof(BYTE)));
    if( msg != NULL )
    {
        memset(msg, 0x00, nLength+5);
        
        msg[0]=0x9f;
        msg[1]=0x88;    
        msg[2]=0x08;    

        if(pBuffer == NULL)
        {
            CI_LOG_DEBG("[CI_MMI_AnswerInput] sending cancel message\r\n");
            msg[3]=1;   
            msg[4]=0x00;        // cancel
        } else {
            CI_LOG_DEBG("[CI_MMI_AnswerInput] sending answer message\r\n");
            msg[3]=(BYTE)(nLength+1);
            msg[4]=0x01;        // answer   

            for(i=0;i<nLength;i++)
                msg[5+i]=pBuffer[i];
        }

        hCI = &CiSlot[bSlotIndex];
        hSession = CiSlot[bSlotIndex].dwMMISession;
        Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,msg, (WORD)(nLength+5));
        free(msg);
    }   
}

#endif // ENABLE_CI


