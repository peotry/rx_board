// ********************************************************
//
//  $Workfile: $ 
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
//
//  This file contains the portable CI Lib interface/internals
//  
#ifndef __CI_H
#define __CI_H

//! @file Ci.h
//! This file contains the core API of the common interface library.

//! @mainpage GkWare EN50221 Common Interface library
//! This is the main index page of the doxygen documentation for the CI library 2.0 (as of June 14th 2008).
//! @section a State of this documentation
//! This doxygen documentation contains the latest version of the core API for initializing the library and
//! for adding/removing additional resources. It also includes documentation
//! for most resources that ship as part resource APIs. Even more inline documentation can be found in the
//! library sourcecode.
//! A good starting point after this index page is the "File List" page, which will lead you
//! to the API descriptions for the different parts of the CI library, including the standard resources.
//! Start with pcmcia.h, to get an overview for the lowlevel PCMCIA API that you have to implement.
//!
//! @section b Definitions
//! The library itself is called CI stack or CI library in this documentation.
//! The term "CI Host" in general refers to a piece of hard- and software that accepts
//! a common interface module. In this documentation, functions that start with the
//! CIHost_ prefix have to be implemented by the user of the CI library (CIHost_ConnectReturnChannel_Telephone() for example).
//! For legacy support reasons, the function SendCIMessage() is the only exception to this rule.
//!
//! @section c Build Configuration and platform header
//! The entire library code is guarded by ##ifdef ENABLE_CI. Some resources that are optional are guarded by extra ##defines.
//! If ENABLE_CI is not defined, the library produces no code during the build process and API calls will result in linker errors.
//! Support for the host control resource is guarded by ##ifdef ENABLE_CI_HOSTCONTROL.
//! Support for the lowspeed communication resource is guarded by ##ifdef ENABLE_CI_LOWSPEEDCOMM.
//! To maintain optimal compiler, runtime and platform portability, all .c files in this library just include one single file 
//! called all.h. This file is responsible for including all required C runtime headers and for
//! defining some standard types (BOOL,BYTE,WORD,DWORD) and defines (TRUE,FALSE,NULL).
//! In addition to these basic types, the macro MSTOTICKS(ms) has to be defined to convert the millisecond parameter
//! into the number of system clock ticks. This macro is used to call the function GURU_Delay(ticks) as part
//! of some lowlevel I/O functions.
//! The DateTime Resource additionally requires the function GURU_GetTickCount() which should return the
//! value of the system global clock tick counter. This mechanism is used to maintain correct intervals
//! for time notifications sent to CI modules.
//! For the first steps and for single-service environments ENABLE_AUTOMATIC_PMT_SEND should be defined as well.
//! If this define is enabled, the library automatically regenerates and re-sends CA PMTs. This mode is incompatible
//! with headend descrambling because the CAPMT is always generated with the CA_PMT_LIST_MANAGEMENT_ONLY code.
//! For headend descrambling, the host is responsible for generating and sending appropriate CA-PMTs.
//!
//!
//! @section d Getting Started
//! To start descrambling PayTV, only very few integration steps and function calls are necessary.
//! <p>1. Implement a PCMCIA driver that implements PCMCIA_IsModuleStillPlugged(), PCMCIA_WriteAttribMem(), PCMCIA_WriteIOMem(),
//!    PCMCIA_ReadIOMem() and PCMCIA_EnableTSRouting() for your hardware. For selected hardware configurations (e.g. SCM CIMax),
//!    an implementation sample can be provided. 
//! <p>2. Declare a "DWORD g_dwNumPCMCIASlots=<1-MAX_CI_SLOTS>" in your PCMCIA driver and call CI_Init() to initialize the library
//! <p>3. Start your slot monitoring task (if you have a multitasking OS) or include the following CI function calls in your
//!    global loop (if you have no multitasking OS)
//! <p>4. call CI_CheckConfig() when you detect that a CI module has been inserted and the CIS has been decoded.
//! <p>5. call CI_CheckSlot() periodically when CI_CheckConfig() returned TRUE
//! <p>6. forward the PMT of the services that you intend to descramble to CI_PMTArrived()
//! <p>7. forward the TDT/TOT to the Datetime resource
//! <p>8. implement your MMI based on SendCiMessage()
//! <p>9. choose which optional resources you want to implement/support 
//! <p>
//! All CIHost functions (which are not mentioned here) that the linker may ask for at step 6 can be left as empty dummy functions in this state.
//! Once the descrambling works, you should fill your SendCIMessage() function with life and display Highlevel MMI
//! messages that the CI modules send. If your box contains a serial port or another returnchannel, you
//! should also implement the CIHost functions for the low speed communication resource to reach support for the
//! full profile level 1.
//!
//! @section e Debugging
//! In most implementation files you will find an XXX_DEBUG_ENABLE ##define that controls the behaviour of debugprints for that
//! file. Debugprints are contained in sourcecode in a way that prevents every compiler supported so far from including the strings
//! or any code if debugging is disabled. No STB should ship with CI debugprints enabled.
//!
//! @section f Troubleshooting
//! <p> <b>If no module is detected</b>, your PCMCIA API does probably not work as expected or your hardware is not fully PCMCIA compliant.
//! <p> <b>If some modules are not detected</b>, your hardware signal timing might not be PCMCIA compliant. Check the module in another STB. If the problem persists, please contact GkWare for support.
//! <p> <b>If modules are detected but do not descramble</b>,
//! <p> 1. Check that the module opens a CA support session by enabling debugging for the CA support resource.
//!        Some modules only open this resource, if a smartcard has been inserted into the module. Some other modules
//!        implement provider locking (Premiere germany, NDS Videoguard) to prevent usage in uncertified STBs. Support
//!        for these modules usually requires the implementation of additional resources and a licensing agreement with the
//!        paytv provider.
//! <p> 2. Check if the module opens a MMI session with a message that might explain the status.
//!        If the module does not report the status by itself, enter and check the CI menu manually.
//!        Automatic popups can be disabled in most modules.
//! <p> 3. Check that the inserted smartcard has the correct entitlement for the service you are trying to descramble.
//! <p> 4. Reduce the polling speed. Polling requires CPU power on modules that implement the lowlevel buffering in software.
//!        Especially some older modules with the Sidsa Chipset are likely to stop or completely refuse descrambling if the
//!        polling speed is too fast.


