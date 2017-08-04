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
#ifndef __CI_PMTDEC_H
#define __CI_PMTDEC_H

//! @file CI_PMTDec.h
//! This file contains the API of the MPEG2 PMT parser
//! The functions are called by the ci library internally. The host
//! can (but does not need to) use these functions directly.

#ifdef __cplusplus
extern "C" {
#endif

//! Return the index-th descriptor with the Tag==nTag in the root-loop of the PMT identified by pBuf.
//! Returns NULL if there is no such descriptor or any descriptor if nTag is -1
const BYTE *CI_PMT_GetDescriptor(const BYTE *pBuf, const short nTag, const BYTE index);
//! Return the index-th descriptor with the Tag==nTag in the bESEntry-th ES loop of the PMT identified by pBuf.
//! Returns NULL if there is no such descriptor or any descriptor if nTag is -1
const BYTE *CI_PMT_GetESDescriptor(const BYTE *pSection, const BYTE bESEntry, const short nDescriptorTag, const BYTE bCount);
//! Decode the bESEntry-th ES loop of the PMT identified by pBuf.
//! Returns FALSE if there is no such ES entry.
BOOL		CI_PMT_GetESEntry(const BYTE *pSection, const BYTE bESEntry, BYTE *pStreamType, WORD *pPID);

const BYTE *CI_ProInfo_GetDescriptor(const BYTE *pBuf, int nLoopLen, const short nTag, const BYTE index);
const BYTE *CI_ESInfo_GetESDescriptor(const BYTE *pDesBuf, U16 u16DesLength, const short nDescriptorTag, const BYTE bCount);
#ifdef __cplusplus
}
#endif

#endif // __CI_PMTDEC_H
