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
//
//  This file contains the portable CI Lib CA-PMT Generator
//  
#include "all.h"
#include "CI_PMTDec.h"
#include "WVCI_PlatformDriver.h"

#ifdef ENABLE_CI

////#define CIPMT_DEBUG_ENABLE
//#define CIPMT_DUMP_ENABLE

#ifdef CIPMT_DEBUG_ENABLE
#define CIPMT_DEBUG(x) printf x
//FAILRELEASEBUILD
#else
#define CIPMT_DEBUG(x)
#endif





// *************************************************************
// externals
#ifdef ENABLE_AUTOMATIC_PMT_SEND
BYTE  g_PMTBuf[MAX_LEN_CA_PMT];
WORD  g_wPMTLen = 0;
#endif // ENABLE_AUTOMATIC_PMT_SEND




BOOL IsCASysIdInList(CI_HANDLE hCI, WORD wCASysId)
{
    int i;
    if(!hCI)
        return TRUE;
//  The spec does not mention any requirement
//  to filter the PMT based on the reported
//  ca system ids, but really a lot of modules
//  require this to function properly.
//  if(!hCI->fModuleRequiresCAIDFiltering)
//      return TRUE;
    for(i=0;i<hCI->wNumCASystems;i++)
    {
        if(hCI->wCASystemIDs[i]==wCASysId)
        {
            return TRUE;
        }
    }

    return FALSE;
}


//! This stream type list covers all MPEG2/MPEG4/AVC elementary stream types which are in use right now.
//! For MPEG2 SD boxes a reduction to "\x01\x02\x03\x04" + \0x06 for Dolby digital/AC3 might be sufficient as well.
//! HD boxes need to support 0x1B (AVC) and/or 0x10 & 0x11 (MPEG4) as well
const BYTE g_pDefaultStreamTypeList[] = 
{
    0x01,               //
    0x02,               //
    0x03,               //
    0x04,               //
    0x05,               //
    0x06,               //
    0x07,               //
    0x08,               //
    0x09,               //
    0x0A,               //
    0x0B,               //
    0x0C,               //
    0x0D,               //
    0x0E,               //
    0x0F,               //
    0x10,               //
    0x11,               //
    0x12,               //
    0x13,               //
    0x14,               //
    0x15,               //
    0x16,               //
    0x18,               //
    0x19,               //
    0x1A,               //
    0x1B,               //
    0x1C,               //
    0x1D,               //
    0x1E,               //
    0x1F,               //

    // 增加AC3的处理
    0x80,
    0x81,

    0x42,               // AVS+
    
    0x00                //    
};


