/*
 * Web_main_process.h
 *
 *  Created on: 2014-6-21
 *      Author: zhouhuan
 */

#ifndef WEB_MAIN_PROCESS_H_
#define WEB_MAIN_PROCESS_H_

#include <stdbool.h>
#include "base.h"
#include "wv_err.h"

#define WEB_MAX_DATA_LEN     (8*1024)
#define WEB_POST_DATA_LEN    (2*1024)  

/********************************************************************************************
                                    请求列表开始
*********************************************************************************************/

#define  HTTP_POST_GETSUBBOARDDETAIL                  "POST /ajax/GetSubboardDetail.w"
#define  HTTP_POST_GETSUBBOARDTSINFO                  "POST /ajax/GetSubboardTSInfo.w"
#define  HTTP_POST_GETOUTPUTBOARDINfO                 "POST /ajax/GetOutputBoardInfo.w"
#define  HTTP_POST_GETTSDETAILSTATUS                  "POST /ajax/GetTSDetailStatus.w"
#define  HTTP_POST_CLEARALLERR                        "POST /ajax/ClearAllErr.w"
#define  HTTP_POST_CLEARALLTS                         "POST /ajax/ClearAllTS.w"

#define  HTTP_POST_SETTSOUTPUT                        "POST /ajax/SetTsOutput.w"
#define  HTTP_POST_SETPROGRAMOUTPUT                   "POST /ajax/SetProgramOutput.w"
#define  HTTP_POST_GETSUBBOARDPARAM                   "POST /ajax/GetSubboardParam.w"
#define  HTTP_POST_SETSUBBOARDPARAM                   "POST /ajax/SetSubboardParam.w"

#define  HTTP_POST_DEFAULT                            "POST /ajax/Default.w"
#define  HTTP_POST_REBOOT                             "POST /ajax/Reboot.w"

#define  HTTP_POST_EXPORTLOGS                         "POST /ajax/ExportLogs.w"

#define  HTTP_POST_GETINPUTTSSTATUS                   "POST /ajax/GetInputTSStatus.w"
#define  HTTP_POST_GETINPUTPROGRAMSTATUS              "POST /ajax/GetInputProgramStatus.w"
#define  HTTP_POST_GETSUBBOARDCIDETAIL                "POST /ajax/GetSubBoardCIDetail.w"

/****************************************调试接口********************************************/

#define  HTTP_POST_READREG                            "POST /ajax/ReadReg.w"
#define  HTTP_POST_WRITEREG                           "POST /ajax/WriteReg.w"
#define  HTTP_POST_DEBUG                              "POST /ajax/Debug.w"

/****************************************调试接口********************************************/

/********************************************************************************************
                                    请求列表结束
*********************************************************************************************/

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
wvErrCode web_ProcessData(buffer * pstWriteBuff,server *pstSrv, connection *pstWebConn);

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
bool web_ProcessIsLargeData(connection *pstWebConn);

#endif /* WEB_MAIN_PROCESS_H_ */
