// ********************************************************
//
//  $Workfile: PCMCIA.c $ 
//
//   Author/Copyright	Gero Kuehn / GkWare e.K.
//						Humboldtstrasse 177
//						45149 Essen
//						GERMANY 
//						Tel: +49 174 520 8026
//						Email: support@gkware.com
//						Web: http://www.gkware.com
//
//	$Date: 2006-06-22 23:05:10 +0200 (Do, 22 Jun 2006) $
//	$Revision: 21 $
//
// ********************************************************
//
//  This file contains the portable PCMCIA CIS decoder
//  

//////////////////////////////////////////
//
// Includes
//
#include "all.h"
#include "PCMCIA.h"




#ifdef ENABLE_PCMCIA


/****************************************************************************
 * Macro
 ****************************************************************************/

////#define PCMCIA_DEBUG_ENABLE
// -------------------------------------------------------------

#ifdef PCMCIA_DEBUG_ENABLE
#define PCMCIA_DEBUG(x) printf x 
////FAILRELEASEBUILD
#else
#define PCMCIA_DEBUG(x)
#endif

/****************************************************************************
 * Local Func
 ****************************************************************************/

extern CIMAX_PCMCIA_HANDLESTRUCT_t     g_CIMax_CIHandles[WVCI_MAX_SLOT_NUM];
#if 0 // 以下是一个正常CAM回读出来的Mem CIS
const unsigned char test_array[] = 
{
    0x1d,  0x04,  0x00, 0xdb  ,0x08  ,0xff  ,0x1c  ,0x03  ,0x00  ,0x08  ,0xff  ,0x15  ,0x15  ,0x05  ,0x00  ,0x53,
    0x43,  0x4d,  0x00, 0x44  ,0x56  ,0x42  ,0x20  ,0x43  ,0x41  ,0x20  ,0x4d  ,0x6f  ,0x64  ,0x75  ,0x6c  ,0x65,
    0x00,  0xff,  0x20, 0x04  ,0xff  ,0xff  ,0x01  ,0x00  ,0x1a  ,0x15  ,0x01  ,0x0f  ,0xfe  ,0x03  ,0x01  ,0xc0,
    0x0e,  0x41,  0x02, 0x44  ,0x56  ,0x42  ,0x5f  ,0x43  ,0x49  ,0x5f  ,0x56  ,0x31  ,0x2e  ,0x30  ,0x30  ,0x1b,
    0x11,  0xc9,  0x41, 0x19  ,0x37  ,0x55  ,0x4e  ,0x5e  ,0x1d  ,0x56  ,0xaa  ,0x60  ,0x20  ,0x03  ,0x03  ,0x50,
    0xff,  0xff,  0x1b, 0x25  ,0xcf  ,0x04  ,0x09  ,0x37  ,0x55  ,0x4d  ,0x5d  ,0x1d  ,0x56  ,0x22  ,0xc0  ,0x09,
    0x44,  0x56,  0x42, 0x5f  ,0x48  ,0x4f  ,0x53  ,0x54  ,0x00  ,0xc1  ,0x0e  ,0x44  ,0x56  ,0x42  ,0x5f  ,0x43,
    0x49,  0x5f,  0x4d, 0x4f  ,0x44  ,0x55  ,0x4c  ,0x45  ,0x00  ,0x14  ,0x00  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff,
    0xff,  0xff,  0xff, 0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff,
    0xff,  0xff,  0xff, 0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff,
    0xff,  0xff,  0xff, 0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xff  ,0xcb  ,0x58  ,0x29  ,0x5c  ,0x73  ,0x00,
    0x2c,  0x14,  0x71, 0xe2  ,0xb4  ,0xe6  ,0xff  ,0x9a  ,0xc5  ,0xca  ,0xaa  ,0x7d  ,0xf3  ,0x40  ,0xb4  ,0x58,
    0xab,  0xf9,  0xef, 0x8e  ,0x53  ,0xf2  ,0x5d  ,0x07  ,0x49  ,0xc0  ,0x68  ,0x03  ,0x5e  ,0x2e  ,0xb6  ,0x6f,
    0x52,  0xb8,  0x68, 0x2c  ,0xa5  ,0x21  ,0xd3  ,0x21  ,0x8f  ,0x8d  ,0x67  ,0x31  ,0x8e  ,0x14  ,0x3b  ,0x91,
    0x89,  0x3a,  0x2a, 0xe4  ,0x9a  ,0x01  ,0x3e  ,0xfd  ,0x1a  ,0x4d  ,0x6c  ,0xc8  ,0xaa  ,0xa8  ,0x96  ,0x8a,
    0x2f,  0x09,  0x2f, 0xe9  ,0xe1  ,0x8e  ,0x03  ,0xd4  ,0x5f  ,0x9c  ,0x96  ,0xb8  ,0xf3  ,0x83  ,0x1f  ,0x93,
    0xc0,  0x99,  0x6e, 0xf3  ,0x0e  ,0x56  ,0x60  ,0xd1  ,0x93  ,0x63  ,0xea  ,0xa8  ,0x7d  ,0x9a  ,0x89  ,0x57,
    0x52,  0x99,  0xcf, 0xe9  ,0xa6  ,0x39  ,0xc5  ,0xc5  ,0x4f  ,0xde  ,0xd0  ,0x4b  ,0xcb  ,0x21  ,0xab  ,0x1e,
    0xbd,  0x16,  0xcb, 0x2b  ,0xf0  ,0x15  ,0xcb  ,0xa2  ,0xbb  ,0x9e  ,0x15  ,0x69  ,0x40  ,0x67  ,0x7b  ,0x72,
    0x4f,  0xd9,  0xae, 0xfe  ,0xb6  ,0xd9  ,0x1a  ,0xc2  ,0xb5  ,0x06  ,0x86  ,0x6c  ,0x55  ,0xb0  ,0x5d  ,0xef,
    0x34,  0xcc,  0x67, 0xfd  ,0x19  ,0xb1  ,0xfe  ,0xbd  ,0x10  ,0xfd  ,0x26  ,0x24  ,0x0c  ,0xec  ,0xec  ,0x06,
    0x0b,  0xa6,  0x6f, 0xaa  ,0xbf  ,0xfe  ,0xe9  ,0x8f  ,0xce  ,0xc4  ,0xf1  ,0xec  ,0x72  ,0xd8  ,0x1e  ,0x8d,
    0x93,  0xf3,  0xcd, 0xed  ,0xf3  ,0x44  ,0xda  ,0x93  ,0x8d  ,0x7b  ,0x6c  ,0xee  ,0xdf  ,0xbe  ,0xcf  ,0x89,
    0x60,  0xf6,  0x3b, 0x72  ,0x8a  ,0x69  ,0xde  ,0xc1  ,0xd7  ,0xef  ,0xda  ,0xf3  ,0xfb  ,0xad  ,0xc6  ,0xd3,
    0x9c,  0xac,  0xdf, 0x58  ,0xfa  ,0xeb  ,0xdb  ,0xcf  ,0xe6  ,0x3b  ,0x59  ,0x97  ,0x71  ,0x61  ,0x0e  ,0x0f,
    0xfd,  0xe5,  0xb8, 0x2b  ,0xec  ,0xf4  ,0xd6  ,0x7f  ,0x79  ,0x78  ,0xde  ,0xdb  ,0xfd  ,0xdb  ,0x0a  ,0x54,
    0x1b,  0xfa,  0x43, 0xf7  ,0x74  ,0xdf  ,0x87  ,0x5b  ,0x0b  ,0xe2  ,0xb8  ,0x70  ,0xc7  ,0xeb  ,0x5f  ,0x84,
    0x82,  0xce,  0x36, 0x8e  ,0x46  ,0x61  ,0x90  ,0xa8  ,0xc0  ,0x7e  ,0x7f  ,0xb6  ,0x9a  ,0x37  ,0x1b  ,0x97,
    0xd1,  0x1f,  0xbf, 0x3c  ,0x93  ,0xd2  ,0xbf  ,0xd4  ,0xa1  ,0x6d  ,0xf3  ,0x64  ,0x79  ,0x3c  ,0x7b  ,0xb3,
    0xe5,  0x58,  0x33, 0x9a  ,0x1e  ,0xcd  ,0xc8  ,0xd6  ,0x07  ,0xb6  ,0x76  ,0x8e  ,0xdc  ,0xd8  ,0xc1  ,0x06,
    0xfc,  0x88,  0x76, 0xfb  ,0x73  ,0x59  ,0xa7  ,0x86  ,0xd9  ,0xbd  ,0xc8  ,0xd2  ,0x37  ,0x08  ,0x2c  ,0x92,
    0x57,  0x70,  0x38, 0x60  ,0x5d  ,0x00  ,0xe1  ,0xe8  ,0x55  ,0xed  ,0x0a  ,0x28  ,0x8d  ,0x9d  ,0xff  ,0x00
};    
#endif

