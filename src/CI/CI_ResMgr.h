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
#ifndef __CI_RESMGR_H
#define __CI_RESMGR_H

#ifdef __cplusplus
extern "C" {
#endif

//! @file CI_ResMgr.h
//! This file contains the API of the resource manager resource
//! This resource does not require any interaction with the CI host.


//! This is the resource id of the resource manager (version 1)
#define RESOURCEID_RESOURCEMANAGER		(0x00010041)	
//! This is the resource manager resource structure (version 1)
extern const RESOURCE_INFO ResMgr;

#ifdef ENABLE_CI_PROFILE2
// This is currently unsupported test code
//! This is the resource id of the resource manager (version 2)
#define RESOURCEID_RESOURCEMANAGERV2	(0x00010042)
//! This is the resource manager resource structure (version 2)
extern const RESOURCE_INFO ResMgrV2;
#endif

// Version 1 & 2 Tags
#define TAG_T_PROFILE_ENQ			(0x9F8010)
#define TAG_T_PROFILE				(0x9F8011)
#define TAG_T_PROFILE_CHANGE		(0x9F8012)

// Version 2 only
#define TAG_T_MODULEID_SEND			(0x9F8013)
#define TAG_T_MODULEID_COMMAND		(0x9F8014)

#ifdef __cplusplus
}
#endif

#endif // __CI_RESMGR_H

