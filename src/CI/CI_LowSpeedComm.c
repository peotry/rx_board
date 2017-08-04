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
#ifdef ENABLE_CI_LOWSPEEDCOMM

//#define LSC_DEBUG_ENABLE

#ifdef LSC_DEBUG_ENABLE 
#define LSC_DEBUG(x) printf x
FAILRELEASEBUILD
#else
#define LSC_DEBUG(x)
#endif


// these are the three default resources. For Modems, the host is responsible
// for declaring an appropriate RESOURCE_INFO structure.
const RESOURCE_INFO LowSpeedCommConax =			{ RESOURCEID_LOWSPEEDCOMM_CONAX,LowSpeedComm_Connect,LowSpeedComm_Data,LowSpeedComm_Close};
const RESOURCE_INFO LowSpeedCommSerial1 =		{ RESOURCEID_LOWSPEEDCOMM_SERIAL1,LowSpeedComm_Connect,LowSpeedComm_Data,LowSpeedComm_Close};
const RESOURCE_INFO LowSpeedCommCableModem =	{ RESOURCEID_LOWSPEEDCOMM_CABLE1,LowSpeedComm_Connect,LowSpeedComm_Data,LowSpeedComm_Close};



BOOL LowSpeedComm_Connect(CI_HANDLE hCI, WORD hSession)
{
	LSC_DEBUG(("LowSpeedComm_Connect\r\n"));

	// only allow one open session
	if(hCI->dwLSCSession)
		return FALSE;

	hCI->dwLSCSession = hSession;
	hCI->fLSCConnecting = FALSE;
	hCI->fLSCConnected = FALSE;
	hCI->bLSCSendPhase = 0;
	hCI->bLSCReceivePhase = 0;
	return TRUE;
}



#define COMMSREPLYID_CONNECTACK		(0x01)
#define COMMSREPLYID_DISCONNECTACK	(0x02)
#define COMMSREPLYID_SETPARAMSACK	(0x03)
#define COMMSREPLYID_STATUS_REPLY	(0x04)
#define COMMSREPLYID_GETNEXTBUFACK	(0x05)
#define COMMSREPLYID_SENDACK		(0x06)

void LowSpeedComm_SendCommsReply(CI_HANDLE hCI, WORD hSession, BYTE bReplyId, BYTE bReturnValue)
{
	BYTE bCOMMSREPLY[6]={0x9f,0x8c,0x02,0x02,0x00,0x00};
	bCOMMSREPLY[4] = bReplyId;
	bCOMMSREPLY[5] = bReturnValue;
	Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,bCOMMSREPLY,sizeof(bCOMMSREPLY));
}

void CI_ConnectResult(CI_HANDLE hCI, BOOL fSuccess)
{
	// ignore this call, if we are not connecting
	if(!hCI->fLSCConnecting)
		return;
	hCI->fLSCConnecting = FALSE;
	if(fSuccess)
	{
		hCI->fLSCConnected = TRUE;
		LowSpeedComm_SendCommsReply(hCI,hCI->dwLSCSession,COMMSREPLYID_CONNECTACK,0);
	} else {
		LowSpeedComm_SendCommsReply(hCI,hCI->dwLSCSession,COMMSREPLYID_CONNECTACK,0xFF);
	}
}


