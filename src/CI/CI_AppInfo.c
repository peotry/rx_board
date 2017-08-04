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

//#define APPINFO_DEBUG_ENABLE

#ifdef APPINFO_DEBUG_ENABLE
#define APPINFO_DEBUG(x) printf x
////FAILRELEASEBUILD
#else
#define APPINFO_DEBUG(x)
#endif


static BOOL AppInfo_Connect(CI_HANDLE hCI, WORD hSession);
static void AppInfo_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen);
static void AppInfo_Close(CI_HANDLE hCI, WORD hSession);
static BOOL AppInfo_Connect2(CI_HANDLE hCI, WORD hSession);
static void AppInfo_Data2(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen);
static void AppInfo_Close2(CI_HANDLE hCI, WORD hSession);
static BOOL AppInfo_Connect3(CI_HANDLE hCI, WORD hSession);
static void AppInfo_Data3(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen);
static void AppInfo_Close3(CI_HANDLE hCI, WORD hSession);


const BYTE APPINFO_ENQ[4]={0x9f,0x80,0x20,00};
const RESOURCE_INFO AppInfo =		{ RESOURCEID_APPINFO,AppInfo_Connect,AppInfo_Data,AppInfo_Close };
const RESOURCE_INFO AppInfoV2 =		{ RESOURCEID_APPINFOV2,AppInfo_Connect2,AppInfo_Data2,AppInfo_Close2 };
const RESOURCE_INFO AppInfoV3 =		{ RESOURCEID_APPINFOV3,AppInfo_Connect3,AppInfo_Data3,AppInfo_Close3 };

///////////////////////////////////////////////////////////////////////////////////////////
// Application Info
BOOL AppInfo_Connect(CI_HANDLE hCI, WORD hSession)
{
	APPINFO_DEBUG(("AppInfo Connect\r\n"));
	hCI->wAppInfoSession = hSession;
	Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,APPINFO_ENQ,4);

	return TRUE;
}

// -------------------------------------------------------------
void AppInfo_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen)
{
	DWORD dwTag;
	DWORD i;
	int nOffset = 3;
	WORD wLen;
	nOffset += CI_DecodeLengthField(&pData[nOffset],&wLen);

	dwTag = pData[0]<<16|pData[1]<<8|pData[2];
	APPINFO_DEBUG(("AppInfo %06X\r\n",dwTag));

#define TAG_T_APPINFO_ENQ			(0x9f8020)
#define TAG_T_APPINFO				(0x9f8021)
#define TAG_T_ENTERMENU				(0x9f8022)

#define TAG_T_REQUESTRESET			(0x9F8023)


	if(dwTag == TAG_T_APPINFO)
	{
		//PrintHexDump(pData,dwLen);
		APPINFO_DEBUG(("Application Info: "));
		APPINFO_DEBUG(("AppType %02X ",pData[nOffset]));
		APPINFO_DEBUG(("Manu %02X%02X ",pData[nOffset+1],pData[nOffset+2]));
		APPINFO_DEBUG(("ManuCode %02X%02X ",pData[nOffset+3],pData[nOffset+4]));
		for(i=0;i<pData[nOffset+5];i++)
		{
			if(i<(MAX_CI_MODULENAME-2))
			{
				hCI->pszModuleName[i]=pData[nOffset+6+i];
				hCI->pszModuleName[i+1]=0;
				APPINFO_DEBUG(("%c",pData[nOffset+6+i]));
			} else {
				APPINFO_DEBUG(("Warning: AppInfo Module name too long\r\n"));
			}
		}
		hCI->bModuleNameLength=pData[nOffset+5];
		APPINFO_DEBUG(("\r\n"));

		// Activate the Conax Bug workaround
		//if((strstr(hCI->pszModuleName,"Conax")==hCI->pszModuleName) 
		//	|| (strstr(hCI->pszModuleName,"Access Control")==hCI->pszModuleName))
		// Change: Filtering is now enabled for all modules.
		{
			hCI->fModuleRequiresCAIDFiltering = TRUE;
			APPINFO_DEBUG(("CA-ID based PMT filtering enabled\r\n"));
		}

		{	
		BYTE *pMsg;
		WORD wMsgLen;		
		wMsgLen = (WORD)(4+hCI->bModuleNameLength);
		pMsg = (BYTE *)malloc( wMsgLen * (sizeof(BYTE))+10 );
		if( pMsg != NULL )
		{
			memset(pMsg, 0x00, wMsgLen);
			pMsg[0] = CI_MSG_CI_INFO;
			pMsg[1] = CI_SLOT_MODULE_NAME_OK;
			pMsg[2] = hCI->bSlotNum;
			
			for(i=0;i<hCI->bModuleNameLength;i++)
			{
				pMsg[3+i]=hCI->pszModuleName[i];	
				APPINFO_DEBUG(("%c",pMsg[3+i]));
			}	
			APPINFO_DEBUG(("\r\n"));
			pMsg[3+hCI->bModuleNameLength]='\0';
			SendCIMessage(hCI, pMsg, wMsgLen);
			free(pMsg);
		}
		}
	}
	else if(dwTag == TAG_T_REQUESTRESET)
	{
		PCMCIA_ResetModule(hCI->hSlotHandle);
	}
	
}

// -------------------------------------------------------------
void AppInfo_Close(CI_HANDLE hCI,WORD hSession)
{
	APPINFO_DEBUG(("AppInfo Close\r\n"));
}


BOOL AppInfo_Connect2(CI_HANDLE hCI,WORD hSession)
{
	APPINFO_DEBUG(("AppInfo_Connect2\r\n"));
	return AppInfo_Connect(hCI, hSession);	
}

void AppInfo_Data2(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen)
{
	APPINFO_DEBUG(("AppInfo_Data2\r\n"));
	AppInfo_Data(hCI, hSession, pData, dwLen);	
}

void AppInfo_Close2(CI_HANDLE hCI,WORD hSession)
{
	APPINFO_DEBUG(("AppInfo_Data2\r\n"));
	AppInfo_Close(hCI, hSession);
	return;
}




BOOL AppInfo_Connect3(CI_HANDLE hCI,WORD hSession)
{
	APPINFO_DEBUG(("AppInfo_Connect3\r\n"));
	return AppInfo_Connect2(hCI, hSession);	
}

void AppInfo_Data3(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen)
{
	APPINFO_DEBUG(("AppInfo_Data3\r\n"));
	AppInfo_Data2(hCI, hSession, pData, dwLen);	
}

void AppInfo_Close3(CI_HANDLE hCI,WORD hSession)
{
	APPINFO_DEBUG(("AppInfo_Data3\r\n"));
	AppInfo_Close2(hCI, hSession);
	return;
}

#endif // ENABLE_CI
