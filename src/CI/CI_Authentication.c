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
#ifdef ENABLE_CI_AUTH

//#define AUTH_DEBUG_ENABLE

#ifdef AUTH_DEBUG_ENABLE 
#define AUTH_DEBUG(x) printf x
#else
#define AUTH_DEBUG(x)
#endif

static BOOL Authentication_Connect(CI_HANDLE hCI, WORD hSession);
static void Authentication_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen);
static void Authentication_Close(CI_HANDLE hCI, WORD hSession);


const RESOURCE_INFO Authentication = { RESOURCEID_AUTHENTICATION,Authentication_Connect,Authentication_Data,Authentication_Close};


BOOL Authentication_Connect(CI_HANDLE hCI, WORD hSession)
{
	AUTH_DEBUG(("Authentication_Connect\r\n"));
	return TRUE;
}

void Authentication_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen)
{
	DWORD dwTag;
	WORD wOffset = 3;
	WORD wLen;
	WORD wProtocolId;
	BYTE pAuthResponse[10+MAX_AUTH_RESPONSE_SIZE]; // header + data
	WORD wAuthResponseLen;
	wOffset += CI_DecodeLengthField(&pData[wOffset],&wLen);

	dwTag = pData[0]<<16|pData[1]<<8|pData[2];
	AUTH_DEBUG(("Authentication_Data %06X\r\n",dwTag));
	
	switch(dwTag)
	{
	case TAG_T_AUTHREQ:
		wProtocolId = (pData[wOffset]<<8)|pData[wOffset+1];
		if(CIHost_Authenticate(wProtocolId, &pData[wOffset+2],(WORD)(dwLen-wOffset-2),&pAuthResponse[8],&wAuthResponseLen))
		{
			// add the size of the auth_protocol_id to the response packet size
			wAuthResponseLen+=2;
			if(wAuthResponseLen<=0x7F)
			{
				// 1 byte length
				pAuthResponse[2] = (TAG_T_AUTHRESP>>16)&0xFF;
				pAuthResponse[3] = (TAG_T_AUTHRESP>>8)&0xFF;
				pAuthResponse[4] = (TAG_T_AUTHRESP>>0)&0xFF;
				pAuthResponse[5] = (BYTE)wAuthResponseLen;
				pAuthResponse[6] = wProtocolId>>8;
				pAuthResponse[7] = wProtocolId>>0;
				Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,&pAuthResponse[2], (WORD)(wAuthResponseLen+4));
			} else {
				// 3 byte length
				pAuthResponse[0] = (TAG_T_AUTHRESP>>16)&0xFF;
				pAuthResponse[1] = (TAG_T_AUTHRESP>>8)&0xFF;
				pAuthResponse[2] = (TAG_T_AUTHRESP>>0)&0xFF;
				pAuthResponse[3] = 0x82;
				pAuthResponse[4] = (BYTE)(wAuthResponseLen>>8);
				pAuthResponse[5] = (BYTE)(wAuthResponseLen>>0);
				pAuthResponse[6] = wProtocolId>>8;
				pAuthResponse[7] = wProtocolId>>0;
				Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,pAuthResponse, (WORD)(wAuthResponseLen+6));
			}
			
		}
		break;
	}
	
	return;
}

void Authentication_Close(CI_HANDLE hCI, WORD hSession)
{
	AUTH_DEBUG(("Authentication_Close\r\n"));
	return;
}

#endif // ENABLE_CI_AUTH
#endif // ENABLE_CI
