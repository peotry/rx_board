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

////#define DATE_DEBUG_ENABLE

#ifdef DATE_DEBUG_ENABLE
#define DATE_DEBUG(x) printf x
////FAILRELEASEBUILD
#else
#define DATE_DEBUG(x)
#endif

#ifdef ENABLE_CI

static BOOL DateTime_Connect(CI_HANDLE hCI, WORD hSession);
static void DateTime_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen);
static void DateTime_Close(CI_HANDLE hCI, WORD hSession);

const RESOURCE_INFO DateTime =		{ RESOURCEID_DATETIME,DateTime_Connect,DateTime_Data,DateTime_Close };

//! DateTime response packet. This variable also serves as temporary storage for
//! the last set UTC time from the CIHost. (TAG_T_DATETIME)
static BYTE DateResp[9]={0x9f,0x84,0x41,5,0x00,0x00,0x00,0x00,0x00}; 

///////////////////////////////////////////////////////////////////////////////////////////
// Date/Time

BOOL DateTime_Connect(CI_HANDLE hCI, WORD hSession)
{
	DATE_DEBUG(("DateTime Connect\r\n"));
	hCI->dwDateTimeSession = hSession;
	hCI->dwDateTimeIntervallTicks = 0;
	return TRUE;
}

// -------------------------------------------------------------
void DateTime_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen)
{
	DWORD dwTag;
	int nOffset = 3;
	WORD wLen;
	nOffset += CI_DecodeLengthField(&pData[nOffset],&wLen);

	dwTag = pData[0]<<16|pData[1]<<8|pData[2];
	DATE_DEBUG(("DateTime Data %06X\r\n",(int)dwTag));

	if(dwTag == TAG_T_DATETIMEENQ)
	{
		hCI->dwDateTimeIntervallTicks = MSTOTICKS(pData[nOffset]*1000);
		DATE_DEBUG(("Date-Time Enq Interval:%d seconds = %d ticks\r\n", pData[nOffset], hCI->dwDateTimeIntervallTicks));
		// send the first packet immediately if we have a time
		if(DateResp[4] || DateResp[5] || DateResp[6] || DateResp[7] || DateResp[8])
		{
			hCI->dwLastDateTimeSendTicks = GURU_GetTickCount();
			Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,DateResp,sizeof(DateResp));
		}
	}
}

// -------------------------------------------------------------
void DateTime_SendDateTime(const BYTE *pDateTime)
{
	CI_HANDLE hCI;
	DWORD i;

	// copy the TDT/TOT UTC_time field into the message buffer
	memcpy(&DateResp[4],pDateTime,5);

	// and send the time to all slots
	for(i=0;i<g_dwNumPCMCIASlots;i++)
	{
		hCI = &CiSlot[i];
	
#ifdef DATE_DEBUG_ENABLE
		{
			short i;
			DATE_DEBUG(("DateTime SendDateTime: "));
			for (i=4;i<9;i++)
				DATE_DEBUG(("0x%X ",DateResp[i]));
			DATE_DEBUG(("\r\n"));	
		}	
#endif

		// if there is a datetimesession and the update timer expired
		if (hCI->dwDateTimeSession && hCI->dwDateTimeIntervallTicks 
				&& (GURU_GetTickCount() > hCI->dwLastDateTimeSendTicks+hCI->dwDateTimeIntervallTicks))
		{
			DATE_DEBUG(("DateTime SendDateTime: Intervall:%d ticks [%s]\r\n",hCI->dwDateTimeIntervallTicks,hCI->pszModuleName));
			Send_SPDU_Data(hCI,hCI->CiSessions[hCI->dwDateTimeSession].bTransportConnID,hCI->CiSessions[hCI->dwDateTimeSession].wSessionID,DateResp,sizeof(DateResp));
			hCI->dwLastDateTimeSendTicks = GURU_GetTickCount();
		}
	}
}	

// -------------------------------------------------------------
void DateTime_Close(CI_HANDLE hCI, WORD hSession)
{
	DATE_DEBUG(("DateTime Close\r\n"));
}

#endif // ENABLE_CI
