/*****************************************************************************
 * Copyright(c), HuiZhou WELLAV Technology Co.,Ltd.
 * All rights reserved.
 *
 * FileName    ：TSProcess.h
 * Description ：MuxProcess
 *
 * Version ：1.0.0
 * Author  ：Momouwei
 * Date    ：2017-02-18
 * Record  : Create
 ****************************************************************************/
 #ifndef _TSPROCESS_H_
 #define _TSPROCESS_H_

#include "appGlobal.h"
#include "ts.h"
#include "SIProcess.h"

#define TS_STRUCT_VERSION          (0)

#define TSP_FLAG_INVALID           (0)
#define TSP_FLAG_VALID             (1)

#define INVALID_PID_VALUE          (0xFFFF)
#define INDEX_INVALID_VALUE        (0xFFFF)
#define U8_INVALID_VALUE           (0xFF)
#define U16_INVALID_VALUE          (0xFFFF)
#define U32_INVALID_VALUE          (0xFFFFFFFF)
#define INVALID_CAM_INDEX          (0xFF)

#define MAX_OTHERPID_NUM           (200)
#define MAX_TS_SCANNER_NUM         (4)
#define MAX_INPUT_CHANNEL_NUM      (4)
#define MAX_OUTPUT_CHANNEL_NUM     (256)
#define MAX_INPUT_LUT_ENTRY_NUM    (2048)
#define MAX_OUTPUT_LUT_ENTRY_NUM   (2048)
#define MAX_OUTPUT_BYPASS_LUT_ENTRY_NUM   (256)
#define MAX_DEST                   (4)

#define MAX_INPUT_LUT_NUM          (4)

#define MAX_DESCRIPTOR_LENGTH      (188)
#define MAX_SDT_NAME_LENGTH        (32)
#define MAX_STREAMTYPE_NAME_LENGTH (32)
#define ONE_SECTION_LENGTH         (1024)

#define INIT_TS_NUM                (4)
#define INIT_PROG_NUM              (100)
#define INIT_PID_NUM               (600)
#define INIT_TABLE_NUM             (50)
#define INIT_DESCRIPTOR_NUM        (50)
#define INIT_OUTPUT_TS_NUM         (2)
#define INIT_OUTPUT_PROG_NUM       (50)
#define INIT_OUTPUT_PID_NUM        (300)
#define INIT_OUTPUT_TABLE_NUM      (1)
#define INIT_OUTPUT_DESCRIPTOR_NUM (1)

#define INIT_TEMP_TS_NUM           (1)
#define INIT_TEMP_PROG_NUM         (40)
#define INIT_TEMP_PID_NUM          (400)
#define INIT_TEMP_TABLE_NUM        (20)
#define INIT_TEMP_DESCRIPTOR_NUM   (20)

#define MAX_PID_NUM_IN_ONE_TS      (500)

#define MAX_SERVICEID              (0xFFFF)
#define MIN_CONFLICT_PID_ALLOCATION (51)

#define MAX_CONSTRUCT_SI_BUFF      (10240)

enum
{
    VIDEO_PID,
    AUDIO_PID,
    PCR_PID,
    EMM_PID,
    PSECM_PID,
    ESECM_PID,
    OTHER_PID,
    PAT_TABLE_PID,
    PMT_TABLE_PID,
    SDT_TABLE_PID,
    CAT_TABLE_PID,
    PRIVATE_DATA_PID,
};

enum
{
    PID_TYPE_UNKNOWN,
    PID_TYPE_PMTPID,
    PID_TYPE_PCRPID,
    PID_TYPE_ESPID,
    PID_TYPE_EMMPID,
    PID_TYPE_PS_ECMPID,
    PID_TYPE_ES_ECMPID,
    PID_TYPE_OTHERPID,
};

typedef struct TSDescriptor
{
    U32    u32StructVersion; //数据结构版本号，以便数据结构升级
    U32    u32Length; //输入板向输出板传数据内容时，需要带该参数

    U8     u8ValidFlag;
    U8     u8SlotID;
    U16    u16ChannelID;
    U16    u16ProgramIndex;

    //以1024为一个单位来存储描述子的内容
    U8     aru8DescriptorInfo[ONE_SECTION_LENGTH];
    U32    aru32Res[32];
} TSDescriptor;

