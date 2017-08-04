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
#ifndef __CI_COPYPROTECTION_DTG_H
#define __CI_COPYPROTECTION_DTG_H

#ifdef __cplusplus
extern "C" {
#endif

//! @file CI_CopyProtection.h
//! This file contains the API for the DTG copy protection default implementation.
//! If the DTG copy protection default implementation is used, the CIHost_GetCopyProtectionStatus and
//! CIHost_CopyProtectionCommand functions in CI_CopyProtection.h do not need to be implemented.
//! The current implementation is based on the D-Book 5.0

//! CIHost_ActivateCopyProtection turns on the receiver builtin copy protection.
//! Active copy protection means: Content can not be recorded. 
//! D-Book: "This specification does not address the method used by the receiver to make its output non-recordable" 
//! @return TRUE on success, false otherwise
BOOL CIHost_ActivateCopyProtection(void);

//! CIHost_ActivateCopyProtection turns off the receiver builtin copy protection.
//! @return TRUE on success, false otherwise
BOOL CIHost_DeactivateCopyProtection(void);

#ifdef __cplusplus
}
#endif

#endif // __CI_COPYPROTECTION_DTG_H
