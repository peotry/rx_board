/*****************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName WV_log.c
* Description : web module 与web交互的接口文件
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-28
* Record    :
*
*****************************************************************************/
#include "web_function.h"
#include "FPGA/FPGA.h"
#include "Tuner.h"
#include "resource.h"
#include "status.h"
#include "REG.h"
#include "BoardMngr.h"
#include "parameters.h"

/*****************************************************************************
  Function:     web_CheckUser
  Description:  检测web登陆用户是否存在于配置文件中
  Input:        const U8 *  pu8UserName        (i)   用户名
                const U8 *  pu8PassWord        (i)   登陆密码
  Output:       none
  Return:       WV_ERR_USER_OPEN_FILE_ERROR        --  打开文件错误
                WV_ERR_USER_LEN_ERROR              --  用户名密码长度错误
                WV_ERR_USER_PWD_ERR                --  用户密码错误
                WV_ERR_USER_NOT_EXIST              --  用户不存在
                WV_SUCCESS                         --  成功
  Author:       dadi.zeng
*****************************************************************************/
wvErrCode web_CheckUser(const U8 * pu8UserName , const U8 * pu8PassWord)
{
    return user_CheckUser(pu8UserName, pu8PassWord);
}

/*****************************************************************************
  Function:     web_RegistUser
  Description:  注册用户
  Input:        const U8 *  pu8UserName        (i)   用户名
                const U8 *  pu8PassWord        (i)   登陆密码
  Output:       none
  Return:       WV_ERR_USER_OPEN_FILE_ERROR        --  打开文件错误
                WV_ERR_USER_LEN_ERROR              --  用户名密码长度错误
                WV_ERR_USER_FULL                   --  用户量达到上限
                WV_ERR_USER_EXIST                  --  添加的用户已经存在
                WV_SUCCESS                         --  成功
  Author:       yaoliang.sun
*****************************************************************************/
wvErrCode web_RegistUser(const U8 * pu8UserName, const U8 * pu8PassWord)
{
    return user_AddUser(pu8UserName, pu8PassWord);
}

/*****************************************************************************
  Function:     web_ModifyPasswd
  Description:  修改密码
  Input:        const U8 *  pu8UserName        (i)   用户名
                const U8 *  pu8PassWord        (i)   登陆密码
  Output:       none
  Return:       WV_ERR_USER_OPEN_FILE_ERROR        --  打开文件错误
                WV_ERR_USER_LEN_ERROR              --  用户名密码长度错误
                WV_ERR_USER_NOT_EXIST              --  用户不存在
                WV_SUCCESS                         --  成功
  Author:       yaoliang.sun
*****************************************************************************/
wvErrCode web_ModifyPasswd(const U8 * pu8UserName, const U8 * pu8PassWord)
{
    return user_ModifyUser(pu8UserName, pu8PassWord);
}

/*****************************************************************************
  Function:     web_DelUser
  Description:  删除用户
  Input:        const U8 * pu8UserName    (i)  用户名
  Output:       none
  Return:       WV_ERR_USER_OPEN_FILE_ERROR        --  打开文件错误
                WV_ERR_USER_NOT_EXIST              --  用户不存在
                WV_SUCCESS                         --  成功
  Author:       yaoliang.sun
*****************************************************************************/
wvErrCode web_DelUser(const U8 * pu8UserName)
{
    return user_DelUser(pu8UserName);
}

/*****************************************************************************
  Function:     web_ShowUser
  Description:  查找用户
  Input:        ST_USER_INFO* pStUserArry    (o)  用户名
                U32 u32MaxNum                (i)  获取用户的上限
                U32 pU32UserNum              (o)  实际用户的个数
  Output:       none
  Return:       WV_ERR_USER_OPEN_FILE_ERROR        --  打开文件错误
                WV_ERR_FAILURE                     --  参数错误
                WV_SUCCESS                         --  成功
  Author:       yaoliang.sun
*****************************************************************************/
wvErrCode web_ShowUser(ST_USER_INFO* pStUserArry, U32 u32MaxNum, U32* pU32UserNum )
{
    return user_ShowUser(pStUserArry, u32MaxNum, pU32UserNum);
}