typedef struct TSTable
{
    U32    u32StructVersion; //数据结构版本号，以便数据结构升级
    U32    u32Length; //输入板向输出板传数据内容时，需要带该参数

    U8     u8ValidFlag;
    U8     u8SlotID;
    U16    u16ChannelID;
    
    U16    u16Index;
    U16    u16NextIndex;
    U16    u16TSIndex;

    U8     u8TableType; //BAT/NIT/OtherSDT/TDT/TOT等
    U16    u16PID;
    U16    u16TableID;
    U16    u16SectionNumber;
    U16    u16LastSectionNumber;

    //以1024为一个单位来存储表的内容
    U16    u16TableInfoLen;
    U8     aru8TableInfo[ONE_SECTION_LENGTH];
    U32    aru32Res[32];
} TSTable;

typedef struct Input_TSPID
{
    U32    u32StructVersion; //数据结构版本号，以便数据结构升级

    U8     u8ValidFlag;
    U8     u8SlotID;
    U16    u16ChannelID;

    U16    u16Index;
    U16    u16NextIndex;
    U16    u16TSIndex;
    U16    u16ProgramIndex;
    U16    u16PIDIndex;
    U16    u16ESECMPIDNumber;
    U16    u16ESECMPIDIndex;
    U16    u16Res;

    U8     u8PIDType; //PCR/Audio/Video/EMM/ECM/OtherPID等
    U8     u8StreamType;
    U8     u8FunctionFlag; //Descr/TC/Scr等标识
    U8     u8CAMIndex;
    U16    u16PID;
    U16    u16CASID;

    U8     aru8DestSlot[MAX_DEST]; //复用去向，目前最多支持4个
    U32    aru32DestChannel[MAX_DEST][8]; //复用目的通道，最多可以支持256个去向

    U8     aru8ESInfo[MAX_DESCRIPTOR_LENGTH];
    U16    u16ESInfoLength;
    U16    u16ESInfoDescriptorIndex; 

    //主备标识及主备组号
    U16    u16BackupPIDIndex;
    U16    u16BackupGroup;
    U8     u8BackupFlag;
    U8     aru8Res;
    U16    u16NewPID; //for CI MUX

    U32    aru32Res2[32];
} Input_TSPID;

typedef struct Output_TSPID
{
    U32     u32StructVersion; //数据结构版本号，以便数据结构升级
    U32     u32Length; //输入板向输出板传数据内容时，需要带该参数

    U8      u8ValidFlag;
    U8      u8SourceSlotID;
    U16     u16SourceChannelID;
    U8      u8SlotID;
    U8      u8Res; //   Byte alignment
    U16     u16ChannelID;

    U32     u32TSIndex;
    U32     u32ProgramIndex;
    U32     u32PIDIndex; ///
    U16     u16ProgramID; //查找输入PID所在的输入节目号
    U16     u16PIDID;//查找输入ESECMPID所在的输入PID号
    U32     u32NextPIDIndex;
    U16     u16ESECMPIDNumber;
    U16     u16ESECMPIDIndex;

    U8      u8PIDType; //PCR/Audio/Video/EMM/ECM/OtherPID等
    U8      u8StreamType; ///
    U16     u16OldPID;
    U16     u16PID;
    U16     u16CASID;

    U16     u16ESInfoLength;
    U16     u16SrcStreamID;
    U8      aru8ESInfo[MAX_DESCRIPTOR_LENGTH];
    U16     u16ESInfoDescriptorIndex;
    U16     u16SrcCIPID;
    U16     u16SrcCIStreamID;
    U16     u16Res2; //   Byte alignment

    U32     u32Res[31];
}Output_TSPID; 


