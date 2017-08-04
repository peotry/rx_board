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
//  This file contains the portable CI Lib Layer2 Protocol
//  

/////////////////////////////////
// Includes
#include <assert.h>
#include "all.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(ENABLE_CI_SEMPHORE)
#include "WV_CI.h"
//#include "sys/alt_irq.h"
//#include "altera_avalon_pio_regs.h"

#endif

#define CIL2_DEFAULT_DELAY_TIME_MS         20 //10 //2 // 35 
#define CIL2_DEFAULT_CAPMT_INTERVAL_MS     300

#if defined(ENABLE_IRQ_CONTROL)
//#include "includes.h"
//extern OS_CPU_SR   cpu_sr;
#endif 

#ifdef ENABLE_PCMCIA
#ifdef ENABLE_CI

//#define CI_DUMP_RX
//#define CI_DUMP_TX
//#define CI_DEBUG_ENABLE

#ifdef CI_DEBUG_ENABLE
#define CI_DEBUG(x)     printf x 
////FAILRELEASEBUILD
#else
#define CI_DEBUG(x)     
#endif

int g_nCAMReset[WVCI_MAX_SLOT_NUM] = {0, 0};

//FRANSAT PRO (NEOTION) 大卡检测 ，重新搜流需重新插拔大卡，否则有节目无法解扰
int WAV_JudgeNEOTIONCam(PCMCIA_HANDLE hCI)
{
    int         nRet = 0;
    CIHANDLE    hCIHandle = (CIHANDLE)hCI;
    //CI_DEBUG(("%s \r\n",  hCIHandle->tPCMCIAInfo.pszManufacturerName));
    //printf("======= This CAM Manufactured by:   %s =============\r\n",hCIHandle->tPCMCIAInfo.pszManufacturerName);

    nRet = strcmp(hCIHandle->tPCMCIAInfo.pszManufacturerName, "NEOTION");
    //printf ("##debug## nRet: %d \r\n", nRet);
    //usleep(1000000);
    
    return nRet;
}

//大唐微电子需要跳过缓冲大小的协商，才能初始化完成
int WAV_SkipWriteBuffSize(PCMCIA_HANDLE hCI)
{
    int         nRet = 0;
    CIHANDLE    hCIHandle = (CIHANDLE)hCI;
    //CI_DEBUG(("%s \r\n",  hCIHandle->tPCMCIAInfo.pszManufacturerName));
    ////printf("This CAM Manufactured by:   %s \r\n",hCIHandle->tPCMCIAInfo.pszManufacturerName);

    nRet = strcmp(hCIHandle->tPCMCIAInfo.pszManufacturerName, "DMT");
    ////printf ("##debug## nRet: %d \r\n", nRet);
    return nRet;
}

void WriteBufferSize(PCMCIA_HANDLE hCI, WORD wBufferSize)
{
    int nRetries = 10;
    BYTE bStatus;
    PCMCIA_WriteIOMem(hCI,CI_PHYS_REG_COMMANDSTATUS,CICOMMAND_SIZEWRITE);
    
    while(nRetries--)
    {
        bStatus = PCMCIA_ReadIOMem(hCI,CI_PHYS_REG_COMMANDSTATUS);
        CI_LOG_DEBG("[WriteBufferSize] Status: %02X\r\n",bStatus);
        if(bStatus & CISTATUS_FREE)
            break;

        if(!PCMCIA_IsModuleStillPlugged(hCI))
            return;
        GURU_DelayMS(10);
    }
    
    PCMCIA_WriteIOMem(hCI,CI_PHYS_REG_SIZELOW,0x02);
    PCMCIA_WriteIOMem(hCI,CI_PHYS_REG_SIZEHIGH,0x00);
    PCMCIA_WriteIOMem(hCI,CI_PHYS_REG_DATA,(BYTE)(wBufferSize>>8));
    PCMCIA_WriteIOMem(hCI,CI_PHYS_REG_DATA,(BYTE)(wBufferSize   ));

    ////GURU_Delay(MSTOTICKS(50));
    GURU_DelayMS(50);
    
    PCMCIA_WriteIOMem(hCI,CI_PHYS_REG_COMMANDSTATUS,0x00);
    // Change: Do not wait for FR bit here. The Neotion NP4 modules do not set it
    // and EN 50221 does not require waiting for it either !
        
}

