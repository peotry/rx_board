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
#ifndef __CI_LOWSPEEDCOMM_H
#define __CI_LOWSPEEDCOMM_H

#ifdef __cplusplus
extern "C" {
#endif

//! @file CI_LowSpeedComm.h
//! This file contains the API declaration for the LowSpeed Communication Resource.
//! Low speed communication resources are not registered by the CI library automatically
//! in response to a call of CI_Init(). A CI host has to call CI_RegisterResource() with
//! resource IDs that match the capabilites (serial port, cable modem or PSTN modem) of the
//! host. The CI stack provides a default implementation that breaks down the low speed
//! communication messages into the API described in this file.
//! The file CI_LowSpeedComm.h also contains several defines and enums to build
//! correct resource ids which do not show up in doxygen. Please note that CONAX modules 
//! only connect to resources with a the id RESOURCEID_LOWSPEEDCOMM_CONAX instead of searching
//! for a compatible returnchannel. 

#define LSCOMM_MODEM_MIN	(0x00)
#define LSCOMM_MODEM_MAX	(0x3F)
#define LSCOMM_SERIAL_MIN	(0x40)
#define LSCOMM_SERIAL_MAX	(0x4F)
#define LSCOMM_CABLE		(0x50)

typedef enum {
	LSC_MODEM_DATAPROC_NEGOTIATE	= 0,
	LSC_MODEM_DATAPROC_NOV42BIS		= 1,
	LSC_MODEM_DATAPROC_NOPROCESSING = 2,
	LSC_MODEM_DATAPROC_RESERVED		= 3
} LSC_MODEM_DATAPROC;

typedef enum {
	LSC_MODEMTYPE_RESERVED0 = 0,	
	LSC_MODEMTYPE_RESERVED1 = 1,
	LSC_MODEMTYPE_V21		= 2,
	LSC_MODEMTYPE_RESERVED3 = 3,
	LSC_MODEMTYPE_V22		= 4,
	LSC_MODEMTYPE_V22BIS	= 5,
	LSC_MODEMTYPE_V23		= 6,
	LSC_MODEMTYPE_RESERVED7 = 7,
	LSC_MODEMTYPE_V32		= 8,
	LSC_MODEMTYPE_V32BIS	= 9,
	LSC_MODEMTYPE_V34		=10,
	LSC_MODEMTYPE_RESERVED8 =11,
	LSC_MODEMTYPE_RESERVED9 =12,
	LSC_MODEMTYPE_RESERVED10=13,
	LSC_MODEMTYPE_V27ter	=14,
	LSC_MODEMTYPE_V29		=15	
} LSC_MODEMTYPE;

#define MAKE_LSC_MODEM_RTYPE(modemtype,dataproc,devicenumber) (((modemtype&0x0F)<<4)|((dataproc&0x03)<<2)|((devicenumber&0x03)))
#define MAKE_LSC_RTYPE(devicetype,devicenumber)				  (((devicetype&0xFF)<<2)|((devicenumber&0x03)))
#define MAKERESOURCEID(residtype,rclass,rtype,rversion)		  ((residtype&0x03)<<30)|((rclass&0x3FFF)<<16)|((rtype&0x3FF)<<6)|(rversion&0x3F)

// This is the resource id requested by a conax V3 or V4 module for the PPV ordering.
// This id is hardcoded in the modules. (0x00601641)
#define RESOURCEID_LOWSPEEDCOMM_CONAX	MAKERESOURCEID(0,96,MAKE_LSC_MODEM_RTYPE(LSC_MODEMTYPE_V22BIS,LSC_MODEM_DATAPROC_NOPROCESSING,1),1)

// This is the resource id for the first serial port
#define RESOURCEID_LOWSPEEDCOMM_SERIAL1	MAKERESOURCEID(0,96,MAKE_LSC_RTYPE(LSCOMM_SERIAL_MIN,1),1)

// This is the resource id for the first cable modem
#define RESOURCEID_LOWSPEEDCOMM_CABLE1	MAKERESOURCEID(0,96,MAKE_LSC_RTYPE(LSCOMM_CABLE,1),1)


//! This is the low speed communication resource structure for STBs supporting the conax returnchannel
extern const RESOURCE_INFO LowSpeedCommConax;
//! This is the low speed communication resource structure for STBs supporting at least one serial port
extern const RESOURCE_INFO LowSpeedCommSerial1;
//! This is the low speed communication resource structure for STBs with a cablemodem
extern const RESOURCE_INFO LowSpeedCommCableModem;

#define TAG_T_COMMS_CMD				(0x9f8c00)
#define TAG_T_CONNECTION_DESCRIPTOR (0x9f8c01)
#define TAG_T_COMMS_REPLY			(0x9f8c02)
#define TAG_T_COMMS_SENDLAST		(0x9f8c03)
#define TAG_T_COMMS_SENDMORE		(0x9f8c04)
#define TAG_T_COMMS_RCVLAST			(0x9f8c05)
#define TAG_T_COMMS_RCVMORE			(0x9f8c06)

#define MAX_LSC_SENDSIZE			(254)

//! This function is implemented by the LowSpeed Communication Resource in the Common Interface Library.
//! It has to be called in response to a CIHost_ConnectReturnChannel_Telephone or CIHost_ConnectReturnChannel_Cable call.
//! @param hCISlot The handle which has previously been passed to one of the CIHost_Connect* functions.
//! @param fSuccess TRUE if the connection has been established, FALSE otherwise
void CI_ConnectResult(CI_HANDLE hCISlot, BOOL fSuccess);

//! This function is implemented by the LowSpeed Communication Resource in the Common Interface Library.
//! It has to be called, when the STB receives data on an existing connection or in response to
//! a CIHost_Send on the serial port.
//! @param hCISlot The CI slot handle for the existing connection.
//! @param pData Pointer to the buffer which contains the received data.
//! @param wLength Size of the pData buffer.
void CI_LSC_Data(CI_HANDLE hCISlot, const BYTE *pData, WORD wLength);

//! This function has to be implemented by CI hosts with a PSTN modem.
//! It is called, when a Module tries to establish a dialup connection using a
//! modem returnchannel (as indicated by a matching LSC resource id). 
//! In response to this call, the host should call CI_ConnectResult to inform
//! the library about the state of the connection.
//! @param hCISlot Handle for the CI slot
//! @param pTelephoneDesc DVB Telephone descriptor, see EN300468 for details.
void CIHost_ConnectReturnChannel_Telephone(CI_HANDLE hCISlot, const BYTE *pTelephoneDesc);

//! This function has to be implemented by CI hosts with a cable modem returnchannel.
//! It is called, when a Module tries to establish a returnchannel connection using a
//! the hosts cablemodem (as indicated by a matching LSC resource id). 
//! In response to this call, the host should call CI_ConnectResult to inform
//! the library about the state of the connection.
//! @param hCISlot Handle for the CI slot
//! @param bChannelId The cablemodem channel id.
void CIHost_ConnectReturnChannel_Cable(CI_HANDLE hCISlot, BYTE bChannelId);

//! This function has to be implemented by a CI host.
//! It is called to send data over an established dialup or cablemodem connection.
//! If no connection has previously been established, the output should be sent to the serial port (if any)
//! @param hCISlot Handle for the CI slot
//! @param pData Pointer to the data which should be sent.
//! @param dwLength Size of the pData buffer.
//! @return TRUE if the data has been sent successfully, FALSE otherwise.
BOOL CIHost_Send(CI_HANDLE hCISlot, const BYTE *pData, DWORD dwLength);

//! This function has to be implemented by a CI host.
//! It is called by the CI library to disconnect an established dialup or cablemodem connection.
//! The host does not have to call any other notification. The library assumes that this call
//! will never fail.
//! @param hCISlot Handle for the CI slot
void CIHost_DisconnectReturnChannel(CI_HANDLE hCISlot);

// These three functions are intentionally not static (like the other resource functions are)
// because the host will have to declare his own RESOURCE_INFO structure for a PSTN modem returnchannel.
extern BOOL LowSpeedComm_Connect(CI_HANDLE hCI, WORD hSession);
extern void LowSpeedComm_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen);
extern void LowSpeedComm_Close(CI_HANDLE hCI, WORD hSession);


#ifdef __cplusplus
}
#endif

#endif // __CI_LOWSPEEDCOMM_H
