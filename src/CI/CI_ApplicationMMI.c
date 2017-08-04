// ********************************************************
//
//   Author/Copyright	Gero Kuehn / GkWare e.K.
//						Humboldtstrasse 177
//						45149 Essen
//						GERMANY 
//						Tel: +49 174 520 8026
//						Email: support@gkware.com
//						Web: http://www.gkware.com
//
//
// ********************************************************
#include "all.h"

#ifdef ENABLE_CI
#ifdef ENABLE_CI_APPLICATIONMMI

//#define CIAPPMMI_DEBUG_ENABLE

#ifdef CIAPPMMI_DEBUG_ENABLE 
#define CIAPPMMI_DEBUG(x) UART_printf x
FAILRELEASEBUILD
#else
#define CIAPPMMI_DEBUG(x)
#endif

static BOOL CIAPPMMI_Connect(CI_HANDLE hCI, CISESSIONHANDLE hSession);
static void CIAPPMMI_Data(CI_HANDLE hCI, CISESSIONHANDLE hSession, const BYTE *pData, DWORD dwLen);
static void CIAPPMMI_Close(CI_HANDLE hCI, CISESSIONHANDLE hSession);


const RESOURCE_INFO ApplicationMMI = { RESOURCEID_APPLICATIONMMI,CIAPPMMI_Connect,CIAPPMMI_Data,CIAPPMMI_Close};


BOOL CIAPPMMI_Connect(CI_HANDLE hCI, CISESSIONHANDLE hSession)
{
	CIAPPMMI_DEBUG(("CIAPPMMI_Connect\r\n"));
	hCI->hAppMMISession = hSession;
	hCI->fAppMMIActive = FALSE;
	return TRUE;
}