void LowSpeedComm_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen)
{
	DWORD dwTag;
	int nOffset = 3;
	WORD wLen,wLen2;
	nOffset += CI_DecodeLengthField(&pData[nOffset],&wLen);

	dwTag = pData[0]<<16|pData[1]<<8|pData[2];
	LSC_DEBUG(("LowSpeedComm_Data %06X\r\n",dwTag));
	switch(dwTag)
	{
	case TAG_T_COMMS_CMD:
		LSC_DEBUG(("TAG_T_COMMS_CMD\r\n"));
		
#define LSC_CMDID_CONNECTONCHANNEL		(0x01)
#define LSC_CMDID_DISCONNECTONCHANNEL	(0x02)
#define LSC_CMDID_SETPARAMS				(0x03)
#define LSC_CMDID_ENQUIRESTATUS			(0x04)
#define LSC_CMDID_GETNEXTBUF			(0x05)
		switch(pData[nOffset])
		{
		case LSC_CMDID_CONNECTONCHANNEL:
			LSC_DEBUG(("ConnectonChannel\r\n"));
			dwTag = pData[nOffset+1]<<16|pData[nOffset+2]<<8|pData[nOffset+3];
			if(dwTag != TAG_T_CONNECTION_DESCRIPTOR) {
				// ignore bad data
				return;
			}
			hCI->fLSCConnecting = TRUE;
			LSC_DEBUG(("TAG_T_CONNECTION_DESCRIPTOR\r\n"));
			nOffset+=4;
			nOffset += CI_DecodeLengthField(&pData[nOffset],&wLen2);
			LSC_DEBUG(("decoded len %04X\r\n",wLen2));
	#define LSC_CONNDESC_TELEPHONE			(0x01)
	#define LSC_CONNDESC_CABLE				(0x02)
			switch(pData[nOffset])
			{
			case LSC_CONNDESC_TELEPHONE:
				LSC_DEBUG(("Telephone\r\n"));
				//PrintHexDump(&pData[wOffset+1],pData[wOffset+2]);
				//DumpTelephoneDesc(&pData[wOffset+1]);
				CIHost_ConnectReturnChannel_Telephone(hCI,&pData[nOffset+1]);
				break;
			case LSC_CONNDESC_CABLE:
				LSC_DEBUG(("Cable ChannelId %02X\r\n",pData[nOffset+1]));
				CIHost_ConnectReturnChannel_Cable(hCI,pData[nOffset+1]);
				break;
			// The Conax returnchannel code that once was here
			// had to be removed due to NDA reasons. If you are an 
			// active Conax CA system licensee, please ask for a
			// full Conax returnchannel handler for CI based
			// PPV ordering support.
			}
			break;

		case LSC_CMDID_DISCONNECTONCHANNEL:
			LSC_DEBUG(("DisconnectonChannel\r\n"));
			CIHost_DisconnectReturnChannel(hCI);
			LowSpeedComm_SendCommsReply(hCI, hSession, COMMSREPLYID_DISCONNECTACK, 0);
			break;

		case LSC_CMDID_SETPARAMS:
			LSC_DEBUG(("SetParams bufSize %02X timeout %02X\r\n", pData[nOffset+1],pData[nOffset+2]));
			hCI->bLSCBufferSize = pData[nOffset+1];
			hCI->bLSCTimeout = pData[nOffset+2];
			LowSpeedComm_SendCommsReply(hCI, hSession, COMMSREPLYID_SETPARAMSACK, pData[nOffset]);
			break;

		case LSC_CMDID_ENQUIRESTATUS:
			LSC_DEBUG(("Enquire_Status\r\n"));
			if(hCI->fLSCConnected)
				LowSpeedComm_SendCommsReply(hCI, hSession, COMMSREPLYID_STATUS_REPLY, 1);
			else
				LowSpeedComm_SendCommsReply(hCI, hSession, COMMSREPLYID_STATUS_REPLY, 0);
			break;
			
		case LSC_CMDID_GETNEXTBUF:
			LSC_DEBUG(("GetNextBuf phase %02X\r\n",pData[nOffset+1]));
			break;
		}
	


		
		break;
	case TAG_T_COMMS_SENDLAST:
		LSC_DEBUG(("TAG_T_COMMS_SENDLAST phase %02X\r\n",pData[nOffset]));
		nOffset++;
		if(pData[nOffset] != hCI->bLSCSendPhase)
		{
			// signal a phase error
			LowSpeedComm_SendCommsReply(hCI, hSession, COMMSREPLYID_SENDACK, 0xFF);
			break;
		}
		hCI->bLSCSendPhase = (BYTE)(1 - pData[nOffset]);
		
		/*
		LSC_DEBUG(("full comms_send\r\n"));
		PrintHexDump(pData,dwLen);
		LSC_DEBUG(("raw data\r\n"));
		PrintHexDump(pData+wOffset,dwLen-wOffset);
		LSC_DEBUG(("const char pszMsg[] = {"));
		{
			DWORD i;
			for(i=0;wOffset+i<dwLen;i++)
			{
				if(i%16==0) {
					LSC_DEBUG(("0x%02X,\r\n",pData[wOffset+i]));
				} else { 
					LSC_DEBUG(("0x%02X,",pData[wOffset+i]));
				}
			}
		}
		LSC_DEBUG(("};\r\n"));
		*/

		if(CIHost_Send(hCI, pData+nOffset, dwLen-nOffset))
		{
			LowSpeedComm_SendCommsReply(hCI, hSession, COMMSREPLYID_SENDACK, pData[nOffset]);
		}


		break;
	case TAG_T_COMMS_SENDMORE:
		LSC_DEBUG(("TAG_T_COMMS_SENDMORE\r\n"));
		PrintHexDump(pData,dwLen);
		break;
	}

	return;
}

void CI_LSC_Data(CI_HANDLE hCI, const BYTE *pData, WORD wLength)
{
	BYTE pResponse[10+MAX_LSC_SENDSIZE]; // header + data
	CISESSIONHANDLE hSession = hCI->dwLSCSession;
	
	LSC_DEBUG(("CI_LSC_Data (%d)\r\n", (int)wLength));
	if(hCI->dwLSCSession == 0)
		return;

	if(wLength > MAX_LSC_SENDSIZE)
	{
		LSC_DEBUG(("Returnchannel data too long (%d)\r\n", (int)wLength));
		return;
	}

	memcpy(&pResponse[7], pData, wLength);

	// add the size of the auth_protocol_id to the response packet size
	wLength+=2;
	if(wLength<=0x7F)
	{
		// 1 byte length
		pResponse[2] = (TAG_T_COMMS_RCVLAST>>16)&0xFF;
		pResponse[3] = (TAG_T_COMMS_RCVLAST>>8)&0xFF;
		pResponse[4] = (TAG_T_COMMS_RCVLAST>>0)&0xFF;
		pResponse[5] = (BYTE)wLength;
		pResponse[6] = hCI->bLSCReceivePhase;
		Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,&pResponse[2], (WORD)(wLength+3));
	} else {
		// 3 byte length
		pResponse[0] = (TAG_T_COMMS_RCVLAST>>16)&0xFF;
		pResponse[1] = (TAG_T_COMMS_RCVLAST>>8)&0xFF;
		pResponse[2] = (TAG_T_COMMS_RCVLAST>>0)&0xFF;
		pResponse[3] = 0x82;
		pResponse[4] = (BYTE)(wLength>>8);
		pResponse[5] = (BYTE)(wLength>>0);
		pResponse[6] = hCI->bLSCReceivePhase;
		Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,pResponse, (WORD)(wLength+5));
	}
	hCI->bLSCReceivePhase = (BYTE)(1-hCI->bLSCReceivePhase);


}


void LowSpeedComm_Close(CI_HANDLE hCI, WORD hSession)
{
	LSC_DEBUG(("LowSpeedComm_Close\r\n"));

	hCI->dwLSCSession = 0;
	hCI->fLSCConnecting = FALSE;
	hCI->fLSCConnected = FALSE;
	hCI->bLSCSendPhase = 0;
	hCI->bLSCReceivePhase = 0;

	return;
}

#endif // ENABLE_CI_LOWSPEEDCOMM
#endif // ENABLE_CI
