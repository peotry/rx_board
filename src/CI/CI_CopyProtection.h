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
#ifndef __CI_COPYPROTECTION_H
#define __CI_COPYPROTECTION_H

#ifdef __cplusplus
extern "C" {
#endif

//! @file CI_CopyProtection.h
//! This file contains the API of the optional copy protection resource.

//! This is the maximum output buffer size for authentications. Please note that
//! a buffer with this size is allocated ON THE STACK for thread safety purposes.
#define MAX_CP_RESPONSE_SIZE		(1024)

//! This is the resource id of the authentication resource (instance zero)
//! Theoretically, more than one copy protection resource can exist in a host
#define RESOURCEID_COPYPROTECTION	(0x00041001)
//! This is the authentication resource structure
extern const RESOURCE_INFO CopyProtection;


typedef enum {
	CI_CP_STATUS_INACTIVE		= 0x01, //!< copy protection system inactive
	CI_CP_STATUS_ACTIVE			= 0x02, //!< copy protection system active
	CI_CP_STATUS_ID_MISMATCH	= 0xFF	//!< the requested ID does not match the one implemented by the host
} CI_CP_STATUS;

//! This function has to be implemented by a CI host that wants to use/implement
//! CI module copy protection. It is called whenever the CI modules asks for the status of a copy protection system.
//! @param pdwCPId This is an input and output parameter. On entry, pdwCPId points to the copy protection id requested by
//!               the module. The host should always write the supported copy protection id into this DWORD before returning.
//! @return The status of the requested copy protection system or CI_CP_STATUS_ID_MISMATCH if the system is unsupported.
CI_CP_STATUS CIHost_GetCopyProtectionStatus(DWORD *pdwCPId);


//! This function has to be implemented by a CI host that wants to use/implement
//! CI module authentication. It is called whenever the CI modules send a copy protection command.
//! The command is passed in the parameters pData and wDataLen. The response should be
//! written into pOut and pwOutLen. The Buffer at pOut has a guaranteed size of MAX_CP_RESPONSE_SIZE.
//! @param dwCPId This is a 24Bit copy protection id.
//! @param pData This is the copy protection command data block sent by the CI module.
//! @param wDataLen This is the size of the Buffer at pData.
//! @param pOut The copy protection response should be written into that buffer.
//! @param pwOutLen Write the size of the data written to pOut into this.
//! @return TRUE if the command was processed correctly or FALSE if the copy protection id is unspported
BOOL CIHost_CopyProtectionCommand(DWORD dwCPId, const BYTE *pData, WORD wDataLen, BYTE *pOut, WORD *pwOutLen);



#define TAG_T_CPQUERY				(0x9F8000)
#define TAG_T_CPREPLY				(0x9F8001)
#define TAG_T_CPCOMMAND				(0x9F8002)
#define TAG_T_CPRESPONSE			(0x9F8003)


#ifdef __cplusplus
}
#endif

#endif // __CI_COPYPROTECTION_H