void CIAPPMMI_Data(CI_HANDLE hCI, CISESSIONHANDLE hSession, const BYTE *pData, DWORD dwLen)
{
	DWORD dwTag;
	int nOffset = 3;
	WORD wLen;
	BYTE bAppDomainIDlen;
	BYTE bInitialObjlen;
	BOOL fFileOK;
	CI_AM_FILEREQTYPE_CODE eRequestType;
	BYTE bFileNameLen;
	DWORD dwFileLen;
	BYTE *pFileName;
	CI_AM_REQSTARTACK_CODE bReqStartAckCode;
	BYTE pDataBuff[MAX_AM_FILE_NAME];

	dwTag = pData[0]<<16|pData[1]<<8|pData[2];

	// Check if hSession is the currently saved hSession for the application MMI resource
	// If not ignore this APDU
	// [shouquan.tang] Replace the Assert to CI_Assert
    CI_Assert(hSession==hCI->hAppMMISession);
	if(hSession!=hCI->hAppMMISession)
	{
		CIAPPMMI_DEBUG(("Application MMI data with unexpected hSession: %d - hCI->hAppMMISession: %d!\r\n",hSession,hCI->hAppMMISession));
		return; // -> silently discard this APDU
	}

	// Parse the variable length field
	nOffset += CI_DecodeLengthField(&pData[nOffset],&wLen);

	// length validity control
	// [shouquan.tang] Replace the Assert to CI_Assert
    CI_Assert((DWORD)(nOffset+wLen)<=dwLen);
	if((DWORD)(nOffset+wLen)>dwLen)
	{
		CIAPPMMI_DEBUG(("Application MMI data length invalid!\r\n"));
		return; // -> silently discard this APDU
	}

	switch(dwTag)
	{
	case TAG_T_REQUESTSTART:
		CIAPPMMI_DEBUG(("Application MMI start request\r\n"));

		// length validity control
		// [shouquan.tang] Replace the Assert to CI_Assert
        CI_Assert((DWORD)(nOffset+2)<=dwLen);
		if((DWORD)(nOffset+2)>dwLen)
		{
			CIAPPMMI_DEBUG(("Application MMI start request parsing error!\r\n"));
			return; // -> silently discard this APDU
		}

		// Get the following two length values
		bAppDomainIDlen = pData[nOffset++];
		bInitialObjlen = pData[nOffset++];

		// length validity control
		// [shouquan.tang] Replace the Assert to CI_Assert
        CI_Assert((DWORD)(nOffset+bAppDomainIDlen+bInitialObjlen)<=dwLen);
		if((DWORD)(nOffset+bAppDomainIDlen+bInitialObjlen)>dwLen)
		{
			CIAPPMMI_DEBUG(("Application MMI start request parsing error!\r\n"));
			return; // -> silently discard this APDU
		}

		// Store the application domain ID in the CI slot structure
		// [shouquan.tang] Replace the Assert to CI_Assert
        CI_Assert(bAppDomainIDlen<MAX_APP_DOMAIN_ID);
		memcpy(hCI->AppDomainID,pData+nOffset,min(bAppDomainIDlen,MAX_APP_DOMAIN_ID-1));
		hCI->AppDomainID[min(bAppDomainIDlen,MAX_APP_DOMAIN_ID-1)] = 0;
		nOffset += bAppDomainIDlen;

		// Store the initial object in the CI slot structure
		// [shouquan.tang] Replace the Assert to CI_Assert
        CI_Assert(bInitialObjlen<MAX_INITIAL_OBJ);
		memcpy(hCI->InitialObject,pData+nOffset,min(bInitialObjlen,MAX_INITIAL_OBJ-1));
		hCI->InitialObject[min(bInitialObjlen,MAX_INITIAL_OBJ-1)] = 0;

		// Signal the RequestStart and its parameters to the Host API
		bReqStartAckCode = CIHost_AppMMIrequestStart(hCI->bSlotNum, hCI->AppDomainID, hCI->InitialObject);
		if(bReqStartAckCode == CI_AM_REQSTARTACK_OK)
		{
			hCI->fAppMMIActive = TRUE;
		}

		// Reply to the RequestStart with the ack code returned from the above function
		pDataBuff[0] = TAG_T_REQUESTSTARTACK>>16&0xFF;
		pDataBuff[1] = TAG_T_REQUESTSTARTACK>>8&0xFF;
		pDataBuff[2] = TAG_T_REQUESTSTARTACK&0xFF;
		pDataBuff[3] = 1;
		pDataBuff[4] = (BYTE)bReqStartAckCode;
		Send_Session_Data(hCI,hSession,pDataBuff,5);
		break;

	case TAG_T_FILEACK:

		// length validity control
		// [shouquan.tang] Replace the Assert to CI_Assert
        CI_Assert((DWORD)(nOffset+2)<=dwLen);
		if((DWORD)(nOffset+2)>dwLen)
		{
			CIAPPMMI_DEBUG(("Application MMI FileRequestACK length invalid!\r\n"));
			return; // -> silently discard this APDU
		}

		// Get the next to byte which are parameters to CIHost_AppMMIfileRequestAck()
		fFileOK = (pData[nOffset++]&0x01)?TRUE:FALSE;
		eRequestType = (CI_AM_FILEREQTYPE_CODE)pData[nOffset++];

		CIAPPMMI_DEBUG(("Application MMI file ACK ok:%d type:%d\r\n",fFileOK,eRequestType));
		// Signal the FileRequestAck and its parameters to the Host API
		if(eRequestType==CI_AM_FILEREQTYPE_FILE)
		{
			// [shouquan.tang] Replace the Assert to CI_Assert
            CI_Assert((DWORD)(nOffset+1)<=dwLen);
			bFileNameLen = 0;
			pFileName = NULL;
			dwFileLen = 0;
			if((DWORD)(nOffset+1)<=dwLen)
			{
				bFileNameLen = pData[nOffset++];
				// [shouquan.tang] Replace the Assert to CI_Assert
                CI_Assert((DWORD)(nOffset+bFileNameLen)<=dwLen);
				if((DWORD)(nOffset+bFileNameLen)<=dwLen)
				{
					memcpy(pDataBuff,pData+nOffset,bFileNameLen);
					pDataBuff[bFileNameLen] = 0x00; // zero terminate the string
					pFileName = pDataBuff;
					nOffset+=bFileNameLen;
					CIAPPMMI_DEBUG(("File: %s\r\n",pFileName));
					// [shouquan.tang] Replace the Assert to CI_Assert
                    CI_Assert((DWORD)(nOffset+4)<=dwLen);
					if((DWORD)(nOffset+4)<=dwLen)
					{
						dwFileLen = (pData[nOffset]<<24)|(pData[nOffset+1]<<16)|(pData[nOffset+2]<<8)|pData[nOffset+3];
						nOffset+=4;
						// [shouquan.tang] Replace the Assert to CI_Assert
                        CI_Assert(nOffset+dwFileLen<=dwLen);
						if(!(nOffset+dwFileLen<=dwLen))
						{
							dwFileLen = 0;
						}
					}
				}
			}
			if(fFileOK)
				CIHost_AppMMIfileRequestAck(hCI->bSlotNum, fFileOK, eRequestType, (const char*)pFileName, pData+nOffset, dwFileLen);
			else
				CIHost_AppMMIfileRequestAck(hCI->bSlotNum, fFileOK, eRequestType, (const char*)pFileName, NULL, 0);
		}
		else if(eRequestType==CI_AM_FILEREQTYPE_DATA)
		{
			CIHost_AppMMIfileRequestAck(hCI->bSlotNum, fFileOK, eRequestType, NULL, pData+nOffset, wLen-2);
		}
		else
		{
			// [shouquan.tang] Replace the Assert to CI_Assert
            CI_Assert(0);
			CIAPPMMI_DEBUG(("Application MMI FileRequestACK invalid request type!\r\n"));
			return; // -> silently discard this APDU
		}
		break;

	case TAG_T_APPABORTREQ:
		CIAPPMMI_DEBUG(("Application MMI application abort request\r\n"));

		// For the time being allow an abort req/ack code of at the most 127 byte
		// [shouquan.tang] Replace the Assert to CI_Assert
        CI_Assert(wLen<=127);
		if(wLen>=MAX_AM_FILE_NAME)
		{
			CIAPPMMI_DEBUG(("Application MMI abort request code exceeds maximum!\r\n"));
			return; // -> silently discard this APDU
		}

		memcpy(pDataBuff+4,pData+nOffset,wLen);
		if(CIHost_AppMMIabort(hCI->bSlotNum, TRUE, pDataBuff+4, &wLen) == TRUE)
		{
			hCI->fAppMMIActive = FALSE;
			// Immediately send an AbortAck with the AbortAck code given in pDataBuff and its length in wLen
			// [shouquan.tang] Replace the Assert to CI_Assert
            CI_Assert(wLen<=127);
			if(wLen<=127)
			{
				pDataBuff[0] = TAG_T_APPABORTACK>>16&0xFF;
				pDataBuff[1] = TAG_T_APPABORTACK>>8&0xFF;
				pDataBuff[2] = TAG_T_APPABORTACK&0xFF;
				pDataBuff[3] = (BYTE)wLen;
				// ! AbortAck code already in the right place
				Send_Session_Data(hCI,hSession,pDataBuff,(WORD)(wLen+4));
			}
		}
		break;

	case TAG_T_APPABORTACK:
		CIAPPMMI_DEBUG(("Application MMI application abort acknowledge\r\n"));

		// For the time being allow an abort req/ack code of at the most 127 byte
		// [shouquan.tang] Replace the Assert to CI_Assert
        CI_Assert(wLen<=127);
		if(wLen>=MAX_AM_FILE_NAME)
		{
			CIAPPMMI_DEBUG(("Application MMI abort ack code exceeds maximum!\r\n"));
			return; // -> silently discard this APDU
		}

		hCI->fAppMMIActive = FALSE;
		memcpy(pDataBuff,pData+nOffset,wLen);
		CIHost_AppMMIabort(hCI->bSlotNum, FALSE, pDataBuff, &wLen);
		break;
	default:
		CIAPPMMI_DEBUG(("CIAPPMMI_Data unknown APDU tag %06X\r\n",(int)dwTag));
		break;
	}
	
	return;
}

