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




#define DTG_COPYPROTECTION_ID (0x00D07F)

BOOL fCopyProtectionActive = FALSE;

CI_CP_STATUS CIHost_GetCopyProtectionStatus(DWORD *pdwCPId)
{
	if(*pdwCPId != DTG_COPYPROTECTION_ID)
	{
		*pdwCPId = DTG_COPYPROTECTION_ID;
		return CI_CP_STATUS_ID_MISMATCH;
	}

	*pdwCPId = DTG_COPYPROTECTION_ID;
	if(fCopyProtectionActive)
		return CI_CP_STATUS_ACTIVE;
	else
		return CI_CP_STATUS_INACTIVE;
}

BOOL CIHost_CopyProtectionCommand(DWORD dwCPId, const BYTE *pData, WORD wDataLen, BYTE *pOut, WORD *pwOutLen)
{
	BOOL ret;
	if(dwCPId != DTG_COPYPROTECTION_ID)
		return FALSE;

	if((wDataLen != 1) || (pData[0]>1))
	{
		pOut[0] = 0;
		*pwOutLen = 1;
	}

	if(pData[0] == 0)
		ret = CIHost_ActivateCopyProtection();
	else
		ret = CIHost_DeactivateCopyProtection();

	if(ret==TRUE)
		pOut[0] = 1; // command understood and implemented
	else
		pOut[0] = 0; // error
	return TRUE;
}



#endif // ENABLE_CI_COPYPROTECTION
#endif // ENABLE_CI
