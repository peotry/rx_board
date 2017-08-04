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
#ifndef __CI_DATETIME_H
#define __CI_DATETIME_H

#ifdef __cplusplus
extern "C" {
#endif

//! @file CI_DateTime.h
//! This file contains the API of the date time resource.

//! This is the resource id of the datetime resource
#define RESOURCEID_DATETIME				(0x00240041)
//! This is the datetime resource structure
extern const RESOURCE_INFO DateTime;

//! This function has to be called by the CI host whenever a TDT or TOT arrives.
//! If a module requested time updates by connecting to this resource, this function
//! forwards the time to the module.
//! @param pDateTime Pointer to the 48bit DateTime structure carried in a TDT or TOT
void DateTime_SendDateTime(const BYTE *pDateTime);

#define TAG_T_DATETIMEENQ		(0x9F8440)
#define TAG_T_DATETIME		    (0x9F8441)

#ifdef __cplusplus
}
#endif

#endif // __CI_DATETIME_H
