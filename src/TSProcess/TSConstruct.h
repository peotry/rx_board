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
  Description:  ��ȡ����ϵͳ�����ṹ��Ϣ��ָ��
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
TSInfo *TSP_GetTSParamHandle(void);

/*****************************************************************************
  Function:     TSP_ClearTSInfo
  Description:  ������ṹ��������Ϣ
  Input:        pstParamTS:
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_ClearTSInfo(TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_InitTSInfo
  Description:  ��ʼ������Ϣ
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
  Description:  �������������
  Input:        pstParamTS
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ClearTSConfiguration(TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_ProcessClearDestSlot
  Description:  ������е���ȥ��Ӧ�Ӱ��ȥ��
  Input:        u8DestSlot
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ProcessClearDestSlot(U8 u8DestSlot);

/*****************************************************************************
  Function:     TSP_Init
  Description:  ��ʼ������ϵͳ������Ϣ
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_Init(void);

/*****************************************************************************
  Function:     TSP_ClearTSInfo
  Description:  ��������������ṹ��������Ϣ
  Input:        pstParamTS:
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_ClearAllInputTSInfo(TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_InitInputTS
  Description:  ��ʼ��������
  Input:        u16InTSIndex:����������
                u16Channel:ͨ����
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_InitInputTS(U16 u16InTSIndex, U16 u16Channel, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_InitInputProgram
  Description:  ��ʼ�������Ŀ
  Input:        u16InTSIndex:��Ŀ�����ڵ���������
                u16InProgIndex:��Ŀ����
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_InitInputProgram(U16 u16InTSIndex, U16 u16InProgIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_InitInputPID
  Description:  ��ʼ������PID
  Input:        u8PIDType:PID����
                    0:UNKNOWN, 1:PCRPID, 2:ESPID, 3:EMMPID, 4:PSECMPID, 5:ESECMPID, 6:OtherPID

                u16SrcIndex:PID��������/��Ŀ/PID������
                    PID����Ϊ EMMPID/OtherPIDʱ:Ϊ������
                    PID����Ϊ PCRPID/ESPID/PSECMPIDʱ:Ϊ��Ŀ����
                    PID����Ϊ ESECMPIDʱ:ΪPID����
                    
                u16InTSIndex:PID����
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_InitInputPID(U8 u8PIDType, U16 u16SrcIndex, U16 u16InPIDIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     web_GetPIDType
  Description:  ��ȡ����PID������(��Ƶ����Ƶ)
  Input:        u16PIDIndex:PID����
                pstParamTS
  Output:       
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
U8 TSP_GetPIDType(U16 u16PIDIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_SetStreamIDBeforeGetPIDBitrate
  Description:  ������ID�����ڻ�ȡPID����
  Input:        u16Channel:
  Output:       
  Return:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_SetStreamIDBeforeGetPIDBitrate(U16 u16Channel);

/*****************************************************************************
  Function:     TSP_GetPIDBitRate
  Description:  ��ȡ����PID����Ч����
  Input:        u16InTSIndex:TS����
                u16PID:
  Output:       
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
U32 TSP_GetPIDBitRate(U16 u16PID);

/*****************************************************************************
  Function:     TSP_GetUnusedInputTSIndexList
  Description:  Ѱ�ҿ��õ��������������б�
  Input:        u32NeedNum:ϣ����ȡ���Ŀ��ø���
                pstParamTS
  Output:       pstIndexList:���õ��������ļ���
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedInputTSIndexList(IndexInfoList *pstIndexList,
                                    TSInfo *pstParamTS,
                                    U32 u32NeedNum);

/*****************************************************************************
  Function:     TSP_GetUnusedInputProgIndexList
  Description:  Ѱ�ҿ��õ������Ŀ�������б�
  Input:        u32NeedNum:ϣ����ȡ���Ŀ��ø���
                pstParamTS
  Output:       pstIndexList:���õĽ�Ŀ�����ļ���
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedInputProgIndexList(IndexInfoList *pstIndexList,
                                    TSInfo *pstParamTS,
                                    U32 u32NeedNum);

/*****************************************************************************
  Function:     TSP_GetUnusedInputPIDIndexList
  Description:  Ѱ�ҿ��õ�����PID�������б�
  Input:        u32NeedNum:ϣ����ȡ���Ŀ��ø���
                pstParamTS
  Output:       pstIndexList:���õ�PID�����ļ���
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetUnusedInputPIDIndexList(IndexInfoList *pstIndexList,
                                    TSInfo *pstParamTS,
                                    U32 u32NeedNum);

/*****************************************************************************
  Function:     TSP_GetInTSProgIndexList
  Description:  ��ȡ�����������н�Ŀ
  Input:        u16InTSIndex:��ȡ�ĸ���
                pstParamTS
  Output:       pstIndexList:���н�Ŀ�����ļ���
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSProgIndexList(U16 u16InTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);

/*****************************************************************************
  Function:     TSP_GetInTSEMMPIDIndexList
  Description:  ��ȡ������������EMMPID
  Input:        u16InTSIndex:��ȡ�ĸ���
                pstParamTS
  Output:       pstIndexList:����EMMPID�����ļ���
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSEMMPIDIndexList(U16 u16InTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);

/*****************************************************************************
  Function:     TSP_GetInTSOtherPIDIndexList
  Description:  ��ȡ������������OtherPID
  Input:        u16InTSIndex:��ȡ�ĸ���
                pstParamTS
  Output:       pstIndexList:����OtherPID�����ļ���
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSOtherPIDIndexList(U16 u16InTSIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);

/*****************************************************************************
  Function:     TSP_GetInProgESPIDIndexList
  Description:  ��ȡ�����Ŀ������ESPID
  Input:        u16InProgIndex:��ȡ�ĸ���Ŀ
                pstParamTS
  Output:       pstIndexList:����ESPID�����ļ���
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInProgESPIDIndexList(U16 u16InProgIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);

/*****************************************************************************
  Function:     TSP_GetInProgPSECMPIDIndexList
  Description:  ��ȡ�����Ŀ������PSECM
  Input:        u16InProgIndex:��ȡ�ĸ���Ŀ
                pstParamTS
  Output:       pstIndexList:����PSECM�����ļ���
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInProgPSECMPIDIndexList(U16 u16InProgIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);

/*****************************************************************************
  Function:     TSP_GetInPIDESECMPIDIndexList
  Description:  ��ȡ����PID������ESECM
  Input:        u16InPIDIndex:��ȡ�ĸ�PID
                pstParamTS
  Output:       pstIndexList:����ESECM�����ļ���
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInPIDESECMPIDIndexList(U16 u16InPIDIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);

/*****************************************************************************
  Function:     TSP_GetInTSLastEMMPIDIndex
  Description:  ��ȡ�����������һ��EMM
  Input:        u16InTSIndex:��ȡ�ĸ���
                pstParamTS
  Output:       pu16LastIndex:���һ��EMM������
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       ���TSû��EMM��(*pu16LastIndex)ΪINDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSLastEMMPIDIndex(U16 u16InTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);

/*****************************************************************************
  Function:     TSP_GetInTSLastOtherPIDIndex
  Description:  ��ȡ�����������һ��OtherPID
  Input:        u16InTSIndex:��ȡ�ĸ���
                pstParamTS
  Output:       pu16LastIndex:���һ��OtherPID������
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       ���TSû��OtherPID��(*pu16LastIndex)ΪINDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSLastOtherPIDIndex(U16 u16InTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);

/*****************************************************************************
  Function:     TSP_GetInTSLastProgIndex
  Description:  ��ȡ�����������һ����Ŀ
  Input:        u16InTSIndex:��ȡ�ĸ���
                pstParamTS
  Output:       pu16LastIndex:���һ����Ŀ������
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       ���TSû�н�Ŀ��(*pu16LastIndex)ΪINDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInTSLastProgIndex(U16 u16InTSIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);

/*****************************************************************************
  Function:     TSP_GetInProgLastESPIDIndex
  Description:  ��ȡ�����Ŀ�����һ��ESPID
  Input:        u16InProgIndex:��ȡ�ĸ���Ŀ
                pstParamTS
  Output:       pu16LastIndex:���һ��ESPID������
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       �����Ŀû��ESPID��(*pu16LastIndex)ΪINDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInProgLastESPIDIndex(U16 u16InProgIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);

/*****************************************************************************
  Function:     TSP_GetInProgLastPSECMPIDIndex
  Description:  ��ȡ�����Ŀ�����һ��PSECM
  Input:        u16InProgIndex:��ȡ�ĸ���Ŀ
                pstParamTS
  Output:       pu16LastIndex:���һ��PSECM������
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       �����Ŀû��PSECM��(*pu16LastIndex)ΪINDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInProgLastPSECMPIDIndex(U16 u16InProgIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);

/*****************************************************************************
  Function:     TSP_GetInPIDLastESECMPIDIndex
  Description:  ��ȡ����PID�����һ��ESECM
  Input:        u16InPIDIndex:��ȡ�ĸ�PID
                pstParamTS
  Output:       pu16LastIndex:���һ��ESECM������
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       ���PIDû��ESECM��(*pu16LastIndex)ΪINDEX_INVALID_VALUE
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInPIDLastESECMPIDIndex(U16 u16InPIDIndex, const TSInfo *pstParamTS, U16 *pu16LastIndex);

/*****************************************************************************
  Function:     TSP_GetInputTSIndex
  Description:  ��ȡ������������
  Input:        u16Channel:��������Channel
                pstParamTS
  Output:       pu16InTSIndex:��ȡ����������
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInputTSIndex(U16 u16Channel, const TSInfo *pstParamTS, U16 *pu16InTSIndex);

/*****************************************************************************
  Function:     TSP_GetInputProgIndex
  Description:  ��ȡ�����Ŀ������
  Input:        u16InTSIndex:�����Ŀ���ڵ���������
                u16SeviceID:Ҫ��ȡ�Ľ�Ŀ��ServiceID
                pstParamTS
  Output:       pu16InProgIndex:��ȡ���Ľ�Ŀ����
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetInputProgIndex(U16 u16InTSIndex, U16 u16SeviceID, const TSInfo *pstParamTS, U16 *pu16InProgIndex);

/*****************************************************************************
  Function:     TSP_AddProgToInputTS
  Description:  ��һ����Ŀ��ӵ���������
  Input:        u16InTSIndex:������������
                u16ProgIndex:��Ŀ����
                pstParamTS
  Output:       
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddProgToInputTS(U16 u16InTSIndex, U16 u16ProgIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_AddEMMPIDToInputTS
  Description:  ��һ��EMMPID��ӵ���������
  Input:        u16InTSIndex:������������
                u16EMMPIDIndex:EMMPID����
                pstParamTS
  Output:       
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddEMMPIDToInputTS(U16 u16InTSIndex, U16 u16EMMPIDIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_AddOhterPIDToInputTS
  Description:  ��һ��OhterPID��ӵ���������
  Input:        u16InTSIndex:������������
                u16OhterPIDIndex:OhterPID����
                pstParamTS
  Output:       
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_AddOhterPIDToInputTS(U16 u16InTSIndex, U16 u16OtherPIDIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_DelInputTS
  Description:  ɾ��һ��������
  Input:        u16InTSIndex:Ҫɾ����������������
                pstParamTS
  
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_DelInputTS(U16 u16InTSIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_DelInputTSByChannel
  Description:  ɾ��һ������ͨ����������
  Input:        u16Channel:ͨ����
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_DelInputTSByChannel(U16 u16Channel);

/*****************************************************************************
  Function:     TSP_IsLegalSlot
  Description:  �жϲ�λ���Ƿ��ǺϷ���
  Input:        u8Slot:Ҫ�жϵĲ�λ��
  Output:       
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IsLegalSlot(U8 u8Slot);

/*****************************************************************************
  Function:     TSP_GetProgDestInfo
  Description:  ��ȡ��Ŀ������ȥ��
  Input:        u16InProgIndex:��Ŀ����
                pstParamTS
  Output:       pstDest:��Ŀ������ȥ��
  Return:       SUCESS �ɹ�
                Others ʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetProgDestInfo(U16 u16InProgIndex, const TSInfo *pstParamTS, Dest *pstDest);

/*****************************************************************************
  Function:     TSP_GetTSDestInfo
  Description:  ��ȡ��������ȥ��
  Input:        u16InTSIndex:������
                pstParamTS
  Output:       pstDest:��������ȥ��
  Return:       SUCESS �ɹ�
                Others ʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetTSDestInfo(U16 u16InTSIndex, const TSInfo *pstParamTS, Dest *pstDest);

/*****************************************************************************
  Function:     TSP_CheckProgDestParam
  Description:  ���Ҫ���õĽ�Ŀ��ȥ���Ƿ�Ϸ�
  Input:        u16InProgIndex:�����Ŀ������
                pstNewDest:����Ҫ���õ�ȥ����Ϣ
                pstParamTS
  Output:       
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_CheckProgDestParam(U16 u16InProgIndex, Dest *pstNewDest, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_CheckTSDestParam
  Description:  ���Ҫ���õ�����ȥ���Ƿ�Ϸ�
  Input:        u16InTSIndex:������������
                pstNewDest:����Ҫ���õ�ȥ����Ϣ
                pstParamTS
  Output:       
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_CheckTSDestParam(U16 u16InTSIndex, Dest *pstNewDest, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_GetDestIndex
  Description:  ��ȡ��Ӧȥ�������
  Input:        u8Slot:ȥ��Ĳ�λ��
                aru8DestSlot:������DestSlot����
  Output:       pu8DestIndex:ȥ������
  Return:       WV_SUCCESS or ErrorCode
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetDestIndex(U8 u8Slot, U8 aru8DestSlot[], U8 *pu8DestIndex);

/*****************************************************************************
  Function:     TSP_ProcessProgDest
  Description:  ����һ����Ŀ��ȥ��
  Input:        u16InProgIndex:�����Ŀ������
                pstNewDest:����Ҫ���õ�ȥ����Ϣ
                pstParamTS
  Output:       
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ProcessProgDest(U16 u16InProgIndex, Dest *pstNewDest, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_ProcessTSDest
  Description:  ����һ������ȥ��
  Input:        u16InTSIndex:������������
                pstNewDest:����Ҫ���õ�ȥ����Ϣ
                pstParamTS
  Output:       
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ProcessTSDest(U16 u16InTSIndex, Dest *pstNewDest, TSInfo *pstParamTS);

BOOL TSP_IsProgSet2BeDescrambled(U16 u16InProgIndex, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_ProcessDescramble
  Description:  ���ý��Ž�Ŀ
  Input:        arstDescrambleProgInfo:Ҫ���ý��ŵĽ�Ŀ�Ľ��Ų�����Ϣ����
                u16DescrambleProgNum:Ҫ���ý��ŵĽ�Ŀ����
                pstParamTS
  Output:       
  Return:       success or error code
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_ProcessDescramble(DescrambleProgInfo arstDescrambleProgInfoList[], U16 u16DescrambleProgNum, TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_GetDescrambledProgList
  Description:  ��ȡ�������������˽��ŵ����н�Ŀ����
  Input:        u16InTSIndex:����������
                pstParamTS
  Output:       pstIndexList:�������������˽��ŵ����н�Ŀ��������
  Return:       success or error code
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_GetDescrambledProgList(U16 u16InTSIndex, TSInfo *pstParamTS, IndexInfoList *pstIndexList);

U16 TSP_InputChannel2StreamID(U16 u16Channel);
U16 TSP_CICamIndex2StreamID(U16 u16CamIndex);

/*****************************************************************************
  Function:     TSP_UpdateInputLUTInfo
  Description:  ��������LUT��IP��Ϣ(��������IP����MAC��ַ�ı�ʱ)
  Input:        u8DestSlot:�����ۺ�
                u32IP:IP��ַ
                aru8MAC:MAC��ַ
  Output:       
  Return:       WV_SUCCESS or error code
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_UpdateInputLUTInfo(U8 u8DestSlot, U32 u32IP, U8 aru8MAC[6]);

/*****************************************************************************
  Function:     TSP_SetInputLUT
  Description:  ��������LUT
  Input:        pstParamTS
  Output:       
  Return:       SUCESS ��ȡ�ɹ�
                Others ��ȡʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SetInputLUT(TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_SetBypassAndMUXFlag
  Description:  ���ð�ͷֱͨ/���ñ�־
  Input:        pstParamTS
  Output:       
  Return:       WV_SUCCESS ���óɹ�
                Others ����ʧ��ԭ��
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_SetBypassAndMUXFlag(TSInfo *pstParamTS);

/*****************************************************************************
  Function:     TSP_DebugPrintLUTInfo
  Description:  ��ӡ����LUT
  Input:        u8LUTIndex:LUT����
  Output:       
  Return:       
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
void TSP_DebugPrintLUTInfo(U8 u8LUTIndex);

/*****************************************************************************
  Function:     TSP_RestoreTSMUXInfo
  Description:  �ָ�������Ϣ
  Input:        
  Output:       
  Return:       WV_SUCCESS or error code
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_RestoreTSMUXInfo(void);

/*****************************************************************************
  Function:     TSP_IsClearTS
  Description:  �ж����Ƿ�����ˣ�������պ�Ҫ������̨
  Input:        u8Channel
  Output:       
  Return:       TRUE or FALSE
  Others:       
  Create:       Momouwei 2017.02.23
*****************************************************************************/
BOOL TSP_IsClearTS(U8 u8Channel);

/*****************************************************************************
  Function:     TSP_SetClearTSFlag
  Description:  �����������־
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
  Description:  ��ȡ����������Ϣ
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