void PCMCIA_ReadAttribMem(BYTE ucSlot,BYTE *pDest)
{

     printf("***PCMCIA_ReadAttribMem ucSlot[%u]\r\n", ucSlot);
    // VARS
	int     ii = 0;
	// CIS readout with 8Bit I/O accesses
	// requires that we read only every second
	// byte. (The result of reading the even addresses does not seem to work on most modules)    
	//while (1)
    {
        usleep(2000);      
        for(ii=0; ii< MAX_CIS_SIZE; ii++)
        {
            usleep(2000);
            pDest[ii] = (U8)WVCI_SYSMEMRead(ucSlot, ii*2);
            /*
            printf ("ucSlot: %d, address: %03d, 0x%02X \r\n",
                    ucSlot,
                    ii*2, pDest[ii]);*/
        }
    }

    #if 1
    #define DEBUG_1(x)  printf x
    DEBUG_1(("++++++++++++++++++++++++++++++++++++++++++++++++++\r\n"));
    for (ii = 0; ii < 16; ii++) // print 16bytes only 
    {
        DEBUG_1 (("%02X ", pDest[ii]));
        if ((ii+1)%16 == 0) 
        {
            DEBUG_1(("\r\n"));
        }
    }
    #endif 

}


/******************************************************************************
 * CIMAX_ReadAttribMem() - CIMAX_ReadAttributeMem
 * DESCRIPTION: - 
 *     read attribute mem 
 * Input  : 
 * Output : 
 * Returns: 
 * 
 * modification history
 * --------------------
 * 03 Nov 2008, WELLAV-TT written
 * --------------------
 ******************************************************************************/
