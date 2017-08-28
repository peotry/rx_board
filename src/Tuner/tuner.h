/*
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName tuner.h
* Description : tuner moudle
* Author    : huada.huang
* Modified  :
* Reviewer  :
* Date      : 2017-02-22
* Record    :
*
*/
#ifndef __TUNER_H__
#define __TUNER_H__

#include "Avl6381Manager.h"

#define DTV4U_TUNER_CHAN_NUM AVL6381_PORT_NUM

#define FILE_NAME_TUNER_TYPE_CONFIG "/wellav_data/tuner_type.config"
#define FILE_NAME_TUNER_C_CONFIG "/wellav_data/tuner_c.config"
#define FILE_NAME_TUNER_S2_CONFIG "/wellav_data/tuner_s2.config"
#define FILE_NAME_TUNER_DTMB_CONFIG "/wellav_data/tuner_dtmb.config"

typedef struct
{
    U8             SourceType;
    BOOL           bLock;
    U8             u8FECCodeRates;
    U8             u8IsMultiStream;
    U32            totalbitrate;
    U32            effecbitrate;
    S32            s32C_N;
    S32            s32RFLevel; 
    U32            u32TunerFrequency;
    U32            u32FrequencyOffset;
    U32            uBerCnt;
    U32            uBerExponent;
    float          packetErrorRate;
    BYTE           ucReserved[4];

}Tuner_SrcStatus_t;

typedef enum
{
    UNLOCK = 0,
    LOCK = 1
}LOCK_enum;

typedef enum
{
    TUNER_C = 0,
    TUNER_S2,
    TUNER_DTMB,
    
    TUNER_TYPE_CNT,
    TUENR_TYPE_UNKNOW = 0xFF
}TUNER_TYPE_enum;

U32 Tuner_getStatus(U8 u8PortIndex, Tuner_SrcStatus_t *pstTunerStatus);
TUNER_TYPE_enum Tuner_GetTunerType();

void Tuner_SetTunerSelectType(TUNER_TYPE_enum tunerType);
TUNER_TYPE_enum Tuner_GetSelectTunerType(void);

S32 Tuner_StoreParam(void);
S32 Tuner_RestoreParam(void);

void Tuner_init();
U32 Tuner_Default(void);
BOOL Tuner_IslockParamChange(U8 u8PortIndex);
BOOL Tuner_IsLockFreqChange(U8 u8PortIndex);
void Tuner_SetLockFreqChangeFlag(U8 u8PortIndex);
void Tuner_ClearLockFreqChangeFlag(U8 u8PortIndex);
BOOL Tuner_isChannelLock(U8 u8PortIndex);

TUNER_TYPE_enum Tuner_checkType(void);
U32 Tuner_reset(U8 u8PortIndex);
U32 Tuner_initial(U8 u8PortIndex);
U32 Tuner_lockSignal(U8 u8PortIndex);
U32 Tuner_process(void);

#endif
