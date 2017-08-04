/*
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName tuner_s2.h
* Description : tuner moudle
* Author    : huada.huang
* Modified  :
* Reviewer  :
* Date      : 2017-03-29
* Record    :
*
*/
#include "appGlobal.h"
#include "Tuner.h"
#include "Avl6211Manager.h"

typedef enum
{
    AVL6211 = 0,

    S2_CHIP_TYPE_UNKNOW = 0xFF
}TUNER_S2_CHIP_TYPE_enum;


typedef enum
{
    LNB_VOLTAGE_OFF,
    LNB_VOLTAGE_18,
    LNB_VOLTAGE_13
}LNB_Voltage_t;


typedef struct
{
    U32             u32FrequencyKHz;
    U32             u32SymbolRate;
    U32             u32LNBFrequencyKHz;
    U32             u32LNBVoltage;
    U32             u32LNB22k;
} Tune_S2_Params_t;




U32 WAVTUN_TUNEFREQ_ABS(U32 A, U32 B);

void Tuner_s2_ParamInit();
U32 Tuner_s2_SetParamAfterInit(void);
U32 Tuner_s2_StoreParam(void);
U32 Tuner_s2_RestoreParam(void);
U32 Tuner_s2_Default(void);

U32 Tuner_s2_initial(U8 u8PortIndex);


int Tuner_s2_SetLNB22KONOFF(U32 u32Port, U8 LnbTone);

int  Tuner_s2_LNBSetting(U32 u32Port, LNB_Voltage_t eLNBVoltage);

U32 Tuner_s2_lockSignal(U8 u8PortIndex);


BOOL Tuner_IsS2LockParamChange(Avl6211_PortIndex_t ePortIndex);

U32 Tuner_s2_getStatus(Avl6211_PortIndex_t ePortIndex, Tuner_SrcStatus_t *status);

U32 Tuner_s2_CheckLockStatus(Avl6211_PortIndex_t ePortIndex, BOOL *b_isLock);

wvErrCode Tuner_s2GetParameter(Avl6211_PortIndex_t ePortIndex, Tune_S2_Params_t *s2_params);

wvErrCode Tuner_s2SetParameter(Avl6211_PortIndex_t ePortIndex, Tune_S2_Params_t *s2_params);






