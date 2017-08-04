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
#ifndef WEB_FUNCION_H
#define WEB_FUNCION_H

#include "wv_user.h"
#include "WV_log.h"
#include "parameters.h"
#include "wv_err.h"
#include "TSProcess.h"
#include "TSConstruct.h"
#include "Tuner.h"
#include "tuner_c.h"
#include "tuner_s2.h"
#include "WV_CI.h"

#define DEFAULT_PATH      "/mnt"
#define DEPTH_SEARCH      3
#define DOWN_LOAD_LOG     WELLAV_APP"web/download/log.tar"
#define DOWN_LOAD_LOG_DIR WELLAV_APP"web/download/"
#define DOWN_LOAD_LOG_REL "download/log.tar"
#define DOWN_LOAD_CONFIG  WELLAV_APP"web/download/config.tar.gz"


typedef struct
{
    U32 port;
    U32 channel;
    BOOL isLock;
    U32 totalbitrate;
    U32 effectivebitrate;
    S32 s32RFLevel;
    U32 uBerCnt;
    U32 uBerExponent;
    S32 s32C_N;
    U32 frequencytune;
    
}Tuner_stat;



wvErrCode web_CheckUser(const U8 * pu8UserName , const U8 * pu8PassWord);
wvErrCode web_RegistUser(const U8 * pu8UserName, const U8 * pu8PassWord);
wvErrCode web_ModifyPasswd(const U8 * pu8UserName, const U8 * pu8PassWord);
wvErrCode web_DelUser(const U8 * pu8UserName);
wvErrCode web_ShowUser(ST_USER_INFO* pStUserArry, U32 u32MaxNum, U32* pU32UserNum );
U8 *      web_GetErrCodeString(wvErrCode enErrCode);
void web_REG_Read_fortest(U32 u32Address, U32 *pu32Data);
void web_REG_Write_fortest(U32 u32Address, U32 u32Data);

int web_TSPMutexLock(void);
int web_TSPMutexUnlock(void);
TSInfo *web_GetTSPramaHandle(void);
wvErrCode web_ClearTSConfiguration(void);
wvErrCode web_GetInProgIndex(U16 u16InTSIndex, U16 u16SeviceID, TSInfo *pstParamTS, U16 *pu16InProgIndex);
wvErrCode web_GetInTSIndex(U16 u16Channel, TSInfo *pstParamTS, U16 *pu16InTSIndex);
wvErrCode web_GetInTSProgIndexList(U16 u16InTSIndex, TSInfo *pstParamTS, IndexInfoList * pstIndexList);
wvErrCode web_GetInProgESPIDIndexList(U16 u16InProgIndex, TSInfo *pstParamTS, IndexInfoList *pstIndexList);
wvErrCode web_GetInProgPSECMPIDIndexList(U16 u16InProgIndex, TSInfo *pstParamTS, IndexInfoList *pstIndexList);
wvErrCode web_GetInPIDESECMPIDIndexList(U16 u16InPIDIndex, const TSInfo *pstParamTS, IndexInfoList *pstIndexList);
U8 web_GetPIDType(U16 u16PIDIndex, TSInfo *pstParamTS);
void web_SetStreamIDBeforeGetPIDBitrate(U16 u16Channel);
U32 web_GetPIDBitRate(U16 u16PID);
wvErrCode web_GetTSDestInfo(U16 u16InTSIndex, TSInfo *pstParamTS, Dest *pstDest);
wvErrCode web_GetProgDestInfo(U16 u16InProgIndex, TSInfo *pstParamTS, Dest *pstDest);
wvErrCode web_ProcessTSDest(U16 u16InTSIndex, Dest *pstNewDest, TSInfo *pstParamTS);
wvErrCode web_ProcessProgDest(U16 u16InProgIndex, Dest *pstNewDest, TSInfo *pstParamTS);
wvErrCode web_ProcessDescramble(DescrambleProgInfo arstDescrambleProgInfo[], U16 u16DescrambleProgNum, TSInfo *pstParamTS);
void web_DebugPrintLUTInfo(U8 u8LUTIndex);
void web_DebugPrintCIOutputLUTInfo(void);


/*tuner api*/
wvErrCode web_GetCTunerParam(U8 u8Port, Tuner_C_Params_t *lockParam );

wvErrCode web_SetCTunerParam(U8 u8Port, Tuner_C_Params_t *lockParam );

wvErrCode web_GetDTMBTunerParam(U8 u8Port, Tuner_C_Params_t *lockParam );

wvErrCode web_SetDTMBTunerParam(U8 u8Port, Tuner_C_Params_t *lockParam );

wvErrCode web_GetS2TunerParam(U8 u8Port, Tune_S2_Params_t *S2Param );

wvErrCode web_SetS2TunerParam(U8 u8Port, Tune_S2_Params_t *S2Param );

wvErrCode web_GetTunerStatus(U8 u8Port, Tuner_stat *tunerStat);

wvErrCode Web_getTunerType(TUNER_TYPE_enum *tunerType);

wvErrCode Web_ExportLogs(U8 * exportURI);

//Resource
BOOL web_IsSubBoardPlugin(U8 u8Slot);
BOOL web_IsOutputBoard(U8 u8BoardType);
U8 web_GetBoardType(U8 u8Slot);
wvErrCode web_GetBoardDescriptor(U8 u8Slot, U8 *pu8Descroptor);
wvErrCode web_GetSelfBoardDescriptor(U8 *pu8Descroptor);
U8 web_GetBoardPortNum(U8 u8Slot);
U16 web_GetInputChannelNum(U8 u8Slot);
U16 web_GetOutputChannelNum(U8 u8Slot);
U8 web_GetOutputChannelStatus(U8 u8Slot, U8 u8Channel);

wvErrCode web_Reboot(void);
wvErrCode web_Default(void);
wvErrCode web_ClearAllTS(void);
wvErrCode web_GetTSBitrateInfo(U16 u16Channel, TSBitrateInfo *pstTSBitrateInfo);
wvErrCode web_ClearPIDCc(void);
void web_GetPIDTypeDesc(U8 u8Type, U8 u8StreamType, U8 *u8TypeName);
void web_GetStreamTypeDesc(U8 u8StreamType, U8 *u8StreamTypeDesc);

wvErrCode web_GetCIStatus(U8 u8CAMIndex, WAVCI_Status_4_web_t *pstCIStatus, TSInfo *pstParamTS);
BOOL web_IsProgSet2BeDescrambled(U16 u16InProgIndex, TSInfo *pstParamTS);

#endif


