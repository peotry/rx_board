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
#ifndef INCLUDE_TOOLS_TIMER_H
#define INCLUDE_TOOLS_TIMER_H 

#include <sys/timerfd.h>
#include <stdbool.h>

#include "appGlobal.h"

typedef void *  (*timer_func_cb)(void *arg);


//U32 g_u32TimerCnt = 0;
U32 GetTimeCnt();
void Timer_Init(U32 u32uSec);
wvErrCode Timer_usleep(U32 u32uSec);
wvErrCode Timer_Start(U32 u32uSec);
void Timer_End(void);

enum TaskStatus
{
    emRunning,
    emStop,
    emStart
};

struct TimerTask
{
    int tm_fd;            //定时器对象
    enum TaskStatus emTaskStatus;  //任务状态
    timer_func_cb func;   //定时执行函数
    void *arg;            //回调函数的数据
};


void Timer_MonitorServer(void);

void *Timer_MonitorTask(void *arg);

void Timer_RegisterTask(const struct itimerspec itimer, struct TimerTask *pstTimerTask);
void Timer_RemoveTask(const struct TimerTask *pstTimerTask);


#endif
