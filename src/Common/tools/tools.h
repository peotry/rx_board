/******************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName tools.h
* Description : system tools
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-03-03
* Record    :
*
******************************************************************************/
#ifndef TOOLS_H
#define TOOLS_H

#include "appGlobal.h"

#define TOOLS_SECCESS 0
#define TOOLS_FAIL    -1

U32 Common_CRCCalculate(U8* data, U32 length, U32 CRCValue);
S32 Common_GetSystemAnalysis();
void Common_ChangeBit8To32(U32 *pu32Data, const U8 *pu8Data, U8 u8Length);

#endif