BYTE CIMAX_ReadAttribMem(BYTE *pucAddress)
{
    // vars
    BYTE    ucValue; 
    
    ucValue = *pucAddress;

    return ucValue;
}

//
int PCMCIA_ReadAttribMem_8BYTES(BYTE u8SlotIndex,BYTE *pDest)
{
	int                 i;
    volatile BYTE      *pAddr = NULL;
    volatile BYTE      *pucAddress = NULL;

    if (u8SlotIndex > 1)
    {
        return -1;
    }

    pAddr = (volatile BYTE               *)g_CIMax_CIHandles[u8SlotIndex].pucSlotMemory;
	// CIS readout with 8Bit I/O accesses
	// requires that we read only every second
	// byte. (The result of reading the even addresses does not seem to work on most modules)
    
    for(i=0;i<8;i++)
    {
        //pucAddress = &(pAddr[2*i]);
        pucAddress = &(pAddr[2*2*i]);                   // in fact we left shift for the Address line in HW, so more address*2
        pDest[i] = CIMAX_ReadAttribMem((BYTE *)pucAddress);
	}
    
    return 0;
}




int min(int a, int b)
{
    return a>b ? b : a;
}


void SendCIMessage(CI_HANDLE hCI, BYTE *pData, DWORD dwDataLength)
{
    WVCI_MessageCallback(hCI->bSlotNum, pData, dwDataLength);
}


