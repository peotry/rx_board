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

// #define RESMGR_DEBUG_ENABLE

#ifdef RESMGR_DEBUG_ENABLE 
#define RESMGR_DEBUG(x) printf x
FAILRELEASEBUILD
#else
#define RESMGR_DEBUG(x)
#endif

static BOOL ResMgr_Connect(CI_HANDLE hCI, WORD hSession);
static void ResMgr_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen);
static void ResMgr_Close(CI_HANDLE hCI, WORD hSession);

const RESOURCE_INFO ResMgr =		{ RESOURCEID_RESOURCEMANAGER,ResMgr_Connect,ResMgr_Data,ResMgr_Close };


const BYTE RESMGR_PROFILE_ENQ[4]={0x9f,0x80,0x10,00};
const BYTE RESMGR_PROFILE_CHANGE[4]={0x9f,0x80,0x12,00};


///////////////////////////////////////////////////////////////////////////////////////////
// Resource Manager
BOOL ResMgr_Connect(CI_HANDLE hCI, WORD hSession)
{
	RESMGR_DEBUG(("ResMgr_Connect, sending profile enq\r\n"));
	Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,RESMGR_PROFILE_ENQ,4);
	hCI->fHighPrio = TRUE;
	hCI->dwResMgrSession = hSession;
	return TRUE;
}

// -------------------------------------------------------------
void ResMgr_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen)
{
	DWORD dwTag;
	int nOffset = 3;
	WORD wLen;
	WORD wNumResources = 0;
	int i;
	BYTE RESMGR_PROFILE[4*MAX_CI_RESOURCES*4];
	nOffset += CI_DecodeLengthField(&pData[nOffset],&wLen);
	
	RESMGR_DEBUG(("ResMgr_Data, "));

	dwTag = pData[0]<<16|pData[1]<<8|pData[2];
	switch(dwTag)
	{
    	case TAG_T_PROFILE:
    		RESMGR_DEBUG(("RX Profile (%d resources), sending profile change\r\n",(pData[3]/4)));
#ifdef RESMGR_DEBUG_ENABLE
    		PrintHexDump(pData,dwLen);
#endif
    		Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,RESMGR_PROFILE_CHANGE,4);
    		break;

    	case TAG_T_PROFILE_ENQ:
    		RESMGR_DEBUG(("RX Profile Enq, sending profile\r\n"));
    		for(i=0;i<MAX_CI_RESOURCES;i++) {
    			if(g_CiResources[i])
    				wNumResources++;
    		}
    		RESMGR_PROFILE[0]=0x9f;
    		RESMGR_PROFILE[1]=0x80;
    		RESMGR_PROFILE[2]=0x11;
    		RESMGR_PROFILE[3]=(BYTE)(wNumResources*4);
    		nOffset=4;
    		for(i=0;i<MAX_CI_RESOURCES;i++) {
    			if(g_CiResources[i])
    			{
    				RESMGR_PROFILE[nOffset+0]=(BYTE)(g_CiResources[i]->dwResourceID>>24);
    				RESMGR_PROFILE[nOffset+1]=(BYTE)(g_CiResources[i]->dwResourceID>>16);
    				RESMGR_PROFILE[nOffset+2]=(BYTE)(g_CiResources[i]->dwResourceID>>8);
    				RESMGR_PROFILE[nOffset+3]=(BYTE)(g_CiResources[i]->dwResourceID>>0);
    				nOffset+=4;
    			}
    		}
    		Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,RESMGR_PROFILE,(WORD)nOffset);
    		break;
    	default:
    		RESMGR_DEBUG(("Unknown TAG on resource manager resource\r\n"));
    		break;

	}
}

// -------------------------------------------------------------
void ResMgr_Close(CI_HANDLE hCI, WORD hSession)
{
	RESMGR_DEBUG(("ResMgr Close\r\n"));
}

#ifdef ENABLE_CI_PROFILE2
// This is currently unsupported test code

static BOOL ResMgr_Connect2(CI_HANDLE hCI, WORD hSession);
static void ResMgr_Data2(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen);
static void ResMgr_Close2(CI_HANDLE hCI, WORD hSession);

const RESOURCE_INFO ResMgrV2 =		{ RESOURCEID_RESOURCEMANAGERV2,ResMgr_Connect2,ResMgr_Data2,ResMgr_Close2 };

BOOL ResMgr_Connect2(CI_HANDLE hCI, WORD hSession)
{
	RESMGR_DEBUG(("ResMgr_ConnectV2, sending profile enq\r\n"));
	Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,RESMGR_PROFILE_ENQ,4);
	hCI->fHighPrio = TRUE;
	hCI->dwResMgrSession = hSession;
	return TRUE;
}

void ResMgr_Data2(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen)
{
	DWORD dwTag;
	int nOffset = 3;
	WORD wLen;
	BYTE bMessage[5];
	BYTE bModuleId;
	nOffset += CI_DecodeLengthField(&pData[nOffset],&wLen);
	
	RESMGR_DEBUG(("ResMgr_Data2\r\n"));

	dwTag = pData[0]<<16|pData[1]<<8|pData[2];
	switch(dwTag)
	{
	case TAG_T_MODULEID_SEND:
		bModuleId = pData[nOffset];
		// **TODO** test code		
		bMessage[0] = (TAG_T_MODULEID_COMMAND>>16)&0xFF;
		bMessage[1] = (TAG_T_MODULEID_COMMAND>>8)&0xFF;
		bMessage[2] = (TAG_T_MODULEID_COMMAND>>0)&0xFF;
		bMessage[3] = 2;
		if(bModuleId == 1)
			bMessage[4] = 0x02;
		else
			bMessage[4] = 0x01;
		Send_SPDU_Data(hCI, hCI->CiSessions[hSession].bTransportConnID, hCI->CiSessions[hSession].wSessionID, bMessage, 5);		
		break;
	default:
		ResMgr_Data(hCI, hSession, pData, dwLen);
		break;
	}
}


void ResMgr_Close2(CI_HANDLE hCI, WORD hSession)
{
	RESMGR_DEBUG(("ResMgr_Close2\r\n"));
}
#endif // ENABLE_CI_PROFILE2

#endif // ENABLE_CI

