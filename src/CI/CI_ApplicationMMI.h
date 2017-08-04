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
#ifndef __CI_APPLICATIONMMI_H
#define __CI_APPLICATIONMMI_H

#ifdef __cplusplus
extern "C" {
#endif

//! @file CI_ApplicationMMI.h
//! @brief This file contains the API of the optional application MMI resource.

//! This is the resource id of the application MMI resource
#define RESOURCEID_APPLICATIONMMI			(0x00410041)
//! This is the application MMI resource structure
extern const RESOURCE_INFO ApplicationMMI;

typedef enum {
	CI_AM_REQSTARTACK_OK		= 0x01, //!< RequestStartAck code: OK
	CI_AM_REQSTARTACK_WRONGAPI  = 0x02, //!< RequestStartAck code: Application domain not supported
	CI_AM_REQSTARTACK_APIBUSY	= 0x03	//!< RequestStartAck code: Application domain currently not available 
} CI_AM_REQSTARTACK_CODE;

typedef enum {
	CI_AM_FILEREQTYPE_FILE		= 0x00, //!< A File is beeing requested
	CI_AM_FILEREQTYPE_DATA		= 0x01  //!< A private data pipe is beeing used
} CI_AM_FILEREQTYPE_CODE;

//! This function has to be implemented by a CI+ host. The reception of an application MMI RequestStart
//! is signaled to the host through a call to this function.
//! @param bSlotIndex Zero based index of the CI slot.
//! @param pAppDomainID This is a pointer to the application domain ID found in the RequestStart.
//! @param pInitialObject This is a pointer to the initial object found in the RequestStart.
//! @return One of the CI_AM_REQSTARTACK_CODE as apporiate to the host.
CI_AM_REQSTARTACK_CODE CIHost_AppMMIrequestStart(BYTE bSlotIndex, const BYTE *pAppDomainID, const BYTE *pInitialObject);

//! This function has to be implemented by a CI+ host. A modules FileRequestAcknowledge with its parameters will
//! be announced to the host with this function call.
//! @param bSlotIndex Zero based index of the CI slot.
//! @param bFileOK This is the value of the "FileOK" flag
//! @param eRequestType This is the "RequestType" code as stated in the CI+ spec.
//! @param pszFileName This ist the file name or NULL depending on RequestType.
//! @param pData This is a pointer to file data or private data depending on RequestType.
//! @param dwDataLen This is the length of the data in pData.
void CIHost_AppMMIfileRequestAck(BYTE bSlotIndex, BOOL bFileOK, CI_AM_FILEREQTYPE_CODE eRequestType, const char *pszFileName, const BYTE *pData, DWORD dwDataLen);

//! This function has to be implemented by a CI+ host. Both AppAbortRequest and AppAbortAck will be
//! signaled to the host through this function call. In case of AppAbortRequest fRequest is set to TRUE else to FALSE.
//! For an AppAbortRequest the host may return TRUE to cause an immediate AppAbortAck being sent to the module.
//! To set the AbortAck code the host may use the pAbortCode buffer and set its lenght through wAbortCodeLen.
//! At the most the host may use wAbortCodeLen from function entry for its AbortAck code.
//! @param bSlotIndex Zero based index of the CI slot.
//! @param fRequest If TRUE an AppAbortRequest is being signaled else an AppAbortAck.
//! @param pAbortCode This is a pointer to the Abort code (can be used for immediate AppAbortAck).
//! @param wAbortCodeLen This is a pointer to the lenght of the Abort code (set the length of an Abort code here).
//! @return TRUE and fRequest == TRUE causes the CI stack to send an immediate AppAbortAck to the module.
BOOL CIHost_AppMMIabort(BYTE bSlotIndex, BOOL fRequest, BYTE *pAbortCode, WORD *wAbortCodeLen);

//! This function has to be called by the CI+ host to prompt the CI+ stack to forward a FileRequest
//! message to the module.
//! @param bSlotIndex Zero based index of the CI slot.
//! @param pBuff This is a pointer to a buffer carrying either a file name or "private data" (depending on the FileRequestType)
//! @param wBuffLen This is the length of the above buffer.
//! @param eRequestType Specifies if pBuff is referring a file name or "private data".
//! @return Returns TRUE if the FileRequest was successfully forwarded to the module.
BOOL CIAPPMMI_FileRequest(BYTE bSlotIndex, const BYTE *pBuff, WORD wBuffLen, CI_AM_FILEREQTYPE_CODE eRequestType);

//! This function can be used by the CI+ host to prompt the CI+ stack to forward an AppAbortRequest or AppAbortAck
//! message to the module.
//! @param bSlotIndex Zero based index of the CI slot.
//! @param pBuff This is a pointer to a buffer carrying the AbortCode.
//! @param wBuffLen This is the length of the AbortCode.
//! @param fRequest If TRUE an AppAbortRequest else an AppAbortAck will be sent to the module.
//! @return Returns TRUE if the AppAbortRequest/AppAbortAck was successfully forwarded to the module.
BOOL CIAPPMMI_Abort(BYTE bSlotIndex, const BYTE *pBuff, WORD wBuffLen, BYTE fRequest);


#define TAG_T_REQUESTSTART				(0x9F8000)
#define TAG_T_REQUESTSTARTACK			(0x9F8001)
#define TAG_T_FILEREQ					(0x9F8002)
#define TAG_T_FILEACK					(0x9F8003)
#define TAG_T_APPABORTREQ				(0x9F8004)
#define TAG_T_APPABORTACK				(0x9F8005)

#define MAX_AM_FILE_NAME				256



#ifdef __cplusplus
}
#endif

#endif // __CI_APPLICATIONMMI_H
