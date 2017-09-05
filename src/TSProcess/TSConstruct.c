/**********************************************************************
* Copyright(c), HuiZhou WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:    TSConstruct.c
* Description: MuxProcess
*
* Version:     1.0.0
* Author:      Momouwei
* Date:        2017.02.22
*
**********************************************************************/
#include "TSConstruct.h"
#include "WV_log.h"
#include "FPGA.h"
#include "IPComm.h"
#include "Resource.h"
#include "parameters.h"
#include "tools.h"
#include "status.h"
#include "SiSend.h"
#include "web_xml.h"

#include <stdbool.h>

#define ALLOCATE_INPUT_PROG_NUM_PER_TIME         (100)
#define ALLOCATE_INPUT_PID_NUM_PER_TIME          (500)
#define ALLOCATE_INPUT_TABLE_NUM_PER_TIME        (100)
#define ALLOCATE_INPUT_DESCRIPTOR_NUM_PER_TIME   (100)

#define ALLOCATE_OUTPUT_PROG_NUM_PER_TIME        (50)
#define ALLOCATE_OUTPUT_PID_NUM_PER_TIME         (500)

#define FILE_NAME_TS_CONFIG "/wellav_data/ts.config"

static TSInfo s_stTSParam;
static pthread_mutex_t s_TSPMutex = PTHREAD_MUTEX_INITIALIZER;

static U8 s_aru8DestSlotRecord[MAX_DEST] = {0xFF, 0xFF, 0xFF, 0xFF};//º«¬ºPID»•¡Àƒƒ–©◊”∞Â£¨PID◊Ó∂‡ƒ‹»•4øÈ◊”∞Â
static U16 s_u16UDPPort = 1234;
static U8 s_aru8ClearTSFlag[MAX_INPUT_CHANNEL_NUM] = {0};

static U8 s_aru8ConstructBuffer[MAX_CONSTRUCT_SI_BUFF];

/*****************************************************************************
  Function:     TSP_MutexLock
  Description:  À¯∂®¡˜Ω·ππª•≥‡À¯
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
int TSP_MutexLock(void)
{
    return pthread_mutex_lock(&s_TSPMutex);
}

/*****************************************************************************
  Function:     TSP_MutexUnlock
  Description:   Õ∑≈¡˜Ω·ππª•≥‡À¯
  Input:        pTSPMutex
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
int TSP_MutexUnlock(void)
{
    return pthread_mutex_unlock(&s_TSPMutex);
}

/*****************************************************************************
  Function:     TSP_GetStructVersion
  Description:  ªÒ»°¡˜Ω·ππµƒ∞Ê±æ∫≈
  Input:        
  Output:       
  Return:       ¡˜Ω·ππµƒ∞Ê±æ∫≈
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
U32 TSP_GetStructVersion(void)
{
    return TS_STRUCT_VERSION;
}

/*****************************************************************************
  Function:     TSP_GetTSParamHandle
  Description:  ªÒ»°’˚∏ˆœµÕ≥µƒ¡˜Ω·ππ–≈œ¢µƒ÷∏’Î
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
TSInfo *TSP_GetTSParamHandle(void)
{
    return &s_stTSParam;
}

/*****************************************************************************
  Function:     TSP_ClearTSInfo
  Description:  «Âø’¡˜Ω·ππµƒÀ˘”––≈œ¢
  Input:        pstParamTS:
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_ClearTSInfo(TSInfo *pstParamTS)
{
    if (!pstParamTS)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error\r\n", __FUNCTION__, __LINE__);
        return;
    }

    TSP_ClearAllInputTSInfo(pstParamTS);
    TSP_ClearAllOutputTSInfo(pstParamTS);
}

/*****************************************************************************
  Function:     TSP_InitTSInfo
  Description:  ≥ı ºªØ¡˜–≈œ¢
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_InitTSInfo(TSInfo *pstParamTS, TSInfoInitNum *pstTSInfoInitNum)
{
    U32 u32Num = 0;
    
    if ((!pstParamTS) || (!pstTSInfoInitNum))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],pstTSInfoInitNum[%u]\r\n",
            __FUNCTION__, __LINE__,pstParamTS, pstTSInfoInitNum);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(pstParamTS, 0, sizeof(TSInfo));

    u32Num = (0 == pstTSInfoInitNum->u32TSNumber) ? INIT_TS_NUM : pstTSInfoInitNum->u32TSNumber;
    pstParamTS->pInputTS = calloc(u32Num, sizeof(Input_TSStream));
    if (!pstParamTS->pInputTS)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc for pInputTS fail,u32Num[%u],TSSize[%u]\r\n",
            __FUNCTION__, __LINE__, u32Num, sizeof(Input_TSStream));
        return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
    }
    pstParamTS->u32InputTSNumber = u32Num;

    u32Num = (0 == pstTSInfoInitNum->u32ProgramNumber) ? INIT_PROG_NUM : pstTSInfoInitNum->u32ProgramNumber;
    pstParamTS->pInputProgram = calloc(u32Num, sizeof(Input_TSProgram));
    if (!pstParamTS->pInputProgram)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc for pInputProgram fail,u32Num[%u],ProgSize[%u]\r\n",
            __FUNCTION__, __LINE__, u32Num, sizeof(Input_TSProgram));
        goto ERROR_ALLOC_FOR_PROG;
    }
    pstParamTS->u32InputProgramNumber = u32Num;

    u32Num = (0 == pstTSInfoInitNum->u32PIDNumber) ? INIT_PID_NUM : pstTSInfoInitNum->u32PIDNumber;
    pstParamTS->pInputPID = calloc(u32Num, sizeof(Input_TSPID));
    if (!pstParamTS->pInputPID)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc for pInputPID fail,u32Num[%u],PIDSize[%u]\r\n",
            __FUNCTION__, __LINE__, u32Num, sizeof(Input_TSPID));
        goto ERROR_ALLOC_FOR_PID;
    }
    pstParamTS->u32InputPIDNumber = u32Num;

    u32Num = (0 == pstTSInfoInitNum->u32TableNumber) ? INIT_TABLE_NUM : pstTSInfoInitNum->u32TableNumber;
    pstParamTS->pInputTable = calloc(u32Num, sizeof(TSTable));
    if (!pstParamTS->pInputTable)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc for pInputTable fail,u32Num[%u],TableSize[%u]\r\n",
            __FUNCTION__, __LINE__, u32Num, sizeof(TSTable));
        goto ERROR_ALLOC_FOR_TABLE;
    }
    pstParamTS->u32InputTableNumber = u32Num;

    u32Num = (0 == pstTSInfoInitNum->u32DescriptorNumber) ? INIT_DESCRIPTOR_NUM : pstTSInfoInitNum->u32DescriptorNumber;
    pstParamTS->pInputDescriptor = calloc(u32Num, sizeof(TSDescriptor));
    if (!pstParamTS->pInputDescriptor)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc for pInputDescriptor fail,u32Num[%u],DescriptorSize[%u]\r\n",
            __FUNCTION__, __LINE__, u32Num, sizeof(TSDescriptor));
        goto ERROR_ALLOC_FOR_DESCRIPTOR;
    }
    pstParamTS->u32InputDescriptorNumber = u32Num;

    u32Num = (0 == pstTSInfoInitNum->u32OutputTSNumber) ? INIT_OUTPUT_TS_NUM : pstTSInfoInitNum->u32OutputTSNumber;
    pstParamTS->pOutputTS = calloc(u32Num, sizeof(Output_TSStream));
    if (!pstParamTS->pOutputTS)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc for pOutputTS fail,u32Num[%u],OutTSSize[%u]\r\n",
            __FUNCTION__, __LINE__, u32Num, sizeof(Output_TSStream));
        goto ERROR_ALLOC_FOR_OUTPUT_TS;
    }
    pstParamTS->u32OutputTSNumber = u32Num;

    u32Num = (0 == pstTSInfoInitNum->u32OutputProgramNumber) ? INIT_OUTPUT_PROG_NUM : pstTSInfoInitNum->u32OutputProgramNumber;
    pstParamTS->pOutputProgram = calloc(u32Num, sizeof(Output_TSProgram));
    if (!pstParamTS->pOutputProgram)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc for pOutputProgram fail,u32Num[%u],OutProgSize[%u]\r\n",
            __FUNCTION__, __LINE__, u32Num, sizeof(Output_TSProgram));
        goto ERROR_ALLOC_FOR_OUTPUT_PROG;
    }
    pstParamTS->u32OutputProgramNumber = u32Num;

    u32Num = (0 == pstTSInfoInitNum->u32OutputPIDNumber) ? INIT_OUTPUT_PID_NUM : pstTSInfoInitNum->u32OutputPIDNumber;
    pstParamTS->pOutputPID = calloc(u32Num, sizeof(Output_TSPID));
    if (!pstParamTS->pOutputPID)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc for pOutputPID fail,u32Num[%u],OutPIDSize[%u]\r\n",
            __FUNCTION__, __LINE__, u32Num, sizeof(Output_TSPID));
        goto ERROR_ALLOC_FOR_OUTPUT_PID;
    }
    pstParamTS->u32OutputPIDNumber = u32Num;

    u32Num = (0 == pstTSInfoInitNum->u32OutputTableNumber) ? INIT_OUTPUT_TABLE_NUM : pstTSInfoInitNum->u32OutputTableNumber;
    pstParamTS->pOutputTable = calloc(u32Num, sizeof(TSTable));
    if (!pstParamTS->pOutputTable)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc for pOutputTable fail,u32Num[%u],TSTableSize[%u]\r\n",
            __FUNCTION__, __LINE__, u32Num, sizeof(TSTable));
        goto ERROR_ALLOC_FOR_OUTPUT_TABLE;
    }
    pstParamTS->u32OutputTableNumber = u32Num;

    u32Num = (0 == pstTSInfoInitNum->u32OutputDescriptorNumber) ? INIT_OUTPUT_DESCRIPTOR_NUM : pstTSInfoInitNum->u32OutputDescriptorNumber;
    pstParamTS->pOutputDescriptor = calloc(u32Num, sizeof(TSDescriptor));
    if (!pstParamTS->pOutputDescriptor)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc for pOutputDescriptor fail,u32Num[%u],TSDescriptorSize[%u]\r\n",
            __FUNCTION__, __LINE__, u32Num, sizeof(TSDescriptor));
        goto ERROR_ALLOC_FOR_OUTPUT_DESCRIPTOR;
    }
    pstParamTS->u32OutputDescriptorNumber = u32Num;
    
    return WV_SUCCESS;
    

ERROR_ALLOC_FOR_OUTPUT_DESCRIPTOR:
    free(pstParamTS->pOutputTable);
    
ERROR_ALLOC_FOR_OUTPUT_TABLE:
    free(pstParamTS->pOutputPID);

ERROR_ALLOC_FOR_OUTPUT_PID:
    free(pstParamTS->pOutputProgram);
    
ERROR_ALLOC_FOR_OUTPUT_PROG:
    free(pstParamTS->pOutputTS);

ERROR_ALLOC_FOR_OUTPUT_TS:
    free(pstParamTS->pInputDescriptor);

ERROR_ALLOC_FOR_DESCRIPTOR:
    free(pstParamTS->pInputTable);

ERROR_ALLOC_FOR_TABLE:
    free(pstParamTS->pInputPID);

ERROR_ALLOC_FOR_PID:
    free(pstParamTS->pInputProgram);

ERROR_ALLOC_FOR_PROG:
    free(pstParamTS->pInputTS);
    
    memset(pstParamTS, 0, sizeof(TSInfo));

    return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
}

/*****************************************************************************
  Function:     TSP_StoreTSConfiguration
  Description:  TSP_StoreTSConfiguration
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_StoreTSConfiguration(void)
{
    wvErrCode enErrCode = WV_SUCCESS;
    TSInfo *pstParamTS = TSP_GetTSParamHandle();
    TSIndex stTSIndex;

    U8 *pu8Buf = NULL;
    U8 *pu8TempBuf = NULL;

    U32 u32TotalSize = 0;
    U32 u32TSTotalSize = 0;
    U32 u32ProgTotalSize = 0;
    U32 u32PIDTotalSize = 0;
    U32 u32TableTotalSize = 0;
    U32 u32DescriptorTotalSize = 0;
    U32 u32OutputTSTotalSize = 0;
    U32 u32OutputProgTotalSize = 0;
    U32 u32OutputPIDTotalSize = 0;
    U32 u32OutputTableTotalSize = 0;
    U32 u32OutputDescriptorTotalSize = 0;
    U32 u32CRC = 0;

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "TSP_StoreTSConfiguration\r\n");
    
    memset(&stTSIndex, 0, sizeof(stTSIndex));

    stTSIndex.u32TSNumber = pstParamTS->u32InputTSNumber;
    stTSIndex.u32ProgramNumber = pstParamTS->u32InputProgramNumber;
    stTSIndex.u32PIDNumber = pstParamTS->u32InputPIDNumber;
    stTSIndex.u32TableNumber = pstParamTS->u32InputTableNumber;
    stTSIndex.u32DescriptorNumber = pstParamTS->u32InputDescriptorNumber;

    stTSIndex.u32OutputTSNumber = pstParamTS->u32OutputTSNumber;
    stTSIndex.u32OutputProgramNumber = pstParamTS->u32OutputProgramNumber;
    stTSIndex.u32OutputPIDNumber = pstParamTS->u32OutputPIDNumber;
    stTSIndex.u32OutputTableNumber = pstParamTS->u32OutputTableNumber;
    stTSIndex.u32OutputDescriptorNumber = pstParamTS->u32OutputDescriptorNumber;

    u32TSTotalSize = stTSIndex.u32TSNumber * sizeof(Input_TSStream);
    u32ProgTotalSize = stTSIndex.u32ProgramNumber * sizeof(Input_TSProgram);
    u32PIDTotalSize = stTSIndex.u32PIDNumber * sizeof(Input_TSPID);
    u32TableTotalSize = stTSIndex.u32TableNumber * sizeof(TSTable);
    u32DescriptorTotalSize = stTSIndex.u32DescriptorNumber * sizeof(TSDescriptor);

    u32OutputTSTotalSize = stTSIndex.u32OutputTSNumber * sizeof(Output_TSStream);
    u32OutputProgTotalSize = stTSIndex.u32OutputProgramNumber * sizeof(Output_TSProgram);
    u32OutputPIDTotalSize = stTSIndex.u32OutputPIDNumber * sizeof(Output_TSPID);
    u32OutputTableTotalSize = stTSIndex.u32OutputTableNumber * sizeof(TSTable);
    u32OutputDescriptorTotalSize = stTSIndex.u32OutputDescriptorNumber * sizeof(TSDescriptor);

    u32TotalSize = sizeof(stTSIndex)
                    + u32TSTotalSize
                    + u32ProgTotalSize
                    + u32PIDTotalSize
                    + u32TableTotalSize
                    + u32DescriptorTotalSize
                    + u32OutputTSTotalSize
                    + u32OutputProgTotalSize
                    + u32OutputPIDTotalSize
                    + u32OutputTableTotalSize
                    + u32OutputDescriptorTotalSize
                    + 4;

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "TSNum[%u],ProgNum[%u],PIDNum[%u],TableNum[%u],DescNum[%u]\r\n",
        stTSIndex.u32TSNumber,
        stTSIndex.u32ProgramNumber,
        stTSIndex.u32PIDNumber,
        stTSIndex.u32TableNumber,
        stTSIndex.u32DescriptorNumber);

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "OutTSNum[%u],OutProgNum[%u],OutPIDNum[%u],OutTableNum[%u],OutDescNum[%u]\r\n",
        stTSIndex.u32OutputTSNumber,
        stTSIndex.u32OutputProgramNumber,
        stTSIndex.u32OutputPIDNumber,
        stTSIndex.u32OutputTableNumber,
        stTSIndex.u32OutputDescriptorNumber);

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "TSSize[%u],ProgSize[%u],PIDSize[%u],TableSize[%u],DescSize[%u]\r\n",
        sizeof(Input_TSStream),
        sizeof(Input_TSProgram),
        sizeof(Input_TSPID),
        sizeof(TSTable),
        sizeof(TSDescriptor));

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "OutTSSize[%u],OutProgSize[%u],OutPIDSize[%u]\r\n",
        sizeof(Output_TSStream),
        sizeof(Output_TSProgram),
        sizeof(Output_TSPID));

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "u32TotalSize[%u]\r\n",u32TotalSize);
    
    pu8Buf = calloc(1, u32TotalSize);
    if (!pu8Buf)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc for u32TotalSize[%u] fail\r\n",
            __FUNCTION__, __LINE__, u32TotalSize);
        return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
    }

    pu8TempBuf = pu8Buf;

    memcpy(pu8TempBuf, &stTSIndex, sizeof(stTSIndex));
    pu8TempBuf += sizeof(stTSIndex);

    memcpy(pu8TempBuf, pstParamTS->pInputTS, u32TSTotalSize);
    pu8TempBuf += u32TSTotalSize;

    memcpy(pu8TempBuf, pstParamTS->pInputProgram, u32ProgTotalSize);
    pu8TempBuf += u32ProgTotalSize;

    memcpy(pu8TempBuf, pstParamTS->pInputPID, u32PIDTotalSize);
    pu8TempBuf += u32PIDTotalSize;

    memcpy(pu8TempBuf, pstParamTS->pInputTable, u32TableTotalSize);
    pu8TempBuf += u32TableTotalSize;

    memcpy(pu8TempBuf, pstParamTS->pInputDescriptor, u32DescriptorTotalSize);
    pu8TempBuf += u32DescriptorTotalSize;

    memcpy(pu8TempBuf, pstParamTS->pOutputTS, u32OutputTSTotalSize);
    pu8TempBuf += u32OutputTSTotalSize;

    memcpy(pu8TempBuf, pstParamTS->pOutputProgram, u32OutputProgTotalSize);
    pu8TempBuf += u32OutputProgTotalSize;

    memcpy(pu8TempBuf, pstParamTS->pOutputPID, u32OutputPIDTotalSize);
    pu8TempBuf += u32OutputPIDTotalSize;

    memcpy(pu8TempBuf, pstParamTS->pOutputTable, u32OutputTableTotalSize);
    pu8TempBuf += u32OutputTableTotalSize;

    memcpy(pu8TempBuf, pstParamTS->pOutputDescriptor, u32OutputDescriptorTotalSize);
    pu8TempBuf += u32OutputDescriptorTotalSize;

    u32CRC = Common_CRCCalculate(pu8Buf, u32TotalSize - 4, 0);

    *pu8TempBuf++ = (u32CRC >> 24) & 0xFF;
    *pu8TempBuf++ = (u32CRC >> 16) & 0xFF;
    *pu8TempBuf++ = (u32CRC >> 8) & 0xFF;
    *pu8TempBuf++ = u32CRC & 0xFF;

    enErrCode = PARAM_WriteFile(FILE_NAME_TS_CONFIG, pu8Buf, u32TotalSize);

    free(pu8Buf);
    
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]PARAM_WriteFile error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_RestoreTSConfiguration
  Description:  TSP_RestoreTSConfiguration
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_RestoreTSConfiguration(void)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8TryTimes = 0;
    TSInfo *pstParamTS = TSP_GetTSParamHandle();
    TSIndex stTSIndex;

    U8 *pu8Buf = NULL;
    U8 *pu8TempBuf = NULL;

    U32 u32TotalSize = 0;
    U32 u32TSTotalSize = 0;
    U32 u32ProgTotalSize = 0;
    U32 u32PIDTotalSize = 0;
    U32 u32TableTotalSize = 0;
    U32 u32DescriptorTotalSize = 0;
    U32 u32OutputTSTotalSize = 0;
    U32 u32OutputProgTotalSize = 0;
    U32 u32OutputPIDTotalSize = 0;
    U32 u32OutputTableTotalSize = 0;
    U32 u32OutputDescriptorTotalSize = 0;
    U32 u32CRC = 0;

    TSInfoInitNum stTSInfoInitNum;

    memset(&stTSInfoInitNum, 0, sizeof(TSInfoInitNum));
    memset(pstParamTS, 0, sizeof(TSInfo));
    memset(&stTSIndex, 0, sizeof(stTSIndex));

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "TSP_RestoreTSConfiguration\r\n");

    do
    {
        enErrCode = PARAM_ReadFile(FILE_NAME_TS_CONFIG, (U8 *)&stTSIndex, sizeof(stTSIndex));
        if ((WV_SUCCESS == enErrCode) || (WV_ERR_FILE_NOT_EXISTING == enErrCode))
        {
            break;
        }
        
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "[%s:%d]PARAM_ReadFile error,u8TryTimes[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u8TryTimes, enErrCode);

        usleep(100000);
        
        u8TryTimes++;
    }while (u8TryTimes < 2);

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "u8TryTimes[%u],enErrCode[%08X]\r\n",u8TryTimes,enErrCode);

    if ((2 <= u8TryTimes) || (WV_ERR_FILE_NOT_EXISTING == enErrCode))
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "[%s:%d]u8TryTimes[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u8TryTimes, enErrCode);

        enErrCode = TSP_InitTSInfo(pstParamTS, &stTSInfoInitNum);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
                "[%s:%d]TSP_InitTSInfo error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            memset(pstParamTS, 0, sizeof(TSInfo));
            return enErrCode;
        }

        enErrCode = TSP_StoreTSConfiguration();
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
                "[%s:%d]TSP_StoreTSConfiguration error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
        }

        return enErrCode;
    }

    u32TSTotalSize = stTSIndex.u32TSNumber * sizeof(Input_TSStream);
    u32ProgTotalSize = stTSIndex.u32ProgramNumber * sizeof(Input_TSProgram);
    u32PIDTotalSize = stTSIndex.u32PIDNumber * sizeof(Input_TSPID);
    u32TableTotalSize = stTSIndex.u32TableNumber * sizeof(TSTable);
    u32DescriptorTotalSize = stTSIndex.u32DescriptorNumber * sizeof(TSDescriptor);

    u32OutputTSTotalSize = stTSIndex.u32OutputTSNumber * sizeof(Output_TSStream);
    u32OutputProgTotalSize = stTSIndex.u32OutputProgramNumber * sizeof(Output_TSProgram);
    u32OutputPIDTotalSize = stTSIndex.u32OutputPIDNumber * sizeof(Output_TSPID);
    u32OutputTableTotalSize = stTSIndex.u32OutputTableNumber * sizeof(TSTable);
    u32OutputDescriptorTotalSize = stTSIndex.u32OutputDescriptorNumber * sizeof(TSDescriptor);

    u32TotalSize = sizeof(stTSIndex)
                    + u32TSTotalSize
                    + u32ProgTotalSize
                    + u32PIDTotalSize
                    + u32TableTotalSize
                    + u32DescriptorTotalSize
                    + u32OutputTSTotalSize
                    + u32OutputProgTotalSize
                    + u32OutputPIDTotalSize
                    + u32OutputTableTotalSize
                    + u32OutputDescriptorTotalSize
                    + 4;

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "TSNum[%u],ProgNum[%u],PIDNum[%u],TableNum[%u],DescNum[%u]\r\n",
        stTSIndex.u32TSNumber,
        stTSIndex.u32ProgramNumber,
        stTSIndex.u32PIDNumber,
        stTSIndex.u32TableNumber,
        stTSIndex.u32DescriptorNumber);

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "OutTSNum[%u],OutProgNum[%u],OutPIDNum[%u],OutTableNum[%u],OutDescNum[%u]\r\n",
        stTSIndex.u32OutputTSNumber,
        stTSIndex.u32OutputProgramNumber,
        stTSIndex.u32OutputPIDNumber,
        stTSIndex.u32OutputTableNumber,
        stTSIndex.u32OutputDescriptorNumber);

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "TSSize[%u],ProgSize[%u],PIDSize[%u],TableSize[%u],DescSize[%u]\r\n",
        sizeof(Input_TSStream),
        sizeof(Input_TSProgram),
        sizeof(Input_TSPID),
        sizeof(TSTable),
        sizeof(TSDescriptor));

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "OutTSSize[%u],OutProgSize[%u],OutPIDSize[%u]\r\n",
        sizeof(Output_TSStream),
        sizeof(Output_TSProgram),
        sizeof(Output_TSPID));

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "u32TotalSize[%u]\r\n",u32TotalSize);

    pu8Buf = calloc(1, u32TotalSize);
    if (!pu8Buf)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]calloc for u32TotalSize[%u] fail\r\n",
                __FUNCTION__, __LINE__, u32TotalSize);
        return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
    }

    u8TryTimes = 0;
    do
    {
        enErrCode = PARAM_ReadFile(FILE_NAME_TS_CONFIG, pu8Buf, u32TotalSize);
        if (WV_SUCCESS == enErrCode)
        {
            u32CRC = Common_CRCCalculate(pu8Buf, u32TotalSize, 0);
            if (0 != u32CRC)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]CRC error,u8TryTimes[%u]\r\n",
                    __FUNCTION__, __LINE__, u8TryTimes);
                
                u8TryTimes++;
                continue;
            }
            
            break;
        }
        
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "[%s:%d]PARAM_ReadFile error,u8TryTimes[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u8TryTimes, enErrCode);

        usleep(100000);
        
        u8TryTimes++;
    }while (u8TryTimes < 2);

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "u8TryTimes[%u]\r\n",u8TryTimes);

    if (u8TryTimes < 2)
    {
        stTSInfoInitNum.u32TSNumber = stTSIndex.u32TSNumber;
        stTSInfoInitNum.u32ProgramNumber = stTSIndex.u32ProgramNumber;
        stTSInfoInitNum.u32PIDNumber = stTSIndex.u32PIDNumber;
        stTSInfoInitNum.u32TableNumber = stTSIndex.u32TableNumber;
        stTSInfoInitNum.u32DescriptorNumber = stTSIndex.u32DescriptorNumber;

        stTSInfoInitNum.u32OutputTSNumber = stTSIndex.u32OutputTSNumber;
        stTSInfoInitNum.u32OutputProgramNumber = stTSIndex.u32OutputProgramNumber;
        stTSInfoInitNum.u32OutputPIDNumber = stTSIndex.u32OutputPIDNumber;
        stTSInfoInitNum.u32OutputTableNumber = stTSIndex.u32OutputTableNumber;
        stTSInfoInitNum.u32OutputDescriptorNumber = stTSIndex.u32OutputDescriptorNumber;
    }

    enErrCode = TSP_InitTSInfo(pstParamTS, &stTSInfoInitNum);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_InitTSInfo error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        memset(pstParamTS, 0, sizeof(TSInfo));
    }

    if (u8TryTimes < 2)
    {
        pu8TempBuf = pu8Buf + sizeof(TSIndex);
        
        memcpy(pstParamTS->pInputTS, pu8TempBuf, u32TSTotalSize);
        pu8TempBuf += u32TSTotalSize;

        memcpy(pstParamTS->pInputProgram, pu8TempBuf, u32ProgTotalSize);
        pu8TempBuf += u32ProgTotalSize;

        memcpy(pstParamTS->pInputPID, pu8TempBuf, u32PIDTotalSize);
        pu8TempBuf += u32PIDTotalSize;

        memcpy(pstParamTS->pInputTable, pu8TempBuf, u32TableTotalSize);
        pu8TempBuf += u32TableTotalSize;

        memcpy(pstParamTS->pInputDescriptor, pu8TempBuf, u32DescriptorTotalSize);
        pu8TempBuf += u32DescriptorTotalSize;

        memcpy(pstParamTS->pOutputTS, pu8TempBuf, u32OutputTSTotalSize);
        pu8TempBuf += u32OutputTSTotalSize;

        memcpy(pstParamTS->pOutputProgram, pu8TempBuf, u32OutputProgTotalSize);
        pu8TempBuf += u32OutputProgTotalSize;

        memcpy(pstParamTS->pOutputPID, pu8TempBuf, u32OutputPIDTotalSize);
        pu8TempBuf += u32OutputPIDTotalSize;

        memcpy(pstParamTS->pOutputTable, pu8TempBuf, u32OutputTableTotalSize);
        pu8TempBuf += u32OutputTableTotalSize;

        memcpy(pstParamTS->pOutputDescriptor, pu8TempBuf, u32OutputDescriptorTotalSize);
        pu8TempBuf += u32OutputDescriptorTotalSize;

        enErrCode = WV_SUCCESS;
    }

    free(pu8Buf);

    enErrCode = TSP_StoreTSConfiguration();
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_StoreTSConfiguration error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }
    
    return enErrCode;
}

/*****************************************************************************
  Function:     TSP_ClearTSConfiguration
  Description:  «Âø’ ‰»Î¡˜≈‰÷√
  Input:        pstParamTS
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ClearTSConfiguration(TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;
    U8 aru8DestSlotRecord[MAX_DEST] = {0};
    U8 u8DestSlot = 0;
    
    if (!pstParamTS)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "************Clear all TS Configuration************\r\n");

    TSP_ClearTSInfo(pstParamTS);

    memcpy(aru8DestSlotRecord, s_aru8DestSlotRecord, sizeof(aru8DestSlotRecord));

    memset(s_aru8DestSlotRecord, 0xFF, sizeof(s_aru8DestSlotRecord));

	//TODO
	/*
	enErrCode = TSP_SetInputLUT(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetInputLUT Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }
    */

    enErrCode = WVCI_UpdateSetting(ALL_SLOTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]WVCI_UpdateSetting Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }

    enErrCode = TSP_StoreTSConfiguration();
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_StoreTSConfiguration Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    for (i = 0; i < MAX_INPUT_CHANNEL_NUM; i++)
    {
        TSP_SetClearTSFlag(i, true);
    }

    for (i = 0; i < MAX_DEST; i++)
    {
        u8DestSlot = aru8DestSlotRecord[i];
        
        if (!TSP_IsLegalSlot(u8DestSlot))
        {
            continue;
        }

        enErrCode= IPComm_SendCmd2Slot(u8DestSlot, IPCOMM_SEND_CLEAR_INPUT_TS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]IPComm_SendCmd2Slot error,enErrCode[%u],u8DestSlot[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode);

            continue;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ClearDestSlotOfPID
  Description:  «Âø’“ª∏ˆPID»•∂‘”¶◊”∞Âµƒ»•œÚ
  Input:        u8DestSlot:“™«Âø’µƒ»•œÚ
                u16InPIDIndex: ‰»ÎPIDÀ˜“˝
                pstParamTS

  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ClearDestSlotOfPID(U8 u8DestSlot, U16 u16InPIDIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8DestIndex = 0;
    U16 i = 0;
    U16 u16ESECMIndex = INDEX_INVALID_VALUE;
    Input_TSPID *pInputPID = NULL;

    IndexInfoList stIndexListInfo;
    
    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InPIDIndex) || (MAX_SLOT_NUM <= u8DestSlot))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InPIDIndex[%u],u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InPIDIndex, u8DestSlot);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    pInputPID = &pstParamTS->pInputPID[u16InPIDIndex];

    enErrCode = TSP_GetDestIndex(u8DestSlot, pInputPID->aru8DestSlot, &u8DestIndex);
    if (WV_SUCCESS == enErrCode)
    {
        pInputPID->aru8DestSlot[u8DestIndex] = 0xFF;
        memset(pInputPID->aru32DestChannel[u8DestIndex], 0, sizeof(U32) * 8);
    }
    else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetDestIndex Error,u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, u8DestSlot);
        return enErrCode;
    }

    if (PID_TYPE_ESPID != pInputPID->u8PIDType)
    {
        return WV_SUCCESS;
    }

    stIndexListInfo.u16IndexListNum = 0;
    enErrCode = TSP_GetInPIDESECMPIDIndexList(u16InPIDIndex, pstParamTS, &stIndexListInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInPIDESECMPIDIndexList Error,u16InPIDIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16InPIDIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexListInfo.u16IndexListNum; i++)
    {
        u16ESECMIndex = stIndexListInfo.aru16IndexList[i];
        enErrCode = TSP_ClearDestSlotOfPID(u8DestSlot, u16ESECMIndex, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_ClearDestSlotOfPID Error,u16InPIDIndex[%u],u16ESECMIndex[%u][%u]\r\n",
                __FUNCTION__, __LINE__, u16InPIDIndex, i, u16ESECMIndex);
            return enErrCode;
        }
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ClearDestSlotOfProg
  Description:  «Âø’“ª∏ˆΩ⁄ƒø»•∂‘”¶◊”∞Âµƒ»•œÚ
  Input:        u8DestSlot:“™«Âø’µƒ»•œÚ
                u16InTSIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                pstParamTS

  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ClearDestSlotOfProg(U8 u8DestSlot, U16 u16InProgIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8DestIndex = 0;
    U16 i = 0;
    U16 u16InPIDIndex = INDEX_INVALID_VALUE;
    Input_TSProgram *pInputProg = NULL;

    IndexInfoList stIndexListInfo;
    
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex) || (MAX_SLOT_NUM <= u8DestSlot))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InProgIndex[%u],u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, u8DestSlot);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    pInputProg = &pstParamTS->pInputProgram[u16InProgIndex];

    enErrCode = TSP_GetDestIndex(u8DestSlot, pInputProg->aru8DestSlot, &u8DestIndex);
    if (WV_SUCCESS == enErrCode)
    {
        pInputProg->aru8DestSlot[u8DestIndex] = 0xFF;
        memset(pInputProg->aru32DestChannel[u8DestIndex], 0, sizeof(U32) * 8);
    }
    else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetDestIndex Error,u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, u8DestSlot);
        return enErrCode;
    }

    if (pstParamTS->u32InputPIDNumber > pInputProg->u16PCRIndex)
    {
        enErrCode = TSP_ClearDestSlotOfPID(u8DestSlot, pInputProg->u16PCRIndex, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_ClearDestSlotOfPID Error,u16InProgIndex[%u],u16InPIDIndex[%u]\r\n",
                __FUNCTION__, __LINE__, u16InProgIndex, pInputProg->u16PCRIndex);
            return enErrCode;
        }
    }

    stIndexListInfo.u16IndexListNum = 0;
    enErrCode = TSP_GetInProgESPIDIndexList(u16InProgIndex, pstParamTS, &stIndexListInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSProgIndexList Error,u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16InProgIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexListInfo.u16IndexListNum; i++)
    {
        u16InPIDIndex = stIndexListInfo.aru16IndexList[i];
        enErrCode = TSP_ClearDestSlotOfPID(u8DestSlot, u16InPIDIndex, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_ClearDestSlotOfPID Error,u16InProgIndex[%u],u16InPIDIndex[%u][%u]\r\n",
                __FUNCTION__, __LINE__, u16InProgIndex, i, u16InPIDIndex);
            return enErrCode;
        }
    }

    stIndexListInfo.u16IndexListNum = 0;
    enErrCode = TSP_GetInProgPSECMPIDIndexList(u16InProgIndex, pstParamTS, &stIndexListInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInProgPSECMPIDIndexList Error,u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16InProgIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexListInfo.u16IndexListNum; i++)
    {
        u16InPIDIndex = stIndexListInfo.aru16IndexList[i];
        enErrCode = TSP_ClearDestSlotOfPID(u8DestSlot, u16InPIDIndex, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_ClearDestSlotOfPID Error,u16InProgIndex[%u],u16InPIDIndex[%u][%u]\r\n",
                __FUNCTION__, __LINE__, u16InProgIndex, i, u16InPIDIndex);
            return enErrCode;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ClearDestSlotOfTS
  Description:  «Âø’“ª∏ˆ¡˜»•∂‘”¶◊”∞Âµƒ»•œÚ
  Input:        u8DestSlot:“™«Âø’µƒ»•œÚ
                u16InTSIndex: ‰»Î¡˜À˜“˝
                pstParamTS
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ClearDestSlotOfTS(U8 u8DestSlot, U16 u16InTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8DestIndex = 0;
    U16 i = 0;
    U16 u16Index = INDEX_INVALID_VALUE;
    Input_TSStream *pInputTS = NULL;

    IndexInfoList stIndexListInfo;
    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (MAX_SLOT_NUM <= u8DestSlot))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InTSIndex[%u],u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, u8DestSlot);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    pInputTS = &pstParamTS->pInputTS[u16InTSIndex];

    enErrCode = TSP_GetDestIndex(u8DestSlot, pInputTS->aru8DestSlot, &u8DestIndex);
    if (WV_SUCCESS == enErrCode)
    {
        pInputTS->aru8DestSlot[u8DestIndex] = 0xFF;
        memset(pInputTS->aru32DestChannel[u8DestIndex], 0, sizeof(U32) * 8);
        memset(pInputTS->aru32BypassFlag[u8DestIndex], 0, sizeof(U32) * 8);
    }
    else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetDestIndex Error,u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, u8DestSlot);
        return enErrCode;
    }

    stIndexListInfo.u16IndexListNum = 0;
    enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stIndexListInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSProgIndexList Error,u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16InTSIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexListInfo.u16IndexListNum; i++)
    {
        u16Index = stIndexListInfo.aru16IndexList[i];
        enErrCode = TSP_ClearDestSlotOfProg(u8DestSlot, u16Index, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_ClearDestSlotOfProg Error,u16InTSIndex[%u],u16Index[%u][%u]\r\n",
                __FUNCTION__, __LINE__, u16InTSIndex, i, u16Index);
            return enErrCode;
        }
    }

    stIndexListInfo.u16IndexListNum = 0;
    enErrCode = TSP_GetInTSEMMPIDIndexList(u16InTSIndex, pstParamTS, &stIndexListInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSEMMPIDIndexList Error,u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16InTSIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexListInfo.u16IndexListNum; i++)
    {
        u16Index = stIndexListInfo.aru16IndexList[i];
        enErrCode = TSP_ClearDestSlotOfPID(u8DestSlot, u16Index, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_ClearDestSlotOfPID Error,u16InTSIndex[%u],u16Index[%u][%u]\r\n",
                __FUNCTION__, __LINE__, u16InTSIndex, i, u16Index);
            return enErrCode;
        }
    }

    stIndexListInfo.u16IndexListNum = 0;
    enErrCode = TSP_GetInTSOtherPIDIndexList(u16InTSIndex, pstParamTS, &stIndexListInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSOtherPIDIndexList Error,u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16InTSIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexListInfo.u16IndexListNum; i++)
    {
        u16Index = stIndexListInfo.aru16IndexList[i];
        enErrCode = TSP_ClearDestSlotOfPID(u8DestSlot, u16Index, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_ClearDestSlotOfPID Error,u16InTSIndex[%u],u16Index[%u][%u]\r\n",
                __FUNCTION__, __LINE__, u16InTSIndex, i, u16Index);
            return enErrCode;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ProcessClearDestSlot
  Description:  «Âø’À˘”–µƒ¡˜»•∂‘”¶◊”∞Âµƒ»•œÚ
  Input:        u8DestSlot
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ProcessClearDestSlot(U8 u8DestSlot)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    TSInfo *pstParamTS = NULL;
    U16 u16InTSIndex = INDEX_INVALID_VALUE;
    U8 u8DestIndex = 0;
    
    if (MAX_SLOT_NUM <= u8DestSlot)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, u8DestSlot);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "TSP_ProcessClearDestSlot:u8DestSlot[%u]\r\n", u8DestSlot);
    
    pstParamTS = TSP_GetTSParamHandle();

    TSP_MutexLock();
    
    for (i = 0; i < MAX_INPUT_CHANNEL_NUM; i++)
    {
        enErrCode = TSP_GetInputTSIndex(i, pstParamTS, &u16InTSIndex);
        if (WV_ERR_TSP_NOT_EXIST == enErrCode)
        {
            continue;
        }
        else if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetInputTSIndex Error:enErrCode[%08X],Channel[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, i);

            TSP_MutexUnlock();

            return enErrCode;
        }

        enErrCode = TSP_ClearDestSlotOfTS(u8DestSlot, u16InTSIndex, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_ClearDestSlotOfTS Error:enErrCode[%08X],Channel[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, i);

            TSP_MutexUnlock();

            return enErrCode;
        }
    }

    enErrCode = TSP_GetDestIndex(u8DestSlot, s_aru8DestSlotRecord, &u8DestIndex);
    if (WV_ERR_TSP_NOT_EXIST == enErrCode)
    {
        TSP_MutexUnlock();

        return WV_SUCCESS;
    }
    else if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetDestIndex Error,u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, u8DestSlot);

        TSP_MutexUnlock();

        return enErrCode;
    }

    s_aru8DestSlotRecord[u8DestIndex] = 0xFF;

	/*
    enErrCode = TSP_SetBypassAndMUXFlag(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetBypassAndMUXFlag error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }
    */

	/*
    enErrCode = TSP_SetInputLUT(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetInputLUT Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }
    */

    enErrCode = TSP_StoreTSConfiguration();
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_StoreTSConfiguration Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }

    TSP_MutexUnlock();
    return enErrCode;
}

/*****************************************************************************
  Function:     TSP_Init
  Description:  ≥ı ºªØ’˚∏ˆœµÕ≥µƒ¡˜–≈œ¢
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_Init(void)
{
    wvErrCode enErrCode = WV_SUCCESS;

    enErrCode = TSP_RestoreTSConfiguration();
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_InitTSInfo Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

#if 1
    U8 i = 0;
    U16 u16InTSIndex = 0;
    TSInfo *pstTS = TSP_GetTSParamHandle();
    for (i = 0; i < MAX_INPUT_CHANNEL_NUM; i++)
    {
        printf("\r\nChannel[%u]\r\n",i);
        enErrCode = TSP_GetInputTSIndex(i, pstTS, &u16InTSIndex);
        if (WV_ERR_TSP_NOT_EXIST == enErrCode)
        {
            printf("No TS\r\n");
            continue;
        }

        printf("Has TS,u16InTSIndex[%u]",u16InTSIndex);
        PrintInputTS(u16InTSIndex, pstTS);
    }
#endif

    enErrCode = TSP_InitTempTSInfo();
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_InitTempTSInfo Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ClearAllInputTSInfo
  Description:  «Âø’À˘”– ‰»Î¡˜Ω·ππµƒÀ˘”––≈œ¢
  Input:        pstParamTS:
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_ClearAllInputTSInfo(TSInfo *pstParamTS)
{
    if (!pstParamTS)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error\r\n", __FUNCTION__, __LINE__);
        return;
    }

    if ((0 != pstParamTS->u32InputTSNumber) && (NULL != pstParamTS->pInputTS))
    {
        memset(pstParamTS->pInputTS, 0, pstParamTS->u32InputTSNumber * sizeof(Input_TSStream));
    }

    if ((0 != pstParamTS->u32InputProgramNumber) && (NULL != pstParamTS->pInputProgram))
    {
        memset(pstParamTS->pInputProgram, 0, pstParamTS->u32InputProgramNumber * sizeof(Input_TSProgram));
    }

    if ((0 != pstParamTS->u32InputPIDNumber) && (NULL != pstParamTS->pInputPID))
    {
        memset(pstParamTS->pInputPID, 0, pstParamTS->u32InputPIDNumber * sizeof(Input_TSPID));
    }

    if ((0 != pstParamTS->u32InputTableNumber) && (NULL != pstParamTS->pInputTable))
    {
        memset(pstParamTS->pInputTable, 0, pstParamTS->u32InputTableNumber * sizeof(TSTable));
    }

    if ((0 != pstParamTS->u32InputDescriptorNumber) && (NULL != pstParamTS->pInputDescriptor))
    {
        memset(pstParamTS->pInputDescriptor, 0, pstParamTS->u32InputDescriptorNumber * sizeof(TSDescriptor));
    }
}

/*****************************************************************************
  Function:     TSP_InitInputTS
  Description:  ≥ı ºªØ ‰»Î¡˜
  Input:        u16InTSIndex: ‰»Î¡˜À˜“˝
                u16Channel:Õ®µ¿
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_InitInputTS(U16 u16InTSIndex, U16 u16Channel, TSInfo *pstParamTS)
{
    Input_TSStream *pstInTS = NULL;

    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex);
        return;
    }

    pstInTS = &pstParamTS->pInputTS[u16InTSIndex];

    memset(pstInTS, 0, sizeof(Input_TSStream));

    pstInTS->u32StructVersion = TSP_GetStructVersion();
    pstInTS->u8ValidFlag = TSP_FLAG_VALID;
    pstInTS->u8SlotID = Status_GetSlotID();
    pstInTS->u16ChannelID = u16Channel;
    pstInTS->u16Index = u16InTSIndex;
    pstInTS->u16ProgramIndex = INDEX_INVALID_VALUE;
    pstInTS->u16OtherPIDIndex = INDEX_INVALID_VALUE;
    pstInTS->u16EMMPIDIndex = INDEX_INVALID_VALUE;

    pstInTS->u16NITIndex = INDEX_INVALID_VALUE;
    pstInTS->u16BATIndex = INDEX_INVALID_VALUE;
    pstInTS->u16TDTIndex = INDEX_INVALID_VALUE;
    pstInTS->u16TOTIndex = INDEX_INVALID_VALUE;
    pstInTS->u16OtherNITIndex = INDEX_INVALID_VALUE;
    pstInTS->u16OtherSDTIndex = INDEX_INVALID_VALUE;
    pstInTS->u16BackupTSIndex = INDEX_INVALID_VALUE;

    memset(pstInTS->aru8DestSlot, 0xFF, sizeof(pstInTS->aru8DestSlot));
    
    return;
}

/*****************************************************************************
  Function:     TSP_InitInputProgram
  Description:  ≥ı ºªØ ‰»ÎΩ⁄ƒø
  Input:        u16InTSIndex:Ω⁄ƒøÀ˘ Ù”⁄µƒ¡˜µƒÀ˜“˝
                u16InProgIndex:Ω⁄ƒøÀ˜“˝
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_InitInputProgram(U16 u16InTSIndex, U16 u16InProgIndex, TSInfo *pstParamTS)
{
    Input_TSProgram *pstTSProg = NULL;

    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex)
       || (pstParamTS->u32InputProgramNumber <= u16InProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, u16InProgIndex);
        
        return;
    }

    pstTSProg = &pstParamTS->pInputProgram[u16InProgIndex];

    memset(pstTSProg, 0, sizeof(Input_TSProgram));

    pstTSProg->u32StructVersion = TSP_GetStructVersion();
    pstTSProg->u8ValidFlag = TSP_FLAG_VALID;
    pstTSProg->u8SlotID = pstParamTS->pInputTS[u16InTSIndex].u8SlotID;
    pstTSProg->u16ChannelID = pstParamTS->pInputTS[u16InTSIndex].u16ChannelID;
    pstTSProg->u16Index = u16InProgIndex;
    pstTSProg->u16NextIndex = INDEX_INVALID_VALUE;
    pstTSProg->u16TSIndex = u16InTSIndex;
    pstTSProg->u16PSECMPIDIndex = INDEX_INVALID_VALUE;
    pstTSProg->u16PCRIndex = INDEX_INVALID_VALUE;
    pstTSProg->u16PIDIndex = INDEX_INVALID_VALUE;
    pstTSProg->u8CAMIndex = INVALID_CAM_INDEX;
    pstTSProg->u16ProgramInfoDescriptorIndex = INDEX_INVALID_VALUE;
    pstTSProg->u16SDTInfoDescriptorIndex = INDEX_INVALID_VALUE;
    pstTSProg->u16BackupProgramIndex = INDEX_INVALID_VALUE;

    memset(pstTSProg->aru8DestSlot, 0xFF, sizeof(pstTSProg->aru8DestSlot));

    return;
}

/*****************************************************************************
  Function:     TSP_InitInputPID
  Description:  ≥ı ºªØ ‰»ÎPID
  Input:        u8PIDType:PID¿‡–Õ
                    0:UNKNOWN, 1:PCRPID, 2:ESPID, 3:EMMPID, 4:PSECMPID, 5:ESECMPID, 6:OtherPID

                u16SrcIndex:PIDÀ˘ Ùµƒ¡˜/Ω⁄ƒø/PIDµƒÀ˜“˝
                    PID¿‡–ÕŒ™ EMMPID/OtherPID ±:Œ™¡˜À˜“˝
                    PID¿‡–ÕŒ™ PCRPID/ESPID/PSECMPID ±:Œ™Ω⁄ƒøÀ˜“˝
                    PID¿‡–ÕŒ™ ESECMPID ±:Œ™PIDÀ˜“˝
                    
                u16InTSIndex:PIDÀ˜“˝
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_InitInputPID(U8 u8PIDType, U16 u16SrcIndex, U16 u16InPIDIndex, TSInfo *pstParamTS)
{
    BOOL bInputErrorFlag = false;
    Input_TSPID *pstPID = NULL;

    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InPIDIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InPIDIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InPIDIndex);
        return;
    }

    pstPID = &pstParamTS->pInputPID[u16InPIDIndex];
    memset(pstPID, 0, sizeof(Input_TSPID));

    switch (u8PIDType)
    {
        case PID_TYPE_PCRPID:
        case PID_TYPE_ESPID:
        case PID_TYPE_PS_ECMPID:
            if (pstParamTS->u32InputProgramNumber <= u16SrcIndex)
            {
                bInputErrorFlag = true;
            }
            else
            {
                pstPID->u8SlotID = pstParamTS->pInputProgram[u16SrcIndex].u8SlotID;
                pstPID->u16ChannelID = pstParamTS->pInputProgram[u16SrcIndex].u16ChannelID;
                
                pstPID->u16TSIndex = pstParamTS->pInputProgram[u16SrcIndex].u16TSIndex;
                pstPID->u16ProgramIndex = u16SrcIndex;
                pstPID->u16PIDIndex = INDEX_INVALID_VALUE;
            }
            break;

        case PID_TYPE_EMMPID:
        case PID_TYPE_OTHERPID:
            if (pstParamTS->u32InputTSNumber <= u16SrcIndex)
            {
                bInputErrorFlag = true;
            }
            else
            {
                pstPID->u8SlotID = pstParamTS->pInputTS[u16SrcIndex].u8SlotID;
                pstPID->u16ChannelID = pstParamTS->pInputTS[u16SrcIndex].u16ChannelID;
                
                pstPID->u16TSIndex = u16SrcIndex;
                pstPID->u16ProgramIndex = INDEX_INVALID_VALUE;
                pstPID->u16PIDIndex = INDEX_INVALID_VALUE;
            }
            break;

        case PID_TYPE_ES_ECMPID:
            if (pstParamTS->u32InputPIDNumber <= u16SrcIndex)
            {
                bInputErrorFlag = true;
            }
            else
            {
                pstPID->u8SlotID = pstParamTS->pInputPID[u16SrcIndex].u8SlotID;
                pstPID->u16ChannelID = pstParamTS->pInputPID[u16SrcIndex].u16ChannelID;
                
                pstPID->u16TSIndex = pstParamTS->pInputPID[u16SrcIndex].u16TSIndex;
                pstPID->u16ProgramIndex = pstParamTS->pInputPID[u16SrcIndex].u16ProgramIndex;
                pstPID->u16PIDIndex = pstParamTS->pInputPID[u16SrcIndex].u16Index;
            }
            break;

        default:
            bInputErrorFlag = true;
            break;
    }

    if (bInputErrorFlag)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:u8PIDType[%u],u16SrcIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u8PIDType, u16SrcIndex);
        return;
    }

    pstPID->u32StructVersion = TSP_GetStructVersion();
    pstPID->u8ValidFlag = TSP_FLAG_VALID;
    pstPID->u16Index = u16InPIDIndex;
    pstPID->u16NextIndex = INDEX_INVALID_VALUE;
    pstPID->u8PIDType = u8PIDType;
    pstPID->u16ESECMPIDIndex = INDEX_INVALID_VALUE;
    pstPID->u16ESInfoDescriptorIndex = INDEX_INVALID_VALUE;
    pstPID->u16BackupPIDIndex = INDEX_INVALID_VALUE;

    memset(pstPID->aru8DestSlot, 0xFF, sizeof(pstPID->aru8DestSlot));
    
    return;
}

/*****************************************************************************
  Function:     TSP_InitInputTable
  Description:  ≥ı ºªØ ‰»Î±Ì
  Input:        u16InTSIndex:±ÌÀ˘ Ù”⁄µƒ¡˜µƒÀ˜“˝
                u16TableIndex:±ÌÀ˜“˝
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_InitInputTable(U16 u16InTSIndex, U16 u16TableIndex, TSInfo *pstParamTS)
{
    TSTable *pstTable = NULL;

    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex)
        || (pstParamTS->u32InputTableNumber <= u16TableIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],u16TableIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, u16TableIndex);
        return;
    }

    pstTable = &pstParamTS->pInputTable[u16TableIndex];
    memset(pstTable, 0, sizeof(TSTable));

    pstTable->u32StructVersion = TSP_GetStructVersion();
    pstTable->u8ValidFlag = TSP_FLAG_VALID;
    pstTable->u8SlotID = pstParamTS->pInputTS[u16InTSIndex].u8SlotID;
    pstTable->u16ChannelID = pstParamTS->pInputTS[u16InTSIndex].u16ChannelID;;
    pstTable->u16Index = u16TableIndex;
    pstTable->u16NextIndex = INDEX_INVALID_VALUE;
    pstTable->u16TSIndex = u16InTSIndex;

    return;
}

/*****************************************************************************
  Function:     TSP_InitOutputPID
  Description:  ”√ ‰»ÎPID≥ı ºªØ“ª∏ˆ ‰≥ˆPID£¨’‚∏ˆPID «“™∑¢ÀÕµΩ ‰≥ˆ∞Âµƒ
  Input:        pstOutPID: ‰≥ˆPID
                pstInPID: ‰»ÎPID
  Output:       
  Return:       SUCESS ≥…π¶
                Others ‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_InitOutputPID(Output_TSPID *pstOutPID, Input_TSPID *pstInPID)
{
    if ((!pstOutPID) || (!pstInPID))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstOutPID[%p],pstInPID[%p]\r\n",
            __FUNCTION__, __LINE__, pstOutPID, pstInPID);
        return;
    }
    
    pstOutPID->u8SourceSlotID = Status_GetSlotID();
    pstOutPID->u16SourceChannelID = pstInPID->u16ChannelID;
    pstOutPID->u16ProgramID = pstInPID->u16ProgramIndex;
    pstOutPID->u8PIDType = pstInPID->u8PIDType;
    pstOutPID->u8StreamType = pstInPID->u8StreamType;
    pstOutPID->u16OldPID = pstInPID->u16PID;
    pstOutPID->u16PID = pstInPID->u16PID;
    pstOutPID->u16CASID = pstInPID->u16CASID;
    pstOutPID->u16ESInfoLength = pstInPID->u16ESInfoLength;
    memcpy(pstOutPID->aru8ESInfo, pstInPID->aru8ESInfo, MAX_DESCRIPTOR_LENGTH);

    pstOutPID->u16SrcStreamID = TSP_InputChannel2StreamID(pstInPID->u16ChannelID);
    pstOutPID->u16SrcCIPID = pstInPID->u16NewPID;
}

/******************************************************************************
 * CheckAC3Desc() -
 * DESCRIPTION: -
 *     ªÒ»°Stream «∑Ò”–AC3◊÷∑˚√Ë ˆ
 * Input  : StreamType:StreamType
 * Output :
 * Returns: ”–∑µªÿ1,Œﬁ∑µªÿ0
 *
 * modification history
 * --------------------
 * 23 Mar 2011, zhimin.tang written
 * --------------------
 ******************************************************************************/
U32 TSP_CheckAC3Desc(U8* pDesc, U32 Length)
{
    U32 nESInfoLen = 0;
    U32 nHadParseDescLen = 0;
    U8* ptemp;
    //U32 nDescriptorTag = 0;
    U32 nDescriptorLen = 0;
    U8 ucDescLen = 0;
    U8 ucDescLenIsZeroTimes = 0;

    // CHECK PARAMS
    if((NULL==pDesc)||(0==Length))
    {
        printf("[CheckAC3Desc]WRONG ARGUMENT!\r\n");
        return 0;
    }
    nESInfoLen = Length;
    ptemp = pDesc;

    while(nHadParseDescLen<nESInfoLen)
    {
        ptemp = pDesc+nHadParseDescLen;
        //nDescriptorTag = *ptemp;
        ucDescLen = *(ptemp+1);
        if(0==ucDescLen)
        {
            ucDescLenIsZeroTimes++;
            if(ucDescLenIsZeroTimes>3)
            {
                break;
            }
        }
        switch(*ptemp)
        {
            case AC3_DESCRIPTOR_TAG:
            {
                return 1;
            }
            case ENHANCED_AC3_DESCRIPTOR_TAG:
            {
                return 1;
            }
            default:
            {
                //nDescriptorTag = *(ptemp);
                ptemp = ptemp + 1;
                nDescriptorLen = *(ptemp);
                ptemp = ptemp + 1;

                nHadParseDescLen = nHadParseDescLen + 2;
                nHadParseDescLen = nHadParseDescLen + nDescriptorLen;
            }
            break;
        }
    }

    return 0;
}

/*****************************************************************************
  Function:     TSP_CheckDRADesc
  Description:
  Input:
  Output:       none
  Return:       SMP_SUCCESS
  Others:
  Create:       ¡ı¡¡ 20160712
*****************************************************************************/
BOOL TSP_CheckDRADesc(U8* pDesc, U32 Length)
{
    U32 nESInfoLen = 0;
    U32 nHadParseDescLen = 0;
    U8* ptemp;
    U32 nDescriptorLen = 0;
    U8 ucDescLen = 0;
    U8 ucDescLenIsZeroTimes = 0;

    // CHECK PARAMS
    if((NULL==pDesc)||(0==Length))
    {
        printf("[TSP_CheckDRADesc]WRONG ARGUMENT!\r\n");
        return 0;
    }
    nESInfoLen = Length;
    ptemp = pDesc;

    while(nHadParseDescLen<nESInfoLen)
    {
        ptemp = pDesc+nHadParseDescLen;
        ucDescLen = *(ptemp+1);
        if(0==ucDescLen)
        {
            ucDescLenIsZeroTimes++;
            if(ucDescLenIsZeroTimes>3)
            {
                break;
            }
        }
        switch(*ptemp)
        {
            case DRA_DESCRIPTOR_TAG:
            {
                return 1;
            }

            default:
            {
                //nDescriptorTag = *(ptemp);
                ptemp = ptemp + 1;
                nDescriptorLen = *(ptemp);
                ptemp = ptemp + 1;

                nHadParseDescLen = nHadParseDescLen + 2;
                nHadParseDescLen = nHadParseDescLen + nDescriptorLen;
            }
            break;
        }
    }

    return 0;
}

/*****************************************************************************
  Function:     web_GetPIDType
  Description:  ªÒ»° ‰»ÎPIDµƒ¿‡–Õ(“Ù∆µªÚ ”∆µ)
  Input:        u16PIDIndex:PIDÀ˜“˝
                pstParamTS
  Output:       
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
U8 TSP_GetPIDType(U16 u16PIDIndex, TSInfo *pstParamTS)
{
    U8 u8StreamType = 0;
    U8 *pu8ESInfo = NULL;
    U16 u16ESInfoLen = 0;
    
    if((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16PIDIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16PIDIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16PIDIndex);
        return PRIVATE_DATA_PID;
    }
    
    u8StreamType = pstParamTS->pInputPID[u16PIDIndex].u8StreamType;
    
    if((u8StreamType == STREAM_TYPE_VIDEO)
        ||(u8StreamType == STREAM_TYPE_VIDEO1)
        ||(u8StreamType == STREAM_TYPE_VIDEO_H264)
        ||(u8StreamType == STREAM_TYPE_VIDEO_H265)
        ||(u8StreamType == STREAM_TYPE_VIDEO_AVS)
        ||(u8StreamType == STREAM_TYPE_VIDEO_ATSC)
        ||(u8StreamType == STREAM_TYPE_VIDEO_MPEG4)
        ||(u8StreamType == 0x1E))
    {
        return VIDEO_PID;
    }
    else if((u8StreamType == STREAM_TYPE_AUDIO)
        ||(u8StreamType == STREAM_TYPE_AUDIO1)
        ||(u8StreamType == STREAM_TYPE_AUDIO_AC3)
        ||(u8StreamType == STREAM_TYPE_MPEG2_AAC)
        ||(u8StreamType == STREAM_TYPE_MPEG4_AAC)
        ||(u8StreamType == STREAM_TYPE_DOLBY_TRUEHD)
        ||(u8StreamType == STREAM_TYPE_AC3_PLAS)
        ||(u8StreamType == STREAM_TYPE_AC3_PLUS_SEC)
        ||(u8StreamType ==STREAM_TYPE_DVB_AUDIO_AC3))
    {
        return AUDIO_PID;
    }
    else if (u8StreamType == STREAM_TYPE_PRIVATE)
    {
        pu8ESInfo = pstParamTS->pInputPID[u16PIDIndex].aru8ESInfo;
        u16ESInfoLen = pstParamTS->pInputPID[u16PIDIndex].u16ESInfoLength;
        
         if (TSP_CheckAC3Desc(pu8ESInfo, u16ESInfoLen)
            || TSP_CheckDRADesc(pu8ESInfo, u16ESInfoLen))
         {
            return AUDIO_PID;
         }
    }

    return PRIVATE_DATA_PID;
}

/*****************************************************************************
  Function:     TSP_SetStreamIDBeforeGetPIDBitrate
  Description:  …Ë÷√¡˜ID£¨”√”⁄ªÒ»°PID¬Î¬ 
  Input:        u16Channel:
  Output:       
  Return:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_SetStreamIDBeforeGetPIDBitrate(U16 u16Channel)
{
    U16 u16StreamId = 0;
    U32 u32Value = 0;

    u16StreamId = TSP_InputChannel2StreamID(u16Channel);

    FPGA_REG_Read(STR_MMNT_RSID, &u32Value);

    if ((U16)u32Value != u16StreamId)
    {
        FPGA_REG_Write(STR_MMNT_RSID, u16StreamId);
        sleep(2);
    }
}

/*****************************************************************************
  Function:     TSP_GetPIDBitRate
  Description:  ªÒ»°PIDµƒ¬Î¬ £¨ ±÷”∆µ¬ ∫Õ¡˜ID±ÿ–Î ¬œ»…Ë÷√∫√(‘⁄ªÒ»°’˚∏ˆ¡˜ªÚΩ⁄ƒøµƒÀ˘”–PID ±£¨Ω⁄ ° ±º‰)
  Input:        u16PID:
  Output:       
  Return:       PID BitRate
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
U32 TSP_GetPIDBitRate(U16 u16PID)
{
    return FPGA_GetPIDBitRate(u16PID);
}

/*****************************************************************************
  Function:     TSP_InitOutputProg
  Description:  ”√ ‰»ÎΩ⁄ƒø≥ı ºªØ“ª∏ˆ ‰≥ˆΩ⁄ƒø£¨’‚∏ˆΩ⁄ƒø «“™∑¢ÀÕµΩ ‰≥ˆ∞Âµƒ
  Input:        pstOutProg: ‰≥ˆΩ⁄ƒø
                pstInProg: ‰»ÎΩ⁄ƒø
  Output:       
  Return:       SUCESS ≥…π¶
                Others ‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_InitOutputProg(Output_TSProgram *pstOutProg, Input_TSProgram *pstInProg)
{
    if ((!pstOutProg) || (!pstInProg))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstOutProg[%p],pstInProg[%p]\r\n",
            __FUNCTION__, __LINE__, pstOutProg, pstInProg);
        return;
    }

    pstOutProg->u8SourceSlotID = Status_GetSlotID();
    pstOutProg->u16SourceChannelID = pstInProg->u16ChannelID;
    pstOutProg->u16ProgramID = pstInProg->u16Index;
    pstOutProg->u16OldServiceID = pstInProg->u16ServiceID;
    pstOutProg->u16OldPCRPID = pstInProg->u16PCRPID;
    pstOutProg->u16OldPMTPID = pstInProg->u16PMTPID;
    
    pstOutProg->u8ServiceNameLen = pstInProg->u8ServiceNameLen;
    memcpy(pstOutProg->aru8ServiceName, pstInProg->aru8ServiceName, MAX_SDT_NAME_LENGTH);

    pstOutProg->u8ServiceProviderLen = pstInProg->u8ServiceProviderLen;
    memcpy(pstOutProg->aru8ServiceProvider, pstInProg->aru8ServiceProvider, MAX_SDT_NAME_LENGTH);

    pstOutProg->u8ServiceType = pstInProg->u8ServiceType;
    pstOutProg->u8RunningStatus = pstInProg->u8RunningStatus;
    pstOutProg->u8FreeCAMode = pstInProg->u8FreeCAMode;
    pstOutProg->u8EITScheduleFlag = pstInProg->u8EITScheduleFlag;
    pstOutProg->u8EITPresentFollowingFlag = pstInProg->u8EITPresentFollowingFlag;
    pstOutProg->u8CAMIndex = pstInProg->u8CAMIndex;
    pstOutProg->u8CIFlag = (pstInProg->u8CAMIndex < WVCI_MAX_SLOT_NUM) ? 1 : 0;
    
    pstOutProg->u16ProgramInfoLength = pstInProg->u16ProgramInfoLength;
    memcpy(pstOutProg->aru8ProgramInfo, pstInProg->aru8ProgramInfo, MAX_DESCRIPTOR_LENGTH);
    
    pstOutProg->u16SDTInfoLength = pstInProg->u16SDTInfoLength;
    memcpy(pstOutProg->aru8SDTInfo, pstInProg->aru8SDTInfo, MAX_DESCRIPTOR_LENGTH);
}

/*****************************************************************************
  Function:     TSP_GetUnusedInputTSIndexList
  Description:  —∞’“ø…”√µƒ ‰»Î¡˜µƒÀ˜“˝¡–±Ì
  Input:        u32NeedNum:œ£Õ˚ªÒ»°µΩµƒø…”√∏ˆ ˝
                pstParamTS
  Output:       pstIndexList:ø…”√µƒ¡˜À˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedInputTSIndexList(IndexInfoList *pstIndexList,
                                    TSInfo *pstParamTS,
                                    U32 u32NeedNum)
{
    U32 i = 0;

    if ((!pstIndexList) || (!pstParamTS) || (u32NeedNum > MAX_INDEX_LIST_NUMBER))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u32NeedNum[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u32NeedNum);

        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == u32NeedNum)
    {
        return WV_SUCCESS;
    }

    pstIndexList->u16IndexListNum = 0;

    for (i = 0; i < pstParamTS->u32InputTSNumber; i++)
    {
        if (TSP_FLAG_VALID != pstParamTS->pInputTS[i].u8ValidFlag)
        {
            pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum] = i;
            pstIndexList->u16IndexListNum += 1;

            if (pstIndexList->u16IndexListNum == u32NeedNum)
            {
                return WV_SUCCESS;
            }
        }
    }

    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
        "[%s:%d]Null TS not enough\r\n", __FUNCTION__, __LINE__);

    return WV_ERR_TSP_RESOURCE_NOT_ENOUGH;
}

/*****************************************************************************
  Function:     TSP_GetUnusedInputProgIndexList
  Description:  —∞’“ø…”√µƒ ‰»ÎΩ⁄ƒøµƒÀ˜“˝¡–±Ì
  Input:        u32NeedNum:œ£Õ˚ªÒ»°µΩµƒø…”√∏ˆ ˝
                pstParamTS
  Output:       pstIndexList:ø…”√µƒΩ⁄ƒøÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedInputProgIndexList(IndexInfoList *pstIndexList,
                                    TSInfo *pstParamTS,
                                    U32 u32NeedNum)
{
    U32 i = 0;
    Input_TSProgram *pstTempProgList = NULL;

    if ((!pstIndexList) || (!pstParamTS) || (u32NeedNum > MAX_INDEX_LIST_NUMBER))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u32NeedNum[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u32NeedNum);

        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == u32NeedNum)
    {
        return WV_SUCCESS;
    }

    pstIndexList->u16IndexListNum = 0;

    for (i = 0; i < pstParamTS->u32InputProgramNumber; i++)
    {
        if (TSP_FLAG_VALID != pstParamTS->pInputProgram[i].u8ValidFlag)
        {
            pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum] = i;
            pstIndexList->u16IndexListNum += 1;

            if (pstIndexList->u16IndexListNum == u32NeedNum)
            {
                return WV_SUCCESS;
            }
        }
    }

    if (pstIndexList->u16IndexListNum < u32NeedNum)
    {
        pstTempProgList = pstParamTS->pInputProgram;
            
        pstTempProgList = realloc(pstTempProgList,
           (pstParamTS->u32InputProgramNumber + ALLOCATE_INPUT_PROG_NUM_PER_TIME) * sizeof(Input_TSProgram));

        if (!pstTempProgList)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]realloc() fail\r\n", __FUNCTION__, __LINE__);
            
            return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
        }

        pstParamTS->pInputProgram = pstTempProgList;

        memset(&pstParamTS->pInputProgram[pstParamTS->u32InputProgramNumber],
                0,
                ALLOCATE_INPUT_PROG_NUM_PER_TIME * sizeof(Input_TSProgram));
        pstParamTS->u32InputProgramNumber += ALLOCATE_INPUT_PROG_NUM_PER_TIME;

        return TSP_GetUnusedInputProgIndexList(pstIndexList, pstParamTS, u32NeedNum);
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetUnusedInputPIDIndexList
  Description:  —∞’“ø…”√µƒ ‰»ÎPIDµƒÀ˜“˝¡–±Ì
  Input:        u32NeedNum:œ£Õ˚ªÒ»°µΩµƒø…”√∏ˆ ˝
                pstParamTS
  Output:       pstIndexList:ø…”√µƒPIDÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedInputPIDIndexList(IndexInfoList *pstIndexList,
                                    TSInfo *pstParamTS,
                                    U32 u32NeedNum)
{
    U32 i = 0;
    Input_TSPID *pstTempPIDList = NULL;

    if ((!pstIndexList) || (!pstParamTS) || (u32NeedNum > MAX_INDEX_LIST_NUMBER))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u32NeedNum[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u32NeedNum);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == u32NeedNum)
    {
        return WV_SUCCESS;
    }

    pstIndexList->u16IndexListNum = 0;

    for (i = 0; i < pstParamTS->u32InputPIDNumber; i++)
    {
        if (TSP_FLAG_VALID != pstParamTS->pInputPID[i].u8ValidFlag)
        {
            pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum] = i;
            pstIndexList->u16IndexListNum += 1;

            if (pstIndexList->u16IndexListNum == u32NeedNum)
            {
                return WV_SUCCESS;
            }
        }
    }

    if (pstIndexList->u16IndexListNum < u32NeedNum)
    {
        pstTempPIDList = pstParamTS->pInputPID;
            
        pstTempPIDList = realloc(pstTempPIDList,
           (pstParamTS->u32InputPIDNumber + ALLOCATE_INPUT_PID_NUM_PER_TIME) * sizeof(Input_TSPID));

        if (!pstTempPIDList)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]realloc() fail\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
        }

        pstParamTS->pInputPID = pstTempPIDList;

        memset(&pstParamTS->pInputPID[pstParamTS->u32InputPIDNumber],
                0,
                ALLOCATE_INPUT_PID_NUM_PER_TIME * sizeof(Input_TSPID));
        pstParamTS->u32InputPIDNumber += ALLOCATE_INPUT_PID_NUM_PER_TIME;

        return TSP_GetUnusedInputPIDIndexList(pstIndexList, pstParamTS, u32NeedNum);
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetUnusedInputTableIndexList
  Description:  —∞’“ø…”√µƒ ‰»ÎTableµƒÀ˜“˝¡–±Ì
  Input:        u32NeedNum:œ£Õ˚ªÒ»°µΩµƒø…”√∏ˆ ˝
                pstParamTS
  Output:       pstIndexList:ø…”√µƒTableÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedInputTableIndexList(IndexInfoList *pstIndexList,
                                    TSInfo *pstParamTS,
                                    U32 u32NeedNum)
{
    U32 i = 0;
    TSTable *pstTempTableList = NULL;

    if ((!pstIndexList) || (!pstParamTS) || (u32NeedNum > MAX_INDEX_LIST_NUMBER))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u32NeedNum[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u32NeedNum);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == u32NeedNum)
    {
        return WV_SUCCESS;
    }

    pstIndexList->u16IndexListNum = 0;

    for (i = 0; i < pstParamTS->u32InputTableNumber; i++)
    {
        if (TSP_FLAG_VALID != pstParamTS->pInputTable[i].u8ValidFlag)
        {
            pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum] = i;
            pstIndexList->u16IndexListNum += 1;

            if (pstIndexList->u16IndexListNum == u32NeedNum)
            {
                return WV_SUCCESS;
            }
        }
    }

    if (pstIndexList->u16IndexListNum < u32NeedNum)
    {
        pstTempTableList = pstParamTS->pInputTable;
            
        pstTempTableList = realloc(pstTempTableList,
           (pstParamTS->u32InputTableNumber + ALLOCATE_INPUT_TABLE_NUM_PER_TIME) * sizeof(TSTable));

        if (!pstTempTableList)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]realloc() fail\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
        }

        pstParamTS->pInputTable = pstTempTableList;

        memset(&pstParamTS->pInputTable[pstParamTS->u32InputTableNumber],
                0,
                ALLOCATE_INPUT_TABLE_NUM_PER_TIME * sizeof(TSTable));
        pstParamTS->u32InputTableNumber += ALLOCATE_INPUT_TABLE_NUM_PER_TIME;

        return TSP_GetUnusedInputTableIndexList(pstIndexList, pstParamTS, u32NeedNum);
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetInTSProgIndexList
  Description:  ªÒ»° ‰»Î¡˜µƒÀ˘”–Ω⁄ƒø
  Input:        u16InTSIndex:ªÒ»°ƒƒ∏ˆ¡˜
                pstParamTS
  Output:       pstIndexList:À˘”–Ω⁄ƒøÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSProgIndexList(U16 u16InTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    if((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!pstIndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    pstIndexList->u16IndexListNum = 0;

    if (0 == pstParamTS->pInputTS[u16InTSIndex].u16ProgramNum)
    {
        return WV_SUCCESS;
    }

    u16TempIndex = pstParamTS->pInputTS[u16InTSIndex].u16ProgramIndex;

    while (u16TempIndex < pstParamTS->u32InputProgramNumber)
    {
        pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum++] = u16TempIndex;

        if (MAX_INDEX_LIST_NUMBER <= pstIndexList->u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:u16InTSIndex[%u],Too many Prog\r\n", __FUNCTION__, __LINE__, u16InTSIndex);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }
        
        u16TempIndex = pstParamTS->pInputProgram[u16TempIndex].u16NextIndex;
    }

    if (pstIndexList->u16IndexListNum != pstParamTS->pInputTS[u16InTSIndex].u16ProgramNum)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:u16InTSIndex[%u],u16IndexListNum[%u] != u16ProgramNum[%u]\r\n",
                __FUNCTION__,
                __LINE__,
                u16InTSIndex,
                pstIndexList->u16IndexListNum,
                pstParamTS->pInputTS[u16InTSIndex].u16ProgramNum);
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetInTSEMMPIDIndexList
  Description:  ªÒ»° ‰»Î¡˜µƒÀ˘”–EMMPID
  Input:        u16InTSIndex:ªÒ»°ƒƒ∏ˆ¡˜
                pstParamTS
  Output:       pstIndexList:À˘”–EMMPIDÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSEMMPIDIndexList(U16 u16InTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    if((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!pstIndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    pstIndexList->u16IndexListNum = 0;

    if (0 == pstParamTS->pInputTS[u16InTSIndex].u16EMMPIDNumber)
    {
        return WV_SUCCESS;
    }

    u16TempIndex = pstParamTS->pInputTS[u16InTSIndex].u16EMMPIDIndex;

    while (u16TempIndex < pstParamTS->u32InputPIDNumber)
    {
        pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum++] = u16TempIndex;

        if (MAX_INDEX_LIST_NUMBER <= pstIndexList->u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:Too many EMMPID\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }
        
        u16TempIndex = pstParamTS->pInputPID[u16TempIndex].u16NextIndex;
    }

    if (pstIndexList->u16IndexListNum != pstParamTS->pInputTS[u16InTSIndex].u16EMMPIDNumber)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:EMMPID Number Not the same,u16IndexListNum[%u],u16EMMPIDNumber[%u]\r\n",
                __FUNCTION__,
                __LINE__,
                pstIndexList->u16IndexListNum,
                pstParamTS->pInputTS[u16InTSIndex].u16EMMPIDNumber);
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetInTSOtherPIDIndexList
  Description:  ªÒ»° ‰»Î¡˜µƒÀ˘”–OtherPID
  Input:        u16InTSIndex:ªÒ»°ƒƒ∏ˆ¡˜
                pstParamTS
  Output:       pstIndexList:À˘”–OtherPIDÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSOtherPIDIndexList(U16 u16InTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    if((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!pstIndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    pstIndexList->u16IndexListNum = 0;

    if (0 == pstParamTS->pInputTS[u16InTSIndex].u16OtherPIDNum)
    {
        return WV_SUCCESS;
    }

    u16TempIndex = pstParamTS->pInputTS[u16InTSIndex].u16OtherPIDIndex;

    while (u16TempIndex < pstParamTS->u32InputPIDNumber)
    {
        pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum++] = u16TempIndex;

        if (MAX_INDEX_LIST_NUMBER <= pstIndexList->u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:Too many OtherPID\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }
        
        u16TempIndex = pstParamTS->pInputPID[u16TempIndex].u16NextIndex;
    }

    if (pstIndexList->u16IndexListNum != pstParamTS->pInputTS[u16InTSIndex].u16OtherPIDNum)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:OtherPID Number Not the same,u16IndexListNum[%u],u16OtherPIDNum[%u]\r\n",
                __FUNCTION__,
                __LINE__,
                pstIndexList->u16IndexListNum,
                pstParamTS->pInputTS[u16InTSIndex].u16OtherPIDNum);
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetInProgESPIDIndexList
  Description:  ªÒ»° ‰»ÎΩ⁄ƒøµƒÀ˘”–ESPID
  Input:        u16InProgIndex:ªÒ»°ƒƒ∏ˆΩ⁄ƒø
                pstParamTS
  Output:       pstIndexList:À˘”–ESPIDÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInProgESPIDIndexList(U16 u16InProgIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    if((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex) || (!pstIndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u16InProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    pstIndexList->u16IndexListNum = 0;

    if (0 == pstParamTS->pInputProgram[u16InProgIndex].u16ESNumber)
    {
        return WV_SUCCESS;
    }

    u16TempIndex = pstParamTS->pInputProgram[u16InProgIndex].u16PIDIndex;

    while (u16TempIndex < pstParamTS->u32InputPIDNumber)
    {
        pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum++] = u16TempIndex;

        if (MAX_INDEX_LIST_NUMBER <= pstIndexList->u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:Too many ESPID\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }
        
        u16TempIndex = pstParamTS->pInputPID[u16TempIndex].u16NextIndex;
    }

    if (pstIndexList->u16IndexListNum != pstParamTS->pInputProgram[u16InProgIndex].u16ESNumber)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:ESPID Number Not the same,u16IndexListNum[%u],u16ESNumber[%u]\r\n",
                __FUNCTION__,
                __LINE__,
                pstIndexList->u16IndexListNum,
                pstParamTS->pInputProgram[u16InProgIndex].u16ESNumber);
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetInProgPSECMPIDIndexList
  Description:  ªÒ»° ‰»ÎΩ⁄ƒøµƒÀ˘”–PSECM
  Input:        u16InProgIndex:ªÒ»°ƒƒ∏ˆΩ⁄ƒø
                pstParamTS
  Output:       pstIndexList:À˘”–PSECMÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInProgPSECMPIDIndexList(U16 u16InProgIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    if((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex) || (!pstIndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u16InProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    pstIndexList->u16IndexListNum = 0;

    if (0 == pstParamTS->pInputProgram[u16InProgIndex].u16PSECMPIDNumber)
    {
        return WV_SUCCESS;
    }

    u16TempIndex = pstParamTS->pInputProgram[u16InProgIndex].u16PSECMPIDIndex;

    while (u16TempIndex < pstParamTS->u32InputPIDNumber)
    {
        pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum++] = u16TempIndex;

        if (MAX_INDEX_LIST_NUMBER <= pstIndexList->u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:Too many ESPID\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }
        
        u16TempIndex = pstParamTS->pInputPID[u16TempIndex].u16NextIndex;
    }

    if (pstIndexList->u16IndexListNum != pstParamTS->pInputProgram[u16InProgIndex].u16PSECMPIDNumber)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:PSECMPID Number Not the same,u16IndexListNum[%u],u16PSECMPIDNumber[%u]\r\n",
                __FUNCTION__,
                __LINE__,
                pstIndexList->u16IndexListNum,
                pstParamTS->pInputProgram[u16InProgIndex].u16PSECMPIDNumber);
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetInPIDESECMPIDIndexList
  Description:  ªÒ»° ‰»ÎPIDµƒÀ˘”–ESECM
  Input:        u16InPIDIndex:ªÒ»°ƒƒ∏ˆPID
                pstParamTS
  Output:       pstIndexList:À˘”–ESECMÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInPIDESECMPIDIndexList(U16 u16InPIDIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    if((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InPIDIndex) || (!pstIndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u16InPIDIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u16InPIDIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    pstIndexList->u16IndexListNum = 0;

    if (0 == pstParamTS->pInputPID[u16InPIDIndex].u16ESECMPIDNumber)
    {
        return WV_SUCCESS;
    }

    u16TempIndex = pstParamTS->pInputPID[u16InPIDIndex].u16ESECMPIDIndex;

    while (u16TempIndex < pstParamTS->u32InputPIDNumber)
    {
        pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum++] = u16TempIndex;

        if (MAX_INDEX_LIST_NUMBER <= pstIndexList->u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:Too many ESECMPID\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }
        
        u16TempIndex = pstParamTS->pInputPID[u16TempIndex].u16NextIndex;
    }

    if (pstIndexList->u16IndexListNum != pstParamTS->pInputPID[u16InPIDIndex].u16ESECMPIDNumber)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:ESECMPID Number Not the same,u16IndexListNum[%u],u16ESECMPIDNumber[%u]\r\n",
                __FUNCTION__,
                __LINE__,
                pstIndexList->u16IndexListNum,
                pstParamTS->pInputPID[u16InPIDIndex].u16ESECMPIDNumber);
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetInTSLastEMMPIDIndex
  Description:  ªÒ»° ‰»Î¡˜µƒ◊Ó∫Û“ª∏ˆEMM
  Input:        u16InTSIndex:ªÒ»°ƒƒ∏ˆ¡˜
                pstParamTS
  Output:       pu16LastIndex:◊Ó∫Û“ª∏ˆEMMµƒÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚TS√ª”–EMM‘Ú(*pu16LastIndex)Œ™INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSLastEMMPIDIndex(U16 u16InTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexInfoList;

    if((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!pu16LastIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16LastIndex[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16LastIndex, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetInTSEMMPIDIndexList(u16InTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSEMMPIDIndexList Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (stIndexInfoList.u16IndexListNum == 0)
    {
        *pu16LastIndex = INDEX_INVALID_VALUE;
    }
    else
    {
        *pu16LastIndex = stIndexInfoList.aru16IndexList[stIndexInfoList.u16IndexListNum - 1];
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetInTSLastOtherPIDIndex
  Description:  ªÒ»° ‰»Î¡˜µƒ◊Ó∫Û“ª∏ˆOtherPID
  Input:        u16InTSIndex:ªÒ»°ƒƒ∏ˆ¡˜
                pstParamTS
  Output:       pu16LastIndex:◊Ó∫Û“ª∏ˆOtherPIDµƒÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚TS√ª”–OtherPID‘Ú(*pu16LastIndex)Œ™INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSLastOtherPIDIndex(U16 u16InTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexInfoList;

    if((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!pu16LastIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16LastIndex[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16LastIndex, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetInTSOtherPIDIndexList(u16InTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSOtherPIDIndexList Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (stIndexInfoList.u16IndexListNum == 0)
    {
        *pu16LastIndex = INDEX_INVALID_VALUE;
    }
    else
    {
        *pu16LastIndex = stIndexInfoList.aru16IndexList[stIndexInfoList.u16IndexListNum - 1];
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetInTSLastProgIndex
  Description:  ªÒ»° ‰»Î¡˜µƒ◊Ó∫Û“ª∏ˆΩ⁄ƒø
  Input:        u16InTSIndex:ªÒ»°ƒƒ∏ˆ¡˜
                pstParamTS
  Output:       pu16LastIndex:◊Ó∫Û“ª∏ˆΩ⁄ƒøµƒÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚TS√ª”–Ω⁄ƒø‘Ú(*pu16LastIndex)Œ™INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSLastProgIndex(U16 u16InTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexInfoList;

    if((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!pu16LastIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16LastIndex[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16LastIndex, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSProgIndexList Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (stIndexInfoList.u16IndexListNum == 0)
    {
        *pu16LastIndex = INDEX_INVALID_VALUE;
    }
    else
    {
        *pu16LastIndex = stIndexInfoList.aru16IndexList[stIndexInfoList.u16IndexListNum - 1];
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetInProgLastESPIDIndex
  Description:  ªÒ»° ‰»ÎΩ⁄ƒøµƒ◊Ó∫Û“ª∏ˆESPID
  Input:        u16InProgIndex:ªÒ»°ƒƒ∏ˆΩ⁄ƒø
                pstParamTS
  Output:       pu16LastIndex:◊Ó∫Û“ª∏ˆESPIDµƒÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚Ω⁄ƒø√ª”–ESPID‘Ú(*pu16LastIndex)Œ™INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInProgLastESPIDIndex(U16 u16InProgIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexInfoList;

    if((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex) || (!pu16LastIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16LastIndex[%p],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16LastIndex, u16InProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetInProgESPIDIndexList(u16InProgIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInProgESPIDIndexList Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (stIndexInfoList.u16IndexListNum == 0)
    {
        *pu16LastIndex = INDEX_INVALID_VALUE;
    }
    else
    {
        *pu16LastIndex = stIndexInfoList.aru16IndexList[stIndexInfoList.u16IndexListNum - 1];
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetInProgLastPSECMPIDIndex
  Description:  ªÒ»° ‰»ÎΩ⁄ƒøµƒ◊Ó∫Û“ª∏ˆPSECM
  Input:        u16InProgIndex:ªÒ»°ƒƒ∏ˆΩ⁄ƒø
                pstParamTS
  Output:       pu16LastIndex:◊Ó∫Û“ª∏ˆPSECMµƒÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚Ω⁄ƒø√ª”–PSECM‘Ú(*pu16LastIndex)Œ™INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInProgLastPSECMPIDIndex(U16 u16InProgIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexInfoList;

    if((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex) || (!pu16LastIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16LastIndex[%p],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16LastIndex, u16InProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetInProgPSECMPIDIndexList(u16InProgIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInProgPSECMPIDIndexList Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (stIndexInfoList.u16IndexListNum == 0)
    {
        *pu16LastIndex = INDEX_INVALID_VALUE;
    }
    else
    {
        *pu16LastIndex = stIndexInfoList.aru16IndexList[stIndexInfoList.u16IndexListNum - 1];
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetInPIDLastESECMPIDIndex
  Description:  ªÒ»° ‰»ÎPIDµƒ◊Ó∫Û“ª∏ˆESECM
  Input:        u16InPIDIndex:ªÒ»°ƒƒ∏ˆPID
                pstParamTS
  Output:       pu16LastIndex:◊Ó∫Û“ª∏ˆESECMµƒÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚PID√ª”–ESECM‘Ú(*pu16LastIndex)Œ™INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInPIDLastESECMPIDIndex(U16 u16InPIDIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexInfoList;

    if((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InPIDIndex) || (!pu16LastIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16LastIndex[%p],u16InPIDIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16LastIndex, u16InPIDIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetInPIDESECMPIDIndexList(u16InPIDIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInPIDESECMPIDIndexList Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (stIndexInfoList.u16IndexListNum == 0)
    {
        *pu16LastIndex = INDEX_INVALID_VALUE;
    }
    else
    {
        *pu16LastIndex = stIndexInfoList.aru16IndexList[stIndexInfoList.u16IndexListNum - 1];
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetInputTSIndex
  Description:  ªÒ»° ‰»Î¡˜µƒÀ˜“˝
  Input:        u16Channel: ‰»Î¡˜µƒChannel
                pstParamTS
  Output:       pu16InTSIndex:ªÒ»°µΩµƒ¡˜À˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInputTSIndex(U16 u16Channel, const TSInfo *pstParamTS, U16 *pu16InTSIndex)
{
    U32 i = 0;

    if ((!pstParamTS) || (!pu16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16InTSIndex[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    for (i = 0; i < pstParamTS->u32InputTSNumber; i++)
    {
        if ((TSP_FLAG_VALID == pstParamTS->pInputTS[i].u8ValidFlag)
            && (u16Channel == pstParamTS->pInputTS[i].u16ChannelID))
        {
            *pu16InTSIndex = i;
            return WV_SUCCESS;
        }
    }

    return WV_ERR_TSP_NOT_EXIST;
}

/*****************************************************************************
  Function:     TSP_GetInputProgIndex
  Description:  ªÒ»° ‰»ÎΩ⁄ƒøµƒÀ˜“˝
  Input:        u16InTSIndex: ‰»ÎΩ⁄ƒøÀ˘‘⁄µƒ¡˜µƒÀ˜“˝
                u16SeviceID:“™ªÒ»°µƒΩ⁄ƒøµƒServiceID
                pstParamTS
  Output:       pu16InProgIndex:ªÒ»°µΩµƒΩ⁄ƒøÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInputProgIndex(U16 u16InTSIndex, U16 u16SeviceID, const TSInfo *pstParamTS, U16 *pu16InProgIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16TempIndex = INDEX_INVALID_VALUE;
    IndexInfoList stProgIndexList;
    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!pu16InProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],pu16InTSIndex[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, pu16InProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    stProgIndexList.u16IndexListNum = 0;

    enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stProgIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSProgIndexList Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    for (i = 0; i < stProgIndexList.u16IndexListNum; i++)
    {
        u16TempIndex = stProgIndexList.aru16IndexList[i];

        if (u16SeviceID == pstParamTS->pInputProgram[u16TempIndex].u16ServiceID)
        {
            *pu16InProgIndex = u16TempIndex;
            return WV_SUCCESS;
        }
    }

    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Error:ServiceID[%u] not found in u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16SeviceID, u16InTSIndex);

    return WV_ERR_TSP_NOT_EXIST; 
}

/*****************************************************************************
  Function:     TSP_GetPrevPIDIndex
  Description:  ªÒ»° ‰»ÎPID‘⁄À˘‘⁄ ‰»Î¡˜ªÚΩ⁄ƒøªÚPID÷–µƒ«∞“ª∏ˆPIDµƒÀ˜“˝
  Input:        u16InPIDIndex: ‰»ÎPIDÀ˜“˝
                pstParamTS
  Output:       pu16PrevPIDIndex:ªÒ»°µΩµƒ«∞“ª∏ˆPIDÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetPrevPIDIndex(U16 u16InPIDIndex, const TSInfo *pstParamTS, U16 *pu16PrevPIDIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    IndexInfoList stIndexList;
    U8 u8PIDType = PID_TYPE_UNKNOWN;
    U16 u16SrcTSIndex = INDEX_INVALID_VALUE;
    U16 u16SrcProgIndex = INDEX_INVALID_VALUE;
    U16 u16SrcPIDIndex = INDEX_INVALID_VALUE;
    
    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InPIDIndex) || (!pu16PrevPIDIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InPIDIndex[%u],pu16PrevPIDIndex[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InPIDIndex, pu16PrevPIDIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    u8PIDType = pstParamTS->pInputPID[u16InPIDIndex].u8PIDType;
    u16SrcTSIndex = pstParamTS->pInputPID[u16InPIDIndex].u16TSIndex;
    u16SrcProgIndex = pstParamTS->pInputPID[u16InPIDIndex].u16ProgramIndex;
    u16SrcPIDIndex = pstParamTS->pInputPID[u16InPIDIndex].u16PIDIndex;
    
    stIndexList.u16IndexListNum = 0;
    
    switch (u8PIDType)
    {
        case PID_TYPE_ESPID:
        {
            enErrCode = TSP_GetInProgESPIDIndexList(u16SrcProgIndex, pstParamTS, &stIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetInProgESPIDIndexList error:u16SrcProgIndex[%u]\r\n",
                    __FUNCTION__, __LINE__, u16SrcProgIndex);
                return enErrCode;
            }
            break;
        }

        case PID_TYPE_PS_ECMPID:
        {
            enErrCode = TSP_GetInProgPSECMPIDIndexList(u16SrcProgIndex, pstParamTS, &stIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetInProgPSECMPIDIndexList error:u16SrcProgIndex[%u]\r\n",
                    __FUNCTION__, __LINE__, u16SrcProgIndex);
                return enErrCode;
            }
            break;
        }

        case PID_TYPE_ES_ECMPID:
        {
            enErrCode = TSP_GetInPIDESECMPIDIndexList(u16SrcPIDIndex, pstParamTS, &stIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetInPIDESECMPIDIndexList error:u16SrcPIDIndex[%u]\r\n",
                    __FUNCTION__, __LINE__, u16SrcPIDIndex);
                return enErrCode;
            }
            break;
        }

        case PID_TYPE_PCRPID:
        {
            return WV_ERR_TSP_NOT_EXIST;
        }

        case PID_TYPE_EMMPID:
        {
            enErrCode = TSP_GetInTSEMMPIDIndexList(u16SrcTSIndex, pstParamTS, &stIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetInTSEMMPIDIndexList error:u16SrcTSIndex[%u]\r\n",
                    __FUNCTION__, __LINE__, u16SrcTSIndex);
                return enErrCode;
            }
            break;
        }

        case PID_TYPE_OTHERPID:
        {
            enErrCode = TSP_GetInTSOtherPIDIndexList(u16SrcTSIndex, pstParamTS, &stIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetInTSEMMPIDIndexList error:u16SrcTSIndex[%u]\r\n",
                    __FUNCTION__, __LINE__, u16SrcTSIndex);
                return enErrCode;
            }
            break;
        }

        default:
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error,unknown pid type:u16InPIDIndex[%u],u8PIDType[%u]\r\n",
                __FUNCTION__, __LINE__, u16InPIDIndex, u8PIDType);
            return WV_ERR_TSP_TS_ERROR;
        }
    }

    if (0 == stIndexList.u16IndexListNum)
    {
        return WV_ERR_TSP_TS_ERROR;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        if (u16InPIDIndex == stIndexList.aru16IndexList[i])
        {
            break;
        }
    }

    if (0 == i)
    {
        return WV_ERR_TSP_NOT_EXIST;
    }
    else if (i < stIndexList.u16IndexListNum)
    {
        *pu16PrevPIDIndex = stIndexList.aru16IndexList[i - 1];
    }
    else
    {
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS; 
}

/*****************************************************************************
  Function:     TSP_GetPrevProgIndex
  Description:  ªÒ»° ‰»ÎΩ⁄ƒø‘⁄À˘‘⁄ ‰»Î¡˜÷–µƒ«∞“ª∏ˆΩ⁄ƒøµƒÀ˜“˝
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                pstParamTS
  Output:       pu16PrevProgIndex:ªÒ»°µΩµƒ«∞“ª∏ˆΩ⁄ƒøÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚Ω⁄ƒø «∆‰À˘‘⁄¡˜÷–µƒµ⁄“ª∏ˆΩ⁄ƒø£¨‘Ú∑µªÿWV_ERR_TSP_NOT_EXIST
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetPrevProgIndex(U16 u16InProgIndex, const TSInfo *pstParamTS, U16 *pu16PrevProgIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16InTSIndex = INDEX_INVALID_VALUE;
    IndexInfoList stIndexList;
    
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex) || (!pu16PrevProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InProgIndex[%u],pu16PrevProgIndex[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, pu16PrevProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    u16InTSIndex = pstParamTS->pInputProgram[u16InProgIndex].u16TSIndex;

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSProgIndexList Error:u16InProgIndex[%u],u16InTSIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16InProgIndex, u16InTSIndex, enErrCode);
        return enErrCode;
    }

    if (0 == stIndexList.u16IndexListNum)
    {
        return WV_ERR_TSP_TS_ERROR;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        if (u16InProgIndex == stIndexList.aru16IndexList[i])
        {
            break;
        }
    }

    if (0 == i)
    {
        return WV_ERR_TSP_NOT_EXIST;
    }
    else if (i < stIndexList.u16IndexListNum)
    {
        *pu16PrevProgIndex = stIndexList.aru16IndexList[i - 1];
    }
    else
    {
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS; 
}

/*****************************************************************************
  Function:     TSP_GetVideoPIDIndex
  Description:  ªÒ»° ‰»ÎΩ⁄ƒøµƒ ”∆µPIDµƒÀ˜“˝
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                pstParamTS
  Output:       pu16VideoPIDIndex:ªÒ»°µΩµƒ ”∆µPIDÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚Ω⁄ƒø√ª”– ”∆µPID£¨‘Ú∑µªÿWV_ERR_TSP_NOT_EXIST
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetVideoPIDIndex(U16 u16InProgIndex, TSInfo *pstParamTS, U16 *pu16VideoPIDIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    IndexInfoList stPIDIndexList;
    U16 u16PIDIndex = INDEX_INVALID_VALUE;
    U8 u8PIDType = 0;
    
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex) || (!pu16VideoPIDIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InProgIndex[%u],pu16VideoPIDIndex[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, pu16VideoPIDIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    stPIDIndexList.u16IndexListNum = 0;

    enErrCode = TSP_GetInProgESPIDIndexList(u16InProgIndex, pstParamTS, &stPIDIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInProgESPIDIndexList Error:u16InProgIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16InProgIndex, enErrCode);
        return enErrCode;
    }

    for (i = 0; i < stPIDIndexList.u16IndexListNum; i++)
    {
        u16PIDIndex = stPIDIndexList.aru16IndexList[i];
        
        u8PIDType = TSP_GetPIDType(u16PIDIndex, pstParamTS);
        
        if (VIDEO_PID == u8PIDType)
        {
            *pu16VideoPIDIndex = u16PIDIndex;
            return WV_SUCCESS;
        }
    }

    return WV_ERR_TSP_NOT_EXIST;
}

/*****************************************************************************
  Function:     TSP_AddProgToInputTS
  Description:  ∞—“ª∏ˆΩ⁄ƒøÃÌº”µΩ ‰»Î¡˜÷–
  Input:        u16InTSIndex: ‰»Î¡˜µƒÀ˜“˝
                u16ProgIndex:Ω⁄ƒøÀ˜“˝
                pstParamTS
  Output:       
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddProgToInputTS(U16 u16InTSIndex, U16 u16ProgIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 u16LastIndex = INDEX_INVALID_VALUE;

    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex)
        || (pstParamTS->u32InputProgramNumber <= u16ProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],u16ProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, u16ProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetInTSLastProgIndex(u16InTSIndex, pstParamTS, &u16LastIndex);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSLastProgIndex Error:u32Ret[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (INDEX_INVALID_VALUE == u16LastIndex)
    {
        pstParamTS->pInputTS[u16InTSIndex].u16ProgramIndex = u16ProgIndex;
    }
    else
    {
        pstParamTS->pInputPID[u16LastIndex].u16NextIndex = u16ProgIndex;
    }
    
    pstParamTS->pInputTS[u16InTSIndex].u16ProgramNum++;
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_AddEMMPIDToInputTS
  Description:  ∞—“ª∏ˆEMMPIDÃÌº”µΩ ‰»Î¡˜÷–
  Input:        u16InTSIndex: ‰»Î¡˜µƒÀ˜“˝
                u16EMMPIDIndex:EMMPIDÀ˜“˝
                pstParamTS
  Output:       
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddEMMPIDToInputTS(U16 u16InTSIndex, U16 u16EMMPIDIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 u16LastIndex = INDEX_INVALID_VALUE;

    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex)
        || (pstParamTS->u32InputPIDNumber <= u16EMMPIDIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],u16EMMPIDIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, u16EMMPIDIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetInTSLastEMMPIDIndex(u16InTSIndex, pstParamTS, &u16LastIndex);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSLastEMMPIDIndex Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (INDEX_INVALID_VALUE == u16LastIndex)
    {
        pstParamTS->pInputTS[u16InTSIndex].u16EMMPIDIndex = u16EMMPIDIndex;
    }
    else
    {
        pstParamTS->pInputPID[u16LastIndex].u16NextIndex = u16EMMPIDIndex;
    }

    pstParamTS->pInputTS[u16InTSIndex].u16EMMPIDNumber++;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_AddOhterPIDToInputTS
  Description:  ∞—“ª∏ˆOhterPIDÃÌº”µΩ ‰»Î¡˜÷–
  Input:        u16InTSIndex: ‰»Î¡˜µƒÀ˜“˝
                u16OhterPIDIndex:OhterPIDÀ˜“˝
                pstParamTS
  Output:       
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddOhterPIDToInputTS(U16 u16InTSIndex, U16 u16OtherPIDIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 u16LastIndex = INDEX_INVALID_VALUE;

    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex)
        || (pstParamTS->u32InputPIDNumber <= u16OtherPIDIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],u16OtherPIDIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, u16OtherPIDIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetInTSLastOtherPIDIndex(u16InTSIndex, pstParamTS, &u16LastIndex);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSLastOtherPIDIndex Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (INDEX_INVALID_VALUE == u16LastIndex)
    {
        pstParamTS->pInputTS[u16InTSIndex].u16OtherPIDIndex = u16OtherPIDIndex;
    }
    else
    {
        pstParamTS->pInputPID[u16LastIndex].u16NextIndex = u16OtherPIDIndex;
    }

    pstParamTS->pInputTS[u16InTSIndex].u16OtherPIDNum++;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_DelInputPID
  Description:  …æ≥˝“ª∏ˆ ‰»ÎPID
  Input:        u16InPIDIndex:“™…æ≥˝µƒ ‰»ÎPIDµƒÀ˜“˝
                pstParamTS
  
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_DelInputPID(U16 u16InPIDIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    IndexInfoList stIndexInfoList;
    U16 u16Index = INDEX_INVALID_VALUE;
    U8 u8PIDType = PID_TYPE_UNKNOWN;
    U16 u16SrcTSIndex = INDEX_INVALID_VALUE;
    U16 u16SrcProgIndex = INDEX_INVALID_VALUE;
    U16 u16SrcPIDIndex = INDEX_INVALID_VALUE;
    U16 u16PrevPIDIndex = INDEX_INVALID_VALUE;
    
    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InPIDIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InPIDIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InPIDIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    u8PIDType = pstParamTS->pInputPID[u16InPIDIndex].u8PIDType;
    u16SrcTSIndex = pstParamTS->pInputPID[u16InPIDIndex].u16TSIndex;
    u16SrcProgIndex = pstParamTS->pInputPID[u16InPIDIndex].u16ProgramIndex;
    u16SrcPIDIndex = pstParamTS->pInputPID[u16InPIDIndex].u16PIDIndex;

    switch (u8PIDType)
    {
        case PID_TYPE_ESPID:
        {
            stIndexInfoList.u16IndexListNum = 0;
            enErrCode = TSP_GetInPIDESECMPIDIndexList(u16InPIDIndex, pstParamTS, &stIndexInfoList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetInPIDESECMPIDIndexList Error:u16InPIDIndex[%u],enErrCode[%08X]\r\n",
                    __FUNCTION__, __LINE__, u16InPIDIndex, enErrCode);
                return enErrCode;
            }

            for (i = 0; i < stIndexInfoList.u16IndexListNum; i++)
            {
                u16Index = stIndexInfoList.aru16IndexList[i];
            
                enErrCode = TSP_DelInputPID(u16Index, pstParamTS);
                if (WV_SUCCESS != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]TSP_DelInputPID Error:u16InPIDIndex[%u],ESECMPID[%u][%u],enErrCode[%08X]\r\n",
                        __FUNCTION__, __LINE__, u16InPIDIndex, i, u16Index, enErrCode);
                    return enErrCode;
                }
            }

            enErrCode = TSP_GetPrevPIDIndex(u16InPIDIndex, pstParamTS, &u16PrevPIDIndex);
            if (WV_SUCCESS == enErrCode)
            {
                pstParamTS->pInputPID[u16PrevPIDIndex].u16NextIndex
                    = pstParamTS->pInputPID[u16InPIDIndex].u16NextIndex;
            }
            else if (WV_ERR_TSP_NOT_EXIST == enErrCode)
            {
                pstParamTS->pInputProgram[u16SrcProgIndex].u16PIDIndex
                    = pstParamTS->pInputPID[u16InPIDIndex].u16NextIndex;
            }
            else
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetPrevPIDIndex Error:u16InPIDIndex[%u],enErrCode[%08X]\r\n",
                    __FUNCTION__, __LINE__, u16InPIDIndex, enErrCode);
                return enErrCode;
            }
            
            pstParamTS->pInputProgram[u16SrcProgIndex].u16ESNumber--;
            break;
        }

        case PID_TYPE_PS_ECMPID:
        {
            enErrCode = TSP_GetPrevPIDIndex(u16InPIDIndex, pstParamTS, &u16PrevPIDIndex);
            if (WV_SUCCESS == enErrCode)
            {
                pstParamTS->pInputPID[u16PrevPIDIndex].u16NextIndex
                    = pstParamTS->pInputPID[u16InPIDIndex].u16NextIndex;
            }
            else if (WV_ERR_TSP_NOT_EXIST == enErrCode)
            {
                pstParamTS->pInputProgram[u16SrcProgIndex].u16PSECMPIDIndex
                    = pstParamTS->pInputPID[u16InPIDIndex].u16NextIndex;
            }
            else
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetPrevPSECMPIDIndex Error:u16InPIDIndex[%u],enErrCode[%08X]\r\n",
                    __FUNCTION__, __LINE__, u16InPIDIndex, enErrCode);
                return enErrCode;
            }
            
            pstParamTS->pInputProgram[u16SrcProgIndex].u16PSECMPIDNumber--;
            break;
        }

        case PID_TYPE_ES_ECMPID:
        {
            enErrCode = TSP_GetPrevPIDIndex(u16InPIDIndex, pstParamTS, &u16PrevPIDIndex);
            if (WV_SUCCESS == enErrCode)
            {
                pstParamTS->pInputPID[u16PrevPIDIndex].u16NextIndex
                    = pstParamTS->pInputPID[u16InPIDIndex].u16NextIndex;
            }
            else if (WV_ERR_TSP_NOT_EXIST == enErrCode)
            {
                pstParamTS->pInputPID[u16SrcPIDIndex].u16ESECMPIDIndex
                    = pstParamTS->pInputPID[u16InPIDIndex].u16NextIndex;
            }
            else
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetPrevESECMPIDIndex Error:u16InPIDIndex[%u],enErrCode[%08X]\r\n",
                    __FUNCTION__, __LINE__, u16InPIDIndex, enErrCode);
                return enErrCode;
            }
            
            pstParamTS->pInputPID[u16SrcPIDIndex].u16ESECMPIDNumber--;
            break;
        }
        
        case PID_TYPE_PCRPID:
        {
            pstParamTS->pInputProgram[u16SrcProgIndex].u16PCRIndex = INDEX_INVALID_VALUE;
            pstParamTS->pInputProgram[u16SrcProgIndex].u16PCRPID = INVALID_PID_VALUE;
        }
        
        case PID_TYPE_EMMPID:
        {
            enErrCode = TSP_GetPrevPIDIndex(u16InPIDIndex, pstParamTS, &u16PrevPIDIndex);
            if (WV_SUCCESS == enErrCode)
            {
                pstParamTS->pInputPID[u16PrevPIDIndex].u16NextIndex
                    = pstParamTS->pInputPID[u16InPIDIndex].u16NextIndex;
            }
            else if (WV_ERR_TSP_NOT_EXIST == enErrCode)
            {
                pstParamTS->pInputTS[u16SrcTSIndex].u16EMMPIDIndex
                    = pstParamTS->pInputPID[u16InPIDIndex].u16NextIndex;
            }
            else
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetPrevPIDIndex Error:u16InPIDIndex[%u],enErrCode[%08X]\r\n",
                    __FUNCTION__, __LINE__, u16InPIDIndex, enErrCode);
                return enErrCode;
            }
            
            pstParamTS->pInputTS[u16SrcTSIndex].u16EMMPIDNumber--;
            break;
        }
        
        case PID_TYPE_OTHERPID:
        {
            enErrCode = TSP_GetPrevPIDIndex(u16InPIDIndex, pstParamTS, &u16PrevPIDIndex);
            if (WV_SUCCESS == enErrCode)
            {
                pstParamTS->pInputPID[u16PrevPIDIndex].u16NextIndex
                    = pstParamTS->pInputPID[u16InPIDIndex].u16NextIndex;
            }
            else if (WV_ERR_TSP_NOT_EXIST == enErrCode)
            {
                pstParamTS->pInputTS[u16SrcTSIndex].u16OtherPIDIndex
                    = pstParamTS->pInputPID[u16InPIDIndex].u16NextIndex;
            }
            else
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetPrevPIDIndex Error:u16InPIDIndex[%u],enErrCode[%08X]\r\n",
                    __FUNCTION__, __LINE__, u16InPIDIndex, enErrCode);
                return enErrCode;
            }
            
            pstParamTS->pInputTS[u16SrcTSIndex].u16OtherPIDNum--;
            break;    
        }

        default:
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]Error,unknown pid type:u16InPIDIndex[%u],u8PIDType[%u]\r\n",
                    __FUNCTION__, __LINE__, u16InPIDIndex, u8PIDType);
            return WV_ERR_TSP_TS_ERROR;
        }
    }

    memset(&pstParamTS->pInputPID[u16InPIDIndex], 0, sizeof(Input_TSPID));
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_DelInputProg
  Description:  …æ≥˝“ª∏ˆ ‰»ÎΩ⁄ƒø
  Input:        u16InTSIndex:“™…æ≥˝µƒ ‰»ÎΩ⁄ƒøµƒÀ˜“˝
                pstParamTS
  
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_DelInputProg(U16 u16InProgIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    IndexInfoList stIndexInfoList;
    U16 u16Index = INDEX_INVALID_VALUE;
    U16 u16InTSIndex = INDEX_INVALID_VALUE;
    U16 u16PrevProgIndex = INDEX_INVALID_VALUE;

    Dest stDest;

    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(&stDest, 0, sizeof(stDest));
    enErrCode = TSP_ProcessProgDest(u16InProgIndex, &stDest, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_ProcessProgDest Error:u16InProgIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16InProgIndex, enErrCode);
        return enErrCode;
    }

    u16Index = pstParamTS->pInputProgram[u16InProgIndex].u16PCRIndex;

    if (pstParamTS->u32InputPIDNumber <= u16Index)
    {
        enErrCode = TSP_DelInputPID(u16Index, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_DelInputPCRPID Error:u16Index[%u],enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, u16Index, enErrCode);
            return enErrCode;
        }
    }

    stIndexInfoList.u16IndexListNum = 0;
    enErrCode = TSP_GetInProgESPIDIndexList(u16InProgIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInProgESPIDIndexList Error:u16InProgIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16InProgIndex, enErrCode);
        return enErrCode;
    }

    for (i = 0; i < stIndexInfoList.u16IndexListNum; i++)
    {
        u16Index = stIndexInfoList.aru16IndexList[i];
    
        enErrCode = TSP_DelInputPID(u16Index, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_DelInputPID Error:u16InProgIndex[%u],ES[%u][%u],enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, u16InProgIndex, i, u16Index, enErrCode);
            return enErrCode;
        }
    }

    stIndexInfoList.u16IndexListNum = 0;
    enErrCode = TSP_GetInProgPSECMPIDIndexList(u16InProgIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInProgPSECMPIDIndexList Error:u16InProgIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16InProgIndex, enErrCode);
        return enErrCode;
    }

    for (i = 0; i < stIndexInfoList.u16IndexListNum; i++)
    {
        u16Index = stIndexInfoList.aru16IndexList[i];
    
        enErrCode = TSP_DelInputPID(u16Index, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_DelInputPID Error:u16InProgIndex[%u],PSECM[%u][%u],enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, u16InProgIndex, i, u16Index, enErrCode);
            return enErrCode;
        }
    }

    u16InTSIndex = pstParamTS->pInputProgram[u16InProgIndex].u16TSIndex;

    enErrCode = TSP_GetPrevProgIndex(u16InProgIndex, pstParamTS, &u16PrevProgIndex);
    if (WV_SUCCESS == enErrCode)
    {
        pstParamTS->pInputProgram[u16PrevProgIndex].u16NextIndex
            = pstParamTS->pInputProgram[u16InProgIndex].u16NextIndex;
    }
    else if (WV_ERR_TSP_NOT_EXIST == enErrCode)
    {
        pstParamTS->pInputTS[u16InTSIndex].u16ProgramIndex
            = pstParamTS->pInputProgram[u16InProgIndex].u16NextIndex;
    }
    else
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetPrevProgIndex Error:u16InProgIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16InProgIndex, enErrCode);
        return enErrCode;
    }
    
    pstParamTS->pInputTS[u16InTSIndex].u16ProgramNum--;
    memset(&pstParamTS->pInputProgram[u16InProgIndex], 0, sizeof(Input_TSProgram));

    return WV_SUCCESS;
}


/*****************************************************************************
  Function:     TSP_DelInputTS
  Description:  …æ≥˝“ª∏ˆ ‰»Î¡˜
  Input:        u16InTSIndex:“™…æ≥˝µƒ ‰»Î¡˜µƒÀ˜“˝
                pstParamTS
  
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_DelInputTS(U16 u16InTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    IndexInfoList stIndexInfoList;
    U16 u16Index = INDEX_INVALID_VALUE;
    Dest stDest;

    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(&stDest, 0, sizeof(stDest));
    enErrCode = TSP_ProcessTSDest(u16InTSIndex, &stDest, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_ProcessTSDest Error:u16InTSIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16InTSIndex, enErrCode);
        return enErrCode;
    }

    stIndexInfoList.u16IndexListNum = 0;
    enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSProgIndexList Error:u16InTSIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16InTSIndex, enErrCode);
        return enErrCode;
    }

    for (i = 0; i < stIndexInfoList.u16IndexListNum; i++)
    {
        u16Index = stIndexInfoList.aru16IndexList[i];
    
        enErrCode = TSP_DelInputProg(u16Index, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_DelInputProg Error:u16InTSIndex[%u],ProgNum[%u],Prog[%u][%u],enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, u16InTSIndex, stIndexInfoList.u16IndexListNum, i, u16Index, enErrCode);
            return enErrCode;
        }
    }

    stIndexInfoList.u16IndexListNum = 0;
    enErrCode = TSP_GetInTSEMMPIDIndexList(u16InTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSEMMPIDIndexList Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    for (i = 0; i < stIndexInfoList.u16IndexListNum; i++)
    {
        u16Index = stIndexInfoList.aru16IndexList[i];
    
        enErrCode = TSP_DelInputPID(u16Index, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_DelInputEMM Error:u16InTSIndex[%u],EMMNum[%u],EMM[%u][%u],enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, u16InTSIndex, stIndexInfoList.u16IndexListNum, i, u16Index, enErrCode);
            return enErrCode;
        }
    }

    stIndexInfoList.u16IndexListNum = 0;
    enErrCode = TSP_GetInTSOtherPIDIndexList(u16InTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSOtherPIDIndexList Error:u16InTSIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16InTSIndex, enErrCode);
        return enErrCode;
    }

    for (i = 0; i < stIndexInfoList.u16IndexListNum; i++)
    {
        u16Index = stIndexInfoList.aru16IndexList[i];
    
        enErrCode = TSP_DelInputPID(u16Index, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_DelInputOtherPID Error:u16InTSIndex[%u],OtherPIDNum[%u],OtherPID[%u][%u],enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, u16InTSIndex, stIndexInfoList.u16IndexListNum, i, u16Index, enErrCode);
            return enErrCode;
        }
    }

    memset(&pstParamTS->pInputTS[u16InTSIndex], 0, sizeof(Input_TSStream));
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_DelInputTSByChannel
  Description:  …æ≥˝“ª∏ˆ ‰»ÎÕ®µ¿µƒ ‰»Î¡˜
  Input:        u16Channel:Õ®µ¿∫≈
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_DelInputTSByChannel(U16 u16Channel)
{
    wvErrCode enErrCode = WV_SUCCESS;
    TSInfo *pstParamTS = NULL;
    U16 u16InTSIndex = INDEX_INVALID_VALUE;

    pstParamTS = TSP_GetTSParamHandle();

    TSP_MutexLock();

    enErrCode = TSP_GetInputTSIndex(u16Channel, pstParamTS, &u16InTSIndex);
    if (WV_ERR_TSP_NOT_EXIST == enErrCode)
    {
        TSP_MutexUnlock();
        return WV_SUCCESS;
    }
    else if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInputTSIndex Error,u16Channel[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16Channel, enErrCode);

        TSP_MutexUnlock();
        return enErrCode;
    }

    enErrCode = TSP_DelInputTS(u16InTSIndex, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_DelInputTS Error,u16Channel[%u],u16InTSIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16Channel, u16InTSIndex, enErrCode);

        TSP_MutexUnlock();
        return enErrCode;
    }
	/*

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
    */

    enErrCode = TSP_StoreTSConfiguration();
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_StoreTSConfiguration Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        TSP_MutexUnlock();
        return enErrCode;
    }
    
    TSP_MutexUnlock();
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_IsLegalSlot
  Description:  ≈–∂œ≤€Œª∫≈ «∑Ò «∫œ∑®µƒ
  Input:        u8Slot:“™≈–∂œµƒ≤€Œª∫≈
  Output:       
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IsLegalSlot(U8 u8Slot)
{
    return (u8Slot < MAX_SLOT_NUM);
}

/*****************************************************************************
  Function:     TSP_GetBoardName
  Description:  ∏˘æ›≤€Œª∫≈ªÒ»°◊”∞Âµƒ√˚◊÷
  Input:        u8Slot:≤€Œª∫≈
                u32BoardNameLen:¥Ê∑≈◊”∞Â√˚◊÷Bufferµƒ≥§∂»
  Output:       pu8BoardName:◊”∞Â√˚◊÷÷∏’Î
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetBoardName(U8 u8Slot, U8 *pu8BoardName, U32 u32BoardNameLen)
{
    if (!pu8BoardName)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pu8BoardName[%p]\r\n",
            __FUNCTION__, __LINE__, pu8BoardName);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (u32BoardNameLen > MAX_BOARD_NAME_LEN)
    {
        u32BoardNameLen = MAX_BOARD_NAME_LEN;
    }

    if (!TSP_IsLegalSlot(u8Slot))
    {
        memset(pu8BoardName, 0, u32BoardNameLen);
    }

    return Resource_getBoardDescriptor(u8Slot, pu8BoardName);
}

/*****************************************************************************
  Function:     TSP_GetDestInfo
  Description:  ªÒ»°À˘”–»•œÚ£¨Õ®π˝¡˜Ω·ππ÷–µƒ»•œÚ–≈œ¢£¨◊È◊∞≥…“ª∏ˆ»•œÚΩ·ππÃÂ£¨∑µªÿ∏¯Õ¯π‹ π”√
  Input:        aru8DestSlot:¡˜Ω·ππ÷–º«¬º»•¡Àƒƒ–©◊”∞Â
                aru32DestChannel:º«¬º¡À»•¡Àƒƒ–©Õ®µ¿
                aru32BypassFlag:º«¬º¡À∂‘”¶µƒÕ®µ¿ «∑Ò «÷±Õ®π˝»•µƒ
  Output:       pstDest:Ω⁄ƒøµƒÀ˘”–»•œÚ
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetDestInfo(U8 aru8DestSlot[], U32 aru32DestChannel[][8], U32 aru32BypassFlag[][8], Dest *pstDest)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;
    U8 u8DestIndex = 0;
    U8 u8DestSlot = 0;
    U8 aru8BoardName[MAX_BOARD_NAME_LEN] = {0};
    BOOL bHaveDestFlag = false;
    
    if ((!pstDest) || (!aru8DestSlot) || (!aru32DestChannel))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstDest[%p],aru8DestSlot[%p],aru32DestChannel[%p]\r\n",
            __FUNCTION__, __LINE__, pstDest, aru8DestSlot, aru32DestChannel);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(pstDest, 0, sizeof(Dest));

    for (u8DestIndex = 0; u8DestIndex < MAX_DEST; u8DestIndex++)
    {
        u8DestSlot = aru8DestSlot[u8DestIndex];
        if (!TSP_IsLegalSlot(u8DestSlot))
        {
            continue;
        }

        pstDest->arstDestSlotInfo[pstDest->u8DestSlotNum].u8Slot = u8DestSlot;

        bHaveDestFlag = false;

        for (i = 0; i < 8; i++)
        {
            pstDest->arstDestSlotInfo[pstDest->u8DestSlotNum].aru32Channel[i]
                = aru32DestChannel[u8DestIndex][i];

            if (aru32BypassFlag)
            {
                pstDest->arstDestSlotInfo[pstDest->u8DestSlotNum].aru32BypassFlag[i]
                    = aru32BypassFlag[u8DestIndex][i];
            }

            if (0 != aru32DestChannel[u8DestIndex][i])
            {
                bHaveDestFlag = true;
            }
        }

        if (!bHaveDestFlag)
        {
            continue;
        }

        enErrCode = TSP_GetBoardName(u8DestSlot, aru8BoardName, MAX_BOARD_NAME_LEN);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetBoardName Error:enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;
        }

        memcpy(pstDest->arstDestSlotInfo[pstDest->u8DestSlotNum].aru8BoardName,
                aru8BoardName,
                MAX_BOARD_NAME_LEN);
        
        pstDest->u8DestSlotNum++;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetProgDestInfo
  Description:  ªÒ»°Ω⁄ƒøµƒÀ˘”–»•œÚ
  Input:        u16InProgIndex:Ω⁄ƒøÀ˜“˝
                pstParamTS
  Output:       pstDest:Ω⁄ƒøµƒÀ˘”–»•œÚ
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetProgDestInfo(U16 u16InProgIndex, const TSInfo *pstParamTS, Dest *pstDest)
{
    wvErrCode enErrCode = WV_SUCCESS;
    
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex) || (!pstDest))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InProgIndex[%u],pstDest[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, pstDest);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(pstDest, 0, sizeof(Dest));

    enErrCode = TSP_GetDestInfo(pstParamTS->pInputProgram[u16InProgIndex].aru8DestSlot, 
                                pstParamTS->pInputProgram[u16InProgIndex].aru32DestChannel, 
                                NULL,
                                pstDest);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetDestInfo Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetTSDestInfo
  Description:  ªÒ»°¡˜µƒÀ˘”–»•œÚ
  Input:        u16InTSIndex:¡˜À˜“˝
                pstParamTS
  Output:       pstDest:¡˜µƒÀ˘”–»•œÚ
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetTSDestInfo(U16 u16InTSIndex, const TSInfo *pstParamTS, Dest *pstDest)
{
    wvErrCode enErrCode = WV_SUCCESS;
    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!pstDest))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],pstDest[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, pstDest);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(pstDest, 0, sizeof(Dest));

    enErrCode = TSP_GetDestInfo(pstParamTS->pInputTS[u16InTSIndex].aru8DestSlot, 
                                pstParamTS->pInputTS[u16InTSIndex].aru32DestChannel, 
                                pstParamTS->pInputTS[u16InTSIndex].aru32BypassFlag,
                                pstDest);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetDestInfo Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetDestSlotInfoInddex
  Description:  ≈–∂œ‘⁄À˘”–µƒ»•œÚ÷– «∑Ò”–∏¯∂®µƒ»•œÚ–≈œ¢
  Input:        pstDestInfo:∏¯∂®µƒ»•œÚ–≈œ¢
                pstDest:À˘”–µƒ»•œÚ
  Output:       
  Return:       WV_SUCCESS or ErrorCode
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetDestSlotInfoIndex(U8 u8DestSlot, Dest *pstDest, U8 *pu8DestSlotInfoIndex)
{
    U8 i = 0;
    
    if ((!pstDest) || (!pu8DestSlotInfoIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstDest[%p],pu8DestSlotInfoIndex[%p]\r\n",
            __FUNCTION__, __LINE__, pstDest, pu8DestSlotInfoIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    for (i = 0; i < pstDest->u8DestSlotNum; i++)
    {
        if (u8DestSlot == pstDest->arstDestSlotInfo[i].u8Slot)
        {
            *pu8DestSlotInfoIndex = i;
            return WV_SUCCESS;
        }
    }

    *pu8DestSlotInfoIndex = MAX_SLOT_NUM;

    return WV_ERR_TSP_NOT_EXIST;
}

/*****************************************************************************
  Function:     TSP_GetUnusedDestIndex
  Description:  ’““ª∏ˆ√ª”– π”√µƒDestSlotÀ˜“˝
  Input:        aru8DestSlot:∏¯∂®µƒDestSlot ˝◊È
  Output:       pu8UnusedDestIndex:Œ¥ π”√µƒDestSlotÀ˜“˝
  Return:       WV_SUCCESS or ErrorCode
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedDestIndex(U8 aru8DestSlot[], U8 *pu8UnusedDestIndex)
{
    U8 i = 0;

    if ((!aru8DestSlot) || (!pu8UnusedDestIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:aru8DestSlot[%p],pu8UnusedDestIndex[%p]\r\n",
            __FUNCTION__, __LINE__, aru8DestSlot, pu8UnusedDestIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    for (i = 0; i < MAX_DEST; i++)
    {
        if (MAX_SLOT_NUM <= aru8DestSlot[i])
        {
            *pu8UnusedDestIndex = i;
            return WV_SUCCESS;
        }
    }

    return WV_ERR_TSP_RESOURCE_NOT_ENOUGH;
}

/*****************************************************************************
  Function:     TSP_GetDestIndex
  Description:  ªÒ»°∂‘”¶»•œÚµƒÀ˜“˝
  Input:        u8Slot:»•œÚµƒ≤€Œª∫≈
                aru8DestSlot:∏¯∂®µƒDestSlot ˝◊È
  Output:       pu8DestIndex:»•œÚÀ˜“˝
  Return:       WV_SUCCESS or ErrorCode
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetDestIndex(U8 u8Slot, U8 aru8DestSlot[], U8 *pu8DestIndex)
{
    U8 i = 0;
    
    if ((!aru8DestSlot) || (!pu8DestIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:aru8DestSlot[%p],pu8DestIndex[%p]\r\n",
            __FUNCTION__, __LINE__, aru8DestSlot, pu8DestIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

	//FIXME
	//ÂÜôÊ≠ªdestlot
	aru8DestSlot[u8Slot] = u8Slot;
	
    for (i = 0; i < MAX_DEST; i++)
    {
        if (u8Slot == aru8DestSlot[i])
        {
            *pu8DestIndex = i;
            return WV_SUCCESS;
        }
    }

    *pu8DestIndex = MAX_DEST;

    return WV_ERR_TSP_NOT_EXIST;
}

/*****************************************************************************
  Function:     TSP_DelDestChannel
  Description:  …æ≥˝ ‰»Î»•œÚ–≈œ¢÷–µƒ“ª∏ˆÕ®µ¿
  Input:        u8Slot:“™…æ≥˝µƒÕ®µ¿∫≈À˘‘⁄µƒ≤€Œª∫≈
                u16Channel:“™…æ≥˝µƒÕ®µ¿∫≈
                aru8DestSlot: ‰»Îµƒ»•œÚ–≈œ¢£¨»•¡Àƒƒ–© ‰≥ˆ◊”∞Â
                aru32DestChannel: ‰»Îµƒ»•œÚ–≈œ¢£¨»•¡À∂‘”¶ ‰≥ˆ◊”∞Âµƒƒƒ–©Õ®µ¿
                aru32BypassFlag: ‰»Îµƒ»•œÚ–≈œ¢£¨ «∑Ò «÷±Õ®µΩ∂‘”¶µƒÕ®µ¿
  Output:       
  Return:       WV_SUCCESS or ErrorCode
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_DelDestChannel(U8 u8Slot,
                        U16 u16Channel,
                        U8 aru8DestSlot[],
                        U32 aru32DestChannel[][8],
                        U32 aru32BypassFlag[][8])
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;
    U8 u8DestIndex = 0;

    if ((!aru8DestSlot) || (!aru32DestChannel))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:aru8DestSlot[%p],aru32DestChannel[%p]\r\n",
            __FUNCTION__, __LINE__, aru8DestSlot, aru32DestChannel);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetDestIndex(u8Slot, aru8DestSlot, &u8DestIndex);
    if (WV_ERR_TSP_NOT_EXIST == enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]DestSlot(u8Slot[%u]) not found\r\n",
            __FUNCTION__, __LINE__, u8Slot);
        return WV_ERR_TSP_NOT_EXIST;
    }
    else if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetDestIndex Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }
    
    aru32DestChannel[u8DestIndex][u16Channel / 32] &= ~(0x1 << (u16Channel % 32));

    if (NULL != aru32BypassFlag)
    {
        aru32BypassFlag[u8DestIndex][u16Channel / 32] &= ~(0x1 << (u16Channel % 32));
    }

    for (i = 0; i < 8; i++)
    {
        if (0 != aru32DestChannel[u8DestIndex][i])
        {
            break;
        }
    }

    if (8 <= i)
    {
        aru8DestSlot[u8DestIndex] = MAX_SLOT_NUM;
        
        if (NULL != aru32BypassFlag)
        {
            memset(aru32BypassFlag[u8DestIndex], 0, 8 * sizeof(U32));
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_DelDestSlot
  Description:  …æ≥˝ ‰»Î»•œÚ–≈œ¢÷–µƒ“ª∏ˆ◊”∞ÂµƒÀ˘”–Õ®µ¿
  Input:        u8Slot:“™…æ≥˝µƒ◊”∞Âµƒ≤€Œª∫≈
                aru8DestSlot: ‰»Îµƒ»•œÚ–≈œ¢£¨»•¡Àƒƒ–© ‰≥ˆ◊”∞Â
                aru32DestChannel: ‰»Îµƒ»•œÚ–≈œ¢£¨»•¡À∂‘”¶ ‰≥ˆ◊”∞Âµƒƒƒ–©Õ®µ¿
                aru32BypassFlag: ‰»Îµƒ»•œÚ–≈œ¢£¨ «∑Ò «÷±Õ®µΩ∂‘”¶µƒÕ®µ¿
  Output:       
  Return:       WV_SUCCESS or ErrorCode
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_DelDestSlot(U8 u8Slot, U8 aru8DestSlot[], U32 aru32DestChannel[][8], U32 aru32BypassFlag[][8])
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8DestIndex = 0;
    
    if (!aru8DestSlot)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:aru8DestSlot[%p]\r\n",
            __FUNCTION__, __LINE__, aru8DestSlot);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetDestIndex(u8Slot, aru8DestSlot, &u8DestIndex);
    if (WV_ERR_TSP_NOT_EXIST == enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]DestSlot(u8Slot[%u]) not found\r\n",
            __FUNCTION__, __LINE__, u8Slot);
        return WV_ERR_TSP_NOT_EXIST;
    }
    else if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetDestIndex Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    aru8DestSlot[u8DestIndex] = MAX_SLOT_NUM;
    memset(aru32DestChannel[u8DestIndex], 0, 8 * sizeof(U32));

    if (aru32BypassFlag)
    {
        memset(aru32BypassFlag[u8DestIndex], 0, 8 * sizeof(U32));
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_AddDestChannel
  Description:  ‘ˆº”Õ®µ¿µΩ ‰»Î»•œÚ–≈œ¢÷–
  Input:        u8Slot:“™‘ˆº”µƒÕ®µ¿À˘‘⁄µƒ≤€Œª∫≈
                u16Channel:“™‘ˆº”µƒÕ®µ¿∫≈
                bBypassFlag:÷±Õ®±Í÷æ
                aru8DestSlot: ‰»Îµƒ»•œÚ–≈œ¢£¨»•¡Àƒƒ–© ‰≥ˆ◊”∞Â
                aru32DestChannel: ‰»Îµƒ»•œÚ–≈œ¢£¨»•¡À∂‘”¶ ‰≥ˆ◊”∞Âµƒƒƒ–©Õ®µ¿
                aru32BypassFlag: ‰»Îµƒ»•œÚ–≈œ¢£¨ «∑Ò «÷±Õ®µΩ∂‘”¶µƒÕ®µ¿
  Output:       
  Return:       WV_SUCCESS or ErrorCode
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddDestChannel(U8 u8Slot,
                        U8 u8Channel,
                        BOOL bBypassFlag,
                        U8 aru8DestSlot[],
                        U32 aru32DestChannel[][8],
                        U32 aru32BypassFlag[][8])
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8DestIndex = MAX_SLOT_NUM;
        
    if ((!aru8DestSlot) || (!aru32DestChannel))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:aru8DestSlot[%p],aru32DestChannel[%p]\r\n",
            __FUNCTION__, __LINE__, aru8DestSlot, aru32DestChannel);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetDestIndex(u8Slot, aru8DestSlot, &u8DestIndex);
    if (WV_ERR_TSP_NOT_EXIST == enErrCode)
    {
        enErrCode = TSP_GetUnusedDestIndex(aru8DestSlot, &u8DestIndex);
        if (WV_SUCCESS != enErrCode)
        {
          log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
              "[%s:%d]TSP_GetUnuseDestIndex Error:enErrCode[%08X]\r\n",
              __FUNCTION__, __LINE__, enErrCode);
          return enErrCode;
        }

        aru8DestSlot[u8DestIndex] = u8Slot;
    }
    else if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetDestIndex Error:enErrCode[%08X]\r\n", __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    aru32DestChannel[u8DestIndex][u8Channel / 32] |= 0x1 << (u8Channel % 32);
    if (aru32BypassFlag)
    {
        if (bBypassFlag)
        {
           aru32BypassFlag[u8DestIndex][u8Channel / 32] |= 0x1 << (u8Channel % 32);
        }
        else
        {
           aru32BypassFlag[u8DestIndex][u8Channel / 32] &= ~(0x1 << (u8Channel % 32));
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_AddDestSlot
  Description:  ‘ˆº”»•œÚ–≈œ¢÷–µƒÀ˘”–Õ®µ¿µΩ ‰»Î»•œÚ–≈œ¢÷–
  Input:        pstDestInfo:“™‘ˆº”µƒ‘ˆº”»•œÚ–≈œ¢
                aru8DestSlot: ‰»Îµƒ»•œÚ–≈œ¢£¨»•¡Àƒƒ–© ‰≥ˆ◊”∞Â
                aru32DestChannel: ‰»Îµƒ»•œÚ–≈œ¢£¨»•¡À∂‘”¶ ‰≥ˆ◊”∞Âµƒƒƒ–©Õ®µ¿
                aru32BypassFlag: ‰»Îµƒ»•œÚ–≈œ¢£¨ «∑Ò «÷±Õ®µΩ∂‘”¶µƒÕ®µ¿
  Output:       
  Return:       WV_SUCCESS or ErrorCode
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddDestSlot(DestInfo *pstDestInfo,
                        U8 aru8DestSlot[],
                        U32 aru32DestChannel[][8],
                        U32 aru32BypassFlag[][8])
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8DestIndex = MAX_SLOT_NUM;
        
    if ((!pstDestInfo) || (!aru8DestSlot) || (!aru32DestChannel))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstDestInfo[%p],aru8DestSlot[%p],aru8DestSlot[%p]\r\n",
            __FUNCTION__, __LINE__, pstDestInfo, aru8DestSlot, aru8DestSlot);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetDestIndex(pstDestInfo->u8Slot, aru8DestSlot, &u8DestIndex);
    if (WV_ERR_TSP_NOT_EXIST == enErrCode)
    {
        enErrCode = TSP_GetUnusedDestIndex(aru8DestSlot, &u8DestIndex);
        if (WV_SUCCESS != enErrCode)
        {
          log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
              "[%s:%d]TSP_GetUnuseDestIndex Error:enErrCode[%08X]\r\n",
              __FUNCTION__, __LINE__, enErrCode);
          return enErrCode;
        }
    }
    else if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetDestIndex Error:enErrCode[%08X]\r\n", __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    aru8DestSlot[u8DestIndex] = pstDestInfo->u8Slot;
    memcpy(aru32DestChannel[u8DestIndex], pstDestInfo->aru32Channel, 8 * sizeof(U32));

    if (aru32BypassFlag)
    {
        memcpy(aru32BypassFlag[u8DestIndex], pstDestInfo->aru32BypassFlag, 8 * sizeof(U32));
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_IfInputProgOutputs2DestChannel
  Description:  ≈–∂œ ‰»ÎΩ⁄ƒø «∑Ò ‰≥ˆµΩ÷∏∂®µƒ≤€Œª÷–÷∏∂®µƒÕ®µ¿
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøµƒÀ˜“˝
                u8DestSlot:÷∏∂®µƒ≤€Œª∫≈
                u16DestChannel:÷∏∂®µƒÕ®µ¿
                pstParamTS
  Output:       
  Return:       true or false
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IfInputProgOutputs2DestChannel(U16 u16InProgIndex, U8 u8DestSlot, U16 u16DestChannel, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U32 *pu32DestChannel = NULL;
    U8 u8DestIndex = MAX_DEST;
    
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex) || (!TSP_IsLegalSlot(u8DestSlot)))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InProgIndex[%u],u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, u8DestSlot);
        return false;
    }

    enErrCode = TSP_GetDestIndex(u8DestSlot,
                        pstParamTS->pInputProgram[u16InProgIndex].aru8DestSlot,
                        &u8DestIndex);
    if (WV_SUCCESS == enErrCode)
    {
        pu32DestChannel = pstParamTS->pInputProgram[u16InProgIndex].aru32DestChannel[u8DestIndex];
        
        if (0 != (pu32DestChannel[u16DestChannel / 32] & (0x1 << (u16DestChannel % 32))))
        {
            return true;
        }
    }
    else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetDestIndex error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }

    return false;
}

/*****************************************************************************
  Function:     TSP_IfInputTSOutputs2DestChannel
  Description:  ≈–∂œ ‰»Î¡˜ «∑Ò ‰≥ˆµΩ÷∏∂®µƒ≤€Œª÷–÷∏∂®µƒÕ®µ¿
  Input:        u16InTSIndex: ‰»Î¡˜µƒÀ˜“˝
                u8DestSlot:÷∏∂®µƒ≤€Œª∫≈
                u16DestChannel:÷∏∂®µƒÕ®µ¿
                pstParamTS
  Output:       
  Return:       true or false
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IfInputTSOutputs2DestChannel(U16 u16InTSIndex, U8 u8DestSlot, U16 u16DestChannel, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U32 *pu32DestChannel = NULL;
    U8 u8DestIndex = MAX_DEST;
    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!TSP_IsLegalSlot(u8DestSlot)))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, u8DestSlot);
        return false;
    }

    enErrCode = TSP_GetDestIndex(u8DestSlot,
                        pstParamTS->pInputTS[u16InTSIndex].aru8DestSlot,
                        &u8DestIndex);
    if (WV_SUCCESS == enErrCode)
    {
        pu32DestChannel = pstParamTS->pInputTS[u16InTSIndex].aru32DestChannel[u8DestIndex];
        
        if (0 != (pu32DestChannel[u16DestChannel / 32] & (0x1 << (u16DestChannel % 32))))
        {
            return true;
        }
    }
    else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetDestIndex error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }

    return false;
}

/*****************************************************************************
  Function:     TSP_IfInputProgOutputs2DestSlot
  Description:  ≈–∂œ ‰»ÎΩ⁄ƒø «∑Ò ‰≥ˆµΩ∂‘”¶u8DestSlotµƒ◊”∞Â
  Input:        u16InProgIndex:Ω⁄ƒøÀ˜“˝
                u8DestSlot: ‰≥ˆ◊”∞Â≤€Œª∫≈
                pstParamTS:
  Output:       
  Return:       true or false
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IfInputProgOutputs2DestSlot(U16 u16InProgIndex, TSInfo *pstParamTS, U8 u8DestSlot)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8DestIndex = MAX_DEST;
    
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex);
        return false;
    }
    
    enErrCode = TSP_GetDestIndex(u8DestSlot, pstParamTS->pInputProgram[u16InProgIndex].aru8DestSlot, &u8DestIndex);
    if (WV_SUCCESS == enErrCode)
    {
        return true;
    }
    
    return false;
}

/*****************************************************************************
  Function:     TSP_IfInputTSOutputs2DestSlot
  Description:  ≈–∂œ ‰»Î¡˜ «∑Ò ‰≥ˆ(∏¥”√ªÚ÷±Õ®)µΩ∂‘”¶u8DestSlotµƒ◊”∞Â
  Input:        u16InTSIndex: ‰»Î¡˜µƒÀ˜“˝
                u8DestSlot: ‰≥ˆ◊”∞Â≤€Œª∫≈
                pstParamTS:
  Output:       
  Return:       true or false
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IfInputTSOutputs2DestSlot(U16 u16InTSIndex, TSInfo *pstParamTS, U8 u8DestSlot)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8DestIndex = MAX_DEST;

    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex);
        return false;
    }

    enErrCode = TSP_GetDestIndex(u8DestSlot, pstParamTS->pInputTS[u16InTSIndex].aru8DestSlot, &u8DestIndex);
    if (WV_SUCCESS == enErrCode)
    {
        return true;
    }
    
    return false;
}

/*****************************************************************************
  Function:     TSP_IfInputTSBypasses2DestSlot
  Description:  ≈–∂œ ‰»ÎTS «∑Ò÷±Õ®µΩ∂‘”¶u8DestSlotµƒ◊”∞Â
  Input:        u16InTSIndex:TSÀ˜“˝
                u8DestSlot: ‰≥ˆ◊”∞Â≤€Œª∫≈
                pstParamTS:
  Output:       
  Return:       true or false
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IfInputTSBypasses2DestSlot(U16 u16InTSIndex, TSInfo *pstParamTS, U8 u8DestSlot)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U8 u8DestIndex = MAX_DEST;
    U32 *pu32DestChannel = NULL;
    U32 *pu32BypassFlag = NULL;
    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex)
        || (MAX_SLOT_NUM <= u8DestSlot))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, u8DestSlot);
        return false;
    }

    enErrCode = TSP_GetDestIndex(u8DestSlot, pstParamTS->pInputTS[u16InTSIndex].aru8DestSlot, &u8DestIndex);
    if (WV_SUCCESS != enErrCode)
    {
        return false;
    }

    pu32DestChannel = pstParamTS->pInputTS[u16InTSIndex].aru32DestChannel[u8DestIndex];
    pu32BypassFlag = pstParamTS->pInputTS[u16InTSIndex].aru32BypassFlag[u8DestIndex];

    for (i = 0; i < MAX_OUTPUT_CHANNEL_NUM; i++)
    {
        if ((0 != (pu32DestChannel[i / 32] & (0x1 << (i % 32))))
            && (0 != (pu32BypassFlag[i / 32] & (0x1 << (i % 32)))))
        {
            return true;
        }
    }
    
    return false;
}

/*****************************************************************************
  Function:     TSP_IfInputTSMuxes2DestSlot
  Description:  ≈–∂œ ‰»ÎTS «∑Ò∏¥”√µΩ∂‘”¶u8DestSlotµƒ◊”∞Â
  Input:        u16InTSIndex:TSÀ˜“˝
                u8DestSlot: ‰≥ˆ◊”∞Â≤€Œª∫≈
                pstParamTS:
  Output:       
  Return:       true or false
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IfInputTSMuxes2DestSlot(U16 u16InTSIndex, TSInfo *pstParamTS, U8 u8DestSlot)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U8 u8DestIndex = MAX_DEST;
    U32 *pu32DestChannel = NULL;
    U32 *pu32BypassFlag = NULL;
    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex)
        || (MAX_SLOT_NUM <= u8DestSlot))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, u8DestSlot);
        return false;
    }

    enErrCode = TSP_GetDestIndex(u8DestSlot, pstParamTS->pInputTS[u16InTSIndex].aru8DestSlot, &u8DestIndex);
    if (WV_SUCCESS != enErrCode)
    {
        return false;
    }

    pu32DestChannel = pstParamTS->pInputTS[u16InTSIndex].aru32DestChannel[u8DestIndex];
    pu32BypassFlag = pstParamTS->pInputTS[u16InTSIndex].aru32BypassFlag[u8DestIndex];

    for (i = 0; i < MAX_OUTPUT_CHANNEL_NUM; i++)
    {
        if ((0 != (pu32DestChannel[i / 32] & (0x1 << (i % 32))))
            && (0 == (pu32BypassFlag[i / 32] & (0x1 << (i % 32)))))
        {
            return true;
        }
    }
    
    return false;
}

/*****************************************************************************
  Function:     TSP_IsThereAnyPIDThatOutputs2DestSlot
  Description:  ≈–∂œ «∑Ò”–PID ‰≥ˆµΩ∂‘”¶u8DestSlotµƒ◊”∞Â
                u8DestSlot: ‰≥ˆ◊”∞Â≤€Œª∫≈
                pstParamTS:
  Output:       
  Return:       true or false
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IsThereAnyPIDThatOutputs2DestSlot(U8 u8DestSlot, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16Channel = 0;
    U16 u16InTSIndex = INDEX_INVALID_VALUE;

    IndexInfoList stIndexList;

    if ((!pstParamTS) || (MAX_SLOT_NUM <= u8DestSlot))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u8DestSlot);
        return false;
    }

    for (u16Channel = 0; u16Channel < MAX_INPUT_CHANNEL_NUM; u16Channel++)
    {
        enErrCode = TSP_GetInputTSIndex(u16Channel, pstParamTS, &u16InTSIndex);
        if (WV_ERR_TSP_NOT_EXIST == enErrCode)
        {
            continue;
        }
        else if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetDestInfo Error:enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return false;
        }

        if (TSP_IfInputTSOutputs2DestSlot(u16InTSIndex, pstParamTS, u8DestSlot))
        {
            return true;
        }
        
        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetInTSProgIndexList Error:enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return false;
        }

        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            if (TSP_IfInputProgOutputs2DestSlot(stIndexList.aru16IndexList[i], pstParamTS, u8DestSlot))
            {
                return true;
            }
        }
    }

    return false;
}

/*****************************************************************************
  Function:     TSP_SendDelProgInfoToOutputBoard
  Description:  ∑¢ÀÕ“™…æ≥˝µƒΩ⁄ƒøµΩ ‰≥ˆ∞Â
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøµƒÀ˜“˝
                pstParamTS
  Output:       pstDestInfo: ‰≥ˆ∞Â–≈œ¢
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SendDelProgInfoToOutputBoard(U16 u16InProgIndex, TSInfo *pstParamTS, DestInfo *pstDestInfo)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;
    U8 j = 0;
    
    U8 u8DestSlot = 0;
    U32 u32TempDestChannel = 0;

    U16 u16DelProgNum = 0;

    Delete_TSProgram *pstDelProg = NULL;

    U8 aru8Buf[1 + (256 * sizeof(Delete_TSProgram))]= {0};
    U32 u32BufLen = 0;

    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex) || (!pstDestInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InProgIndex[%u],pstDestInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, pstDestInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(aru8Buf, 0, sizeof(aru8Buf));
    pstDelProg = (Delete_TSProgram *)(&aru8Buf[1]);
    
    u8DestSlot = pstDestInfo->u8Slot;

    u16DelProgNum = 0;

    for (i = 0; i < 8; i++)
    {
        u32TempDestChannel = pstDestInfo->aru32Channel[i];

        if (0 == u32TempDestChannel)
        {
            continue;
        }

        for (j = 0; j < 32; j++)
        {
            if (0 == (u32TempDestChannel & (0x1 << j)))
            {
                continue;
            }

            pstDelProg[u16DelProgNum].u8SourceSlotID = Status_GetSlotID();
            pstDelProg[u16DelProgNum].u16SourceChannelID = pstParamTS->pInputProgram[u16InProgIndex].u16ChannelID;
            pstDelProg[u16DelProgNum].u16ProgramID = u16InProgIndex;

            pstDelProg[u16DelProgNum].u16ChannelID = i * 32 + j;

            u16DelProgNum++;
        }
    }

    if (0 == u16DelProgNum)
    {
        return WV_SUCCESS;
    }

    aru8Buf[0] = u16DelProgNum;
    u32BufLen = 1 + u16DelProgNum * sizeof(Delete_TSProgram);

    enErrCode = (wvErrCode)IPComm_SendData2Slot(u8DestSlot, IPCOMM_SEND_DEL_PROG, aru8Buf, u32BufLen);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendData2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    enErrCode = (wvErrCode)IPComm_SendCmd2Slot(u8DestSlot, IPCOMM_SEND_DONE_FLAG);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendCmd2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_DelOutputBoardProg
  Description:  ∞— ‰»ÎΩ⁄ƒø¥” ‰≥ˆ∞Â÷–…æ≥˝
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                pstParamTS:
                pstDestInfo: ‰≥ˆ∞Â–≈œ¢
  Output:       
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_DelOutputBoardProg(U16 u16InProgIndex, TSInfo *pstParamTS, Dest *pstDelDest)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;
    
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex) || (!pstDelDest))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InProgIndex[%u],pstDelDest[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, pstDelDest);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    for (i = 0; i < pstDelDest->u8DestSlotNum; i++)
    {
        enErrCode = TSP_SendDelProgInfoToOutputBoard(u16InProgIndex, pstParamTS, &pstDelDest->arstDestSlotInfo[i]);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_SendDelProgInfoToOutputBoard error\r\n",
                __FUNCTION__, __LINE__);
            return enErrCode;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SendDelTSInfoToOutputBoard
  Description:  ∑¢ÀÕ“™…æ≥˝µƒ¡˜µΩ ‰≥ˆ∞Â
  Input:        u16InTSIndex: ‰»Î¡˜À˜“˝
                pstParamTS:
                pstDestInfo: ‰≥ˆ∞Â–≈œ¢
  Output:       
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SendDelTSInfoToOutputBoard(U16 u16InTSIndex, TSInfo *pstParamTS, DestInfo *pstDestInfo)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;
    U8 j = 0;
    
    U8 u8DestSlot = 0;
    U32 u32TempDestChannel = 0;

    U16 u16DelTSNum = 0;

    Delete_TSStream *pstDelTS = NULL;

    U8 aru8Buf[1 + (256 * sizeof(Delete_TSStream))]= {0};
    U32 u32BufLen = 0;

    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!pstDestInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],pstDestInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, pstDestInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(aru8Buf, 0, sizeof(aru8Buf));
    pstDelTS = (Delete_TSStream *)(&aru8Buf[1]);
    
    u8DestSlot = pstDestInfo->u8Slot;

    u16DelTSNum = 0;

    for (i = 0; i < 8; i++)
    {
        u32TempDestChannel = pstDestInfo->aru32Channel[i];

        if (0 == u32TempDestChannel)
        {
            continue;
        }

        for (j = 0; j < 32; j++)
        {
            if (0 == (u32TempDestChannel & (0x1 << j)))
            {
                continue;
            }

            pstDelTS[u16DelTSNum].u8SourceSlotID = Status_GetSlotID();
            pstDelTS[u16DelTSNum].u16SourceChannelID = pstParamTS->pInputTS[u16InTSIndex].u16ChannelID;
            pstDelTS[u16DelTSNum].u16ChannelID = i * 32 + j;

            u16DelTSNum++;
        }
    }

    if (0 == u16DelTSNum)
    {
        return WV_SUCCESS;
    }

    aru8Buf[0] = u16DelTSNum;
    u32BufLen = 1 + u16DelTSNum * sizeof(Delete_TSStream);

    enErrCode = (wvErrCode)IPComm_SendData2Slot(u8DestSlot, IPCOMM_SEND_DEL_TS, aru8Buf, u32BufLen);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendData error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    enErrCode = (wvErrCode)IPComm_SendCmd2Slot(u8DestSlot, IPCOMM_SEND_DONE_FLAG);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendCmd2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_DelOutputBoardTS
  Description:  ∞— ‰»Î¡˜¥” ‰≥ˆ∞Â÷–…æ≥˝
  Input:        u16InTSIndex: ‰»Î¡˜À˜“˝
                pstParamTS:
                pstDelDest: ‰≥ˆ∞Â–≈œ¢
  Output:       
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_DelOutputBoardTS(U16 u16InTSIndex, TSInfo *pstParamTS, Dest *pstDelDest)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;
    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!pstDelDest))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],pstDelDest[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, pstDelDest);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    for (i = 0; i < pstDelDest->u8DestSlotNum; i++)
    {
        enErrCode = TSP_SendDelTSInfoToOutputBoard(u16InTSIndex, pstParamTS, &pstDelDest->arstDestSlotInfo[i]);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_SendDelTSInfoToOutputBoard error\r\n",
                __FUNCTION__, __LINE__);
            return enErrCode;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SendInputProgPIDToOutputBoard
  Description:  ∞— ‰»ÎΩ⁄ƒøµƒÀ˘”–PIDΩ·ππ∑¢ÀÕµΩ ‰≥ˆ◊”∞Â
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                pstParamTS:
                u8DestSlot: ‰≥ˆ◊”∞Â≤€Œª∫≈
                u16DestChannel:Õ®µ¿∫≈
  Output:       
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SendInputProgPIDToOutputBoard(U16 u16InProgIndex,
                                            TSInfo *pstParamTS,
                                            U8 u8DestSlot,
                                            U16 u16DestChannel)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 j = 0;

    U8 *pu8Buf = NULL;
    U32 u32BufLen = 0;
    
    IndexInfoList stESIndexList;
    IndexInfoList stESECMIndexList;
    IndexInfoList stPSECMIndexList;

    U16 u16PCRIndex = INDEX_INVALID_VALUE;
    U16 u16PIDIndex = INDEX_INVALID_VALUE;
    U16 u16ESECMIndex = INDEX_INVALID_VALUE;
    U16 u16PIDNum = 0;
    U16 u16StreamID = 0;
    U8 u8CAMIndex = 0;
    
    Output_TSPID *pstOutPID = NULL;
    
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    u8CAMIndex = pstParamTS->pInputProgram[u16InProgIndex].u8CAMIndex;

    if (u8CAMIndex < WVCI_MAX_SLOT_NUM)
    {
        u16StreamID = TSP_CICamIndex2StreamID(u8CAMIndex);
    }

    u16PCRIndex = pstParamTS->pInputProgram[u16InProgIndex].u16PCRIndex;
    if (u16PCRIndex < pstParamTS->u32InputPIDNumber)
    {
        u16PIDNum += 1;
    }
    
    stESIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetInProgESPIDIndexList(u16InProgIndex, pstParamTS, &stESIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]TSP_GetInProgESPIDIndexList error\r\n",
            __FUNCTION__, __LINE__);
        return enErrCode;
    }

    u16PIDNum += stESIndexList.u16IndexListNum;

    stPSECMIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetInProgPSECMPIDIndexList(u16InProgIndex, pstParamTS, &stPSECMIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]TSP_GetInProgPSECMPIDIndexList error\r\n",
            __FUNCTION__, __LINE__);
        return enErrCode;
    }

    u16PIDNum += stPSECMIndexList.u16IndexListNum;

    for (i = 0; i < stESIndexList.u16IndexListNum; i++)
    {
        u16PIDIndex = stESIndexList.aru16IndexList[i];

        stESECMIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInPIDESECMPIDIndexList(u16PIDIndex, pstParamTS, &stESECMIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_GetInPIDESECMPIDIndexList error\r\n",
                __FUNCTION__, __LINE__);
            return enErrCode;
        }

        u16PIDNum += stESECMIndexList.u16IndexListNum;
    }

    if (0 == u16PIDNum)
    {
        return WV_SUCCESS;
    }

    u32BufLen = 1 + sizeof(Output_TSPID) * u16PIDNum;

    pu8Buf = calloc(1, u32BufLen);
    if (!pu8Buf)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc Error,u32BufLen[%u],u16PIDNum[%u]\r\n",
            __FUNCTION__, __LINE__, u32BufLen, u16PIDNum);
        return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
    }
    
    pstOutPID = (Output_TSPID *)(&pu8Buf[1]);

    if (u16PCRIndex < pstParamTS->u32InputPIDNumber)
    {
        TSP_InitOutputPID(pstOutPID, &pstParamTS->pInputPID[u16PCRIndex]);
        pstOutPID->u8SlotID = u8DestSlot;
        pstOutPID->u16ChannelID = u16DestChannel;
        pstOutPID->u16SrcCIStreamID = u16StreamID;

        pstOutPID++;
    }

    for (i = 0; i < stESIndexList.u16IndexListNum; i++)
    {
        u16PIDIndex = stESIndexList.aru16IndexList[i];

        TSP_InitOutputPID(pstOutPID, &pstParamTS->pInputPID[u16PIDIndex]);
        pstOutPID->u8SlotID = u8DestSlot;
        pstOutPID->u16ChannelID = u16DestChannel;
        pstOutPID->u16PIDID = u16PIDIndex;
        pstOutPID->u16SrcCIStreamID = u16StreamID;

        pstOutPID++;
    }

    for (i = 0; i < stPSECMIndexList.u16IndexListNum; i++)
    {
        u16PIDIndex = stPSECMIndexList.aru16IndexList[i];

        TSP_InitOutputPID(pstOutPID, &pstParamTS->pInputPID[u16PIDIndex]);
        pstOutPID->u8SlotID = u8DestSlot;
        pstOutPID->u16ChannelID = u16DestChannel;

        pstOutPID++;
    }

    for (i = 0; i < stESIndexList.u16IndexListNum; i++)
    {
        u16PIDIndex = stESIndexList.aru16IndexList[i];

        stESECMIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInPIDESECMPIDIndexList(u16PIDIndex, pstParamTS, &stESECMIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetInPIDESECMPIDIndexList error\r\n",
                __FUNCTION__, __LINE__);

            free(pu8Buf);
            pu8Buf = NULL;
            
            return enErrCode;
        }

        for (j = 0; j < stESECMIndexList.u16IndexListNum; j++)
        {
            u16ESECMIndex = stESECMIndexList.aru16IndexList[j];

            TSP_InitOutputPID(pstOutPID, &pstParamTS->pInputPID[u16ESECMIndex]);
            pstOutPID->u8SlotID = u8DestSlot;
            pstOutPID->u16ChannelID = u16DestChannel;
            pstOutPID->u16PIDID = u16PIDIndex;
    
            pstOutPID++;
        }
    }

    pu8Buf[0] = u16PIDNum;

    enErrCode = (wvErrCode)IPComm_SendData2Slot(u8DestSlot, IPCOMM_SEND_ADD_PID, pu8Buf, u32BufLen);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendData2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        free(pu8Buf);
        pu8Buf = NULL;
    
        return enErrCode;
    }
        
    free(pu8Buf);
    pu8Buf = NULL;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SendInputTSPIDToOutputBoard
  Description:  ∞— ‰»Î¡˜µƒÀ˘”–PID(EMM/OtherPID)Ω·ππ∑¢ÀÕµΩ ‰≥ˆ◊”∞Â
  Input:        u16InTSIndex: ‰»Î¡˜À˜“˝
                pstParamTS:
                u8DestSlot: ‰≥ˆ◊”∞Â≤€Œª∫≈
                u16DestChannel:Õ®µ¿∫≈
  Output:       
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SendInputTSPIDToOutputBoard(U16 u16InTSIndex,
                                            TSInfo *pstParamTS,
                                            U8 u8DestSlot,
                                            U16 u16DestChannel)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;

    U8 *pu8Buf = NULL;
    U32 u32BufLen = 0;

    IndexInfoList stEMMIndexList;
    IndexInfoList stOtherPIDIndexList;

    U16 u16PIDNum = 0;

    Output_TSPID *pstOutPID = NULL;

    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    stEMMIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetInTSEMMPIDIndexList(u16InTSIndex, pstParamTS, &stEMMIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]TSP_GetInTSEMMPIDIndexList error\r\n",
            __FUNCTION__, __LINE__);
        return enErrCode;
    }

    u16PIDNum += stEMMIndexList.u16IndexListNum;

    stOtherPIDIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetInTSOtherPIDIndexList(u16InTSIndex, pstParamTS, &stOtherPIDIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]TSP_GetInTSOtherPIDIndexList error\r\n",
            __FUNCTION__, __LINE__);
        return enErrCode;
    }

    u16PIDNum += stOtherPIDIndexList.u16IndexListNum;

    if (0 == u16PIDNum)
    {
        return WV_SUCCESS;
    }

    u32BufLen = 1 + sizeof(Output_TSPID) * u16PIDNum;

    pu8Buf = calloc(1, u32BufLen);
    if (!pu8Buf)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc Error,u32BufLen[%u]\r\n", __FUNCTION__, __LINE__, u32BufLen);
        return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
    }

    pstOutPID = (Output_TSPID *)(&pu8Buf[1]);

    for (i = 0; i < stEMMIndexList.u16IndexListNum; i++)
    {
        TSP_InitOutputPID(pstOutPID, &pstParamTS->pInputPID[stEMMIndexList.aru16IndexList[i]]);
        pstOutPID->u8SlotID = u8DestSlot;
        pstOutPID->u16ChannelID = u16DestChannel;

        pstOutPID++;
    }

    for (i = 0; i < stOtherPIDIndexList.u16IndexListNum; i++)
    {
        TSP_InitOutputPID(pstOutPID, &pstParamTS->pInputPID[stOtherPIDIndexList.aru16IndexList[i]]);
        pstOutPID->u8SlotID = u8DestSlot;
        pstOutPID->u16ChannelID = u16DestChannel;

        pstOutPID++;
    }

    pu8Buf[0] = u16PIDNum;

    enErrCode = (wvErrCode)IPComm_SendData2Slot(u8DestSlot, IPCOMM_SEND_ADD_PID, pu8Buf, u32BufLen);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendData2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        free(pu8Buf);
        pu8Buf = NULL;
    
        return enErrCode;
    }
    
    free(pu8Buf);
    pu8Buf = NULL;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SendProgToOutputBoard
  Description:  ∞— ‰»Î¡˜µƒÀ˘”–Ω⁄ƒøΩ·ππ∑¢ÀÕµΩ ‰≥ˆ◊”∞Â
  Input:        u16InTSIndex: ‰»Î¡˜À˜“˝
                pstParamTS:
                u8DestSlot: ‰≥ˆ◊”∞Â≤€Œª∫≈
                u16DestChannel:Õ®µ¿∫≈
  Output:       
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SendInputTSProgToOutputBoard(U16 u16InTSIndex,
                                            TSInfo *pstParamTS,
                                            U8 u8DestSlot,
                                            U16 u16DestChannel)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;

    U8 *pu8Buf = NULL;
    U32 u32BufLen = 0;

    Output_TSProgram *pstOutProg = NULL;
    U16 u16InProgIndex = INDEX_INVALID_VALUE;
    
    IndexInfoList stIndexInfoList;
    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    stIndexInfoList.u16IndexListNum = 0;
    enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]TSP_GetInTSProgIndexList error\r\n",
            __FUNCTION__, __LINE__);
        return enErrCode;
    }

    if (0 == stIndexInfoList.u16IndexListNum)
    {
        return WV_SUCCESS;
    }

    u32BufLen = 1 + (sizeof(Output_TSProgram) * stIndexInfoList.u16IndexListNum);

    pu8Buf = calloc(1, u32BufLen);
    if (!pu8Buf)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc Error,u32BufLen[%u]\r\n", __FUNCTION__, __LINE__, u32BufLen);
        return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
    }

    pstOutProg = (Output_TSProgram *)(&pu8Buf[1]);

    for (i = 0; i < stIndexInfoList.u16IndexListNum; i++)
    {
        u16InProgIndex = stIndexInfoList.aru16IndexList[i];
        
        TSP_InitOutputProg(pstOutProg, &pstParamTS->pInputProgram[u16InProgIndex]);

        pstOutProg->u8SlotID = u8DestSlot;
        pstOutProg->u16ChannelID = u16DestChannel;

        pstOutProg++;
    }
    
    pu8Buf[0] = stIndexInfoList.u16IndexListNum;

    enErrCode = (wvErrCode)IPComm_SendData2Slot(u8DestSlot, IPCOMM_SEND_ADD_PROG, pu8Buf, u32BufLen);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendData2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        free(pu8Buf);
        pu8Buf = NULL;
    
        return enErrCode;
    }
    
    free(pu8Buf);
    pu8Buf = NULL;

    for (i = 0; i < stIndexInfoList.u16IndexListNum; i++)
    {
        u16InProgIndex = stIndexInfoList.aru16IndexList[i];
        
        enErrCode = TSP_SendInputProgPIDToOutputBoard(u16InProgIndex, pstParamTS, u8DestSlot, u16DestChannel);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_SendInputProgPIDToOutputBoard error\r\n",
                __FUNCTION__, __LINE__);
            return enErrCode;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SendProgToOutputBoard
  Description:  ∞— ‰»ÎΩ⁄ƒøΩ·ππ∑¢ÀÕµΩ ‰≥ˆ◊”∞Â
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                pstParamTS:
                u8DestSlot: ‰≥ˆ◊”∞Â≤€Œª∫≈
                aru16DestChannel:Õ®µ¿¡¥±Ì
                u16DestChannelNum:Õ®µ¿∏ˆ ˝
  Output:       
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SendProgToOutputBoard(U16 u16InProgIndex,
                                    TSInfo *pstParamTS,
                                    U8 u8DestSlot,
                                    U16 aru16DestChannel[],
                                    U16 u16DestChannelNum)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;

    U8 *pu8Buf = NULL;
    U32 u32BufLen = 0;

    Output_TSProgram *pstOutProg = NULL;
    
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex) || (!aru16DestChannel))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InProgIndex[%u],aru16DestChannel[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, aru16DestChannel);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == u16DestChannelNum)
    {
        return WV_SUCCESS;
    }

    u32BufLen = 1 + (sizeof(Output_TSProgram) * u16DestChannelNum);

    pu8Buf = calloc(1, u32BufLen);
    if (!pu8Buf)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc Error,u32BufLen[%u]\r\n", __FUNCTION__, __LINE__, u32BufLen);
        return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
    }

    pstOutProg = (Output_TSProgram *)(&pu8Buf[1]);

    for (i = 0; i < u16DestChannelNum; i++)
    {
        TSP_InitOutputProg(pstOutProg, &pstParamTS->pInputProgram[u16InProgIndex]);
        pstOutProg->u8SlotID = u8DestSlot;
        pstOutProg->u16ChannelID = aru16DestChannel[i];

        pstOutProg++;
    }

    pu8Buf[0] = u16DestChannelNum;

    enErrCode = (wvErrCode)IPComm_SendData2Slot(u8DestSlot, IPCOMM_SEND_ADD_PROG, pu8Buf, u32BufLen);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendData2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        free(pu8Buf);
        pu8Buf = NULL;
        
        return enErrCode;
    }

    free (pu8Buf);
    pu8Buf = NULL;

    for (i = 0; i < u16DestChannelNum; i++)
    {
        enErrCode = TSP_SendInputProgPIDToOutputBoard(u16InProgIndex, pstParamTS, u8DestSlot, aru16DestChannel[i]);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_SendInputProgPIDToOutputBoard error\r\n",
                __FUNCTION__, __LINE__);
            return enErrCode;
        }
    }

    enErrCode = (wvErrCode)IPComm_SendCmd2Slot(u8DestSlot, IPCOMM_SEND_DONE_FLAG);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendCmd2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        return enErrCode;
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SendTSToOutputBoard
  Description:  ∞— ‰»Î¡˜Ω·ππ∑¢ÀÕµΩ ‰≥ˆ◊”∞Â
  Input:        u16InTSIndex: ‰»Î¡˜À˜“˝
                pstParamTS:
                u8DestSlot: ‰≥ˆ◊”∞Â≤€Œª∫≈
                aru16DestChannel:Õ®µ¿¡¥±Ì
                u16DestChannelNum:Õ®µ¿∏ˆ ˝
  Output:       
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SendTSToOutputBoard(U16 u16InTSIndex,
                            TSInfo *pstParamTS,
                            U8 u8DestSlot,
                            U16 aru16DestChannel[],
                            U8 aru8BypassFlag[],
                            U16 u16DestChannelNum)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;

    U8 *pu8Buf = NULL;
    U32 u32BufLen = 0;

    Output_TSStream *pstOutTS = NULL;
    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!aru16DestChannel)
        || (!aru8BypassFlag))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],aru16DestChannel[%p],aru8BypassFlag[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, aru16DestChannel, aru8BypassFlag);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == u16DestChannelNum)
    {
        return WV_SUCCESS;
    }

    u32BufLen = 1 + (sizeof(Output_TSStream) * u16DestChannelNum);

    pu8Buf = calloc(1, u32BufLen);
    if (!pu8Buf)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc Error,u32BufLen[%u]\r\n", __FUNCTION__, __LINE__, u32BufLen);
        return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
    }

    pstOutTS = (Output_TSStream *)(&pu8Buf[1]);

    for (i = 0; i < u16DestChannelNum; i++)
    {
        pstOutTS[i].u16NetworkID = pstParamTS->pInputTS[u16InTSIndex].u16NetworkID;
        pstOutTS[i].u16TransportStreamID = pstParamTS->pInputTS[u16InTSIndex].u16TransportStreamID;
        pstOutTS[i].u16OriginalNetworkID = pstParamTS->pInputTS[u16InTSIndex].u16OriginalNetworkID;
        pstOutTS[i].u8ValidFlag = TSP_FLAG_VALID;
        pstOutTS[i].u8SlotID = u8DestSlot;
        pstOutTS[i].u16ChannelID = aru16DestChannel[i];

        pstOutTS[i].u8BypassFlag = aru8BypassFlag[i];
        pstOutTS[i].u8SrcSlotID = Status_GetSlotID();
        pstOutTS[i].u16SrcChannelID = pstParamTS->pInputTS[u16InTSIndex].u16ChannelID;
    }

    pu8Buf[0] = u16DestChannelNum;

    enErrCode = (wvErrCode)IPComm_SendData2Slot(u8DestSlot, IPCOMM_SEND_ADD_TS, pu8Buf, u32BufLen);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendData2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        free(pu8Buf);
        pu8Buf = NULL;
        
        return enErrCode;
    }

    free(pu8Buf);
    pu8Buf = NULL;

    for (i = 0; i < u16DestChannelNum; i++)
    {
        enErrCode = TSP_SendInputTSProgToOutputBoard(u16InTSIndex,
                                                    pstParamTS,
                                                    u8DestSlot,
                                                    aru16DestChannel[i]);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_SendInputTSProgToOutputBoard error\r\n",
                __FUNCTION__, __LINE__);
            return enErrCode;
        }

        enErrCode = TSP_SendInputTSPIDToOutputBoard(u16InTSIndex,
                                                    pstParamTS,
                                                    u8DestSlot,
                                                    aru16DestChannel[i]);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_SendInputTSPIDToOutputBoard error\r\n",
                __FUNCTION__, __LINE__);
            return enErrCode;
        }
    }

    enErrCode = (wvErrCode)IPComm_SendCmd2Slot(u8DestSlot, IPCOMM_SEND_DONE_FLAG);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendCmd2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_AddProgToOutputBoard
  Description:  ∏˘æ›“™ÃÌº”µƒ»•œÚ£¨∞— ‰»ÎΩ⁄ƒø∑¢ÀÕµΩ ‰≥ˆ◊”∞Â
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                pstAddDest:»•œÚ–≈œ¢
                pstParamTS
  Output:       
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddProgToOutputBoard(U16 u16InProgIndex, TSInfo *pstParamTS, Dest *pstAddDest)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;
    U8 j = 0;
    U8 k = 0;

    U8 u8DestSlot = 0;
    U32 u32TempDestChannel = 0;
    U16 u16DestChannelNum = 0;
    U16 aru16DestChannel[256] = {0};

    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex) || (!pstAddDest))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InProgIndex[%u],pstAddDest[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, pstAddDest);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    for (i = 0; i < pstAddDest->u8DestSlotNum; i++)
    {
        u8DestSlot = pstAddDest->arstDestSlotInfo[i].u8Slot;
        
        u16DestChannelNum = 0;

        for (j = 0; j < 8; j++)
        {
            u32TempDestChannel = pstAddDest->arstDestSlotInfo[i].aru32Channel[j];
    
            if (0 == u32TempDestChannel)
            {
                continue;
            }
    
            for (k = 0; k < 32; k++)
            {
                if (0 == (u32TempDestChannel & (0x1 << k)))
                {
                    continue;
                }
    
                aru16DestChannel[u16DestChannelNum++] = j * 32 + k;
            }
        }
        
        enErrCode = TSP_SendProgToOutputBoard(u16InProgIndex,
                                                pstParamTS,
                                                u8DestSlot,
                                                aru16DestChannel,
                                                u16DestChannelNum);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_SendProgToOutputBoard error\r\n",
                __FUNCTION__, __LINE__);
            return enErrCode;
        }
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_AddTSToOutputBoard
  Description:  ∏˘æ›“™ÃÌº”µƒ»•œÚ£¨∞— ‰»Î¡˜∑¢ÀÕµΩ ‰≥ˆ◊”∞Â
  Input:        u16InTSIndex: ‰»Î¡˜À˜“˝
                pstAddDest:»•œÚ–≈œ¢
                pstParamTS
  Output:       
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddTSToOutputBoard(U16 u16InTSIndex, TSInfo *pstParamTS, Dest *pstAddDest)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;
    U8 j = 0;
    U8 k = 0;

    U32 u32TempDestChannel = 0;
    U16 u16DestChannelNum = 0;
    U16 aru16DestChannel[MAX_OUTPUT_CHANNEL_NUM] = {0};
    U32 u32TempBypassFlag = 0;
    U8 aru8BypassFlag[MAX_OUTPUT_CHANNEL_NUM] = {0};
    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!pstAddDest))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],pstAddDest[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, pstAddDest);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    for (i = 0; i < pstAddDest->u8DestSlotNum; i++)
    {
        u16DestChannelNum = 0;

        for (j = 0; j < 8; j++)
        {
            u32TempDestChannel = pstAddDest->arstDestSlotInfo[i].aru32Channel[j];
    
            if (0 == u32TempDestChannel)
            {
                continue;
            }

            u32TempBypassFlag = pstAddDest->arstDestSlotInfo[i].aru32BypassFlag[j];
    
            for (k = 0; k < 32; k++)
            {
                if (0 == (u32TempDestChannel & (0x1 << k)))
                {
                    continue;
                }

                if (0 == (u32TempBypassFlag & (0x1 << k)))
                {
                    aru8BypassFlag[u16DestChannelNum] = false;
                }
                else
                {
                    aru8BypassFlag[u16DestChannelNum] = true;
                }

                aru16DestChannel[u16DestChannelNum++] = j * 32 + k;
            }
        }

        enErrCode = TSP_SendTSToOutputBoard(u16InTSIndex,
                                    pstParamTS,
                                    pstAddDest->arstDestSlotInfo[i].u8Slot,
                                    aru16DestChannel,
                                    aru8BypassFlag,
                                    u16DestChannelNum);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_SendTSToOutputBoard error\r\n",
                __FUNCTION__, __LINE__);
            return enErrCode;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_CheckProgDestParam
  Description:  ºÏ≤È“™…Ë÷√µƒΩ⁄ƒøµƒ»•œÚ «∑Ò∫œ∑®
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøµƒÀ˜“˝
                pstNewDest:Õ¯π‹“™…Ë÷√µƒ»•œÚ–≈œ¢
                pstParamTS
  Output:       
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_CheckProgDestParam(U16 u16InProgIndex, Dest *pstNewDest, TSInfo *pstParamTS)
{
    U8 i = 0;
    U8 j = 0;
    U8 k = 0;
    U8 u8DestSlot = MAX_SLOT_NUM;
    U16 u16DestChannel= 0;
    U32 u32NewChannel = 0;
    U16 u16InTSIndex = INDEX_INVALID_VALUE;

    if ((!pstNewDest) || (!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstNewDest[%p],pstParamTS[%p],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstNewDest, pstParamTS, u16InProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    u16InTSIndex = pstParamTS->pInputProgram[u16InProgIndex].u16TSIndex;

    for (i = 0; i < pstNewDest->u8DestSlotNum; i++)
    {
        u8DestSlot = pstNewDest->arstDestSlotInfo[i].u8Slot;
        
        for (j = 0; j < 8; j++)
        {
            u32NewChannel = pstNewDest->arstDestSlotInfo[i].aru32Channel[j];
            
            for (k = 0; k < 32; k++)
            {
                if (0 == (u32NewChannel & (0x1 << k)))
                {
                    continue;
                }

                u16DestChannel = j * 32 + k;

                if (TSP_IfInputTSOutputs2DestChannel(u16InTSIndex, u8DestSlot, u16DestChannel, pstParamTS))
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]Service[%u] already output to Channel[%u] of Slot[%u] in TS level\r\n",
                        __FUNCTION__,
                        __LINE__,
                        pstParamTS->pInputProgram[u16InProgIndex].u16ServiceID,
                        u16DestChannel,
                        u8DestSlot);
                    return WV_ERR_TSP_PROG_ALREADY_EXIST;
                }
            }
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_CheckTSDestParam
  Description:  ºÏ≤È“™…Ë÷√µƒ¡˜µƒ»•œÚ «∑Ò∫œ∑®
  Input:        u16InTSIndex: ‰»Î¡˜µƒÀ˜“˝
                pstNewDest:Õ¯π‹“™…Ë÷√µƒ»•œÚ–≈œ¢
                pstParamTS
  Output:       
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_CheckTSDestParam(U16 u16InTSIndex, Dest *pstNewDest, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;
    U8 j = 0;
    U8 k = 0;
    U8 u8DestSlot = MAX_SLOT_NUM;
    U16 u16DestChannel= 0;
    U32 u32NewChannel = 0;
    U32 u32NewBypassFlag = 0;
    U8 u8ChannelStatus = 0;
    IndexInfoList stIndexInfoList;
    U16 u16ProgNum = 0;
    U16 u16ProgIndex = 0;

    if ((!pstNewDest) || (!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstNewDest[%p],pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstNewDest, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    stIndexInfoList.u16IndexListNum = 0;
    enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSProgIndexList Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        return enErrCode;
    }

    for (i = 0; i < pstNewDest->u8DestSlotNum; i++)
    {
        u8DestSlot = pstNewDest->arstDestSlotInfo[i].u8Slot;
        
        for (j = 0; j < 8; j++)
        {
            u32NewChannel = pstNewDest->arstDestSlotInfo[i].aru32Channel[j];
            u32NewBypassFlag = pstNewDest->arstDestSlotInfo[i].aru32BypassFlag[j];
            
            for (k = 0; k < 32; k++)
            {
                if (0 == (u32NewChannel & (0x1 << k)))
                {
                    continue;
                }

                u16DestChannel = j * 32 + k;

                for (u16ProgNum = 0; u16ProgNum < stIndexInfoList.u16IndexListNum; u16ProgNum++)
                {
                    u16ProgIndex = stIndexInfoList.aru16IndexList[u16ProgNum];
                    if (TSP_IfInputProgOutputs2DestChannel(u16ProgIndex, u8DestSlot, u16DestChannel, pstParamTS))
                    {
                        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                            "[%s:%d]Service[%d] in TS already output to Channel[%u] of Slot[%u]\r\n",
                            __FUNCTION__,
                            __LINE__,
                            pstParamTS->pInputProgram[u16ProgIndex].u16ServiceID,
                            u16DestChannel,
                            u8DestSlot);
                        
                        return WV_ERR_TSP_PROG_ALREADY_EXIST;
                    }
                }

                if (0 == (u32NewBypassFlag & (0x1 << k)))
                {
                    continue;
                }

                u8ChannelStatus = Resource_getOutputChannelStatus(u8DestSlot, u16DestChannel);

                if ((0 != u8ChannelStatus) && (1 != u8ChannelStatus))
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]Channel[%u] of Slot[%u] already has prog,can not bypass to this channel\r\n",
                        __FUNCTION__,
                        __LINE__,
                        u16DestChannel,
                        u8DestSlot);
                    
                    return WV_ERR_TSP_PROG_ALREADY_EXIST;
                }
            }
        }
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ProcessDest
  Description:  ¥¶¿Ì“ª∏ˆ¡˜ªÚΩ⁄ƒøµƒ»•œÚ
  Input:        pstOldDest:æ…µƒ»•œÚ–≈œ¢
                pstNewDest:Õ¯π‹“™…Ë÷√µƒ»•œÚ–≈œ¢
                aru8DestSlot:
                aru32DestChannel:
                aru32BypassFlag:
                pstParamTS
  Output:       pstDelDest:…æ≥˝¡Àµƒ»•œÚ
                pstAddDest:‘ˆº”¡Àµƒ»•œÚ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ProcessDest(Dest *pstOldDest,
                    Dest *pstNewDest,
                    U8 aru8DestSlot[],
                    U32 aru32DestChannel[][8],
                    U32 aru32BypassFlag[][8],
                    Dest *pstDelDest,
                    Dest *pstAddDest,
                    TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;
    U8 j = 0;
    U8 k = 0;

    U8 u8DestSlot = 0;
    U8 u8SlotInfoIndex = 0;
    U32 u32OldChannel = 0;
    U32 u32NewChannel = 0;
    U32 u32NewBypassFlag = 0;
    BOOL bBypassFlag = false;

    BOOL bDelFlag = false;
    BOOL bAddFlag = false;

    U8 u8TempDestSlotIndex = 0;

    if ((!pstNewDest) || (!pstOldDest) || (!aru8DestSlot) || (!aru32DestChannel))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstNewDest[%p],pstOldDest[%p],aru8DestSlot[%p],aru32DestChannel[%p]\r\n",
            __FUNCTION__, __LINE__, pstNewDest, pstOldDest, aru8DestSlot, aru32DestChannel);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(pstDelDest, 0, sizeof(Dest));
    memset(pstAddDest, 0, sizeof(Dest));

    //–¬æ…»•œÚ–≈œ¢∂‘±»£¨¥¶¿Ì“™…æ≥˝µƒ»•œÚ
    for (i = 0; i < pstOldDest->u8DestSlotNum; i++)
    {
        u8DestSlot = pstOldDest->arstDestSlotInfo[i].u8Slot;
        
        //º«¬º…æ≥˝¡Àµƒ»•œÚ£¨∫Û√Ê‘ŸÕ≥“ª∑¢ÀÕµΩ ‰≥ˆ∞Â
        pstDelDest->arstDestSlotInfo[pstDelDest->u8DestSlotNum].u8Slot = u8DestSlot;

        //‘⁄–¬µƒ»•œÚ–≈œ¢÷–’“æ…µƒDestSlot
        enErrCode = TSP_GetDestSlotInfoIndex(u8DestSlot, pstNewDest, &u8SlotInfoIndex);
        if (WV_ERR_TSP_NOT_EXIST == enErrCode)//√ª’“µΩ£¨‘Ú±Ì√˜’‚∏ˆDestSlot «“™…æ≥˝µƒ
        {
            enErrCode = TSP_DelDestSlot(u8DestSlot,
                                        aru8DestSlot,
                                        aru32DestChannel,
                                        aru32BypassFlag);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_DelDestSlot Error:u8DestSlot[%u],enErrCode[%08X]\r\n",
                    __FUNCTION__, __LINE__, u8DestSlot, enErrCode);

                return enErrCode;
            }

            //º«¬º…æ≥˝µƒÀ˘”–Õ®µ¿
            memcpy(pstDelDest->arstDestSlotInfo[pstDelDest->u8DestSlotNum].aru32Channel,
                    pstOldDest->arstDestSlotInfo[i].aru32Channel,
                    8 * sizeof(U32));
            pstDelDest->u8DestSlotNum++;
            
            continue;
        }
        else if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetDestSlotInfoInddex Error:u8DestSlot[%u],enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, u8DestSlot, enErrCode);
            return enErrCode;
        }

        bDelFlag = false;

        //‘⁄–¬µƒ»•œÚ–≈œ¢÷–’“µΩ¡À∂‘”¶µƒDestSlot£¨ªπ–Ë“™∂‘±»–¬æ…µƒChannel£¨»∑∂®“™…æ≥˝ƒƒ–©Õ®µ¿
        for (j = 0; j < 8; j++)
        {
            u32OldChannel = pstOldDest->arstDestSlotInfo[i].aru32Channel[j];
            u32NewChannel = pstNewDest->arstDestSlotInfo[u8SlotInfoIndex].aru32Channel[j];

            if ((0 == u32OldChannel) || (u32OldChannel == u32NewChannel))
            {
                continue;
            }

            for (k = 0; k < 32; k++)
            {
                if ((0 == (u32OldChannel & (0x1 << k))) || (0 != (u32NewChannel & (0x1 << k))))
                {
                    continue;
                }

                enErrCode = TSP_DelDestChannel(u8DestSlot,
                                            j * 32 + k,
                                            aru8DestSlot,
                                            aru32DestChannel,
                                            aru32BypassFlag);
                if (WV_SUCCESS != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]TSP_DelDestChannel Error:u8DestSlot[%u],Channel[%u],enErrCode[%08X]\r\n",
                        __FUNCTION__, __LINE__, u8DestSlot, j * 32 + k, enErrCode);

                    return enErrCode;
                }

                //º«¬º…æ≥˝¡Àƒƒ∏ˆÕ®µ¿
                pstDelDest->arstDestSlotInfo[pstDelDest->u8DestSlotNum].aru32Channel[j] |= 0x1 << k;
                bDelFlag = true;
            }
        }

        if (bDelFlag)
        {
            pstDelDest->u8DestSlotNum++;
        }
    }

    //…æ≥˝¡À»•œÚ£¨–Ë“™≈–∂œ‘⁄À˘”–µƒ¡˜÷– «∑Òªπ”–PID»•∂‘”¶µƒDestSlot
    //»Áπ˚√ª”–‘Ú“™∞—DestSlot¥”»´æ÷»•œÚs_aru8DestSlotRecord ˝◊È÷–…æ≥˝
    for (i = 0; i < pstDelDest->u8DestSlotNum; i++)
    {
        u8DestSlot = pstDelDest->arstDestSlotInfo[i].u8Slot;

        enErrCode = TSP_GetDestIndex(u8DestSlot, s_aru8DestSlotRecord, &u8TempDestSlotIndex);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetDestIndex Error:enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;
        }

        if (!TSP_IsThereAnyPIDThatOutputs2DestSlot(u8DestSlot, pstParamTS))
        {
            s_aru8DestSlotRecord[u8TempDestSlotIndex] = MAX_SLOT_NUM;
        }
    }

    if (0 != pstDelDest->u8DestSlotNum)
    {
        enErrCode = TSP_GetDestInfo(aru8DestSlot, 
                                        aru32DestChannel, 
                                        aru32BypassFlag,
                                        pstOldDest);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetDestInfo Error:enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;
        }
    }

    //–¬æ…»•œÚ–≈œ¢∂‘±»£¨¥¶¿Ì“™ÃÌº”µƒ»•œÚ
    for (i = 0; i < pstNewDest->u8DestSlotNum; i++)
    {
        u8DestSlot = pstNewDest->arstDestSlotInfo[i].u8Slot;

        //º«¬º“™ÃÌº”µƒ»•œÚ£¨∫Û√ÊÕ≥“ª∑¢ÀÕµΩ ‰≥ˆ∞Â
        pstAddDest->arstDestSlotInfo[pstAddDest->u8DestSlotNum].u8Slot = u8DestSlot;

        //‘⁄æ…µƒ»•œÚ–≈œ¢÷–’“–¬µƒDestSlot
        enErrCode = TSP_GetDestSlotInfoIndex(u8DestSlot, pstOldDest, &u8SlotInfoIndex);
        if (WV_ERR_TSP_NOT_EXIST == enErrCode)//»Áπ˚√ª’“µΩ£¨‘Ú±Ì√˜’‚∏ˆDestSlot «“™–¬‘ˆº”µƒ
        {
            enErrCode = TSP_AddDestSlot(&pstNewDest->arstDestSlotInfo[i],
                                        aru8DestSlot,
                                        aru32DestChannel,
                                        aru32BypassFlag);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_AddSlotInTSDest Error:enErrCode[%08X]\r\n",
                    __FUNCTION__, __LINE__, enErrCode);

                return enErrCode;
            }

            memcpy(pstAddDest->arstDestSlotInfo[pstAddDest->u8DestSlotNum].aru32Channel,
                    pstNewDest->arstDestSlotInfo[i].aru32Channel,
                    8 * sizeof(U32));
            memcpy(pstAddDest->arstDestSlotInfo[pstAddDest->u8DestSlotNum].aru32BypassFlag,
                    pstNewDest->arstDestSlotInfo[i].aru32BypassFlag,
                    8 * sizeof(U32));
            pstAddDest->u8DestSlotNum++;

            //–¬‘ˆ“ª∏ˆDestSlot£¨“™≈–∂œ‘⁄»´æ÷»•œÚs_aru8DestSlotRecord ˝◊È÷– «∑Ò“—æ≠”–¡À’‚∏ˆDestSlot£¨»Áπ˚√ª”–‘Ú“™ÃÌº”Ω¯»•
            enErrCode = TSP_GetDestIndex(u8DestSlot, s_aru8DestSlotRecord, &u8TempDestSlotIndex);
            if (WV_ERR_TSP_NOT_EXIST == enErrCode)
            {
                enErrCode = TSP_GetUnusedDestIndex(s_aru8DestSlotRecord, &u8TempDestSlotIndex);
                if (WV_SUCCESS != enErrCode)
                {
                  log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                      "[%s:%d]TSP_GetUnuseDestIndex Error:enErrCode[%08X]\r\n",
                      __FUNCTION__, __LINE__, enErrCode);
                  return enErrCode;
                }

                s_aru8DestSlotRecord[u8TempDestSlotIndex] = u8DestSlot;
            }
            else if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetDestIndex Error:enErrCode[%08X]\r\n",
                    __FUNCTION__, __LINE__, enErrCode);
                return enErrCode;
            }

            continue;
        }
        else if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetDestSlotInfoInddex Error:enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;
        }

        bAddFlag = false;

        //‘⁄æ…µƒ»•œÚ–≈œ¢÷–’“µΩ¡À∂‘”¶µƒDestSlot£¨ªπ–Ë“™∂‘±»–¬æ…µƒChannel£¨»∑∂®“™‘ˆº”ƒƒ–©Õ®µ¿
        for (j = 0; j < 8; j++)
        {
            u32OldChannel = pstOldDest->arstDestSlotInfo[u8SlotInfoIndex].aru32Channel[j];
            u32NewChannel = pstNewDest->arstDestSlotInfo[i].aru32Channel[j];
            
            if ((0 == u32NewChannel) || (u32NewChannel == u32OldChannel))
            {
                continue;
            }

            u32NewBypassFlag = pstNewDest->arstDestSlotInfo[i].aru32BypassFlag[j];
            
            for (k = 0; k < 32; k++)
            {
                if ((0 == (u32NewChannel & (0x1 << k))) || (0 != (u32OldChannel & (0x1 << k))))
                {
                    continue;
                }

                bBypassFlag = (0 == (u32NewBypassFlag & (0x1 << k))) ? false : true;

                enErrCode = TSP_AddDestChannel(u8DestSlot,
                                                j * 32 + k,
                                                bBypassFlag,
                                                aru8DestSlot,
                                                aru32DestChannel,
                                                aru32BypassFlag);
                if (WV_SUCCESS != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]TSP_AddChannelInTSDest Error:enErrCode[%08X]\r\n",
                        __FUNCTION__, __LINE__, enErrCode);

                    return enErrCode;
                }
                
                //º«¬º‘ˆº”¡Àƒƒ–©Õ®µ¿
                pstAddDest->arstDestSlotInfo[pstAddDest->u8DestSlotNum].aru32Channel[j] |= 0x1 << k;

                if (bBypassFlag)
                {
                    pstAddDest->arstDestSlotInfo[pstAddDest->u8DestSlotNum].aru32BypassFlag[j] |= 0x1 << k;
                }
                
                bAddFlag = true;
            }
        }

        if (bAddFlag)
        {
            pstAddDest->u8DestSlotNum++;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ProcessProgDest
  Description:  ¥¶¿Ì“ª∏ˆΩ⁄ƒøµƒ»•œÚ
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøµƒÀ˜“˝
                pstNewDest:Õ¯π‹“™…Ë÷√µƒ»•œÚ–≈œ¢
                pstParamTS
  Output:       
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ProcessProgDest(U16 u16InProgIndex, Dest *pstNewDest, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    Dest stOldDest;
    Dest stDelDest;
    Dest stAddDest;

    U8 aru8BackupDestSlot[MAX_DEST] = {0};

    Input_TSProgram stBackupInputProg;
    Input_TSProgram *pstInputProg = NULL;

    if ((!pstNewDest) || (!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstNewDest[%p],pstParamTS[%p],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstNewDest, pstParamTS, u16InProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    pstInputProg = &pstParamTS->pInputProgram[u16InProgIndex];

    memset(&stOldDest, 0, sizeof(Dest));
    memset(&stDelDest, 0, sizeof(Dest));
    memset(&stAddDest, 0, sizeof(Dest));
    
    //±∏∑›£¨≥ˆ¥Ì ±ªÿÕÀ
    memcpy(&stBackupInputProg, pstInputProg, sizeof(Input_TSProgram));
    memcpy(aru8BackupDestSlot, s_aru8DestSlotRecord, sizeof(aru8BackupDestSlot));

    enErrCode = TSP_GetDestInfo(pstInputProg->aru8DestSlot, 
                                pstInputProg->aru32DestChannel, 
                                NULL,
                                &stOldDest);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetDestInfo Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    enErrCode = TSP_ProcessDest(&stOldDest,
                                pstNewDest,
                                pstInputProg->aru8DestSlot,
                                pstInputProg->aru32DestChannel,
                                NULL,
                                &stDelDest,
                                &stAddDest,
                                pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_ProcessDest Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        
        goto TSP_ERROR;
    }

    enErrCode = TSP_DelOutputBoardProg(u16InProgIndex, pstParamTS, &stDelDest);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_DelOutputBoardProg Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        goto TSP_ERROR;
    }
    
    enErrCode = TSP_AddProgToOutputBoard(u16InProgIndex, pstParamTS, &stAddDest);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_AddProgToOutputBoard Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        goto TSP_ERROR;
    }
    
    return WV_SUCCESS;

TSP_ERROR:
    memcpy(pstInputProg, &stBackupInputProg, sizeof(Input_TSStream));
    memcpy(s_aru8DestSlotRecord, aru8BackupDestSlot, sizeof(aru8BackupDestSlot));
    
    return enErrCode;
}

/*****************************************************************************
  Function:     TSP_ProcessTSDest
  Description:  ¥¶¿Ì“ª∏ˆ¡˜µƒ»•œÚ
  Input:        u16InTSIndex: ‰»Î¡˜µƒÀ˜“˝
                pstNewDest:Õ¯π‹“™…Ë÷√µƒ»•œÚ–≈œ¢
                pstParamTS
  Output:       
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ProcessTSDest(U16 u16InTSIndex, Dest *pstNewDest, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    Dest stOldDest;
    Dest stDelDest;
    Dest stAddDest;

    U8 aru8BackupDestSlot[MAX_DEST] = {0};

    Input_TSStream stBackupInputTS;
    Input_TSStream *pstInputTS = NULL;

    if ((!pstNewDest) || (!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstNewDest[%p],pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstNewDest, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    pstInputTS = &pstParamTS->pInputTS[u16InTSIndex];

    memset(&stOldDest, 0, sizeof(Dest));
    memset(&stDelDest, 0, sizeof(Dest));
    memset(&stAddDest, 0, sizeof(Dest));
    
    //±∏∑›£¨≥ˆ¥Ì ±ªÿÕÀ
    memcpy(&stBackupInputTS, pstInputTS, sizeof(Input_TSStream));
    memcpy(aru8BackupDestSlot, s_aru8DestSlotRecord, sizeof(aru8BackupDestSlot));

    enErrCode = TSP_GetDestInfo(pstInputTS->aru8DestSlot, 
                                pstInputTS->aru32DestChannel, 
                                pstInputTS->aru32BypassFlag,
                                &stOldDest);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetDestInfo Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    enErrCode = TSP_ProcessDest(&stOldDest,
                                pstNewDest,
                                pstInputTS->aru8DestSlot,
                                pstInputTS->aru32DestChannel,
                                pstInputTS->aru32BypassFlag,
                                &stDelDest,
                                &stAddDest,
                                pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_ProcessDest Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        
        goto TSP_ERROR;
    }

    enErrCode = TSP_DelOutputBoardTS(u16InTSIndex, pstParamTS, &stDelDest);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_DelOutputBoardTS Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        goto TSP_ERROR;
    }
    
    enErrCode = TSP_AddTSToOutputBoard(u16InTSIndex, pstParamTS, &stAddDest);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_AddTSToOutputBoard Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        goto TSP_ERROR;
    }

    return WV_SUCCESS;

TSP_ERROR:
    memcpy(pstInputTS, &stBackupInputTS, sizeof(Input_TSStream));
    memcpy(s_aru8DestSlotRecord, aru8BackupDestSlot, sizeof(aru8BackupDestSlot));
    
    return enErrCode;
}

/*****************************************************************************
  Function:     TSP_IsProgSet2BeDescrambled
  Description:  ≈–∂œ ‰»ÎΩ⁄ƒø «∑Ò±ª…Ë÷√¡ÀΩ‚»≈
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                pstParamTS
  Output:       
  Return:       true or false
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IsProgSet2BeDescrambled(U16 u16InProgIndex, TSInfo *pstParamTS)
{
    U8 u8CAMIndex = INVALID_CAM_INDEX;
    
    if((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__,pstParamTS, u16InProgIndex);
        return false;
    }

    u8CAMIndex = pstParamTS->pInputProgram[u16InProgIndex].u8CAMIndex;

    return (u8CAMIndex < WVCI_MAX_SLOT_NUM) ? true : false;
}

/*****************************************************************************
  Function:     TSP_SendReplaceInfoOfPID2DestSlot
  Description:  ∞—“™ÃÊªª‘¥–≈œ¢µƒPID∑¢ÀÕµΩ ‰≥ˆ∞Â
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                u8DestSlot: ‰≥ˆ◊”∞Â≤€Œª∫≈
                pstParamTS:
  Output:       
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SendReplaceInfoOfPID2DestSlot(U16 u16InProgIndex, U8 u8DestSlot, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U8 *pu8Buf = NULL;
    U32 u32BufLen = 0;
    IndexInfoList stESIndexList;
    U16 u16PCRIndex = INDEX_INVALID_VALUE;
    U16 u16PIDIndex = INDEX_INVALID_VALUE;
    U16 u16PIDNum = 0;
    Output_TSPID *pstOutPID = NULL;

    U8 u8CAMIndex = INVALID_CAM_INDEX;
    U16 u16StreamID = 0;
    
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    u8CAMIndex = pstParamTS->pInputProgram[u16InProgIndex].u8CAMIndex;

    if (WVCI_MAX_SLOT_NUM <= u8CAMIndex)
    {
        return WV_SUCCESS;
    }

    u16StreamID = TSP_CICamIndex2StreamID(u8CAMIndex);

    u16PCRIndex = pstParamTS->pInputProgram[u16InProgIndex].u16PCRIndex;
    if (u16PCRIndex < pstParamTS->u32InputPIDNumber)
    {
        u16PIDNum += 1;
    }

    stESIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetInProgESPIDIndexList(u16InProgIndex, pstParamTS, &stESIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]TSP_GetInProgESPIDIndexList error\r\n",
            __FUNCTION__, __LINE__);
        return enErrCode;
    }

    u16PIDNum += stESIndexList.u16IndexListNum;
    
    if (0 == u16PIDNum)
    {
        return WV_SUCCESS;
    }

    u32BufLen = 1 + sizeof(Output_TSPID) * u16PIDNum;

    pu8Buf = calloc(1, u32BufLen);
    if (!pu8Buf)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc Error,u32BufLen[%u],u16PIDNum[%u]\r\n",
            __FUNCTION__, __LINE__, u32BufLen, u16PIDNum);
        return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
    }
    
    pstOutPID = (Output_TSPID *)(&pu8Buf[1]);

    if (u16PCRIndex < pstParamTS->u32InputPIDNumber)
    {
        TSP_InitOutputPID(pstOutPID, &pstParamTS->pInputPID[u16PCRIndex]);
        pstOutPID->u8SlotID = u8DestSlot;
        pstOutPID->u16SrcCIStreamID = u16StreamID;

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, 
            "PCR[%u],SrcChannel[%u],u16ProgramID[%u],OldPID[%u],SrcCIPID[%u],SrcCISteamID[%u]\r\n",
            u16PCRIndex,
            pstOutPID->u16SourceChannelID,
            pstOutPID->u16ProgramID,
            pstOutPID->u16OldPID,
            pstOutPID->u16SrcCIPID,
            pstOutPID->u16SrcCIStreamID);

        pstOutPID++;
    }

    for (i = 0; i < stESIndexList.u16IndexListNum; i++)
    {
        u16PIDIndex = stESIndexList.aru16IndexList[i];

        TSP_InitOutputPID(pstOutPID, &pstParamTS->pInputPID[u16PIDIndex]);
        pstOutPID->u8SlotID = u8DestSlot;
        pstOutPID->u16PIDID = u16PIDIndex;
        pstOutPID->u16SrcCIStreamID = u16StreamID;

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, 
            "ESPID[%u][%u],SrcChannel[%u],u16ProgramID[%u],OldPID[%u],SrcCIPID[%u],SrcCISteamID[%u]\r\n",
            i,
            u16PIDIndex,
            pstOutPID->u16SourceChannelID,
            pstOutPID->u16ProgramID,
            pstOutPID->u16OldPID,
            pstOutPID->u16SrcCIPID,
            pstOutPID->u16SrcCIStreamID);

        pstOutPID++;
    }

    pu8Buf[0] = u16PIDNum;

    enErrCode = (wvErrCode)IPComm_SendData2Slot(u8DestSlot, IPCOMM_REPLACE_PID_SRC_INFO, pu8Buf, u32BufLen);

    free(pu8Buf);
    pu8Buf = NULL;

    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendData2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SendSetDescrambledProg2DestSlot
  Description:  ∞—“™…Ë÷√Ω‚»≈µƒΩ⁄ƒø–≈œ¢∑¢ÀÕµΩ ‰≥ˆ∞Â
  Input:        pstProgList:»°…Ë÷√»≈µƒΩ⁄ƒø¡¥±Ì
                u8DestSlot: ‰≥ˆ∞Â≤€∫≈
                pstParamTS
  Output:       
  Return:       success or error code
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SendSetDescrambledProg2DestSlot(IndexInfoList *pstProgList, U8 u8DestSlot, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16InProgIndex = INDEX_INVALID_VALUE;
    U32 u32BufLen = 0;
    U8 *pu8Buf = NULL;

    Output_TSProgram *pstOutProg = NULL;

    if ((!pstProgList) || (!pstParamTS))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstProgList[%p],pstParamTS[%p]\r\n",
            __FUNCTION__, __LINE__, pstProgList, pstParamTS);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == pstProgList->u16IndexListNum)
    {
        return WV_SUCCESS;
    }

    u32BufLen = 1 + (sizeof(Output_TSProgram) * pstProgList->u16IndexListNum);

    pu8Buf = calloc(1, u32BufLen);
    if (!pu8Buf)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc Error,u32BufLen[%u]\r\n", __FUNCTION__, __LINE__, u32BufLen);
        return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
    }

    pstOutProg = (Output_TSProgram *)(&pu8Buf[1]);

    for (i = 0; i < pstProgList->u16IndexListNum; i++)
    {
        u16InProgIndex = pstProgList->aru16IndexList[i];

        TSP_InitOutputProg(pstOutProg, &pstParamTS->pInputProgram[u16InProgIndex]);
        pstOutProg->u8SlotID = u8DestSlot;

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, 
            "OutProg[%u],SrcSlot[%u],SrcChannel[%u],OldServiceID[%u],u8CAMIndex[%u]\r\n",
            i,
            pstOutProg->u8SourceSlotID,
            pstOutProg->u16SourceChannelID,
            pstOutProg->u16OldServiceID,
            pstOutProg->u8CAMIndex);

        pstOutProg++;
    }

    pu8Buf[0] = pstProgList->u16IndexListNum;

    enErrCode = (wvErrCode)IPComm_SendData2Slot(u8DestSlot, IPCOMM_PROG_SET_DESCRAMBLE, pu8Buf, u32BufLen);

    free(pu8Buf);
    pu8Buf = NULL;

    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendData2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    
        return enErrCode;
    }

    for (i = 0; i < pstProgList->u16IndexListNum; i++)
    {
        u16InProgIndex = pstProgList->aru16IndexList[i];

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, 
            "InProg[%u][%u],Channel[%u],ServiceID[%u]\r\n",
            i,
            u16InProgIndex,
            pstParamTS->pInputProgram[u16InProgIndex].u16ChannelID,
            pstParamTS->pInputProgram[u16InProgIndex].u16ServiceID);
        
        enErrCode = TSP_SendReplaceInfoOfPID2DestSlot(u16InProgIndex, u8DestSlot, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_SendReplaceInfoOfPID2DestSlot error,enErrCode[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
        
            return enErrCode;
        }
    }

    enErrCode = IPComm_SendCmd2Slot(u8DestSlot, IPCOMM_SEND_DONE_FLAG);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendCmd2Slot error,enErrCode[%u],u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode, u8DestSlot);

        return enErrCode;
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SendUnsetDescrambledProg2DestSlot
  Description:  ∞—“™»°œ˚Ω‚»≈µƒΩ⁄ƒø–≈œ¢∑¢ÀÕµΩ ‰≥ˆ∞Â
  Input:        pstProgList:»°œ˚Ω‚»≈µƒΩ⁄ƒø¡¥±Ì
                u8DestSlot: ‰≥ˆ∞Â≤€∫≈
                pstParamTS
  Output:       
  Return:       success or error code
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SendUnsetDescrambledProg2DestSlot(IndexInfoList *pstProgList, U8 u8DestSlot, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16InProgIndex = INDEX_INVALID_VALUE;
    U32 u32BufLen = 0;
    U8 *pu8Buf = NULL;

    Output_TSProgram *pstOutProg = NULL;

    if ((!pstProgList) || (!pstParamTS))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstProgList[%p],pstParamTS[%p]\r\n",
            __FUNCTION__, __LINE__, pstProgList, pstParamTS);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == pstProgList->u16IndexListNum)
    {
        return WV_SUCCESS;
    }

    u32BufLen = 1 + (sizeof(Output_TSProgram) * pstProgList->u16IndexListNum);

    pu8Buf = calloc(1, u32BufLen);
    if (!pu8Buf)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]calloc Error,u32BufLen[%u]\r\n", __FUNCTION__, __LINE__, u32BufLen);
        return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
    }

    pstOutProg = (Output_TSProgram *)(&pu8Buf[1]);

    for (i = 0; i < pstProgList->u16IndexListNum; i++)
    {
        u16InProgIndex = pstProgList->aru16IndexList[i];

        TSP_InitOutputProg(pstOutProg, &pstParamTS->pInputProgram[u16InProgIndex]);
        pstOutProg->u8SlotID = u8DestSlot;

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, 
            "OutProg[%u],SrcSlot[%u],SrcChannel[%u],OldServiceID[%u],u8CAMIndex[%u]\r\n",
            i,
            pstOutProg->u8SourceSlotID,
            pstOutProg->u16SourceChannelID,
            pstOutProg->u16OldServiceID,
            pstOutProg->u8CAMIndex);

        pstOutProg++;
    }

    pu8Buf[0] = pstProgList->u16IndexListNum;

    enErrCode = (wvErrCode)IPComm_SendData2Slot(u8DestSlot, IPCOMM_PROG_UNSET_DESCRAMBLE, pu8Buf, u32BufLen);

    free(pu8Buf);
    pu8Buf = NULL;

    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendData2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    
        return enErrCode;
    }

    enErrCode = IPComm_SendCmd2Slot(u8DestSlot, IPCOMM_SEND_DONE_FLAG);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendCmd2Slot error,enErrCode[%u],u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode, u8DestSlot);

        return enErrCode;
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_DelAllCaDescriptors
  Description:  …æ≥˝Ω⁄ƒø–≈œ¢÷–ÃÌÀ˘”–µƒCA√Ë ˆ◊”
  Input:        aru8Descriptors:
                pu16Len:
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_DelAllCaDescriptors(U8 aru8Descriptors[MAX_DESCRIPTOR_LENGTH], U16 *pu16Len)
{
    U16 u16HadParseLen = 0;
    U8 u8DescriptorTag = 0;
    U8 u8DescriptorLen = 0;
    U8 *pu8Descriptor = aru8Descriptors;
    U16 u16Len = 0;
    
    if ((!aru8Descriptors) || (!pu16Len))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:aru8Descriptors[%p],pu16Len[%p]\r\n",
            __FUNCTION__, __LINE__, aru8Descriptors, pu16Len);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    u16Len = *pu16Len;

    while (u16HadParseLen < u16Len)
    {
        u8DescriptorTag = pu8Descriptor[0];
        u8DescriptorLen = pu8Descriptor[1];

        if (CA_DESCRIPTOR_TAG == u8DescriptorTag)
        {
            memmove(pu8Descriptor, pu8Descriptor + 2 + u8DescriptorLen, u16Len - u16HadParseLen - 2 - u8DescriptorLen);
            u16Len -= (2 + u8DescriptorLen);
        }
        else
        {
            pu8Descriptor += (2 + u8DescriptorLen);
            u16HadParseLen += (2 + u8DescriptorLen);
        }
    }

    *pu16Len = u16Len;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_AddCaDescriptor2ProgInfo
  Description:  œÚΩ⁄ƒø–≈œ¢÷–ÃÌº”“ª∏ˆCA√Ë ˆ◊”
  Input:        u16CaSystemID:
                u16CAPID:
                aru8Descriptors:
                pu16Len:
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddCaDescriptor2ProgInfo(U16 u16CaSystemID, U16 u16CAPID, U8 aru8Descriptors[MAX_DESCRIPTOR_LENGTH], U16 *pu16Len)
{
    U16 u16TempLen = 0;
    
    if ((!aru8Descriptors) || (!pu16Len))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:aru8Descriptors[%p],pu16Len[%p]\r\n",
            __FUNCTION__, __LINE__, aru8Descriptors, pu16Len);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    u16TempLen = *pu16Len;

    if (u16TempLen + 2 + 4 > MAX_DESCRIPTOR_LENGTH)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:u16TempLen[%u]\r\n",
            __FUNCTION__, __LINE__, u16TempLen);
        return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
    }

    aru8Descriptors[u16TempLen++] = CA_DESCRIPTOR_TAG;
    aru8Descriptors[u16TempLen++] = 4;
    aru8Descriptors[u16TempLen++] = (u16CaSystemID >> 8) & 0xFF;
    aru8Descriptors[u16TempLen++] = u16CaSystemID & 0xFF;
    aru8Descriptors[u16TempLen++] = 0xE0 | ((u16CAPID >> 8) & 0x1F);
    aru8Descriptors[u16TempLen++] = u16CAPID & 0xFF;

    *pu16Len = u16TempLen;
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ChangeCADescriptorsOfOutProg
  Description:  ∏¸∏ƒ ‰≥ˆΩ⁄ƒø÷–µƒCA√Ë ˆ◊”£¨PSECMPID/ESECMPID≥ÂÕª∫ÛPIDø…ƒ‹∑¢…˙∏ƒ±‰£¨
                ¥À ±Ω⁄ƒø–≈œ¢ªÚES–≈œ¢÷–µƒCAPID“™∏ƒŒ™≥ÂÕªΩœ—È∫ÛµƒPID
  Input:        u16OutProgIndex: ‰≥ˆΩ⁄ƒøÀ˜“˝
                pstParamTS
  Output:       
  Return:       success or error code
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ChangeCADescriptorsOfOutProg(U16 u16OutProgIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 j = 0;
    IndexInfoList stIndexList;
    IndexInfoList stECMIndexList;
    Output_TSProgram *pOutProg = NULL;
    Output_TSPID *pOutPID = NULL;
    U16 u16ESIndex = INDEX_INVALID_VALUE;
    
    if ((!pstParamTS) || (pstParamTS->u32OutputProgramNumber <= u16OutProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16OutProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    pOutProg = &pstParamTS->pOutputProgram[u16OutProgIndex];

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetOutProgESPIDIndexList(u16OutProgIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutProgESPIDIndexList Error,u16OutProgIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16OutProgIndex, enErrCode);
        return enErrCode;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16ESIndex = stIndexList.aru16IndexList[i];

        pOutPID = &pstParamTS->pOutputPID[u16ESIndex];

        stECMIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetOutPIDESECMPIDIndexList(u16ESIndex, pstParamTS, &stECMIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetOutPIDESECMPIDIndexList Error,u16ESIndex[%u],enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, u16ESIndex, enErrCode);
            return enErrCode;
        }

        if (0 != stECMIndexList.u16IndexListNum)
        {
            (void)TSP_DelAllCaDescriptors(pOutPID->aru8ESInfo, &pOutPID->u16ESInfoLength);
        }
        
        for (j = 0; j < stECMIndexList.u16IndexListNum; j++)
        {
            (void)TSP_AddCaDescriptor2ProgInfo(pstParamTS->pOutputPID[stECMIndexList.aru16IndexList[j]].u16CASID,
                                                pstParamTS->pOutputPID[stECMIndexList.aru16IndexList[j]].u16PID,
                                                pOutPID->aru8ESInfo,
                                                &pOutPID->u16ESInfoLength);
        }
    }

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetOutProgPSECMPIDIndexList(u16OutProgIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutProgPSECMPIDIndexList Error,u16OutProgIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16OutProgIndex, enErrCode);
        return enErrCode;
    }

    if (0 != stIndexList.u16IndexListNum)
    {
        (void)TSP_DelAllCaDescriptors(pOutProg->aru8ProgramInfo, &pOutProg->u16ProgramInfoLength);
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        (void)TSP_AddCaDescriptor2ProgInfo(pstParamTS->pOutputPID[stIndexList.aru16IndexList[i]].u16CASID,
                                            pstParamTS->pOutputPID[stIndexList.aru16IndexList[i]].u16PID,
                                            pOutProg->aru8ProgramInfo,
                                            &pOutProg->u16ProgramInfoLength);
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SendSetDescrambledProg2OutputBoard
  Description:  ∞—“™…Ë÷√ªÚ“™»°œ˚Ω‚»≈µƒΩ⁄ƒø–≈œ¢∑¢ÀÕµΩ ‰≥ˆ∞Â
  Input:        bSetFlag:…Ë÷√±Í÷æ
                pstProgList:“™…Ë÷√ªÚ»°œ˚Ω‚»≈µƒΩ⁄ƒø¡¥±Ì
                pstParamTS
  Output:       
  Return:       success or error code
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SendDescrambledProg2OutputBoard(BOOL bSetFlag, IndexInfoList *pstProgList, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16InTSIndex = INDEX_INVALID_VALUE;
    U16 u16InProgIndex = INDEX_INVALID_VALUE;
    U8 u8DestSlot = 0;
    IndexInfoList stIndexList;

    if ((!pstProgList) || (!pstParamTS))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstProgList[%p],pstParamTS[%p]\r\n",
            __FUNCTION__, __LINE__, pstProgList, pstParamTS);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == pstProgList->u16IndexListNum)
    {
        return WV_SUCCESS;
    }

    for (u8DestSlot = 0; u8DestSlot < MAX_SLOT_NUM; u8DestSlot++)
    {
        stIndexList.u16IndexListNum = 0;
        
        for (i = 0; i < pstProgList->u16IndexListNum; i++)
        {
            u16InProgIndex = pstProgList->aru16IndexList[i];
            u16InTSIndex = pstParamTS->pInputProgram[u16InProgIndex].u16TSIndex;

            if (!TSP_IfInputProgOutputs2DestSlot(u16InProgIndex, pstParamTS, u8DestSlot)
                && !TSP_IfInputTSMuxes2DestSlot(u16InTSIndex, pstParamTS, u8DestSlot))
            {
                continue;
            }

            stIndexList.aru16IndexList[stIndexList.u16IndexListNum++] = u16InProgIndex;
        }

        if (bSetFlag)
        {
            enErrCode = TSP_SendSetDescrambledProg2DestSlot(&stIndexList, u8DestSlot, pstParamTS);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_SendSetDescrambledProg2DestSlot Error,u8DestSlot[%u],enErrCode[%08X]\r\n",
                    __FUNCTION__, __LINE__, u8DestSlot, enErrCode);
                return enErrCode;
            }
        }
        else
        {
            enErrCode = TSP_SendUnsetDescrambledProg2DestSlot(&stIndexList, u8DestSlot, pstParamTS);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_SendUnsetDescrambledProg2DestSlot Error,u8DestSlot[%u],enErrCode[%08X]\r\n",
                    __FUNCTION__, __LINE__, u8DestSlot, enErrCode);
                return enErrCode;
            }
        }
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ProcessDescramble
  Description:  …Ë÷√Ω‚»≈Ω⁄ƒø
  Input:        arstDescrambleProgInfo:“™…Ë÷√Ω‚»≈µƒΩ⁄ƒøµƒΩ‚»≈≤Œ ˝–≈œ¢¡¥±Ì
                u16DescrambleProgNum:“™…Ë÷√Ω‚»≈µƒΩ⁄ƒø∏ˆ ˝
                pstParamTS
  Output:       
  Return:       success or error code
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ProcessDescramble(DescrambleProgInfo arstDescrambleProgInfoList[], U16 u16DescrambleProgNum, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16InTSIndex = INDEX_INVALID_VALUE;
    U16 u16OutTSIndex = INDEX_INVALID_VALUE;
    U16 u16Channel = 0;
    U16 u16ServiceID = 0;
    U8 u8OldCAMIndex = INVALID_CAM_INDEX;
    U8 u8CAMIndex = INVALID_CAM_INDEX;
    U16 u16InProgIndex = INDEX_INVALID_VALUE;
    U16 u16OutProgIndex = INDEX_INVALID_VALUE;
    U16 u16LastProgIndex = INDEX_INVALID_VALUE;

    IndexInfoList stUnsetProgList;
    IndexInfoList stSetProgList;
    IndexInfoList stOldCAMIndexList;
    
    if ((!arstDescrambleProgInfoList) || (!pstParamTS))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:arstDescrambleProgInfoList[%p],pstParamTS[%p]\r\n",
            __FUNCTION__, __LINE__, arstDescrambleProgInfoList, pstParamTS);
        return WV_ERR_TSP_INPUT_PARAM;
    }

	//ËÆæÁΩÆËæìÂá∫LUT
	/*
    enErrCode = TSP_SetInputLUT(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetInputLUT Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }
    */

	return WV_SUCCESS;

    stUnsetProgList.u16IndexListNum = 0;
    stSetProgList.u16IndexListNum = 0;
    stOldCAMIndexList.u16IndexListNum = 0;

	//ËÆ∞ÂΩïËß£Êâ∞‰ø°ÊÅØ
    for (i = 0; i < u16DescrambleProgNum; i++)
    {
        u16Channel = arstDescrambleProgInfoList[i].u16Channel;
        u16ServiceID = arstDescrambleProgInfoList[i].u16ServiceID;
        u8CAMIndex = arstDescrambleProgInfoList[i].u8CAMIndex;

		//ÈÄöËøáËäÇÁõÆÁöÑÈÄöÈÅìÂè∑ÔºåËé∑ÂèñËäÇÁõÆÂ±û‰∫éÂì™‰∏Ä‰∏™ÊµÅ
        enErrCode = TSP_GetInputTSIndex(u16Channel, pstParamTS, &u16InTSIndex);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetInputTSIndex Error\r\n",
                __FUNCTION__, __LINE__);
            return enErrCode;
        }

		//ÈÄöËøáts index Âíåserver id Ëé∑Âèñprogram index
        enErrCode = TSP_GetInputProgIndex(u16InTSIndex, u16ServiceID, pstParamTS, &u16InProgIndex);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetInputProgIndex Error\r\n",
                __FUNCTION__, __LINE__);
            return enErrCode;
        }

		//ÈÄöËøáËäÇÁõÆÁöÑCAM index Ëé∑ÂèñËØ•ËäÇÁõÆÂéüÊù•ÊòØÂê¶ÊòØËß£Êâ∞ÁöÑ
        u8OldCAMIndex = pstParamTS->pInputProgram[u16InProgIndex].u8CAMIndex;

		//ÂéüÊù•ËäÇÁõÆÊòØËß£Êâ∞ÁöÑ
        if (u8OldCAMIndex < WVCI_MAX_SLOT_NUM)
        {
			//Áé∞Âú®ËøòÊòØËäÇÁõÆËøòÊòØËß£Êâ∞
            if (u8CAMIndex < WVCI_MAX_SLOT_NUM)
            {
				//ËäÇÁõÆËß£Êâ∞ÁöÑCAMÂç°ÂèòÂåñ‰∫Ü
                if (u8CAMIndex != u8OldCAMIndex)
                {
					//ËÆ∞ÂΩïËß£Èô§ÂéüÊù•ËÆæÁΩÆ„ÄÅÈáçÊñ∞ËÆæÁΩÆ
                    stUnsetProgList.aru16IndexList[stUnsetProgList.u16IndexListNum++] = u16InProgIndex;
                    stOldCAMIndexList.aru16IndexList[stOldCAMIndexList.u16IndexListNum++] = u8OldCAMIndex;

                    stSetProgList.aru16IndexList[stSetProgList.u16IndexListNum++] = u16InProgIndex;
                }
            }
            else
            {	//Ëß£Èô§ËäÇÁõÆËß£Êâ∞
                stUnsetProgList.aru16IndexList[stUnsetProgList.u16IndexListNum++] = u16InProgIndex;
                stOldCAMIndexList.aru16IndexList[stOldCAMIndexList.u16IndexListNum++] = u8OldCAMIndex;

                u8CAMIndex = INVALID_CAM_INDEX;
            }
        }
		//ÂéüÊù•ËäÇÁõÆ‰∏çËß£Êâ∞
        else
        {
			//Áé∞Âú®ËäÇÁõÆËß£Êâ∞
            if (u8CAMIndex < WVCI_MAX_SLOT_NUM)
            {
                stSetProgList.aru16IndexList[stSetProgList.u16IndexListNum++] = u16InProgIndex;
            }
            else
            {
                u8CAMIndex = INVALID_CAM_INDEX;
            }
        }

        pstParamTS->pInputProgram[u16InProgIndex].u8CAMIndex = u8CAMIndex;
    }

    //Ëß£Èô§ÂéüÊúâËÆæÁΩÆ
    for (i = 0; i < stUnsetProgList.u16IndexListNum; i++)
    {
        u16InProgIndex = stUnsetProgList.aru16IndexList[i];

        u8CAMIndex = stOldCAMIndexList.aru16IndexList[i];

        enErrCode = TSP_GetOutputTSIndex(u8CAMIndex, pstParamTS, &u16OutTSIndex);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_GetOutputTSIndex Error:enErrCode[%X],u8CAMIndex[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u8CAMIndex);
            return (int)enErrCode;
        }

        enErrCode = TSP_GetOutputProgIndexBySrcProgIndex(u16InProgIndex, u16OutTSIndex, pstParamTS, &u16OutProgIndex);
        if (WV_ERR_TSP_NOT_EXIST == enErrCode)
        {
            continue;
        }
        else if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_GetOutputProgIndexBySrcProg Error:enErrCode[%X],u16InProgIndex[%u],u16OutTSIndex[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16InProgIndex, u16OutTSIndex);
            return (int)enErrCode;
        }

        enErrCode = TSP_DelOutputProg(u16OutProgIndex, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_DelOutputProg Error:enErrCode[%X],u16InProgInde[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16InProgIndex);
            return (int)enErrCode;
        }
    }


    enErrCode = TSP_SendDescrambledProg2OutputBoard(false, &stUnsetProgList, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_SendDescrambledProg2OutputBoard Unset, Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return (int)enErrCode;
    }

    //ÈáçÊñ∞ËÆæÁΩÆËß£Êâ∞
    for (i = 0; i < stSetProgList.u16IndexListNum; i++)
    {
        u16InProgIndex = stSetProgList.aru16IndexList[i];

        u8CAMIndex = pstParamTS->pInputProgram[u16InProgIndex].u8CAMIndex;

        enErrCode = TSP_GetOutputTSIndex(u8CAMIndex, pstParamTS, &u16OutTSIndex);
        if (WV_ERR_TSP_NOT_EXIST == enErrCode)
        {
            enErrCode = TSP_ConstructNullOutputTS(u8CAMIndex, pstParamTS, &u16OutTSIndex);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                   "[%s:%d]TSP_GetUnusedOutputTSIndexList Error:enErrCode[%X],u8CAMIndex[%u]\r\n",
                   __FUNCTION__, __LINE__, enErrCode, u8CAMIndex);
                return (int)enErrCode;
            }
        }
        else if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_GetOutputTSIndex Error:enErrCode[%X],u8CAMIndex[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u8CAMIndex);
            return (int)enErrCode;
        }

        enErrCode = TSP_ConstructOutputTSByAddProg(u16InProgIndex, u16OutTSIndex, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_ConstructOutputTSByAddProg Error:enErrCode[%X],u16InProgIndex[%u],u16OutTSIndex[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16InProgIndex, u16OutTSIndex);
            return (int)enErrCode;
        }

        enErrCode = TSP_GetOutTSLastProgIndex(u16OutTSIndex, pstParamTS, &u16LastProgIndex);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_GetOutTSLastProgIndex Error:enErrCode[%X],u16OutTSIndex[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
            return (int)enErrCode;
        }

        enErrCode = TSP_ChangeCADescriptorsOfOutProg(u16LastProgIndex, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_ChangeCADescriptorsOfOutProg Error:enErrCode[%X],u16LastProgIndex[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16LastProgIndex);
            return (int)enErrCode;
        }
    }

    enErrCode = TSP_SendDescrambledProg2OutputBoard(true, &stSetProgList, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_SendDescrambledProg2OutputBoard set, Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return (int)enErrCode;
    }


	//ÊûÑÂª∫CIË°®
    enErrCode = TSP_ConstructSITableOfCIOutput(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_ConstructSITableOfCIOutput Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return (int)enErrCode;
    }


	//ËøôÊòØÂπ≤ÂòõÁöÑ, 
    enErrCode = TSP_SetCIOutputLUT(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetCIOutputMuxLUT Error\r\n",
            __FUNCTION__, __LINE__);
    }

	//ËøôÊòØÂπ≤Âòõ
	/*
    enErrCode = TSP_SetBypassAndMUXFlag(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetBypassAndMUXFlag Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }


	//ËÆæÁΩÆËæìÂá∫LUT
	
    enErrCode = TSP_SetInputLUT(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetInputLUT Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }
    */

    //(void)WVCI_UpdateSetting(ALL_SLOTS);

    enErrCode = TSP_StoreTSConfiguration();
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_StoreTSConfiguration Error\r\n",
            __FUNCTION__,
            __LINE__);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetDescrambledProgList
  Description:  ªÒ»° ‰»Î¡˜÷–…Ë÷√¡ÀΩ‚»≈µƒÀ˘”–Ω⁄ƒøÀ˜“˝
  Input:        u16InTSIndex: ‰»Î¡˜À˜“˝
                pstParamTS
  Output:       pstIndexList: ‰»Î¡˜÷–…Ë÷√¡ÀΩ‚»≈µƒÀ˘”–Ω⁄ƒøÀ˜“˝¡¥±Ì
  Return:       success or error code
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetDescrambledProgList(U16 u16InTSIndex, TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    if((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex) || (!pstIndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    pstIndexList->u16IndexListNum = 0;

    if (0 == pstParamTS->pInputTS[u16InTSIndex].u16ProgramNum)
    {
        return WV_SUCCESS;
    }

    u16TempIndex = pstParamTS->pInputTS[u16InTSIndex].u16ProgramIndex;

    while (u16TempIndex < pstParamTS->u32InputProgramNumber)
    {
        if (TSP_IsProgSet2BeDescrambled(u16TempIndex, pstParamTS))
        {
            pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum++] = u16TempIndex;
        }

        if (MAX_INDEX_LIST_NUMBER <= pstIndexList->u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:Too many Prog\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }
        
        u16TempIndex = pstParamTS->pInputProgram[u16TempIndex].u16NextIndex;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SetInputLUTIPInfo
  Description:  …Ë÷√ ‰»ÎLUTµƒIP–≈œ¢
  Input:        u8LUTIndex:LUTÀ˜“˝
                u32IP:IPµÿ÷∑
                u16UDPPort:UDP∂Àø⁄∫≈
                aru8MAC:MACµÿ÷∑
  Output:       
  Return:       SUCESS …Ë÷√≥…π¶
                Others …Ë÷√ ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SetInputLUTIPInfo(U8 u8LUTIndex, U32 u32IP, U16 u16UDPPort, U8 aru8MAC[6])
{
    U32 u32MACH = 0;
    U16 u16MACL = 0;
    
    if ((MAX_INPUT_LUT_NUM <= u8LUTIndex) || (!aru8MAC))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:u8LUTIndex[%u],aru8MAC[%p]\r\n",
            __FUNCTION__, __LINE__, u8LUTIndex, aru8MAC);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    FPGA_REG_Write(TSIP_CPU_ADDR, (0x1 << 9) | u8LUTIndex);
    FPGA_REG_Write(TSIP_DAT_H, 0);
    FPGA_REG_Write(TSIP_DAT_L, u32IP);
    FPGA_REG_Write(TSIP_CPU_WREN, 0);
    FPGA_REG_Write(TSIP_CPU_WREN, 1);
    FPGA_REG_Write(TSIP_CPU_WREN, 0);

	//Â∞ÜipÂú∞ÂùÄÊò†Â∞Ñ‰∏∫mac
	LUT_MultiIpMapMAC(u32IP, aru8MAC);

    u32MACH = (aru8MAC[0] << 24) | (aru8MAC[1] << 16) | (aru8MAC[2] << 8) | aru8MAC[3];
    u16MACL = (aru8MAC[4] << 8) | aru8MAC[5]; 

    FPGA_REG_Write(TSIP_CPU_ADDR, (0x1 << 9) | (0x1 << 8) | u8LUTIndex);
    FPGA_REG_Write(TSIP_DAT_H, u32MACH);
    FPGA_REG_Write(TSIP_DAT_L, (u16MACL << 16) | u16UDPPort);
    FPGA_REG_Write(TSIP_CPU_WREN, 0);
    FPGA_REG_Write(TSIP_CPU_WREN, 1);
    FPGA_REG_Write(TSIP_CPU_WREN, 0);

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_UpdateInputLUTInfo
  Description:  ∏¸–¬ ‰»ÎLUTµƒIP–≈œ¢(µ± ‰≥ˆ∞ÂµƒIPªÚ’ﬂMACµÿ÷∑∏ƒ±‰ ±)
  Input:        u8DestSlot: ‰≥ˆ∞Â≤€∫≈
                u32IP:IPµÿ÷∑
                u16UDPPort:UDP∂Àø⁄∫≈
                aru8MAC:MACµÿ÷∑
  Output:       
  Return:       WV_SUCCESS or error code
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_UpdateInputLUTInfo(U8 u8DestSlot, U32 u32IP, U8 aru8MAC[6])
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8DestIndex = 0;
    
    if ((MAX_SLOT_NUM <= u8DestSlot) || (!aru8MAC))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:u8DestSlot[%u]\r\n",
            __FUNCTION__, __LINE__, u8DestSlot);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetDestIndex(u8DestSlot, s_aru8DestSlotRecord, &u8DestIndex);
    if (WV_ERR_TSP_NOT_EXIST == enErrCode)
    {
        return WV_SUCCESS;
    }
    else if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetDestIndex Error:u32Ret[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        return enErrCode;
    }

    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]u8DestSlot[%u] ip info changed,update LUTIPIfno\r\n",
            __FUNCTION__, __LINE__, u8DestSlot);

    enErrCode = TSP_SetInputLUTIPInfo(u8DestIndex, u32IP, s_u16UDPPort, aru8MAC);
    if (WV_SUCCESS != enErrCode) 
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetInputLUTIPInfo Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_InputChannel2StreamID
  Description:    ªÒ»°¬ﬂº≠Õ®µ¿∫≈
  Input:          u16Channel:Õ®µ¿∫≈
  Output:         
  Return:         
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
U16 TSP_InputChannel2StreamID(U16 u16Channel)
{
    //∂‘”⁄CΩ” ’∞Â£¨¬ﬂº≠µƒÕ®µ¿∫≈¥”0xFCø™ º
    return 0xFC + u16Channel;
}

/*****************************************************************************
  Function:       TSP_CICamIndex2StreamID
  Description:    ªÒ»°CIµƒCAMø®∂‘”¶µƒ¬ﬂº≠StreamId
  Input:          u16CamIndex:CAMø®À˜“˝
  Return:         
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
U16 TSP_CICamIndex2StreamID(U16 u16CamIndex)
{
    //∂‘”⁄CI£¨¬ﬂº≠µƒÕ®µ¿∫≈¥”0xFAø™ º
    return 0xFA + u16CamIndex;
}

BOOL TSP_IsInputLUTEntryAlreadyExist(INPUTLutEntry *pstEntry, INPUTLutEntry arstLUTEntry[MAX_INPUT_LUT_ENTRY_NUM], U16 u16EntryNum)
{
    U16 i = 0;
    
    if ((!pstEntry) || (!arstLUTEntry) || (u16EntryNum > MAX_INPUT_LUT_ENTRY_NUM))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstEntry[%p],arstLUTEntry[%p],u16EntryNum[%u]\r\n",
            __FUNCTION__, __LINE__, pstEntry, arstLUTEntry, u16EntryNum);
        return false;
    }

    for (i = 0; i < u16EntryNum; i++)
    {
        if (0 == memcmp(pstEntry, &arstLUTEntry[i], sizeof(INPUTLutEntry)))
        {
            return true;
        }
    }

    return false;
}

wvErrCode TSP_GetInputLUTEntry(U8 u8DestSlot, TSInfo *pstParamTS, INPUTLutEntry arstLUTEntry[MAX_INPUT_LUT_ENTRY_NUM], U16 *pu16EntryNum)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 j = 0;
    U16 k = 0;
    U16 u16InTSIndex = INDEX_INVALID_VALUE;
    U16 u16OutTSIndex = INDEX_INVALID_VALUE;
    U16 u16EntryNum = 0;
    IndexInfoList stIndexList;
    IndexInfoList stESIndexList;
    IndexInfoList stECMIndexList;
    U16 u16SrcTSIndex = INDEX_INVALID_VALUE;
    U16 u16SrcProgIndex = INDEX_INVALID_VALUE;
    U16 u16Index = INDEX_INVALID_VALUE;
    U16 u16PIDIndex = INDEX_INVALID_VALUE;
    U16 u16StreamID = 0;
    INPUTLutEntry stTempEntry;
    U8 u8CamIndex = 0;
    U16 u16Channel = 0;
    BOOL bInputTSMux2DestSlotFlag = false;
    
    if ((!pstParamTS) || (!arstLUTEntry) || (!pu16EntryNum))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],arstLUTEntry[%p],pu16EntryNum[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, arstLUTEntry, pu16EntryNum);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    *pu16EntryNum = 0;
	//TODO
	//just test
	{
		PIDINFO stPIDInfo;
		//Ëé∑Âèñstream id
		u16StreamID = TSP_CICamIndex2StreamID(u8DestSlot);
		//Ëé∑ÂèñÊüê‰∏™ËæìÂá∫ÈÄöÈÅìÁöÑÊâÄÊúâPID
		TSP_GetAllPIDInChannel(u8DestSlot, 0, &stPIDInfo);
		int index = 0;
		for(index = 0; index < stPIDInfo.u16PIDNum; ++index)
		{
			arstLUTEntry[index].u16SrcStream = u16StreamID;
			arstLUTEntry[index].u16SrcPID =stPIDInfo.aru16PIDList[index];
		}
		*pu16EntryNum = stPIDInfo.u16PIDNum;
	}
    
    for (u8CamIndex = 0; u8CamIndex < WVCI_MAX_SLOT_NUM; u8CamIndex++)
    {
        if (WV_SUCCESS != TSP_GetOutputTSIndex(u8CamIndex, pstParamTS, &u16OutTSIndex))
        {
			printf("No Output Ts...\n");
            continue;
        }

        u16StreamID = TSP_CICamIndex2StreamID(u8CamIndex);

        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetOutTSProgIndexList(u16OutTSIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode) 
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetOutTSProgIndexList Error:enErrCode[%08X],u16OutTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);

            return enErrCode;
        }

		printf("stIndexList.u16IndexListNum = %u\n", stIndexList.u16IndexListNum);

        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            u16Index = stIndexList.aru16IndexList[i];

            u16SrcProgIndex = pstParamTS->pOutputProgram[u16Index].u16ProgramID;
            u16SrcTSIndex = pstParamTS->pInputProgram[u16SrcProgIndex].u16TSIndex;

			//ËæìÂÖ•ËäÇÁõÆÂíåËæìÂÖ•ÊµÅÁöÑËæìÂá∫Âú∞ÂùÄÂåπÈÖç
            if ((!TSP_IfInputProgOutputs2DestSlot(u16SrcProgIndex, pstParamTS, u8DestSlot))
                && (!TSP_IfInputTSOutputs2DestSlot(u16SrcTSIndex, pstParamTS, u8DestSlot)))
            {
                continue;
            }

			//ËäÇÁõÆÁöÑPCR PID
            stTempEntry.u16SrcStream = u16StreamID;
            stTempEntry.u16SrcPID = pstParamTS->pOutputProgram[u16Index].u16PCRPID;

			//Entry ÊòØÂê¶Â∑≤ÁªèÂ≠òÂú®
            if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
            {
                memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
            }

			//Ëé∑ÂèñES PID
            stESIndexList.u16IndexListNum = 0;
            enErrCode = TSP_GetOutProgESPIDIndexList(u16Index, pstParamTS, &stESIndexList);
            if (WV_SUCCESS != enErrCode) 
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetOutProgESPIDIndexList Error:enErrCode[%08X],u16Index[%u]\r\n",
                    __FUNCTION__, __LINE__, enErrCode, u16Index);

                return enErrCode;
            }

            for (j = 0; j < stESIndexList.u16IndexListNum; j++)
            {
                u16PIDIndex = stESIndexList.aru16IndexList[j];
                
                stTempEntry.u16SrcStream = u16StreamID;
                stTempEntry.u16SrcPID = pstParamTS->pOutputPID[u16PIDIndex].u16PID;
    
                if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                    && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
                {
                    memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
                }
            }
        }
    }

    for (u16Channel = 0; u16Channel < MAX_INPUT_CHANNEL_NUM; u16Channel++)
    {
        if (WV_SUCCESS != TSP_GetInputTSIndex(u16Channel, pstParamTS, &u16InTSIndex))
        {
            continue;
        }

        if (TSP_IfInputTSBypasses2DestSlot(u16InTSIndex, pstParamTS, u8DestSlot))
        {
            continue;
        }

        u16StreamID = TSP_InputChannel2StreamID(u16Channel);

        bInputTSMux2DestSlotFlag = TSP_IfInputTSMuxes2DestSlot(u16InTSIndex, pstParamTS, u8DestSlot);

        //Prog
        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode) 
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetInTSProgIndexList Error:enErrCode[%08X],u16InTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u16InTSIndex);

            return enErrCode;
        }

        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            u16Index = stIndexList.aru16IndexList[i];

            if ((!TSP_IfInputProgOutputs2DestSlot(u16Index, pstParamTS, u8DestSlot))
                && (!bInputTSMux2DestSlotFlag))
            {
                continue;
            }

            if (TSP_IsProgSet2BeDescrambled(u16Index, pstParamTS))
            {
                continue;
            }

            stTempEntry.u16SrcStream = u16StreamID;
            stTempEntry.u16SrcPID = pstParamTS->pInputProgram[u16Index].u16PCRPID;

            if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
            {
                memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
            }

            stESIndexList.u16IndexListNum = 0;
            enErrCode = TSP_GetInProgESPIDIndexList(u16Index, pstParamTS, &stESIndexList);
            if (WV_SUCCESS != enErrCode) 
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetInProgESPIDIndexList Error:enErrCode[%08X],u16Index[%u]\r\n",
                    __FUNCTION__, __LINE__, enErrCode, u16Index);

                return enErrCode;
            }

            for (j = 0; j < stESIndexList.u16IndexListNum; j++)
            {
                u16PIDIndex = stESIndexList.aru16IndexList[j];
                
                stTempEntry.u16SrcStream = u16StreamID;
                stTempEntry.u16SrcPID = pstParamTS->pInputPID[u16PIDIndex].u16PID;
    
                if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                    && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
                {
                    memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
                }

                stECMIndexList.u16IndexListNum = 0;
                enErrCode = TSP_GetInPIDESECMPIDIndexList(u16PIDIndex, pstParamTS, &stECMIndexList);
                if (WV_SUCCESS != enErrCode) 
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]TSP_GetInPIDESECMPIDIndexList Error:enErrCode[%08X],u16PIDIndex[%u]\r\n",
                        __FUNCTION__, __LINE__, enErrCode, u16PIDIndex);

                    return enErrCode;
                }

                for (k = 0; k < stECMIndexList.u16IndexListNum; k++)
                {
                    u16PIDIndex = stECMIndexList.aru16IndexList[k];
                    
                    stTempEntry.u16SrcStream = u16StreamID;
                    stTempEntry.u16SrcPID = pstParamTS->pInputPID[u16PIDIndex].u16PID;
        
                    if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                        && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
                    {
                        memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
                    }
                }
            }

            stECMIndexList.u16IndexListNum = 0;
            enErrCode = TSP_GetInProgPSECMPIDIndexList(u16Index, pstParamTS, &stECMIndexList);
            if (WV_SUCCESS != enErrCode) 
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetInProgPSECMPIDIndexList Error:enErrCode[%08X],u16Index[%u]\r\n",
                    __FUNCTION__, __LINE__, enErrCode, u16Index);

                return enErrCode;
            }

            for (j = 0; j < stECMIndexList.u16IndexListNum; j++)
            {
                u16PIDIndex = stECMIndexList.aru16IndexList[j];
                
                stTempEntry.u16SrcStream = u16StreamID;
                stTempEntry.u16SrcPID = pstParamTS->pInputPID[u16PIDIndex].u16PID;
    
                if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                    && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
                {
                    memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
                }
            }
        }

        if (!bInputTSMux2DestSlotFlag)
        {
            continue;
        }
        
        //EMM
        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInTSEMMPIDIndexList(u16InTSIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode) 
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetInTSEMMPIDIndexList Error:enErrCode[%08X],u16InTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u16InTSIndex);

            return enErrCode;
        }

        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            u16Index = stIndexList.aru16IndexList[i];
            
            stTempEntry.u16SrcStream = u16StreamID;
            stTempEntry.u16SrcPID = pstParamTS->pInputPID[u16Index].u16PID;

            if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
            {
                memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
            }
        }

        //OtherPID
        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInTSOtherPIDIndexList(u16InTSIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode) 
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetInTSOtherPIDIndexList Error:enErrCode[%08X],u16InTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u16InTSIndex);

            return enErrCode;
        }

        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            u16Index = stIndexList.aru16IndexList[i];
            
            stTempEntry.u16SrcStream = u16StreamID;
            stTempEntry.u16SrcPID = pstParamTS->pInputPID[u16Index].u16PID;

            if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
            {
                memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
            }
        }
    }

    *pu16EntryNum = u16EntryNum;

    return WV_SUCCESS;
}

void TSP_SortInputLUTEntry(INPUTLutEntry arstLUTEntry[MAX_INPUT_LUT_ENTRY_NUM], U16 u16EntryNum)
{
    U16 i = 0;
    U16 j = 0;
    U32 u32TempData = 0;
    U32 u32TempData2 = 0;

    INPUTLutEntry stTempEntry;
    
    if ((!arstLUTEntry) || (u16EntryNum > MAX_INPUT_LUT_ENTRY_NUM))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:arstLUTEntry[%p],u16EntryNum[%u]\r\n",
            __FUNCTION__, __LINE__, arstLUTEntry, u16EntryNum);
        return;
    }
        
    for (i = 0; i < u16EntryNum; i++)
    {
        for (j = 0; j < (u16EntryNum - i - 1); j++)
        {
            u32TempData = ((arstLUTEntry[j].u16SrcStream & 0x3FF) << 13)
                        | (arstLUTEntry[j].u16SrcPID & 0x1FFF);

            u32TempData2 = ((arstLUTEntry[j + 1].u16SrcStream & 0x3FF) << 13)
                        | (arstLUTEntry[j + 1].u16SrcPID & 0x1FFF);
                
            if (u32TempData > u32TempData2)
            {
                stTempEntry = arstLUTEntry[j];
                arstLUTEntry[j] = arstLUTEntry[j + 1];
                arstLUTEntry[j + 1] = stTempEntry;
            }
        }
    }
}

/*****************************************************************************
  Function:     TSP_SetInputLUT
  Description:  …Ë÷√ ‰»ÎLUT
  Input:        pstParamTS
  Output:       
  Return:       WV_SUCCESS …Ë÷√≥…π¶
                Others …Ë÷√ ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SetInputLUT(TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U8 u8LUTIndex = 0;
    U8 u8DestSlot = 0xFF;
    U32 u32LUTData = 0;
    U32 u32IPAddr = 0;
    U16 u16UDPPort = 0;
    U8 aru8MAC[6] = {0};

    INPUTLutEntry arstLUTEntry[MAX_INPUT_LUT_ENTRY_NUM];
    U16 u16EntryNum = 0;

    if (!pstParamTS)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    for (u8LUTIndex = 0; u8LUTIndex < MAX_INPUT_LUT_NUM; u8LUTIndex++)
    {
        u8DestSlot = s_aru8DestSlotRecord[u8LUTIndex];

		u8DestSlot = u8LUTIndex;
	
        //if (TSP_IsLegalSlot(u8DestSlot))
        
        if(1 == 1)
        {
            u32IPAddr = Resource_getBoardIPAddress(u8DestSlot);
            Resource_getMACAddress(u8DestSlot, aru8MAC);
			u32IPAddr = 0xE30A145A + u8LUTIndex;
            u16UDPPort = s_u16UDPPort;

            if ((0xA0 != aru8MAC[0]) || (0x69 != aru8MAC[1]) || (0x86 != aru8MAC[2]))
            {
                aru8MAC[0] = 0x00;
                aru8MAC[1] = 0x0a;
                aru8MAC[2] = 0x35;
                aru8MAC[3] = 0x00;
                aru8MAC[4] = 0x01;
                aru8MAC[5] = 0x22 + u8LUTIndex;
            }

            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, 
                "TSP_SetInputLUT:u8LUTIndex[%u],u8DestSlot[%u],IP[%u.%u.%u.%u],MAC[%02X%02X%02X%02X%02X%02X]\r\n",
                u8LUTIndex,
                u8DestSlot,
                (u32IPAddr >> 24) & 0xFF, (u32IPAddr >> 16) & 0xFF, (u32IPAddr >> 8) & 0xFF, u32IPAddr & 0xFF,
                aru8MAC[0], aru8MAC[1], aru8MAC[2], aru8MAC[3], aru8MAC[4], aru8MAC[5]);
        }
        else
        {
            u32IPAddr = 0;
            u16UDPPort = 0;
            memset(aru8MAC, 0, sizeof(aru8MAC));
        }

		//ËÆæÁΩÆËæìÂá∫IP MAC ÂíåÁ´ØÂè£
        enErrCode = TSP_SetInputLUTIPInfo(u8LUTIndex, u32IPAddr, u16UDPPort, aru8MAC);
        if (WV_SUCCESS != enErrCode) 
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_SetInputLUTIPInfo Error:enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);

            return enErrCode;
        }

        //if (!TSP_IsLegalSlot(u8DestSlot))
        //FIXME
        if(1 == 0)
        {
            //clear LUT
            for (i = 0; i < MAX_INPUT_LUT_ENTRY_NUM; i++)
            {
                FPGA_REG_Write(LUT_ADDR, (u8LUTIndex << 12) | i);
                FPGA_REG_Write(LUT_DATA, 0);
                FPGA_REG_Write(LUT_WRITE_EN, 0);
                FPGA_REG_Write(LUT_WRITE_EN, 1);
                FPGA_REG_Write(LUT_WRITE_EN, 0);
            }

            FPGA_REG_Write(LUT_SWAP, 0);
            FPGA_REG_Write(LUT_SWAP, 1);
            FPGA_REG_Write(LUT_SWAP, 0);

            continue;
        }

        u16EntryNum = 0;
        memset(arstLUTEntry, 0, sizeof(arstLUTEntry));
        
        enErrCode = TSP_GetInputLUTEntry(u8DestSlot, pstParamTS, arstLUTEntry, &u16EntryNum);
        if (WV_SUCCESS != enErrCode) 
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_SetInputLUTIPInfo Error:enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);

            return enErrCode;
        }
        
        TSP_SortInputLUTEntry(arstLUTEntry, u16EntryNum);

        printf("TSP_SetInputLUT:u8LUTIndex[%u],u16EntryNum[%u]\r\n", u8LUTIndex, u16EntryNum);

        for (i = 0; i < u16EntryNum; i++)
        {
            printf("[%4u],u16SrcStream[%3u],u16SrcPID[%u]\r\n",
                i, arstLUTEntry[i].u16SrcStream, arstLUTEntry[i].u16SrcPID);
        }

        for (i = 0; i < u16EntryNum; i++)
        {
            u32LUTData = (1 << 23)
                        | ((arstLUTEntry[i].u16SrcStream & 0x3FF) << 13)
                        | (arstLUTEntry[i].u16SrcPID & 0x1FFF);
            
            FPGA_REG_Write(LUT_ADDR, (u8LUTIndex << 12) | i);
            FPGA_REG_Write(LUT_DATA, u32LUTData);
            FPGA_REG_Write(LUT_WRITE_EN, 0);
            FPGA_REG_Write(LUT_WRITE_EN, 1);
            FPGA_REG_Write(LUT_WRITE_EN, 0);
        }

        for (i = u16EntryNum; i < MAX_INPUT_LUT_ENTRY_NUM; i++)
        {
            FPGA_REG_Write(LUT_ADDR, (u8LUTIndex << 12) | i);
            FPGA_REG_Write(LUT_DATA, 0);
            FPGA_REG_Write(LUT_WRITE_EN, 0);
            FPGA_REG_Write(LUT_WRITE_EN, 1);
            FPGA_REG_Write(LUT_WRITE_EN, 0);
        }

        FPGA_REG_Write(LUT_SWAP, 0);
        FPGA_REG_Write(LUT_SWAP, 1);
        FPGA_REG_Write(LUT_SWAP, 0);
    }

    return WV_SUCCESS;
}



void TSP_ClearAllBypassAndMUXFlag(U8 u8LUTIndex)
{
    U16 i = 0;
    U16 u16StreamID = 0;
    
    if (u8LUTIndex >= MAX_INPUT_LUT_NUM)
    {
        return;
    }

    for (i = 0; i < MAX_INPUT_CHANNEL_NUM; i++)
    {
        u16StreamID = TSP_InputChannel2StreamID(i);
        
        FPGA_REG_Write(LUT_ADDR, (u8LUTIndex << 12) | (0x1 << 11) | u16StreamID);
        FPGA_REG_Write(LUT_DATA, 0);
        FPGA_REG_Write(LUT_WRITE_EN, 0);
        FPGA_REG_Write(LUT_WRITE_EN, 1);
        FPGA_REG_Write(LUT_WRITE_EN, 0);
    }

    for (i = 0; i < WVCI_MAX_SLOT_NUM; i++)
    {
        u16StreamID = TSP_CICamIndex2StreamID(i);
        
        FPGA_REG_Write(LUT_ADDR, (u8LUTIndex << 12) | (0x1 << 11) | u16StreamID);
        FPGA_REG_Write(LUT_DATA, 0);
        FPGA_REG_Write(LUT_WRITE_EN, 0);
        FPGA_REG_Write(LUT_WRITE_EN, 1);
        FPGA_REG_Write(LUT_WRITE_EN, 0);
    }
}

BOOL TSP_IsCINeed2Mux2DestSlot(U8 u8CamIndex, U8 u8DestSlot, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16SrcProgIndex = INDEX_INVALID_VALUE;
    U16 u16SrcTSIndex = INDEX_INVALID_VALUE;
    U16 u16ProgIndex = INDEX_INVALID_VALUE;
    U16 u16OutTSIndex = INDEX_INVALID_VALUE;
    IndexInfoList stProgIndexList;
    
    if ((WVCI_MAX_SLOT_NUM <= u8CamIndex) || (!pstParamTS))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:u8CamIndex[%u],pstParamTS[%p]\r\n",
            __FUNCTION__, __LINE__, u8CamIndex, pstParamTS);
        return false;
    }
    
    if (WV_SUCCESS != TSP_GetOutputTSIndex(u8CamIndex, pstParamTS, &u16OutTSIndex))
    {
        return false;
    }

    stProgIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetOutTSProgIndexList(u16OutTSIndex, pstParamTS, &stProgIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutTSProgIndexList Error:u16OutTSIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex, enErrCode);
        return false;
    }

    for (i = 0; i < stProgIndexList.u16IndexListNum; i++)
    {
        u16ProgIndex = stProgIndexList.aru16IndexList[i];

        u16SrcProgIndex = pstParamTS->pOutputProgram[u16ProgIndex].u16ProgramID;
        u16SrcTSIndex = pstParamTS->pInputProgram[u16SrcProgIndex].u16TSIndex;

        if (TSP_IfInputProgOutputs2DestSlot(u16SrcProgIndex, pstParamTS, u8DestSlot)
            || TSP_IfInputTSMuxes2DestSlot(u16SrcTSIndex, pstParamTS, u8DestSlot))
        {
            return true;
        }
    }

    return false;
}

/*****************************************************************************
  Function:     TSP_GetInputTSBypassAndMuxFlag
  Description:  ªÒ»° ‰»Î¡˜¿≠µΩ ‰≥ˆ ±µƒ÷±Õ®/∏¥”√±Í÷æ
  Input:        u8DestSlot: ‰≥ˆ◊”∞Â≤€Œª∫≈
                u16InTSIndex: ‰»Î¡˜À˜“˝
                pstParamTS
  Output:       pbBypassFlag:÷±Õ®±Í÷æ
                pbMUXFlag:∏¥”√±Í÷æ
  Return:       WV_SUCCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetBypassAndMuxFlagOfInputTS(U8 u8DestSlot,
                                        U16 u16InTSIndex,
                                        TSInfo *pstParamTS,
                                        BOOL *pbBypassFlag,
                                        BOOL *pbMUXFlag)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    IndexInfoList stIndexList;
    BOOL bBypassFlag = false;
    BOOL bMUXFlag = false;
    BOOL bTSMuxFlag = false;
    BOOL bProgMuxFlag = false;

    U16 u16InProgIndex = INDEX_INVALID_VALUE;
    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex)
        || (!pbBypassFlag) || (!pbMUXFlag))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16InTSIndex[%u],pbBypassFlag[%p],pbMUXFlag[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex, pbBypassFlag, pbMUXFlag);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    bBypassFlag = TSP_IfInputTSBypasses2DestSlot(u16InTSIndex, pstParamTS, u8DestSlot);
    bTSMuxFlag = TSP_IfInputTSMuxes2DestSlot(u16InTSIndex, pstParamTS, u8DestSlot);

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode) 
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSProgIndexList Error:u16InTSIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16InTSIndex, enErrCode);

        return enErrCode;
    }

    //…Ë÷√¡ÀΩ‚»≈µƒΩ⁄ƒø£¨∆‰TS∞¸ª·”…∂‘”¶µƒ¥”CI ‰≥ˆµƒTS∞¸ÃÊªª ‰≥ˆ»•£¨“Ú¥À
    //»Áπ˚’˚∏ˆ¡˜µƒÀ˘”––Ë“™∏¥”√≥ˆ»•µƒΩ⁄ƒø∂º…Ë÷√¡ÀΩ‚»≈£¨’‚∏ˆ¡˜≤ª–Ë“™…Ë÷√∏¥”√±Í÷æ°£
    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16InProgIndex = stIndexList.aru16IndexList[i];

        bProgMuxFlag = TSP_IfInputProgOutputs2DestSlot(u16InProgIndex, pstParamTS, u8DestSlot);
        
        if ((bTSMuxFlag || bProgMuxFlag) && (!TSP_IsProgSet2BeDescrambled(u16InProgIndex, pstParamTS)))
        {
            bMUXFlag = true;
            break;
        }
    }

    *pbBypassFlag = bBypassFlag;
    *pbMUXFlag = bMUXFlag;
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SetBypassAndMUXFlag
  Description:  …Ë÷√∞¸Õ∑÷±Õ®/∏¥”√±Í÷æ
  Input:        pstParamTS
  Output:       
  Return:       WV_SUCCESS …Ë÷√≥…π¶
                Others …Ë÷√ ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SetBypassAndMUXFlag(TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U8 u8LUTIndex = 0;
    U8 u8DestSlot = MAX_SLOT_NUM;
    U16 u16StreamID = 0;
    U16 u16InTSIndex = INDEX_INVALID_VALUE;
    BOOL bBypassFlag = false;
    BOOL bMUXFlag = false;

    if (!pstParamTS)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    for (u8LUTIndex = 0; u8LUTIndex < MAX_INPUT_LUT_NUM; u8LUTIndex++)
    {
        u8DestSlot = s_aru8DestSlotRecord[u8LUTIndex];

        if (!TSP_IsLegalSlot(u8DestSlot))
        {
            TSP_ClearAllBypassAndMUXFlag(u8LUTIndex);
            continue;
        }

        for (i = 0; i < MAX_INPUT_CHANNEL_NUM; i++)
        {
            bBypassFlag = false;
            bMUXFlag = false;

            u16StreamID = TSP_InputChannel2StreamID(i);

            enErrCode = TSP_GetInputTSIndex(i, pstParamTS, &u16InTSIndex);
            if (WV_SUCCESS == enErrCode)
            {
                enErrCode = TSP_GetBypassAndMuxFlagOfInputTS(u8DestSlot, u16InTSIndex, pstParamTS, &bBypassFlag, &bMUXFlag);
                if (WV_SUCCESS != enErrCode)
                {
                    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
                        "[%s:%d]TSP_GetInputTSBypassAndMuxFlag error,enErrCode[%08X]\r\n",
                        __FUNCTION__, __LINE__, enErrCode);
                    
                    bBypassFlag = false;
                    bMUXFlag = false;
                }
            }
            
            FPGA_REG_Write(LUT_ADDR, (u8LUTIndex << 12) | (0x1 << 11) | u16StreamID);
            FPGA_REG_Write(LUT_DATA, (bBypassFlag << 1) | bMUXFlag);
            FPGA_REG_Write(LUT_WRITE_EN, 0);
            FPGA_REG_Write(LUT_WRITE_EN, 1);
            FPGA_REG_Write(LUT_WRITE_EN, 0);
        }

        for (i = 0; i < WVCI_MAX_SLOT_NUM; i++)
        {
            u16StreamID = TSP_CICamIndex2StreamID(i);
            
            bMUXFlag = TSP_IsCINeed2Mux2DestSlot(i, u8DestSlot, pstParamTS);
            
            FPGA_REG_Write(LUT_ADDR, (u8LUTIndex << 12) | (0x1 << 11) | u16StreamID);
            FPGA_REG_Write(LUT_DATA, bMUXFlag);
            FPGA_REG_Write(LUT_WRITE_EN, 0);
            FPGA_REG_Write(LUT_WRITE_EN, 1);
            FPGA_REG_Write(LUT_WRITE_EN, 0);
        }
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_DebugPrintLUTInfo
  Description:  ¥Ú”° ‰»ÎLUT
  Input:        u8LUTIndex:LUTÀ˜“˝
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_DebugPrintLUTInfo(U8 u8LUTIndex)
{
    U16 i = 0;
    U32 aru32LUTData[MAX_INPUT_LUT_ENTRY_NUM] = {0};

    if (MAX_INPUT_LUT_NUM <= u8LUTIndex)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:u8LUTIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u8LUTIndex);
        return;
    }
    
    for (i = 0; i < MAX_INPUT_LUT_ENTRY_NUM; i++)
    {
        FPGA_REG_Write(LUT_ADDR, (u8LUTIndex << 12) | i);
        FPGA_REG_Read(LUT_READ_BACK, &aru32LUTData[i]);
    }

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "Start print input LUT info:u8LUTIndex[%u]\r\n", u8LUTIndex);
    
    for (i = 0; i < MAX_INPUT_LUT_ENTRY_NUM; i++)
    {
        if (0 == aru32LUTData[i])
        {
            continue;
        }

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "Addr[%08X],Data[%08X]\r\n", ((u8LUTIndex << 12) | i), aru32LUTData[i]);
    }
    
    for (i = 0; i < MAX_OUTPUT_CHANNEL_NUM; i++)
    {
        FPGA_REG_Write(LUT_ADDR, (u8LUTIndex << 12) | (0x1 << 11) | i);
        FPGA_REG_Read(LUT_READ_BACK, &aru32LUTData[i]);
    }

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "Start print Bypass and MUX Flag info:\r\n");
    
    for (i = 0; i < MAX_OUTPUT_CHANNEL_NUM; i++)
    {
        if (0 == aru32LUTData[i])
        {
            continue;
        }

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "Addr[%08X],Data[%08X]\r\n", ((u8LUTIndex << 12) | (0x1 << 11) | i), aru32LUTData[i]);
    }

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "Start print Dest IP info:\r\n");

    
    FPGA_REG_Write(TSIP_CPU_ADDR, (0x1 << 9) | u8LUTIndex);
    FPGA_REG_Read(TSIP_CPU_RBK_L, &aru32LUTData[0]);

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "Addr[%08X],IP[%u.%u.%u.%u]\r\n",
            (0x1 << 9) | u8LUTIndex,
            (aru32LUTData[0] >> 24) & 0xFF,
            (aru32LUTData[0] >> 16) & 0xFF,
            (aru32LUTData[0] >> 8) & 0xFF,
            aru32LUTData[0] & 0xFF);

    FPGA_REG_Write(TSIP_CPU_ADDR, (0x1 << 9) | (0x1 << 8) | u8LUTIndex);
    FPGA_REG_Read(TSIP_CPU_RBK_H, &aru32LUTData[0]);
    FPGA_REG_Read(TSIP_CPU_RBK_L, &aru32LUTData[1]);

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "Addr[%08X],MAC[%02X%02X%02X%02X%02X%02X],Port[%u]\r\n",
            (0x1 << 9) | (0x1 << 8) | u8LUTIndex,
            (aru32LUTData[0] >> 24) & 0xFF,
            (aru32LUTData[0] >> 16) & 0xFF,
            (aru32LUTData[0] >> 8) & 0xFF,
            aru32LUTData[0] & 0xFF,
            (aru32LUTData[1] >> 24) & 0xFF,
            (aru32LUTData[1] >> 16) & 0xFF,
            aru32LUTData[1]& 0xFFFF);
    
    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "End print Dest IP info:\r\n");
}

/*****************************************************************************
  Function:     TSP_RestoreDestSlotRecord
  Description:  ª÷∏¥»•œÚº«¬º–≈œ¢
  Input:        
  Output:       
  Return:       WV_SUCCESS or error code
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_RestoreDestSlotRecord(U8 *pu8DestSlot)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;
    U8 u8DestSlot = 0xFF;
    U8 u8DestSlotIndex = 0;
    
    for (i = 0; i < MAX_DEST; i++)
    {
        u8DestSlot = pu8DestSlot[i];
        if (MAX_SLOT_NUM <= u8DestSlot)
        {
            continue;
        }

        enErrCode = TSP_GetDestIndex(u8DestSlot, s_aru8DestSlotRecord, &u8DestSlotIndex);
        if (WV_ERR_TSP_NOT_EXIST == enErrCode)
        {
            enErrCode = TSP_GetUnusedDestIndex(s_aru8DestSlotRecord, &u8DestSlotIndex);
            if (WV_SUCCESS == enErrCode)
            {
                s_aru8DestSlotRecord[u8DestSlotIndex] = u8DestSlot;
            }
            else if (WV_ERR_TSP_RESOURCE_NOT_ENOUGH == enErrCode)
            {
                log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetUnusedDestIndex error,enErrCode[%08X]\r\n",
                    __FUNCTION__, __LINE__, enErrCode);
            }
        }
        else if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetDestIndex error,enErrCode[%08X],u8DestSlot[%u][%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, i, u8DestSlot);
            return enErrCode;
        }
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_RestoreTSMUXInfo
  Description:  ª÷∏¥∏¥”√–≈œ¢
  Input:        
  Output:       
  Return:       WV_SUCCESS or error code
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode  TSP_RestoreTSMUXInfo(void)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 u16InTSIndex = 0;
    U16 u16InProgIndex = INDEX_INVALID_VALUE;
    TSInfo *pstParamTS = NULL;
    U16 i = 0;
    IndexInfoList stProgIndexList;
    pstParamTS = TSP_GetTSParamHandle();

	/*
    TSP_BypassAllTS2CIOutput();
    
    enErrCode = TSP_ConstructSITableOfCIOutput(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "[%s:%d]TSP_ConstructSITableOfCIOutput error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }
    */

    for (u16InTSIndex = 0; u16InTSIndex < pstParamTS->u32InputTSNumber; u16InTSIndex++)
    {
        if (TSP_FLAG_VALID != pstParamTS->pInputTS[u16InTSIndex].u8ValidFlag)
        {
            continue;
        }

        enErrCode = TSP_RestoreDestSlotRecord(pstParamTS->pInputTS[u16InTSIndex].aru8DestSlot);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
                "[%s:%d]TSP_RestoreDestSlotRecord error,enErrCode[%08X],u16InTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u16InTSIndex);
            return enErrCode;
        }

        stProgIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stProgIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetInTSProgIndexList error,enErrCode[%08X],u16InTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u16InTSIndex);
            return enErrCode;
        }

        for (i = 0; i < stProgIndexList.u16IndexListNum; i++)
        {
            u16InProgIndex = stProgIndexList.aru16IndexList[i];
            
            enErrCode = TSP_RestoreDestSlotRecord(pstParamTS->pInputProgram[u16InProgIndex].aru8DestSlot);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
                    "[%s:%d]TSP_RestoreDestSlotRecord error,enErrCode[%08X],u16InTSIndex[%u],Prog[%u][%u]\r\n",
                    __FUNCTION__, __LINE__, enErrCode, u16InTSIndex, i, u16InProgIndex);
                return enErrCode;
            }
        }
    }

	/*
    enErrCode = TSP_SetCIOutputLUT(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetCIOutputLUT error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    enErrCode = TSP_SetBypassAndMUXFlag(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetBypassAndMUXFlag error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }
    */

	/*
    enErrCode = TSP_SetInputLUT(pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "[%s:%d]TSP_SetInputLUT error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }
    */

	//ÈÄâÊã©ËæìÂá∫ÊµÅÂè∑
	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		LUT_OpenStream(i, TSP_InputChannel2StreamID(i));
	}

	//TODO
	//ËÆæÁΩÆËæìÂá∫ÂíåÁ´ØÂè£
    for (i = 0; i < MAX_INPUT_LUT_NUM; i++)
    {		
		ParamsInfoPtr pstParamsInfo = WebXml_GetParamsInfoPtr(i);
	
		//ËÆæÁΩÆËæìÂá∫ÁöÑIP „ÄÅÁ´ØÂè£„ÄÅMAC
		LUT_SetInputLUTIPInfo(i, pstParamsInfo->u32TsIpAddr, pstParamsInfo->u16TsPort);
    }

	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		WVCI_UpdateSetting(i);
	}

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_IsClearTS
  Description:  ≈–∂œ¡˜ «∑Ò±ª«Âø’¡À£¨»Ù±ª«Âø’∫Û“™÷ÿ–¬À—Ã®
  Input:        u8Channel
  Output:       
  Return:       true or false
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IsClearTS(U8 u8Channel)
{
    if (MAX_INPUT_CHANNEL_NUM <= u8Channel)
    {
        return false;
    }

    if (0 == s_aru8ClearTSFlag[u8Channel])
    {
        return false;
    }

    return true;
}


bool TSP_IsFirstScanTS(U8 u8Channel)
{
	static U8 aru8Count[MAX_INPUT_CHANNEL_NUM] = {0};

	bool ret = false;

	if(aru8Count[u8Channel])
	{
		ret = false;
	}
	else
	{
		ret = true;
		aru8Count[u8Channel] = 1;
	}

	return ret;
}

/*****************************************************************************
  Function:     TSP_SetClearTSFlag
  Description:  …Ë÷√«Âø’¡˜±Í÷æ
  Input:        u8Channel
                bFlag
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_SetClearTSFlag(U8 u8Channel, BOOL bFlag)
{
    if (MAX_INPUT_CHANNEL_NUM <= u8Channel)
    {
        return;
    }

    s_aru8ClearTSFlag[u8Channel] = bFlag;
}

/*****************************************************************************
  Function:     TSP_SortPIDList
  Description:  sort source Info from small to large
  Input:        pu16PIDList    srcslot,srcchannel,oldPID
                u16PIDNum      number of SrcInfo
  Output:       pu16IndexList   index of SrcInfo after sort
  Return:       sucess or error code
  Others:       
  Create:       lipeng 20170303
*****************************************************************************/
void TSP_SortPIDList(U16 *pu16PIDList, U16 u16PIDNum, U16 *pu16IndexList)
{
    U32 i = 0;
    U32 j = 0;
    U16 u16Temp = 0;
    
    if ((!pu16PIDList) || (!pu16IndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pu16PIDList[%p],pu16IndexList[%p]\r\n",
            __FUNCTION__, __LINE__, pu16PIDList, pu16IndexList);
        return;
    }

    for (i = 0; i < u16PIDNum; i++)
    {
        pu16IndexList[i] = i;
    }
    
    for (i = 0; i < u16PIDNum; i++)
    {
        for (j = 0; j < (u16PIDNum - i - 1); j++)
        {
            if (pu16PIDList[j] > pu16PIDList[j + 1])
            {
                u16Temp = pu16PIDList[j];
                pu16PIDList[j] = pu16PIDList[j + 1];
                pu16PIDList[j + 1] = u16Temp;
                
                u16Temp = pu16IndexList[j];
                pu16IndexList[j] = pu16IndexList[j + 1];
                pu16IndexList[j + 1] = u16Temp;
            }
        }
    }
}

/*****************************************************************************
  Function:     TSP_GetPIDBitrateInfoIndex
  Description:  ‘⁄PID¬Î¬ –≈œ¢÷–ªÒ»°∂‘”¶PIDµƒÀ˜“˝
  Input:        u16PID
                pstTSBitrateInfo
  Output:       pu16Index
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetPIDBitrateInfoIndex(U16 u16PID, TSBitrateInfo *pstTSBitrateInfo, U16 *pu16Index)
{
    U16 i = 0;

    if ((!pstTSBitrateInfo) || (!pu16Index))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstTSBitrateInfo[%p],pu16Index[%p]\r\n",
            __FUNCTION__, __LINE__, pstTSBitrateInfo, pu16Index);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    for (i = 0; i < pstTSBitrateInfo->u16PIDNum; i++)
    {
        if (u16PID == pstTSBitrateInfo->arstPIDBitrateInfo[i].u16PID)
        {
            *pu16Index = i;
            return WV_SUCCESS;
        }
    }

    return WV_ERR_TSP_NOT_EXIST;
}


/*****************************************************************************
  Function:     TSP_GetProgPIDBitrateInfo
  Description:  ªÒ»°¡˜µƒPIDµƒ¬Î¬ –≈œ¢
  Input:        u8Channel
                u16PID
                pu32Bitrate
                pu8CCNum
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetProgPIDBitrateInfo(Input_TSProgram *pstInProg,
                                    U16 u16PID,
                                    TSBitrateInfo *pstTSBitrateInfo, 
                                    PIDBitrateInfo *pstPIDBitrateInfo)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 u16PIDBitrateInfoIndex = 0;

    if ((!pstInProg) || (!pstTSBitrateInfo) || (!pstPIDBitrateInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstInProg[%p],pstTSBitrateInfo[%p],pstPIDBitrateInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstInProg, pstTSBitrateInfo, pstPIDBitrateInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(pstPIDBitrateInfo, 0, sizeof(PIDBitrateInfo));
    
    enErrCode = TSP_GetPIDBitrateInfoIndex(u16PID, pstTSBitrateInfo, &u16PIDBitrateInfoIndex);
    if (WV_SUCCESS == enErrCode)
    {
        pstPIDBitrateInfo->u32Bitrate =
            pstTSBitrateInfo->arstPIDBitrateInfo[u16PIDBitrateInfoIndex].u32Bitrate;
        pstPIDBitrateInfo->u8CcNum = 
            pstTSBitrateInfo->arstPIDBitrateInfo[u16PIDBitrateInfoIndex].u8CcNum;

        pstTSBitrateInfo->arstPIDBitrateInfo[u16PIDBitrateInfoIndex].u16SamePIDNum += 1;
    }
    else if (WV_ERR_TSP_NOT_EXIST == enErrCode)
    {
        return WV_ERR_TSP_NOT_EXIST;
    }
    else
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]TSP_GetPIDIndexInTSBitrateInfo error:enErrCode[%X],u16PID[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode, u16PID);

        return enErrCode;
    }

    pstPIDBitrateInfo->u16PID = u16PID;
    pstPIDBitrateInfo->u16ServiceID = pstInProg->u16ServiceID;
    pstPIDBitrateInfo->u8ServiceNameLen = pstInProg->u8ServiceNameLen;
    memcpy(pstPIDBitrateInfo->aru8ServiceName, pstInProg->aru8ServiceName, MAX_SDT_NAME_LENGTH);
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetTSPIDBitrateInfo
  Description:  ªÒ»°¡˜µƒPIDµƒ¬Î¬ –≈œ¢
  Input:        u8Channel
                u16PID
                pu32Bitrate
                pu8CCNum
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetTSPIDBitrateInfo(U16 u16PID,
                                    TSBitrateInfo *pstTSBitrateInfo, 
                                    PIDBitrateInfo *pstPIDBitrateInfo)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 u16PIDBitrateInfoIndex = 0;

    if ((!pstTSBitrateInfo) || (!pstPIDBitrateInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstTSBitrateInfo[%p],pstPIDBitrateInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstTSBitrateInfo, pstPIDBitrateInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    enErrCode = TSP_GetPIDBitrateInfoIndex(u16PID, pstTSBitrateInfo, &u16PIDBitrateInfoIndex);
    if (WV_SUCCESS == enErrCode)
    {
        pstPIDBitrateInfo->u32Bitrate =
            pstTSBitrateInfo->arstPIDBitrateInfo[u16PIDBitrateInfoIndex].u32Bitrate;
        pstPIDBitrateInfo->u8CcNum = 
            pstTSBitrateInfo->arstPIDBitrateInfo[u16PIDBitrateInfoIndex].u8CcNum;

        pstTSBitrateInfo->arstPIDBitrateInfo[u16PIDBitrateInfoIndex].u16SamePIDNum += 1;
    }
    else if (WV_ERR_TSP_NOT_EXIST == enErrCode)
    {
        return WV_ERR_TSP_NOT_EXIST;
    }
    else
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]TSP_GetPIDIndexInTSBitrateInfo error:enErrCode[%X],u16PID[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode, u16PID);

        return enErrCode;
    }

    pstPIDBitrateInfo->u16PID = u16PID;
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetChannelBitrateInfo
  Description:  ªÒ»°’˚∏ˆÕ®µ¿µƒÀ˘”–PIDµƒ¬Î¬ ”ÎCC¥ÌŒÛ–≈œ¢
  Input:        u16Channel
  Output:       pstTSBitrateInfo
  Return:       
  Author:       Momouwei 2017.04.17
*****************************************************************************/
void TSP_GetChannelBitrateInfo(U16 u16Channel, TSBitrateInfo *pstTSBitrateInfo)
{
    U16 i = 0;
    U16 u16StreamID = 0;
    U32 u32RegValue = 0;
    U16 u16PIDNum = 0;
    
    if ((MAX_INPUT_CHANNEL_NUM <= u16Channel) || (!pstTSBitrateInfo))
    {
        return;
    }

    u16StreamID = TSP_InputChannel2StreamID(u16Channel);

    FPGA_REG_Read(STR_MMNT_RSID, &u32RegValue);
    
    if (u16StreamID != u32RegValue)
    {
        FPGA_REG_Write(STR_MMNT_RSID, u16StreamID);

        FPGA_REG_Write(STR_MMNT_CLR_CC, 0);
        FPGA_REG_Write(STR_MMNT_CLR_CC, 1);
        FPGA_REG_Write(STR_MMNT_CLR_CC, 0);
        sleep(1);
    }
    
    FPGA_REG_Write(STR_MMNT_ADDR1, u16StreamID);

    memset(pstTSBitrateInfo, 0, sizeof(TSBitrateInfo));

    FPGA_REG_Read(STR_MMNT_TOTLR, &u32RegValue);
    
    pstTSBitrateInfo->u32TotalBitrate = PACKET_2_BIT(u32RegValue);
    
    for (i = 0; i < MAX_PID_NUMBER; i++)
    {
        FPGA_REG_Write(STR_MMNT_ADDR0, i);
        FPGA_REG_Read(STR_MMNT_PID_CC, &u32RegValue);

        if (0 == (u32RegValue & 0x80))
        {
            continue;
        }
        pstTSBitrateInfo->arstPIDBitrateInfo[u16PIDNum].u8CcNum = u32RegValue & 0x7F;
        
        FPGA_REG_Read(STR_MMNT_PID_RATE, &u32RegValue);
        pstTSBitrateInfo->arstPIDBitrateInfo[u16PIDNum].u32Bitrate = PACKET_2_BIT(u32RegValue);
        pstTSBitrateInfo->arstPIDBitrateInfo[u16PIDNum].u16PID = i;
        
        u16PIDNum++;
        if (MAX_PID_NUM_IN_ONE_TS <= u16PIDNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]too many PID\r\n",
                __FUNCTION__, __LINE__);
            break;
        }
    }

    pstTSBitrateInfo->u16PIDNum = u16PIDNum;
}

/*****************************************************************************
  Function:     TSP_GetTSBitrateInfo
  Description:  ªÒ»°¡˜µƒ¬Î¬ –≈œ¢
  Input:        u8Channel
                pstTSBitrateInfo
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetTSBitrateInfo(U16 u16Channel, TSBitrateInfo *pstTSBitrateInfo)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 j = 0;
    U16 k = 0;

    TSInfo *pstParamTS = NULL;
    
    U16 u16PIDNum = 0;
    TSBitrateInfo stTempTSBitrateInfo;
    TSBitrateInfo stTSBitrateInfoWithProgInfo;
    U16 aru16PIDList[MAX_PID_NUM_IN_ONE_TS] = {0};
    U16 aru16IndexList[MAX_PID_NUM_IN_ONE_TS] = {0};

    U16 u16InTSIndex = INDEX_INVALID_VALUE;
    U16 u16Index = INDEX_INVALID_VALUE;
    U16 u16PCRIndex = INDEX_INVALID_VALUE;
    U16 u16PIDIndex = INDEX_INVALID_VALUE;
    U16 u16ECMIndex = INDEX_INVALID_VALUE;
    
    IndexInfoList stIndexList;
    IndexInfoList stECMIndexList;
    IndexInfoList stPIDIndexList;
    
    if ((MAX_INPUT_CHANNEL_NUM <= u16Channel) || (!pstTSBitrateInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstTSBitrateInfo[%p],u16Channel[%u]\r\n",
            __FUNCTION__, __LINE__, pstTSBitrateInfo, u16Channel);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    TSP_GetChannelBitrateInfo(u16Channel, &stTempTSBitrateInfo);
    
    memset(&stTSBitrateInfoWithProgInfo, 0, sizeof(TSBitrateInfo));

    pstParamTS = TSP_GetTSParamHandle();

    enErrCode = TSP_GetInputTSIndex(u16Channel, pstParamTS, &u16InTSIndex);
    if (WV_SUCCESS == enErrCode)
    {
        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_GetInTSProgIndexList:u16InTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, u16InTSIndex);

            return enErrCode;
        }

        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            if (MAX_PID_NUM_IN_ONE_TS <= u16PIDNum)
            {
                break;
            }
            
            u16Index = stIndexList.aru16IndexList[i];
            u16PCRIndex = pstParamTS->pInputProgram[u16Index].u16PCRIndex;

            if (u16PCRIndex < pstParamTS->u32InputPIDNumber)
            {
                aru16PIDList[u16PIDNum] = pstParamTS->pInputProgram[u16Index].u16PCRPID;
                
                enErrCode = TSP_GetProgPIDBitrateInfo(&pstParamTS->pInputProgram[u16Index],
                                                        pstParamTS->pInputProgram[u16Index].u16PCRPID,
                                                        &stTempTSBitrateInfo,
                                                        &stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum]);
                if (WV_SUCCESS == enErrCode)
                {
                    stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum].u8PIDType = PCR_PID;
                    u16PIDNum++;
                }
                else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                        "[%s:%d]TSP_GetProgPIDBitrateInfo error:u16Channel[%u],u16InTSIndex[%u],Prog[%u][%u]\r\n",
                        __FUNCTION__, __LINE__, u16Channel, u16InTSIndex, i, u16Index);
            
                    return enErrCode;
                }
            }


            if (MAX_PID_NUM_IN_ONE_TS <= u16PIDNum)
            {
                break;
            }

            aru16PIDList[u16PIDNum] = pstParamTS->pInputProgram[u16Index].u16PMTPID;
            enErrCode = TSP_GetProgPIDBitrateInfo(&pstParamTS->pInputProgram[u16Index],
                                                    pstParamTS->pInputProgram[u16Index].u16PMTPID,
                                                    &stTempTSBitrateInfo,
                                                    &stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum]);
            if (WV_SUCCESS == enErrCode)
            {
                stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum].u8PIDType = PMT_TABLE_PID;
                u16PIDNum++;
            }
            else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                    "[%s:%d]TSP_GetProgPIDBitrateInfo error:u16Channel[%u],u16InTSIndex[%u],Prog[%u][%u]\r\n",
                    __FUNCTION__, __LINE__, u16Channel, u16InTSIndex, i, u16Index);
        
                return enErrCode;
            }

            stPIDIndexList.u16IndexListNum = 0;
            enErrCode = TSP_GetInProgESPIDIndexList(u16Index, pstParamTS, &stPIDIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                    "[%s:%d]TSP_GetInProgESPIDIndexList error:u16Channel[%u],u16InTSIndex[%u],Prog[%u][%u]\r\n",
                    __FUNCTION__, __LINE__, u16Channel, u16InTSIndex, i, u16Index);
        
                return enErrCode;
            }

            for (j = 0; j < stPIDIndexList.u16IndexListNum; j++)
            {
                if (MAX_PID_NUM_IN_ONE_TS <= u16PIDNum)
                {
                    break;
                }
                
                u16PIDIndex = stPIDIndexList.aru16IndexList[j];
                
                aru16PIDList[u16PIDNum] = pstParamTS->pInputPID[u16PIDIndex].u16PID;
                
                enErrCode = TSP_GetProgPIDBitrateInfo(&pstParamTS->pInputProgram[u16Index],
                                                        pstParamTS->pInputPID[u16PIDIndex].u16PID,
                                                        &stTempTSBitrateInfo,
                                                        &stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum]);
                if (WV_SUCCESS == enErrCode)
                {
                    stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum].u8PIDType = TSP_GetPIDType(u16PIDIndex, pstParamTS);
                    u16PIDNum++;
                }
                else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                        "[%s:%d]TSP_GetProgPIDBitrateInfo error:u16Channel[%u],u16InTSIndex[%u],Prog[%u][%u]\r\n",
                        __FUNCTION__, __LINE__, u16Channel, u16InTSIndex, i, u16Index);
            
                    return enErrCode;
                }

                stECMIndexList.u16IndexListNum = 0;
                enErrCode = TSP_GetInPIDESECMPIDIndexList(u16PIDIndex, pstParamTS, &stECMIndexList);
                if (WV_SUCCESS != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                        "[%s:%d]TSP_GetInPIDESECMPIDIndexList error:u16Channel[%u],u16InTSIndex[%u],Prog[%u][%u],Es[%u][%u]\r\n",
                        __FUNCTION__, __LINE__, u16Channel, u16InTSIndex, i, u16Index, j, u16PIDIndex);
            
                    return enErrCode;
                }

                for (k = 0; k < stECMIndexList.u16IndexListNum; k++)
                {
                    if (MAX_PID_NUM_IN_ONE_TS <= u16PIDNum)
                    {
                        break;
                    }
                    
                    u16ECMIndex = stECMIndexList.aru16IndexList[k];

                    aru16PIDList[u16PIDNum] = pstParamTS->pInputPID[u16ECMIndex].u16PID;

                    enErrCode = TSP_GetProgPIDBitrateInfo(&pstParamTS->pInputProgram[u16Index],
                                                        pstParamTS->pInputPID[u16ECMIndex].u16PID,
                                                        &stTempTSBitrateInfo,
                                                        &stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum]);
                    if (WV_SUCCESS == enErrCode)
                    {
                        stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum].u8PIDType = ESECM_PID;
                        u16PIDNum++;
                    }
                    else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
                    {
                        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                            "[%s:%d]TSP_GetProgPIDBitrateInfo error:u16Channel[%u],u16InTSIndex[%u],Prog[%u][%u]\r\n",
                            __FUNCTION__, __LINE__, u16Channel, u16InTSIndex, i, u16Index);
                
                        return enErrCode;
                    }
                }
            }

            stECMIndexList.u16IndexListNum = 0;
            enErrCode = TSP_GetInProgPSECMPIDIndexList(u16Index, pstParamTS, &stECMIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                    "[%s:%d]TSP_GetInProgPSECMPIDIndexList error:u16Channel[%u],u16InTSIndex[%u],Prog[%u][%u]\r\n",
                    __FUNCTION__, __LINE__, u16Channel, u16InTSIndex, i, u16Index);
        
                return enErrCode;
            }

            for (j = 0; j < stECMIndexList.u16IndexListNum; j++)
            {
                if (MAX_PID_NUM_IN_ONE_TS <= u16PIDNum)
                {
                    break;
                }
                
                u16ECMIndex = stECMIndexList.aru16IndexList[j];

                aru16PIDList[u16PIDNum] = pstParamTS->pInputPID[u16ECMIndex].u16PID;
                
                enErrCode = TSP_GetProgPIDBitrateInfo(&pstParamTS->pInputProgram[u16Index],
                                                        pstParamTS->pInputPID[u16ECMIndex].u16PID,
                                                        &stTempTSBitrateInfo,
                                                        &stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum]);
                if (WV_SUCCESS == enErrCode)
                {
                    stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum].u8PIDType = PSECM_PID;
                    u16PIDNum++;
                }
                else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                        "[%s:%d]TSP_GetProgPIDBitrateInfo error:u16Channel[%u],u16InTSIndex[%u],Prog[%u][%u]\r\n",
                        __FUNCTION__, __LINE__, u16Channel, u16InTSIndex, i, u16Index);
            
                    return enErrCode;
                }
            }
        }

        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInTSEMMPIDIndexList(u16InTSIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_GetInTSEMMPIDIndexList:u16InTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, u16InTSIndex);

            return enErrCode;
        }

        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            if (MAX_PID_NUM_IN_ONE_TS <= u16PIDNum)
            {
                break;
            }
            
            u16Index = stIndexList.aru16IndexList[i];

            aru16PIDList[u16PIDNum] = pstParamTS->pInputPID[u16Index].u16PID;

            enErrCode = TSP_GetTSPIDBitrateInfo(pstParamTS->pInputPID[u16Index].u16PID,
                                                &stTempTSBitrateInfo,
                                                &stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum]);
            if (WV_SUCCESS == enErrCode)
            {
                stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum].u8PIDType = EMM_PID;
                u16PIDNum++;
            }
            else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                    "[%s:%d]TSP_GetTSPIDBitrateInfo error:u16Channel[%u],u16InTSIndex[%u],Prog[%u][%u]\r\n",
                    __FUNCTION__, __LINE__, u16Channel, u16InTSIndex, i, u16Index);
        
                return enErrCode;
            }
        }

        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInTSOtherPIDIndexList(u16InTSIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_GetInTSOtherPIDIndexList:u16InTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, u16InTSIndex);

            return enErrCode;
        }

        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            if (MAX_PID_NUM_IN_ONE_TS <= u16PIDNum)
            {
                break;
            }
            
            u16Index = stIndexList.aru16IndexList[i];

            aru16PIDList[u16PIDNum] = pstParamTS->pInputPID[u16Index].u16PID;
            
            enErrCode = TSP_GetTSPIDBitrateInfo(pstParamTS->pInputPID[u16Index].u16PID,
                                                &stTempTSBitrateInfo,
                                                &stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum]);
            if (WV_SUCCESS == enErrCode)
            {
                stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum].u8PIDType = OTHER_PID;
                u16PIDNum++;
            }
            else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                    "[%s:%d]TSP_GetTSPIDBitrateInfo error:u16Channel[%u],u16InTSIndex[%u],Prog[%u][%u]\r\n",
                    __FUNCTION__, __LINE__, u16Channel, u16InTSIndex, i, u16Index);
        
                return enErrCode;
            }
        }
    }
    else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]TSP_GetInputTSIndex error:u16Channel[%u]\r\n",
            __FUNCTION__, __LINE__, u16Channel);

        return enErrCode;
    }

    for (i = 0; i < stTempTSBitrateInfo.u16PIDNum; i++)
    {
        if (MAX_PID_NUM_IN_ONE_TS <= u16PIDNum)
        {
            break;
        }
        
        if (0 != stTempTSBitrateInfo.arstPIDBitrateInfo[i].u16SamePIDNum)
        {
            continue;
        }

        aru16PIDList[u16PIDNum] = stTempTSBitrateInfo.arstPIDBitrateInfo[i].u16PID;

        memcpy(&stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum],
                &stTempTSBitrateInfo.arstPIDBitrateInfo[i],
                sizeof(PIDBitrateInfo));

        if (PAT_PID == stTempTSBitrateInfo.arstPIDBitrateInfo[i].u16PID)
        {
            stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum].u8PIDType = PAT_TABLE_PID;
        }
        else if (CAT_PID == stTempTSBitrateInfo.arstPIDBitrateInfo[i].u16PID)
        {
            stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum].u8PIDType = CAT_TABLE_PID;
        }
        else if (SDT_PID == stTempTSBitrateInfo.arstPIDBitrateInfo[i].u16PID)
        {
            stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum].u8PIDType = SDT_TABLE_PID;
        }
        else
        {
            stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[u16PIDNum].u8PIDType = OTHER_PID;
        }

        u16PIDNum++;
    }

    memset(pstTSBitrateInfo, 0, sizeof(TSBitrateInfo));

    pstTSBitrateInfo->u16PIDNum = u16PIDNum;
    pstTSBitrateInfo->u32TotalBitrate = stTempTSBitrateInfo.u32TotalBitrate;
    pstTSBitrateInfo->u8ValidFlag = TSP_FLAG_VALID;

    TSP_SortPIDList(aru16PIDList, u16PIDNum, aru16IndexList);

    for (i = 0; i < u16PIDNum; i++)
    {
        memcpy(&pstTSBitrateInfo->arstPIDBitrateInfo[i],
                &stTSBitrateInfoWithProgInfo.arstPIDBitrateInfo[aru16IndexList[i]],
                sizeof(PIDBitrateInfo));
        pstTSBitrateInfo->arstPIDBitrateInfo[i].u16SamePIDNum = 1;
    }

    for (i = 0; i < u16PIDNum; i++)
    {
        for (j = i + 1; j < u16PIDNum; j++)
        {
            if (pstTSBitrateInfo->arstPIDBitrateInfo[i].u16PID != pstTSBitrateInfo->arstPIDBitrateInfo[j].u16PID)
            {
                break;
            }
        }

        pstTSBitrateInfo->arstPIDBitrateInfo[i].u16SamePIDNum = j - i;

        i = j - 1;
    }


    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ClearPIDCC
  Description:  TSP_ClearPIDCC
  Input:        
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_ClearPIDCC(void)
{
    FPGA_REG_Write(STR_MMNT_CLR_CC, 0);
    FPGA_REG_Write(STR_MMNT_CLR_CC, 1);
    FPGA_REG_Write(STR_MMNT_CLR_CC, 0);
}

/*****************************************************************************
  Function:     TSP_GetCIPIDInfo
  Description:  TSP_GetCIPIDInfo
  Input:        
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetCIPIDInfo(U8 u8CAMIndex,
                            PID_INFO_4_CI_t arstPIDInfo[MAX_PID_NUM_FOR_CI_MONITOR],
                            U8 *pu8PIDNum,
                            TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 j = 0;
    U16 u16Channel = 0;
    U16 u16InTSIndex = INDEX_INVALID_VALUE;
    U16 u16ProgIndex = INDEX_INVALID_VALUE;
    U16 u16PIDIndex = INDEX_INVALID_VALUE;
    IndexInfoList stProgIndexList;
    U8 u8PIDNum = 0;

    U32 u32CIPIDDetectAddr = 0;
    U32 u32PIDDescrambleStatusAddr = 0;
    U32 u32PIDDescrambleStatusClearAddr = 0;
    U32 u32PIDDescrambleStatus = 0;

    U8 u8LoopCounter = 0;
    U8 u8TempPIDNum = 0;
    
    if ((WVCI_MAX_SLOT_NUM <= u8CAMIndex) || (!arstPIDInfo) || (!pu8PIDNum) || (!pstParamTS))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:arstPIDInfo[%p],pu8PIDNum[%p],pstParamTS[%p],u8CAMIndex[%u]\r\n",
            __FUNCTION__, __LINE__, arstPIDInfo, pu8PIDNum, pstParamTS, u8CAMIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    for (u16Channel = 0; u16Channel < MAX_INPUT_CHANNEL_NUM; u16Channel++)
    {
        enErrCode = TSP_GetInputTSIndex(u16Channel, pstParamTS, &u16InTSIndex);
        if (WV_SUCCESS != enErrCode)
        {
            continue;
        }

        stProgIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stProgIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetInTSProgIndexList error,enErrCode[%08X],u16InTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u16InTSIndex);

            continue;
        }

        for (i = 0; i < stProgIndexList.u16IndexListNum; i++)
        {
            u16ProgIndex = stProgIndexList.aru16IndexList[i];
            
            if ((pstParamTS->pInputProgram[u16ProgIndex].u8CAMIndex != u8CAMIndex)
                || (0 == pstParamTS->pInputProgram[u16ProgIndex].u16ESNumber))
            {
                continue;
            }

            enErrCode = TSP_GetVideoPIDIndex(u16ProgIndex, pstParamTS, &u16PIDIndex);
            if (WV_ERR_TSP_NOT_EXIST == enErrCode)
            {
                u16PIDIndex = pstParamTS->pInputProgram[u16ProgIndex].u16PIDIndex;
            }
            else if (WV_SUCCESS != enErrCode)
            {
                continue;
            }

            if (u8PIDNum < MAX_PID_NUM_FOR_CI_MONITOR)
            {
                arstPIDInfo[u8PIDNum].u16Channel = u16Channel;
                arstPIDInfo[u8PIDNum].u16ServiceID = pstParamTS->pInputProgram[u16ProgIndex].u16ServiceID;
                memcpy(arstPIDInfo[u8PIDNum].aru8ServiceName,
                        pstParamTS->pInputProgram[u16ProgIndex].aru8ServiceName,
                        MAX_SDT_NAME_LENGTH);
    
                arstPIDInfo[u8PIDNum].u16PID = pstParamTS->pInputPID[u16PIDIndex].u16PID;
                arstPIDInfo[u8PIDNum].u16NewPID = pstParamTS->pInputPID[u16PIDIndex].u16NewPID;
                arstPIDInfo[u8PIDNum].u8PIDType = TSP_GetPIDType(u16PIDIndex, pstParamTS);
    
                u8PIDNum++;
            }
        }
    }

    *pu8PIDNum = u8PIDNum;

    if (0 == u8CAMIndex)
    {
        u32CIPIDDetectAddr = CI_PID_DETECT0_A;
        u32PIDDescrambleStatusAddr = CI0_PES_STS;
        u32PIDDescrambleStatusClearAddr = CI0_PES_CLR;
    }
    else
    {
        u32CIPIDDetectAddr = CI_PID_DETECT0_B;
        u32PIDDescrambleStatusAddr = CI1_PES_STS;
        u32PIDDescrambleStatusClearAddr = CI1_PES_CLR;
    }

    u8LoopCounter = (u8PIDNum / 8) + (0 == (u8PIDNum % 8) ? 0 : 1);

    for (i = 0; i < u8LoopCounter; i++)
    {
        FPGA_REG_Write(u32PIDDescrambleStatusClearAddr, 0);
        FPGA_REG_Write(u32PIDDescrambleStatusClearAddr, 0xFF);
        FPGA_REG_Write(u32PIDDescrambleStatusClearAddr, 0);

        if ((0 == i) || (i == (u8LoopCounter - 1)))
        {
            u8TempPIDNum = u8PIDNum % 8;

            u8TempPIDNum = (0 == u8TempPIDNum ? 8 : u8TempPIDNum);
        }
        else
        {
            u8TempPIDNum = 8;
        }
    
        for (j = 0; j < u8TempPIDNum; j++)
        {
            FPGA_REG_Write(u32CIPIDDetectAddr + j, arstPIDInfo[i + j].u16NewPID);
        }
    
        usleep(200000);
    
        FPGA_REG_Read(u32PIDDescrambleStatusAddr, &u32PIDDescrambleStatus);
    
        for (j = 0; j < u8TempPIDNum; j++)
        {
            if (0 != (u32PIDDescrambleStatus & (0x1 << j)))
            {
                arstPIDInfo[i + j].u8DescrambleStatus = 1;
            }
            else
            {
                arstPIDInfo[i + j].u8DescrambleStatus = 0;
            }
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ClearAllOutputTSInfo
  Description:  «Âø’À˘”– ‰≥ˆ¡˜Ω·ππµƒÀ˘”––≈œ¢
  Input:        pstParamTS:
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_ClearAllOutputTSInfo(TSInfo *pstParamTS)
{
    if (!pstParamTS)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS);
        return;
    }

    if ((0 != pstParamTS->u32OutputTSNumber) && (NULL != pstParamTS->pOutputTS))
    {
        memset(pstParamTS->pOutputTS, 0, pstParamTS->u32OutputTSNumber * sizeof(Output_TSStream));
    }

    if ((0 != pstParamTS->u32OutputProgramNumber) && (NULL != pstParamTS->pOutputProgram))
    {
        memset(pstParamTS->pOutputProgram, 0, pstParamTS->u32OutputProgramNumber * sizeof(Output_TSProgram));
    }

    if ((0 != pstParamTS->u32OutputPIDNumber) && (NULL != pstParamTS->pOutputPID))
    {
        memset(pstParamTS->pOutputPID, 0, pstParamTS->u32OutputPIDNumber * sizeof(Output_TSPID));
    }

    if ((0 != pstParamTS->u32OutputTableNumber) && (NULL != pstParamTS->pOutputTable))
    {
        memset(pstParamTS->pOutputTable, 0, pstParamTS->u32OutputTableNumber * sizeof(TSTable));
    }

    if ((0 != pstParamTS->u32OutputDescriptorNumber) && (NULL != pstParamTS->pOutputDescriptor))
    {
        memset(pstParamTS->pOutputDescriptor, 0, pstParamTS->u32OutputDescriptorNumber * sizeof(TSDescriptor));
    }

    SSD_InitBroadcastSI();
}

/*****************************************************************************
  Function:     TSP_InitOutputTS
  Description:  ≥ı ºªØ ‰»Î¡˜
  Input:        u16InTSIndex: ‰»Î¡˜À˜“˝
                u16Channel:Õ®µ¿
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_InitOutputTS(U16 u16OutTSIndex, U16 u16Channel, TSInfo *pstParamTS)
{
    Output_TSStream *pstOutTS = NULL;

    if ((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutTSIndex);
        return;
    }

    pstOutTS = &pstParamTS->pOutputTS[u16OutTSIndex];

    memset(pstOutTS, 0, sizeof(Output_TSStream));

    pstOutTS->u32StructVersion = TSP_GetStructVersion();
    pstOutTS->u8ValidFlag = TSP_FLAG_VALID;
    pstOutTS->u8SlotID = Status_GetSlotID();
    pstOutTS->u16ChannelID = u16Channel;
    
    pstOutTS->u16ProgramIndex = INDEX_INVALID_VALUE;
    pstOutTS->u16OtherPIDIndex = INDEX_INVALID_VALUE;
    pstOutTS->u16EMMPIDIndex = INDEX_INVALID_VALUE;

    pstOutTS->u16NITIndex = INDEX_INVALID_VALUE;
    pstOutTS->u16BATIndex = INDEX_INVALID_VALUE;
    pstOutTS->u16TDTIndex = INDEX_INVALID_VALUE;
    pstOutTS->u16TOTIndex = INDEX_INVALID_VALUE;
    pstOutTS->u16OtherNITIndex = INDEX_INVALID_VALUE;
    pstOutTS->u16OtherSDTIndex = INDEX_INVALID_VALUE;
    
    return;
}

/*****************************************************************************
  Function:     TSP_GetUnusedOutputTSIndexList
  Description:  —∞’“ø…”√µƒ ‰≥ˆ¡˜µƒÀ˜“˝¡–±Ì
  Input:        u32NeedNum:œ£Õ˚ªÒ»°µΩµƒø…”√∏ˆ ˝
                pstParamTS
  Output:       pstIndexList:ø…”√µƒ¡˜À˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedOutputTSIndexList(IndexInfoList *pstIndexList, TSInfo *pstParamTS, U32 u32NeedNum)
{
    U32 i = 0;

    if ((!pstIndexList) || (!pstParamTS) || (u32NeedNum > MAX_INDEX_LIST_NUMBER))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u32NeedNum[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u32NeedNum);

        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == u32NeedNum)
    {
        return WV_SUCCESS;
    }

    pstIndexList->u16IndexListNum = 0;

    for (i = 0; i < pstParamTS->u32OutputTSNumber; i++)
    {
        if (TSP_FLAG_VALID != pstParamTS->pOutputTS[i].u8ValidFlag)
        {
            pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum] = i;
            pstIndexList->u16IndexListNum += 1;

            if (pstIndexList->u16IndexListNum == u32NeedNum)
            {
                return WV_SUCCESS;
            }
        }
    }

    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
        "[%s:%d]Null TS not enough\r\n", __FUNCTION__, __LINE__);

    return WV_ERR_TSP_RESOURCE_NOT_ENOUGH;
}

/*****************************************************************************
  Function:     TSP_GetUnusedOutputProgIndexList
  Description:  —∞’“ø…”√µƒ ‰≥ˆΩ⁄ƒøµƒÀ˜“˝¡–±Ì
  Input:        u32NeedNum:œ£Õ˚ªÒ»°µΩµƒø…”√∏ˆ ˝
                pstParamTS
  Output:       pstIndexList:ø…”√µƒΩ⁄ƒøÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedOutputProgIndexList(IndexInfoList *pstIndexList, TSInfo *pstParamTS, U32 u32NeedNum)
{
    U32 i = 0;
    Output_TSProgram *pstTempProgList = NULL;

    if ((!pstIndexList) || (!pstParamTS) || (u32NeedNum > MAX_INDEX_LIST_NUMBER))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u32NeedNum[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u32NeedNum);

        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == u32NeedNum)
    {
        return WV_SUCCESS;
    }

    pstIndexList->u16IndexListNum = 0;

    for (i = 0; i < pstParamTS->u32OutputProgramNumber; i++)
    {
        if (TSP_FLAG_VALID != pstParamTS->pOutputProgram[i].u8ValidFlag)
        {
            pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum] = i;
            pstIndexList->u16IndexListNum += 1;

            if (pstIndexList->u16IndexListNum == u32NeedNum)
            {
                return WV_SUCCESS;
            }
        }
    }

    if (pstIndexList->u16IndexListNum < u32NeedNum)
    {
        pstTempProgList = pstParamTS->pOutputProgram;
            
        pstTempProgList = realloc(pstTempProgList,
           (pstParamTS->u32OutputProgramNumber + ALLOCATE_OUTPUT_PROG_NUM_PER_TIME) * sizeof(Output_TSProgram));

        if (!pstTempProgList)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]realloc() fail\r\n", __FUNCTION__, __LINE__);
            
            return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
        }

        pstParamTS->pOutputProgram = pstTempProgList;

        memset(&pstParamTS->pOutputProgram[pstParamTS->u32OutputProgramNumber],
                0,
                ALLOCATE_OUTPUT_PROG_NUM_PER_TIME * sizeof(Output_TSProgram));
        pstParamTS->u32OutputProgramNumber += ALLOCATE_OUTPUT_PROG_NUM_PER_TIME;

        return TSP_GetUnusedOutputProgIndexList(pstIndexList, pstParamTS, u32NeedNum);
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetUnusedOutputPIDIndexList
  Description:  —∞’“ø…”√µƒ ‰≥ˆPIDµƒÀ˜“˝¡–±Ì
  Input:        u32NeedNum:œ£Õ˚ªÒ»°µΩµƒø…”√∏ˆ ˝
                pstParamTS
  Output:       pstIndexList:ø…”√µƒPIDÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedOutputPIDIndexList(IndexInfoList *pstIndexList, TSInfo *pstParamTS, U32 u32NeedNum)
{
    U32 i = 0;
    Output_TSPID *pstTempPIDList = NULL;

    if ((!pstIndexList) || (!pstParamTS) || (u32NeedNum > MAX_INDEX_LIST_NUMBER))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u32NeedNum[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u32NeedNum);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == u32NeedNum)
    {
        return WV_SUCCESS;
    }

    pstIndexList->u16IndexListNum = 0;

    for (i = 0; i < pstParamTS->u32OutputPIDNumber; i++)
    {
        if (TSP_FLAG_VALID != pstParamTS->pOutputPID[i].u8ValidFlag)
        {
            pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum] = i;
            pstIndexList->u16IndexListNum += 1;

            if (pstIndexList->u16IndexListNum == u32NeedNum)
            {
                return WV_SUCCESS;
            }
        }
    }

    if (pstIndexList->u16IndexListNum < u32NeedNum)
    {
        pstTempPIDList = pstParamTS->pOutputPID;
            
        pstTempPIDList = realloc(pstTempPIDList,
           (pstParamTS->u32OutputPIDNumber + ALLOCATE_OUTPUT_PID_NUM_PER_TIME) * sizeof(Output_TSPID));

        if (!pstTempPIDList)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]realloc() fail\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_MEMORY_NOT_ENOUGH;
        }

        pstParamTS->pOutputPID = pstTempPIDList;

        memset(&pstParamTS->pOutputPID[pstParamTS->u32OutputPIDNumber],
                0,
                ALLOCATE_OUTPUT_PID_NUM_PER_TIME * sizeof(Output_TSPID));
        pstParamTS->u32OutputPIDNumber += ALLOCATE_OUTPUT_PID_NUM_PER_TIME;

        return TSP_GetUnusedOutputPIDIndexList(pstIndexList, pstParamTS, u32NeedNum);
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutTSProgIndexList
  Description:  ªÒ»° ‰≥ˆ¡˜µƒÀ˘”–Ω⁄ƒø
  Input:        u16OutTSIndex:ªÒ»°ƒƒ∏ˆ¡˜
                pstParamTS
  Output:       pstIndexList:À˘”–Ω⁄ƒøÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutTSProgIndexList(U16 u16OutTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    if((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex) || (!pstIndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    pstIndexList->u16IndexListNum = 0;

    if (0 == pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramNum)
    {
        return WV_SUCCESS;
    }

    u16TempIndex = pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramIndex;

    while (u16TempIndex < pstParamTS->u32OutputProgramNumber)
    {
        pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum++] = u16TempIndex;

        if (MAX_INDEX_LIST_NUMBER <= pstIndexList->u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:u16OutTSIndex[%u],Too many Prog\r\n", __FUNCTION__, __LINE__, u16OutTSIndex);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }
        
        u16TempIndex = pstParamTS->pOutputProgram[u16TempIndex].u16NextProgramIndex;
    }

    if (pstIndexList->u16IndexListNum != pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramNum)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:u16OutTSIndex[%u],u16IndexListNum[%u] != u16ProgramNum[%u]\r\n",
                __FUNCTION__,
                __LINE__,
                u16OutTSIndex,
                pstIndexList->u16IndexListNum,
                pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramNum);
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutTSEMMPIDIndexList
  Description:  ªÒ»° ‰≥ˆ¡˜µƒÀ˘”–EMMPID
  Input:        u16OutTSIndex:ªÒ»°ƒƒ∏ˆ¡˜
                pstParamTS
  Output:       pstIndexList:À˘”–EMMPIDÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutTSEMMPIDIndexList(U16 u16OutTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    if((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex) || (!pstIndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    pstIndexList->u16IndexListNum = 0;

    if (0 == pstParamTS->pOutputTS[u16OutTSIndex].u16EMMPIDNumber)
    {
        return WV_SUCCESS;
    }

    u16TempIndex = pstParamTS->pOutputTS[u16OutTSIndex].u16EMMPIDIndex;

    while (u16TempIndex < pstParamTS->u32OutputPIDNumber)
    {
        pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum++] = u16TempIndex;

        if (MAX_INDEX_LIST_NUMBER <= pstIndexList->u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:Too many EMMPID\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }
        
        u16TempIndex = pstParamTS->pOutputPID[u16TempIndex].u32NextPIDIndex;
    }

    if (pstIndexList->u16IndexListNum != pstParamTS->pOutputTS[u16OutTSIndex].u16EMMPIDNumber)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:EMMPID Number Not the same,u16IndexListNum[%u],u16EMMPIDNumber[%u]\r\n",
                __FUNCTION__,
                __LINE__,
                pstIndexList->u16IndexListNum,
                pstParamTS->pOutputTS[u16OutTSIndex].u16EMMPIDNumber);
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutTSOtherPIDIndexList
  Description:  ªÒ»° ‰»Î¡˜µƒÀ˘”–OtherPID
  Input:        u16OutTSIndex:ªÒ»°ƒƒ∏ˆ¡˜
                pstParamTS
  Output:       pstIndexList:À˘”–OtherPIDÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutTSOtherPIDIndexList(U16 u16OutTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    if((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex) || (!pstIndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    pstIndexList->u16IndexListNum = 0;

    if (0 == pstParamTS->pOutputTS[u16OutTSIndex].u16OtherPIDNum)
    {
        return WV_SUCCESS;
    }

    u16TempIndex = pstParamTS->pOutputTS[u16OutTSIndex].u16OtherPIDIndex;

    while (u16TempIndex < pstParamTS->u32OutputPIDNumber)
    {
        pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum++] = u16TempIndex;

        if (MAX_INDEX_LIST_NUMBER <= pstIndexList->u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:Too many OtherPID\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }
        
        u16TempIndex = pstParamTS->pOutputPID[u16TempIndex].u32NextPIDIndex;
    }

    if (pstIndexList->u16IndexListNum != pstParamTS->pOutputTS[u16OutTSIndex].u16OtherPIDNum)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:OtherPID Number Not the same,u16IndexListNum[%u],u16OtherPIDNum[%u]\r\n",
                __FUNCTION__,
                __LINE__,
                pstIndexList->u16IndexListNum,
                pstParamTS->pOutputTS[u16OutTSIndex].u16OtherPIDNum);
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutProgESPIDIndexList
  Description:  ªÒ»° ‰»ÎΩ⁄ƒøµƒÀ˘”–ESPID
  Input:        u16OutProgIndex:ªÒ»°ƒƒ∏ˆΩ⁄ƒø
                pstParamTS
  Output:       pstIndexList:À˘”–ESPIDÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutProgESPIDIndexList(U16 u16OutProgIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    if((!pstParamTS) || (pstParamTS->u32OutputProgramNumber <= u16OutProgIndex) || (!pstIndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u16OutProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u16OutProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    pstIndexList->u16IndexListNum = 0;

    if (0 == pstParamTS->pOutputProgram[u16OutProgIndex].u16ESNumber)
    {
        return WV_SUCCESS;
    }

    u16TempIndex = pstParamTS->pOutputProgram[u16OutProgIndex].u16PIDIndex;

    while (u16TempIndex < pstParamTS->u32OutputPIDNumber)
    {
        pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum++] = u16TempIndex;

        if (MAX_INDEX_LIST_NUMBER <= pstIndexList->u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:Too many ESPID\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }
        
        u16TempIndex = pstParamTS->pOutputPID[u16TempIndex].u32NextPIDIndex;
    }

    if (pstIndexList->u16IndexListNum != pstParamTS->pOutputProgram[u16OutProgIndex].u16ESNumber)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:ESPID Number Not the same,u16IndexListNum[%u],u16ESNumber[%u]\r\n",
                __FUNCTION__,
                __LINE__,
                pstIndexList->u16IndexListNum,
                pstParamTS->pOutputProgram[u16OutProgIndex].u16ESNumber);
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutProgPSECMPIDIndexList
  Description:  ªÒ»° ‰≥ˆΩ⁄ƒøµƒÀ˘”–PSECM
  Input:        u16OutProgIndex:ªÒ»°ƒƒ∏ˆΩ⁄ƒø
                pstParamTS
  Output:       pstIndexList:À˘”–PSECMÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutProgPSECMPIDIndexList(U16 u16OutProgIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    if((!pstParamTS) || (pstParamTS->u32OutputProgramNumber <= u16OutProgIndex) || (!pstIndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u16OutProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u16OutProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    pstIndexList->u16IndexListNum = 0;

    if (0 == pstParamTS->pOutputProgram[u16OutProgIndex].u16PSECMPIDNumber)
    {
        return WV_SUCCESS;
    }

    u16TempIndex = pstParamTS->pOutputProgram[u16OutProgIndex].u16PSECMPIDIndex;

    while (u16TempIndex < pstParamTS->u32OutputPIDNumber)
    {
        pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum++] = u16TempIndex;

        if (MAX_INDEX_LIST_NUMBER <= pstIndexList->u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:Too many PSECMPID\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }
        
        u16TempIndex = pstParamTS->pOutputPID[u16TempIndex].u32NextPIDIndex;
    }

    if (pstIndexList->u16IndexListNum != pstParamTS->pOutputProgram[u16OutProgIndex].u16PSECMPIDNumber)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:PSECMPID Number Not the same,u16IndexListNum[%u],u16PSECMPIDNumber[%u]\r\n",
                __FUNCTION__,
                __LINE__,
                pstIndexList->u16IndexListNum,
                pstParamTS->pOutputProgram[u16OutProgIndex].u16PSECMPIDNumber);
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutPIDESECMPIDIndexList
  Description:  ªÒ»° ‰≥ˆPIDµƒÀ˘”–ESECM
  Input:        u16OutPIDIndex:ªÒ»°ƒƒ∏ˆPID
                pstParamTS
  Output:       pstIndexList:À˘”–ESECMÀ˜“˝µƒºØ∫œ
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutPIDESECMPIDIndexList(U16 u16OutPIDIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    if((!pstParamTS) || (pstParamTS->u32OutputPIDNumber <= u16OutPIDIndex) || (!pstIndexList))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstIndexList[%p],pstParamTS[%p],u16OutPIDIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstIndexList, pstParamTS, u16OutPIDIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    pstIndexList->u16IndexListNum = 0;

    if (0 == pstParamTS->pOutputPID[u16OutPIDIndex].u16ESECMPIDNumber)
    {
        return WV_SUCCESS;
    }

    u16TempIndex = pstParamTS->pOutputPID[u16OutPIDIndex].u16ESECMPIDIndex;

    while (u16TempIndex < pstParamTS->u32OutputPIDNumber)
    {
        pstIndexList->aru16IndexList[pstIndexList->u16IndexListNum++] = u16TempIndex;

        if (MAX_INDEX_LIST_NUMBER <= pstIndexList->u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:Too many ESECMPID\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }
        
        u16TempIndex = pstParamTS->pOutputPID[u16TempIndex].u32NextPIDIndex;
    }

    if (pstIndexList->u16IndexListNum != pstParamTS->pOutputPID[u16OutPIDIndex].u16ESECMPIDNumber)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error:ESECMPID Number Not the same,u16IndexListNum[%u],u16ESECMPIDNumber[%u]\r\n",
                __FUNCTION__,
                __LINE__,
                pstIndexList->u16IndexListNum,
                pstParamTS->pOutputPID[u16OutPIDIndex].u16ESECMPIDNumber);
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutProgLastESPIDIndex
  Description:  ªÒ»° ‰≥ˆΩ⁄ƒøµƒ◊Ó∫Û“ª∏ˆESPID
  Input:        u16OutProgIndex:ªÒ»°ƒƒ∏ˆΩ⁄ƒø
                pstParamTS
  Output:       pu16LastIndex:◊Ó∫Û“ª∏ˆESPIDµƒÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚Ω⁄ƒø√ª”–ESPID‘Ú(*pu16LastIndex)Œ™INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutProgLastESPIDIndex(U16 u16OutProgIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexInfoList;

    if((!pstParamTS) || (pstParamTS->u32OutputProgramNumber <= u16OutProgIndex) || (!pu16LastIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16LastIndex[%p],u16OutProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16LastIndex, u16OutProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetOutProgESPIDIndexList(u16OutProgIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutProgESPIDIndexList Error:enErrCode[%08X],u16OutProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode, u16OutProgIndex);
        return enErrCode;
    }

    if (stIndexInfoList.u16IndexListNum == 0)
    {
        *pu16LastIndex = INDEX_INVALID_VALUE;
    }
    else
    {
        *pu16LastIndex = stIndexInfoList.aru16IndexList[stIndexInfoList.u16IndexListNum - 1];
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutProgLastPSECMPIDIndex
  Description:  ªÒ»° ‰≥ˆΩ⁄ƒøµƒ◊Ó∫Û“ª∏ˆPSECM
  Input:        u16InProgIndex:ªÒ»°ƒƒ∏ˆΩ⁄ƒø
                pstParamTS
  Output:       pu16LastIndex:◊Ó∫Û“ª∏ˆPSECMµƒÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚Ω⁄ƒø√ª”–PSECM‘Ú(*pu16LastIndex)Œ™INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutProgLastPSECMPIDIndex(U16 u16OutProgIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexInfoList;

    if((!pstParamTS) || (pstParamTS->u32OutputProgramNumber <= u16OutProgIndex) || (!pu16LastIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16LastIndex[%p],u16OutProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16LastIndex, u16OutProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetOutProgPSECMPIDIndexList(u16OutProgIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutProgPSECMPIDIndexList Error:enErrCode[%08X],u16OutProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode, u16OutProgIndex);
        return enErrCode;
    }

    if (stIndexInfoList.u16IndexListNum == 0)
    {
        *pu16LastIndex = INDEX_INVALID_VALUE;
    }
    else
    {
        *pu16LastIndex = stIndexInfoList.aru16IndexList[stIndexInfoList.u16IndexListNum - 1];
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutPIDLastESECMPIDIndex
  Description:  ªÒ»° ‰≥ˆPIDµƒ◊Ó∫Û“ª∏ˆESECM
  Input:        u16InPIDIndex:ªÒ»°ƒƒ∏ˆPID
                pstParamTS
  Output:       pu16LastIndex:◊Ó∫Û“ª∏ˆESECMµƒÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚PID√ª”–ESECM‘Ú(*pu16LastIndex)Œ™INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutPIDLastESECMPIDIndex(U16 u16OutPIDIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexInfoList;

    if((!pstParamTS) || (pstParamTS->u32OutputPIDNumber <= u16OutPIDIndex) || (!pu16LastIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16LastIndex[%p],u16OutPIDIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16LastIndex, u16OutPIDIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetOutPIDESECMPIDIndexList(u16OutPIDIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutPIDESECMPIDIndexList Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (stIndexInfoList.u16IndexListNum == 0)
    {
        *pu16LastIndex = INDEX_INVALID_VALUE;
    }
    else
    {
        *pu16LastIndex = stIndexInfoList.aru16IndexList[stIndexInfoList.u16IndexListNum - 1];
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutTSLastEMMPIDIndex
  Description:  ªÒ»° ‰≥ˆ¡˜µƒ◊Ó∫Û“ª∏ˆEMM
  Input:        u16InTSIndex:ªÒ»°ƒƒ∏ˆ¡˜
                pstParamTS
  Output:       pu16LastIndex:◊Ó∫Û“ª∏ˆEMMµƒÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚TS√ª”–EMM‘Ú(*pu16LastIndex)Œ™INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutTSLastEMMPIDIndex(U16 u16OutTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexInfoList;

    if((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex) || (!pu16LastIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16LastIndex[%p],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16LastIndex, u16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetOutTSEMMPIDIndexList(u16OutTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutTSEMMPIDIndexList Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (stIndexInfoList.u16IndexListNum == 0)
    {
        *pu16LastIndex = INDEX_INVALID_VALUE;
    }
    else
    {
        *pu16LastIndex = stIndexInfoList.aru16IndexList[stIndexInfoList.u16IndexListNum - 1];
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutTSLastOtherPIDIndex
  Description:  ªÒ»° ‰≥ˆ¡˜µƒ◊Ó∫Û“ª∏ˆOtherPID
  Input:        u16OutTSIndex:ªÒ»°ƒƒ∏ˆ¡˜
                pstParamTS
  Output:       pu16LastIndex:◊Ó∫Û“ª∏ˆOtherPIDµƒÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚TS√ª”–OtherPID‘Ú(*pu16LastIndex)Œ™INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutTSLastOtherPIDIndex(U16 u16OutTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexInfoList;

    if((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex) || (!pu16LastIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16LastIndex[%p],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16LastIndex, u16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetOutTSOtherPIDIndexList(u16OutTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSOtherPIDIndexList Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (stIndexInfoList.u16IndexListNum == 0)
    {
        *pu16LastIndex = INDEX_INVALID_VALUE;
    }
    else
    {
        *pu16LastIndex = stIndexInfoList.aru16IndexList[stIndexInfoList.u16IndexListNum - 1];
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutTSLastProgIndex
  Description:  ªÒ»° ‰≥ˆ¡˜µƒ◊Ó∫Û“ª∏ˆΩ⁄ƒø
  Input:        u16OutTSIndex:ªÒ»°ƒƒ∏ˆ¡˜
                pstParamTS
  Output:       pu16LastIndex:◊Ó∫Û“ª∏ˆΩ⁄ƒøµƒÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚TS√ª”–Ω⁄ƒø‘Ú(*pu16LastIndex)Œ™INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutTSLastProgIndex(U16 u16OutTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexInfoList;

    if((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex) || (!pu16LastIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16LastIndex[%p],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16LastIndex, u16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetOutTSProgIndexList(u16OutTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutTSProgIndexList Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (stIndexInfoList.u16IndexListNum == 0)
    {
        *pu16LastIndex = INDEX_INVALID_VALUE;
    }
    else
    {
        *pu16LastIndex = stIndexInfoList.aru16IndexList[stIndexInfoList.u16IndexListNum - 1];
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutputTSIndex
  Description:  ªÒ»° ‰≥ˆ¡˜µƒÀ˜“˝
  Input:        u16Channel: ‰»Î¡˜µƒChannel
                pstParamTS
  Output:       pu16OutTSIndex:ªÒ»°µΩµƒ¡˜À˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutputTSIndex(U16 u16Channel, const TSInfo *pstParamTS, U16 *pu16OutTSIndex)
{
    U32 i = 0;

    if ((!pstParamTS) || (!pu16OutTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],pu16OutTSIndex[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    for (i = 0; i < pstParamTS->u32OutputTSNumber; i++)
    {
        if ((TSP_FLAG_VALID == pstParamTS->pOutputTS[i].u8ValidFlag)
            && (u16Channel == pstParamTS->pOutputTS[i].u16ChannelID))
        {
            *pu16OutTSIndex = i;
            return WV_SUCCESS;
        }
    }

    return WV_ERR_TSP_NOT_EXIST;
}

/*****************************************************************************
  Function:     TSP_GetOutputProgIndexBySrcProgIndex
  Description:  Õ®π˝‘¥Ω⁄ƒø‘⁄ ‰≥ˆ¡˜÷–’“∂‘”¶ ‰≥ˆΩ⁄ƒøµƒÀ˜“˝
  Input:        u16SrcProgIndex:‘¥ ‰»ÎΩ⁄ƒøÀ˜“˝
                u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS
  Output:       pu16OutProgIndex:∂‘”¶ ‰≥ˆΩ⁄ƒøµƒÀ˜“˝÷∏’Î
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetOutputProgIndexBySrcProgIndex(U16 u16SrcProgIndex, U16 u16OutTSIndex, const TSInfo *pstParamTS, U16 *pu16OutProgIndex)
{
    U16 u16TempIndex = INDEX_INVALID_VALUE;
    
    if((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex)
        || (!pu16OutProgIndex) || pstParamTS->u32InputProgramNumber <= u16SrcProgIndex)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16OutTSIndex[%u],pu16OutProgIndex[%p],u16SrcProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutTSIndex, pu16OutProgIndex, u16SrcProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    *pu16OutProgIndex = INDEX_INVALID_VALUE;

    u16TempIndex = pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramIndex;

    while (u16TempIndex < pstParamTS->u32OutputProgramNumber)
    {
        if (u16SrcProgIndex == pstParamTS->pOutputProgram[u16TempIndex].u16ProgramID)
        {
            *pu16OutProgIndex = u16TempIndex;
            return WV_SUCCESS;
        }
        
        u16TempIndex = pstParamTS->pOutputProgram[u16TempIndex].u16NextProgramIndex;
    }

    return WV_ERR_TSP_NOT_EXIST;
}

/*****************************************************************************
  Function:     TSP_GetServiceIDUseflagOfOutputTS
  Description:  ªÒ»° ‰≥ˆ¡˜“—æ≠ π”√¡ÀµƒServiceID–≈œ¢
  Input:        u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS
  Output:       aru32ServiceIDUseFlag:ªÒ»°µΩµƒæ≠ π”√¡ÀµƒServiceID–≈œ¢
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetServiceIDUseflagOfOutputTS(U16 u16OutTSIndex, TSInfo *pstParamTS, U32 aru32ServiceIDUseFlag[2048])
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    
    U16 u16ProgIndex = INDEX_INVALID_VALUE;
    U16 u16ServiceID = 0;
    IndexInfoList stIndexInfoList;

    if ((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex)
        || (!aru32ServiceIDUseFlag))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]:Inpput error,aru32ServiceIDUseFlag[%p],pstParamTS[%p],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, aru32ServiceIDUseFlag, pstParamTS, u16OutTSIndex); 
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    stIndexInfoList.u16IndexListNum = 0;
    enErrCode = TSP_GetOutTSProgIndexList(u16OutTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]:TSP_GetOutTSProgIndexList error,enErrCode[%X],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexInfoList.u16IndexListNum; i++)
    {
        u16ProgIndex = stIndexInfoList.aru16IndexList[i];
        
        u16ServiceID = pstParamTS->pOutputProgram[u16ProgIndex].u16ServiceID;
        
        aru32ServiceIDUseFlag[u16ServiceID / 32] |= 1 << (u16ServiceID % 32);
    }

    return WV_SUCCESS;
}


/*****************************************************************************
  Function:     TSP_GetPIDUseFlagOfOutputTS
  Description:  ªÒ»° ‰≥ˆ¡˜“—æ≠ π”√¡ÀµƒPID–≈œ¢
  Input:        u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS
  Output:       arstPIDUseInfo:ªÒ»°µΩµƒæ≠ π”√¡ÀµƒPID–≈œ¢
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetPIDUseFlagOfOutputTS(U16 u16OutTSIndex, TSInfo *pstParamTS, PIDUseInfo arstPIDUseInfo[MAX_PID_NUMBER + 1])
{
    wvErrCode enErrCode = WV_SUCCESS;
    U32 i = 0;
    U32 j = 0; 
    U32 k = 0;

    U16 u16PID = 0;

    U16 u16Index = INDEX_INVALID_VALUE;
    U16 u16ESPIDIndex = INDEX_INVALID_VALUE;
    U16 u16ECMPIDIndex = INDEX_INVALID_VALUE;

    IndexInfoList stIndexList;
    IndexInfoList stESIndexList;
    IndexInfoList stECMIndexList;
    
    if ((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex) || (!arstPIDUseInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16OutTSIndex[%u],arstPIDUseInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutTSIndex, arstPIDUseInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetOutTSProgIndexList(u16OutTSIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetOutTSProgIndexList Error:enErrCode[%X],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16Index = stIndexList.aru16IndexList[i];

        u16PID = pstParamTS->pOutputProgram[u16Index].u16PMTPID;
        if (u16PID < MAX_PID_NUMBER)
        {
            arstPIDUseInfo[u16PID].u8UseFlag = 1;
            arstPIDUseInfo[u16PID].u8SrcSlot = Status_GetSlotID();
            arstPIDUseInfo[u16PID].u16SrcChannel =  pstParamTS->pOutputProgram[u16Index].u16SourceChannelID;
            arstPIDUseInfo[u16PID].u16OldPID = u16PID;
        }

        u16PID = pstParamTS->pOutputProgram[u16Index].u16PCRPID;
        if (u16PID < MAX_PID_NUMBER)
        {
            arstPIDUseInfo[u16PID].u8UseFlag = 1;
            arstPIDUseInfo[u16PID].u8SrcSlot = Status_GetSlotID();
            arstPIDUseInfo[u16PID].u16SrcChannel =  pstParamTS->pOutputProgram[u16Index].u16SourceChannelID;
            arstPIDUseInfo[u16PID].u16OldPID = u16PID;
        }

        stESIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetOutProgESPIDIndexList(u16Index, pstParamTS, &stESIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_GetOutProgESPIDIndexList Error:enErrCode[%X],u16OutTSIndex[%u],Prog[%u][%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex, i, u16Index);
            return enErrCode;
        }

        for (j = 0; j < stESIndexList.u16IndexListNum; j++)
        {
            u16ESPIDIndex = stESIndexList.aru16IndexList[j];

            stECMIndexList.u16IndexListNum = 0;
            enErrCode = TSP_GetOutPIDESECMPIDIndexList(u16ESPIDIndex, pstParamTS, &stECMIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                   "[%s:%d]TSP_GetOutPIDESECMPIDIndexList Error:enErrCode[%X],u16OutTSIndex[%u],Prog[%u][%u],ES[%u][%u]\r\n",
                   __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex, i, u16Index, j, u16ESPIDIndex);
                return enErrCode;
            }

            for (k = 0; k < stECMIndexList.u16IndexListNum; k++)
            {
                u16ECMPIDIndex = stECMIndexList.aru16IndexList[k];
                u16PID = pstParamTS->pOutputPID[u16ECMPIDIndex].u16PID;
                if (u16PID < MAX_PID_NUMBER)
                {
                    arstPIDUseInfo[u16PID].u8UseFlag = 1;
                    arstPIDUseInfo[u16PID].u8SrcSlot = Status_GetSlotID();
                    arstPIDUseInfo[u16PID].u16SrcChannel =  pstParamTS->pOutputPID[u16ECMPIDIndex].u16SourceChannelID;
                    arstPIDUseInfo[u16PID].u16OldPID = u16PID;
                }
            }

            u16PID = pstParamTS->pOutputPID[u16ESPIDIndex].u16PID;
            if (u16PID < MAX_PID_NUMBER)
            {
                arstPIDUseInfo[u16PID].u8UseFlag = 1;
                arstPIDUseInfo[u16PID].u8SrcSlot = Status_GetSlotID();
                arstPIDUseInfo[u16PID].u16SrcChannel =  pstParamTS->pOutputPID[u16ESPIDIndex].u16SourceChannelID;
                arstPIDUseInfo[u16PID].u16OldPID = u16PID;
            }
        }

        stECMIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetOutProgPSECMPIDIndexList(u16Index, pstParamTS, &stECMIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_GetOutProgPSECMPIDIndexList Error:enErrCode[%X],u16OutTSIndex[%u],Prog[%u][%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex, i, u16Index);
            return enErrCode;
        }

        for (j = 0; j < stECMIndexList.u16IndexListNum; j++)
        {
            u16ECMPIDIndex = stECMIndexList.aru16IndexList[j];
            u16PID = pstParamTS->pOutputPID[u16ECMPIDIndex].u16PID;
            if (u16PID < MAX_PID_NUMBER)
            {
                arstPIDUseInfo[u16PID].u8UseFlag = 1;
                arstPIDUseInfo[u16PID].u8SrcSlot = Status_GetSlotID();
                arstPIDUseInfo[u16PID].u16SrcChannel =  pstParamTS->pOutputPID[u16ECMPIDIndex].u16SourceChannelID;
                arstPIDUseInfo[u16PID].u16OldPID = u16PID;
            }
        }
    }

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetOutTSEMMPIDIndexList(u16OutTSIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetOutTSEMMPIDIndexList Error:enErrCode[%X],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16Index = stIndexList.aru16IndexList[i];
        u16PID = pstParamTS->pOutputPID[u16Index].u16PID;
        if (u16PID < MAX_PID_NUMBER)
        {
            arstPIDUseInfo[u16PID].u8UseFlag = 1;
            arstPIDUseInfo[u16PID].u8SrcSlot = Status_GetSlotID();
            arstPIDUseInfo[u16PID].u16SrcChannel =  pstParamTS->pOutputProgram[u16Index].u16SourceChannelID;
            arstPIDUseInfo[u16PID].u16OldPID = u16PID;
        }
    }

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetOutTSOtherPIDIndexList(u16OutTSIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetOutTSOtherPIDIndexList Error:enErrCode[%X],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16Index = stIndexList.aru16IndexList[i];
        u16PID = pstParamTS->pOutputPID[u16Index].u16PID;
        if (u16PID < MAX_PID_NUMBER)
        {
            arstPIDUseInfo[u16PID].u8UseFlag = 1;
            arstPIDUseInfo[u16PID].u8SrcSlot = Status_GetSlotID();
            arstPIDUseInfo[u16PID].u16SrcChannel =  pstParamTS->pOutputProgram[u16Index].u16SourceChannelID;
            arstPIDUseInfo[u16PID].u16OldPID = u16PID;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ConstructNullOutputTS
  Description:  ππ‘Ï“ª∏ˆø’µƒ ‰≥ˆ¡˜
  Input:        u16Channel: ‰≥ˆ¡˜µƒChannel∫≈
                pstParamTS
  Output:       
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ConstructNullOutputTS(U16 u16Channel, TSInfo *pstParamTS, U16 *pu16OutTSIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexList;

    if ((!pstParamTS) || (!pu16OutTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],pu16OutTSIndex[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, pu16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetUnusedOutputTSIndexList(&stIndexList, pstParamTS, 1);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetUnusedOutputTSIndexList Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    *pu16OutTSIndex = stIndexList.aru16IndexList[0];

    TSP_InitOutputTS(*pu16OutTSIndex, u16Channel, pstParamTS);
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_CheckServiceIDConflict
  Description:  ServiceID≥ÂÕªºÏ≤ÈΩ⁄£¨‘⁄∞— ‰»ÎΩ⁄ƒø¿≠µΩ ‰≥ˆ¡˜«∞Ω¯––ºÏ≤È£¨»Áπ˚”–≥ÂÕª¡ÀæÕ∑÷≈‰–¬µƒServiceID
  Input:        u16InProgIndex:“™ºÏ≤ÈµƒServiceIDµƒΩ⁄ƒøÀ˜“˝
                u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS
  Output:       
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_CheckServiceIDConflict(U16 u16InProgIndex, U16 u16OutTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16ServiceID = 0;
    U32 aru32ServiceIdList[2048] = {0};

    if ((NULL == pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex)
        || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]:Inpput error,pstParamTS[%p],u16InProgIndex[%u],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, u16OutTSIndex); 
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetServiceIDUseflagOfOutputTS(u16OutTSIndex, pstParamTS, aru32ServiceIdList);
    if (WV_SUCCESS != enErrCode)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]:TSP_GetServiceIDUseflagOfOutputTS error,enErrCode[%X],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
        return enErrCode;
    }

    u16ServiceID = pstParamTS->pInputProgram[u16InProgIndex].u16ServiceID;
    if (0 != (aru32ServiceIdList[u16ServiceID / 32] & (1 << (u16ServiceID % 32))))
    {
        for (i = 1; i <= MAX_SERVICEID; i++)
        {
            if (0 == (aru32ServiceIdList[i / 32] & (1 << (i % 32))))
            {
                u16ServiceID = i;
                aru32ServiceIdList[u16ServiceID / 32] |= (U32)(1 << (u16ServiceID % 32));
                break;
            }
        }

        if (i > MAX_SERVICEID)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]:ServiceID conflict,but no more free ID\r\n",
                __FUNCTION__, __LINE__);
            return WV_ERR_TSP_RESOURCE_NOT_ENOUGH;
        }
    }

    pstParamTS->pInputProgram[u16InProgIndex].u16NewServiceID = u16ServiceID;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_CheckPIDConflict
  Description:  PID≥ÂÕªºÏ≤ÈΩ⁄£¨»Áπ˚”–≥ÂÕª¡ÀæÕ∑÷≈‰–¬µƒPID
  Input:        u8PIDType:“™ºÏ≤ÈµƒPIDµƒ¿‡–Õ
                u16Index: ‰»ÎÀ˜“˝
                pstParamTS
                arstOutputTSPIDUseInfo:
  Output:       
  Return:       SUCESS ≥…π¶
                Others  ß∞‹‘≠“Ú
  Others:      
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_CheckPIDConflict(U8 u8PIDType, U16 u16Index, TSInfo *pstParamTS, PIDUseInfo arstOutputTSPIDUseInfo[MAX_PID_NUMBER + 1])
{
    U16 i = 0;
    U16 u16PID = 0;
    U16 *pu16NewPID = NULL;
    U16 *pu16SrcChannel = NULL;
    
    if ((!pstParamTS) || (!arstOutputTSPIDUseInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],arstOutputTSPIDUseInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, arstOutputTSPIDUseInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    switch (u8PIDType)
    {
        case PID_TYPE_PMTPID:
            if (u16Index >= pstParamTS->u32InputProgramNumber)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]Input Error,u16Index[%u]\r\n",
                    __FUNCTION__, __LINE__, u16Index);
                return WV_ERR_TSP_INPUT_PARAM;
            }
            
            pu16SrcChannel = &pstParamTS->pInputProgram[u16Index].u16ChannelID;
            pu16NewPID = &pstParamTS->pInputProgram[u16Index].u16NewPMTPID;
            u16PID = pstParamTS->pInputProgram[u16Index].u16PMTPID;

            break;

        case PID_TYPE_PCRPID:
            if (u16Index >= pstParamTS->u32InputProgramNumber)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]Input Error,u16Index[%u]\r\n",
                    __FUNCTION__, __LINE__, u16Index);
                return WV_ERR_TSP_INPUT_PARAM;
            }

            pu16SrcChannel = &pstParamTS->pInputProgram[u16Index].u16ChannelID;
            pu16NewPID = &pstParamTS->pInputProgram[u16Index].u16NewPCRPID;
            u16PID = pstParamTS->pInputProgram[u16Index].u16PCRPID;

            break;

        default:
            if (u16Index >= pstParamTS->u32InputPIDNumber)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]Input Error,u8PIDType[%u],u16Index[%u]\r\n",
                    __FUNCTION__, __LINE__, u8PIDType, u16Index);
                return WV_ERR_TSP_INPUT_PARAM;
            }

            pu16SrcChannel = &pstParamTS->pInputPID[u16Index].u16ChannelID;
            pu16NewPID = &pstParamTS->pInputPID[u16Index].u16NewPID;
            u16PID = pstParamTS->pInputPID[u16Index].u16PID;

            break;
    }

    if (1 != arstOutputTSPIDUseInfo[u16PID].u8UseFlag)
    {
        *pu16NewPID = u16PID;
        arstOutputTSPIDUseInfo[u16PID].u8UseFlag = 1;
        arstOutputTSPIDUseInfo[u16PID].u8SrcSlot = Status_GetSlotID();
        arstOutputTSPIDUseInfo[u16PID].u16SrcChannel = *pu16SrcChannel;
        arstOutputTSPIDUseInfo[u16PID].u16OldPID = u16PID;
    }
    else
    {
        for (i = 0; i < MAX_PID_NUMBER; i++)
        {
            if (0 == arstOutputTSPIDUseInfo[i].u8UseFlag)
            {
                continue;
            }

            if ((arstOutputTSPIDUseInfo[i].u16OldPID == u16PID)
                && (arstOutputTSPIDUseInfo[i].u8SrcSlot == Status_GetSlotID())
                && (arstOutputTSPIDUseInfo[i].u16SrcChannel == (*pu16SrcChannel)))
            {
                break;
            }
        }

        if (i < MAX_PID_NUMBER)
        {
            *pu16NewPID = i;
        }
        else
        {
            for (i = MIN_CONFLICT_PID_ALLOCATION; i < MAX_PID_NUMBER; i++)
            {
                if (0 == arstOutputTSPIDUseInfo[i].u8UseFlag)
                {
                    *pu16NewPID = i;
                    arstOutputTSPIDUseInfo[i].u8UseFlag = 1;
                    arstOutputTSPIDUseInfo[i].u8SrcSlot = Status_GetSlotID();
                    arstOutputTSPIDUseInfo[i].u16SrcChannel = *pu16SrcChannel;
                    arstOutputTSPIDUseInfo[i].u16OldPID = u16PID;
                    break;
                }
            }

            if (i >= MAX_PID_NUMBER)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]No free PID\r\n", __FUNCTION__, __LINE__);
                return WV_ERR_TSP_TS_ERROR;
            }
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_CheckPMTPIDConflict
  Description:  ºÏ≤È“ª∏ˆ ‰»ÎΩ⁄ƒøµƒPMTPID «∑Ò”Î ‰≥ˆ¡˜÷–µƒPID≥ÂÕª£¨»Áπ˚”–PID≥ÂÕª‘Ú∑÷≈‰–¬µƒPID
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                arstOutputTSPIDUseInfo: ‰≥ˆ¡˜÷–À˘”–“—æ≠ π”√µƒPID–≈œ¢
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_CheckPMTPIDConflict(U16 u16InProgIndex, TSInfo *pstParamTS, PIDUseInfo arstOutputTSPIDUseInfo[MAX_PID_NUMBER + 1])
{
    wvErrCode enErrCode = WV_SUCCESS;

    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex)
        || (!arstOutputTSPIDUseInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InProgIndex[%u],arstOutputTSPIDUseInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, arstOutputTSPIDUseInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    enErrCode = TSP_CheckPIDConflict(PID_TYPE_PMTPID, u16InProgIndex, pstParamTS, arstOutputTSPIDUseInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_CheckPIDConflict Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_CheckPCRPIDConflict
  Description:  ºÏ≤È“ª∏ˆ ‰»ÎΩ⁄ƒøµƒPCRPID «∑Ò”Î ‰≥ˆ¡˜÷–µƒPID≥ÂÕª£¨»Áπ˚”–PID≥ÂÕª‘Ú∑÷≈‰–¬µƒPID
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                arstOutputTSPIDUseInfo: ‰≥ˆ¡˜÷–À˘”–“—æ≠ π”√µƒPID–≈œ¢
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_CheckPCRPIDConflict(U16 u16InProgIndex, TSInfo *pstParamTS, PIDUseInfo arstOutputTSPIDUseInfo[MAX_PID_NUMBER + 1])
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 u16PCRIndex = INDEX_INVALID_VALUE;

    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex)
        || (!arstOutputTSPIDUseInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InProgIndex[%u],arstOutputTSPIDUseInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, arstOutputTSPIDUseInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    enErrCode = TSP_CheckPIDConflict(PID_TYPE_PCRPID, u16InProgIndex, pstParamTS, arstOutputTSPIDUseInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_CheckPIDConflict Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    u16PCRIndex = pstParamTS->pInputProgram[u16InProgIndex].u16PCRIndex;
    if (u16PCRIndex < pstParamTS->u32InputPIDNumber)
    {
        pstParamTS->pInputPID[u16PCRIndex].u16NewPID = pstParamTS->pInputProgram[u16InProgIndex].u16NewPCRPID;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_CheckESPIDConflict
  Description:  ºÏ≤È“ª∏ˆESPID «∑Ò”Î ‰≥ˆ¡˜÷–µƒPID≥ÂÕª£¨»Áπ˚”–PID≥ÂÕª‘Ú∑÷≈‰–¬µƒPID
  Input:        u16InESIndex: ‰»ÎESPIDÀ˜“˝
                arstOutputTSPIDUseInfo: ‰≥ˆ¡˜÷–À˘”–“—æ≠ π”√µƒPID–≈œ¢
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_CheckESPIDConflict(U16 u16InESIndex, TSInfo *pstParamTS, PIDUseInfo arstOutputTSPIDUseInfo[MAX_PID_NUMBER + 1])
{
    wvErrCode enErrCode = WV_SUCCESS;

    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InESIndex)
        || (!arstOutputTSPIDUseInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InESIndex[%u],arstOutputTSPIDUseInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InESIndex, arstOutputTSPIDUseInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    enErrCode = TSP_CheckPIDConflict(PID_TYPE_ESPID, u16InESIndex, pstParamTS, arstOutputTSPIDUseInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_CheckPIDConflict Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_CheckESECMPIDConflict
  Description:  ºÏ≤È“ª∏ˆESECMPID «∑Ò”Î ‰≥ˆ¡˜÷–µƒPID≥ÂÕª£¨»Áπ˚”–PID≥ÂÕª‘Ú∑÷≈‰–¬µƒPID
  Input:        u16InECMIndex: ‰»ÎESECMPIDÀ˜“˝
                arstOutputTSPIDUseInfo: ‰≥ˆ¡˜÷–À˘”–“—æ≠ π”√µƒPID–≈œ¢
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_CheckESECMPIDConflict(U16 u16InECMIndex, TSInfo *pstParamTS, PIDUseInfo arstOutputTSPIDUseInfo[MAX_PID_NUMBER + 1])
{
    wvErrCode enErrCode = WV_SUCCESS;

    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InECMIndex)
        || (!arstOutputTSPIDUseInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InECMIndex[%u],arstOutputTSPIDUseInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InECMIndex, arstOutputTSPIDUseInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    enErrCode = TSP_CheckPIDConflict(PID_TYPE_ES_ECMPID, u16InECMIndex, pstParamTS, arstOutputTSPIDUseInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_CheckPIDConflict Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_CheckPSECMPIDConflict
  Description:  ºÏ≤È“ª∏ˆPSECMPID «∑Ò”Î ‰≥ˆ¡˜÷–µƒPID≥ÂÕª£¨»Áπ˚”–PID≥ÂÕª‘Ú∑÷≈‰–¬µƒPID
  Input:        u16InECMIndex: ‰»ÎPSECMPIDÀ˜“˝
                arstOutputTSPIDUseInfo: ‰≥ˆ¡˜÷–À˘”–“—æ≠ π”√µƒPID–≈œ¢
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_CheckPSECMPIDConflict(U16 u16InECMIndex, TSInfo *pstParamTS, PIDUseInfo arstOutputTSPIDUseInfo[MAX_PID_NUMBER + 1])
{
    wvErrCode enErrCode = WV_SUCCESS;

    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InECMIndex)
        || (!arstOutputTSPIDUseInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InECMIndex[%u],arstOutputTSPIDUseInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InECMIndex, arstOutputTSPIDUseInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    enErrCode = TSP_CheckPIDConflict(PID_TYPE_PS_ECMPID, u16InECMIndex, pstParamTS, arstOutputTSPIDUseInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_CheckPIDConflict Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_CheckEMMPIDConflict
  Description:  ºÏ≤È“ª∏ˆEMMPID «∑Ò”Î ‰≥ˆ¡˜÷–µƒPID≥ÂÕª£¨»Áπ˚”–PID≥ÂÕª‘Ú∑÷≈‰–¬µƒPID
  Input:        u16InEMMIndex: ‰»ÎEMMPIDÀ˜“˝
                arstOutputTSPIDUseInfo: ‰≥ˆ¡˜÷–À˘”–“—æ≠ π”√µƒPID–≈œ¢
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_CheckEMMPIDConflict(U16 u16InEMMIndex, TSInfo *pstParamTS, PIDUseInfo arstOutputTSPIDUseInfo[MAX_PID_NUMBER + 1])
{
    wvErrCode enErrCode = WV_SUCCESS;

    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InEMMIndex)
        || (!arstOutputTSPIDUseInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InEMMIndex[%u],arstOutputTSPIDUseInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InEMMIndex, arstOutputTSPIDUseInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    enErrCode = TSP_CheckPIDConflict(PID_TYPE_EMMPID, u16InEMMIndex, pstParamTS, arstOutputTSPIDUseInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_CheckPIDConflict Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_CheckOtherPIDConflict
  Description:  ºÏ≤È“ª∏ˆOtherPID «∑Ò”Î ‰≥ˆ¡˜÷–µƒPID≥ÂÕª£¨»Áπ˚”–PID≥ÂÕª‘Ú∑÷≈‰–¬µƒPID
  Input:        u16InOhterPIDIndex: ‰»ÎOtherPIDÀ˜“˝
                arstOutputTSPIDUseInfo: ‰≥ˆ¡˜÷–À˘”–“—æ≠ π”√µƒPID–≈œ¢
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_CheckOtherPIDConflict(U16 u16InOhterPIDIndex, TSInfo *pstParamTS, PIDUseInfo arstOutputTSPIDUseInfo[MAX_PID_NUMBER + 1])
{
    wvErrCode enErrCode = WV_SUCCESS;

    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InOhterPIDIndex)
        || (!arstOutputTSPIDUseInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InOhterPIDIndex[%u],arstOutputTSPIDUseInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InOhterPIDIndex, arstOutputTSPIDUseInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }
    
    enErrCode = TSP_CheckPIDConflict(PID_TYPE_OTHERPID, u16InOhterPIDIndex, pstParamTS, arstOutputTSPIDUseInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_CheckPIDConflict Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_CheckPIDConflictOfProg
  Description:  ºÏ≤È“ª∏ˆ ‰»ÎΩ⁄ƒøµƒÀ˘”–PID «∑Ò”Î ‰≥ˆ¡˜÷–µƒPID≥ÂÕª£¨»Áπ˚”–PID≥ÂÕª‘Ú∑÷≈‰–¬µƒPID
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_CheckPIDConflictOfProg(U16 u16InProgIndex, U16 u16OutTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 j = 0;

    PIDUseInfo arstOutputTSPIDUseInfo[MAX_PID_NUMBER + 1];


    U16 u16ESIndex = INDEX_INVALID_VALUE;
    U16 u16ECMIndex = INDEX_INVALID_VALUE;

    IndexInfoList stIndexList;
    IndexInfoList stECMIndexList;
    
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(arstOutputTSPIDUseInfo, 0, sizeof(arstOutputTSPIDUseInfo));
    enErrCode = TSP_GetPIDUseFlagOfOutputTS(u16OutTSIndex, pstParamTS, arstOutputTSPIDUseInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetPIDUseFlagOfOutputTS Error:enErrCode[%X],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
        return enErrCode;
    }

    enErrCode = TSP_CheckPMTPIDConflict(u16InProgIndex, pstParamTS, arstOutputTSPIDUseInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_CheckPMTPIDConflict Error:enErrCode[%X],u16InProgIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16InProgIndex);
        return enErrCode;
    }
    
    enErrCode = TSP_CheckPCRPIDConflict(u16InProgIndex, pstParamTS, arstOutputTSPIDUseInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_CheckPCRPIDConflict Error:enErrCode[%X],u16InProgIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16InProgIndex);
        return enErrCode;
    }

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetInProgESPIDIndexList(u16InProgIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetInProgESPIDIndexList Error:enErrCode[%X],u16InProgIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16InProgIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16ESIndex = stIndexList.aru16IndexList[i];

        stECMIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInPIDESECMPIDIndexList(u16ESIndex, pstParamTS, &stECMIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_GetInPIDESECMPIDIndexList Error:enErrCode[%X],u16InProgIndex[%u],ES[%u][%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16InProgIndex, i, u16ESIndex);
            return enErrCode;
        }

        for (j = 0; j < stECMIndexList.u16IndexListNum; j++)
        {
            u16ECMIndex = stECMIndexList.aru16IndexList[j];
            
            enErrCode = TSP_CheckESECMPIDConflict(u16ECMIndex, pstParamTS, arstOutputTSPIDUseInfo);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                   "[%s:%d]TSP_CheckESECMPIDConflict Error:enErrCode[%X],u16InProgIndex[%u],ES[%u][%u],ECM[%u][%u]\r\n",
                   __FUNCTION__, __LINE__, enErrCode, u16InProgIndex, i, u16ESIndex, j, u16ECMIndex);
                return enErrCode;
            }
        }

        enErrCode = TSP_CheckESPIDConflict(u16ESIndex, pstParamTS, arstOutputTSPIDUseInfo);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_CheckESPIDConflict Error:enErrCode[%X],u16InProgIndex[%u],ES[%u][%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16InProgIndex, i, u16ESIndex);
            return enErrCode;
        }
    }

    stECMIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetInProgPSECMPIDIndexList(u16InProgIndex, pstParamTS, &stECMIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetInProgPSECMPIDIndexList Error:enErrCode[%X],u16InProgIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16InProgIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16ECMIndex = stECMIndexList.aru16IndexList[j];
        
        enErrCode = TSP_CheckPSECMPIDConflict(u16ECMIndex, pstParamTS, arstOutputTSPIDUseInfo);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_CheckPSECMPIDConflict Error:enErrCode[%X],u16InProgIndex[%u],ECM[%u][%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16InProgIndex, i, u16ECMIndex);
            return enErrCode;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetPrevOutputPIDIndex
  Description:  ªÒ»° ‰≥ˆPID‘⁄À˘‘⁄ ‰≥ˆ¡˜ªÚΩ⁄ƒøªÚPID÷–µƒ«∞“ª∏ˆPIDµƒÀ˜“˝
  Input:        u16OutPIDIndex: ‰≥ˆPIDÀ˜“˝
                pstParamTS
  Output:       pu16PrevPIDIndex:ªÒ»°µΩµƒ«∞“ª∏ˆPIDÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetPrevOutputPIDIndex(U16 u16OutPIDIndex, TSInfo *pstParamTS, U16 *pu16PrevPIDIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    IndexInfoList stIndexList;
    U8 u8PIDType = PID_TYPE_UNKNOWN;
    U16 u16SrcTSIndex = INDEX_INVALID_VALUE;
    U16 u16SrcProgIndex = INDEX_INVALID_VALUE;
    U16 u16SrcPIDIndex = INDEX_INVALID_VALUE;
    
    if ((!pstParamTS) || (pstParamTS->u32OutputPIDNumber <= u16OutPIDIndex) || (!pu16PrevPIDIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16OutPIDIndex[%u],pu16PrevPIDIndex[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutPIDIndex, pu16PrevPIDIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    u8PIDType = pstParamTS->pOutputPID[u16OutPIDIndex].u8PIDType;
    u16SrcTSIndex = pstParamTS->pOutputPID[u16OutPIDIndex].u32TSIndex;
    u16SrcProgIndex = pstParamTS->pOutputPID[u16OutPIDIndex].u32ProgramIndex;
    u16SrcPIDIndex = pstParamTS->pOutputPID[u16OutPIDIndex].u32PIDIndex;
    
    stIndexList.u16IndexListNum = 0;
    
    switch (u8PIDType)
    {
        case PID_TYPE_ESPID:
        {
            enErrCode = TSP_GetOutProgESPIDIndexList(u16SrcProgIndex, pstParamTS, &stIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetOutProgESPIDIndexList error:u16SrcProgIndex[%u]\r\n",
                    __FUNCTION__, __LINE__, u16SrcProgIndex);
                return enErrCode;
            }
            break;
        }

        case PID_TYPE_PCRPID:
        {
            *pu16PrevPIDIndex = INDEX_INVALID_VALUE;
            return WV_SUCCESS;
        }

        case PID_TYPE_PS_ECMPID:
        {
            enErrCode = TSP_GetOutProgPSECMPIDIndexList(u16SrcProgIndex, pstParamTS, &stIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetOutProgPSECMPIDIndexList error:u16SrcProgIndex[%u]\r\n",
                    __FUNCTION__, __LINE__, u16SrcProgIndex);
                return enErrCode;
            }
            break;
        }

        case PID_TYPE_ES_ECMPID:
        {
            enErrCode = TSP_GetOutPIDESECMPIDIndexList(u16SrcPIDIndex, pstParamTS, &stIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetOutPIDESECMPIDIndexList error:u16SrcPIDIndex[%u]\r\n",
                    __FUNCTION__, __LINE__, u16SrcPIDIndex);
                return enErrCode;
            }
            break;
        }

        case PID_TYPE_EMMPID:
        {
            enErrCode = TSP_GetOutTSEMMPIDIndexList(u16SrcTSIndex, pstParamTS, &stIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetOutTSEMMPIDIndexList error:u16SrcTSIndex[%u]\r\n",
                    __FUNCTION__, __LINE__, u16SrcTSIndex);
                return enErrCode;
            }
            break;
        }

        case PID_TYPE_OTHERPID:
        {
            enErrCode = TSP_GetOutTSOtherPIDIndexList(u16SrcTSIndex, pstParamTS, &stIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetOutTSEMMPIDIndexList error:u16SrcTSIndex[%u]\r\n",
                    __FUNCTION__, __LINE__, u16SrcTSIndex);
                return enErrCode;
            }
            break;
        }

        default:
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Error,unknown pid type:u16OutPIDIndex[%u],u8PIDType[%u]\r\n",
                __FUNCTION__, __LINE__, u16OutPIDIndex, u8PIDType);
            return WV_ERR_TSP_TS_ERROR;
        }
    }

    if (0 == stIndexList.u16IndexListNum)
    {
        return WV_ERR_TSP_TS_ERROR;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        if (u16OutPIDIndex == stIndexList.aru16IndexList[i])
        {
            break;
        }
    }
    
    if (0 == i)
    {
        *pu16PrevPIDIndex = INDEX_INVALID_VALUE;
    }
    else if (i < stIndexList.u16IndexListNum)
    {
        *pu16PrevPIDIndex = stIndexList.aru16IndexList[i - 1];
    }
    else
    {
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS; 
}

/*****************************************************************************
  Function:     TSP_GetPrevOutputProgIndex
  Description:  ªÒ»° ‰≥ˆΩ⁄ƒø‘⁄À˘‘⁄ ‰≥ˆ¡˜÷–µƒ«∞“ª∏ˆΩ⁄ƒøµƒÀ˜“˝
  Input:        u16OutProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                pstParamTS
  Output:       pu16PrevProgIndex:ªÒ»°µΩµƒ«∞“ª∏ˆΩ⁄ƒøÀ˜“˝
  Return:       SUCESS ªÒ»°≥…π¶
                Others ªÒ»° ß∞‹‘≠“Ú
  Others:       »Áπ˚Ω⁄ƒø «∆‰À˘‘⁄¡˜÷–µƒµ⁄“ª∏ˆΩ⁄ƒø£¨‘Ú∑µªÿWV_ERR_TSP_NOT_EXIST
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetPrevOutputProgIndex(U16 u16OutProgIndex, TSInfo *pstParamTS, U16 *pu16PrevProgIndex)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16OutTSIndex = INDEX_INVALID_VALUE;
    IndexInfoList stIndexList;
    
    if ((!pstParamTS) || (pstParamTS->u32OutputProgramNumber <= u16OutProgIndex) || (!pu16PrevProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16OutProgIndex[%u],pu16PrevProgIndex[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutProgIndex, pu16PrevProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    u16OutTSIndex = pstParamTS->pOutputProgram[u16OutProgIndex].u16TSIndex;

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetOutTSProgIndexList(u16OutTSIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutTSProgIndexList Error:u16OutProgIndex[%u],u16OutTSIndex[%u],enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, u16OutProgIndex, u16OutTSIndex, enErrCode);
        return enErrCode;
    }

    if (0 == stIndexList.u16IndexListNum)
    {
        return WV_ERR_TSP_TS_ERROR;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        if (u16OutProgIndex == stIndexList.aru16IndexList[i])
        {
            break;
        }
    }

    if (0 == i)
    {
        *pu16PrevProgIndex = INDEX_INVALID_VALUE;
    }
    else if (i < stIndexList.u16IndexListNum)
    {
        *pu16PrevProgIndex = stIndexList.aru16IndexList[i - 1];
    }
    else
    {
        return WV_ERR_TSP_TS_ERROR;
    }

    return WV_SUCCESS; 
}

/*****************************************************************************
  Function:     TSP_DelOutputPID
  Description:  …æ≥˝“ª∏ˆ ‰≥ˆPID
  Input:        u16OutPIDIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_DelOutputPID(U16 u16OutPIDIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    IndexInfoList stIndexList;
    U16 u16OutTSIndex = INDEX_INVALID_VALUE;
    U16 u16OutProgIndex = INDEX_INVALID_VALUE;
    U16 u16SrcPIDIndex = INDEX_INVALID_VALUE;
    U16 u16Index = INDEX_INVALID_VALUE;
    U16 u16PrevIndex = INDEX_INVALID_VALUE;
    U8 u8PIDType = PID_TYPE_UNKNOWN;
    
    if ((!pstParamTS) || (pstParamTS->u32OutputPIDNumber <= u16OutPIDIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16OutProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutPIDIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    u8PIDType = pstParamTS->pOutputPID[u16OutPIDIndex].u8PIDType;

    if (PID_TYPE_ESPID == u8PIDType)
    {
        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetOutPIDESECMPIDIndexList(u16OutPIDIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_GetOutPIDESECMPIDIndexList Error:enErrCode[%X],u16OutPIDIndex[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16OutPIDIndex);
            return enErrCode;
        }
    
        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            u16Index = stIndexList.aru16IndexList[i];

            enErrCode = TSP_DelOutputPID(u16Index, pstParamTS);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                   "[%s:%d]TSP_DelOutputPID Error:enErrCode[%X],ESECM[%u][%u]\r\n",
                   __FUNCTION__, __LINE__, enErrCode, i, u16Index);
                return enErrCode;
            }
        }
    }
    
    enErrCode = TSP_GetPrevOutputPIDIndex(u16OutPIDIndex, pstParamTS, &u16PrevIndex);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetPrevOutputPID Error:enErrCode[%X],u16OutPIDIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16OutPIDIndex);
        return enErrCode;
    }

    switch (u8PIDType)
    {
        case PID_TYPE_ESPID:
            u16OutProgIndex = pstParamTS->pOutputPID[u16OutPIDIndex].u32ProgramIndex;
            
            if (INDEX_INVALID_VALUE == u16PrevIndex)
            {
                pstParamTS->pOutputProgram[u16OutProgIndex].u16PIDIndex = 
                    pstParamTS->pOutputPID[u16OutPIDIndex].u32NextPIDIndex;
            }
            else
            {
                pstParamTS->pOutputPID[u16PrevIndex].u32NextPIDIndex = 
                    pstParamTS->pOutputPID[u16OutPIDIndex].u32NextPIDIndex;
            }
            
            pstParamTS->pOutputProgram[u16OutProgIndex].u16ESNumber -= 1;
            break;

        case PID_TYPE_PCRPID:
            u16OutProgIndex = pstParamTS->pOutputPID[u16OutPIDIndex].u32ProgramIndex;
            pstParamTS->pOutputProgram[u16OutProgIndex].u16PCRIndex = INDEX_INVALID_VALUE;
            pstParamTS->pOutputProgram[u16OutProgIndex].u16PCRPID = MAX_PID_NUMBER;
            break;

        case PID_TYPE_PS_ECMPID:
            u16OutProgIndex = pstParamTS->pOutputPID[u16OutPIDIndex].u32ProgramIndex;
            
            if (INDEX_INVALID_VALUE == u16PrevIndex)
            {
                pstParamTS->pOutputProgram[u16OutProgIndex].u16PSECMPIDIndex = 
                    pstParamTS->pOutputPID[u16OutPIDIndex].u32NextPIDIndex;
            }
            else
            {
                pstParamTS->pOutputPID[u16PrevIndex].u32NextPIDIndex = 
                    pstParamTS->pOutputPID[u16OutPIDIndex].u32NextPIDIndex;
            }
            
            pstParamTS->pOutputProgram[u16OutProgIndex].u16PSECMPIDNumber -= 1;
            break;

        case PID_TYPE_ES_ECMPID:
            u16SrcPIDIndex = pstParamTS->pOutputPID[u16OutPIDIndex].u32PIDIndex;

            if (INDEX_INVALID_VALUE == u16PrevIndex)
            {
                pstParamTS->pOutputPID[u16SrcPIDIndex].u16ESECMPIDIndex = 
                    pstParamTS->pOutputPID[u16OutPIDIndex].u32NextPIDIndex;
            }
            else
            {
                pstParamTS->pOutputPID[u16PrevIndex].u32NextPIDIndex = 
                    pstParamTS->pOutputPID[u16OutPIDIndex].u32NextPIDIndex;
            }
            
            pstParamTS->pOutputPID[u16SrcPIDIndex].u16ESECMPIDNumber -= 1;
            break;

        case PID_TYPE_EMMPID:
            u16OutTSIndex = pstParamTS->pOutputPID[u16OutPIDIndex].u32TSIndex;

            if (INDEX_INVALID_VALUE == u16PrevIndex)
            {
                pstParamTS->pOutputTS[u16OutTSIndex].u16EMMPIDIndex = 
                    pstParamTS->pOutputPID[u16OutPIDIndex].u32NextPIDIndex;
            }
            else
            {
                pstParamTS->pOutputPID[u16PrevIndex].u32NextPIDIndex = 
                    pstParamTS->pOutputPID[u16OutPIDIndex].u32NextPIDIndex;
            }

            pstParamTS->pOutputTS[u16OutTSIndex].u16EMMPIDNumber -= 1;
            break;

        case PID_TYPE_OTHERPID:
            u16OutTSIndex = pstParamTS->pOutputPID[u16OutPIDIndex].u32TSIndex;

            if (INDEX_INVALID_VALUE == u16PrevIndex)
            {
                pstParamTS->pOutputTS[u16OutTSIndex].u16OtherPIDIndex = 
                    pstParamTS->pOutputPID[u16OutPIDIndex].u32NextPIDIndex;
            }
            else
            {
                pstParamTS->pOutputPID[u16PrevIndex].u32NextPIDIndex = 
                    pstParamTS->pOutputPID[u16OutPIDIndex].u32NextPIDIndex;
            }

            pstParamTS->pOutputTS[u16OutTSIndex].u16OtherPIDNum -= 1;
            break;

        default:
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Unknown u8PIDType[%u]\r\n",
                __FUNCTION__, __LINE__, u8PIDType);
            return WV_ERR_TSP_INPUT_PARAM;
    }
    
    memset(&pstParamTS->pOutputPID[u16OutPIDIndex], 0, sizeof(Output_TSPID));

    return WV_SUCCESS;
}


/*****************************************************************************
  Function:     TSP_DelOutputProg
  Description:  …æ≥˝“ª∏ˆ ‰≥ˆΩ⁄ƒø
  Input:        u16OutProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_DelOutputProg(U16 u16OutProgIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    IndexInfoList stIndexList;
    U16 u16OutTSIndex = INDEX_INVALID_VALUE;
    U16 u16Index = INDEX_INVALID_VALUE;
    U16 u16PrevIndex = INDEX_INVALID_VALUE;
    
    if ((!pstParamTS) || (pstParamTS->u32OutputProgramNumber <= u16OutProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16OutProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    //del PCRPID
    u16Index = pstParamTS->pOutputProgram[u16OutProgIndex].u16PCRIndex;
    
    if (u16Index < pstParamTS->u32InputPIDNumber)
    {
        enErrCode = TSP_DelOutputPID(u16Index, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_DelOutputPID PCR Error:enErrCode[%X],u16Index[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16Index);
            return enErrCode;
        }
    }

    //del ESPID
    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetOutProgESPIDIndexList(u16OutProgIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetInProgESPIDIndexList Error:enErrCode[%X],u16OutProgIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16OutProgIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16Index = stIndexList.aru16IndexList[i];

        enErrCode = TSP_DelOutputPID(u16Index, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_DelOutputPID Error:enErrCode[%X],ES[%u][%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, i, u16Index);
            return enErrCode;
        }
    }

    //del PSECMPID
    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetOutProgPSECMPIDIndexList(u16OutProgIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetOutProgPSECMPIDIndexList Error:enErrCode[%X],u16OutProgIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16OutProgIndex);
        return enErrCode;
    }
    
    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16Index = stIndexList.aru16IndexList[i];

        enErrCode = TSP_DelOutputPID(u16Index, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_DelOutputPID Error:enErrCode[%X],PSECM[%u][%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, i, u16Index);
            return enErrCode;
        }
    }

    u16OutTSIndex = pstParamTS->pOutputProgram[u16OutProgIndex].u16TSIndex;

    enErrCode = TSP_GetPrevOutputProgIndex(u16OutProgIndex, pstParamTS, &u16PrevIndex);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetPrevOutputProgIndex Error:enErrCode[%X],u16OutProgIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16OutProgIndex);
        return enErrCode;
    }

    if (INDEX_INVALID_VALUE == u16PrevIndex)
    {
        pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramIndex = 
            pstParamTS->pOutputProgram[u16OutProgIndex].u16NextProgramIndex;
    }
    else
    {
        pstParamTS->pOutputProgram[u16PrevIndex].u16NextProgramIndex = 
            pstParamTS->pOutputProgram[u16OutProgIndex].u16NextProgramIndex;
    }

    pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramNum -= 1;

    memset(&pstParamTS->pOutputProgram[u16OutProgIndex], 0, sizeof(Output_TSProgram));

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_AddPID2OutputPID
  Description:  ÃÌº”“ª∏ˆ ‰»ÎPID(ESECMPID)µΩ ‰≥ˆPID(ESPID)÷–
  Input:        u16InPIDIndex: ‰»ÎPIDÀ˜“˝
                u16OutPIDIndex: ‰≥ˆPIDÀ˜“˝
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_AddPID2OutputPID(U16 u16InPIDIndex, U16 u16OutPIDIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexList;
    U16 u16OutESECMIndex = INDEX_INVALID_VALUE;
    Input_TSPID *pInPID = NULL;
    Output_TSPID *pOutESECM = NULL;
    U16 u16LastIndex = INDEX_INVALID_VALUE;
    
    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InPIDIndex)
        || (pstParamTS->u32OutputPIDNumber <= u16OutPIDIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InPIDIndex[%u],u16OutPIDIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InPIDIndex, u16OutPIDIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetUnusedOutputPIDIndexList(&stIndexList, pstParamTS, 1);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetUnusedOutputPIDIndexList Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    u16OutESECMIndex = stIndexList.aru16IndexList[0];

    pInPID = &pstParamTS->pInputPID[u16InPIDIndex];
    pOutESECM = &pstParamTS->pOutputPID[u16OutESECMIndex];

    memset(pOutESECM, 0, sizeof(Output_TSPID));

    pOutESECM->u32StructVersion = TSP_GetStructVersion();
    pOutESECM->u8ValidFlag = TSP_FLAG_VALID;
    pOutESECM->u8SourceSlotID = Status_GetSlotID();
    pOutESECM->u16SourceChannelID = pInPID->u16ChannelID;
    pOutESECM->u8SlotID = Status_GetSlotID();
    pOutESECM->u16ChannelID = pstParamTS->pOutputPID[u16OutPIDIndex].u16ChannelID;
    pOutESECM->u32TSIndex = pstParamTS->pOutputPID[u16OutPIDIndex].u32TSIndex;
    pOutESECM->u32ProgramIndex = pstParamTS->pOutputPID[u16OutPIDIndex].u32ProgramIndex;
    pOutESECM->u32PIDIndex = u16OutPIDIndex;
    
    pOutESECM->u16ProgramID = pInPID->u16ProgramIndex;
    pOutESECM->u16PIDID = u16InPIDIndex;
    
    pOutESECM->u32NextPIDIndex = INDEX_INVALID_VALUE;
    pOutESECM->u16ESECMPIDIndex = INDEX_INVALID_VALUE;
    pOutESECM->u8PIDType = PID_TYPE_ES_ECMPID;
    
    pOutESECM->u8StreamType = pInPID->u8StreamType;
    pOutESECM->u16OldPID = pInPID->u16PID;
    pOutESECM->u16PID = pInPID->u16NewPID;
    
    pOutESECM->u16CASID = pInPID->u16CASID;
    pOutESECM->u16ESInfoLength = pInPID->u16ESInfoLength;
    memcpy(pOutESECM->aru8ESInfo, pInPID->aru8ESInfo, MAX_DESCRIPTOR_LENGTH);
    pOutESECM->u16ESInfoDescriptorIndex = INDEX_INVALID_VALUE;

    enErrCode = TSP_GetOutPIDLastESECMPIDIndex(u16OutPIDIndex, pstParamTS, &u16LastIndex);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetOutPIDLastESECMPIDIndex Error:enErrCode[%X],u16OutPIDIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16OutPIDIndex);
        return enErrCode;
    }

    if (INDEX_INVALID_VALUE == u16LastIndex)
    {
        pstParamTS->pOutputPID[u16OutPIDIndex].u16ESECMPIDIndex = u16OutESECMIndex;
        pstParamTS->pOutputPID[u16OutPIDIndex].u16ESECMPIDNumber = 1;
    }
    else
    {
        pstParamTS->pOutputPID[u16LastIndex].u32NextPIDIndex = u16OutESECMIndex;
        pstParamTS->pOutputPID[u16OutPIDIndex].u16ESECMPIDNumber += 1;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_AddPID2OutputProg
  Description:  ÃÌº”“ª∏ˆ ‰»ÎPIDµΩ ‰≥ˆΩ⁄ƒø÷–
  Input:        u8PIDType:PID¿‡–Õ
                u16InPIDIndex: ‰»ÎPIDÀ˜“˝
                u16OutProgIndex: ‰≥ˆΩ⁄ƒøÀ˜“˝
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_AddPID2OutputProg(U8 u8PIDType, U16 u16InPIDIndex, U16 u16OutProgIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    IndexInfoList stIndexList;
    U16 u16LastIndex = INDEX_INVALID_VALUE;
    
    Input_TSPID *pInPID = NULL;
    Output_TSPID *pOutPID = NULL;
    U16 u16OutPIDIndex = INDEX_INVALID_VALUE;
    U16 u16ESECMIndex = INDEX_INVALID_VALUE;

    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InPIDIndex)
        || (pstParamTS->u32OutputProgramNumber <= u16OutProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InPIDIndex[%u],u16OutProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InPIDIndex, u16OutProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetUnusedOutputPIDIndexList(&stIndexList, pstParamTS, 1);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetUnusedOutputProgIndexList Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    u16OutPIDIndex = stIndexList.aru16IndexList[0];

    pInPID = &pstParamTS->pInputPID[u16InPIDIndex];
    pOutPID = &pstParamTS->pOutputPID[u16OutPIDIndex];

    memset(pOutPID, 0, sizeof(Output_TSPID));

    pOutPID->u32StructVersion = TSP_GetStructVersion();
    pOutPID->u8ValidFlag = TSP_FLAG_VALID;
    
    pOutPID->u8SourceSlotID = Status_GetSlotID();
    pOutPID->u16SourceChannelID = pInPID->u16ChannelID;

    pOutPID->u8SlotID = Status_GetSlotID();
    pOutPID->u16ChannelID = pstParamTS->pOutputProgram[u16OutProgIndex].u16ChannelID;

    pOutPID->u32TSIndex = pstParamTS->pOutputProgram[u16OutProgIndex].u16TSIndex;
    pOutPID->u32ProgramIndex = u16OutProgIndex;
    pOutPID->u32PIDIndex = INDEX_INVALID_VALUE;
    
    pOutPID->u16ProgramID = pInPID->u16ProgramIndex;
    pOutPID->u16PIDID = u16InPIDIndex;
    
    pOutPID->u32NextPIDIndex = INDEX_INVALID_VALUE;
    pOutPID->u16ESECMPIDIndex = INDEX_INVALID_VALUE;
    
    pOutPID->u8PIDType = u8PIDType;
    
    pOutPID->u8StreamType = pInPID->u8StreamType;
    pOutPID->u16OldPID = pInPID->u16PID;
    pOutPID->u16PID = pInPID->u16NewPID;
    
    pOutPID->u16CASID = pInPID->u16CASID;
    pOutPID->u16ESInfoLength = pInPID->u16ESInfoLength;
    memcpy(pOutPID->aru8ESInfo, pInPID->aru8ESInfo, MAX_DESCRIPTOR_LENGTH);
    pOutPID->u16ESInfoDescriptorIndex = INDEX_INVALID_VALUE;

    switch (u8PIDType)
    {
        case PID_TYPE_PCRPID:
            pstParamTS->pOutputProgram[u16OutProgIndex].u16PCRIndex = u16OutPIDIndex;
            break;

        case PID_TYPE_PS_ECMPID:
            enErrCode = TSP_GetOutProgLastPSECMPIDIndex(u16OutProgIndex, pstParamTS, &u16LastIndex);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                   "[%s:%d]TSP_GetOutProgLastPSECMPIDIndex Error:enErrCode[%X],u16OutProgIndex[%u]\r\n",
                   __FUNCTION__, __LINE__, enErrCode, u16OutProgIndex);
                return enErrCode;
            }

            if (INDEX_INVALID_VALUE == u16LastIndex)
            {
                pstParamTS->pOutputProgram[u16OutProgIndex].u16PSECMPIDIndex = u16OutPIDIndex;
                pstParamTS->pOutputProgram[u16OutProgIndex].u16PSECMPIDNumber = 1;
            }
            else
            {
                pstParamTS->pOutputPID[u16LastIndex].u32NextPIDIndex = u16OutPIDIndex;
                pstParamTS->pOutputProgram[u16OutProgIndex].u16PSECMPIDNumber += 1;
            }
            
            break;

        case PID_TYPE_ESPID:
            enErrCode = TSP_GetOutProgLastESPIDIndex(u16OutProgIndex, pstParamTS, &u16LastIndex);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                   "[%s:%d]TSP_GetOutProgLastESPIDIndex Error:enErrCode[%X],u16OutProgIndex[%u]\r\n",
                   __FUNCTION__, __LINE__, enErrCode, u16OutProgIndex);
                return enErrCode;
            }

            if (INDEX_INVALID_VALUE == u16LastIndex)
            {
                pstParamTS->pOutputProgram[u16OutProgIndex].u16PIDIndex = u16OutPIDIndex;
                pstParamTS->pOutputProgram[u16OutProgIndex].u16ESNumber = 1;
            }
            else
            {
                pstParamTS->pOutputPID[u16LastIndex].u32NextPIDIndex = u16OutPIDIndex;
                pstParamTS->pOutputProgram[u16OutProgIndex].u16ESNumber += 1;
            }

            break;

        default:
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]Input Error,u8PIDType[%u]\r\n",
                    __FUNCTION__, __LINE__, u8PIDType);
            return WV_ERR_TSP_INPUT_PARAM;
    }

    if (PID_TYPE_ESPID != u8PIDType)
    {
        return WV_SUCCESS;
    }

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetInPIDESECMPIDIndexList(u16InPIDIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetInPIDESECMPIDIndexList Error:enErrCode[%X],u16InPIDIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16InPIDIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16ESECMIndex = stIndexList.aru16IndexList[i];

        enErrCode = TSP_AddPID2OutputPID(u16ESECMIndex, u16OutPIDIndex, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_AddPID2OutputPID Error:enErrCode[%X],ES[%u][%u],u16OutPIDIndex[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, i, u16ESECMIndex, u16OutPIDIndex);
            return enErrCode;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_AddPID2OutputTS
  Description:  ÃÌº”“ª∏ˆ ‰»ÎPIDµΩ ‰≥ˆ¡˜÷–
  Input:        u8PIDType:PID¿‡–Õ
                u16InPIDIndex: ‰»ÎPIDÀ˜“˝
                u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_AddPID2OutputTS(U8 u8PIDType, U16 u16InPIDIndex, U16 u16OutTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    IndexInfoList stIndexList;
    U16 u16LastIndex = INDEX_INVALID_VALUE;
    
    Input_TSPID *pInPID = NULL;
    Output_TSPID *pOutPID = NULL;
    U16 u16OutPIDIndex = INDEX_INVALID_VALUE;

    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InPIDIndex)
        || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InPIDIndex[%u],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InPIDIndex, u16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetUnusedOutputPIDIndexList(&stIndexList, pstParamTS, 1);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetUnusedOutputProgIndexList Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    u16OutPIDIndex = stIndexList.aru16IndexList[0];

    pInPID = &pstParamTS->pInputPID[u16InPIDIndex];
    pOutPID = &pstParamTS->pOutputPID[u16OutPIDIndex];

    memset(pOutPID, 0, sizeof(Output_TSPID));

    pOutPID->u32StructVersion = TSP_GetStructVersion();
    pOutPID->u8ValidFlag = TSP_FLAG_VALID;
    
    pOutPID->u8SourceSlotID = Status_GetSlotID();
    pOutPID->u16SourceChannelID = pInPID->u16ChannelID;

    pOutPID->u8SlotID = Status_GetSlotID();
    pOutPID->u16ChannelID = pstParamTS->pOutputTS[u16OutTSIndex].u16ChannelID;

    pOutPID->u32TSIndex = u16OutTSIndex;
    pOutPID->u32ProgramIndex = INDEX_INVALID_VALUE;
    pOutPID->u32PIDIndex = INDEX_INVALID_VALUE;
    
    pOutPID->u16ProgramID = INDEX_INVALID_VALUE;
    pOutPID->u16PIDID = u16InPIDIndex;
    
    pOutPID->u32NextPIDIndex = INDEX_INVALID_VALUE;
    pOutPID->u16ESECMPIDIndex = INDEX_INVALID_VALUE;
    
    pOutPID->u8PIDType = u8PIDType;
    
    pOutPID->u8StreamType = pInPID->u8StreamType;
    pOutPID->u16OldPID = pInPID->u16PID;
    pOutPID->u16PID = pInPID->u16NewPID;
    
    pOutPID->u16CASID = pInPID->u16CASID;
    pOutPID->u16ESInfoLength = pInPID->u16ESInfoLength;
    memcpy(pOutPID->aru8ESInfo, pInPID->aru8ESInfo, MAX_DESCRIPTOR_LENGTH);
    pOutPID->u16ESInfoDescriptorIndex = INDEX_INVALID_VALUE;

    switch (u8PIDType)
    {
        case PID_TYPE_EMMPID:
            enErrCode = TSP_GetOutTSLastEMMPIDIndex(u16OutTSIndex, pstParamTS, &u16LastIndex);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                   "[%s:%d]TSP_GetOutTSLastEMMPIDIndex Error:enErrCode[%X],u16OutTSIndex[%u]\r\n",
                   __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
                return enErrCode;
            }

            if (INDEX_INVALID_VALUE == u16LastIndex)
            {
                pstParamTS->pOutputTS[u16OutTSIndex].u16EMMPIDIndex = u16OutPIDIndex;
                pstParamTS->pOutputTS[u16OutTSIndex].u16EMMPIDNumber = 1;
            }
            else
            {
                pstParamTS->pOutputPID[u16LastIndex].u32NextPIDIndex = u16OutPIDIndex;
                pstParamTS->pOutputTS[u16OutTSIndex].u16EMMPIDNumber += 1;
            }
            
            break;

        case PID_TYPE_OTHERPID:
            enErrCode = TSP_GetOutTSLastOtherPIDIndex(u16OutTSIndex, pstParamTS, &u16LastIndex);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                   "[%s:%d]TSP_GetOutTSLastEMMPIDIndex Error:enErrCode[%X],u16OutTSIndex[%u]\r\n",
                   __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
                return enErrCode;
            }

            if (INDEX_INVALID_VALUE == u16LastIndex)
            {
                pstParamTS->pOutputTS[u16OutTSIndex].u16OtherPIDIndex = u16OutPIDIndex;
                pstParamTS->pOutputTS[u16OutTSIndex].u16OtherPIDNum = 1;
            }
            else
            {
                pstParamTS->pOutputPID[u16LastIndex].u32NextPIDIndex = u16OutPIDIndex;
                pstParamTS->pOutputTS[u16OutTSIndex].u16OtherPIDNum += 1;
            }

            break;

        default:
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]Input Error,u8PIDType[%u]\r\n",
                    __FUNCTION__, __LINE__, u8PIDType);
            return WV_ERR_TSP_INPUT_PARAM;
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_AddProg2OutputTS
  Description:  ÃÌº”“ª∏ˆ ‰»ÎΩ⁄ƒøµΩ ‰≥ˆ¡˜÷–
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_AddProg2OutputTS(U16 u16InProgIndex, U16 u16OutTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    IndexInfoList stIndexList;
    U16 u16OutProgIndex = INDEX_INVALID_VALUE;
    Input_TSProgram *pInProg = NULL;
    Output_TSProgram *pOutProg = NULL;

    U16 u16Index = INDEX_INVALID_VALUE;
    U16 u16LastIndex = INDEX_INVALID_VALUE;
    
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex)
        || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InProgIndex[%u],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, u16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetUnusedOutputProgIndexList(&stIndexList, pstParamTS, 1);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetUnusedOutputProgIndexList Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    u16OutProgIndex = stIndexList.aru16IndexList[0];

    pInProg = &pstParamTS->pInputProgram[u16InProgIndex];
    pOutProg = &pstParamTS->pOutputProgram[u16OutProgIndex];

    memset(pOutProg, 0, sizeof(Output_TSProgram));

    pOutProg->u32StructVersion = TSP_GetStructVersion();
    pOutProg->u8ValidFlag = TSP_FLAG_VALID;
    pOutProg->u8SourceSlotID = Status_GetSlotID();
    pOutProg->u16SourceChannelID = pInProg->u16ChannelID;
    pOutProg->u16ChannelID = pstParamTS->pOutputTS[u16OutTSIndex].u16ChannelID;
    pOutProg->u8SlotID = Status_GetSlotID();
    pOutProg->u16TSIndex = u16OutTSIndex;
    pOutProg->u16ProgramIndex = u16OutProgIndex;
    pOutProg->u16ProgramID = u16InProgIndex;
    pOutProg->u16NextProgramIndex = INDEX_INVALID_VALUE;
    pOutProg->u16PSECMPIDIndex = INDEX_INVALID_VALUE;
    
    pOutProg->u16OldServiceID = pInProg->u16ServiceID;
    pOutProg->u16OldPCRPID = pInProg->u16PCRPID;
    pOutProg->u16OldPMTPID = pInProg->u16PMTPID;

    pOutProg->u16ServiceID = pInProg->u16NewServiceID;
    pOutProg->u16PCRPID = pInProg->u16NewPCRPID;
    pOutProg->u16PMTPID = pInProg->u16NewPMTPID;

    pOutProg->u16PCRIndex = INDEX_INVALID_VALUE;
    pOutProg->u16PIDIndex = INDEX_INVALID_VALUE;

    pOutProg->u8ServiceNameLen = pInProg->u8ServiceNameLen;
    memcpy(pOutProg->aru8ServiceName, pInProg->aru8ServiceName, MAX_SDT_NAME_LENGTH);
    pOutProg->u8ServiceProviderLen = pInProg->u8ServiceProviderLen;
    memcpy(pOutProg->aru8ServiceProvider, pInProg->aru8ServiceProvider, MAX_SDT_NAME_LENGTH);

    pOutProg->u8ServiceType = pInProg->u8ServiceType;
    pOutProg->u8RunningStatus = pInProg->u8RunningStatus;
    pOutProg->u8FreeCAMode = pInProg->u8FreeCAMode;
    pOutProg->u8EITPresentFollowingFlag = pInProg->u8EITPresentFollowingFlag;

    pOutProg->u16ProgramInfoLength = pInProg->u16ProgramInfoLength;
    memcpy(pOutProg->aru8ProgramInfo, pInProg->aru8ProgramInfo, MAX_DESCRIPTOR_LENGTH);

    pOutProg->u16SDTInfoLength = pInProg->u16SDTInfoLength;
    memcpy(pOutProg->aru8SDTInfo, pInProg->aru8SDTInfo, MAX_DESCRIPTOR_LENGTH);

    //add PCRPID
    if (pInProg->u16PCRIndex < pstParamTS->u32InputPIDNumber)
    {
        enErrCode = TSP_AddPID2OutputProg(PID_TYPE_PCRPID, pInProg->u16PCRIndex, u16OutProgIndex, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_AddPID2OutputProg Error:enErrCode[%X],u16PCRIndex[%u],u16OutProgIndex[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, i, u16Index, pInProg->u16PCRIndex, u16OutProgIndex);
            return enErrCode;
        }
    }

    //add ESPID
    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetInProgESPIDIndexList(u16InProgIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetInProgESPIDIndexList Error:enErrCode[%X],u16InProgIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16InProgIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16Index = stIndexList.aru16IndexList[i];

        enErrCode = TSP_AddPID2OutputProg(PID_TYPE_ESPID, u16Index, u16OutProgIndex, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_AddPID2OutputProg Error:enErrCode[%X],ES[%u][%u],u16OutProgIndex[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, i, u16Index, u16OutProgIndex);
            return enErrCode;
        }
    }

    //add PSECMPID
    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetInProgPSECMPIDIndexList(u16InProgIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetInProgPSECMPIDIndexList Error:enErrCode[%X],u16InProgIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16InProgIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16Index = stIndexList.aru16IndexList[i];

        enErrCode = TSP_AddPID2OutputProg(PID_TYPE_PS_ECMPID, u16Index, u16OutProgIndex, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_AddPID2OutputProg Error:enErrCode[%X],PSECM[%u][%u],u16OutProgIndex[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, i, u16Index, u16OutProgIndex);
            return enErrCode;
        }
    }

    enErrCode = TSP_GetOutTSLastProgIndex(u16OutTSIndex, pstParamTS, &u16LastIndex);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetOutTSLastProgIndex Error:enErrCode[%X],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
        return enErrCode;
    }

    if (INDEX_INVALID_VALUE == u16LastIndex)
    {
        pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramIndex = u16OutProgIndex;
        pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramNum = 1;
    }
    else
    {
        pstParamTS->pOutputProgram[u16LastIndex].u16NextProgramIndex = u16OutProgIndex;
        pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramNum += 1;
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ConstructOutputTSByAddProg
  Description:  ∞—“ª∏ˆ ‰»ÎΩ⁄ƒø¿≠µΩ ‰≥ˆ¡˜÷–
  Input:        u16InProgIndex: ‰»ÎΩ⁄ƒøÀ˜“˝
                u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_ConstructOutputTSByAddProg(U16 u16InProgIndex, U16 u16OutTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 u16OutProgIndex = INDEX_INVALID_VALUE;
    
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex)
        || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InProgIndex[%u],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex, u16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_GetOutputProgIndexBySrcProgIndex(u16InProgIndex, u16OutTSIndex, pstParamTS, &u16OutProgIndex);
    if (WV_SUCCESS == enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]u16InProgIndex[%u] already exists u16OutTSIndex[%u],u16OutProgIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16InProgIndex, u16OutTSIndex, u16OutProgIndex);
        return WV_SUCCESS;
    }
    else if (WV_ERR_TSP_NOT_EXIST != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetOutputProgIndexBySrcProgIndex Error:enErrCode[%X],u16InProgIndex[%u],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16InProgIndex, u16OutTSIndex);
        return enErrCode;
    }

    enErrCode = TSP_CheckServiceIDConflict(u16InProgIndex, u16OutTSIndex, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_CheckServiceIDConflict Error:enErrCode[%X],u16InProgIndex[%u],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16InProgIndex, u16OutTSIndex);
        return enErrCode;
    }

    enErrCode = TSP_CheckPIDConflictOfProg(u16InProgIndex, u16OutTSIndex, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_CheckPIDConflictOfProg Error:enErrCode[%X],u16InProgIndex[%u],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16InProgIndex, u16OutTSIndex);
        return enErrCode;
    }

    enErrCode = TSP_AddProg2OutputTS(u16InProgIndex, u16OutTSIndex, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_AddProg2OutputTS Error:enErrCode[%X],u16InProgIndex[%u],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16InProgIndex, u16OutTSIndex);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ConstructOutputTSByAddPID
  Description:  ∞—“ª∏ˆ ‰»ÎPID(EMMPID/OtherPID)¿≠µΩ ‰≥ˆ¡˜÷–
  Input:        u8PIDType: ‰»ÎPIDµƒ¿‡–Õ
                u16InPIDIndex: ‰»ÎPIDÀ˜“˝
                u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_ConstructOutputTSByAddPID(U8 u8PIDType, U16 u16InPIDIndex, U16 u16OutTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    PIDUseInfo arstOutputTSPIDUseInfo[MAX_PID_NUMBER + 1];
    
    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InPIDIndex)
        || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InPIDIndex[%u],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InPIDIndex, u16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(arstOutputTSPIDUseInfo, 0, sizeof(arstOutputTSPIDUseInfo));
    enErrCode = TSP_GetPIDUseFlagOfOutputTS(u16OutTSIndex, pstParamTS, arstOutputTSPIDUseInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetPIDUseFlagOfOutputTS Error:enErrCode[%X],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
        return enErrCode;
    }

    enErrCode = TSP_CheckPIDConflict(u8PIDType, u16InPIDIndex, pstParamTS, arstOutputTSPIDUseInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_CheckPIDConflict Error:enErrCode[%X],u8PIDType[%u],u16InPIDIndex[%u],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u8PIDType, u16InPIDIndex, u16OutTSIndex);
        return enErrCode;
    }

    enErrCode = TSP_AddPID2OutputTS(u8PIDType, u16InPIDIndex, u16OutTSIndex, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_AddPID2OutputTS Error:enErrCode[%X],u8PIDType[%u],u16InPIDIndex[%u],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, enErrCode, u8PIDType, u16InPIDIndex, u16OutTSIndex);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     PrintOutputTS
  Description:  ¥Ú”°“ª∏ˆ ‰≥ˆ¡˜µƒ–≈œ¢
  Input:        u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS:
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
void PrintOutputTS(U16 u16OutTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 j = 0;
    U16 k = 0;
    IndexInfoList stIndexList;
    IndexInfoList stESIndexList;
    IndexInfoList stECMIndexList;
    U16 u16ProgIndex = INDEX_INVALID_VALUE;
    U16 u16ESIndex = INDEX_INVALID_VALUE;
    U16 u16ECMIndex = INDEX_INVALID_VALUE;

    if ((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutTSIndex);
        return;
    }

    printf("statr print OutTS[%u]\r\n",u16OutTSIndex);
    
    if (TSP_FLAG_VALID != pstParamTS->pOutputTS[u16OutTSIndex].u8ValidFlag)
    {
        printf("TS is invalid\r\n");
        return;
    }

    printf("Slot[%u],Ch[%u],Prog[%u][%u],EMM[%u][%u],Ohter[%u][%u]\r\n",
        pstParamTS->pOutputTS[u16OutTSIndex].u8SlotID,
        pstParamTS->pOutputTS[u16OutTSIndex].u16ChannelID,
        pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramNum,
        pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramIndex,
        pstParamTS->pOutputTS[u16OutTSIndex].u16EMMPIDNumber,
        pstParamTS->pOutputTS[u16OutTSIndex].u16EMMPIDIndex,
        pstParamTS->pOutputTS[u16OutTSIndex].u16OtherPIDNum,
        pstParamTS->pOutputTS[u16OutTSIndex].u16OtherPIDIndex);

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetOutTSProgIndexList(u16OutTSIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetOutTSProgIndexList Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        stIndexList.u16IndexListNum = 0;
    }

    printf("    start print prog info:ProgNum[%u]\r\n", stIndexList.u16IndexListNum);
    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16ProgIndex = stIndexList.aru16IndexList[i];
        printf("    Prog[%u][%u],Name[%s],Provider[%s],ServID[%u][%u],PMT[%u][%u],PCR[%u][%u]\r\n",
            i,
            u16ProgIndex,
            pstParamTS->pOutputProgram[u16ProgIndex].aru8ServiceName,
            pstParamTS->pOutputProgram[u16ProgIndex].aru8ServiceProvider,
            pstParamTS->pOutputProgram[u16ProgIndex].u16OldServiceID,
            pstParamTS->pOutputProgram[u16ProgIndex].u16ServiceID,
            pstParamTS->pOutputProgram[u16ProgIndex].u16OldPMTPID,
            pstParamTS->pOutputProgram[u16ProgIndex].u16PMTPID,
            pstParamTS->pOutputProgram[u16ProgIndex].u16OldPCRPID,
            pstParamTS->pOutputProgram[u16ProgIndex].u16PCRPID);
        printf("      TSIndex[%u],ProgIndex[%u],ProgID[%u],PCR[%u],PSECM[%u][%u],ES[%u][%u],Next[%u]\r\n",
            pstParamTS->pOutputProgram[u16ProgIndex].u16TSIndex,
            pstParamTS->pOutputProgram[u16ProgIndex].u16ProgramIndex,
            pstParamTS->pOutputProgram[u16ProgIndex].u16ProgramID,
            pstParamTS->pOutputProgram[u16ProgIndex].u16PCRIndex,
            pstParamTS->pOutputProgram[u16ProgIndex].u16PSECMPIDNumber,
            pstParamTS->pOutputProgram[u16ProgIndex].u16PSECMPIDIndex,
            pstParamTS->pOutputProgram[u16ProgIndex].u16ESNumber,
            pstParamTS->pOutputProgram[u16ProgIndex].u16PIDIndex,
            pstParamTS->pOutputProgram[u16ProgIndex].u16NextProgramIndex);

        stESIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetOutProgESPIDIndexList(u16ProgIndex, pstParamTS, &stESIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_GetOutProgESPIDIndexList Error:enErrCode[%X],Prog[%u][%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, i, u16ProgIndex);
            stESIndexList.u16IndexListNum = 0;
        }

        printf("        start print es info:ESNum[%u]\r\n", stESIndexList.u16IndexListNum);
        for (j = 0; j < stESIndexList.u16IndexListNum; j++)
        {
            u16ESIndex = stESIndexList.aru16IndexList[j];
            printf("        Es[%u][%u],PID[%u][%u],StreamType[%u],TSIndex[%u],ProgIndex[%u],PIDIndex[%u]\r\n",
                j,
                u16ESIndex,
                pstParamTS->pOutputPID[u16ESIndex].u16OldPID,
                pstParamTS->pOutputPID[u16ESIndex].u16PID,
                pstParamTS->pOutputPID[u16ESIndex].u8StreamType,
                pstParamTS->pOutputPID[u16ESIndex].u32TSIndex,
                pstParamTS->pOutputPID[u16ESIndex].u32ProgramIndex,
                pstParamTS->pOutputPID[u16ESIndex].u32PIDIndex);

            printf("          ProgID[%u],PIDID[%u],ESECM[%u][%u],Next[%u]\r\n",
                pstParamTS->pOutputPID[u16ESIndex].u16ProgramID,
                pstParamTS->pOutputPID[u16ESIndex].u16PIDID,
                pstParamTS->pOutputPID[u16ESIndex].u16ESECMPIDNumber,
                pstParamTS->pOutputPID[u16ESIndex].u16ESECMPIDIndex,
                pstParamTS->pOutputPID[u16ESIndex].u32NextPIDIndex);

            stECMIndexList.u16IndexListNum = 0;
            enErrCode = TSP_GetOutPIDESECMPIDIndexList(u16ESIndex, pstParamTS, &stECMIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                   "[%s:%d]TSP_GetOutProgESPIDIndexList Error:enErrCode[%X],u16ESIndex[%u][%u]\r\n",
                   __FUNCTION__, __LINE__, enErrCode, j, u16ESIndex);
                stECMIndexList.u16IndexListNum = 0;
            }
            printf("            statr print esecm info:ESECMNum[%u]\r\n", stECMIndexList.u16IndexListNum);
            for (k = 0; k < stECMIndexList.u16IndexListNum; k++)
            {
                u16ECMIndex = stECMIndexList.aru16IndexList[k];
                printf("            ESECM[%u][%u],PID[%u][%u],CASID[%u],TSIndex[%u],ProgIndex[%u],PIDIndex[%u]\r\n",
                    k,
                    u16ECMIndex,
                    pstParamTS->pOutputPID[u16ECMIndex].u16OldPID,
                    pstParamTS->pOutputPID[u16ECMIndex].u16PID,
                    pstParamTS->pOutputPID[u16ECMIndex].u16CASID,
                    pstParamTS->pOutputPID[u16ECMIndex].u32TSIndex,
                    pstParamTS->pOutputPID[u16ECMIndex].u32ProgramIndex,
                    pstParamTS->pOutputPID[u16ECMIndex].u32PIDIndex);

                printf("              ProgID[%u],PIDID[%u],ESECM[%u][%u],Next[%u]\r\n",
                    pstParamTS->pOutputPID[u16ECMIndex].u16ProgramID,
                    pstParamTS->pOutputPID[u16ECMIndex].u16PIDID,
                    pstParamTS->pOutputPID[u16ECMIndex].u16ESECMPIDNumber,
                    pstParamTS->pOutputPID[u16ECMIndex].u16ESECMPIDIndex,
                    pstParamTS->pOutputPID[u16ECMIndex].u32NextPIDIndex);

            }
            printf("            end print esecm info\r\n");
            
        }
        printf("        end print es info\r\n");

        stECMIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetOutProgPSECMPIDIndexList(u16ProgIndex, pstParamTS, &stECMIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_GetOutProgPSECMPIDIndexList Error:enErrCode[%X],Prog[%u][%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, i, u16ProgIndex);
            stECMIndexList.u16IndexListNum = 0;
        }

        printf("        start print psecm info:PSECMNum[%u]\r\n", stECMIndexList.u16IndexListNum);
        for (j = 0; j < stECMIndexList.u16IndexListNum; j++)
        {
            u16ECMIndex = stECMIndexList.aru16IndexList[j];
            printf("        ESECM[%u][%u],PID[%u][%u],CASID[%u],TSIndex[%u],ProgIndex[%u],PIDIndex[%u]\r\n",
                j,
                u16ECMIndex,
                pstParamTS->pOutputPID[u16ECMIndex].u16OldPID,
                pstParamTS->pOutputPID[u16ECMIndex].u16PID,
                pstParamTS->pOutputPID[u16ECMIndex].u16CASID,
                pstParamTS->pOutputPID[u16ECMIndex].u32TSIndex,
                pstParamTS->pOutputPID[u16ECMIndex].u32ProgramIndex,
                pstParamTS->pOutputPID[u16ECMIndex].u32PIDIndex);

            printf("          ProgID[%u],PIDID[%u],ESECM[%u][%u],Next[%u]\r\n",
                pstParamTS->pOutputPID[u16ECMIndex].u16ProgramID,
                pstParamTS->pOutputPID[u16ECMIndex].u16PIDID,
                pstParamTS->pOutputPID[u16ECMIndex].u16ESECMPIDNumber,
                pstParamTS->pOutputPID[u16ECMIndex].u16ESECMPIDIndex,
                pstParamTS->pOutputPID[u16ECMIndex].u32NextPIDIndex);

        }
        printf("        end print psecm info\r\n");
    }
    printf("    end print prog info\r\n");

    stIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetOutTSEMMPIDIndexList(u16OutTSIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetOutTSEMMPIDIndexList Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        stIndexList.u16IndexListNum = 0;
    }

    printf("    start print EMM info:EMMNum[%u]\r\n", stIndexList.u16IndexListNum);
    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16ProgIndex = stIndexList.aru16IndexList[i];
        printf("    EMM[%u][%u],PID[%u][%u],CASID[%u],TSIndex[%u],NextIndex[%u]\r\n",
            i,
            u16ProgIndex,
            pstParamTS->pOutputPID[u16ProgIndex].u16OldPID,
            pstParamTS->pOutputPID[u16ProgIndex].u16PID,
            pstParamTS->pOutputPID[u16ProgIndex].u16CASID,
            pstParamTS->pOutputPID[u16ProgIndex].u32TSIndex,
            pstParamTS->pOutputPID[u16ProgIndex].u32NextPIDIndex);
    }
    printf("    end print EMM info\r\n");

    printf("end print OutTS[%u]\r\n",u16OutTSIndex);
}

/*****************************************************************************
  Function:     TSP_IsCIOutputLUTEntryAlreadyExist
  Description:  ≈–∂œ“ª∏ˆLUTµ•‘™ «∑Ò“—æ≠¥Ê‘⁄
  Input:        pstEntry:“™≈–∂œµƒLUTµ•‘™µƒ÷∏’Î
                arstLUTEntry:¥Ê∑≈À˘”–µƒLUTµ•‘™ ˝◊È
                u16EntryNum:LUTµ•‘™∏ˆ ˝
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
BOOL TSP_IsCIOutputLUTEntryAlreadyExist(OUTPUTLUTEntry *pstEntry, OUTPUTLUTEntry arstLUTEntry[MAX_OUTPUT_LUT_ENTRY_NUM], U16 u16EntryNum)
{
    U16 i = 0;
    
    if ((!pstEntry) || (!arstLUTEntry) || (u16EntryNum > MAX_OUTPUT_LUT_ENTRY_NUM))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstEntry[%p],arstLUTEntry[%p],u16EntryNum[%u]\r\n",
            __FUNCTION__, __LINE__, pstEntry, arstLUTEntry, u16EntryNum);
        return false;
    }

    for (i = 0; i < u16EntryNum; i++)
    {
        if (0 == memcmp(pstEntry, &arstLUTEntry[i], sizeof(OUTPUTLUTEntry)))
        {
            return true;
        }
    }

    return false;
}

/*****************************************************************************
  Function:     TSP_GetCIOutputLUTEntry
  Description:  ªÒ»°À˘”–µƒLUTµ•‘™
  Input:        pstParamTS
  Output:       arstLUTEntry:¥Ê∑≈À˘”–µƒLUTµ•‘™ ˝◊È
                u16EntryNum:LUTµ•‘™∏ˆ ˝
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_GetCIOutputLUTEntry(TSInfo *pstParamTS, OUTPUTLUTEntry arstLUTEntry[MAX_OUTPUT_LUT_ENTRY_NUM], U16 *pu16EntryNum)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 j = 0;
    U16 k = 0;
    U8 u8CamIndex = 0;
    U16 u16OutPIDIndex = INDEX_INVALID_VALUE;
    U16 u16OutProgIndex = INDEX_INVALID_VALUE;
    U16 u16OutTSIndex = INDEX_INVALID_VALUE;
    U16 u16EntryNum = 0;

    IndexInfoList stIndexList;
    IndexInfoList stESIndexList;
    IndexInfoList stECMIndexList;

    OUTPUTLUTEntry stTempEntry;

    if ((!pstParamTS) || (!arstLUTEntry) || (!pu16EntryNum))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],arstLUTEntry[%p],pu16EntryNum[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, arstLUTEntry, pu16EntryNum);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    for (u8CamIndex = 0; u8CamIndex < WVCI_MAX_SLOT_NUM; u8CamIndex++)
    {
        if (WV_SUCCESS != TSP_GetOutputTSIndex(u8CamIndex, pstParamTS, &u16OutTSIndex))
        {
            continue;
        }
        
        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetOutTSProgIndexList(u16OutTSIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_GetOutTSProgIndexList Error:enErrCode[%X],u16OutTSIndex[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
            return enErrCode;
        }

        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            u16OutProgIndex = stIndexList.aru16IndexList[i];

            if (pstParamTS->pOutputProgram[u16OutProgIndex].u16PCRPID < MAX_PID_NUMBER)
            {
                u16OutPIDIndex = pstParamTS->pOutputProgram[u16OutProgIndex].u16PCRIndex;
                stTempEntry.u8SrcSlot = Status_GetSlotID();
                stTempEntry.u16SrcStream = TSP_InputChannel2StreamID(pstParamTS->pOutputPID[u16OutPIDIndex].u16SourceChannelID);
                stTempEntry.u16SrcPID = pstParamTS->pOutputPID[u16OutPIDIndex].u16OldPID;
                stTempEntry.u16DestStream = TSP_CICamIndex2StreamID(u8CamIndex);
                stTempEntry.u16DestPID = pstParamTS->pOutputPID[u16OutPIDIndex].u16PID;

                if (!TSP_IsCIOutputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                    && (u16EntryNum < MAX_OUTPUT_LUT_ENTRY_NUM))
                {
                    memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(OUTPUTLUTEntry));
                }
            }
            
            stESIndexList.u16IndexListNum = 0;
            enErrCode = TSP_GetOutProgESPIDIndexList(u16OutProgIndex, pstParamTS, &stESIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                   "[%s:%d]TSP_GetOutProgESPIDIndexList Error:enErrCode[%X],u16OutProgIndex[%u][%u]\r\n",
                   __FUNCTION__, __LINE__, enErrCode, i, u16OutProgIndex);
                return enErrCode;
            }
    
            for (j = 0; j < stESIndexList.u16IndexListNum; j++)
            {
                u16OutPIDIndex = stESIndexList.aru16IndexList[j];
                
                stTempEntry.u8SrcSlot = Status_GetSlotID();
                stTempEntry.u16SrcStream = TSP_InputChannel2StreamID(pstParamTS->pOutputPID[u16OutPIDIndex].u16SourceChannelID);
                stTempEntry.u16SrcPID = pstParamTS->pOutputPID[u16OutPIDIndex].u16OldPID;
                stTempEntry.u16DestStream = TSP_CICamIndex2StreamID(u8CamIndex);
                stTempEntry.u16DestPID = pstParamTS->pOutputPID[u16OutPIDIndex].u16PID;

                if (!TSP_IsCIOutputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                    && (u16EntryNum < MAX_OUTPUT_LUT_ENTRY_NUM))
                {
                    memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(OUTPUTLUTEntry));
                }

                stECMIndexList.u16IndexListNum = 0;
                enErrCode = TSP_GetOutPIDESECMPIDIndexList(u16OutPIDIndex, pstParamTS, &stECMIndexList);
                if (WV_SUCCESS != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                       "[%s:%d]TSP_GetOutPIDESECMPIDIndexList Error:enErrCode[%X],u16OutPIDIndex[%u][%u]\r\n",
                       __FUNCTION__, __LINE__, enErrCode, i, u16OutPIDIndex);
                    return enErrCode;
                }
        
                for (k = 0; k < stECMIndexList.u16IndexListNum; k++)
                {
                    u16OutPIDIndex = stECMIndexList.aru16IndexList[k];
                    
                    stTempEntry.u8SrcSlot = Status_GetSlotID();
                    stTempEntry.u16SrcStream = TSP_InputChannel2StreamID(pstParamTS->pOutputPID[u16OutPIDIndex].u16SourceChannelID);
                    stTempEntry.u16SrcPID = pstParamTS->pOutputPID[u16OutPIDIndex].u16OldPID;
                    stTempEntry.u16DestStream = TSP_CICamIndex2StreamID(u8CamIndex);
                    stTempEntry.u16DestPID = pstParamTS->pOutputPID[u16OutPIDIndex].u16PID;

                    if (!TSP_IsCIOutputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                        && (u16EntryNum < MAX_OUTPUT_LUT_ENTRY_NUM))
                    {
                        memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(OUTPUTLUTEntry));
                    }
                }
            }

            stECMIndexList.u16IndexListNum = 0;
            enErrCode = TSP_GetOutProgPSECMPIDIndexList(u16OutProgIndex, pstParamTS, &stECMIndexList);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                   "[%s:%d]TSP_GetOutProgPSECMPIDIndexList Error:enErrCode[%X],u16OutProgIndex[%u][%u]\r\n",
                   __FUNCTION__, __LINE__, enErrCode, i, u16OutProgIndex);
                return enErrCode;
            }
    
            for (j = 0; j < stECMIndexList.u16IndexListNum; j++)
            {
                u16OutPIDIndex = stECMIndexList.aru16IndexList[j];
                
                stTempEntry.u8SrcSlot = Status_GetSlotID();
                stTempEntry.u16SrcStream = TSP_InputChannel2StreamID(pstParamTS->pOutputPID[u16OutPIDIndex].u16SourceChannelID);
                stTempEntry.u16SrcPID = pstParamTS->pOutputPID[u16OutPIDIndex].u16OldPID;
                stTempEntry.u16DestStream = TSP_CICamIndex2StreamID(u8CamIndex);
                stTempEntry.u16DestPID = pstParamTS->pOutputPID[u16OutPIDIndex].u16PID;

                if (!TSP_IsCIOutputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                    && (u16EntryNum < MAX_OUTPUT_LUT_ENTRY_NUM))
                {
                    memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(OUTPUTLUTEntry));
                }
            }
        }

        #if 1
        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetOutTSEMMPIDIndexList(u16OutTSIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
               "[%s:%d]TSP_GetOutTSEMMPIDIndexList Error:enErrCode[%X],u16OutTSIndex[%u]\r\n",
               __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
            return enErrCode;
        }

        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            u16OutPIDIndex = stIndexList.aru16IndexList[i];

            stTempEntry.u8SrcSlot = Status_GetSlotID();
            stTempEntry.u16SrcStream = TSP_InputChannel2StreamID(pstParamTS->pOutputPID[u16OutPIDIndex].u16SourceChannelID);
            stTempEntry.u16SrcPID = pstParamTS->pOutputPID[u16OutPIDIndex].u16OldPID;
            stTempEntry.u16DestStream = TSP_CICamIndex2StreamID(u8CamIndex);
            stTempEntry.u16DestPID = pstParamTS->pOutputPID[u16OutPIDIndex].u16PID;

            if (!TSP_IsCIOutputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                && (u16EntryNum < MAX_OUTPUT_LUT_ENTRY_NUM))
            {
                memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(OUTPUTLUTEntry));
            }
        }
        #endif
    }

    *pu16EntryNum = u16EntryNum;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SortCIOutputLUTEntry
  Description:  Ω´À˘”–µƒLUTµ•‘™∞¥Src–≈œ¢¥”–°µΩ¥Û≈≈–Ú
  Input:        arstLUTEntry:À˘”–µƒLUTµ•‘™ ˝◊È
                u16EntryNum:LUTµ•‘™∏ˆ ˝
  Output:       
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
void TSP_SortCIOutputLUTEntry(OUTPUTLUTEntry arstLUTEntry[MAX_OUTPUT_LUT_ENTRY_NUM], U16 u16EntryNum)
{
    U16 i = 0;
    U16 j = 0;
    U32 u32TempData = 0;
    U32 u32TempData2 = 0;
    OUTPUTLUTEntry stTempEntry;

    if ((!arstLUTEntry) || (u16EntryNum > MAX_OUTPUT_LUT_ENTRY_NUM))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:arstLUTEntry[%p],u16EntryNum[%u]\r\n",
            __FUNCTION__, __LINE__, arstLUTEntry, u16EntryNum);
        return;
    }
    
    for (i = 0; i < u16EntryNum; i++)
    {
        for (j = 0; j < (u16EntryNum - i - 1); j++)
        {
            u32TempData = ((arstLUTEntry[j].u8SrcSlot & 0x3F) << 23)
                            | ((arstLUTEntry[j].u16SrcStream & 0x3FF) << 13)
                            | (arstLUTEntry[j].u16SrcPID & 0x1FFF);

            u32TempData2 = ((arstLUTEntry[j + 1].u8SrcSlot & 0x3F) << 23)
                            | ((arstLUTEntry[j + 1].u16SrcStream & 0x3FF) << 13)
                            | (arstLUTEntry[j + 1].u16SrcPID & 0x1FFF);
                
            if (u32TempData > u32TempData2)
            {
                stTempEntry = arstLUTEntry[j];
                arstLUTEntry[j] = arstLUTEntry[j + 1];
                arstLUTEntry[j + 1] = stTempEntry;
            }
        }
    }
}

/*****************************************************************************
  Function:     TSP_SumTheSameCIOutputLUTEntry
  Description:  Õ≥º∆”–∂‡…Ÿ∏ˆœ‡Õ¨Src–≈œ¢µƒLUTµ•‘™
  Input:        arstLUTEntry:À˘”–µƒLUTµ•‘™ ˝◊È
                u16EntryNum:LUTµ•‘™∏ˆ ˝
  Output:       
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
void TSP_SumTheSameCIOutputLUTEntry(OUTPUTLUTEntry arstLUTEntry[MAX_OUTPUT_LUT_ENTRY_NUM], U16 u16EntryNum)
{
    U16 i = 0;
    U16 j = 0;
    U16 k = 0;

    if ((!arstLUTEntry) || (u16EntryNum > MAX_OUTPUT_LUT_ENTRY_NUM))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:arstLUTEntry[%p],u16EntryNum[%u]\r\n",
            __FUNCTION__, __LINE__, arstLUTEntry, u16EntryNum);
        return;
    }
    
    while (i < u16EntryNum)
    {
        for (j = i + 1; j < u16EntryNum; j++)
        {
            if ((arstLUTEntry[i].u8SrcSlot != arstLUTEntry[j].u8SrcSlot)
                || (arstLUTEntry[i].u16SrcStream != arstLUTEntry[j].u16SrcStream)
                || (arstLUTEntry[i].u16SrcPID != arstLUTEntry[j].u16SrcPID))
            {
                break;
            }
        }

        for (k = i; k < j; k++)
        {
            arstLUTEntry[k].u8Num = j - i - 1;
        }

        i = j;
    }
}

/*****************************************************************************
  Function:     TSP_SetCIOutputLUT
  Description:  …Ë÷√CI ‰≥ˆLUT
  Input:        pstParamTS
  Output:       
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_SetCIOutputLUT(TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    OUTPUTLUTEntry arstLUTEntry[MAX_OUTPUT_LUT_ENTRY_NUM];
    U16 u16EntryNum = 0;
    U32 u32LUTAddr = 0;
    U32 u32LUTDataH = 0;
    U32 u32LUTDataL = 0;

    if (!pstParamTS)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(arstLUTEntry, 0, sizeof(arstLUTEntry));

    enErrCode = TSP_GetCIOutputLUTEntry(pstParamTS, arstLUTEntry, &u16EntryNum);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]TSP_GetOutputLUTEntry Error:enErrCode[%X]\r\n",
           __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    TSP_SortCIOutputLUTEntry(arstLUTEntry, u16EntryNum);

    TSP_SumTheSameCIOutputLUTEntry(arstLUTEntry, u16EntryNum);

    for (i = 0; i < u16EntryNum; i++)
    {
        printf("Entry[%4u],u8Num[%3u],u8SrcSlot[%2u],u16SrcStream[%3u],u16SrcPID[%4u],u16DestStream[%3u],u16DestPID[%4u]\r\n",
            i,
            arstLUTEntry[i].u8Num,
            arstLUTEntry[i].u8SrcSlot,
            arstLUTEntry[i].u16SrcStream,
            arstLUTEntry[i].u16SrcPID,
            arstLUTEntry[i].u16DestStream,
            arstLUTEntry[i].u16DestPID);
    }

    for (i = 0; i < u16EntryNum; i++)
    {
        u32LUTAddr = (1 << 11) | i;

        u32LUTDataH = (1 << 28)
                        | (arstLUTEntry[i].u8Num << 20)
                        | ((arstLUTEntry[i].u8SrcSlot & 0x3F) << 14)
                        | ((arstLUTEntry[i].u16SrcStream & 0x3FF) << 4)
                        | ((arstLUTEntry[i].u16SrcPID >> 9) & 0xF);
        u32LUTDataL = ((arstLUTEntry[i].u16SrcPID & 0x1FF) << 23)
                        | ((arstLUTEntry[i].u16DestStream & 0x3FF) << 13)
                        | (arstLUTEntry[i].u16DestPID & 0x1FFF);
        
        FPGA_REG_Write(OUTPUT_LUT_ADDR, u32LUTAddr);
        FPGA_REG_Write(OUTPUT_LUT_DATA_H, u32LUTDataH);
        FPGA_REG_Write(OUTPUT_LUT_DATA_L, u32LUTDataL);

        FPGA_REG_Write(OUTPUT_LUT_WRITE_EN, 0);
        FPGA_REG_Write(OUTPUT_LUT_WRITE_EN, 1);
        FPGA_REG_Write(OUTPUT_LUT_WRITE_EN, 0);
    }

    for (i = u16EntryNum; i < MAX_OUTPUT_LUT_ENTRY_NUM; i++)
    {
        FPGA_REG_Write(OUTPUT_LUT_ADDR, (1 << 11) | i);
        FPGA_REG_Write(OUTPUT_LUT_DATA_H, 0);
        FPGA_REG_Write(OUTPUT_LUT_DATA_L, 0);

        FPGA_REG_Write(OUTPUT_LUT_WRITE_EN, 0);
        FPGA_REG_Write(OUTPUT_LUT_WRITE_EN, 1);
        FPGA_REG_Write(OUTPUT_LUT_WRITE_EN, 0);
    }

    FPGA_REG_Write(OUTPUT_LUT_SWAP, 0);
    FPGA_REG_Write(OUTPUT_LUT_SWAP, 1);
    FPGA_REG_Write(OUTPUT_LUT_SWAP, 0);

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_BypassAllTS2CIOutput
  Description:  ∞—4∏ˆTunerΩ¯¿¥µƒ¡˜¥”CI ‰≥ˆLUT÷–÷±Õ®≥ˆ»•
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
void TSP_BypassAllTS2CIOutput(void)
{
    U16 i = 0;
    U8 u8SrcSlot = Status_GetSlotID();
    U16 u16StreamID = 0;

    U32 u32LUTAddr = 0;
    U32 u32LUTDataH = 0;
    U32 u32LUTDataL = 0;

    for (i = 0; i < MAX_INPUT_CHANNEL_NUM; i++)
    {
        u16StreamID = i + 0xFC;

        u32LUTAddr = i;
        u32LUTDataH = 1 << 2;
        u32LUTDataL = ((u8SrcSlot & 0x3F) << 20)
                    | ((u16StreamID & 0x3FF) << 10)
                    | (u16StreamID & 0x3FF);

        FPGA_REG_Write(OUTPUT_LUT_ADDR, u32LUTAddr);
        FPGA_REG_Write(OUTPUT_LUT_DATA_H, u32LUTDataH);
        FPGA_REG_Write(OUTPUT_LUT_DATA_L, u32LUTDataL);

        FPGA_REG_Write(OUTPUT_LUT_WRITE_EN, 0);
        FPGA_REG_Write(OUTPUT_LUT_WRITE_EN, 1);
        FPGA_REG_Write(OUTPUT_LUT_WRITE_EN, 0);
    }

    for (i = MAX_INPUT_CHANNEL_NUM; i < MAX_OUTPUT_BYPASS_LUT_ENTRY_NUM; i++)
    {
        FPGA_REG_Write(OUTPUT_LUT_ADDR, i);
        FPGA_REG_Write(OUTPUT_LUT_DATA_H, 0);
        FPGA_REG_Write(OUTPUT_LUT_DATA_L, 0);

        FPGA_REG_Write(OUTPUT_LUT_WRITE_EN, 0);
        FPGA_REG_Write(OUTPUT_LUT_WRITE_EN, 1);
        FPGA_REG_Write(OUTPUT_LUT_WRITE_EN, 0);
    }

    FPGA_REG_Write(OUTPUT_LUT_SWAP, 0);
    FPGA_REG_Write(OUTPUT_LUT_SWAP, 1);
    FPGA_REG_Write(OUTPUT_LUT_SWAP, 0);
}

/*****************************************************************************
  Function:     TSP_DebugPrintCIOutputLUTInfo
  Description:  ¥Ú”°CI ‰≥ˆLUT
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
void TSP_DebugPrintCIOutputLUTInfo(void)
{
    U16 i = 0;
    U32 u32LUTDataH = 0;
    U32 u32LUTDataL = 0;

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "Start print CI output LUT info:\r\n");
    
    for (i = 0; i < MAX_OUTPUT_LUT_ENTRY_NUM; i++)
    {
        FPGA_REG_Write(OUTPUT_LUT_ADDR, (1 << 11) | i);
        FPGA_REG_Read(OUTPUT_LUT_READ_BACK_H, &u32LUTDataH);
        FPGA_REG_Read(OUTPUT_LUT_READ_BACK_L, &u32LUTDataL);
        
        if ((0 == u32LUTDataH) && (0 == u32LUTDataL))
        {
            continue;
        }

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "Addr[%08X],Data[%08X %08X]\r\n", ((1 << 11) | i), u32LUTDataH, u32LUTDataL);
    }

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "Start print CI output bypass LUT info:\r\n");

    for (i = 0; i < MAX_OUTPUT_BYPASS_LUT_ENTRY_NUM; i++)
    {
        FPGA_REG_Write(OUTPUT_LUT_ADDR, i);
        FPGA_REG_Read(OUTPUT_LUT_READ_BACK_H, &u32LUTDataH);
        FPGA_REG_Read(OUTPUT_LUT_READ_BACK_L, &u32LUTDataL);
        
        if ((0 == u32LUTDataH) && (0 == u32LUTDataL))
        {
            continue;
        }

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
            "Addr[%08X],Data[%08X %08X]\r\n", i, u32LUTDataH, u32LUTDataL);
    }

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "End print CI output bypass LUT info:\r\n");
}

/*****************************************************************************
  Function:     TSP_GetOutputPATTable
  Description:  ªÒ»° ‰≥ˆ¡˜µƒPPT±Ì–≈œ¢
  Input:        u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS
  Output:       pstAMTTable:ªÒ»°µΩµƒPAT±Ì–≈œ¢
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_GetOutputPATTable(U16 u16OutTSIndex, TSInfo *pstParamTS, PATTable_t *pstPATTable)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16ProgramOffset = 0;

    IndexInfoList stIndexInfoList;

    if ((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex) || (!pstPATTable))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16OutTSIndex[%u],pstPATTable[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutTSIndex, pstPATTable);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(pstPATTable, 0, sizeof(PATTable_t));

    memset(&stIndexInfoList, 0, sizeof(stIndexInfoList));
    enErrCode = TSP_GetOutTSProgIndexList(u16OutTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutTSProgIndexList Error:u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex);
        return enErrCode;        
    }

    if (MAX_PROGRAMS_NUMBER_IN_PAT < stIndexInfoList.u16IndexListNum)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]u16OutTSIndex[%u],ProgNum[%u] out of rang\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex, stIndexInfoList.u16IndexListNum);
        return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE; 
    }

    pstPATTable->u16TransportStreamID = pstParamTS->pOutputTS[u16OutTSIndex].u16TransportStreamID;
    pstPATTable->u8VersionNum         = pstParamTS->pOutputTS[u16OutTSIndex].u8VersionNumber;

    // ±‰¡øΩ⁄ƒø¡–±Ì£¨ªÒ»°pmt°¢service idµ»Ω⁄ƒø–≈œ¢
    for (i = 0; i < stIndexInfoList.u16IndexListNum; i++)
    {
        u16ProgramOffset = stIndexInfoList.aru16IndexList[i];

        pstPATTable->arstProgramInfo[i].u16PMTPid    = pstParamTS->pOutputProgram[u16ProgramOffset].u16PMTPID;
        pstPATTable->arstProgramInfo[i].u16ServiceId = pstParamTS->pOutputProgram[u16ProgramOffset].u16ServiceID;
        pstPATTable->u16ProgramCount++;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutputCATTable
  Description:  ªÒ»° ‰≥ˆΩ⁄ƒøµƒPMT±Ì–≈œ¢
  Input:        u16OutProgIndex: ‰≥ˆΩ⁄ƒøÀ˜“˝
                pstParamTS
  Output:       pstPMTTable:ªÒ»°µΩµƒPMT±Ì–≈œ¢
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_GetOutputPMTTable(U16 u16OutProgIndex, const TSInfo *pstParamTS, PMTTable_t *pstPMTTable)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 u16PIDListOffset      = 0;
    U16 u16PSECMPIDListOffset = 0;
    U16 u16ESECMPIDListOffset = 0;
    U16 i                     = 0;
    U16 j                     = 0;

    const Output_TSProgram *pstTSProg = NULL;
    const Output_TSPID *pstESPID = NULL;
    const Output_TSPID *pstECMPID = NULL;
    PMTESInfo_t   *pstPMTESInfo = NULL;
    PmtCAInfo_t   *pstPmtCAInfo = NULL;

    IndexInfoList stESIndexInfoList;
    IndexInfoList stECMIndexInfoList;

    if ((!pstParamTS) || (pstParamTS->u32OutputProgramNumber <= u16OutProgIndex) || (!pstPMTTable))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16OutProgIndex[%u],pstPMTTable[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutProgIndex, pstPMTTable);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(pstPMTTable, 0, sizeof(PMTTable_t));

    memset(&stESIndexInfoList, 0, sizeof(stESIndexInfoList));
    enErrCode = TSP_GetOutProgESPIDIndexList(u16OutProgIndex, pstParamTS, &stESIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutProgESPIDIndexList Error:u16OutProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16OutProgIndex);
        return enErrCode;        
    }

    // ES PID∏ˆ ˝‘ΩΩÁ ∑µªÿ ß∞‹
    if (MAX_ES_NUM_IN_PMT < stESIndexInfoList.u16IndexListNum)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]u16OutProgIndex[%u],ESNum[%u] out of rang\r\n",
            __FUNCTION__, __LINE__, u16OutProgIndex, stESIndexInfoList.u16IndexListNum);
        return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE; 
    }

    pstTSProg = &pstParamTS->pOutputProgram[u16OutProgIndex];

    // Ω⁄ƒø√Ë ˆ–≈œ¢‘ΩΩÁ ∑µªÿ ß∞‹
    if (MAX_PROGRAM_INFO_LEN < pstTSProg->u16ProgramInfoLength)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]u16OutProgIndex[%u],u16ProgramInfoLength[%u] out of rang\r\n",
            __FUNCTION__, __LINE__, u16OutProgIndex, pstTSProg->u16ProgramInfoLength);
        return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE; 
    }

    // ES–≈œ¢¥¶¿Ì
    for (i = 0; i < stESIndexInfoList.u16IndexListNum; i++)
    {
        u16PIDListOffset = stESIndexInfoList.aru16IndexList[i];
        pstESPID = &pstParamTS->pOutputPID[u16PIDListOffset];
        pstPMTESInfo = &pstPMTTable->arstPMTESInfo[i];

        if (MAX_ES_INFO_LEN < pstESPID->u16ESInfoLength)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]u16OutProgIndex[%u],ES[%u][%u],u16ProgramInfoLength[%u] out of rang\r\n",
                __FUNCTION__, __LINE__, u16OutProgIndex, i, u16PIDListOffset, pstESPID->u16ESInfoLength);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE; 
        }

        // ≤È’“ES ECM–≈œ¢
        memset(&stECMIndexInfoList, 0, sizeof(stECMIndexInfoList));
        enErrCode = TSP_GetOutPIDESECMPIDIndexList(u16PIDListOffset, pstParamTS, &stECMIndexInfoList);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetOutPIDESECMPIDIndexList Error:u16OutProgIndex[%u],ES[%u][%u]\r\n",
                __FUNCTION__, __LINE__, u16OutProgIndex, i, u16PIDListOffset);
            return enErrCode;        
        }

        // ECM∏ˆ ˝‘ΩΩÁ ∑µªÿ ß∞‹
        if (MAX_CASYS_NUM_IN_PMT < stECMIndexInfoList.u16IndexListNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]u16OutProgIndex[%u],ES[%u][%u],ECMNum[%u] out of rang\r\n",
                __FUNCTION__, __LINE__, u16OutProgIndex, i, u16PIDListOffset, stECMIndexInfoList.u16IndexListNum);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }

        // ES –≈œ¢∏≥÷µ
        pstPMTESInfo->u8CaSysNum    = (U8)stECMIndexInfoList.u16IndexListNum;
        pstPMTESInfo->u8Type        = pstESPID->u8StreamType;
        pstPMTESInfo->u16Pid        = pstESPID->u16PID;
        pstPMTESInfo->u16ESInfoLen  = pstESPID->u16ESInfoLength;
        if (0 < pstPMTESInfo->u16ESInfoLen)
        {
            memcpy(pstPMTESInfo->aru8ESInfo, pstESPID->aru8ESInfo, pstESPID->u16ESInfoLength);
        }

        // ESµƒECM–≈œ¢∏≥÷µ
        for (j = 0; j < stECMIndexInfoList.u16IndexListNum; j++)
        {
            u16ESECMPIDListOffset = stECMIndexInfoList.aru16IndexList[j];
            pstECMPID    = &pstParamTS->pOutputPID[u16ESECMPIDListOffset];
            pstPmtCAInfo = &pstPMTTable->arstPMTESInfo[i].arstCaInfo[j];

            pstPmtCAInfo->u16EcmPid  = pstECMPID->u16PID;
            pstPmtCAInfo->u16CaSysID = pstECMPID->u16CASID;
        }
    }

    // ≤È’“PS ECM–≈œ¢
    memset(&stECMIndexInfoList, 0, sizeof(stECMIndexInfoList));
    enErrCode = TSP_GetOutProgPSECMPIDIndexList(u16OutProgIndex, pstParamTS, &stECMIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutProgPSECMPIDIndexList Error:u16OutProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16OutProgIndex);
        return enErrCode;        
    }

    // PS ECM∏ˆ ˝‘ΩΩÁ ∑µªÿ ß∞‹
    if (MAX_CASYS_NUM_IN_PMT < stECMIndexInfoList.u16IndexListNum)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]u16OutProgIndex[%u],PSECMNum[%u] out of rang\r\n",
            __FUNCTION__, __LINE__, u16OutProgIndex, stECMIndexInfoList.u16IndexListNum);
        return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE; 
    }

    // PS ECM –≈œ¢∏≥÷µ
    pstPMTTable->u8CaSysNum = (U8)stECMIndexInfoList.u16IndexListNum;
    for (i = 0; i < stECMIndexInfoList.u16IndexListNum; i++)
    {
        u16PSECMPIDListOffset = stECMIndexInfoList.aru16IndexList[i];
        pstECMPID = &pstParamTS->pOutputPID[u16PSECMPIDListOffset];

        pstPmtCAInfo = &pstPMTTable->arstCaInfo[i];

        pstPmtCAInfo->u16EcmPid  = pstECMPID->u16PID;
        pstPmtCAInfo->u16CaSysID = pstECMPID->u16CASID;
    }

    // Ω⁄ƒø–≈œ¢∏≥÷µ
    pstPMTTable->u16ServiceId   = pstTSProg->u16ServiceID;
    pstPMTTable->u16PCRPID      = pstTSProg->u16PCRPID;
    pstPMTTable->u8VersionNum   = pstTSProg->u8PMTVersionNumber;
    pstPMTTable->u8EsNum        = (U8)stESIndexInfoList.u16IndexListNum;
    pstPMTTable->u16ProgInfoLen = pstTSProg->u16ProgramInfoLength;
    if (0 < pstPMTTable->u16ProgInfoLen)
    {
        memcpy(pstPMTTable->aru8ProgInfo, pstTSProg->aru8ProgramInfo, pstTSProg->u16ProgramInfoLength);
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutputCATTable
  Description:  ªÒ»° ‰≥ˆ¡˜µƒCAT±Ì–≈œ¢
  Input:        u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS
  Output:       pstCATTable:ªÒ»°µΩµƒCAT±Ì–≈œ¢
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_GetOutputCATTable(U16 u16OutTSIndex, TSInfo *pstParamTS, CATTable_t *pstCATTable)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16EMMPIDListOffset = 0;
    const Output_TSPID *pstEMMPID = NULL;
    IndexInfoList stIndexInfoList;

    if ((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex) || (!pstCATTable))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16OutTSIndex[%u],pstCATTable[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutTSIndex, pstCATTable);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(pstCATTable, 0, sizeof(CATTable_t));

    memset(&stIndexInfoList, 0, sizeof(stIndexInfoList));
    enErrCode = TSP_GetOutTSEMMPIDIndexList(u16OutTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutTSEMMPIDIndexList Error:u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex);
        return enErrCode;
    }

    if (MAX_CASYS_NUM_IN_CAT < stIndexInfoList.u16IndexListNum)
    {
       log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]u16OutTSIndex[%u],CaSysNum[%u] out of rang\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex, stIndexInfoList.u16IndexListNum);
        return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE; 
    }

    // EMM –≈œ¢∏≥÷µ
    for (i = 0; i < stIndexInfoList.u16IndexListNum; i++)
    {
        u16EMMPIDListOffset = stIndexInfoList.aru16IndexList[i];

        pstEMMPID = &pstParamTS->pOutputPID[u16EMMPIDListOffset];

        pstCATTable->arstCaInfo[i].u16CaSysID = pstEMMPID->u16CASID;
        pstCATTable->arstCaInfo[i].u16EmmPid  = pstEMMPID->u16PID;
    }

    pstCATTable->u8VersionNum = pstParamTS->pOutputTS[u16OutTSIndex].u8VersionNumber;
    pstCATTable->u16CaSysNum  = stIndexInfoList.u16IndexListNum;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_GetOutputSDTTable
  Description:  ªÒ»° ‰≥ˆ¡˜µƒSDT±Ì–≈œ¢
  Input:        u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS
  Output:       pstSDTTable:ªÒ»°µΩµƒSDT±Ì–≈œ¢
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_GetOutputSDTTable(U16 u16OutTSIndex, const TSInfo *pstParamTS, SDTTable_t *pstSDTTable)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16ProgramOffset = 0;
    const Output_TSProgram *pstTSProg = NULL;
    SDTProgramInfo_t *pstSDTProgramInfo = NULL;
    IndexInfoList stIndexInfoList;

    if ((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex) || (!pstSDTTable))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16OutTSIndex[%u],pstSDTTable[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutTSIndex, pstSDTTable);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    memset(pstSDTTable, 0, sizeof(SDTTable_t));

    memset(&stIndexInfoList, 0, sizeof(stIndexInfoList));
    enErrCode = TSP_GetOutTSProgIndexList(u16OutTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutTSProgIndexList Error:u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex);
        return enErrCode;
    }

    if (MAX_PROGRAMS_NUM_IN_SDT < stIndexInfoList.u16IndexListNum)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]u16OutTSIndex[%u],ProgNum[%u] out of rang\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex, stIndexInfoList.u16IndexListNum);
        return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE; 
    }

    for (i = 0; i < stIndexInfoList.u16IndexListNum; i++)
    {
        u16ProgramOffset  = stIndexInfoList.aru16IndexList[i];
        pstTSProg = &pstParamTS->pOutputProgram[u16ProgramOffset];

        if (MAX_SDT_OTHER_INFO_LEN < pstTSProg->u16SDTInfoLength)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]u16OutTSIndex[%u],Prog[%u][%u],u16SDTInfoLength[%u] out of rang\r\n",
                __FUNCTION__, __LINE__, u16OutTSIndex, i, u16ProgramOffset, pstTSProg->u16SDTInfoLength);
            return WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE;
        }

        pstSDTProgramInfo = &pstSDTTable->arstProgInfo[i];

        pstSDTProgramInfo->u16ServiceID              = pstTSProg->u16ServiceID;
        pstSDTProgramInfo->u8ServiceType             = pstTSProg->u8ServiceType;
        pstSDTProgramInfo->u8FreeCAMode              = pstTSProg->u8FreeCAMode;
        pstSDTProgramInfo->u8RunningStatus           = pstTSProg->u8RunningStatus;
        pstSDTProgramInfo->u8EITScheduleFlag         = pstTSProg->u8EITScheduleFlag;
        pstSDTProgramInfo->u8EITPresentFollowingFlag = pstTSProg->u8EITPresentFollowingFlag;
        pstSDTProgramInfo->u8ProviderNameLen         = pstTSProg->u8ServiceProviderLen;
        pstSDTProgramInfo->u8ServiceNameLen          = pstTSProg->u8ServiceNameLen;
        memcpy(pstSDTProgramInfo->aru8ProviderName, pstTSProg->aru8ServiceProvider, MAX_SDT_NAME_LENGTH);
        memcpy(pstSDTProgramInfo->aru8ServiceName, pstTSProg->aru8ServiceName, MAX_SDT_NAME_LENGTH);

        pstSDTProgramInfo->u16OtherInfoLen = pstTSProg->u16SDTInfoLength;

        if (0 < pstTSProg->u16SDTInfoLength)
        {
            memcpy(pstSDTProgramInfo->aru8OtherInfo, pstTSProg->aru8SDTInfo, pstTSProg->u16SDTInfoLength);
        }
    }

    pstSDTTable->u16OriginalNetworkId = pstParamTS->pOutputTS[u16OutTSIndex].u16OriginalNetworkID;
    pstSDTTable->u16TransportStreamID = pstParamTS->pOutputTS[u16OutTSIndex].u16TransportStreamID;
    pstSDTTable->u8VersionNum         = pstParamTS->pOutputTS[u16OutTSIndex].u8VersionNumber;
    pstSDTTable->u16ServiceNum        = stIndexInfoList.u16IndexListNum;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ConstructPAT
  Description:  ππ‘Ï“ª∏ˆ ‰≥ˆ¡˜µƒPAT±Ì
  Input:        u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_ConstructPAT(U16 u16OutTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    PATTable_t stPATInfo;
    U16 u16SectionLength = 0;

    if ((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramNum)
    {
        return WV_SUCCESS;
    }

    /*PAT*/
    memset(s_aru8ConstructBuffer, 0, sizeof(s_aru8ConstructBuffer));
    memset(&stPATInfo, 0, sizeof(stPATInfo));
    
    (void)TSP_GetOutputPATTable(u16OutTSIndex, pstParamTS, &stPATInfo);
    
    enErrCode = SIP_ConstructPAT(&stPATInfo, sizeof(s_aru8ConstructBuffer), s_aru8ConstructBuffer, &u16SectionLength);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]SIP_ConstructPAT Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    (void)SSD_AddBroadcastSI(s_aru8ConstructBuffer, u16SectionLength, u16OutTSIndex, PAT_PID);

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ConstructPMTs
  Description:  ππ‘Ï“ª∏ˆ ‰≥ˆΩ⁄ƒøµƒPMT±Ì
  Input:        u16OutProgIndex: ‰≥ˆΩ⁄ƒøÀ˜“˝
                u16OutTSIndex: ‰≥ˆΩ⁄ƒøÀ˘‘⁄¡˜µƒ¡˜À˜“˝
                pstParamTS
  Output:       pu16SectionLength:PMT±Ìµƒ≥§∂»
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_ConstructPMTForProg(U16 u16OutProgIndex, U16 u16OutTsIndex, TSInfo *pstParamTS, U16* pu16SectionLength)
{
    wvErrCode enErrCode = WV_SUCCESS;
    PMTTable_t stPMTInfo;

    memset(s_aru8ConstructBuffer, 0, sizeof(s_aru8ConstructBuffer));
    memset(&stPMTInfo, 0, sizeof(stPMTInfo));

    (void)TSP_GetOutputPMTTable(u16OutProgIndex, pstParamTS, &stPMTInfo);

    // s_stPMTInfo versionnum–ﬁ’˝£¨Õ≥“ª–ﬁ∏ƒŒ™”Î ‰≥ˆTS“ª÷¬
    stPMTInfo.u8VersionNum = pstParamTS->pOutputTS[u16OutTsIndex].u8VersionNumber;

    enErrCode = SIP_ConstructPMT(&stPMTInfo, sizeof(s_aru8ConstructBuffer), s_aru8ConstructBuffer, pu16SectionLength);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]SIP_ConstructPMT Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ConstructPMTs
  Description:  ππ‘Ï“ª∏ˆ ‰≥ˆ¡˜µƒÀ˘”–Ω⁄ƒøµƒPMT±Ì
  Input:        u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_ConstructPMTs(U16 u16OutTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;

    U16 u16SectionLength = 0;
    U16 u16PMTPID = 0;
    U16 u16OutProgIndex = 0;
    U16 i = 0;
    IndexInfoList stIndexList;

    if ((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramNum)
    {
        return WV_SUCCESS;
    }
    
    memset(&stIndexList, 0, sizeof(stIndexList));
    enErrCode = TSP_GetOutTSProgIndexList(u16OutTSIndex, pstParamTS, &stIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetOutTSProgIndexList Error:enErrCode[%08X],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode, u16OutTSIndex);
        return enErrCode;
    }

    for (i = 0; i < stIndexList.u16IndexListNum; i++)
    {
        u16OutProgIndex = stIndexList.aru16IndexList[i];
        u16PMTPID = pstParamTS->pOutputProgram[u16OutProgIndex].u16PMTPID;

        (void)TSP_ConstructPMTForProg(u16OutProgIndex, u16OutTSIndex, pstParamTS, &u16SectionLength);

        (void)SSD_AddBroadcastSI(s_aru8ConstructBuffer, u16SectionLength, u16OutTSIndex, u16PMTPID);
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ConstructCAT
  Description:  ππ‘Ï“ª∏ˆ ‰≥ˆ¡˜µƒCAT±Ì
  Input:        u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_ConstructCAT(U16 u16OutTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    CATTable_t stCATInfo;
    U16 u16SectionLength = 0;

    if ((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex))
    {
       log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]Input Error:pstParamTS[%p],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, pstParamTS, u16OutTSIndex);
       return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == pstParamTS->pOutputTS[u16OutTSIndex].u16EMMPIDNumber)
    {
        return WV_SUCCESS;
    }

    /*CAT*/
    memset(s_aru8ConstructBuffer, 0, sizeof(s_aru8ConstructBuffer));
    memset(&stCATInfo, 0, sizeof(stCATInfo));
    
    (void)TSP_GetOutputCATTable(u16OutTSIndex, pstParamTS, &stCATInfo);
    
    enErrCode = SIP_ConstructCAT(&stCATInfo, sizeof(s_aru8ConstructBuffer), s_aru8ConstructBuffer, &u16SectionLength);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]SIP_ConstructPAT Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    (void)SSD_AddBroadcastSI(s_aru8ConstructBuffer, u16SectionLength, u16OutTSIndex, CAT_PID);
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ConstructSDT
  Description:  ππ‘Ï“ª∏ˆ ‰≥ˆ¡˜µƒSDT±Ì
  Input:        u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_ConstructSDT(U16 u16OutTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    SDTTable_t stSDTInfo;
    U16 u16SectionLength = 0;

    if ((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex))
    {
       log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
           "[%s:%d]Input Error:pstParamTS[%p],u16OutTSIndex[%u]\r\n",
           __FUNCTION__, __LINE__, pstParamTS, u16OutTSIndex);
       return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == pstParamTS->pOutputTS[u16OutTSIndex].u16ProgramNum)
    {
        return WV_SUCCESS;
    }

    /*SDT*/
    memset(s_aru8ConstructBuffer, 0, sizeof(s_aru8ConstructBuffer));
    memset(&stSDTInfo, 0 ,sizeof(stSDTInfo));
    
    (void)TSP_GetOutputSDTTable(u16OutTSIndex, pstParamTS, &stSDTInfo);

    enErrCode = SIP_ConstructSDT(&stSDTInfo, sizeof(s_aru8ConstructBuffer), s_aru8ConstructBuffer, &u16SectionLength);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]SIP_ConstructPAT Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    (void)SSD_AddBroadcastSI(s_aru8ConstructBuffer, u16SectionLength, u16OutTSIndex, SDT_PID);

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ConstructSITable
  Description:  ππΩ®“ª∏ˆ ‰≥ˆ¡˜µƒSI±Ì
  Input:        u16OutTSIndex: ‰≥ˆ¡˜À˜“˝
                pstParamTS
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_ConstructSITable(U16 u16OutTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    
    if ((!pstParamTS) || (pstParamTS->u32OutputTSNumber <= u16OutTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16OutTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    /*PAT*/
    enErrCode = TSP_ConstructPAT(u16OutTSIndex, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_ConstructPAT Error:u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex);
        return enErrCode;
    }

    /*CAT*/
    enErrCode = TSP_ConstructCAT(u16OutTSIndex, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_ConstructCAT Error:u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex);
        return enErrCode;
    }

    /*SDT*/
    enErrCode = TSP_ConstructSDT(u16OutTSIndex, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_ConstructSDT Error:u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex);
        return enErrCode;
    }

    /*PMTS*/
    enErrCode = TSP_ConstructPMTs(u16OutTSIndex, pstParamTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_ConstructPMTs Error:u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex);
        return enErrCode;
    }
	
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_ConstructSITableOfCIOutput
  Description:  ππΩ®CI ‰≥ˆµƒÀ˘”–¡˜µƒSI±Ì
  Input:        pstParamTS
  Output:
  Return:       
  Others:
  Create:       Momouuwei 2017.06.22
*****************************************************************************/
wvErrCode TSP_ConstructSITableOfCIOutput(TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8CamIndex = 0;
    U16 u16OutTSIndex = INDEX_INVALID_VALUE;

    if (!pstParamTS)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    SSD_MutexLock();

    SSD_InitBroadcastSI();

    for (u8CamIndex = 0; u8CamIndex < WVCI_MAX_SLOT_NUM; u8CamIndex++)
    {
        enErrCode = TSP_GetOutputTSIndex(u8CamIndex, pstParamTS, &u16OutTSIndex);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetOutputTSIndex Error:enErrCode[%08X],u8CamIndex[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u8CamIndex);
            continue;
        }

        enErrCode = TSP_ConstructSITable(u16OutTSIndex, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_ConstructSITable Error:enErrCode[%08X],u8CamIndex[%u],u16OutTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u8CamIndex, u16OutTSIndex);

            SSD_MutexUnlock();
            return enErrCode;
        }
    }

    SSD_MutexUnlock();

    return WV_SUCCESS;
}