// -------------------------------------------------------------
int PCMCIA_ParseAttribMem(const BYTE *pAttribMem, DWORD dwLen, PCMCIA_INFO *pInfo)
{
    int  nRet = PCMCIA_SUCCESS;      // return value
    
	BYTE bTag;
	BYTE bTagLen;
	int i;//,len;
	BYTE FieldSize;
	BYTE LastIndex;
	BYTE MaskByte;
	BYTE SubTagByte;
	BYTE SubTagLen;
	WORD STCI_IFN;
	char CiDetStr[20];
	BOOL fDefault;
	BOOL fInterface;
	BYTE bFeature;

    if (pAttribMem[0] == 0xFF   &&
        pAttribMem[1] == 0xFF   &&
        pAttribMem[2] == 0xFF   &&
        pAttribMem[3] == 0xFF)
    {
        printf ("##debug## exception data!! fff \r\n");
        return -1;
    }
    if (pAttribMem[0] == 0x00   &&
        pAttribMem[1] == 0x00   &&
        pAttribMem[2] == 0x00   &&
        pAttribMem[3] == 0x00)
    {
        printf ("##debug## exception data!! 000 \r\n");
        return -1;
    }
	memset(pInfo,0x00,sizeof(PCMCIA_INFO));
	do
	{
		bTag=pAttribMem[0];
		bTagLen=pAttribMem[1];
		if((DWORD)bTagLen+2 > dwLen)
		{
            nRet = PCMCIA_FAILURE;
			return nRet;
		}
        
		dwLen -= bTagLen+2;
		PCMCIA_DEBUG(("Parse_PCMCIA_AttribMem:bTag=%.2x %d\r\n",bTag,(int)dwLen));
		switch(bTag)
		{
		case 0x00:
			;//_asm{ int 3	};
			
		case 0x01:
			//PCMCIA_DEBUG(("CISTPL_DEVICE\r\n"));
			// Memory type
			// Access speed
			// Memory Size
			break;
		case 0x13:
			//PCMCIA_DEBUG(("CISTPL_LINKTARGET\r\n"));
			// "CIS"
			break;
		case 0x15:
			pInfo->wPCMCIAStdRev = (WORD)(pAttribMem[2]<<8|pAttribMem[3]);
			pInfo->dwValidFlags |= PCMCIAINFO_VERS1_VALID;
			i=4;
			if(bTagLen<2)
				break; // error
			memcpy(pInfo->pszManufacturerName, pAttribMem+i, min(bTagLen+2-i,MAX_PCMCIA_STRLEN-1));
			pInfo->pszManufacturerName[min(bTagLen+2-i,MAX_PCMCIA_STRLEN-1)] = 0;
			i+=strlen((char*)pAttribMem+i)+1;
			if(i<bTagLen+2) {
				memcpy(pInfo->pszProductName, pAttribMem+i, min(bTagLen+2-i,MAX_PCMCIA_STRLEN-1));
				pInfo->pszProductName[min(bTagLen+2-i,MAX_PCMCIA_STRLEN-1)] = 0;
				i+=strlen((char*)pAttribMem+i)+1;
			}
			if(i<bTagLen+2) {
				memcpy(pInfo->pszProductInfo1, pAttribMem+i, min(bTagLen+2-i,MAX_PCMCIA_STRLEN-1));
				pInfo->pszProductInfo1[min(bTagLen+2-i,MAX_PCMCIA_STRLEN-1)] = 0;
				i+=strlen((char*)pAttribMem+i)+1;
			}
			if(i<bTagLen+2) {
				memcpy(pInfo->pszProductInfo2, pAttribMem+i, min(bTagLen+2-i,MAX_PCMCIA_STRLEN-1));
				pInfo->pszProductInfo2[min(bTagLen+2-i,MAX_PCMCIA_STRLEN-1)] = 0;
			}

			//PCMCIA_DEBUG(("CISTPL_VERS1, SpecVer %04X\r\nManufacturer:%s\r\nProduct: %s,%s %s\r\n",pInfo->wPCMCIAStdRev,pInfo->pszManufacturerName,pInfo->pszProductName,pInfo->pszProductInfo1,pInfo->pszProductInfo2 ));
			break;
		case 0x17:
			//PCMCIA_DEBUG(("CISTPL_DEVICE_A\r\n"));
			// Like 0x01... for device(s) in attribute memory
			// Memory type
			// Access speed
			// Memory Size
			break;
		case 0x1a:
			PCMCIA_DEBUG(("CISTPL_CONFIG\r\n"));
			/*{
				BYTE bNumAddrBytes;
				BYTE bNumConfigRegs;
				BYTE bLastIndex;

				bNumAddrBytes = (pAttribMem[2]&0x03)+1;
				bNumConfigRegs = ((pAttribMem[2]>>2)&0x0F)+1;
				bLastIndex = pAttribMem[3]&0x3F;
				for(i=0;i<bNumAddrBytes;i++)
				{
					pInfo->ConfigOffset = pInfo->ConfigOffset<<8;
					pInfo->ConfigOffset |= pAttribMem[4+i];
					PCMCIA_DEBUG(("ConfigOffset %04X\r\n",pInfo->ConfigOffset));
				}
				i++;
			}*/
			FieldSize = pAttribMem[2];
			LastIndex = pAttribMem[3];
			if(FieldSize==0)
				pInfo->ConfigOffset=pAttribMem[4];
			else if(FieldSize==1)
				pInfo->ConfigOffset=pAttribMem[5]<<8|pAttribMem[4];
			PCMCIA_DEBUG(("ConfigOffset %04X\r\n",(int)pInfo->ConfigOffset));
			MaskByte = pAttribMem[5+FieldSize];
			SubTagByte = pAttribMem[6+FieldSize];
			SubTagLen = pAttribMem[7+FieldSize];
			STCI_IFN = (WORD)((pAttribMem[9+FieldSize]<<8)|pAttribMem[8+FieldSize]);
			memcpy(CiDetStr, pAttribMem+10+FieldSize, min(SubTagLen-2,sizeof(CiDetStr)-1));
			CiDetStr[min(SubTagLen-2,sizeof(CiDetStr)-1)] = 0;
			PCMCIA_DEBUG(("FieldSize %d, LastIndex %d MaskByte %02X SubTag %02X ID %02X %s\r\n",FieldSize,LastIndex,MaskByte,SubTagByte,STCI_IFN,CiDetStr));
			break;
		case 0x1b:
			PCMCIA_DEBUG(("CISTPL_CFTABLE_ENTRY\r\n"));
			{
				BYTE bIF;
				BYTE bFlags;
				int j;
				pInfo->Config[pInfo->bNumConfigs].bConfigIndex = (BYTE)(pAttribMem[2]&0x3F);
				if(pAttribMem[2]&0x40)
				{
					; // Default
				}
				if(pAttribMem[2]&0x80) 
                {
					bIF = pAttribMem[3];
					i=4;
				} else {
					i=3;
				}
				bFlags = pAttribMem[i];
				i++;
				if(bFlags & 0x03)
				{
					BYTE bPowerBits=pAttribMem[i];
					PCMCIA_DEBUG(("PowerDesc %02X\r\n",bPowerBits));
					i++;
					for(j=0;j<7;j++)
					{
						if((bPowerBits>>j)&0x01)
							i++;
						while(pAttribMem[i]&0x80)
							i++; // extension byte
					}
				}
				if(bFlags &0x04) {
					PCMCIA_DEBUG(("TimingDesc %02X\r\n",pAttribMem[i]));
					i++;
				}
				if(bFlags &0x08) {
					if(pAttribMem[i]&0x80)
					{
						BYTE bAddrBytes;
						BYTE bLenBytes;
						BYTE bSize[4]={0,1,2,4};
						BYTE bNumDesc;
						DWORD dwEALen = 0;
						DWORD dwEAAddr = 0;
						bNumDesc = (BYTE)(pAttribMem[i+1]&0x0F);
						bAddrBytes = bSize[(pAttribMem[i+1]>>4)&0x03];
						bLenBytes = bSize[(pAttribMem[i+1]>>6)&0x03];
						PCMCIA_DEBUG(("EADesc %02X %d %d %d\r\n",pAttribMem[i+1],bNumDesc,bAddrBytes,bLenBytes));
						i+=2;
						switch(bAddrBytes)
						{
						case 1:
							dwEAAddr = pAttribMem[i];
							break;
						case 2:
							dwEAAddr = pAttribMem[i]|pAttribMem[i+1]<<8;
							break;
						case 4:
							dwEAAddr = pAttribMem[i]|pAttribMem[i+1]<<8|pAttribMem[i+2]<<16|pAttribMem[i+3]<<24;
							break;
						}
						pInfo->Config[pInfo->bNumConfigs].dwEAAddr = dwEAAddr;
						i+=bLenBytes;
						switch(bLenBytes)
						{
						case 1:
							dwEALen = pAttribMem[i];
							break;
						case 2:
							dwEALen = pAttribMem[i]|pAttribMem[i+1]<<8;
							break;
						case 4:
							dwEALen = pAttribMem[i]|pAttribMem[i+1]<<8|pAttribMem[i+2]<<16|pAttribMem[i+3]<<24;
							break;
						}
						pInfo->Config[pInfo->bNumConfigs].dwEALen = dwEALen;
						i+=bAddrBytes;
						PCMCIA_DEBUG(("Addr %04X Len %04X",(int)dwEAAddr,(int)dwEALen));
					} else
						i++;
				}
				if(bFlags &0x10) {
					PCMCIA_DEBUG(("IrqDesc\r\n"));
					pInfo->Config[pInfo->bNumConfigs].bIRQDesc1 = pAttribMem[i];
					if(pAttribMem[i]&0x10) {
						pInfo->Config[pInfo->bNumConfigs].wIRQData= (WORD)((pAttribMem[i]<<8)|pAttribMem[i+1]);
						i+=2;
					}
					i++;
				}
				if(bFlags &0x60) {
					PCMCIA_DEBUG(("MemoryDesc\r\n"));
					i++;
				}
				if(bFlags &0x80) {
					PCMCIA_DEBUG(("MixedDesc\r\n"));
					i++;
				}


				while(i<(bTagLen+2))
				{
				PCMCIA_DEBUG(("SubTag %02X %d %d",pAttribMem[i],i,bTagLen));
					if(pAttribMem[i]==0xc0) {
						if(strcmp((char*)pAttribMem+i+2,"DVB_HOST")==0)
							pInfo->Config[pInfo->bNumConfigs].fCITagsPresent |= 0x01;
						PCMCIA_DEBUG(("%s\r\n",pAttribMem+i+2));
					}
					if(pAttribMem[i]==0xc1) {
						if(strcmp((char*)pAttribMem+i+2,"DVB_CI_MODULE")==0)
							pInfo->Config[pInfo->bNumConfigs].fCITagsPresent |= 0x02;
						PCMCIA_DEBUG(("%s\r\n",pAttribMem+i+2));
					}
				i+=pAttribMem[i+1]+2;
				}

				pInfo->bNumConfigs++;			
			}
			fDefault = pAttribMem[2]>>7;
			fInterface = (pAttribMem[2]>>6)&0x01;
			PCMCIA_DEBUG(("(default) "));
			if(fInterface)
			{
				PCMCIA_DEBUG(("IF %02X ",pAttribMem[3]));
			}
			bFeature = pAttribMem[3+fInterface];
			
			PCMCIA_DEBUG(("\r\n"));
			
			for(i=0;i<bTagLen;i++)
				PCMCIA_DEBUG(("%02X ",pAttribMem[2+i]));
			PCMCIA_DEBUG(("\r\n"));
			for(i=0;i<bTagLen;i++)
				PCMCIA_DEBUG(("%c  ",pAttribMem[2+i]));
			PCMCIA_DEBUG(("\r\n"));
		
			break;
		case 0x1c:
			PCMCIA_DEBUG(("CISTPL_DEVICE_OC\r\n"));
			break;
		case 0x1D:
			PCMCIA_DEBUG(("CISTPL_DEVICE_OA\r\n"));
			break;
		case 0x20:
			//PCMCIA_DEBUG(("CISTPL_MANFID\r\n"));
			pInfo->dwValidFlags |= PCMCIAINFO_MANID_VALID;
			pInfo->wManufacturerId = (WORD)((pAttribMem[2]<<8)|pAttribMem[3]);
			pInfo->wCardID = (WORD)((pAttribMem[4]<<8)|pAttribMem[5]);
			PCMCIA_DEBUG(("Manufacturer code %04X Product Code %04X\r\n",pInfo->wManufacturerId,pInfo->wCardID));
			break;
		case 0x21:
			//PCMCIA_DEBUG(("CISTPL_FUNCID\r\n"));
			PCMCIA_DEBUG(("CISTPL_FUNCID\r\n"));
			pInfo->dwValidFlags |= PCMCIAINFO_FUNCID_VALID;
			pInfo->FuncType = (PCMCIA_FUNCTYPE)pAttribMem[2];
			pInfo->bFuncIDSysInfo = pAttribMem[3];
			break;
		case 0x40: // CISTPL_VERS2
			break;
		default:
			PCMCIA_DEBUG(("Tag %02X, Len %d\r\n",bTag,bTagLen));
			for(i=0;i<bTagLen;i++) {
				PCMCIA_DEBUG(("%02X ",pAttribMem[2+i]));
			}
			PCMCIA_DEBUG(("\r\n"));
			for(i=0;i<bTagLen;i++) {
				PCMCIA_DEBUG(("%c",pAttribMem[2+i]));
			}
			PCMCIA_DEBUG(("\r\n"));
			break;
		}
		pAttribMem+=2+bTagLen;
	} while(bTag!=0x14&&bTag!=0xFF&&dwLen);

        
	return nRet;
}
    