// ----------------------------------------------
void CI_WriteData(CI_TPDUBUFHANDLE hBuf, WORD wBufferSize, const BYTE *pData, WORD wPMTEndFlag)
{
    int i;
    if(wBufferSize > hBuf->wBufferSize) {
        CI_LOG_ERR("[CI_WriteData] Packet too big %08X %08X %08X\r\n",(int)hBuf,wBufferSize,hBuf->wBufferSize);
        return;
    }
    for(i=0;i<CI_T_SENDQSIZE;i++)
    {
        if(!hBuf->CI_T_SENDQ_Size[i])
        {
            memcpy(hBuf->CI_T_SENDQ[i],pData,wBufferSize);
            hBuf->CI_T_SENDQ_Size[i]=wBufferSize;
            hBuf->CI_T_SENDQ_CAPMT_ENDFRAME[i] = wPMTEndFlag;
            return;
        }
    }
    CI_LOG_ERR("[CI_WriteData] no free tx slot\r\n");
}

// ----------------------------------------------
BOOL CI_ReallyWriteData(CI_HANDLE hCI,WORD wBufferSize, const BYTE *pData)
{
    int i;
    BYTE bStatus = 0;
    PCMCIA_HANDLE hPCMCIA;
    hPCMCIA = hCI->hSlotHandle;

#ifdef CI_DUMP_TX
    CI_LOG_DEBG("[CI_ReallyWriteData] CI TX(%d): ", wBufferSize);
    for(i=0;i<wBufferSize;i++)
        CI_LOG_DEBG("%02X ",pData[i]);
    CI_LOG_DEBG(LOG_TYPE_DEBUG, MODULE_SI_PROCESS,"\r\n");
#endif
 
    if(wBufferSize > hCI->IOBuffer_Host2CI.wBufferSize)
    {
        CI_LOG_ERR("[CI_ReallyWriteData] CI TX Packet too big\r\n");
    }

    PCMCIA_WriteIOMem(hPCMCIA, CI_PHYS_REG_COMMANDSTATUS, CICOMMAND_HOSTCONTROL);
  //  CI_DEBUG(("CI_ReallyWriteData() - wrote hc bit!tick = %d\r\n",OSTimeGet()));
    //GURU_DelayMS(10); 
#define MAX_FR_RETRY 10
    i=0;
    while(!((bStatus=PCMCIA_ReadIOMem(hPCMCIA,CI_PHYS_REG_COMMANDSTATUS))&CISTATUS_FREE))
    {
        // if the FREE bit did not come up in the first attenot
        CI_LOG_ERR("[CI_ReallyWriteData] Status not free %X\r\n",bStatus);
        if (i++==MAX_FR_RETRY) {
            return FALSE;
        }
        // give the module some time
        GURU_DelayMS(20);

        if(!(PCMCIA_ReadIOMem(hPCMCIA,CI_PHYS_REG_COMMANDSTATUS)&CISTATUS_FREE))
        {
            // if it still does not react, try (re-)setting the bits
            PCMCIA_WriteIOMem(hPCMCIA, CI_PHYS_REG_COMMANDSTATUS, 0);
            CI_LOG_ERR("[CI_ReallyWriteData] CI_ReallyWriteData not \"free\" - Status Register %02X\r\n",bStatus);
            //GURU_Delay(15);
            GURU_DelayMS(10);
            PCMCIA_WriteIOMem(hPCMCIA, CI_PHYS_REG_COMMANDSTATUS, CICOMMAND_HOSTCONTROL);
            GURU_DelayMS(10);
        }
        else
            break;
    }

#if defined(ENABLE_IRQ_CONTROL)
    //OS_ENTER_CRITICAL();    //close irq
 //   alt_irq_context context;
   // context = alt_irq_disable_all();
#endif

    PCMCIA_WriteIOMem(hPCMCIA,CI_PHYS_REG_SIZELOW, (BYTE)(wBufferSize   ));
    PCMCIA_WriteIOMem(hPCMCIA,CI_PHYS_REG_SIZEHIGH,(BYTE)(wBufferSize>>8));

    for(i=0;i<wBufferSize;i++)
        PCMCIA_WriteIOMem(hPCMCIA,CI_PHYS_REG_DATA,pData[i]);

    PCMCIA_WriteIOMem(hPCMCIA,CI_PHYS_REG_COMMANDSTATUS,(BYTE)(0x00|hCI->bIRQBits));

#if defined(ENABLE_IRQ_CONTROL)
  //  alt_irq_enable_all(context);
   // OS_EXIT_CRITICAL();     //Open irq
#endif

    return TRUE;
}


