/*
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName tuner_dtmb.h
* Description : tuner moudle
* Author    : huada.huang
* Modified  :
* Reviewer  :
* Date      : 2017-02-25
* Record    :
*
*/

#ifndef _TUNER_C_H_
#define _TUNER_C_H_

#include "Avl6381Manager.h"
#include "tuner.h"

typedef struct
{
    U32             u32FrequencyKHz;
    U32             u32SymbolRate;      // 0: 6M,  1: 7M, 2: 8M
    U32             u32QamMode;
    U32             u32Mode;
} Tuner_DTMB_Params_t;

typedef enum
{
    AVL6381 = 0,

    CHIP_TYPE_UNKNOW = 0xFF
}TUNER_DTMB_CHIP_TYPE_enum;

void Tuner_dtmb_ParamInit();
U32 Tuner_dtmb_StoreParam(void);
U32 Tuner_dtmb_RestoreParam(void);
U32 Tuner_dtmb_Default(void);
U32 Tuner_dtmb_initial(U8 u8PortIndex);
U32 Tuner_dtmb_lockSignal(U8 u8PortIndex);
U32 Tuner_dtmb_getStatus(Avl6381_PortIndex_t ePortIndex, Tuner_SrcStatus_t *status);
U32 Tuner_dtmb_CheckLockStatus(Avl6381_PortIndex_t ePortIndex, BOOL *b_lockstatus);

BOOL Tuner_dtmb_IsLockParamChange(U8 ePortIndex);

U32 Tuner_dtmb_GetLockParam(U8 u8PortIndex, Tuner_C_Params_t *CParam);

U32 Tuner_dtmb_SetLockParam(U8 u8PortIndex, Tuner_C_Params_t *CParam);

#endif
