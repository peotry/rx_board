/*****************************************************************************
* Copyright (c) 2012,WellAV Technologies Ltd. All rights reserved. 
* File Name£º       Avl6211Manager.h
* Description:              
* History:      
                        
<Author>            <Date>              <Version >          <Reason>
Jiahong.he        2010/8/4            1.0                   Create
*****************************************************************************/

#ifndef AVL6211_MANAGER
#define AVL6211_MANAGER

//#include "stddefs.h"
#include "appGlobal.h"

#define  AVL_SUCCESS  0
#define  AVL_ERROR   -1

#define  AVL_UNLOCK   1

typedef enum
{
    AVL_6211_7903,
    AVL_6211_7A03,
    AVL_6211_Unknow = 0xFF
}AVL6211_ChipType_t;

typedef enum
{
    AVL_PORT_0 = 0,
    AVL_PORT_1,
    AVL_PORT_2,
    AVL_PORT_3,

    AVL_PORT_NUM
}Avl6211_PortIndex_t;


typedef struct
{
    DWORD       dwFrequency;
    DWORD       dwSymbolRate;
    DWORD       dwLNBFrequencyKHz;
} Avl6211_SearchParams_t;



int  AVLTunerInit(Avl6211_PortIndex_t ePortIndex);
int  AVLTunerLockSignal(Avl6211_PortIndex_t ePortIndex, Avl6211_SearchParams_t *pSearchParams);
int AVLTunerGetTunFeqOffset(Avl6211_PortIndex_t ePortIndex);
U32 AVLTunerGetLockFrequency(Avl6211_PortIndex_t ePortIndex);
int  AVLTunerGetFECCodeRate(Avl6211_PortIndex_t ePortIndex, U8 *pCodeRate);
int  AVLTunerGetPER(Avl6211_PortIndex_t ePortIndex, float *fpuPER);
int  AVLTunerGetBER_PER(Avl6211_PortIndex_t ePortIndex, U32 *pu32BerCnt, U32 *pu32Expo);
int  AVLTunerGetSignalQuality(Avl6211_PortIndex_t ePortIndex, int  *pnSignalQuality);
int  AVLTunerGetSignalStrength(Avl6211_PortIndex_t ePortIndex, int  *pnSignalStrength);

//int  AVLTunerGetChipId(Avl6211_PortIndex_t ePortIndex);
BOOL AVLTunerGetTunerStatus(Avl6211_PortIndex_t ePortIndex);
int  Av16211_SetLNB22K(int PortIndex, unsigned char  ucLNB22KEnable);

#endif