void Read_CI_T(CI_HANDLE hCI)
{
    int i;
    PCMCIA_HANDLE hPCMCIA = hCI->hSlotHandle;
    GURU_DelayMS(10);
    
    if (!WVCI_SYSIsModuleStillPlugged(hCI->bSlotNum))
    {
        CI_LOG_ERR("[WVCI_SYSIORead] cam%d, is removed! \r\n", hCI->bSlotNum);
        return;
    }
    
    WORD CI_T_RXLEN = (WORD)(PCMCIA_ReadIOMem(hPCMCIA,CI_PHYS_REG_SIZEHIGH)<<8
                             |PCMCIA_ReadIOMem(hPCMCIA,CI_PHYS_REG_SIZELOW));

    if(CI_T_RXLEN < 3)
    {
        CI_LOG_ERR("[Read_CI_T] rev %d bytes data.return!\r\n", CI_T_RXLEN);
        return;
    }
    
    for(i=0;i<CI_T_RXLEN;i++) {
        if(i<MAX_CI_TPDU)
            hCI->CI_T_RX[i] = PCMCIA_ReadIOMem(hPCMCIA,CI_PHYS_REG_DATA);
        else {  
            hCI->CI_T_RX[MAX_CI_TPDU-1] = PCMCIA_ReadIOMem(hPCMCIA,CI_PHYS_REG_DATA);
            #if 0 // 7109在这里会导致死机
            // **GK** 060518 This is definitely an error !
            assert(0);
            #else
            CI_LOG_ERR("[Read_CI_T] ASSERT HERE!! ERROR!!!!\r\n");
            #endif
            
            return;
        }
    }

#ifdef CI_DUMP_RX
    CI_LOG_DEBG("[Read_CI_T]CI RX(%d): ", CI_T_RXLEN);
    for(i=0;i<CI_T_RXLEN;i++) {
        CI_LOG_DEBG("%02X ",hCI->CI_T_RX[i]);
    }
    CI_LOG_DEBG("\r\n");
#endif

    hCI->fRXPending = Parse_CI_T_Packet(hCI,hCI->CI_T_RX,CI_T_RXLEN);
}

// ----------------------------------------------
BOOL WaitForCIBit(PCMCIA_HANDLE hCI,BYTE BitMask)
{
    // VARS
    int         ii = 0;
    int         nBitStatus = 0;
    
    for(ii = 0; ii< 20; ii++)
    {
        // Do not poll for the presence of a CI module here
        // this causes too high I2C load or bus traffic !
        nBitStatus = PCMCIA_ReadIOMem(hCI,CI_PHYS_REG_COMMANDSTATUS) & BitMask;
        if(nBitStatus != 0x00) 
        {
            return TRUE;
        }
        CI_OSSleep(100);
    }
    return FALSE;
}



