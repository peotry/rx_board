#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "AppGlobal.h"

#include "web_main_process.h"
#include "web_data_process.h"

#include "base.h"

S8 * g_ps8RevData = NULL;
extern bool g_IsLicExpired;

typedef wvErrCode (*ProcessHandle)(buffer * pstWriteBuff );
typedef bool (*ProcessHandleSup)(void);

typedef struct
{
    char             * pcStr;
    ProcessHandle      pfProcess;
    ProcessHandleSup   pAuthorization;
}WEBInterface;


static void freeRevData();

static bool Web_passCheckLic()
{
    return true;
}

//在这个结构体数组中添加请求和处理函数的对应关系
WEBInterface arstWEBComm[] = {    
    //{  HTTP_POST_BEAT                        ,  NULL                                      ,  Web_passCheckLic            },
    {  HTTP_POST_READREG                     ,  web_processReadReg                        ,  Web_passCheckLic            },
    {  HTTP_POST_WRITEREG                    ,  web_processWriteReg                       ,  Web_passCheckLic            },
    {  HTTP_POST_GETSUBBOARDDETAIL           ,  web_processGetSubboardDetail              ,  NULL },
    {  HTTP_POST_GETTSDETAILSTATUS           ,  web_processGetTSDetailStatus              ,  NULL },
    {  HTTP_POST_CLEARALLERR                 ,  web_processClearAllErr                    ,  NULL },
    {  HTTP_POST_CLEARALLTS                  ,  web_processClearAllTS                     ,  NULL },
    {  HTTP_POST_GETSUBBOARDTSINFO           ,  web_processGetSubboardTSInfo              ,  NULL },
    {  HTTP_POST_GETOUTPUTBOARDINfO          ,  web_processGetOutputBoardInfo             ,  NULL },
    {  HTTP_POST_SETTSOUTPUT                 ,  web_processSetTsOutput                    ,  NULL },
    {  HTTP_POST_SETPROGRAMOUTPUT            ,  web_processSetProgramOutput               ,  NULL },
    {  HTTP_POST_GETSUBBOARDPARAM            ,  web_processGetSubboardParam               ,  NULL },
    {  HTTP_POST_SETSUBBOARDPARAM            ,  web_processSetSubboardParam               ,  NULL }, 
    {  HTTP_POST_DEBUG                       ,  web_processDebug                          ,  NULL },
    {  HTTP_POST_EXPORTLOGS                  ,  web_processExportLogs                     ,  Web_passCheckLic            },

    {  HTTP_POST_DEFAULT                     ,  web_processDefault                        ,  NULL },
    {  HTTP_POST_REBOOT                      ,  web_processReboot                         ,  NULL },
    {  HTTP_POST_GETINPUTTSSTATUS            ,  web_processGetInputTSStatus               ,  NULL },
    {  HTTP_POST_GETINPUTPROGRAMSTATUS       ,  web_processGetInputProgramStatus          ,  NULL },
    {  HTTP_POST_GETSUBBOARDCIDETAIL         ,  web_processGetSubBoardCIDetail            ,  NULL },
    {  NULL                                  ,  NULL                                      ,  NULL                        } //最后一个
};



/****************************************************************************
 *
 * 名字:
 *      web_ProcessData
 *
 * 目标:
 *      HTTP POST请求的总处理函数
 *
 * 用法：
 *      通过请求，调用对应的处理函数
 *
 * 返回值：       
 *      WV_SUCCESS             -处理成功
 *      WV_ERR_WEB_NOTEXIST    -未定义处理函数
 *      其它                   -处理失败      
 *
 * 作者:
 *      zhouhuan in 2014-6-21
 *
 ****************************************************************************/
