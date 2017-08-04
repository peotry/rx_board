/*****************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName  timer.h
* Description : 定时器
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-04-24
* Record    :
*
*****************************************************************************/
#ifndef TIMER_H
#define TIMER_H

#include "appGlobal.h"

//U32 g_u32TimerCnt = 0;
U32 GetTimeCnt();
void Timer_Init(U32 u32uSec);
wvErrCode Timer_usleep(U32 u32uSec);
wvErrCode Timer_Start(U32 u32uSec);
void Timer_End(void);

#endif
