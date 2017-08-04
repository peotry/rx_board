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
#ifndef __CI_AUTHENTICATION_H
#define __CI_AUTHENTICATION_H

#ifdef __cplusplus
extern "C" {
#endif

//! @file CI_Authentication.h
//! This file contains the API of the optional authentication resource.

//! This is the maximum output buffer size for authentications. Please note that
//! a buffer with this size is allocated ON THE STACK for thread safety purposes.
#define MAX_AUTH_RESPONSE_SIZE		(1024)

//! This is the resource id of the authentication resource
#define RESOURCEID_AUTHENTICATION	(0x00100041)
//! This is the authentication resource structure
extern const RESOURCE_INFO Authentication;

//! This function has to be implemented by a CI host that wants to use/implement
//! CI module authentication. It is called whenever the CI modules requests an authentication.
//! The "challenge" is sent in the parameters pData and wDataLen. The response should be
//! written into pOut and pwOutLen. The Buffer at pOut has a guaranteed size of MAX_AUTH_RESPONSE_SIZE.
//! @param wProtocolId This is a 16Bit authentication protocol id which is not specified by EN50221.
//! @param pData This is the authentication data block sent by the CI module.
//! @param wDataLen This is the size of the Buffer at pData.
//! @param pOut The authentication response should be written into that buffer.
//! @param pwOutLen Write the size of the data written to pOut into this.
BOOL CIHost_Authenticate(WORD wProtocolId, const BYTE *pData, WORD wDataLen, BYTE *pOut, WORD *pwOutLen);

#define TAG_T_AUTHREQ				(0x9F8200)
#define TAG_T_AUTHRESP				(0x9F8201)


#ifdef __cplusplus
}
#endif

#endif // __CI_AUTHENTICATION_H
