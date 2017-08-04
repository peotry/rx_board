/*****************************************************************************
* Copyright (c) 2012,WellAV Technologies Ltd. All rights reserved. 
* File Name£º       Avl6211Manager.h
* Description:              
* History:      
                        
<Author>            <Date>              <Version >          <Reason>
Jiahong.he        2010/8/4            1.0                   Create
*****************************************************************************/

#ifndef AVL6311_MANAGER
#define AVL6311_MANAGER

#include "AppGlobal.h"

#define  AVL_SUCCESS  0
#define  AVL_ERROR   -1

#define  AVL_UNLOCK   1

typedef enum
{
    AVL6381_PORT_0 = 0,
    AVL6381_PORT_1,
    AVL6381_PORT_2,
    AVL6381_PORT_3,

    AVL6381_PORT_NUM
}Avl6381_PortIndex_t;


typedef struct
{
    U32             dwFrequencyKHz;
    U32             dwSymbolRate;
    U8              ucBandWidthMode;      // 6: 6M,  7: 7M, 8: 8M
    U8              ucMode;
    U8              ucPLPMode;
    U8              ucPLPNum;
} Avl6381_SearchParams_t;


int  AVL6381TunerInit(Avl6381_PortIndex_t ePortIndex, unsigned char ucMode);
int  AVL6381TunerLockSignal(Avl6381_PortIndex_t ePortIndex, Avl6381_SearchParams_t *pSearchParams);
int  AVL6381TunerGetTunFeqOffset(Avl6381_PortIndex_t ePortIndex);
U32  AVL6381TunerGetLockFrequency(Avl6381_PortIndex_t ePortIndex);
int  AVL6381TunerGetFECCodeRate(Avl6381_PortIndex_t ePortIndex, U8 *pCodeRate);
int  AVL6381TunerGetBER(Avl6381_PortIndex_t ePortIndex, U32 *pu32BerCnt, U32 *pu32Expo);
int  AVL6381TunerGetPER(Avl6381_PortIndex_t ePortIndex, float *fpuPER);
int  AVL6381TunerGetSignalQuality(Avl6381_PortIndex_t ePortIndex, int  *pnSignalQuality);
int  AVL6381TunerGetSignalStrength(Avl6381_PortIndex_t ePortIndex, int  *pnSignalStrength);
BOOL AVL6381TunerGetLockStatus(Avl6381_PortIndex_t ePortIndex);

#endif