void CIAPPMMI_Close(CI_HANDLE hCI, CISESSIONHANDLE hSession)
{
	CIAPPMMI_DEBUG(("CIAPPMMI_Close\r\n"));
	if(hCI->fAppMMIActive)
	{
		CIHost_AppMMIabort(hCI->bSlotNum, TRUE, NULL, 0);
		hCI->fAppMMIActive = 0;
	}
	hCI->hAppMMISession = 0;
	return;
}

BOOL CIAPPMMI_FileRequest(BYTE bSlotIndex,const BYTE *pBuff,WORD wBuffLen,CI_AM_FILEREQTYPE_CODE eRequestType)
{
	CI_HANDLE hCI;
	int nTotalBytes;
	int nVarLengthBytes = 3;
	BYTE *pTxBuff;

	CIAPPMMI_DEBUG(("CIAPPMMI_FileRequest with length: %d and request Type: %d %s\r\n",wBuffLen,eRequestType,pBuff));

	// Check for a valid slot
	// [shouquan.tang] Replace the Assert to CI_Assert
    CI_Assert(bSlotIndex<MAX_CI_SLOTS);
	if(bSlotIndex>=MAX_CI_SLOTS)
	{
		// Invalid slot index return FALSE, indicating no success
		CIAPPMMI_DEBUG(("CIAPPMMI_FileRequest ERROR invalid slot index!\r\n"));
		return FALSE;
	}
	hCI = &CiSlot[bSlotIndex];

	// Check for an existing yet opened application MMI session
	// [shouquan.tang] Replace the Assert to CI_Assert
    CI_Assert(hCI->hAppMMISession);
	if(!hCI->hAppMMISession)
	{
		// Application MMI session NOT yet opened
		CIAPPMMI_DEBUG(("CIAPPMMI_FileRequest ERROR Application MMI session NOT yet opened!\r\n"));
		return FALSE;
	}
	
	// Check for NULL-pointer buffer
	// [shouquan.tang] Replace the Assert to CI_Assert
    CI_Assert(pBuff);
	if(!pBuff)
	{
		// FileRequest with NULL-pointer
		CIAPPMMI_DEBUG(("CIAPPMMI_FileRequest ERROR FileRequest with NULL-pointer!\r\n"));
		return FALSE;
	}

	// Prepare the FileRequestAck buffer and than transmit it over the opened session
	if(wBuffLen<256)
	{
		nVarLengthBytes--;
		if(wBuffLen<128)
		{
			nVarLengthBytes--;
		}
	}
	nTotalBytes = 3+nVarLengthBytes+1+wBuffLen;

	// As we can only Send_SPDU with length being WORD make sure nTatalBytes does not exceed 0xFFFF
	// [shouquan.tang] Replace the Assert to CI_Assert
    CI_Assert(nTotalBytes<=0xFFFF);
	if(nTotalBytes>0xFFFF)
	{
		CIAPPMMI_DEBUG(("CIAPPMMI_FileRequest ERROR nTotalBytes>0xFFFF!\r\n"));
		return FALSE;
	}

	pTxBuff = (BYTE*)malloc(nTotalBytes);
	// [shouquan.tang] Replace the Assert to CI_Assert
    CI_Assert(pTxBuff);
	if(!pTxBuff) {
		CIAPPMMI_DEBUG(("CIAPPMMI_FileRequest ERROR tx buffer allocation failed!\r\n"));
		return FALSE;
	}

	pTxBuff[0] = TAG_T_FILEREQ>>16&0xFF;
	pTxBuff[1] = TAG_T_FILEREQ>>8&0xFF;
	pTxBuff[2] = TAG_T_FILEREQ&0xFF;
	switch(nVarLengthBytes)
	{
	case 1:
		pTxBuff[3] = (BYTE)(wBuffLen+1);
		break;
	case 2:
		pTxBuff[3] = 0x81;
		pTxBuff[4] = (BYTE)(wBuffLen+1);
		break;
	case 3:
		pTxBuff[3] = 0x82;
		pTxBuff[4] = (BYTE)(((wBuffLen+1)>>8)&0xFF);
		pTxBuff[5] = (BYTE)((wBuffLen+1)&0xFF);
		break;
	}
	pTxBuff[3+nVarLengthBytes] = (BYTE)eRequestType;
	memcpy(pTxBuff+3+nVarLengthBytes+1,pBuff,wBuffLen);
	Send_Session_Data(hCI,hCI->hAppMMISession,pTxBuff,(WORD)nTotalBytes);

	// free memory again!
	free(pTxBuff);
	return TRUE;
}