/*****************************************************************************
  Function:     web_GetErrCodeString
  Description:  通过错误码得到相应的字符串
  Input:        wvErrCode  enErrCode        (i)   错误代码
  Output:       none
  Return:       错误码对应的字符串
  Author:       dadi.zeng
*****************************************************************************/
U8 * web_GetErrCodeString(wvErrCode enErrCode)
{
    return (U8 *)err_GetErrCodeString(enErrCode);
}

/*****************************************************************************
  Function:     web_REG_Read_fortest
  Description:  read register for web test
  Input:        u32Address     --- address of register
  Output:       pu32Data       --- value of register
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void web_REG_Read_fortest(U32 u32Address, U32 *pu32Data)
{
    REG_Read(u32Address, pu32Data);
}

/*****************************************************************************
  Function:     web_REG_Write_fortest
  Description:  write register
  Input:        u32Address      --- address of register
                u32Data         --- value of register
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void web_REG_Write_fortest(U32 u32Address, U32 u32Data)
{
    REG_Write(u32Address, u32Data);
}

/*****************************************************************************
  Function:     TSP_MutexLock
  Description:  锁定流结构互赤锁
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
int web_TSPMutexLock(void)
{
    return TSP_MutexLock();
}

/*****************************************************************************
  Function:     TSP_MutexUnlock
  Description:  释放流结构互赤锁
  Input:        pTSPMutex
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
int web_TSPMutexUnlock(void)
{
    return TSP_MutexUnlock();
}

/*****************************************************************************
  Function:     web_GetTSPramaHandle
  Description:  获取整个系统的流结构信息的指针
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
TSInfo *web_GetTSPramaHandle(void)
{
    return TSP_GetTSParamHandle();
}

/*****************************************************************************
  Function:     web_ClearTSConfiguration
  Description:  清空流配置
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_ClearTSConfiguration(void)
{
    TSInfo *pstParamTS = web_GetTSPramaHandle();
        
    return TSP_ClearTSConfiguration(pstParamTS);
}

/*****************************************************************************
  Function:     TSP_GetInputProgIndex
  Description:  获取输入节目的索引
  Input:        u16InTSIndex:输入节目所在的流的索引
                u16SeviceID:要获取的节目的ServiceID
                pstParamTS
  Output:       pu16InProgIndex:获取到的节目索引
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_GetInProgIndex(U16 u16InTSIndex, U16 u16SeviceID, TSInfo *pstParamTS, U16 *pu16InProgIndex)
{
    return TSP_GetInputProgIndex(u16InTSIndex, u16SeviceID, pstParamTS, pu16InProgIndex);
}

/*****************************************************************************
  Function:     TSP_GetInputTSIndex
  Description:  获取输入流的索引
  Input:        u16Channel:输入流的Channel
                pstParamTS
  Output:       pu16InTSIndex:获取到的流索引
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_GetInTSIndex(U16 u16Channel, TSInfo *pstParamTS, U16 *pu16InTSIndex)
{
    return TSP_GetInputTSIndex(u16Channel, pstParamTS, pu16InTSIndex);
}

/*****************************************************************************
  Function:     TSP_GetInTSProgIndexList
  Description:  获取输入流的所有节目
  Input:        u16InTSIndex:获取哪个流
                pstParamTS
  Output:       pstIndexList:所有节目索引的集合
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_GetInTSProgIndexList(U16 u16InTSIndex, TSInfo *pstParamTS, IndexInfoList * pstIndexList)
{
    return TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, pstIndexList);
}

/*****************************************************************************
  Function:     web_GetInProgESPIDIndexList
  Description:  获取输入节目的所有ESPID
  Input:        u16InProgIndex:获取哪个节目
                pstParamTS
  Output:       pstIndexList:所有ESPID索引的集合
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_GetInProgESPIDIndexList(U16 u16InProgIndex, TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    return TSP_GetInProgESPIDIndexList(u16InProgIndex, pstParamTS, pstIndexList);
}

/*****************************************************************************
  Function:     web_GetInProgPSECMPIDIndexList
  Description:  获取输入节目的所有PSECM
  Input:        u16InProgIndex:获取哪个节目
                pstParamTS
  Output:       pstIndexList:所有PSECM索引的集合
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_GetInProgPSECMPIDIndexList(U16 u16InProgIndex, TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    return TSP_GetInProgPSECMPIDIndexList(u16InProgIndex, pstParamTS, pstIndexList);
}

/*****************************************************************************
  Function:     web_GetInPIDESECMPIDIndexList
  Description:  获取输入PID的所有ESECM
  Input:        u16InPIDIndex:获取哪个PID
                pstParamTS
  Output:       pstIndexList:所有ESECM索引的集合
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_GetInPIDESECMPIDIndexList(U16 u16InPIDIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    return TSP_GetInPIDESECMPIDIndexList(u16InPIDIndex, pstParamTS, pstIndexList);
}

/*****************************************************************************
  Function:     web_GetPIDType
  Description:  获取输入PID的类型(音频或视频)
  Input:        u16PIDIndex:PID索引
                pstParamTS
  Output:       
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
U8 web_GetPIDType(U16 u16PIDIndex, TSInfo *pstParamTS)
{
    return TSP_GetPIDType(u16PIDIndex, pstParamTS);
}

/*****************************************************************************
  Function:     web_SetStreamIDBeforeGetPIDBitrate
  Description:  设置流ID，用于获取PID码率
  Input:        u16Channel:
  Output:       
  Return:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void web_SetStreamIDBeforeGetPIDBitrate(U16 u16Channel)
{
    TSP_SetStreamIDBeforeGetPIDBitrate(u16Channel);
}

/*****************************************************************************
  Function:     TSP_GetPIDBitRate
  Description:  获取PID的码率，时钟频率和流ID必须事先设置好(在获取整个流或节目的所有PID时，节省时间)
  Input:        u16PID:
  Output:       
  Return:       PID BitRate
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
U32 web_GetPIDBitRate(U16 u16PID)
{
    return TSP_GetPIDBitRate(u16PID);
}

/*****************************************************************************
  Function:     web_GetTSDestInfo
  Description:  获取流的所有去向
  Input:        u16InTSIndex:流索引
                pstParamTS
  Output:       pstDest:流的所有去向
  Return:       SUCESS 成功
                Others 失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_GetTSDestInfo(U16 u16InTSIndex, TSInfo *pstParamTS, Dest *pstDest)
{
    return TSP_GetTSDestInfo(u16InTSIndex, pstParamTS, pstDest);
}

/*****************************************************************************
  Function:     web_GetProgDestInfo
  Description:  获取节目的所有去向
  Input:        u16InProgIndex:节目索引
                pstParamTS
  Output:       pstDest:节目的所有去向
  Return:       SUCESS 成功
                Others 失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_GetProgDestInfo(U16 u16InProgIndex, TSInfo *pstParamTS, Dest *pstDest)
{
    return TSP_GetProgDestInfo(u16InProgIndex, pstParamTS, pstDest);
}

/*****************************************************************************
  Function:     web_ProcessTSDest
  Description:  处理一个流的去向
  Input:        u16InTSIndex:输入流的索引
                pstNewDest:网管要设置的去向信息
                pstParamTS
  Output:       
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_ProcessTSDest(U16 u16InTSIndex, Dest *pstNewDest, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    
    enErrCode = TSP_CheckTSDestParam(u16InTSIndex, pstNewDest, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_CheckTSDestParam Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }
    
    enErrCode = TSP_ProcessTSDest(u16InTSIndex, pstNewDest, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_ProcessTSDest Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    enErrCode = TSP_SetBypassAndMUXFlag(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetBypassAndMUXFlag Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }

    enErrCode = TSP_SetInputLUT(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetInputLUT Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }

    return TSP_StoreTSConfiguration();
}

/*****************************************************************************
  Function:     web_ProcessProgDest
  Description:  处理一个节目的去向
  Input:        u16InProgIndex:输入节目的索引
                pstNewDest:网管要设置的去向信息
                pstParamTS
  Output:       
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_ProcessProgDest(U16 u16InProgIndex, Dest *pstNewDest, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    
    enErrCode = TSP_CheckProgDestParam(u16InProgIndex, pstNewDest, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_CheckProgDestParam Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }
    
    enErrCode = TSP_ProcessProgDest(u16InProgIndex, pstNewDest, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_ProcessProgDest Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    enErrCode = TSP_SetBypassAndMUXFlag(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetBypassAndMUXFlag Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }

    enErrCode = TSP_SetInputLUT(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetInputLUT Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }

    return TSP_StoreTSConfiguration();
}

/*****************************************************************************
  Function:     web_ProcessDescramble
  Description:  设置解扰节目
  Input:        arstDescrambleProgInfo:要设置解扰的节目的解扰参数信息链表
                u16DescrambleProgNum:要设置解扰的节目个数
                pstParamTS
  Output:       
  Return:       success or error code
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_ProcessDescramble(DescrambleProgInfo arstDescrambleProgInfo[], U16 u16DescrambleProgNum, TSInfo *pstParamTS)
{
    return TSP_ProcessDescramble(arstDescrambleProgInfo, u16DescrambleProgNum, pstParamTS);
}

/*****************************************************************************
  Function:     web_DebugPrintLUTInfo
  Description:  打印输入LUT
  Input:        u8LUTIndex:LUT索引
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void web_DebugPrintLUTInfo(U8 u8LUTIndex)
{
    TSP_DebugPrintLUTInfo(u8LUTIndex);
}

/*****************************************************************************
  Function:     web_DebugPrintCIOutputLUTInfo
  Description:  打印CIOutputLUTInfo
  Input:        
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void web_DebugPrintCIOutputLUTInfo(void)
{
    TSP_DebugPrintCIOutputLUTInfo();
}

/*****************************************************************************
  Function:     web_GetCTunerParam
  Description:  获取tuner参数
  Input:        u8Port:tuner端口号
                lockParam:参数结构体指针
  Output:       
  Return:       SUCESS 
                Others 
  Others:       
  Create:       huada.huang
*****************************************************************************/
wvErrCode web_GetCTunerParam(U8 u8Port, Tuner_C_Params_t *lockParam )
{
    return  Tuner_c_GetLockParam(u8Port, lockParam);
}