wvErrCode web_ProcessData(buffer * pstWriteBuff,server *pstSrv, connection *pstWebConn)
{   
    wvErrCode     eRetCode    = WV_ERR_WEB_NOTEXIST;
    int           i           = 0; 
    char        * p           = NULL;
    int           nRevLen     = 0;
    chunk       * pTemp       = pstWebConn->request_content_queue->first;
    int           nHadRevlen  = WEB_POST_DATA_LEN;

    
    g_ps8RevData = (S8 *)malloc(WEB_POST_DATA_LEN);
    if (NULL == g_ps8RevData)
    {
       WEB_LOG("Malloc error!");
       return eRetCode;
    }
    
    //这里是POST请求带过来的数据   
    while ((pTemp != NULL) && (pTemp->mem != NULL) && (pTemp->mem->ptr != NULL))
    {        
        if ( (nRevLen + strlen(pTemp->mem->ptr)) >  nHadRevlen )
        {
            nHadRevlen += ((nRevLen + strlen(pTemp->mem->ptr) - nHadRevlen)/WEB_POST_DATA_LEN + 1)*WEB_POST_DATA_LEN;
            g_ps8RevData = (S8 *)realloc(g_ps8RevData , nHadRevlen);
            if (NULL == g_ps8RevData)
            {
                WEB_LOG("Realloc error!");
                return eRetCode;
            }
        }

        //将post过来的数据存储起来
        memcpy((char *)(g_ps8RevData + nRevLen),pTemp->mem->ptr, strlen(pTemp->mem->ptr));

        nRevLen += strlen(pTemp->mem->ptr);
        
        pTemp = pTemp->next;
    }

    //WEB_LOG("=======================================");
    //WEB_LOG("%s",g_ars8RevData);
    //WEB_LOG("=======================================");
    
    
    for (i = 0; NULL != arstWEBComm[i].pcStr; i++)
    {
        p = arstWEBComm[i].pcStr;

        //判断是否是当前请求
        if (NULL == strstr((const char *)pstWebConn->request.request->ptr, p))
        {
            continue;
        }
        
        if(NULL != arstWEBComm[i].pAuthorization && false == arstWEBComm[i].pAuthorization())
        {
            eRetCode = WV_ERR_LIC_EXPIRED;
            freeRevData();
            return web_processErr( pstWriteBuff , eRetCode, p );
        }
        
        if (NULL != arstWEBComm[i].pfProcess)
        {
           
            //调用函数
            eRetCode = arstWEBComm[i].pfProcess( pstWriteBuff );
            //错误处理
            if (WV_SUCCESS != eRetCode)
            {
                freeRevData();
                return web_processErr( pstWriteBuff , eRetCode, p );
            }
        }

        freeRevData();
        return eRetCode;
    }

    freeRevData();
    return eRetCode;
}

/****************************************************************************
 *
 * 名字:
 *      web_ProcessIsLargeData
 *
 * 目标:
 *      HTTP POST请求是否带大数据
 *
 * 用法：
 *     
 *
 * 返回值：       
 *      false             -数据未超过64k的请求
 *      true              -数据超过了64k的请求
 *          
 *
 * 作者:
 *      zhouhuan in 2014-8-22
 *
 ****************************************************************************/
bool web_ProcessIsLargeData(connection *pstWebConn)
{
    char * pcStr = NULL;
    
    if (NULL == pstWebConn)
    {
        return false;
    }
    
    pcStr = pstWebConn->request.request->ptr;
    
    /*
    if ( (NULL != strstr((const char *)pcStr, HTTP_POST_UPGRADE))
        || (NULL != strstr((const char *)pcStr, HTTP_POST_IMPORTCONFIG))
        )
    {
        return true;
    }*/

    return false;
}

/****************************************************************************
 *
 * 名字:
 *      freeRevData
 *
 * 目标:
 *      释放获取的HTTP数据的内存
 *
 * 用法：
 *     
 *
 * 返回值： -
 *          
 *
 * 作者:
 *      zhouhuan in 2015-5-14
 *
 ****************************************************************************/
static void freeRevData()
{
    if (NULL != g_ps8RevData)
    {
        free(g_ps8RevData);
    }
}