WORD Generate_Multi_CA_PMT(BYTE *CAPMTBuf,BYTE **pBuf,BYTE *PMTs[1024], DWORD dwNumPMTs, DWORD dwLen,CI_HANDLE hCI)
{
    WORD wSizePtr;
    int nLoopSizePtr;
    int nTotalLen;
    int nLoopLen;
    BYTE bMatchedDescriptors = 0;
    WORD wNumPMT;
    
    BYTE i,k;

    
    CI_LOG_DEBG("[Generate_Multi_CA_PMT] %d pmts\r\n", (int)dwNumPMTs);
    
    CAPMTBuf[0] = 0x9F;
    CAPMTBuf[1] = 0x80;
    CAPMTBuf[2] = 0x32;
    wSizePtr = 3;
    
    // 1 byte size,
    // larger ca_pmts are handled below
    nTotalLen = 4; // 5 for long size
    CAPMTBuf[3] = 0;  // size comes here
    

    CAPMTBuf[nTotalLen++] = CA_PMT_LIST_MANAGEMENT_ONLY;
    CAPMTBuf[nTotalLen++] = PMTs[0][3];  // program number
    CAPMTBuf[nTotalLen++] = PMTs[0][4];  // program number
    CAPMTBuf[nTotalLen++] = PMTs[0][5];  // version
    nLoopSizePtr = nTotalLen;
    CAPMTBuf[nTotalLen++] = 0;  // size comes here
    CAPMTBuf[nTotalLen++] = 0;  // size comes here
    //wTotalLen = 11;
    nLoopLen = 0;
    i=0;

    // this block would just copy CA descriptors in the root loop into
    // the capmt. This makes no sense for a combined capmt for several services.
    // Root loop descriptors are moved to the elementary stream loop
    /*
    wNumPMT=0;
    for(wNumPMT=0;wNumPMT<dwNumPMTs;wNumPMT++)
    {
        if(PMTs[wNumPMT])
        {
            k=0;
            while(CI_PMT_GetDescriptor(PMTs[wNumPMT],0x09,k))
            {
                BYTE bSize;
                const BYTE *pDesc = CI_PMT_GetDescriptor(PMTs[wNumPMT],0x09,k);
                
                if(IsCASysIdInList(hCI,(WORD)((pDesc[2]<<8)|pDesc[3])))
                {
                    if(bMatchedDescriptors==0) {
                        CAPMTBuf[wTotalLen] = 0x01;
                        wTotalLen++;
                        wLoopLen++;
                    }
                    
                    CIPMT_DEBUG(("Descriptor %02X Len %02X (%02X%02X %02X%02X)\r\n",pDesc[0],pDesc[1],pDesc[2],pDesc[3],pDesc[4],pDesc[5]));
                    
                    bSize=pDesc[1]+2;
                    memcpy(&CAPMTBuf[wTotalLen],pDesc,bSize);
                    wTotalLen+=bSize;
                    wLoopLen+=bSize;
                    bMatchedDescriptors++;
                } 
                //          else {
                //              CIPMT_DEBUG(("Discarding CA Descriptor due to CASysId Filtering\r\n"));
                //          }
                i++;
            }
        }
    }
    */
    CI_LOG_DEBG("[Generate_Multi_CA_PMT] total len %04X, LoopLen %04X\r\n",nTotalLen,nLoopLen);
    CAPMTBuf[nLoopSizePtr] = (BYTE)(0xF0 | (nLoopLen>>8)); // reserved bits + upper part of the loop size
    CAPMTBuf[nLoopSizePtr+1] = (BYTE)(nLoopLen&0xFF);
    nLoopLen = 0;
    wNumPMT=0;
    // The stream loop
    for(wNumPMT=0;wNumPMT<dwNumPMTs;wNumPMT++)
    {
        i=0;
        {
            BYTE bStreamType;
            WORD wElementaryPid;
            if(PMTs[wNumPMT])
            {           

                // ES loop CAPMT part
                while(CI_PMT_GetESEntry(PMTs[wNumPMT],i,&bStreamType,&wElementaryPid))
                {

                    BYTE j = 0;
                    
                    CI_LOG_DEBG("[Generate_Multi_CA_PMT] from PMT %d %04X\r\n", (int)wNumPMT, (int)wElementaryPid);
                    
                    CAPMTBuf[nTotalLen] = bStreamType;
                    CAPMTBuf[nTotalLen+1] = (BYTE)(0xE0 | (wElementaryPid>>8)); // reserved bits + upper part of the pid
                    CAPMTBuf[nTotalLen+2] = (BYTE)(wElementaryPid&0xFF);
                    nLoopSizePtr = nTotalLen+3;
                    CAPMTBuf[nTotalLen+3] = 0; // looplen
                    CAPMTBuf[nTotalLen+4] = 0;
                    nTotalLen+=5;
                    nLoopLen=0;
                    bMatchedDescriptors = 0;





                    // PMT root loop descriptors that get moved to every ES
                    k=0;
                    while(CI_PMT_GetDescriptor(PMTs[wNumPMT],0x09,k))
                    {
                        BYTE bSize;
                        const BYTE *pDesc = CI_PMT_GetDescriptor(PMTs[wNumPMT],0x09,k);
                        
                        if(IsCASysIdInList(hCI,(WORD)((pDesc[2]<<8)|pDesc[3])))
                        {
                            if(bMatchedDescriptors==0) {
                                CAPMTBuf[nTotalLen] = 0x01;
                                nTotalLen++;
                                nLoopLen++;
                            }
                            
                            CI_LOG_DEBG("[Generate_Multi_CA_PMT] Moved Descriptor %02X Len %02X (%02X%02X %02X%02X)\r\n",
                                                                    pDesc[0],
                                                                    pDesc[1],
                                                                    pDesc[2],
                                                                    pDesc[3],
                                                                    pDesc[4],
                                                                    pDesc[5]);
                            
                            bSize=(BYTE)(pDesc[1]+2);
                            memcpy(&CAPMTBuf[nTotalLen],pDesc,bSize);
                            nTotalLen+=bSize;
                            nLoopLen+=bSize;
                            bMatchedDescriptors++;
                        } 
                        //          else {
                        //              CIPMT_DEBUG(("Discarding CA Descriptor due to CASysId Filtering\r\n"));
                        //          }
                        k++;
                    }


                    // ES loop descriptors
                    while(CI_PMT_GetESDescriptor(PMTs[wNumPMT],i,0x09,j))
                    {
                        BYTE bSize;
                        const BYTE *pDesc = CI_PMT_GetESDescriptor(PMTs[wNumPMT],i,0x09,j);
                        if(IsCASysIdInList(hCI,(WORD)((pDesc[2]<<8)|pDesc[3])))
                        {
                            if(bMatchedDescriptors==0) 
                            {
                                CAPMTBuf[nTotalLen] = 0x01;
                                nTotalLen++;
                                nLoopLen++;
                            }
                            
                            CI_LOG_DEBG("[Generate_Multi_CA_PMT] Descriptor %02X Len %02X (%02X%02X %02X%02X) %d\r\n",pDesc[0],pDesc[1],pDesc[2],pDesc[3],pDesc[4],pDesc[5], (int)wNumPMT);
                            bSize=(BYTE)(pDesc[1]+2);
                            memcpy(&CAPMTBuf[nTotalLen],pDesc,bSize);
                            nTotalLen+=bSize;
                            nLoopLen+=bSize;
                            bMatchedDescriptors++;
                        } else {
                            CI_LOG_DEBG("[Generate_Multi_CA_PMT] Discarding CA Descriptor due to CASysId Filtering\r\n");
                        }
                        j++;
                    }   
                    CAPMTBuf[nLoopSizePtr] = (BYTE)(0xF0 | (nLoopLen>>8)); // reserved bits + upper part of the loop size
                    CAPMTBuf[nLoopSizePtr+1] = (BYTE)(nLoopLen&0xFF);
                    i++;
                }
            }
        }
    }

    // fix the complete size
    // 1
    CAPMTBuf[wSizePtr] = (BYTE)(nTotalLen-4);

    // Large PMT handling
    //length_field
    if (((nTotalLen-4) > 127) && ((nTotalLen-4) < 255))
    {
        int i;
        for (i=nTotalLen;i>3;i--)
            CAPMTBuf[i+1] = CAPMTBuf[i];
        CAPMTBuf[3] = 0x81;             
        CAPMTBuf[4] = (BYTE)(nTotalLen-4);
        nTotalLen+=1;
    }   
    if ((nTotalLen-4) >= 255)
    {
        int i;
        for (i=nTotalLen;i>3;i--)
            CAPMTBuf[i+2] = CAPMTBuf[i];
        CAPMTBuf[3] = 0x82;             
        CAPMTBuf[4] = (BYTE)((nTotalLen-4)>>8);
        CAPMTBuf[5] = (BYTE)((nTotalLen-4)&0xFF);
        nTotalLen+=2;
    }   

#ifdef CIPMT_DUMP_ENABLE
    PrintHexDump(CAPMTBuf,nTotalLen);
#endif
    return (WORD)nTotalLen;
}


#endif //ENABLE_CI

