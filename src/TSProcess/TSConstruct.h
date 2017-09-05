/**********************************************************************
* Copyright(c), HuiZhou WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:    TSConstruct.h
* Description: MuxProcess
*
* Version:     1.0.0
* Author:      Momouwei
* Date:        2017.02.22
*
**********************************************************************/
#ifndef _TSCONSTRUCT_H_
#define _TSCONSTRUCT_H_
#include <pthread.h>
#include <stdbool.h>

#include "appGlobal.h"
#include "ts.h"
#include "TSProcess.h"
#include "WV_CI.h"

int TSP_MutexLock(void);
int TSP_MutexUnlock(void);

/*****************************************************************************
  Function:     TSP_GetTSParamHandle
  Description:  获取整个系统的流结构信息的指针
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
TSInfo *TSP_GetTSParamHandle(void);

/*****************************************************************************
  Function:     TSP_ClearTSInfo
  Description:  清空流结构的所有信息
  Input:        pstParamTS:
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_ClearTSInfo(TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_InitTSInfo
  Description:  初始化流信息
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_InitTSInfo(TSInfo *pstParamTS, TSInfoInitNum *pstTSInfoInitNum);

/*****************************************************************************
  Function:     TSP_StoreTSConfiguration
  Description:  TSP_StoreTSConfiguration
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_StoreTSConfiguration(void);

/*****************************************************************************
  Function:     TSP_ClearTSConfiguration
  Description:  清空输入流配置
  Input:        pstParamTS
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ClearTSConfiguration(TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_ProcessClearDestSlot
  Description:  清空所有的流去对应子板的去向
  Input:        u8DestSlot
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ProcessClearDestSlot(U8 u8DestSlot);

/*****************************************************************************
  Function:     TSP_Init
  Description:  初始化整个系统的流信息
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_Init(void);

/*****************************************************************************
  Function:     TSP_ClearTSInfo
  Description:  清空所有输入流结构的所有信息
  Input:        pstParamTS:
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_ClearAllInputTSInfo(TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_InitInputTS
  Description:  初始化输入流
  Input:        u16InTSIndex:输入流索引
                u16Channel:通道号
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_InitInputTS(U16 u16InTSIndex, U16 u16Channel, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_InitInputProgram
  Description:  初始化输入节目
  Input:        u16InTSIndex:节目所属于的流的索引
                u16InProgIndex:节目索引
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_InitInputProgram(U16 u16InTSIndex, U16 u16InProgIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_InitInputPID
  Description:  初始化输入PID
  Input:        u8PIDType:PID类型
                    0:UNKNOWN, 1:PCRPID, 2:ESPID, 3:EMMPID, 4:PSECMPID, 5:ESECMPID, 6:OtherPID

                u16SrcIndex:PID所属的流/节目/PID的索引
                    PID类型为 EMMPID/OtherPID时:为流索引
                    PID类型为 PCRPID/ESPID/PSECMPID时:为节目索引
                    PID类型为 ESECMPID时:为PID索引
                    
                u16InTSIndex:PID索引
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_InitInputPID(U8 u8PIDType, U16 u16SrcIndex, U16 u16InPIDIndex, TSInfo *pstParamTS);

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
U8 TSP_GetPIDType(U16 u16PIDIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_SetStreamIDBeforeGetPIDBitrate
  Description:  设置流ID，用于获取PID码率
  Input:        u16Channel:
  Output:       
  Return:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_SetStreamIDBeforeGetPIDBitrate(U16 u16Channel);

/*****************************************************************************
  Function:     TSP_GetPIDBitRate
  Description:  获取输入PID的有效码率
  Input:        u16InTSIndex:TS索引
                u16PID:
  Output:       
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
U32 TSP_GetPIDBitRate(U16 u16PID);

/*****************************************************************************
  Function:     TSP_GetUnusedInputTSIndexList
  Description:  寻找可用的输入流的索引列表
  Input:        u32NeedNum:希望获取到的可用个数
                pstParamTS
  Output:       pstIndexList:可用的流索引的集合
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedInputTSIndexList(IndexInfoList *pstIndexList,
                                    TSInfo *pstParamTS,
                                    U32 u32NeedNum);

/*****************************************************************************
  Function:     TSP_GetUnusedInputProgIndexList
  Description:  寻找可用的输入节目的索引列表
  Input:        u32NeedNum:希望获取到的可用个数
                pstParamTS
  Output:       pstIndexList:可用的节目索引的集合
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedInputProgIndexList(IndexInfoList *pstIndexList,
                                    TSInfo *pstParamTS,
                                    U32 u32NeedNum);

/*****************************************************************************
  Function:     TSP_GetUnusedInputPIDIndexList
  Description:  寻找可用的输入PID的索引列表
  Input:        u32NeedNum:希望获取到的可用个数
                pstParamTS
  Output:       pstIndexList:可用的PID索引的集合
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedInputPIDIndexList(IndexInfoList *pstIndexList,
                                    TSInfo *pstParamTS,
                                    U32 u32NeedNum);

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
wvErrCode TSP_GetInTSProgIndexList(U16 u16InTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);

/*****************************************************************************
  Function:     TSP_GetInTSEMMPIDIndexList
  Description:  获取输入流的所有EMMPID
  Input:        u16InTSIndex:获取哪个流
                pstParamTS
  Output:       pstIndexList:所有EMMPID索引的集合
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSEMMPIDIndexList(U16 u16InTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);

/*****************************************************************************
  Function:     TSP_GetInTSOtherPIDIndexList
  Description:  获取输入流的所有OtherPID
  Input:        u16InTSIndex:获取哪个流
                pstParamTS
  Output:       pstIndexList:所有OtherPID索引的集合
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSOtherPIDIndexList(U16 u16InTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);

/*****************************************************************************
  Function:     TSP_GetInProgESPIDIndexList
  Description:  获取输入节目的所有ESPID
  Input:        u16InProgIndex:获取哪个节目
                pstParamTS
  Output:       pstIndexList:所有ESPID索引的集合
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInProgESPIDIndexList(U16 u16InProgIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);

/*****************************************************************************
  Function:     TSP_GetInProgPSECMPIDIndexList
  Description:  获取输入节目的所有PSECM
  Input:        u16InProgIndex:获取哪个节目
                pstParamTS
  Output:       pstIndexList:所有PSECM索引的集合
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInProgPSECMPIDIndexList(U16 u16InProgIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);

/*****************************************************************************
  Function:     TSP_GetInPIDESECMPIDIndexList
  Description:  获取输入PID的所有ESECM
  Input:        u16InPIDIndex:获取哪个PID
                pstParamTS
  Output:       pstIndexList:所有ESECM索引的集合
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInPIDESECMPIDIndexList(U16 u16InPIDIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);

/*****************************************************************************
  Function:     TSP_GetInTSLastEMMPIDIndex
  Description:  获取输入流的最后一个EMM
  Input:        u16InTSIndex:获取哪个流
                pstParamTS
  Output:       pu16LastIndex:最后一个EMM的索引
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       如果TS没有EMM则(*pu16LastIndex)为INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSLastEMMPIDIndex(U16 u16InTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);

/*****************************************************************************
  Function:     TSP_GetInTSLastOtherPIDIndex
  Description:  获取输入流的最后一个OtherPID
  Input:        u16InTSIndex:获取哪个流
                pstParamTS
  Output:       pu16LastIndex:最后一个OtherPID的索引
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       如果TS没有OtherPID则(*pu16LastIndex)为INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSLastOtherPIDIndex(U16 u16InTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);

/*****************************************************************************
  Function:     TSP_GetInTSLastProgIndex
  Description:  获取输入流的最后一个节目
  Input:        u16InTSIndex:获取哪个流
                pstParamTS
  Output:       pu16LastIndex:最后一个节目的索引
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       如果TS没有节目则(*pu16LastIndex)为INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSLastProgIndex(U16 u16InTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);

/*****************************************************************************
  Function:     TSP_GetInProgLastESPIDIndex
  Description:  获取输入节目的最后一个ESPID
  Input:        u16InProgIndex:获取哪个节目
                pstParamTS
  Output:       pu16LastIndex:最后一个ESPID的索引
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       如果节目没有ESPID则(*pu16LastIndex)为INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInProgLastESPIDIndex(U16 u16InProgIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);

/*****************************************************************************
  Function:     TSP_GetInProgLastPSECMPIDIndex
  Description:  获取输入节目的最后一个PSECM
  Input:        u16InProgIndex:获取哪个节目
                pstParamTS
  Output:       pu16LastIndex:最后一个PSECM的索引
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       如果节目没有PSECM则(*pu16LastIndex)为INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInProgLastPSECMPIDIndex(U16 u16InProgIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);

/*****************************************************************************
  Function:     TSP_GetInPIDLastESECMPIDIndex
  Description:  获取输入PID的最后一个ESECM
  Input:        u16InPIDIndex:获取哪个PID
                pstParamTS
  Output:       pu16LastIndex:最后一个ESECM的索引
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       如果PID没有ESECM则(*pu16LastIndex)为INDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInPIDLastESECMPIDIndex(U16 u16InPIDIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);

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
wvErrCode TSP_GetInputTSIndex(U16 u16Channel, const TSInfo *pstParamTS, U16 *pu16InTSIndex);

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
wvErrCode TSP_GetInputProgIndex(U16 u16InTSIndex, U16 u16SeviceID, const TSInfo *pstParamTS, U16 *pu16InProgIndex);

/*****************************************************************************
  Function:     TSP_AddProgToInputTS
  Description:  把一个节目添加到输入流中
  Input:        u16InTSIndex:输入流的索引
                u16ProgIndex:节目索引
                pstParamTS
  Output:       
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddProgToInputTS(U16 u16InTSIndex, U16 u16ProgIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_AddEMMPIDToInputTS
  Description:  把一个EMMPID添加到输入流中
  Input:        u16InTSIndex:输入流的索引
                u16EMMPIDIndex:EMMPID索引
                pstParamTS
  Output:       
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddEMMPIDToInputTS(U16 u16InTSIndex, U16 u16EMMPIDIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_AddOhterPIDToInputTS
  Description:  把一个OhterPID添加到输入流中
  Input:        u16InTSIndex:输入流的索引
                u16OhterPIDIndex:OhterPID索引
                pstParamTS
  Output:       
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddOhterPIDToInputTS(U16 u16InTSIndex, U16 u16OtherPIDIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_DelInputTS
  Description:  删除一个输入流
  Input:        u16InTSIndex:要删除的输入流的索引
                pstParamTS
  
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_DelInputTS(U16 u16InTSIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_DelInputTSByChannel
  Description:  删除一个输入通道的输入流
  Input:        u16Channel:通道号
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_DelInputTSByChannel(U16 u16Channel);

/*****************************************************************************
  Function:     TSP_IsLegalSlot
  Description:  判断槽位号是否是合法的
  Input:        u8Slot:要判断的槽位号
  Output:       
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IsLegalSlot(U8 u8Slot);

/*****************************************************************************
  Function:     TSP_GetProgDestInfo
  Description:  获取节目的所有去向
  Input:        u16InProgIndex:节目索引
                pstParamTS
  Output:       pstDest:节目的所有去向
  Return:       SUCESS 成功
                Others 失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetProgDestInfo(U16 u16InProgIndex, const TSInfo *pstParamTS, Dest *pstDest);

/*****************************************************************************
  Function:     TSP_GetTSDestInfo
  Description:  获取流的所有去向
  Input:        u16InTSIndex:流索引
                pstParamTS
  Output:       pstDest:流的所有去向
  Return:       SUCESS 成功
                Others 失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetTSDestInfo(U16 u16InTSIndex, const TSInfo *pstParamTS, Dest *pstDest);

/*****************************************************************************
  Function:     TSP_CheckProgDestParam
  Description:  检查要设置的节目的去向是否合法
  Input:        u16InProgIndex:输入节目的索引
                pstNewDest:网管要设置的去向信息
                pstParamTS
  Output:       
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_CheckProgDestParam(U16 u16InProgIndex, Dest *pstNewDest, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_CheckTSDestParam
  Description:  检查要设置的流的去向是否合法
  Input:        u16InTSIndex:输入流的索引
                pstNewDest:网管要设置的去向信息
                pstParamTS
  Output:       
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_CheckTSDestParam(U16 u16InTSIndex, Dest *pstNewDest, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_GetDestIndex
  Description:  获取对应去向的索引
  Input:        u8Slot:去向的槽位号
                aru8DestSlot:给定的DestSlot数组
  Output:       pu8DestIndex:去向索引
  Return:       WV_SUCCESS or ErrorCode
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetDestIndex(U8 u8Slot, U8 aru8DestSlot[], U8 *pu8DestIndex);

/*****************************************************************************
  Function:     TSP_ProcessProgDest
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
wvErrCode TSP_ProcessProgDest(U16 u16InProgIndex, Dest *pstNewDest, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_ProcessTSDest
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
wvErrCode TSP_ProcessTSDest(U16 u16InTSIndex, Dest *pstNewDest, TSInfo *pstParamTS);

BOOL TSP_IsProgSet2BeDescrambled(U16 u16InProgIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_ProcessDescramble
  Description:  设置解扰节目
  Input:        arstDescrambleProgInfo:要设置解扰的节目的解扰参数信息链表
                u16DescrambleProgNum:要设置解扰的节目个数
                pstParamTS
  Output:       
  Return:       success or error code
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ProcessDescramble(DescrambleProgInfo arstDescrambleProgInfoList[], U16 u16DescrambleProgNum, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_GetDescrambledProgList
  Description:  获取输入流中设置了解扰的所有节目索引
  Input:        u16InTSIndex:输入流索引
                pstParamTS
  Output:       pstIndexList:输入流中设置了解扰的所有节目索引链表
  Return:       success or error code
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetDescrambledProgList(U16 u16InTSIndex, TSInfo *pstParamTS, IndexInfoList *pstIndexList);

U16 TSP_InputChannel2StreamID(U16 u16Channel);
U16 TSP_CICamIndex2StreamID(U16 u16CamIndex);

/*****************************************************************************
  Function:     TSP_UpdateInputLUTInfo
  Description:  更新输入LUT的IP信息(当输出板的IP或者MAC地址改变时)
  Input:        u8DestSlot:输出板槽号
                u32IP:IP地址
                aru8MAC:MAC地址
  Output:       
  Return:       WV_SUCCESS or error code
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_UpdateInputLUTInfo(U8 u8DestSlot, U32 u32IP, U8 aru8MAC[6]);

/*****************************************************************************
  Function:     TSP_SetInputLUT
  Description:  设置输入LUT
  Input:        pstParamTS
  Output:       
  Return:       SUCESS 获取成功
                Others 获取失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SetInputLUT(TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_SetBypassAndMUXFlag
  Description:  设置包头直通/复用标志
  Input:        pstParamTS
  Output:       
  Return:       WV_SUCCESS 设置成功
                Others 设置失败原因
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SetBypassAndMUXFlag(TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_DebugPrintLUTInfo
  Description:  打印输入LUT
  Input:        u8LUTIndex:LUT索引
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_DebugPrintLUTInfo(U8 u8LUTIndex);

/*****************************************************************************
  Function:     TSP_RestoreTSMUXInfo
  Description:  恢复复用信息
  Input:        
  Output:       
  Return:       WV_SUCCESS or error code
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_RestoreTSMUXInfo(void);

/*****************************************************************************
  Function:     TSP_IsClearTS
  Description:  判断流是否被清空了，若被清空后要重新搜台
  Input:        u8Channel
  Output:       
  Return:       TRUE or FALSE
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IsClearTS(U8 u8Channel);

/*****************************************************************************
  Function:     TSP_SetClearTSFlag
  Description:  设置清空流标志
  Input:        u8Channel
                bFlag
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_SetClearTSFlag(U8 u8Channel, BOOL bFlag);

/*****************************************************************************
  Function:     TSP_GetTSBitrateInfo
  Description:  获取流的码率信息
  Input:        u8Channel
                pstTSBitrateInfo
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetTSBitrateInfo(U16 u16Channel, TSBitrateInfo *pstTSBitrateInfo);

/*****************************************************************************
  Function:     TSP_ClearPIDCC
  Description:  TSP_ClearPIDCC
  Input:        
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_ClearPIDCC(void);

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
                            TSInfo *pstParamTS);


void TSP_ClearAllOutputTSInfo(TSInfo *pstParamTS);
void TSP_InitOutputTS(U16 u16OutTSIndex, U16 u16Channel, TSInfo *pstParamTS);
wvErrCode TSP_GetUnusedOutputTSIndexList(IndexInfoList *pstIndexList, TSInfo *pstParamTS, U32 u32NeedNum);
wvErrCode TSP_GetUnusedOutputProgIndexList(IndexInfoList *pstIndexList, TSInfo *pstParamTS, U32 u32NeedNum);
wvErrCode TSP_GetUnusedOutputPIDIndexList(IndexInfoList *pstIndexList, TSInfo *pstParamTS, U32 u32NeedNum);
wvErrCode TSP_GetOutTSProgIndexList(U16 u16OutTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);
wvErrCode TSP_GetOutTSEMMPIDIndexList(U16 u16OutTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);
wvErrCode TSP_GetOutTSOtherPIDIndexList(U16 u16OutTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);
wvErrCode TSP_GetOutProgESPIDIndexList(U16 u16OutProgIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);
wvErrCode TSP_GetOutProgPSECMPIDIndexList(U16 u16OutProgIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);
wvErrCode TSP_GetOutPIDESECMPIDIndexList(U16 u16OutPIDIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);
wvErrCode TSP_GetOutProgLastESPIDIndex(U16 u16OutProgIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);
wvErrCode TSP_GetOutProgLastPSECMPIDIndex(U16 u16OutProgIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);
wvErrCode TSP_GetOutPIDLastESECMPIDIndex(U16 u16OutPIDIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);
wvErrCode TSP_GetOutTSLastEMMPIDIndex(U16 u16OutTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);
wvErrCode TSP_GetOutTSLastOtherPIDIndex(U16 u16OutTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);
wvErrCode TSP_GetOutTSLastProgIndex(U16 u16OutTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);
wvErrCode TSP_GetOutputTSIndex(U16 u16Channel, const TSInfo *pstParamTS, U16 *pu16OutTSIndex);
wvErrCode TSP_GetOutputProgIndexBySrcProgIndex(U16 u16SrcProgIndex, U16 u16OutTSIndex, const TSInfo *pstParamTS, U16 *pu16OutProgIndex);
wvErrCode TSP_GetServiceIDUseflagOfOutputTS(U16 u16OutTSIndex, TSInfo *pstParamTS, U32 aru32ServiceIDUseFlag[2048]);
wvErrCode TSP_GetPIDUseFlagOfOutputTS(U16 u16OutTSIndex, TSInfo *pstParamTS, PIDUseInfo arstPIDUseInfo[MAX_PID_NUMBER + 1]);
wvErrCode TSP_ConstructNullOutputTS(U16 u16Channel, TSInfo *pstParamTS, U16 *pu16OutTSIndex);
wvErrCode TSP_CheckServiceIDConflict(U16 u16InProgIndex, U16 u16OutTSIndex, TSInfo *pstParamTS);
wvErrCode TSP_CheckPIDConflict(U8 u8PIDType, U16 u16Index, TSInfo *pstParamTS, PIDUseInfo arstOutputTSPIDUseInfo[MAX_PID_NUMBER + 1]);
wvErrCode TSP_CheckPIDConflictOfProg(U16 u16InProgIndex, U16 u16OutTSIndex, TSInfo *pstParamTS);
wvErrCode TSP_GetPrevOutputPIDIndex(U16 u16OutPIDIndex, TSInfo *pstParamTS, U16 *pu16PrevPIDIndex);
wvErrCode TSP_GetPrevOutputProgIndex(U16 u16OutProgIndex, TSInfo *pstParamTS, U16 *pu16PrevProgIndex);
wvErrCode TSP_DelOutputPID(U16 u16OutPIDIndex, TSInfo *pstParamTS);
wvErrCode TSP_DelOutputProg(U16 u16OutProgIndex, TSInfo *pstParamTS);
wvErrCode TSP_AddPID2OutputPID(U16 u16InPIDIndex, U16 u16OutPIDIndex, TSInfo *pstParamTS);
wvErrCode TSP_AddPID2OutputProg(U8 u8PIDType, U16 u16InPIDIndex, U16 u16OutProgIndex, TSInfo *pstParamTS);
wvErrCode TSP_AddPID2OutputTS(U8 u8PIDType, U16 u16InPIDIndex, U16 u16OutTSIndex, TSInfo *pstParamTS);
wvErrCode TSP_AddProg2OutputTS(U16 u16InProgIndex, U16 u16OutTSIndex, TSInfo *pstParamTS);
wvErrCode TSP_ConstructOutputTSByAddProg(U16 u16InProgIndex, U16 u16OutTSIndex, TSInfo *pstParamTS);
wvErrCode TSP_ConstructOutputTSByAddPID(U8 u8PIDType, U16 u16InPIDIndex, U16 u16OutTSIndex, TSInfo *pstParamTS);
void PrintOutputTS(U16 u16OutTSIndex, TSInfo *pstParamTS);
wvErrCode TSP_SetCIOutputLUT(TSInfo *pstParamTS);
void TSP_BypassAllTS2CIOutput(void);
void TSP_DebugPrintCIOutputLUTInfo(void);

wvErrCode TSP_ConstructSITableOfCIOutput(TSInfo *pstParamTS);

bool TSP_IsFirstScanTS(U8 u8Channel);



#endif

