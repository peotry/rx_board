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


#ifdef ENABLE_CI

////#define DUMP_CA_IDS
////#define CASUPPORT_DEBUG_ENABLE 

#ifdef CASUPPORT_DEBUG_ENABLE 
#define CASUPPORT_DEBUG(x) printf x
////FAILRELEASEBUILD
#else
#define CASUPPORT_DEBUG(x)
#endif





static BOOL CaSupport_Connect(CI_HANDLE hCI, WORD hSession);
static void CaSupport_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen);
static void CaSupport_Close(CI_HANDLE hCI, WORD hSession);

const BYTE CAINFO_ENQ[4]={0x9f,0x80,0x30,00};
const BYTE CAPMT_TAG_REPLY[4]={0x9f,0x80,0x33,00};

const RESOURCE_INFO CaSupport =     { RESOURCEID_CASUPPORT,CaSupport_Connect,CaSupport_Data,CaSupport_Close };


///////////////////////////////////////////////////////////////////////////////////////////
// Ca Support

BOOL CaSupport_Connect(CI_HANDLE hCI, WORD hSession)
{
    CI_LOG_DEBG("[CaSupport_Connect] entry!\r\n");
    hCI->dwCASupportSession = hSession;
    Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,CAINFO_ENQ,4);
    return TRUE;
}

void CaSupport_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen)
{
    DWORD dwTag;
    int nOffset = 3;
    WORD wLen;
    
    nOffset += CI_DecodeLengthField(&pData[nOffset],&wLen);

    dwTag = pData[0]<<16|pData[1]<<8|pData[2];
    CI_LOG_DEBG("[CaSupport_Data] CaSupport %06X\r\n",dwTag);
    if(dwTag == TAG_T_CA_INFO)
    {
        DWORD i;
#ifdef DUMP_CA_IDS
        CI_LOG_DEBG("*******************************************************************\r\n");
        CI_LOG_DEBG("**                                                               **\r\n");
//      LogPrint(LOG_TYPE_DEBUG, LOG_MODULE_CI,"**              Dieses Modul wurde erfolgreich erkannt           **\r\n");
//      LogPrint(LOG_TYPE_DEBUG, LOG_MODULE_CI,"**           Die Liste der unterstützten CA Systeme folgt        **\r\n");
        CI_LOG_DEBG("**            This module has been detected successfully         **\r\n");
        CI_LOG_DEBG("**            The list of suppported CA systems follows:         **\r\n");
        CI_LOG_DEBG("**                                                               **\r\n");
        CI_LOG_DEBG("*******************************************************************\r\n");
        CI_LOG_DEBG("CA Info: ");
#endif
        hCI->wNumCASystems = 0;
        for(i=nOffset;i<dwLen;i+=2)
        {
            if(i<MAX_CASYS_IDS-1) {
            hCI->wCASystemIDs[hCI->wNumCASystems] = (WORD)((pData[i]<<8)|pData[i+1]);
            hCI->wNumCASystems++;
            } else {
                CI_LOG_ERR("To many CA system IDs\r\n");
            }
#ifdef DUMP_CA_IDS
            CI_LOG_DEBG("************************************************\r\n");
            CI_LOG_DEBG("***  The CA System ID of the CAM is %02X%02X ***\r\n",pData[i],pData[i+1]);
            CI_LOG_DEBG("************************************************\r\n");
#endif
        }
        CI_LOG_DEBG("[CaSupport_Data] hCI->wNumCASystems: %d\r\n", hCI->wNumCASystems); 

#if 0 // add debug for customer of terry: shouquan.tang test only, it is not public version 20130130
{
    // check exist or not
    int  tt = 0; 
    int  found = 0;
    for (tt = 0; tt < hCI->wNumCASystems; tt++)
    {
        if (hCI->wCASystemIDs[hCI->wNumCASystems] == 0x4ab0)
        {
            found = 1;
            break;
        }
    }
    // if no exist, add it(0x4ab0) force
    if (found != 1)
    {
        hCI->wCASystemIDs[hCI->wNumCASystems] = 0x4ab0;
        hCI->wNumCASystems++;
    }
}
#endif
        
        {   
        BYTE *pMsg;
        WORD wMsgLen;
        wMsgLen = (WORD)(4+(hCI->wNumCASystems*2));
        pMsg = (BYTE *)malloc( wMsgLen * (sizeof(BYTE))+10 );
        if( pMsg != NULL )
        {
            memset(pMsg, 0x00, wMsgLen);
            pMsg[0] = CI_MSG_CI_INFO;
            pMsg[1] = CI_SLOT_MODULE_CA_OK;
            pMsg[2] = hCI->bSlotNum;
            
            pMsg[3]=(BYTE)hCI->wNumCASystems;
            for(i=0;(int)i<hCI->wNumCASystems;i++)
            {
                pMsg[4+2*i]   = (BYTE)(hCI->wCASystemIDs[i]>>8);
                pMsg[4+2*i+1] = (BYTE)(hCI->wCASystemIDs[i]&0xFF);
            }
            SendCIMessage(hCI, pMsg, wMsgLen);
            free(pMsg);
        }
        }


#ifdef ENABLE_AUTOMATIC_PMT_SEND
        // if we already have a PMT
        if(g_wPMTLen)
        {
            BYTE  CAPMTBuf[MAX_LEN_CA_PMT];
            WORD  wCAPMTLen = 0;
            
            CI_LOG_DEBG("[CaSupport_Data] Generating PMT\r\n"); 
            //wCAPMTLen = Generate_CA_PMT(CAPMTBuf,g_PMTBuf,g_wPMTLen,hCI,CA_PMT_LIST_MANAGEMENT_ONLY);
            CI_LOG_DEBG("[CaSupport_Data] Sending PMT\r\n"));   
            CaSupport_SendPMT(hCI,CAPMTBuf,wCAPMTLen);
            
            hCI->fHighPrio = FALSE; 
        }
#endif
        
        CI_LOG_DEBG("[CaSupport_Data] Sent PMT\r\n");   
                
    } 
    if(dwTag == TAG_T_CA_PMT_REPLY)
    {
        // no known module supports PMT replys
        // this is unused code
        CI_LOG_DEBG("\n\r*******************************************************************\r\n");
        CI_LOG_DEBG("\n\rCA PMT Reply object coding: ");
        CI_LOG_DEBG("\n\rProgram_number = %.4x",pData[4]<<8|pData[5]);
        CI_LOG_DEBG("\n\rVersion_number = %d",(pData[6]>>1) & 0x1F);
        CI_LOG_DEBG("\n\rCur_next_indic = %d",pData[6] & 0x01);
    }
}

void CaSupport_Close(CI_HANDLE hCI, WORD hSession)
{
    CI_LOG_DEBG("[CaSupport_Close] entry!\r\n");
    hCI->dwCASupportSession = 0;
}

void CaSupport_SendPMT(CI_HANDLE hCI, const BYTE *pData, WORD wLen)
{
    CI_LOG_DEBG("[CaSupport_SendPMT] wLen:%d\r\n",wLen);

    if(hCI->dwCASupportSession)
        Send_SPDU_Data(hCI, hCI->CiSessions[hCI->dwCASupportSession].bTransportConnID, hCI->CiSessions[hCI->dwCASupportSession].wSessionID, pData, wLen);       
    else {
        CI_LOG_DEBG("[CaSupport_SendPMT] PMT Discarded/No CA Session (Handle %08X)\r\n",(U32)hCI);
    }
}   

#endif // ENABLE_CI