/*****************************************************************************
  Function:     web_SetCTunerParam
  Description:  设置tuner参数
  Input:        u8Port:tuner端口号
                lockParam:参数结构体指针
  Output:       
  Return:       SUCESS 
                Others 
  Others:       
  Create:       huada.huang
*****************************************************************************/
wvErrCode web_SetCTunerParam(U8 u8Port, Tuner_C_Params_t *lockParam )
{
    return  Tuner_c_SetLockParam(u8Port, lockParam);
}


/*****************************************************************************
  Function:     web_GetDTMBTunerParam
  Description:  获取tuner参数
  Input:        u8Port:tuner端口号
                lockParam:参数结构体指针
  Output:       
  Return:       SUCESS 
                Others 
  Others:       
  Create:       huada.huang
*****************************************************************************/
wvErrCode web_GetDTMBTunerParam(U8 u8Port, Tuner_C_Params_t *lockParam )
{
    return  Tuner_dtmb_GetLockParam(u8Port, lockParam);
}

/*****************************************************************************
  Function:     web_SetDTMBTunerParam
  Description:  设置tuner参数
  Input:        u8Port:tuner端口号
                lockParam:参数结构体指针
  Output:       
  Return:       SUCESS 
                Others 
  Others:       
  Create:       huada.huang
*****************************************************************************/
wvErrCode web_SetDTMBTunerParam(U8 u8Port, Tuner_C_Params_t *lockParam )
{
    return  Tuner_dtmb_SetLockParam(u8Port, lockParam);
}


