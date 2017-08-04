// ********************************************************
//
//  $Workfile: PCMCIA.h $ 
//
//   Author/Copyright	Gero Kuehn / GkWare e.K.
//						Humboldtstrasse 177
//						45149 Essen
//						GERMANY 
//						Tel: +49 174 520 8026
//						Email: support@gkware.com
//						Web: http://www.gkware.com
//
//	$Date:  $
//	$Revision:  $
//
// ********************************************************
//

//////////////////////////////////////////
//
// Includes
//
#ifndef __PCMCIA_H
#define __PCMCIA_H

#ifdef __cplusplus
extern "C" {
#endif
#include "appGlobal.h"
//! @file PCMCIA.h
//! @brief This file contains the API of the PCMCIA CIS parser and the declaration
//! for the PCMCIA driver API. PCMCIA drivers are expected to implement the functions starting with PCMCIA_ except for PCMCIA_ParseAttribMem(),
//! The PCMCIA_ParseAttribMem() implementation is provided together with the GkWare CI Stack

/****************************************************************************
 * Macro
 ****************************************************************************/

// add by shouquan.tang
#define PCMCIA_SUCCESS      (0)
#define PCMCIA_FAILURE      (-1)


#define MAX_CIS_SIZE		(0x150)	//!< The maximum size of a CIS, that is understood by this driver
#define MAX_PCMCIA_CONFIGS	(10)	//!< The maximum number of configurations supported by a PCMCIA card
#define MAX_PCMCIA_STRLEN	(20)	//!< The maximum name of vendor/manufacturer/info strings

#define PCMCIAINFO_MANID_VALID	        (0x00000001)
#define PCMCIAINFO_VERS1_VALID          (0x00000002)
#define PCMCIAINFO_FUNCID_VALID         (0x00000004)


////////////// PCMCIA Specifical //////////////////
#define CISTPL_NULL                         0x00
#define CISTPL_DEVICE                       0x01
#define CISTPL_LONGLINK_CB                  0x02
#define CISTPL_INDIRECT                     0x03
#define CISTPL_CONFIG_CB                    0x04
#define CISTPL_CFTABLE_ENTRY_CB             0x05
#define CISTPL_LONGLINK_MFC                 0x06
#define CISTPL_BAR                          0x07
#define CISTPL_PWR_MGMNT                    0x08
#define CISTPL_EXTDEVICE                    0x09

//( 0x0A... 0x0F)
#define CISTPL_CHECKSUM                     0x10
#define CISTPL_LONGLINK_A                   0x11
#define CISTPL_LONGLINK_C                   0x12
#define CISTPL_LINKTARGET                   0x13
#define CISTPL_NO_LINK                      0x14
#define CISTPL_VERS_1                       0x15
#define CISTPL_ALTSTR                       0x16
#define CISTPL_DEVICE_A                     0x17
#define CISTPL_JEDEC_C                      0x18
#define CISTPL_JEDEC_A                      0x19
#define CISTPL_CONFIG                       0x1A
#define CISTPL_CFTABLE_ENTRY                0x1B
#define CISTPL_DEVICE_OC                    0x1C
#define CISTPL_DEVICE_OA                    0x1D
#define CISTPL_DEVICEGEO                    0x1E
#define CISTPL_DEVICEGEO_A                  0x1F

#define CISTPL_MANFID                       0x20
#define CISTPL_FUNCID                       0x21
#define CISTPL_FUNCE                        0x22
#define CISTPL_SWIL                         0x23

//(24H...3FH) Reserved for future Layer 2 tuples

#define CISTPL_VERS_2                       0x40
#define CISTPL_FORMAT                       0x41
#define CISTPL_GEOMETRY                     0x42
#define CISTPL_BYTEORDER                    0x43
#define CISTPL_DATE                         0x44
#define CISTPL_BATTERY                      0x45
#define CISTPL_ORG                          0x46
#define CISTPL_FORMAT_A                     0x47

//( 80H...8FH) Vendor unique tuples

#define CISTPL_SPCL                         0x90

//( 90H... FEH) Reserved for future Layer 4 tuples

#define CISTPL_END                          0xFF
//////////////////////////////////////////////////////////

/****************************************************************************
 * enum
 ****************************************************************************/

//! This enum contains the card types, that can be encoded in CISTPL_FUNCID (0x21)
typedef enum {
	PCMCIA_FUNC_CUSTOM = 0,
	PCMCIA_FUNC_MEMORY = 1,
	PCMCIA_FUNC_SERIAL = 2,
	PCMCIA_FUNC_PARALLEL = 3,
	PCMCIA_FUNC_DRIVE = 4,
	PCMCIA_FUNC_VIDEO = 5,
	PCMCIA_FUNC_NETWORK = 6,
	PCMCIA_FUNC_AIMS = 7,
	PCMCIA_FUNC_SCSI = 8,
	PCMCIA_FUNC_CARDBUS = 9,
	PCMCIA_FUNC_MANUFACTURER = 0xFF		// manifacturer designed purpose
} PCMCIA_FUNCTYPE;

typedef enum 
{
	CIMAX_EMPTY,                // no CAM Card insert
	CIMAX_INSERTED,             // CAM Card inserted
	CIMAX_PCMCIA_RDY,           // PCMCIA Ready
	CIMAX_BADCIS,               // BAD CIs
	CIMAX_CISREAD,              // CIS Read
	CIMAX_CISTART,              // CIS Start
	CIMAX_TS_DISABLED,          // ChECK TS DISABLE
	CIMAX_HOT_INSERTED,         // add by shouquan.tang, for not power off
	CIMAX_NOSPEC                // No specify
} CIMaxSlotState_enum;




/****************************************************************************
 * type
 ****************************************************************************/

//! This structure contains information about exactly one possible PCMCIA card configuration.
typedef struct {
	BYTE  bConfigIndex;		//!< The value of bConfigIndex has to be written into the card configuration register to activate this configuration.
	DWORD dwEAAddr;			//!< EA-address
	DWORD dwEALen;			//!< size of the EA area (zero for none)
	BYTE  bIRQDesc1;		//!< Interrupt descriptor byte
	WORD  wIRQData;			//!< supported IRQ number mask
	BYTE  fCITagsPresent;	//!< Bitfield which is 0x03 if both required CI tags are present. 0x01 for DVB_HOST, 0x02 for DVB_CI_MODULE
} PCMCIA_CONFIG;


//! This structure provides simple access to the PCMCIA card information
//! after decoding of the Card Information Structure. This decoding is performed by PCMCIA_ParseAttribMem()
typedef struct 
{
	DWORD ConfigOffset;						//!< Offset of the Configuration byte in the Attribute Memory
	DWORD dwValidFlags;								//!< Bitmask that defines which of the other fields are valid


	WORD wManufacturerId;							//!< 16Bit Manufacturer ID (PCMCIAINFO_MANID_VALID)
	WORD wCardID;									//!< 16Bit Card ID (PCMCIAINFO_MANID_VALID)

	WORD wPCMCIAStdRev;								//!< PCMCIA Standard version supported by the card (PCMCIAINFO_VERS1_VALID)
	char pszManufacturerName[MAX_PCMCIA_STRLEN];	//!< Name of the card manufacturer (PCMCIAINFO_VERS1_VALID)
	char pszProductName[MAX_PCMCIA_STRLEN];			//!< Product name (PCMCIAINFO_VERS1_VALID)
	char pszProductInfo1[MAX_PCMCIA_STRLEN];		//!< (PCMCIAINFO_VERS1_VALID)
	char pszProductInfo2[MAX_PCMCIA_STRLEN];		//!< (PCMCIAINFO_VERS1_VALID)

	PCMCIA_FUNCTYPE FuncType;						//!< Card function type (PCMCIAINFO_FUNCID_VALID)
	BYTE bFuncIDSysInfo;							//!< SysInitByte from the FuncID block (PCMCIAINFO_FUNCID_VALID)

	BYTE bNumConfigs;								//!< The number of configurations supported by the card. Exactly bNumConfigs entries are valid in the Config array.
	PCMCIA_CONFIG Config[MAX_PCMCIA_CONFIGS];		//!< The array of possible card configurations
} PCMCIA_INFO;


typedef struct 
{
    BYTE                        ucSlotIndex;
	volatile BYTE               *pucSlotMemory;
	BYTE                        ucSlotReg;
	BYTE                        ucAttribMem[MAX_CIS_SIZE];
	PCMCIA_INFO                 tPCMCIAInfo;
	CIMaxSlotState_enum         emSlotState;
	BOOL                        bLastAccessWasToAttrMem;
	
} CIMAX_PCMCIA_HANDLESTRUCT_t, *CIHANDLE;

void PCMCIA_ReadAttribMem(BYTE ucSlot,BYTE *pDest);

//! This function decodes a raw attribute memory dump into an easily readable PCMCIA_INFO structure.
//! The PCMCIA/PnP task is responsible for reading the raw attribute memory. This function
//! parses the card info structure (CIS) and decodes the relevant parts. Callers should check the
//! dwValidFlags Bitmask before accessing other fields.
int  PCMCIA_ParseAttribMem(const BYTE *pAttribMem, DWORD dwLen, PCMCIA_INFO *pInfo);

extern DWORD g_dwNumPCMCIASlots;

//! This file typedefs PCMCIA_HANDLE as void*.
//! Actual PCMCIA driver implementations can hide whatever they want inside this handle.
//! Higher level drivers pass
typedef void *PCMCIA_HANDLE;

//! Initialization function. During system initialization, this function
//! is called once to initialize the driver.
BOOL PCMCIA_Init(void);
//! This function is called by higher level drivers to check if a module is (still) present.
//! Usually, this check is performed by checking the card detect GPIO pins of a PCMCIA slot.
BOOL PCMCIA_IsModuleStillPlugged(PCMCIA_HANDLE hSlot);
//! This function is called to write the byte bData into the card attribute memory at address wAddr.
void PCMCIA_WriteAttribMem(PCMCIA_HANDLE hSlot,WORD wAddr,BYTE bData);
//! This function is called to write the byte bData into the card IO memory at address wAddr.
int  PCMCIA_WriteIOMem(PCMCIA_HANDLE hSlot,WORD wAddr,BYTE bData);
//! This function is read one byte of from the card IO memory at address wAddr.
BYTE PCMCIA_ReadIOMem(PCMCIA_HANDLE hSlot,WORD wAddr);
//! This function is called to enable or disable the TS stream for PCMCIA common interface slots.
//! @param hSlot Handle for the slot for which the TS is to be en-/disabled.
//! @param fEnable TRUE to enable the TS, FALSE otherwise.
int PCMCIA_EnableTSRouting(PCMCIA_HANDLE hSlot,BOOL fEnable);
//! The implementation of this function is optional for CI, but a must for CI+.
//! It is called when the CI+ stack reveives a request_cicam_reset-APDU.
//! It is also called when a higher level driver detects an unrecoverable error that requires
//! a reset to restart the communication. If possible, this function should trigger a hardware
//! reset of the module.
void PCMCIA_ResetModule(PCMCIA_HANDLE hSlot);
int  PCMCIA_ResetModule2(int nSlot);
//! Implementation of this function is required only for CI+.
//! It is used for the CI+ stack to query if a maximum data rate of 96Mbits is supported on
//! the transport stream interface.
//! @param hSlot Handle for the slot which is requested.
//! @return TRUE if 96Mbits is supported.
BOOL PCMCIA_SocketSupports96Mbit(PCMCIA_HANDLE hSlot);

int PCMCIA_PowerOnModule(int nSlot);
int PCMCIA_PowerOffModule(int nSlot);

#ifdef __cplusplus
}
#endif

#endif // __PCMCIA_H
