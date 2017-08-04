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
#ifndef __CI_HOSTCONTROL_H
#define __CI_HOSTCONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

//! @file CI_Hostcontrol.h
//! This file contains the API of the host control resource.

//! This is the resource id of the host control resource
#define RESOURCEID_HOSTCONTROL			(0x00200041)
//! This is the host control resource structure
extern const RESOURCE_INFO HostControl;

//! This function is called by the CI library when a module wants to
//! tune to another service, identified by the IDs
//! given in the parameters. In response to this function call, the
//! Box should tune to the requested service and stay there
//! until CIHost_HC_Closed() is called. Such a request is frequently used
//! for module software updates.
//! @param wNID Network ID
//! @param wONID Original Network ID
//! @param wTSID Transport Stream ID
//! @param wSID Service ID
void CIHost_HC_Tune(WORD wNID, WORD wONID, WORD wTSID, WORD wSID);

//! This function is called by the CI library when a module wants to
//! replace the data on one pid by data from another pid.
//! @param bReference A Reference which uniquely identifies one replace request.
//! @param wReplacedPid The PID which gets replaced.
//! @param wReplacementPid The PID with the content that replaces wReplacedPid.
void CIHost_HC_Replace(BYTE bReference, WORD wReplacedPid, WORD wReplacementPid);

//! This function is called by the CI library when a module wants to
//! stop a pid replacement initiated by CIHost_HC_Replace().
//! @param bReference A Reference previously passed to CIHost_HC_Replace().
void CIHost_HC_ClearReplace(BYTE bReference);

//! This function is called, when the Host control resource is no longer
//! used by a CI module.
void CIHost_HC_Closed(void);

//! This function can be called by the CI host, when it wants to end an ongoing HostControl operation.
//! After calling this function, the Host can resume normal operation (tuning to whatever service
//! or frequency it wants) whithout waiting for an acknowledgment from the CI module.
//! Usually, the CI modules close the Host control session after the "ask release" message is
//! sent. It is not necessary to call this function in response to CIHost_HC_Closed.
void CI_HostControl_Ask4HostRelease(void);


#define TAG_T_TUNE					(0x9F8400)
#define TAG_T_REPLACE				(0x9F8401)
#define TAG_T_CLEARREPLACE		    (0x9F8402)
#define TAG_T_ASKRELEASE		    (0x9F8403)


#ifdef __cplusplus
}
#endif

#endif // __CI_DATETIME_H