/*****************************************************************************
  Function:     web_GetS2TunerParam
  Description:  获取tuner参数
  Input:        u8Port:tuner端口号
                lockParam:参数结构体指针
  Output:       
  Return:       SUCESS 
                Others 
  Others:       
  Create:       huada.huang
*****************************************************************************/
wvErrCode web_GetS2TunerParam(U8 u8Port, Tune_S2_Params_t *S2Param )
{
    return Tuner_s2GetParameter(u8Port, S2Param);
}


/*****************************************************************************
  Function:     web_SetS2TunerParam
  Description:  获取tuner参数
  Input:        u8Port:tuner端口号
                lockParam:参数结构体指针
  Output:       
  Return:       SUCESS 
                Others 
  Others:       
  Create:       huada.huang
*****************************************************************************/
wvErrCode web_SetS2TunerParam(U8 u8Port, Tune_S2_Params_t *S2Param )
{
    return Tuner_s2SetParameter(u8Port, S2Param);
}


/*****************************************************************************
  Function:     web_GetTunerStatus
  Description:  获取tuner状态
  Input:        u8Port:tuner端口号
                tunerStat: 状态结构体指针
  Output:       
  Return:       SUCESS 
                Others 
  Others:       
  Create:       huada.huang
*****************************************************************************/
wvErrCode web_GetTunerStatus(U8 u8Port, Tuner_stat *tunerStat)
{
    wvErrCode ret;
    Tuner_SrcStatus_t  pstTunerStatus;
     memset(&pstTunerStatus, 0, sizeof(Tuner_SrcStatus_t));
     

    ret = Tuner_getStatus(u8Port, &pstTunerStatus);
    
    if(WV_SUCCESS != ret)
    {
       return ret;
    }

    tunerStat->port = u8Port;

    tunerStat->channel = 1;
 
    tunerStat->isLock = pstTunerStatus.bLock;
    
    tunerStat->totalbitrate = pstTunerStatus.totalbitrate;
    tunerStat->effectivebitrate = pstTunerStatus.effecbitrate;
    
    tunerStat->s32RFLevel = pstTunerStatus.s32RFLevel;
               

    tunerStat->uBerCnt = pstTunerStatus.uBerCnt;
    tunerStat->uBerExponent = pstTunerStatus.uBerExponent;
    tunerStat->s32C_N =  pstTunerStatus.s32C_N;
    
    tunerStat->frequencytune = pstTunerStatus.u32TunerFrequency;

    return WV_SUCCESS;
}


