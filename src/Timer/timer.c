/*****************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName  timer.c
* Description : 定时器
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-04-24
* Record    :
*
*****************************************************************************/
#include <pthread.h>
#include <errno.h>
#include "timer.h"
#include "wv_err.h"
#include "WV_log.h"

U32 g_u32uSec = 0;
BOOL g_TimerEnd_flag = 1;
U32 g_u32TimerCnt = 0;

/*****************************************************************************
  Function:     GetTimeCnt
  Description:  
  Input:        
  Output:       g_u32TimerCnt
  Return:       
  Author:       lipeng
*****************************************************************************/
U32 GetTimeCnt()
{
    return g_u32TimerCnt;
}

/*****************************************************************************
  Function:     Timer_usleep
  Description:  微秒延时
  Input:        U32 u32uSec
  Output:       none
  Return:       WV_ERR_FAILURE         --  失败
                WV_SUCCESS             --  成功
  Author:       ruibin.zhang
*****************************************************************************/
wvErrCode Timer_usleep(U32 u32uSec)
{
    struct timeval tv;
    S32 s32Ret = 0;

    tv.tv_sec = u32uSec/1000000;
    tv.tv_usec = u32uSec%1000000;

    s32Ret = select(1,NULL,NULL,NULL,&tv);
    if(s32Ret < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Select error :%s(errno: %d)",strerror(errno),errno);
        return WV_ERR_FAILURE;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     Timer_Init
  Description:  定时器初始化
  Input:        U32 u32uSec
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void Timer_Init(U32 u32uSec)
{
    g_u32uSec = u32uSec;
}

/*****************************************************************************
  Function:     Timer_usleep
  Description:  微秒延时
  Input:        U32 u32uSec
  Output:       none
  Return:       WV_ERR_FAILURE         --  失败
                WV_SUCCESS             --  成功
  Author:       ruibin.zhang
*****************************************************************************/
void Timer_Service()
{
    while(1)
    {
        if(1 == g_TimerEnd_flag)
        {
            g_u32TimerCnt = 0;
            break;
        }

        Timer_usleep(g_u32uSec);

        g_u32TimerCnt ++;

        if(g_u32TimerCnt >= 0xfffffff0)
        {
            g_u32TimerCnt = 0;
        }
    }
}

/*****************************************************************************
  Function:     Timer_Start
  Description:  定时器启动
  Input:        none
  Output:       none
  Return:       WV_ERR_FAILURE         --  失败
                WV_SUCCESS             --  成功
  Author:       ruibin.zhang
*****************************************************************************/
wvErrCode Timer_Start(U32 u32uSec)
{
    int res = 0;
    pthread_t Timer_thread;

    //FIX 简单判断一下
    if(0 == g_TimerEnd_flag)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Timer is occupy");
        return WV_SUCCESS;
    }

    g_TimerEnd_flag = 0;
    Timer_Init(u32uSec);

    res = pthread_create(&Timer_thread, NULL, (void*)Timer_Service, NULL);
    if(-1 == res)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Timer thread create error :%s(errno: %d)",strerror(errno),errno);
        return WV_ERR_COMM_PTHREAD_CREAT;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "Timer create successfully!");
    }

    res = pthread_detach(Timer_thread);
    if(-1 == res)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Timer thread detach error :%s(errno: %d)",strerror(errno),errno);
        return WV_ERR_COMM_PTHREAD_CREAT;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "Timer thread detach successfully!");
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     Timer_End
  Description:  定时器停止
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void Timer_End(void)
{
    g_TimerEnd_flag = 1;
}
