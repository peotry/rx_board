/*
 *           Copyright 2007-2009 Availink, Inc.
 *
 *  This software contains Availink proprietary information and
 *  its use and disclosure are restricted solely to the terms in
 *  the corresponding written license agreement. It shall not be 
 *  disclosed to anyone other than valid licensees without
 *  written permission of Availink, Inc.
 *
 */


#ifndef AVL6211_LOCKSIGNAL_H
#define AVL6211_LOCKSIGNAL_H

#include "avl_dvbsx.h"

#ifdef AVL_CPLUSPLUS
extern "C" {
#endif



#define Tuner_Slave_Address     0xC0    //The Tuner slave address. It is the write address of the tuner device. i.e. it is 8 bits address and the last bit is the R/W flag which is zero. Although this data structure support 10 bits I2C address. The AVL2108 chip not supports it. 
#define Tuner_I2Cbus_Clock      200     //The clock speed of the tuner dedicated I2C bus, in a unit of kHz.
#define Tuner_LPF               340     //The LPF of the tuner,in a unit of 100kHz.
#define Diseqc_Tone_Frequency   22      // The Diseqc bus speed in the unit of kHz. Normally, it should be 22kHz. 

struct Signal_Level
{
    AVL_uint16 SignalLevel;
    AVL_int16 SignalDBM;
};

AVL_DVBSx_ErrorCode A6211_GetBitErrorRate(struct AVL_DVBSx_Chip *pAVLChip, AVL_uint32 *uiBER);
AVL_DVBSx_ErrorCode A6211_GetSignalQuality(struct AVL_DVBSx_Chip *pAVLChip, AVL_int32 *SignalQuality);
AVL_DVBSx_ErrorCode A6211_GetSignalStrength(struct AVL_DVBSx_Chip *pAVLChip, AVL_int32 *SignalStrength);
AVL_DVBSx_ErrorCode A6211_Initialize(struct AVL_DVBSx_Chip *pAVLChip,struct AVL_Tuner *pTuner);
AVL_DVBSx_ErrorCode A6211_LockSignal(AVL_uint32 TunerFrequency,
                                 AVL_uint32 SignalSymbolRate,
                                 AVL_uint32 *pNewFrequency,
                                 struct AVL_DVBSx_Chip * pA2108Chip,
                                 struct AVL_Tuner * pTuner);
#ifdef AVL_CPLUSPLUS
}
#endif

#endif


