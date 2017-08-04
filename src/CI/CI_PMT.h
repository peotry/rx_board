// ********************************************************
//
//   Author/Copyright	Gero Kuehn / GkWare e.K.
//						Humboldtstrasse 177
//						45149 Essen
//						GERMANY 
//						Tel: +49 174 520 8026
//						Email: support@gkware.com
//						Web: http://www.gkware.com
//
//
// ********************************************************
#ifndef __CI_PMT_H
#define __CI_PMT_H

#ifdef __cplusplus
extern "C" {
#endif

//! @file CI_PMT.h
//! This file contains the API of the PMT -> CAPMT translator
//! The functions are called by the ci library internally. The host
//! can (but does not need to) use these functions directly.

//! This function converts a standard MPEG2 PMT into a EN50221 CAPMT.
//! Only the CA descriptors that contain a CA System id supported by the module
//! are included in the CAPMT. All other descriptors are discarded.
//! The function sets the list management mode to the value specified by eMode.
//! It allows the function to be used for single-service and headend/pvr CAPMT generation.
//! The output buffer should support at least 1024 bytes (maximum PMT size) for safe operation.
//! @param CAPMTBuf Output pointer for the generated CA-PMT
//! @param pBuf Input pointer (MPEG2 PMT)
//! @param dwLen Size of the input PMT
//! @param hCI Handle for the CI slot that will use/send the PMT. The generator will only
//!            include CA-descriptors supported by the module.
//! @param eMode List management mode byte.
//! @return Size of the generated CA-PMT
WORD Generate_CA_PMT(BYTE *CAPMTBuf, const BYTE *pBuf, DWORD dwLen, CI_HANDLE hCI, CAPMTLISTMANAGEMENT eMode);


//[shouquan.tang 多解支持]
WORD Generate_Multi_CA_PMT(BYTE *CAPMTBuf,BYTE **pBuf,BYTE *PMTs[1024], DWORD dwNumPMTs, DWORD dwLen,CI_HANDLE hCI);

#ifdef __cplusplus
}
#endif

#endif // __CI_PMT_H