/*****************************************************************************
  Function:     web_GetTunerStatus
  Description:  获取tuner类型
  Input:        tunerType:tuner类型数据
  Output:       
  Return:       SUCESS 
  Others:       
  Create:       huada.huang
*****************************************************************************/
wvErrCode Web_getTunerType(TUNER_TYPE_enum *tunerType)
{
    *tunerType = Tuner_GetTunerType();
    return WV_SUCCESS;
}


/*****************************************************************************
  Function:     Web_ExportLogs
  Description:  日志导出
  Input:        none
  Output:       U8 *exportURI 打包后地址
  Return:       WV_SUCCESS      -   成功
  Author:       ruibin.zhang
*****************************************************************************/
wvErrCode Web_ExportLogs(U8 * exportURI)
{
    if(NULL == exportURI)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_WEB, "Param is NULL!");
        return WV_ERR_PARAMS;
    }

    U8 u8Command[MAX_COMMAND_LENGTH];

    if(access(LOG_SAVE_FILE, F_OK) == -1)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_WEB, "Can't find %s file!",LOG_SAVE_FILE);
        return WV_ERR_FILE_NOT_EXISTING;
    }

    if(NULL == opendir(DOWN_LOAD_LOG_DIR))
    {
        if(mkdir(DOWN_LOAD_LOG_DIR, 0777) != 0)
        {
            LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_WEB, "Create dir %s fail!",DOWN_LOAD_LOG_DIR);
            return WV_ERR_WEB_DIRCREATFAIL;
        }
    }

    snprintf((char *)exportURI, MAX_FILENAME_LENGTH, "%s", DOWN_LOAD_LOG_REL);

    snprintf((char *)u8Command, MAX_COMMAND_LENGTH, "tar -cf %s %s", DOWN_LOAD_LOG, LOG_SAVE_FILE);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_WEB, "Command : %s", u8Command);
    system((char *)u8Command);

    if(access((char *)DOWN_LOAD_LOG, F_OK) == -1)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_WEB, "tar %s fail!", DOWN_LOAD_LOG);
        return WV_ERR_WEB_TARFAIL;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     web_IsSubBoardPlugin
  Description:  判断对应槽位是否有插入子板
  Input:        u8Slot
  Output:       none
  Return:       TRUE or FALSE
  Author:       Momouwei 2017.04.14
