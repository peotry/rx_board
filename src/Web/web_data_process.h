/*
 * web_data_process.h
 *
 *  Created on: 2014-6-21
 *      Author: zhouhuan
 */

#ifndef WEB_DATA_PROCESS_H_
#define WEB_DATA_PROCESS_H_

#include "AppGlobal.h"
#include "base.h"
#include "parameters.h"

#define   MAX_PARAMS_LENGTH       (16) 
#define   UTF8_STRING_MAX_LENGTH  (256)

/****************************************************************************
 *
 * 名字:
 *      web_processErr
 *
 * 目标:
 *      HTTP POST请求失败时的数据组装
 *
 * 用法：    
 *
 * 返回值：       
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败      
 *
 * 作者:
 *      zhouhuan in 2014-7-8
 *
 ****************************************************************************/
wvErrCode web_processErr(buffer * pstWriteBuff , wvErrCode eErrCode , char * pRequest);

/****************************************************************************
 *
 * 名字:
 *      web_processSuccess
 *
 * 目标:
 *      HTTP POST请求成功但不需要带数据返回时的数据组装
 *
 * 用法：    
 *
 * 返回值：       
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败      
 *
 * 作者:
 *      zhouhuan in 2014-7-8
 *
 ****************************************************************************/
wvErrCode web_processSuccess(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processReadReg
 *
 * 目标:
 *      读取寄存器的值
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      zhouhuan in 2017-3-2
 *
 ****************************************************************************/
wvErrCode web_processReadReg(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processWriteReg
 *
 * 目标:
 *      设置寄存器的值
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      zhouhuan in 2017-3-2
 *
 ****************************************************************************/
wvErrCode web_processWriteReg(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processGetSubboardDetail
 *
 * 目标:
 *      读取子板详情
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      xiazhi.zhang in 2017-4-13
 *
 ****************************************************************************/
wvErrCode web_processGetSubboardDetail(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processGetSubboardTSInfo
 *
 * 目标:
 *      获取子板流信息
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-6
 *
 ****************************************************************************/
wvErrCode web_processGetSubboardTSInfo(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processGetOutputBoardInfo
 *
 * 目标:
 *      获取输出板信息
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-6
 *
 ****************************************************************************/
wvErrCode web_processGetOutputBoardInfo(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processSetTsOutput
 *
 * 目标:
 *      设置流去向
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-6
 *
 ****************************************************************************/
wvErrCode web_processSetTsOutput(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processGetTSDetailStatus
 *
 * 目标:
 *      获取子板的流的详细信息
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      huan.zhou in 2017-4-21
 *
 ****************************************************************************/
wvErrCode web_processGetTSDetailStatus(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processClearAllErr
 *
 * 目标:
 *      清除流的错误计数
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      huan.zhou in 2017-4-21
 *
 ****************************************************************************/
wvErrCode web_processClearAllErr(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processClearAllTS
 *
 * 目标:
 *      清除所有配置
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      huan.zhou in 2017-4-20
 *
 ****************************************************************************/
wvErrCode web_processClearAllTS(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processSetProgramOutput
 *
 * 目标:
 *      设置节目去向
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-6
 *
 ****************************************************************************/
wvErrCode web_processSetProgramOutput(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processGetSubboardParam
 *
 * 目标:
 *      获取子板参数
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-8
 *
 ****************************************************************************/
wvErrCode web_processGetSubboardParam(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processSetSubboardParam
 *
 * 目标:
 *      设置子板参数
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-8
 *
 ****************************************************************************/
wvErrCode web_processSetSubboardParam(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processDefault
 *
 * 目标:
 *      恢复出厂设置
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-7
 *
 ****************************************************************************/
wvErrCode web_processDefault(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processReboot
 *
 * 目标:
 *      重启
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-7
 *
 ****************************************************************************/
wvErrCode web_processReboot(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processExportLogs
 *
 * 目标:
 *      导出日志
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-22
 *
 ****************************************************************************/
wvErrCode web_processExportLogs(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processDebug
 *
 * 目标:
 *      调试接口
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      huan.zhou 2017-3-23
 *
 ****************************************************************************/
wvErrCode web_processDebug(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processGetInputTSStatus
 *
 * 目标:
 *      获取输入流状态
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      xiazhi.zhang in 2017-3-21
 *
 ****************************************************************************/
wvErrCode web_processGetInputTSStatus(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processGetInputProgramStatus
 *
 * 目标:
 *      获取节目信息
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      xiazhi.zhang in 2017-3-21
 *
 ****************************************************************************/
wvErrCode web_processGetInputProgramStatus(buffer * pstWriteBuff);

/****************************************************************************
 *
 * 名字:
 *      web_processGetSubBoardCIDetail
 *
 * 目标:
 *      获取子板CI信息
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      xiazhi.zhang in 2017-5-11
 *
 ****************************************************************************/
wvErrCode web_processGetSubBoardCIDetail(buffer * pstWriteBuff);

#endif /* WEB_DATA_PROCESS_H_ */