#ifdef ENABLE_CI

#ifdef __cplusplus
extern "C" {
#endif

// PC CARD Physical Layer


#define CI_PHYS_REG_DATA            (0)
#define CI_PHYS_REG_COMMANDSTATUS   (1)
#define CI_PHYS_REG_SIZELOW         (2)
#define CI_PHYS_REG_SIZEHIGH        (3)


// Status register bits
#define CISTATUS_DATAAVAILABLE (0x80)   //!< CI Status register bit - The module wants to send data
#define CISTATUS_FREE          (0x40)   //!< CI Status register bit - The module can accept data
#define CISTATUS_RESERVEDBITS  (0x3C)   //!< CI Status register bits - reserved
#define CISTATUS_WRITEERROR    (0x02)   //!< CI Status register bit - Write error
#define CISTATUS_READERROR     (0x01)   //!< CI Status register bit - Read error

#define CICOMMAND_RESET         (0x08)  //!< CI Command register bit - Reset
#define CICOMMAND_SIZEREAD      (0x04)  //!< CI Command register bit - Size read
#define CICOMMAND_SIZEWRITE     (0x02)  //!< CI Command register bit - Size Write
#define CICOMMAND_HOSTCONTROL   (0x01)  //!< CI Command register bit - Host control

#define CICOMMAND_DAIE          (0x80)  //!< CI Command register bit - R206-001.V1 extension
#define CICOMMAND_FRIE          (0x40)  //!< CI Command register bit - R206-001.V1 extension

///////////////////////////////////////////////////////////////////
// Internals / CI Protocol Stack Part

#define MAX_CI_SESSIONS     (15)            //!< Configuration - Maximum number of CI sessions per slot
#define MAX_CASYS_IDS       (32)//(100)       //!< Configuration - Maximum number of CA system IDs
#define MAX_CI_MODULENAME   (32)//(50)        //!< Configuration - Maximum length of a CI module name
#define MAX_LEN_CA_PMT      (1024)      //!< MPEG2 defined maximum size of a PMT section


#define MAX_CI_SLOTS        (4)         //!< Configuration - This is the maximum number of CI slots
#define MAX_CI_RESOURCES    (20)        //!< Configuration - This is the maximum number of simultaneously registered resources

//! This structure holds the data for an established connection to a CI resource.
typedef struct 
{
    BOOL fSlotInUse;        //!< flag if this session slot is in-use
    BYTE bTransportConnID;  //!< ID of the lowlevel transport connection
    WORD wSessionID;        //!< session id
    struct RESOURCE_INFO const *pResource;
} CI_SESSION; 

#define CI_T_SENDQSIZE      120                                                 // 30
#define CI_T_SENDQBUFSIZE   0x300
#define MAX_CI_TPDU         4096

typedef struct {
    WORD wBufferSize;
    WORD wReserved;
    WORD CI_T_SENDQ_CAPMT_ENDFRAME[CI_T_SENDQSIZE];                             // capmt flag
    BYTE CI_T_SENDQ[CI_T_SENDQSIZE][CI_T_SENDQBUFSIZE];
    WORD CI_T_SENDQ_Size[CI_T_SENDQSIZE];

} CI_RXTXTPDUBUFFER,*CI_TPDUBUFHANDLE;

#define MSTOTICKS(x)    (x*1000)

typedef WORD CISESSIONHANDLE;

//! This structure represents the state of one individual CI slot
typedef struct 
{
    ////////////////////////////////
    // PCMCIA related
    PCMCIA_HANDLE hSlotHandle;  //!< handle to the PCMCIA Driver
    BYTE bSlotNum;              //!< internal (0-based) slot number
    BOOL fHighPrio;             // hint to the PCMCIA Driver (for P Modules)
    BYTE bIRQBits;              //

    ////////////////////////////////
    // Buffer related
    CI_RXTXTPDUBUFFER IOBuffer_Host2CI; //!< data buffer, contains data to be sent during the next poll
    BYTE CI_T_RX[MAX_CI_TPDU];          //!< buffer for the re-assembly of incoming TPDUs
    BYTE TPDU[MAX_CI_TPDU];             //!< buffer for the re-assembly of incoming TPDUs
    DWORD TPDULen;                      //!< current length of an incoming TPDU
    DWORD dwRXTPDULen;
    BYTE bRXTPDUBuf[MAX_CI_TPDU];
    BOOL fRXPending;                    //!< flag that is set to TRUE if an multipart TPDU is waiting to be completed
    BOOL fGotPollAck;                   //!< flag that is set to TRUE in response to a T_SB (enforce 1 T_SB per poll)
    BYTE bUnknown_TPDU;                 //!< unknown TPDU counter, used to trigger a reset for crashed modules

    ////////////////////////////////
    // Misc
    BYTE ModulReset; // internal flag... causes a call to PCMCIA Reset later

    ////////////////////////////////
    // Session related
    CI_SESSION CiSessions[MAX_CI_SESSIONS]; //!< array for open sessions on this slot

    WORD wAppInfoSession;                   //!< current session handle for the application info resource (0 if none)
    char pszModuleName[MAX_CI_MODULENAME];  //!< Name of the module, according to the application info resource
    BYTE bModuleNameLength;                 //!< length of the module name, according to the application info resource

    BOOL fIsPremiereModule;
    BOOL fModuleRequiresCAIDFiltering;

    DWORD dwCASupportSession;               //!< current session handle for the CA support resource (0 if none)
    WORD wCASystemIDs[MAX_CASYS_IDS];       //!< array of DVB CA System IDs supported by the module
    WORD wNumCASystems;                     //!< number of valid entries in the ca_system_ids array

    DWORD dwDateTimeSession;                //!< current session handle for the datetime resource (0 if none)
    DWORD dwDateTimeIntervallTicks;         //!< requested interval for date/time notifications in system ticks (0 for none)
    DWORD dwLastDateTimeSendTicks;          //!< System tick counter value at which the last date/time packet has been sent or 0 if no packet has been sent yet
    
#if defined(ENABLE_FINGER_PRINT)
    CISESSIONHANDLE wFP_Session;            //!< current session handle for the SAS resource (0 if none)
#endif
    CISESSIONHANDLE dwMMISession;           //!< current session handle for the mmi resource (0 if none)

    CISESSIONHANDLE dwAuthSession;          //!< current session handle for the authentication resource (0 if none or unsupported)

    CISESSIONHANDLE dwMacrovisionSession;   // private
    
    CISESSIONHANDLE dwPinMgmtSession;       // private
    
    CISESSIONHANDLE dwResMgrSession;        //!< current session handle for the resource manager resource (0 if none)

    CISESSIONHANDLE dwPowerMgmtSession;     //!< current session handle for the power manager resource (0 if none)

    // Lowspeed Communication
    CISESSIONHANDLE dwLSCSession;           //!< current session handle for the low speed communication resource (0 if none)
    BYTE bLSCBufferSize;
    BYTE bLSCTimeout;
    BOOL fLSCConnecting;
    BOOL fLSCConnected;
    BYTE bLSCSendPhase;
    BYTE bLSCReceivePhase;
} CI_HANDLESTRUCT,*CI_HANDLE;

//! Structure representing a common interface resource.
typedef struct RESOURCE_INFO
{
    DWORD dwResourceID;                     //!< The 24bit resource id
    BOOL (*Connect)(CI_HANDLE hCI, WORD hSession);                              //!< This function is called when a module connects to a CI resource. The resource implementation can return TRUE to accept the connection or FALSE to reject it.
    void (*Data)(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen); //!< This function is called when a module sends data to a resource.
    void (*Close)(CI_HANDLE hCI, WORD hSession);                                //!< This function is called when a resource connection is closed.
} RESOURCE_INFO;

extern RESOURCE_INFO const *g_CiResources[MAX_CI_RESOURCES];

//! This is the global SLOT status array
extern CI_HANDLESTRUCT CiSlot[MAX_CI_SLOTS];

//! This is a pointer to the CI library version string
extern const char *g_pszCiStackVersion;

///////////////////////////////////////////////////////////////////

//! This enum represents the values of the ca_pmt_list_management table in EN50221.
//! 95% of all modules only support CA_PMT_LIST_MANAGEMENT_ONLY. PVR systems
//! require more than just CA_PMT_LIST_MANAGEMENT_ONLY to descramble more than one
//! service at the same time.
typedef enum
{
  CA_PMT_LIST_MANAGEMENT_MORE   = 0x00,     //!< more PMTs following
  CA_PMT_LIST_MANAGEMENT_FIRST  = 0x01,     //!< this is the first of a list of new PMTs
  CA_PMT_LIST_MANAGEMENT_LAST   = 0x02,     //!< this is the last PMT of a list started with CA_PMT_LIST_MANAGEMENT_FIRST
  CA_PMT_LIST_MANAGEMENT_ONLY   = 0x03,     //!< this is the PMT of the only service which should be scrambled
  CA_PMT_LIST_MANAGEMENT_ADD    = 0x04,     //!< add this PMT to the list of currently descrambled services
  CA_PMT_LIST_MANAGEMENT_UPDATE = 0x05,      //!< update the PMT of a service which is already in the active PMT list
  CA_PMT_LIST_MANAGEMENT_WELLAV_DESCRAMBLE = 0x11 // WELLAV增加测试用，不符合en50221标准
} CAPMTLISTMANAGEMENT;

//! This enum represents the values of the ca_pmt_cmd_id table in EN50221.
//! Only CA_PMT_CMD_ID_OK_DESCRAMBLING is supported by the
//! modules tested so far.
typedef enum
{
  CA_PMT_CMD_ID_OK_DESCRAMBLING     = 0x01, //!< just descramble
  CA_PMT_CMD_ID_OK_MMI              = 0x02, //!< just open the mmi (if required) but do not descramble
  CA_PMT_CMD_ID_QUERY               = 0x03, //!< check if descrambling is possible
  CA_PMT_CMD_ID_NOT_SELECTED        = 0x04  //!< stop descrambling
} CAPMTCMDID;

// SendCIMessage Tags
#define CI_MSG_NONE                 (0)
#define CI_MSG_CI_INFO              (1)
#define CI_MMI_MENU                 (2)
#define CI_MMI_LIST                 (3)
#define CI_MMI_TEXT                 (4)
#define CI_MMI_REQUEST_INPUT        (5)
#define CI_MMI_CLOSE_MMI_IMM        (9)

// the following codes are used as second byte of a CI_MSG_CI_INFO message
#define CI_SLOT_EMPTY               (0)
#define CI_SLOT_MODULE_INSERTED     (1)
#define CI_SLOT_MODULE_NAME_OK      (2)
#define CI_SLOT_MODULE_CA_OK        (3)



/////////////////////////////////////////////////////////////////
// TPDUs
#define CI_T_SB             0x80 // Host <-  Module
#define CI_T_RCV            0x81 // Host  -> Module
#define CI_T_CREATE         0x82 // Host  -> Module
#define CI_T_CREATE_REPLY   0x83 // Host <-  Module
#define CI_T_DELETE_TC      0x84 // Host <-> Module
#define CI_T_DELETE_REPLY   0x85 // Host <-> Module
#define CI_T_REQUEST_TC     0x86 // Host <-  Module
#define CI_T_NEW_TC         0x87 // Host  -> Module
#define CI_T_TC_ERROR       0x88 // Host  -> Module
#define CI_T_DATALAST       0xA0 // Host <-> Module
#define CI_T_DATAMORE       0xA1 // Host <-> Module
// SPDUs
#define CI_SESSION_OPEN_REQUEST     0x91
#define CI_SESSION_OPEN_RESPONSE    0x92
#define CI_SESSION_CREATE           0x93
#define CI_SESSION_CREATE_RESPONSE  0x94
#define CI_SESSION_CLOSE_REQUEST    0x95
#define CI_SESSION_CLOSE_RESPONSE   0x96
#define CI_SESSION_DATA             0x90

/////////////////////////////////////////////////////////////////////////////////////
// Public APIs
// 

//! This is the first function a CI host should call. It is used to initialize the common interface
//! library. This function will initialize all internal structures and it will register
//! all standard resources which are implemented in the library.
extern BOOL CI_Init(void);

//! This function registers an additional resource implemented by the CI host.
//! It should be ideally called immediately after the CI_Init call and must not be called while
//! CI_CheckSlot is executed to maintain thread-safety.
extern BOOL CI_RegisterResource(const RESOURCE_INFO *pRes);

//! This function deregisters an additional resource implemented by the CI host.
//! The same calling rules apply as for CI_RegisterResource().
extern BOOL CI_UnregisterResource(const RESOURCE_INFO *pRes);

// PCMCIA Bus Driver(s) to CI stack
//! This function boots a common interface module, after the CIS has been parsed.
//! In the context of this function, the buffer size negotiation and the TS-enable happen.
//! @param hCI Private handle of the PCMCIA slot, used for the PCMCIA API calls.
//! @param pInfo PCMCIA structure, as generated by the CIS parser
//! @param bSlotNum zero-based index of the CI slot.
BOOL CI_CheckConfig(PCMCIA_HANDLE hCI, PCMCIA_INFO *pInfo, BYTE bSlotNum);




/////////////////////////////////////////////////////////////////////////////////////
// Internal APIs
// for communication between the different layers


// L2->CI Protocol

//! This function is called automatically by CI_CheckConfig, when a valid CI module has been
//! detected in the slot identified by bSlotNum. This function resets various
//! values in the CI handle array and informs the CI host, that a module has been inserted.
//! @param hCI Private handle of the PCMCIA slot, used for the PCMCIA API calls.
//! @param bSlotNum zero-based index of the CI slot number.
void CI_ModuleInserted(PCMCIA_HANDLE hCI, BYTE bSlotNum);

//! This function has to be called by a PCMCIA driver, when a module has been
//! removed from the slot identified by bSlotNum. This function closes all opened
//! sessions and informs the CI host, that a module has been removed.
//! @param hCI Private handle of the PCMCIA slot, used for the PCMCIA API calls.
//! @param bSlotNum zero-based index of the CI slot.
void CI_ModuleRemoved(PCMCIA_HANDLE hCI, BYTE bSlotNum);

//! This function has to be called periodically by a PCMCIA driver for inserted modules.
//! In the context of this function call, the library checks the module status and sends
//! pending TPDUs or receives data from the module. 
//! @param hCI Private handle of the PCMCIA slot, used for the PCMCIA API calls.
//! @param bSlotNum zero-based index of the CI slot.
//! @return TRUE if the module is ok. FALSE if an error occured and the module should be reset.
BOOL CI_CheckSlot(PCMCIA_HANDLE hCI, BYTE bSlotNum);


// add by shouquan.tang
int CI_SendCAPMT2CAM(BYTE ucSlotIndex, const BYTE *pucCAPMT, WORD wCAPMTLen);
//


// Hardware Layer -> CI Stack
BOOL Parse_CI_T_Packet(CI_HANDLE hCI, const BYTE *pData, WORD dwLen);
void Send_T_CREATE(CI_HANDLE hCI, BYTE bConnId);
void Send_T_DataLast(CI_HANDLE hCI, BYTE bConnId, const BYTE *pData, WORD dwLen);

// CI Stack -> Hardware layer
extern void CI_WriteData(CI_TPDUBUFHANDLE hBuf, WORD wBufferSize, const BYTE *pData, WORD wPMTEndFlag);

// CI Stack -> API

/**
This function has to be implemented by the host in order to process MMI-relevant events. 
The library uses this function to report events like module insertion, completion of the
initialization (CI_SLOT_MODULE_OK), removal and reception of MMI messages. The host
usually calls functions from the CI_MMI.h API in response to CI_MMI_* messages after
presenting the available choices to the user.<p>
 The CI library calls this function with one of the following messages</p>
<table border="1">
  <tr>
    <td>pData[0]</td>
    <td>pData[1]</td>

    <td>pData[2]</td>
    <td>&nbsp;</td>
  </tr>
  <tr>
    <td>CI_MSG_CI_INFO</td>
    <td>CI_SLOT_EMPTY </td>
    <td>Slot number</td>

    <td>The module has been removed (unexpectedly)</td>
  </tr>
  <tr>
    <td>CI_MSG_CI_INFO</td>
    <td>CI_SLOT_MODULE_INSERTED </td>
    <td>Slot number</td>

    <td>A module has been inserted</td>
  </tr>
  <tr>
    <td>CI_MSG_CI_INFO</td>
    <td>CI_SLOT_MODULE_NAME_OK </td>
    <td>Slot number</td>

    <td>A module has been initialized. The zero-terminated Module name is
      stored at pData[3].
    </td>
  </tr>
  <tr>
    <td>CI_MSG_CI_INFO</td>
    <td>CI_SLOT_MODULE_CA_OK </td>
    <td>Slot number</td>

    <td>A module has been initialized and the CA support session has been opened. 
      pData[3] contains the number of 2-byte CA-System IDs that follow. 
      The list can change at any time (which means that this message can be sent
      by a module more than one time. 

      If the ENABLE_AUTOMATIC_PMT_SEND define is not used, the host should (re-)send
      PMTs in the context of this notification message.
    </td>
  </tr>
  <tr>
    <td>CI_MMI_MENU</td>
    <td>Slot Number</td>
    <td>&nbsp;</td>
    <td>Byte 2/3 contain a session id. Byte 4 contains the number of text
      entries that follow. In response to this message, the host should call
      CI_MMI_AnswerMenu to select a menu entry.</td>

  </tr>
  <tr>
    <td>CI_MMI_LIST </td>
    <td>Slot Number</td>
    <td>&nbsp;</td>
    <td>Byte 2/3 contain a session id. Byte 4 contains the number of text
      entries that follow. In response to this message, the host should call
      CI_MMI_AnswerMenu to send an ok or exit keypress as response. List entries
      should not be selectable.
    </td>
  </tr>

  <tr>
    <td>CI_MMI_REQUEST_INPUT</td>
    <td>Slot Number </td>
    <td>&nbsp;</td>
    <td>Byte 2/3 contain a session id. Byte 4 contains a flag for Pin blinding.
      Byte 5 contains the length of the expected pin response.
      Raw MMI data with a text string describing the request follows...<br>
      In response to this message, the host should call
      CI_MMI_AnswerInput to reply to the input request.
    </td>
  </tr>
  <tr>

    <td>CI_MMI_CLOSE_MMI_IMM</td>
    <td>Slot Number</td>
    <td>&nbsp;</td>
    <td>Byte 2/3 contain a session id.</td>
  </tr>
</table>

  */

void SendCIMessage(CI_HANDLE hCI, BYTE *pData, DWORD dwDataLength);
int CI_DescramblePMTArrived(BYTE ucSlotIndex, const BYTE *pucPMTBuff, WORD wPMTLen);
// API -> CI Stack

//! This function is called by the standard resource implementations and external addon resources
//! to send data on an established Session. This function is responsible for splitting the data
//! into the required TPDUs and for submitting them to the lower communication layer(s).
//! @param hCI Handle of the CI slot for which the bConnID and wSessionNumber are valid
//! @param bConnID Transport connection ID on which the session has been established.
//! @param wSessionNumber The session handle that has been assigned for the session.
//! @param pData Pointer to the data which should be sent.
//! @param wLen Size of the pData buffer.
void Send_SPDU_Data(CI_HANDLE hCI, BYTE bConnID, CISESSIONHANDLE wSessionNumber, const BYTE *pData, WORD wLen);

//! This function is called by the MMI implementation to send a session close request when a TAG_T_CLOSEMMI arrived
//! @param hCI Handle of the CI slot for which the bConnID and wSessionNumber are valid
//! @param bConnID Transport connection ID on which the session has been established.
//! @param wSessionNumber The session handle that has been assigned for the session.
void Send_SPDU_CloseRequest(CI_HANDLE hCI, BYTE bConnID, CISESSIONHANDLE wSessionNumber);


//! This internal function decodes a ASN.1 length field as it is used
//! by almost all layers and messages. A CI host should not need to call this function directly.
//! @param pData IN, pointer to length field
//! @param pLength OUT, decoded length
//! @return size of the lenght field
WORD CI_DecodeLengthField(const BYTE *pData,WORD *pLength);

// include declarations for the standard resources
#include "CI_AppInfo.h"
#include "CI_CaSupport.h"
#include "CI_DateTime.h"
#include "CI_MMI.h"
#include "CI_PMT.h"
#include "CI_PMTDec.h"
#include "CI_ResMgr.h"

// and the optional ones that are rarely or not-at-all used 
#ifdef ENABLE_CI_HOSTCONTROL
#include "CI_Hostcontrol.h"
#endif
#ifdef ENABLE_CI_COPYPROTECTION
#include "CI_CopyProtection.h"
#include "CI_CopyProtection_DTG.h"
#endif
#ifdef ENABLE_CI_STATUSQUERY
#include "CI_StatusQuery.h"
#endif
#ifdef ENABLE_CI_EVENTMGMT
#include "CI_EventMgmt.h"
#endif
#ifdef ENABLE_CI_LOWSPEEDCOMM
#include "CI_LowSpeedComm.h"
#endif
#ifdef ENABLE_CI_POWERMANAGEMENT
#include "CI_PowerMgmt.h"
#endif
#ifdef ENABLE_CI_APPLICATIONMMI
#include "CI_ApplicationMMI.h"
#endif

#if defined(ENABLE_FINGER_PRINT)
#include "CI_FingerPrint.h"
#endif

#ifdef __cplusplus
}
#endif

#endif // ENABLE_CI

#endif // __CINEU_H