*****************************************************************************/
BOOL web_IsSubBoardPlugin(U8 u8Slot)
{
    return Status_CheckSubBoardPlugExist(u8Slot);
}

/*****************************************************************************
  Function:     web_IsOutputBoard
  Description:  判断子板是不是输出板类型
  Input:        u8BoardType:子板类型
  Output:       
  Return:       TRUE or FALSE
  Author:       Momouwei 2017.04.14
*****************************************************************************/
BOOL web_IsOutputBoard(U8 u8BoardType)
{
    return Resource_IsOutputBoard(u8BoardType);
}

/*****************************************************************************
  Function:     web_GetBoardType
  Description:  获取对应槽位的子板的类型
  Input:        u8Slot:
  Output:       
  Return:       success or error code
  Author:       Momouwei 2017.04.14
*****************************************************************************/
U8 web_GetBoardType(U8 u8Slot)
{
    return Resource_getBoardType(u8Slot);
}

/*****************************************************************************
  Function:     web_GetSelfBoardDescriptor
  Description:  获取对应槽位的子板的描述(名称)
  Input:        u8Slot:
  Output:       pu8Descroptor:对应槽位的子板的描述(名称)
  Return:       success or error code
  Author:       Momouwei 2017.04.14
*****************************************************************************/
wvErrCode web_GetSelfBoardDescriptor(U8 *pu8Descroptor)
{
    return Resource_getBoardDescriptor(Status_GetSlotID(), pu8Descroptor);
}

/*****************************************************************************
  Function:     web_GetBoardDescriptor
  Description:  获取对应槽位的子板的描述(名称)
  Input:        u8Slot:
  Output:       pu8Descroptor:对应槽位的子板的描述(名称)
  Return:       success or error code
  Author:       Momouwei 2017.04.14
*****************************************************************************/
wvErrCode web_GetBoardDescriptor(U8 u8Slot, U8 *pu8Descroptor)
{
    return Resource_getBoardDescriptor(u8Slot, pu8Descroptor);
}

/*****************************************************************************
  Function:     web_GetBoardPortNum
  Description:  获取对应槽位的子板的端口个数
  Input:        u8Slot:
  Output:       
  Return:       对应槽位的子板的端口个数
  Author:       Momouwei 2017.04.14
*****************************************************************************/
U8 web_GetBoardPortNum(U8 u8Slot)
{
    return Resource_getBoardPortNum(u8Slot);
}

/*****************************************************************************
  Function:     web_GetInputChannelNum
  Description:  获取对应槽位的子板的输入通道个数
  Input:        u8Slot:
  Output:       
  Return:       对应槽位的子板的输入通道个数
  Author:       Momouwei 2017.04.14
*****************************************************************************/
U16 web_GetInputChannelNum(U8 u8Slot)
{
    return Resource_getInputChannelNum(u8Slot);
}

/*****************************************************************************
  Function:     web_GetOutputChannelNum
  Description:  获取对应槽位的子板的输出通道个数
  Input:        u8Slot:
  Output:       
  Return:       对应槽位的子板的输出通道个数
  Author:       Momouwei 2017.04.14
*****************************************************************************/
U16 web_GetOutputChannelNum(U8 u8Slot)
{
    return Resource_getOutputChannelNum(u8Slot);
}

/*****************************************************************************
  Function:     web_GetOutputChannelStatus
  Description:  获取对应槽位的子板的对应输出通道的状态(是否直通)
  Input:        u8Slot:
                u8Channel:
  Output:       
  Return:       对应槽位的子板的对应输出通道的状态(是否直通)
  Author:       Momouwei 2017.04.14
*****************************************************************************/
U8 web_GetOutputChannelStatus(U8 u8Slot, U8 u8Channel)
{
    return Resource_getOutputChannelStatus(u8Slot, u8Channel);
}

