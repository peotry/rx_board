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

#include "event/event.h"
#include "thread/thread.h"

#include <errno.h>
#include <poll.h>
#include <string.h>
#include <time.h>
#include <stdint.h>


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


#define ERR_BUF_LEN           (80)
#define MAX_TASK              (100)
#define OPEN_MAX_FD           (500)
#define NANO_OF_ONE_SECOND    (1000000000)

static int s_EventFd = 0;
static struct TimerTask s_stTimerTask[MAX_TASK];


/*
* function: Timer_InitTaskList
*
* description: 初始化任务列表
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
void Timer_InitTaskList(void)
{
    int i = 0;

    for(i = 0; i < MAX_TASK; ++i)
    {
        s_stTimerTask[i].tm_fd = -1;
        s_stTimerTask[i].emTaskStatus = emStop;
    }
}


/*
* function: Timer_CreateEvent
*
* description: 创建事件通知句柄
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
static void Timer_CreateEvent(void)
{
    s_EventFd = Event_CreateFd();
}


/*
* function: Timer_Notify
*
* description: 事件通知
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
static void Timer_Notify(void)
{
    if(Event_Write(s_EventFd) == EVENT_ERR)
    {
		printf("Error: Timer_Notify...");
    }
}


/*
* function: Timer_RegisterTask
*
* description: 注册定时执行任务
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
void Timer_RegisterTask(const struct itimerspec itimer, struct TimerTask *pstTimerTask)
{
    if(!pstTimerTask)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: params = NULL");
        return ;
    }

    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "Timer_RegisterTask ...");

    int i = 0;
    int tm_fd = 0;
    char err_buf[ERR_BUF_LEN] = {0};
    
    //生成一个定时器对象
    if(-1 == (tm_fd = timerfd_create(CLOCK_MONOTONIC, 0)))
    {
        strerror_r(errno, err_buf, sizeof(err_buf));
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "error: call timerfd_create: %s", err_buf);
        return ;
    }


#if 0
    //获取当前时间
    if(-1 == clock_gettime(CLOCK_REALTIME, &now))
    {
        strerror_r(errno, err_buf, sizeof(err_buf));
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TOOLS, "error: call tclock_gettime: %s", err_buf);
        return ;
    }

    //第一次触发定时器是绝对时间
    nanosecond = now.tv_nsec + itimer.it_value.tv_nsec;
    if(nanosecond >= NANO_OF_ONE_SECOND)
    {
        new_itimer.it_value.tv_nsec = nanosecond - NANO_OF_ONE_SECOND;
        new_itimer.it_value.tv_sec = itimer.it_value.tv_nsec + now.tv_sec  + 1;
    }
    else
    {
        new_itimer.it_value.tv_nsec = nanosecond;
        new_itimer.it_value.tv_sec = itimer.it_value.tv_sec + now.tv_sec;
    }
#endif

    if( -1 == timerfd_settime(tm_fd, 0, &itimer, NULL))
    {
        strerror_r(errno, err_buf, sizeof(err_buf));
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call timerfd_settime: %s", err_buf);
        return ;
    }

    //将任务推入任务列表
    for(i = 0; i < MAX_TASK; ++i)
    {
        if(-1 == s_stTimerTask[i].tm_fd)
        {
            pstTimerTask->tm_fd = tm_fd;
            pstTimerTask->emTaskStatus = emStart;
            s_stTimerTask[i].tm_fd = tm_fd;
            s_stTimerTask[i].emTaskStatus = emStart;
            s_stTimerTask[i].func = pstTimerTask->func;
            s_stTimerTask[i].arg = pstTimerTask->arg;
            break;
        }
    }

    //任务列表已经满
    if(i == MAX_TASK)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: task list is full！");
        close(tm_fd);
    }
    else
    {
        //通知事件监听线程，监听新的对象
        Timer_Notify();
    }
   
}


/*
* function: Timer_ExecuteTask
*
* description: 执行注册的定时任务
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
static void Timer_ExecuteTask(int tm_fd)
{
    int i = 0;

    for(i = 0; i < MAX_TASK; ++i)
    {
        if(tm_fd == s_stTimerTask[i].tm_fd)
        {
            THREAD_NEW_DETACH(s_stTimerTask[i].func, s_stTimerTask[i].arg, "Timer_ExecuteTask");
            break;
        }
    }

}


/*
* function: Timer_HandleTask
*
* description: 处理注册的定时任务
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
static void Timer_HandleTask(struct pollfd * timerfds, int maxfd)
{
   int i = 0; 
   int size = 0;
   uint64_t exp = 0;

   for(i = 1; i <= maxfd; ++i)
   {
        if(timerfds[i].revents & POLLIN)
        {
            size = read(timerfds[i].fd, &exp, sizeof(exp));
            if(size != sizeof(exp))
            {
                LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DRIVERS, "Error: read faild!");
                continue;
            }

            Timer_ExecuteTask(timerfds[i].fd);
        }
   }
}


void Timer_MonitorServer(void)
{
	THREAD_NEW_DETACH(Timer_MonitorTask, NULL, "Timer_MonitorTask");
}

/*
* function: Timer_MonitorTask
*
* description: 监测所有定时任务的IO，定时执行注册的任务
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
void *Timer_MonitorTask(void *arg)
{
    int i = 0;
    int j = 0;
    int maxi = 0;
    int nready = 0;
    uint64_t num = 0;
    struct pollfd timerfds[OPEN_MAX_FD];

    Timer_CreateEvent();
	Timer_InitTaskList();

    //添加监听描述符
    timerfds[0].fd = s_EventFd;
    timerfds[0].events = POLLIN;

    //初始化事件监听
    for(i = 1; i < OPEN_MAX_FD; ++i)
    {
        timerfds[i].fd = -1;
    }
    
    maxi = 0;

    while(1)
    {
        nready = poll(timerfds, maxi + 1, -1);
        if((-1 == nready) && (EINTR == errno))
        {
            continue;
        }

        //执行定时任务
        Timer_HandleTask(timerfds, maxi);

        //有新任务注册或撤销任务
        if(timerfds[0].revents & POLLIN)
        {
             read(timerfds[0].fd, &num, sizeof(num));
             //初始化事件监听
             for(i = 1; i < OPEN_MAX_FD; ++i)
             {
                 timerfds[i].fd = -1;
             }
             maxi = 0;

             //更新监听任务列表
             for(i = 1, j = 0; j < MAX_TASK; ++j)
             {
                 if(-1 == s_stTimerTask[j].tm_fd)
                 {
                     continue;
                 }
                 timerfds[i].fd = s_stTimerTask[j].tm_fd;
                 timerfds[i].events = POLLIN;
                 i++;
             }

             maxi = i - 1;
             printf("maxi = %d\n", maxi);
        }
    }

}



/*
* function: Timer_RemoveTask
*
* description: 注销任务
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
void Timer_RemoveTask(const struct TimerTask *pstTimerTask)
{
    if(!pstTimerTask)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DRIVERS, "Error: param = NULL");
        return;
    }

    int i = 0;
    
    for(i = 0; i < MAX_TASK; ++i)
    {
        if(pstTimerTask->tm_fd == s_stTimerTask[i].tm_fd)
        {
            s_stTimerTask[i].emTaskStatus = emStop;
            close(s_stTimerTask[i].tm_fd);
            s_stTimerTask[i].tm_fd = -1;

            //通知监听线程，取消监听
            Timer_Notify();
            break;
        }
    }

    if(i == MAX_TASK)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: can't find task!!");
    }

}