typedef struct Input_TSProgram
{
    U32    u32StructVersion; //数据结构版本号，以便数据结构升级

    U8     u8ValidFlag;
    U8     u8SlotID;
    U16    u16ChannelID;

    U16    u16Index;
    U16    u16NextIndex;
    U16    u16TSIndex;
    U16    u16PSECMPIDNumber;
    U16    u16PSECMPIDIndex;
    U16    u16ServiceID;
    U16    u16PMTPID;
    U16    u16PCRPID;
    U16    u16PCRIndex;
    U16    u16ESNumber;
    U16    u16PIDIndex;
    U8     u8FunctionFlag; //Descr/TC/Scr等标识
    U8     u8CAMIndex;

    // From SDT
    
    U8     aru8ServiceName[MAX_SDT_NAME_LENGTH];
    U8     aru8ServiceProvider[MAX_SDT_NAME_LENGTH];
    U8     u8ServiceNameLen;
    U8     u8ServiceProviderLen;  
    U8     u8ServiceType;
    U8     u8RunningStatus;
    U8     u8FreeCAMode;
    U8     u8EITScheduleFlag;
    U8     u8EITPresentFollowingFlag;
    
    U8     u8VersionNumber;

    U8     aru8ProgramInfo[MAX_DESCRIPTOR_LENGTH];
    U8     aru8SDTInfo[MAX_DESCRIPTOR_LENGTH];
    U16    u16ProgramInfoLength;
    U16    u16SDTInfoLength;
    
    U16    u16ProgramInfoDescriptorIndex;
    U16    u16SDTInfoDescriptorIndex;

    //used for ATSC
    U16    u16MajorChannelNum;
    U16    u16MinorChannelNum;
    U32    u32CarrierFrequency;
    U16    u16ChannelTSID;
    U16    u16SourceId;
    U8     u8ModulationMode;
    U8     u8VCTType; // TVCT OR  CVCT
    U16    u16Res1;
    
    //主备标识及主备组号
    U16    u16BackupProgramIndex;
    U16    u16BackupGroup;
    U8     u8BackupFlag;
    U8     aru8Res[3];

    U8     aru8DestSlot[MAX_DEST];
    U32    aru32DestChannel[MAX_DEST][8];

    U16    u16NewPMTPID;//for CI MUX
    U16    u16NewPCRPID;//for CI MUX
    U16    u16NewServiceID;//for CI MUX
    U16    u16Res2;
    U32    aru32Res[98];
} Input_TSProgram;

typedef struct Output_TSProgram
{
    U32     u32StructVersion; //数据结构版本号，以便数据结构升级
    U32     u32Length; //输入板向输出板传数据内容时，需要带该参数

    U8      u8ValidFlag;
    U8      u8SourceSlotID;
    U16     u16SourceChannelID;
    U16     u16ChannelID;
    U8      u8SlotID;
    U8      u8PMTVersionNumber;///

    U16     u16TSIndex;
    U16     u16ProgramIndex;  
    U16     u16ProgramID; //输入节目在输入流中的ID号
    U16     u16NextProgramIndex;
    U16     u16PSECMPIDNumber;
    U16     u16PSECMPIDIndex;
    
    U16     u16OldServiceID;
    U16     u16OldPCRPID;
    U16     u16OldPMTPID;
    U16     u16ServiceID;
    U16     u16PCRPID;
    U16     u16PMTPID;
    U16     u16ESNumber;
    U16     u16PCRIndex;
    U16     u16PIDIndex;

    // From SDT
    U8      u8ServiceNameLen;
    U8      u8ServiceProviderLen;
    U8      aru8ServiceName[MAX_SDT_NAME_LENGTH];
    U8      aru8ServiceProvider[MAX_SDT_NAME_LENGTH];
    U8      u8ServiceType;
    U8      u8RunningStatus; 
    U8      u8FreeCAMode; 
    U8      u8EITScheduleFlag;
    U8      u8EITPresentFollowingFlag;
    
    U8      u8PMTChangeFlag;

    U16     u16ProgramInfoLength;
    U8      aru8ProgramInfo[MAX_DESCRIPTOR_LENGTH];
    U16     u16SDTInfoLength;
    U8      u8CAMIndex;
    U8      u8CIFlag;
    U8      aru8SDTInfo[MAX_DESCRIPTOR_LENGTH];
    U16     u16ProgramInfoDescriptorIndex;         
    U16     u16SDTInfoDescriptorIndex;

    //used for ATSC
    U16     u16MajorChannelNum;
    U16     u16MinorChannelNum;
    U32     u32CarrierFrequency;
    U16     u16ChannelTSID;
    U16     u16SourceId;
    U8      u8ModulationMode;
    U8      u8VCTType; // TVCT OR  CVCT
    U16     u16Res2; //   Byte alignment

    U32     u32Res[100];
}Output_TSProgram;  //sizeof = 

