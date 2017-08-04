#ifndef UPGRADE_H
#define UPGRADE_H

#include "AppGlobal.h"
#include "boardType.h"

//TODO 板子的标识，升级制作软件添加，防止不同的升级文件可以互相升级
#define WV_UPGRADE_SYNC_FLAG    0x99999999
#define WV_MACHINE_TYPE         CMP_MACHINE_TYPE
#define WV_BOARD_TYPE           CMP_RECEIVER_C_S2

#define WV_UPGRADE_FILE         "/var/upgrade.tar"
#define WV_DEL_NET_FILE         "/var/delnetheader"


typedef struct
{ 
    // Struct Info
    U32 SyncFlag;
    U32 HeaderVersion; // Header Struct Version
    U32 HeaderLength; // Header Struct Length
    // Data Info 
    U32 BoardType; // ChipID 
    U32 DataMaxSize; // MaxSize
    U32 DataTimestamp; // Timestamp,build the resource file timestamp
    U32 DataLength; // Length (Firmware.len+Softare.len)
    U32 PlatformType; // Software DataLength 0:Altera 1:Xilinx
    U32 DataCRC32; // CRC32
    // Firmware
    U8 ucFirmwareVersion[20]; // Firmware Version
    U32 FirmwareLength; // Firmware Data Length
    // Software
    U8 ucSoftwareVersion[20]; //Software Version
    U32 SoftwareLength; // Software DataLength
    //hardware
    U32 ucHardwareVersion; 
    U32 MachineType; // MachineType 
    U32 u32SWFunction; //software function
    U32 u32OrgSize; //original file size
    U8 u8BeginSMPMainboardValid; //该子板版本是否有有效的对应的SMP起始主板版本.0:无,1:有
    U8 u8BeginDMPMainboardValid; //该子板版本是否有有效的对应的DMP起始主板版本.0:无,1:有
    U8 aru8Reserved[2]; // Reserved ... 
    U8 aru8BeginSMPMainboardSWVer[20]; //Software Version
    U8 aru8BeginDMPMainboardSWVer[20]; //Software Version
    U32 u32OrgFirmSize; // original firmware file size
    U32 aru32Reserved[26]; // Reserved ... 
    U32 HeaderCRC32; // Header CRC
} ImageHeader_t; 

wvErrCode upgrade_DelNetHeader(const U8 * u8UpgradeFileName);
wvErrCode upgrade_CheckUpgrade(const U8 * UpgradeFileName);
wvErrCode upgrade_UpgradeSystem(const U8 * u8UpgradeFileName);
wvErrCode upgrade_RecieveFile(const U8 * fileData, U32 u32Datalen);

#endif



