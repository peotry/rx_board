/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     BoardMngr.h
* Description:  Board Manager Include
* Author    :   jie.zhan
* Modified  :
* Reviewer  :
* Date      :   2017-03-28
* Record    :
*
**********************************************************************/

#ifndef _BOARDMNGR_H
#define _BOARDMNGR_H

#include "LicenseParse.h"
#include "appGlobal.h"
#include "DS2432.h"

#define REBOOT_WAIT_TIME_S (60)
#define DEFAULT_WAIT_TIME_S (5)

DS2432_PerBoard_t * BMN_GetDS2432_Handle();
LicenseInfo_t * BMN_GetLicenseInfo_Handle();
U32 BMN_GetBoardLicense(void);
U32 BMN_ParseBoardLicense(void);
wvErrCode BMN_Reboot(void);

#endif /*_BOARDMNGR_H*/


