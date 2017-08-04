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
#ifdef ENABLE_CI_COPYPROTECTION

// #define CP_DEBUG_ENABLE

#ifdef CP_DEBUG_ENABLE 
#define CP_DEBUG(x) printf x
FAILRELEASEBUILD
#else
#define CP_DEBUG(x)
#endif

static BOOL CopyProtection_Connect(CI_HANDLE hCI, WORD hSession);
static void CopyProtection_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen);
static void CopyProtection_Close(CI_HANDLE hCI, WORD hSession);


const RESOURCE_INFO CopyProtection = { RESOURCEID_COPYPROTECTION,CopyProtection_Connect,CopyProtection_Data,CopyProtection_Close};


BOOL CopyProtection_Connect(CI_HANDLE hCI, WORD hSession)
{
	CP_DEBUG(("CopyProtection_Connect\r\n"));
	return TRUE;
}

void CopyProtection_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen)
{
	DWORD dwTag;
	int nOffset = 3;
	WORD wLen;
	DWORD dwCPId;
	BYTE pCPResponse[10+MAX_CP_RESPONSE_SIZE]; // header + data
	WORD wCPResponseLen;
	CI_CP_STATUS eStatus;
	nOffset += CI_DecodeLengthField(&pData[nOffset],&wLen);

	dwTag = pData[0]<<16|pData[1]<<8|pData[2];
	CP_DEBUG(("CopyProtection_Data %06X\r\n",dwTag));
	
	switch(dwTag)
	{
	case TAG_T_CPQUERY:
		CP_DEBUG(("CP Query\r\n"));
		dwCPId = (pData[nOffset]<<16)|(pData[nOffset+1]<<8)|pData[nOffset+2];
		
		eStatus = CIHost_GetCopyProtectionStatus(&dwCPId);
		pCPResponse[0] = (TAG_T_CPREPLY>>16)&0xFF;
		pCPResponse[1] = (TAG_T_CPREPLY>>8)&0xFF;
		pCPResponse[2] = (TAG_T_CPREPLY>>0)&0xFF;
		pCPResponse[3] = 4;
		pCPResponse[4] = (BYTE)(dwCPId>>16);
		pCPResponse[5] = (BYTE)(dwCPId>>8);
		pCPResponse[6] = (BYTE)(dwCPId>>0);
		pCPResponse[7] = (BYTE)(eStatus);
		Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,pCPResponse, 8);

		
		break;
	case TAG_T_CPCOMMAND:
		dwCPId = (pData[nOffset]<<16)|(pData[nOffset+1]<<8)|pData[nOffset+2];
		CP_DEBUG(("CP Command\r\n"));
		if(CIHost_CopyProtectionCommand(dwCPId, &pData[nOffset+3],(WORD)(dwLen-nOffset-2),&pCPResponse[9],&wCPResponseLen))
		{
			// add the size of the auth_protocol_id to the response packet size
			wCPResponseLen+=3;
			if(wCPResponseLen<=0x7F)
			{
				// 1 byte length
				pCPResponse[2] = (TAG_T_CPRESPONSE>>16)&0xFF;
				pCPResponse[3] = (TAG_T_CPRESPONSE>>8)&0xFF;
				pCPResponse[4] = (TAG_T_CPRESPONSE>>0)&0xFF;
				pCPResponse[5] = (BYTE)wCPResponseLen;
				pCPResponse[6] = (BYTE)(dwCPId>>16);
				pCPResponse[7] = (BYTE)(dwCPId>>8);
				pCPResponse[8] = (BYTE)(dwCPId>>0);
				Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,&pCPResponse[2], (WORD)(wCPResponseLen+4));
			} else {
				// 3 byte length
				pCPResponse[0] = (TAG_T_CPRESPONSE>>16)&0xFF;
				pCPResponse[1] = (TAG_T_CPRESPONSE>>8)&0xFF;
				pCPResponse[2] = (TAG_T_CPRESPONSE>>0)&0xFF;
				pCPResponse[3] = 0x82;
				pCPResponse[4] = (BYTE)(wCPResponseLen>>8);
				pCPResponse[5] = (BYTE)(wCPResponseLen>>0);
				pCPResponse[6] = (BYTE)(dwCPId>>16);
				pCPResponse[7] = (BYTE)(dwCPId>>8);
				pCPResponse[8] = (BYTE)(dwCPId>>0);
				Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,pCPResponse, (WORD)(wCPResponseLen+6));
			}			
		} else {
			pCPResponse[0] = (TAG_T_CPREPLY>>16)&0xFF;
			pCPResponse[1] = (TAG_T_CPREPLY>>8)&0xFF;
			pCPResponse[2] = (TAG_T_CPREPLY>>0)&0xFF;
			pCPResponse[3] = 4;
			pCPResponse[4] = (BYTE)(dwCPId>>16);
			pCPResponse[5] = (BYTE)(dwCPId>>8);
			pCPResponse[6] = (BYTE)(dwCPId>>0);
			pCPResponse[7] = CI_CP_STATUS_ID_MISMATCH;
			Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,pCPResponse, 8);
		}		
		break;
	default:
		CP_DEBUG(("Unknown TAG on copy protection resource\r\n"));
		break;
	}
	
	return;
}

void CopyProtection_Close(CI_HANDLE hCI, WORD hSession)
{
	CP_DEBUG(("CopyProtection_Close\r\n"));
	return;
}

#endif // ENABLE_CI_COPYPROTECTION
#endif // ENABLE_CI