//! Initialization function. During system initialization, this function
//! is called once to initialize the driver.
BOOL PCMCIA_Init(void)
{
    return TRUE;
}

BOOL PCMCIA_IsModuleStillPlugged(PCMCIA_HANDLE hSlot)
{
    // VARS    
    CIHANDLE    hCIHandle = (CIHANDLE)hSlot;
    
    // CHECK PARAMS
    if (hCIHandle == NULL)
    {
       return -1; 
    }
    
    return WVCI_SYSIsModuleStillPlugged(hCIHandle->ucSlotIndex);
}

void PCMCIA_WriteAttribMem(PCMCIA_HANDLE hSlot,WORD wAddr,BYTE bData)
{
    // VARS    
    CIHANDLE    hCIHandle = (CIHANDLE)hSlot;
    
    // CHECK PARAMS
    if (hCIHandle == NULL)
    {
       return ; //0xFF; 
    }

    //printf ("##debug-CI## [PCMCIA_WriteAttribMem] ====> Handle:0x%08X, u8Slot:%d \r\n", (U32)hCIHandle, hCIHandle->ucSlotIndex);

    WVCI_SYSMEMWrite(hCIHandle->ucSlotIndex, wAddr, bData);
}


BYTE PCMCIA_ReadIOMem(PCMCIA_HANDLE hSlot,WORD wAddr)
{
    // VARS    
    BYTE rData = 0;
    U32 ret;
    CIHANDLE    hCIHandle = (CIHANDLE)hSlot;
    
    // CHECK PARAMS
    if (hCIHandle == NULL)
    {
       return 0xFF; 
    }

    //printf ("##debug-CI## [PCMCIA_ReadIOMem] ====> Handle:0x%08X, u8Slot:%d \r\n", (U32)hCIHandle, hCIHandle->ucSlotIndex);
    //usleep(2000);
    
    ret = WVCI_SYSIORead(hCIHandle->ucSlotIndex, wAddr, &rData);

    if(WV_SUCCESS != ret)
    {
        return 0xFF;
    }

    
    return rData;
}


