
#ifndef __RDA5818_H__
#define __RDA5818_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "Avl_dvbsx.h"


#define TUNER_ADDRESS   0x18

void RDA5815Initial( AVL_uint32 u32Port, struct AVL_DVBSx_Chip * pAVLChip);
                      
unsigned char RDA5815Set( AVL_uint32 u32Port, unsigned long fPLL, unsigned long fSym , struct AVL_DVBSx_Chip * pAVLChip);


unsigned  int RDA5815Lock(AVL_uint32 u32Port, unsigned int u32Frequency, unsigned int u32SymbolRate, struct AVL_DVBSx_Chip * pAVLChip);

#if 0
unsigned char RDA5815_LockStatus(unsigned char tuner_addr);

unsigned char RDA5815_Sleep(unsigned char tuner_addr);

unsigned char RDA5815_Wakeup(unsigned char tuner_addr);

unsigned char RDA5815_RunningStatus(unsigned char tuner_addr);
#endif


#ifdef __cplusplus
}
#endif

#endif

