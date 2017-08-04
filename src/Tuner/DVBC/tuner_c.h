/*
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName tuner_c.h
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
    U32             u32SymbolRate;
    U32             u32QamMode;  // 0: 6M,  1: 7M, 2: 8M
    U32             u32Mode;
} Tuner_C_Params_t;

typedef enum
{
    AVL6381 = 0,

    CHIP_TYPE_UNKNOW = 0xFF
}TUNER_C_CHIP_TYPE_enum;

void Tuner_c_ParamInit();
U32 Tuner_c_StoreParam(void);
U32 Tuner_c_RestoreParam(void);
U32 Tuner_c_Default(void);
U32 Tuner_c_initial(U8 u8PortIndex);
U32 Tuner_c_lockSignal(U8 u8PortIndex);
U32 Tuner_c_getStatus(Avl6381_PortIndex_t ePortIndex, Tuner_SrcStatus_t *status);
U32 Tuner_c_CheckLockStatus(Avl6381_PortIndex_t ePortIndex, BOOL *b_lockstatus);

BOOL Tuner_c_IsLockParamChange(U8 ePortIndex);

U32 Tuner_c_GetLockParam(U8 u8PortIndex, Tuner_C_Params_t *CParam);

U32 Tuner_c_SetLockParam(U8 u8PortIndex, Tuner_C_Params_t *CParam);

#endif
