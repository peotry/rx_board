// ********************************************************
//
//   Author/Copyright   Gero Kuehn / GkWare e.K.
//                      Humboldtstrasse 177
//                      45149 Essen
//                      GERMANY 
//                      Tel: +49 174 520 8026
//                      Email: support@gkware.com
//                      Web: http://www.gkware.com
//
//
// ********************************************************
#ifndef __CI_APPINFO_H
#define __CI_APPINFO_H

#ifdef __cplusplus
extern "C" {
#endif

//! @file CI_AppInfo.h
//! This file contains the API of the date application info resource.
//! This resource does not require any interaction with the CI host.

//! This is the resource id of the application info resource (version 1)
#define RESOURCEID_APPINFO              (0x00020041)
//! This is the resource id of the application info resource (version 2)
#define RESOURCEID_APPINFOV2            (0x00020042)
//! This is the resource id of the application info resource (version 3)
#define RESOURCEID_APPINFOV3            (0x00020043)

//! This is the application info resource structure (version 1)
extern const RESOURCE_INFO AppInfo;
//! This is the application info resource structure (version 2)
extern const RESOURCE_INFO AppInfoV2;
//! This is the application info resource structure (version 3)
extern const RESOURCE_INFO AppInfoV3;

#ifdef __cplusplus
}
#endif

#endif // __CI_APPINFO_H
