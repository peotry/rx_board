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
#ifdef ENABLE_CI_HOSTCONTROL

//#define HOSTCONTROL_DEBUG_ENABLE

#ifdef HOSTCONTROL_DEBUG_ENABLE 
#define HOSTCONTROL_DEBUG(x) printf x
FAILRELEASEBUILD
#else
#define HOSTCONTROL_DEBUG(x)
#endif

static BOOL HostControl_Connect(CI_HANDLE hCI, WORD hSession);
static void HostControl_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen);
static void HostControl_Close(CI_HANDLE hCI, WORD hSession);

//! current session handle for the host control resource (0 if none)
static DWORD g_dwHostControlSession = 0;
//! slot that has opened the host control session
static CI_HANDLE g_dwActiveHostControlSlot;
static const BYTE HostControlAskRelease[4]={0x9f,0x84,0x03,0x00};
static BOOL g_fHostControlActive;

const RESOURCE_INFO HostControl = { RESOURCEID_HOSTCONTROL,HostControl_Connect,HostControl_Data,HostControl_Close};


BOOL HostControl_Connect(CI_HANDLE hCI, WORD hSession)
{
	HOSTCONTROL_DEBUG(("HostControl_Connect\r\n"));

	// only allow one open session
	if(g_dwHostControlSession)
	{
		HOSTCONTROL_DEBUG(("HostControl_Connect resource busy\r\n"));
		return FALSE;
	}

	g_dwHostControlSession = hSession;
	g_dwActiveHostControlSlot = hCI;
	g_fHostControlActive = FALSE;
	return TRUE;
}

void HostControl_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen)
{
	DWORD dwTag;
	int nOffset = 3;
	WORD wLen;
	WORD wNID;
	WORD wONID;
	WORD wTSID;
	WORD wSID;
	BYTE bReplacementRef;
	WORD wReplacedPid;
	WORD wReplacementPid;
	nOffset += CI_DecodeLengthField(&pData[nOffset],&wLen);

	dwTag = pData[0]<<16|pData[1]<<8|pData[2];
	HOSTCONTROL_DEBUG(("HostControl_Data %06X\r\n",dwTag));
	switch(dwTag)
	{
	case TAG_T_TUNE:
		wNID  = (WORD)((pData[nOffset+0]<<8)|pData[nOffset+1]);
		wONID = (WORD)((pData[nOffset+2]<<8)|pData[nOffset+3]);
		wTSID = (WORD)((pData[nOffset+4]<<8)|pData[nOffset+5]);
		wSID  = (WORD)((pData[nOffset+6]<<8)|pData[nOffset+7]);
		g_fHostControlActive = TRUE;
		CIHost_HC_Tune(wNID, wONID, wTSID, wSID);		
		break;
	case TAG_T_REPLACE:
		bReplacementRef = (pData[nOffset+0]);
		wReplacedPid	= (WORD)((pData[nOffset+1]<<8)|pData[nOffset+2]);
		wReplacementPid	= (WORD)((pData[nOffset+3]<<8)|pData[nOffset+4]);
		g_fHostControlActive = TRUE;
		CIHost_HC_Replace(bReplacementRef, wReplacedPid, wReplacementPid);
		break;
	case TAG_T_CLEARREPLACE:
		bReplacementRef = (pData[nOffset]);
		g_fHostControlActive = TRUE;
		CIHost_HC_ClearReplace(bReplacementRef);
		break;
	}
	return;
}

void HostControl_Close(CI_HANDLE hCI, WORD hSession)
{
	HOSTCONTROL_DEBUG(("HostControl_Close\r\n"));
	g_dwHostControlSession = 0;
	g_dwActiveHostControlSlot = NULL;
	if(g_fHostControlActive)
		CIHost_HC_Closed();
	g_fHostControlActive = FALSE;
	return;
}

void CI_HostControl_Ask4HostRelease(void)
{
	HOSTCONTROL_DEBUG(("CI_HostControl_Ask4HostRelease\r\n"));
	 
	if(g_dwHostControlSession == 0)
		return;

	Send_SPDU_Data(g_dwActiveHostControlSlot,g_dwActiveHostControlSlot->CiSessions[g_dwHostControlSession].bTransportConnID,g_dwActiveHostControlSlot->CiSessions[g_dwHostControlSession].wSessionID,HostControlAskRelease,sizeof(HostControlAskRelease));
}

#endif // ENABLE_CI_HOSTCONTROL
#endif // ENABLE_CI