BOOL CIAPPMMI_Abort(BYTE bSlotIndex, const BYTE *pBuff, WORD wBuffLen, BYTE fRequest)
{
	CI_HANDLE hCI;
	BYTE pDataBuff[127+4];

	CIAPPMMI_DEBUG(("CIAPPMMI_Abort with length: %d and fRrequest: %d\r\n",wBuffLen,fRequest));

	// Check for a valid slot
	// [shouquan.tang] Replace the Assert to CI_Assert
    CI_Assert(bSlotIndex<MAX_CI_SLOTS);
	if(bSlotIndex>=MAX_CI_SLOTS)
	{
		// Invalid slot index return FALSE, indicating no success
		CIAPPMMI_DEBUG(("CIAPPMMI_Abort ERROR invalid slot index!\r\n"));
		return FALSE;
	}
	hCI = &CiSlot[bSlotIndex];
	
	// Check for an existing yet opened application MMI session
	// [shouquan.tang] Replace the Assert to CI_Assert
    CI_Assert(hCI->hAppMMISession);
	if(!hCI->hAppMMISession)
	{
		// Application MMI session NOT yet opened
		CIAPPMMI_DEBUG(("CIAPPMMI_Abort ERROR Application MMI session NOT yet opened!\r\n"));
		return FALSE;
	}
	
	// Check for NULL-pointer buffer
	// [shouquan.tang] Replace the Assert to CI_Assert
    CI_Assert(pBuff||wBuffLen==0);
	if(!pBuff&&wBuffLen!=0)
	{
		// Abort Req/Ack with NULL-pointer
		CIAPPMMI_DEBUG(("CIAPPMMI_Abort ERROR Abort with NULL-pointer!\r\n"));
		return FALSE;
	}

	// For the time being allow an abort req/ack code of at the most 127 byte
	// [shouquan.tang] Replace the Assert to CI_Assert
    CI_Assert(wBuffLen<=127);
	if(wBuffLen>=MAX_AM_FILE_NAME)
	{
		CIAPPMMI_DEBUG(("CIAPPMMI_Abort ERROR ack code exceeds maximum!\r\n"));
		return FALSE; // -> silently discard this APDU
	}
	if(fRequest)
	{
		pDataBuff[0] = TAG_T_APPABORTREQ>>16&0xFF;
		pDataBuff[1] = TAG_T_APPABORTREQ>>8&0xFF;
		pDataBuff[2] = TAG_T_APPABORTREQ&0xFF;
	}
	else
	{
		pDataBuff[0] = TAG_T_APPABORTACK>>16&0xFF;
		pDataBuff[1] = TAG_T_APPABORTACK>>8&0xFF;
		pDataBuff[2] = TAG_T_APPABORTACK&0xFF;
	}
	pDataBuff[3] = (BYTE)wBuffLen;
	memcpy(pDataBuff+4,pBuff,wBuffLen);
	Send_Session_Data(hCI,hCI->hAppMMISession,pDataBuff,(WORD)(4+wBuffLen));
	return TRUE;
}

#endif // ENABLE_CI_APPLICATIONMMI
#endif // ENABLE_CI
