#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "web.h"
#include "WV_log.h"
#include "server.h"

/*****************************************************************************
  Function:     web_GetHttpRequestCount
  Description:  查看未处理的HTTP请求，从而判断WEB 服务器是否已经挂起
  Input:        none
  Output:       none
  Return:       Value            -  未处理的WEB请求
  Author:       dadi.zeng
*****************************************************************************/
int  web_GetHttpRequestCount(void)
{
    int ret = 0;
    char u8Line[200]    = { 0 };
    char u8TempLine1[200]    = { 0 };
    int port = 0;
    int requestcnt = 0;

    // Vars
    FILE * fp = NULL;

    // 打开文件
    fp = popen("netstat -nl | grep 80", "r");
    if( NULL == fp )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_WEB, "Can't open command [netstat -nl | grep 80] file!\n");  
        goto daemon_exit;
    }

    // 格式化输入  eg. tcp        0      0 0.0.0.0:80              0.0.0.0:*               LISTEN   
    while(!feof(fp))
    {
        memset((char*)u8Line, 0, ARRAY_SIZE(u8Line));
        fgets((char *)u8Line, sizeof(u8Line), fp);

        sscanf((char*)u8Line, "tcp        %d      0 0.0.0.0:%d  %s", &requestcnt, &port, u8TempLine1);
    }

    if(80 == port)
    {
        ret = requestcnt;
    }

    // 关闭文件
    pclose(fp);

daemon_exit:
    return ret;
}

/*****************************************************************************
  Function:     web_MonitorServer
  Description:  WEB服务器后台监控函数，用于查看服务器是否还正常工作。
                                                如果未正常工作，则关闭服务器，释放端口，并重新启动
  Input:        none
  Output:       none
  Return:       WEB_SUCCESS          : server 正常
                WEB_RESTART_SUCCESS  : server 重新启动成功
                WEB_RESTART_FAILURE  : server 重新启动失败
  Author:       dadi.zeng
*****************************************************************************/
wvWebErrCode web_MonitorServer(void)
{
    wvWebErrCode nRet = WEB_SUCCESS;
    int firstcnt = 0;
    int secondcnt = 0;
    bool isRestart = false;
    U32  u32Timeout = 0;
    U32  u32MaxTimeOut = 5;

    /* 查看是否还有请求未处理 */
    firstcnt = web_GetHttpRequestCount();
    //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_WEB, "Get no-handled requeset first count = %d!\n", firstcnt);

    /* 如果请求未处理，则延迟一秒再查看是否还有未处理的请求 */
    if(0 != firstcnt)
    {
        usleep(2000000);
        secondcnt = web_GetHttpRequestCount();

        //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_WEB, "Get no-handled requeset second count = %d!\n",secondcnt);

        if(0 != secondcnt)
        {
        	isRestart = true;
        }
    }

    /* 如果两次都检测有请求未处理，则重启WEB_SERVER */
    if(true == isRestart)
    {
        //将服务器重启时间，记录到日志
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_WEB, "Restart web server!\n");
    
        //关闭WEB服务器线程
        lighttpd_shutdown(true);
        do
        {
            sleep(3);    
            u32Timeout++;
            if(true == lighttpd_isServerClose())
            {
                 /* 关闭线程成功，则重启线程 */
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_WEB, "Close server successfully!\n");
                break;
            }

            if(u32Timeout > u32MaxTimeOut)
            {
                /* 关闭线程失败，返回 */
                LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_WEB, "Close server Timeout!\n");
                nRet = WEB_RESTART_FAILURE;
                goto  web_exit;
            }   
            
        }while(1);

        //重置服务器运行标志位
        lighttpd_shutdown(false);
    
        //重启WEB服务
        web_Start();

        nRet = WEB_RESTART_SUCCESS;
    }

web_exit: 
    return nRet;
}

/*****************************************************************************
  Function:     web_Start
  Description:  启动web-server
  Input:        none
  Output:       none
  Return:       WV_ERR_FAILURE         --  失败
                WV_SUCCESS             --  成功
  Author:       dadi.zeng
*****************************************************************************/
wvWebErrCode web_Start(void)
{
    int res = 0;
    wvWebErrCode  enRet = WEB_SUCCESS;
    pthread_attr_t thread_attr;
    pthread_t web_thread;

	printf("######## web start\n");
    
    res = pthread_attr_init(&thread_attr);
    if(0 != res)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_WEB, "Thread attribute creation failed\n");
        return WEB_FAILURE;
    }

    res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if(0 != res)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_WEB, "Thread attribute setting failed\n");
        return WEB_FAILURE;
    }

    res = pthread_create(&web_thread, &thread_attr, (void*)lighttpd_configuration, NULL );
    if(-1 == res)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_WEB, "Web thread create failed!\r\n");
        enRet |= WEB_FAILURE;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_WEB, "Web thread create successfully!\r\n");
    }

    return enRet;
}