BOOL CI_CheckSlot(PCMCIA_HANDLE hPCMCIA, BYTE bSlotNum)
{
    BYTE        bData;
    BYTE        bBusy = FALSE;
    CI_HANDLE   hCI = &CiSlot[bSlotNum];
    WORD        wCAPMT_ENDFRAME = 0;

    // while we are busy    
    do 
    {
        bBusy = FALSE;
        bData = PCMCIA_ReadIOMem(hPCMCIA,CI_PHYS_REG_COMMANDSTATUS);

        // check for reserved bits (should always read as zero)
        if((bData&CISTATUS_RESERVEDBITS) || (hCI->ModulReset)) 
        { 
            CI_LOG_ERR("[CI_CheckSlot] RESERVED BIT CHECK FAILED %02X\r\n",bData);
            PCMCIA_ResetModule(hPCMCIA);
            hCI->ModulReset=0;
            return FALSE;
        } 

        // does the module want something ?
        if(bData&CISTATUS_DATAAVAILABLE) 
        {
            //printf ("[l2] bSlotNum:%d module want something... \r\n", bSlotNum);
            Read_CI_T(hCI);
            bBusy = TRUE;
            //GURU_Delay(MSTOTICKS(CIL2_DEFAULT_DELAY_TIME_MS));
            GURU_DelayMS(CIL2_DEFAULT_DELAY_TIME_MS);
        }
        else
        {       
            // are we allowed to send ?
            if((!hCI->fRXPending) /*&& (bData&CISTATUS_FREE)*/)
            {
                int i=0;

#if defined(ENABLE_CI_SEMPHORE)
            if (WVCI_SUCCESS == WVCI_GetSemphore(50)) //Time out is 50*10 ms.
            {
                bData = PCMCIA_ReadIOMem(hPCMCIA,CI_PHYS_REG_COMMANDSTATUS);
                if(bData&CISTATUS_DATAAVAILABLE) 
                {
                    //printf("prcessing rx first, goout and recheckslot.... \r\n");
                    WVCI_ReleaseSemphore();
                    return FALSE;
                }
                
                while(hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[0])
                {
                    if(CI_ReallyWriteData(hCI,hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[0],hCI->IOBuffer_Host2CI.CI_T_SENDQ[0])) {
                        wCAPMT_ENDFRAME = hCI->IOBuffer_Host2CI.CI_T_SENDQ_CAPMT_ENDFRAME[0];
                        memmove(&hCI->IOBuffer_Host2CI.CI_T_SENDQ[0],&hCI->IOBuffer_Host2CI.CI_T_SENDQ[1],(CI_T_SENDQSIZE-1)*CI_T_SENDQBUFSIZE);
                        memmove(&hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[0],&hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[1],(CI_T_SENDQSIZE-1)*sizeof(hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[0]));
                        memmove(&hCI->IOBuffer_Host2CI.CI_T_SENDQ_CAPMT_ENDFRAME[0],&hCI->IOBuffer_Host2CI.CI_T_SENDQ_CAPMT_ENDFRAME[1],(CI_T_SENDQSIZE-1)*sizeof(hCI->IOBuffer_Host2CI.CI_T_SENDQ_CAPMT_ENDFRAME[0]));
                        hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[CI_T_SENDQSIZE-1] = 0;
                        hCI->fGotPollAck = FALSE;
                        if (wCAPMT_ENDFRAME != 0x00)
                        {
                            //printf ("=========== delay N ms ============= \r\n");
                            GURU_DelayMS(CIL2_DEFAULT_CAPMT_INTERVAL_MS);
                        }
                    }
                    else 
                    {
                        // if we are still not able to send data despite the retry mechanisms, reset the module
                        CI_LOG_ERR("[CI_CheckSlot] CI_REALLYWRITEDATA FAILED!RESET CAM0!\r\n");
                        STATUS_ResetCAM(bSlotNum,CI_COMMUNICATION_ERROR);    // 重点怀疑这里出了问题, shouquan.tang
                        WVCI_ReleaseSemphore();

                        return FALSE;
                    }               
                    bBusy = TRUE;
                    GURU_DelayMS(CIL2_DEFAULT_DELAY_TIME_MS);
                    break;
                }
                WVCI_ReleaseSemphore();

                // if we are not busy.. check if we should send
                // a poll packet to the module
                if((!bBusy) || (i==CI_T_SENDQSIZE)) 
                {
                    if(hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[0]==0)
                    {
                        if(hCI->fGotPollAck)
                        {
                            Send_T_DataLast(hCI,0x01,NULL,0);
                            hCI->fGotPollAck = FALSE;
                        }
                    }
                }
            }
            else{
                CI_LOG_WARR("[CI_CheckSlot] Get semphore fail... wait for next time... \r\n");       
            }
#else
                while(hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[0])
                {
                    if(CI_ReallyWriteData(hCI,hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[i],hCI->IOBuffer_Host2CI.CI_T_SENDQ[i])) {
                        hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[i] = 0;
                        memmove(&hCI->IOBuffer_Host2CI.CI_T_SENDQ[0],&hCI->IOBuffer_Host2CI.CI_T_SENDQ[1],(CI_T_SENDQSIZE-1)*CI_T_SENDQBUFSIZE);
                        memmove(&hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[0],&hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[1],(CI_T_SENDQSIZE-1)*sizeof(hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[0]));
                        hCI->fGotPollAck = FALSE;
                    }
                    else 
                    {
                        // if we are still not able to send data despite the retry mechanisms, reset the module
                        CI_LOG_ERR("[CI_CheckSlot] CI_REALLYWRITEDATA FAILED!RESET CAM1!\r\n");
                        ////PCMCIA_ResetModule(hPCMCIA);
                        STATUS_ResetCAM_Extend(hPCMCIA);    // 重点怀疑这里出了问题, shouquan.tang

                        return FALSE;
                    }               
                    bBusy = TRUE;
                    //GURU_Delay(MSTOTICKS(CIL2_DEFAULT_DELAY_TIME_MS));
                    GURU_DelayMS(CIL2_DEFAULT_DELAY_TIME_MS);
                    break;
                }

                // if we are not busy.. check if we should send
                // a poll packet to the module
                if((!bBusy) || (i==CI_T_SENDQSIZE)) 
                {
                    if(hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size[0]==0)
                    {
                        if(hCI->fGotPollAck)
                        {
                            Send_T_DataLast(hCI,0x01,NULL,0);
                            hCI->fGotPollAck = FALSE;
                        }
                    }
                }
#endif      
            } 
        }
        bData = PCMCIA_ReadIOMem(hPCMCIA,CI_PHYS_REG_COMMANDSTATUS);
        if(bData&CISTATUS_DATAAVAILABLE)
            bBusy = TRUE;
        
    } while(bBusy);

    ////CI_DEBUG(("\r\n[CI_l2.c] CI_CheckSlot() return OK!!\r\n"));
    return TRUE;
}