typedef struct Input_TSStream
{
    U32    u32StructVersion; //数据结构版本号，以便数据结构升级

    U8     u8ValidFlag;
    U8     u8SlotID;
    U16    u16ChannelID;

    U16    u16Index;
    U16    u16NetworkID; //from NIT table
    U16    u16TransportStreamID; //from PAT, SDT or EIT
    U16    u16OriginalNetworkID; //from SDT, NIT or EIT
    
    U16    u16ProgramNum;
    U16    u16ProgramIndex;
    U16    u16OtherPIDNum;
    U16    u16OtherPIDIndex;
    U16    u16EMMPIDNumber;
    U16    u16EMMPIDIndex;

    U16    u16NITIndex;
    U16    u16BATIndex;
    U16    u16TDTIndex;
    U16    u16TOTIndex;
    U16    u16OtherNITIndex;
    U16    u16OtherSDTIndex;

    U8     aru8DestSlot[MAX_DEST];
    U32    aru32DestChannel[MAX_DEST][8];
    U32    aru32BypassFlag[MAX_DEST][8];

    U16    u16BackupTSIndex;
    U16    u16BackupGroup; //主备组号
    U8     u8BackupFlag; //主备标识
    U8     aru8Res[3];

    U32    aru32Res[100];
} Input_TSStream;

typedef struct Output_TSStream
{
    U32     u32StructVersion; //数据结构版本号，以便数据结构升级
    U32     u32Length; //输入板向输出板传数据内容时，需要带该参数

    U16     u16NetworkID; // from input or user
    U16     u16TransportStreamID; // from input or user
    U16     u16OriginalNetworkID; //  from input or user

    U8      u8ValidFlag;
    U8      u8SlotID;
    U16     u16ChannelID;

    U16     u16ProgramNum; // number of programs on this stream
    U16     u16ProgramIndex;
    U16     u16OtherPIDNum;
    U16     u16OtherPIDIndex;
    U16     u16EMMPIDNumber;
    U16     u16EMMPIDIndex;
    U8      u8VersionNumber;
    U8      u8ChangeFlag;
    U8      u8BypassFlag;
    U8      u8SrcSlotID;
    U16     u16SrcChannelID;

    U16     u16NITIndex;
    U16     u16BATIndex;
    U16     u16TDTIndex;
    U16     u16TOTIndex;
    U16     u16OtherNITIndex; 
    U16     u16OtherSDTIndex;

    U32     aru32Res[100];
} Output_TSStream;

typedef struct TSInfo
{
    Input_TSStream      *pInputTS;
    Input_TSProgram     *pInputProgram;
    Input_TSPID         *pInputPID;
    TSTable             *pInputTable;
    TSDescriptor        *pInputDescriptor;
    U32                 u32InputTSNumber;
    U32                 u32InputProgramNumber;
    U32                 u32InputPIDNumber;
    U32                 u32InputTableNumber;
    U32                 u32InputDescriptorNumber;

    Output_TSStream     *pOutputTS;
    Output_TSProgram    *pOutputProgram;
    Output_TSPID        *pOutputPID;
    TSTable             *pOutputTable;
    TSDescriptor        *pOutputDescriptor;
    U32                 u32OutputTSNumber;
    U32                 u32OutputProgramNumber;
    U32                 u32OutputPIDNumber;
    U32                 u32OutputTableNumber;
    U32                 u32OutputDescriptorNumber;

} TSInfo;

