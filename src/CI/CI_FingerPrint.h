/*****************************************************************************
* Copyright (c) 2012,WellAV Technologies Ltd. All rights reserved. 
* File Name£º   	CI_FingerPrint.h
* Description:		
* History: 		
                		
<Author>    		<Date>      		<Version >  		<Reason>
Hawayi.Liu		    2012/11/22	    	1.0			        Create
*****************************************************************************/

#ifndef __CI_FINGER_PRINT_H
#define __CI_FINGER_PRINT_H

#ifdef __cplusplus
extern "C" {
#endif

//! @file CI_FingerPrint.h
//! @brief This file contains the API of the Finger Print resource.

//! This is the resource id of the Finger Print resource
#define FP_PRIVATE_APP_ID		(0x70726976744650)//-"privatFP"

#define RESOURCEID_SAS			(0x961001)

#define TAG_SAS_CONNECT_RQST	(0x9F9A00) // Host --> Module
#define TAG_SAS_CONNECT_CNF		(0x9F9A01) // Host <-- Module
/*
#define TAG_SAS_DATA_RQST		(0x9F9A02) // Host <--> Module
#define TAG_SAS_DATA_AV			(0x9F9A03) // Host <--> Module
#define TAG_SAS_DATA_CNF		(0x9F9A04) // Host <--> Module
#define TAG_SAS_SERVER_QUERY	(0x9F9A05) // Host <--> Module
#define TAG_SAS_SERVER_REPLY	(0x9F9A06) // Host <--> Module
*/
#define TAG_SAS_ASYNC_MSG		(0x9F9A07) // Host <--> Module


#define CMD_AUTHENTICATION_REQUEST		(0x80) // Host <-- Module
#define CMD_AUTHENTICATION_RESPONSE		(0x81) // Host --> Module
#define CMD_FINGERPRINT_SEND			(0x82) // Host <-- Module
#define CMD_FINGERPRINT_RESPONSE		(0x83) // Host --> Module

#define FP_STRING_MAX_LENGTH	256
#define FP_LOGO_MAX_LENGTH		65536
#define FP_SERVICE_MAX_NUM		32
#define FP_DAT_TYPE_MAX_NUM		32

typedef enum _FP_DataType
{
	DTID_FP_AUTH = 0x80, //
	DTID_FP_SERVICE_ID,
	DTID_FP_STRING,
	DTID_FP_LOGO,
	DTID_FP_PIXEL,
	DTID_FP_START_TIME,
	DTID_FP_DURATION,
	DTID_FP_LOCATION,
	DTID_FP_STATUS,

	DTID_FP_MAX
}FP_DataType;

typedef enum _FP_Status
{
	FPST_UNKNOWN			= 0x01, //
	FPST_AUTH_SUCCESS		= 0x11, //
	FPST_AUTH_FAIL			= 0x12, //
	FPST_NO_FP				= 0x21, //
	FPST_FP_SUCCESS			= 0x22, //
	FPST_FP_LENGTH_ERROR	= 0x23, //

	FPST_MAX
}FP_Status;

typedef enum _FP_Location
{
	FPLC_CENTER,	
	FPLC_UPPER_CENTER,	//N
	FPLC_UPPER_RIGHT,	//NE
	FPLC_CENTER_RIGHT,	//E
	FPLC_LOWER_RIGHT,	//SE
	FPLC_LOWER_CENTER,	//S
	FPLC_LOWER_LEFT,	//SW
	FPLC_CENTER_LEFT,	//W
	FPLC_UPPER_LEFT,	//NW

	FPLC_MAX
}FP_Location;

typedef struct _RECT
{
    int left;
    int top;
    int width;
    int height;
}RECT;

typedef struct _FP_Stime
{
	WORD wMJD;
	BYTE u8UTC[3];
}FP_Stime; //start time

typedef struct _FP_Msg
{
	BYTE bShowOn;
	BYTE bNewStr;
	RECT Rect;		//FP position
	BYTE u8StrLen;	//string data size
	BYTE *pStr;
	WORD wLogoLen;
	BYTE *pLogo;
	
	DWORD dwStartTime;
	DWORD dwDuration;
	WORD wServiceNum;
	WORD wServiceID[FP_SERVICE_MAX_NUM];
	BYTE reserved;
}FP_Msg;

typedef struct _FP_Infor
{
	BYTE u8CmdID;		//Command ID
	WORD wCaSystemID;	//CA system ID
	DWORD dwTransID;	//Transaction ID
	BYTE u8DataTypeNum; //Data type Number
	FP_Msg Msg; 
}FP_Infor;


//! This is the Finger Print resource structure
extern const RESOURCE_INFO FingerPrint;

void WVCI_CheckFingerPrint(void);



#ifdef __cplusplus
}
#endif

#endif // __CI_FINGER_PRINT_H