BOOL CI_CheckConfig(PCMCIA_HANDLE hPCMCIA, PCMCIA_INFO *pInfo, BYTE bSlotNum)
{
    int i;
    
    CI_HANDLE hCI = &CiSlot[bSlotNum];
    hCI->hSlotHandle = hPCMCIA;
    for(i=0;i<pInfo->bNumConfigs;i++)
    {
        if(pInfo->Config[i].fCITagsPresent==0x03)
        {
            hCI->TPDULen = 0;

            PCMCIA_WriteAttribMem(hPCMCIA, (WORD)pInfo->ConfigOffset, pInfo->Config[i].bConfigIndex);     
           
            CI_ModuleInserted(hPCMCIA, bSlotNum);
            memset(hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size, 0x00, sizeof(hCI->IOBuffer_Host2CI.CI_T_SENDQ_Size));
      
            // Note1: Some modules do not like an initial reset:
            // like Cryptoworks and Black Viaccess
            // Note2: Do not fail the initialization because of
            // failed Ready-Bits here ! CONAX DOES NOT SET THESE BITS !
            
            // write size read
            
            // This is preliminary testcode to enable the IRQ processing
            // if(pInfo->Config[i].bIRQDesc1)
            // {
            //   CI_DEBUG(("####### ENABLING IRQ support\r\n"));
            //   hCI->bIRQBits = 0x80;//+0x40;
            // }

            // 2009-04-22 精简CI模块的处理流程
            ////CI_OSSleep(2000);

            CI_LOG_DEBG("[CI_CheckConfig] Write Size Read\r\n");    
            PCMCIA_WriteIOMem(hPCMCIA,CI_PHYS_REG_COMMANDSTATUS,CICOMMAND_SIZEREAD);
            if(!WaitForCIBit(hPCMCIA,CISTATUS_DATAAVAILABLE))
            {
                // write reset                  
                CI_LOG_DEBG("[CI_CheckConfig] Write Reset\r\n");
                PCMCIA_WriteIOMem(hPCMCIA,CI_PHYS_REG_COMMANDSTATUS,CICOMMAND_RESET);
                WaitForCIBit(hPCMCIA,CISTATUS_FREE);
                // write size read
                CI_LOG_DEBG("[CI_CheckConfig] Write Size Read\r\n");
                PCMCIA_WriteIOMem(hPCMCIA,CI_PHYS_REG_COMMANDSTATUS,0x04);
                if(!WaitForCIBit(hPCMCIA,CISTATUS_DATAAVAILABLE)) 
                {
                    CI_LOG_DEBG("[CI_CheckConfig] CI Init hangs\r\n");
                }

            }           
            // Dump Buffersize
            // read the size (this is always 0002, byteswapped)
            hCI->IOBuffer_Host2CI.wBufferSize = (WORD)(PCMCIA_ReadIOMem(hPCMCIA,CI_PHYS_REG_SIZELOW)<<8|PCMCIA_ReadIOMem(hPCMCIA,CI_PHYS_REG_SIZEHIGH));
            CI_LOG_DEBG("[CI_CheckConfig] Datalen %04X \r\n",hCI->IOBuffer_Host2CI.wBufferSize);
            
            // if the module returned an invalid data size, initiate a reset
            if(hCI->IOBuffer_Host2CI.wBufferSize != 0x0200) {
                CI_LOG_ERR("[CI_CheckConfig] CI Module returned an invalid buffer size(1). Initiating a reset.\r\n");
                PCMCIA_ResetModule(hPCMCIA);
                return FALSE;
            }

            hCI->IOBuffer_Host2CI.wBufferSize =  (WORD)(PCMCIA_ReadIOMem(hPCMCIA,CI_PHYS_REG_DATA)<<8);
            hCI->IOBuffer_Host2CI.wBufferSize |= PCMCIA_ReadIOMem(hPCMCIA,CI_PHYS_REG_DATA);
            CI_LOG_DEBG("[CI_CheckConfig] Buflen %02X \r\n",hCI->IOBuffer_Host2CI.wBufferSize);
            
            // if the module returned an invalid buffer size, initiate a reset
            // the EN50221 spec requires a minimum of 16 bytes
            if(hCI->IOBuffer_Host2CI.wBufferSize < 0x10) {
                CI_LOG_ERR("[CI_CheckConfig] CI Module returned an invalid buffer size(2). Initiating a reset.\r\n");
                PCMCIA_ResetModule(hPCMCIA);
                return FALSE;
            }
            
            
            PCMCIA_WriteIOMem(hPCMCIA,CI_PHYS_REG_COMMANDSTATUS,0x00);

            
            // select min(255,what the module offers)
            if(hCI->IOBuffer_Host2CI.wBufferSize > CI_T_SENDQBUFSIZE)
                hCI->IOBuffer_Host2CI.wBufferSize = CI_T_SENDQBUFSIZE;

            // Delay for NP4 Chipset
            //GURU_Delay(MSTOTICKS(10));
            GURU_DelayMS(10);

            if(0x00 == WAV_JudgeNEOTIONCam(hPCMCIA))
            {
                if(bSlotNum < WVCI_MAX_SLOT_NUM)
                {
                    g_nCAMReset[bSlotNum] = 1;
                }
            }
            
            // Normal operate:
            if (WAV_SkipWriteBuffSize(hPCMCIA) != 0x00)
            {
                CI_LOG_DEBG("[CI_CheckConfig] WriteBuffersize %04X\r\n",hCI->IOBuffer_Host2CI.wBufferSize);
                WriteBufferSize(hPCMCIA,hCI->IOBuffer_Host2CI.wBufferSize);

                //GURU_Delay(MSTOTICKS(10));
                GURU_DelayMS(10);
                CI_LOG_DEBG("[CI_CheckConfig] Sending T_Create\r\n");
                // create the initial session
                Send_T_CREATE(hCI,0x01);
                
                // CI config has been set
                // so it is safe to enable the TS stream now
                //PCMCIA_EnableTSRouting(hPCMCIA,TRUE);
            }
            // for DMT's CAMs
            // 1. skip the write buffer size
            // 2. write the hc bit first
            else
            {
                ////CI_DEBUG(("WriteBuffersize %04X\r\n",hCI->IOBuffer_Host2CI.wBufferSize));
                ////WriteBufferSize(hPCMCIA,hCI->IOBuffer_Host2CI.wBufferSize);
                PCMCIA_WriteIOMem(hPCMCIA, CI_PHYS_REG_COMMANDSTATUS, CICOMMAND_HOSTCONTROL);   // write hc bit 
                
                //GURU_Delay(MSTOTICKS(10));
                GURU_DelayMS(10);
                CI_LOG_DEBG("[CI_CheckConfig] Sending T_Create\r\n");
                // create the initial session
                Send_T_CREATE(hCI,0x01);
             
                // CI config has been set
                // so it is safe to enable the TS stream now
                //PCMCIA_EnableTSRouting(hPCMCIA,TRUE);
            }
           
            return TRUE;
        }
    }
    return FALSE;
}


#endif
#endif