int PCMCIA_WriteIOMem(PCMCIA_HANDLE hSlot,WORD wAddr,BYTE bData)
{
    // VARS    
    CIHANDLE    hCIHandle = (CIHANDLE)hSlot;
    
    // CHECK PARAMS
    if (hCIHandle == NULL)
    {
       return -1; 
    }
    //printf ("##debug-CI## [PCMCIA_WriteIOMem] ====> Handle:0x%08X, u8Slot:%d \r\n", (U32)hCIHandle, hCIHandle->ucSlotIndex);

    //usleep(2000);
    return WVCI_SYSIOWrite(hCIHandle->ucSlotIndex, wAddr, bData);
}

int PCMCIA_EnableTSRouting(PCMCIA_HANDLE hSlot,BOOL fEnable)
{
    //return CIMAX_EnableTSRouting(hSlot, fEnable);
    return TRUE;
}

void PCMCIA_ResetModule(PCMCIA_HANDLE hSlot)
{
    // VARS    
    CIHANDLE    hCIHandle = (CIHANDLE)hSlot;
    
    // CHECK PARAMS
    if (hCIHandle == NULL)
    {
       //printf ("bad params");
       //return -1; 
       return;
    }

    //printf ("[PCMCIA_ResetModule] ...hCIHandle->ucSlotIndex: %d  \r\n", hCIHandle->ucSlotIndex);
    WVCI_SYSResetModule(hCIHandle->ucSlotIndex);
}


int PCMCIA_PowerOnModule(int nSlot)
{
    return WVCI_SYSPowerOnModule(nSlot);
}

int PCMCIA_PowerOffModule(int nSlot)
{
    return WVCI_SYSPowerOffModule(nSlot);
}


int PCMCIA_ResetModule2(int nSlot)
{
    return WVCI_SYSResetModule(nSlot);
}


#endif		// ENABLE_CI

