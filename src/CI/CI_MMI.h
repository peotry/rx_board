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
#ifndef __CI_MMI_H
#define __CI_MMI_H

#ifdef __cplusplus
extern "C" {
#endif

//! @file CI_MMI.h
//! This file contains the API of the mmi resource

//! This is the resource id of the mmi resource
#define RESOURCEID_MMI					(0x00400041)


//! This is the mmi resource structure
extern const RESOURCE_INFO MMI;



// MMI
//! This function tells the CI library to close the MMI session that might be open
//! on the specified slot.
void CI_MMI_Close(BYTE bSlotIndex);



//! This function tells the CI library to open an MMI session on the specified slot.
//! This message is sent on the application info session, but logically, it belongs
//! into the MMI API and feature set.
void CI_MMI_EnterMenu(BYTE bSlotIndex);



//! This function sends a MMI response for a PIN query screen.
//! The host has to call this function in response to a CI_MMI_REQUEST_INPUT mmi message.
//! @param bSlotIndex Zero based index of the CI slot.
//! @param pBuffer Pointer to a buffer that contains the ASCII characters that
//!                the user has entered. If this parameter is NULL, the CI libary
//!                sends a "cancel input" message instead of the "input answer".
//! @param nLength Size of the buffer that pBuffer points to.
void CI_MMI_AnswerInput(BYTE bSlotIndex, const BYTE *pBuffer, BYTE nLength);


//! This function sends a MMI response for a list or menu screen.
//! @param bSlotIndex Zero based index of the CI slot.
//! @param bChoice The choice can be zero (exit key) or 1-(number of menu entries) for valid responses.
void CI_MMI_AnswerMenu(BYTE bSlotIndex, BYTE bChoice);


#define TAG_T_CLOSEMMI			(0x9f8800)
#define TAG_T_DISPLAYCONTROL	(0x9f8801)
#define TAG_T_DISPLAYREPLY		(0x9f8802)
#define TAG_T_TEXTLAST			(0x9f8803)
#define TAG_T_TEXTMORE			(0x9f8804)
#define TAG_T_KEYPADCONTROL		(0x9f8805)
#define TAG_T_KEYPRESS			(0x9f8806)
#define TAG_T_ENQ				(0x9f8807)
#define TAG_T_ANSWER			(0x9f8808)
#define TAG_T_MENULAST			(0x9f8809)
#define TAG_T_MENUMORE			(0x9f880A)
#define TAG_T_MENUANSWER		(0x9f880B)
#define TAG_T_LISTLAST			(0x9f880C)
#define TAG_T_LISTMORE			(0x9f880D)

#define TAG_T_SUBTITLESEGMENTLAST	(0x9f880E)
#define TAG_T_SUBTITLESEGMENTMORE	(0x9f880F)
#define TAG_T_DISPLAYMESSAGE		(0x9f8810)
#define TAG_T_SCENEENDMARK			(0x9f8811)
#define TAG_T_SCENEDONE				(0x9f8812)
#define TAG_T_SCENECONTROL			(0x9f8813)
#define TAG_T_SUBTITLEDOWNLOADLAST	(0x9f8814)
#define TAG_T_SUBTITLEDOWNLOADMORE	(0x9f8815)
#define TAG_T_FLUSHDOWNLOAD			(0x9f8816)
#define TAG_T_DOWNLOADREPLY			(0x9f8817)


// TS 101 699 extensions
#define TAG_T_REQUESTSTART		(0x9F8000)
#define TAG_T_REQUESTSTART_ACK	(0x9F8001)
#define TAG_T_FILE_REQUEST		(0x9F8002)
#define TAG_T_FILE_ACK			(0x9F8003)
#define TAG_T_APPABORT_REQUEST	(0x9F8004)
#define TAG_T_APPABORT_ACK		(0x9F8005)


#ifdef __cplusplus
}
#endif

#endif // __CI_MMI_H
