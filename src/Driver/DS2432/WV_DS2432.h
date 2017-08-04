/*****************************************************************************
* Copyright (c) 2010,WellAV Technologies Ltd. All rights reserved.
* File Name��       WV_DS2432.h
* Description:
* History:
*****************************************************************************/
#ifndef WV_DS2432_H_
#define WV_DS2432_H_

#include "appGlobal.h"

#define DS2432_DEBUG_ENABLE
#ifdef DS2432_DEBUG_ENABLE
#define WVDS2432_InfoPrint(x)           printf x
#define WVDS2432_ErrorPrint(x)          printf x
#define WVDS2432_DebugPrint(x)          printf x
#else
#define WVDS2432_InfoPrint(x)
#define WVDS2432_ErrorPrint(x)
#define WVDS2432_DebugPrint(x)
#endif

/* Constants */
#define CRC_POLY                0x8C          /* Polynomial used to generate 8-bit CRCs */
#define CRC_16_POLY             0xA001        /* Polynomial used to generate 16-bit CRCs */
#define ADDRESS_MSB             0x00          /* Upper byte of the DS2432 address - will always be 0 */
#define FAMILY_CODE             0x33          /* Family code of the DS2432 chip */

/* DS2432 ROM Command Codes */
#define READ_ROM_CMD            0x33          /* read rom command */
#define MATCH_ROM_CMD           0x55          /* match rom command */
#define SEARCH_ROM_CMD          0xF0          /* search rom command */
#define SKIP_ROM_CMD            0xCC          /* skip rom command */
#define OD_SKIP_ROM_CMD         0x3C          /* over drive skip rom command */
#define RESUME_CMD              0xA5          /* resume command */

/* DS2432 Memory and SHA Command Codes */
#define WRITE_SP_CMD            0x0F          /* write scratch pad command */
#define READ_SP_CMD             0xAA          /* read scratch pad command */
#define LOAD_SECRET_CMD         0x5A          /* load secret command */
#define COMPUTE_SECRET_CMD      0x33          /* compute new secret command */
#define COPY_SP_CMD             0x55          /* copy scratch pad data command */
#define READ_AUTH_PAGE_CMD      0xA5          /* read authorization page command */
#define READ_MEMORY_CMD         0xF0          /* read memory command */

/* data length limit */
#define PAGE_MIN                   0           /* the min page number */
#define PAGE_MAX                   3           /* the max page number */
#define AUTH_BYTES                 3           /* three authorization bytes */
#define PAGE_DATA_LENGTH           32          /* one eeprom page data length */
#define SCRATCHPAD_LENGTH          8           /* scratch pad data length --- 8 bytes */
#define SECRET_LENGTH              8           /* secret data length --- 8 bytes */
#define EIGHT_BYTE_RANDOM_DATA     8           /* 8 bytes random data */
#define MAX_DATA_LENGTH            151         /* max read memory data */
#define MAX_ADDRESS                0x97        /* max address */

/* DS2432 error type */
typedef enum
{
    ERR_OK,                                   //No errors
    ERR_COMM,                                 //Communication error
    ERR_NODEVICE,                             //No touch device present on touch pin
    ERR_RANGE,                                //Trying to access a byte not allowed by the function
    ERR_SECRET_READ,                          //Trying to read the secret
    ERR_DEVICE_ID,                            //No device chosen to write to
    ERR_SECRET_OW,                            //A byte of the function would be overwritten
    ERR_NO_WRITE,                             //The byte was not written successfully
    ERR_BAD_AUTH,                             //The authorization bytes did not match the chips
    ERR_COLLISION,                            //Data collision during communication with device
    ERR_PROTECTED,                            //Area being written to is write protected
    ERR_PAGE_RANGE                            //Given page number doesn't exist

}enmWVDS2432ErrorType;

/* DS2432 data struct */
typedef struct
{
    U8                m_ucarChipID[8];                         /* 8 bytes chip id */
    U8                m_ucarEepromPageData[32];                /* 32 bytes eeprom data read from chip */

}DS2432_DataStatus_t;

typedef struct
{
    int                 m_nEepromPage;                           /* Eeprom page number */
    U8                  m_ucPageAddress;                         /* Eeprom page address */
    U8                m_ucarSecret[8];                         /* Chip secret */

}BusMaster_DataStatus_t;

typedef unsigned char WVDS2432PinCfg;

/*************************************************
  Function:       WVDS2432Initialize
  Description:    Reset the ds2432 chip(s) by depriving
                  them of power
                  Initialize several global parameters
  Input:          A page number which the bus master
                  assigned,
                  A pointer to an array contains 8 bytes
                  secret from bus master,
                  The length of secret array(8 bytes),
                  The pio pin number.
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432Initialize(S32 s32Page, U8 *pu8Secret, U32 u32Len);


/*************************************************
  Function:       WVDS2432ReadRom
  Description:    To read the laser ROM contained the
                  family code and chip serial number
  Input:          A pointer to an array of U8 8 (must
                  contain at least 7 elements)
                  The length of the chip id and crc code
  Output:         A series bytes of Family ID Byte,
                  Serial Byte 0, Serial Byte 1, Serial
                  Byte 2, Serial Byte 3, Serial Byte 4,
                  Serial Byte 5, CRC Byte
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432ReadRom(U8 *pu8IDBuffer, U32 u32Len);

/*************************************************
  Function:       WVDS2432LoadSecret
  Description:    To load a secret into the DS2432 chip
  Input:          NONE
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432LoadSecret(void);

/*************************************************
  Function:       WVDS2432WriteAuthPage
  Description:    To write a verified page of data
  Input:          A pointer to an array of 32 bytes
                  data write to eeprom page
                  The length of the data will be written
                  to eeprom
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432WriteAuthPage(S32 s32EEPROMPage, U8 *pu8EepromWriteData, U32 u32Len);

/*************************************************
  Function:       WVDS2432ReadAuthPage
  Description:    To read a verified page of memory
                  after finish the authentication answer
  Input:          An array of 3 bytes random challenge
                  from bus master
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432ReadAuthPage(S32 s32EEPROMPage, U8 *pu8RandomChallenge, U32 u32Len);

/*************************************************
  Function:       WVDS2432ProtectSecret
  Description:    Write protects secret of the specified chip
  Input:          NONE
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432ProtectSecret(void);

/*************************************************
  Function:       WVDS2432ProtectPageZero
  Description:    Activate page 0 write protect function
  Input:          NONE
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432ProtectPageZero(void);

/*************************************************
  Function:       WVDS2432ProtectAllPages
  Description:    Activate all pages write protect function
  Input:          NONE
  Output:         NONE
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432ProtectAllPages(void);

/*************************************************
  Function:       WVDS2432ReadMemory
  Description:    Reads a number of user specified
                  bytes
  Input:          An int representing the address
                  of the byte to start reading from,
                  A pointer to an array where the
                  read bytes will be stored, and an
                  int holding the number of bytes to
                  be read in
                  The length of the data read
  Output:         The data read from chip
  Return:         emErrType
  Others:
  Create:         2010/10/25
*************************************************/
enmWVDS2432ErrorType WVDS2432ReadMemory(U8 u8ReadMemoryAddress, U8 *pu8DataBuffer, U32 u32Length);

#endif /*WV_DS2432_H_*/