/*****************************************************************************
  Function:     web_Reboot
  Description:  重启
  Input:        
                
  Output:       
  Return:       
  Author:       Momouwei 2017.04.14
*****************************************************************************/
wvErrCode web_Reboot(void)
{
    return BMN_Reboot();
}

/*****************************************************************************
  Function:     web_Default
  Description:  恢复出厂设置
  Input:        
                
  Output:       
  Return:       
  Author:       Momouwei 2017.04.14
*****************************************************************************/
wvErrCode web_Default(void)
{
    PARAM_Default();

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     web_ClearAllTS
  Description:  清空流配置
  Input:        
                
  Output:       
  Return:       
  Author:       Momouwei 2017.04.14
*****************************************************************************/
wvErrCode web_ClearAllTS(void)
{
    return TSP_ClearTSConfiguration(TSP_GetTSParamHandle());
}

/*****************************************************************************
  Function:     web_GetTSBitrateInfo
  Description:  获取流的码率信息
  Input:        u8Channel
                pstTSBitrateInfo
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_GetTSBitrateInfo(U16 u16Channel, TSBitrateInfo *pstTSBitrateInfo)
{
    return TSP_GetTSBitrateInfo(u16Channel, pstTSBitrateInfo);
}

/*****************************************************************************
  Function:     web_ClearPIDCc
  Description:  web_ClearPIDCc
  Input:        
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode web_ClearPIDCc(void)
{
    TSP_ClearPIDCC();
    return WV_SUCCESS;
}

/********************************************************************
Function:     Web_GetPIDTypeDesc
Description:  Get PID Type Desc
Input:        u8Type
Output:       -
Return:       PIDTypeDesc
Author:       huan.zhou
********************************************************************/
void web_GetPIDTypeDesc(U8 u8Type, U8 u8StreamType, U8 *u8TypeName)
{
    if (!u8TypeName)
    {
        return;
    }

    U8 aru8StreamTypeDesc[MAX_STREAMTYPE_NAME_LENGTH] = {0};
    
    switch (u8Type)
    {
        case VIDEO_PID:
            if (0 == u8StreamType)
            {
                snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "Video");
            }
            else
            {
                web_GetStreamTypeDesc(u8StreamType, aru8StreamTypeDesc);
                snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "%s", aru8StreamTypeDesc);
            }
            
            break;
        case AUDIO_PID:
            if (0 == u8StreamType)
            {
                snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "Audio");
            }
            else
            {
                web_GetStreamTypeDesc(u8StreamType, aru8StreamTypeDesc);
                snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "%s", aru8StreamTypeDesc);
            }
            break;
        case PCR_PID:
            snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "PCR");
            break;
        case EMM_PID:
            snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "EMM");
            break;
        case PSECM_PID:
        case ESECM_PID:
            snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "ECM");
            break;
        case OTHER_PID:
            snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "Other");
            break;
        case PAT_TABLE_PID:
            snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "PAT");
            break;
        case SDT_TABLE_PID:
            snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "SDT");
            break;
        case CAT_TABLE_PID:
            snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "CAT");
            break;
        case PMT_TABLE_PID:
            snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "PMT");
            break;
        case PRIVATE_DATA_PID:
            snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "PrivateData");
            break;
        default:
            snprintf(u8TypeName, MAX_STREAMTYPE_NAME_LENGTH, "Unknown");
            break;
    }
}