typedef struct TSIndex
{
    U32     u32StructVersion; //数据结构版本号，以便数据结构升级
    U32     u32TSNumber; //TS数量
    U32     u32OutputTSNumber; //TS数据结构存储在flash上的基地址
    U32     u32ProgramNumber; //节目数量
    U32     u32OutputProgramNumber; //节目数据结构存储在flash上的基地址
    U32     u32PIDNumber; //PID数量
    U32     u32OutputPIDNumber; //PID数据结构存储在flash上的基地址
    U32     u32TableNumber; //特殊表数量(BAT/NIT/OtherSDT)
    U32     u32OutputTableNumber; //表数据结构存储在flash上的基地址
    U32     u32DescriptorNumber; //超长描述子数量
    U32     u32OutputDescriptorNumber; //描述子结构存储在flash上的基地址

    U32     aru32Res[53];
} TSIndex;

typedef struct TSInfoInitNum
{
    U32     u32TSNumber;
    U32     u32ProgramNumber;
    U32     u32PIDNumber;
    U32     u32TableNumber;
    U32     u32DescriptorNumber;

    U32     u32OutputTSNumber;
    U32     u32OutputProgramNumber;
    U32     u32OutputPIDNumber;
    U32     u32OutputTableNumber;
    U32     u32OutputDescriptorNumber;

} TSInfoInitNum;

typedef struct PIDINFO
{
    U16 aru16PIDList[MAX_PID_NUMBER];
    U16 u16PIDNum;
}PIDINFO;

#define MAX_INDEX_LIST_NUMBER (300)
typedef struct IndexInfoList
{
    U16 aru16IndexList[MAX_INDEX_LIST_NUMBER];
    U16 u16IndexListNum;
} IndexInfoList;

#define MAX_BOARD_NAME_LEN   (20)
typedef struct DEST_INFO
{
    U32 aru32Channel[8];
    U32 aru32BypassFlag[8];
    U8 aru8BoardName[MAX_BOARD_NAME_LEN];
    U8 u8Slot;
} DestInfo;

typedef struct DEST
{
    DestInfo arstDestSlotInfo[MAX_DEST];
    U8 u8DestSlotNum;
} Dest;

typedef struct Delete_TSProgram
{
    U8      u8SourceSlotID;
    U8      u8Res;
    U16     u16SourceChannelID;
    U16     u16ChannelID;
    //U16     u16OldServiceID;
    U16     u16ProgramID;
}Delete_TSProgram;//删除流去向时用

typedef struct Delete_TSStream
{
    U8      u8SourceSlotID;
    U8      u8Res;
    U16     u16Res;
    U16     u16SourceChannelID;
    U16     u16ChannelID;
}Delete_TSStream;//删除节目去向时用

typedef struct DescrambleProgInfo
{
    U16 u16ServiceID;
    U16 u16Channel;
    U8 u8CAMIndex;
    U8 u8Res[3];
}DescrambleProgInfo;

typedef struct PIDBitrateInfo
{
    U32     u32Bitrate;
    U16     u16SamePIDNum;
    U16     u16PID;
    U16     u16ServiceID;
    U8      u8PIDType;
    U8      u8CcNum;
    U8      aru8ServiceName[MAX_SDT_NAME_LENGTH];    
    U8      u8ServiceNameLen;
}PIDBitrateInfo;

typedef struct TSBitrateInfo
{
    U32     u32TotalBitrate;
    U16     u16PIDNum;
    U8      u8ValidFlag;
    U8      u8Res;
    PIDBitrateInfo arstPIDBitrateInfo[MAX_PID_NUM_IN_ONE_TS];
}TSBitrateInfo;

typedef struct PIDUseInfo
{
    U8 u8UseFlag;
    U8 u8SrcSlot;
    U16 u16SrcChannel;
    U16 u16OldPID;
}PIDUseInfo;

typedef struct INPUTLutEntry
{
    U16 u16SrcStream;
    U16 u16SrcPID;
}INPUTLutEntry;

typedef struct OUTPUTLutEntry
{
    U8 u8Num;
    U8 u8SrcSlot;
    U16 u16SrcStream;
    U16 u16SrcPID;
    U16 u16DestStream;
    U16 u16DestPID;
    U16 u16Res;
}OUTPUTLUTEntry;

int TSP_ScanTSStart(void);
wvErrCode TSP_InitTempTSInfo(void);
void PrintInputTS(U16 u16InTSIndex, TSInfo *pstParamTS);

#endif