/********************************************************************
Function:     GetStreamTypeDesc
Description:  Get Stream Type Desc
Input:        u8Type
Output:       -
Return:       StreamTypeDesc
Author:       lipeng
********************************************************************/
void web_GetStreamTypeDesc(U8 u8StreamType, U8 *u8StreamTypeDesc)
{
    int s32TempLen = 0;
    int s32MaxLen = MAX_STREAMTYPE_NAME_LENGTH;
    U8 *pu8Temp = NULL;
    
    if (!u8StreamTypeDesc)
    {
        return;
    }

    s32TempLen= snprintf(u8StreamTypeDesc, s32MaxLen, "StreamType:%d-", u8StreamType);

    if ((s32TempLen <= 0) || (MAX_STREAMTYPE_NAME_LENGTH <= s32TempLen))
    {
        return;
    }
    
    pu8Temp = u8StreamTypeDesc + s32TempLen;
    s32MaxLen = MAX_STREAMTYPE_NAME_LENGTH - s32TempLen;

    switch (u8StreamType)
    {
        case STREAM_TYPE_VIDEO:
        case STREAM_TYPE_VIDEO1:
            snprintf(pu8Temp, s32MaxLen, "Video(MPEG2)");
            break;
        case STREAM_TYPE_VIDEO_AVS:
            snprintf(pu8Temp, s32MaxLen, "Video(AVS)");
            break;
        case STREAM_TYPE_VIDEO_H264:
            snprintf(pu8Temp, s32MaxLen, "Video(H264)");
            break;
        case STREAM_TYPE_VIDEO_H265:
            snprintf(pu8Temp, s32MaxLen, "Video(H265)");
            break;
        case STREAM_TYPE_VIDEO_MPEG4:
            snprintf(pu8Temp, s32MaxLen, "Video(MPEG4)");
            break;
        case STREAM_TYPE_AUDIO_AC3:
            snprintf(pu8Temp, s32MaxLen, "Audio(AC3)");
            break;
        case STREAM_TYPE_DVB_AUDIO_AC3:
            snprintf(pu8Temp, s32MaxLen, "Private Data/AC3");
            break;
        case STREAM_TYPE_AUDIO:
        case STREAM_TYPE_AUDIO1:
            snprintf(pu8Temp, s32MaxLen, "Audio");
            break;
        case STREAM_TYPE_MPEG2_AAC:
            snprintf(pu8Temp, s32MaxLen, "Audio(MPEG2_AAC)");
            break;
        case STREAM_TYPE_MPEG4_AAC:
            snprintf(pu8Temp, s32MaxLen, "Audio(MPEG4_AAC)");
            break;
        case STREAM_TYPE_ECM:
            snprintf(pu8Temp, s32MaxLen, "ECM");
            break;
        case STREAM_TYPE_PRIVATE2:
            snprintf(pu8Temp, s32MaxLen, "Private Data");
            break;
        case STREAM_TYPE_USER_PRIVATE:
        default:
            snprintf(pu8Temp, s32MaxLen, "User Private");
            break;
    }
}

/********************************************************************
Function:     web_GetCIStatus
Description:  获取CI解扰状态信息
Input:        u8CAMIndex:
Output:       pstCIStatus:CI解扰状态信息
Return:       
Author:       Momouwei 2017.05.11
********************************************************************/
wvErrCode web_GetCIStatus(U8 u8CAMIndex, WAVCI_Status_4_web_t *pstCIStatus, TSInfo *pstParamTS)
{
    return WVCI_GetCIStatus(u8CAMIndex, pstCIStatus, pstParamTS);
}

/*****************************************************************************
  Function:     web_IsProgSet2BeDescrambled
  Description:  判断输入节目是否被设置了解扰
  Input:        u16InProgIndex:输入节目索引
                pstParamTS
  Output:       
  Return:       TRUE or FALSE
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL web_IsProgSet2BeDescrambled(U16 u16InProgIndex, TSInfo *pstParamTS)
{
    return TSP_IsProgSet2BeDescrambled(u16InProgIndex, pstParamTS);
}





wvErrCode web_XmlDescramble(DescrambleProgInfo arstDescrambleProgInfo[], U16 u16DescrambleProgNum, TSInfo *pstParamTS)
{
	wvErrCode ret = WV_SUCCESS;

	//鐩稿叧CI瑙ｆ壈浠ｇ爜


	//璁剧疆LUT
	LUT_SetInputLUT(pstParamTS);

	return ret;
}

