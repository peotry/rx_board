/**********************************************************************
* Copyright(c), HuiZhou WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:    SIProcess.
* Description: PSI/SI Proocess
*
* Version:     1.0.0
* Author:      Momouwei
* Date:        2017.02.22
*
**********************************************************************/
#ifndef _SI_PROCESS_
#define _SI_PROCESS_

#include "appGlobal.h"
#include "ts.h"

#define MAX_SERVICE_NUM_ONESECTION      (30) // for more than one section of SDT
#define MAX_SI_SECTION_NUM              (20)
#define MAX_SI_SECTION_LEN              (1024) // SI整个分段最大长度为1024
#define PACKET_LENGTH                   (188)
#define PACKET_LENGTH_WELLAV            (196)
#define PACKET_LENGTH_WELLAV_32BIT      (49)
#define PACKET_HEADER_LEN               (8)
#define PACKET_DATA_LENGTH              (184)         //Packet data length
#define PACKET_SECTION_LENGTH           (1024)

// PAT  Table  Structure
#define MAX_PROGRAMS_NUMBER_IN_PAT 200
typedef struct _ProgramInfoInPAT
{
    U16    u16ServiceId;
    U16    u16PMTPid;
}ProgramInfoInPat_t;

typedef struct _PATTable
{
    U16    u16TransportStreamID; // 流id
    U16    u16ProgramCount; // 节目数
    U8     u8VersionNum;
    ProgramInfoInPat_t arstProgramInfo[MAX_PROGRAMS_NUMBER_IN_PAT];
}PATTable_t;

// PMT Table Structure
#define MAX_CASYS_NUM_IN_PMT      20
#define MAX_ECM_NUM_IN_PMT        20
#define MAX_PROGRAM_INFO_LEN      350
#define MAX_ES_INFO_LEN           300
#define MAX_ES_NUM_IN_PMT         200

typedef struct _PmtCAInfo
{
    U16   u16CaSysID; //  CA信息
    U16   u16EcmPid; //  CA信息
}PmtCAInfo_t;

typedef struct _PMTESInfo
{
    U8           u8Type;
    U16          u16Pid;
    U8           u8CaSysNum;  // ES里的Ca信息,仅解析使用
    PmtCAInfo_t  arstCaInfo[MAX_CASYS_NUM_IN_PMT];  //  CA信息，仅解析使用
    U16          u16ESInfoLen;
    U8           aru8ESInfo[MAX_ES_INFO_LEN];
}PMTESInfo_t;

typedef struct  _PMTTable
{
    U16          u16ServiceId;
    U16          u16PCRPID;
    U8           u8VersionNum;
    U8           u8CaSysNum; // program里的Ca信息,仅解析使用
    PmtCAInfo_t  arstCaInfo[MAX_CASYS_NUM_IN_PMT];  //  CA信息，仅解析使用
    U16          u16ProgInfoLen;  // program_info_length
    U8           aru8ProgInfo[MAX_PROGRAM_INFO_LEN];  // program_descriptor
    U8           u8LastSectionNum;
    U8           aru8Rvd[2];  // 保留字段
    U8           u8EsNum;
    PMTESInfo_t  arstPMTESInfo[MAX_ES_NUM_IN_PMT];

}PMTTable_t;

//  CAT Table Structure
#define MAX_CASYS_NUM_IN_CAT      32

typedef struct _CatCAInfo
{
    U16   u16CaSysID; //  CA信息
    U16   u16EmmPid;  //  CA信息
}CatCAInfo_t;

typedef struct _CATTable
{
    U8           u8VersionNum;
    U8           u8Rvd;  // 保留字段
    U16          u16CaSysNum;
    CatCAInfo_t  arstCaInfo[MAX_CASYS_NUM_IN_CAT];
}CATTable_t;

//  SDT Table Structure
#define MAX_PROGRAMS_NUM_IN_SDT 100
#define MAX_SDT_NAME_LEN        32
#define MAX_SDT_OTHER_INFO_LEN  256

typedef struct _SDTProgramInfo
{
    U16  u16ServiceID;
    U8   u8ServiceType;
    U8   u8FreeCAMode;
    U8   u8RunningStatus;
    U8   u8EITScheduleFlag;
    U8   u8EITPresentFollowingFlag;
    U8   aru8Rvd[3];  // 保留字段
    U8   u8ProviderNameLen;
    U8   u8ServiceNameLen;
    U8   aru8ProviderName[MAX_SDT_NAME_LEN];
    U8   aru8ServiceName[MAX_SDT_NAME_LEN];
    U16  u16OtherInfoLen;
    U8   aru8OtherInfo[MAX_SDT_OTHER_INFO_LEN];

}SDTProgramInfo_t;

typedef struct _SDTable
{
    U16  u16OriginalNetworkId;
    U16  u16TransportStreamID;
    U8   u8VersionNum;
    U8   u8LastSectionNum;
    U16  u16ServiceNum;
    U16  u16SectionLen;
    SDTProgramInfo_t arstProgInfo[MAX_PROGRAMS_NUM_IN_SDT];
}SDTTable_t;

//  NIT Table Structure
#define  MIN_NIT_SECTION_LENGTH    (16)
#define  MAX_PROGRAMS_NUM_IN_TP    100
#define  MAX_TP_STREAMS            50

typedef struct   _ServiceStruct
{
    U16  u16ServiceID;
    U8   u8ServiceType;
    U8   u8Rvd; // 保留字段
}ServiceStruct_t;

typedef struct _TPStruct
{
    U32  u32Frequency;
    U32  u32SymbolRate;
    U16  u16Modulation;
    U16  u16TPID;
    U16  u16ServiceNum;
    ServiceStruct_t  arstServiceList[MAX_PROGRAMS_NUM_IN_TP];
}TPStruct_t;


typedef struct _NITStruct
{
    U16  u16NetworkID;
    U16  u16TPNumber;
    U16  u16SectionLen;
    U8   u8LastSectionNum;
    ServiceStruct_t  arstTPData[MAX_TP_STREAMS];
}NITTable_t;

// BAT Table Structure
#define MIN_BAT_SECTION_LENGTH     (16)
#define MAX_BOUQUET_INFO_LENGHT   (1000)
#define MAX_TS_NUMBER_INT_BAT     (300)
#define MAX_TS_DESCRIPTORS_LENGHT (300)

typedef struct _TSInfoStruct
{
    U16 u16TSID;
    U16 u16OriginalNetId;
    U16 u16DescriptorsLen;
    U8  aru8Descriptors[MAX_TS_DESCRIPTORS_LENGHT];
}TSInfoStruct_t;

typedef struct _BATStruct
{
    U8  u8VersionNum;
    U8  u8LastSectionNum;
    U16 u16SectionLen;
    U16 u16BouquetId;
    U16 u16BouquetInfoLen;
    U8  aru8BouquetInfo[MAX_BOUQUET_INFO_LENGHT];
    U16 u16Rvd;
    U16 u16TransportStreamNum;
    TSInfoStruct_t arstTSInfo[MAX_TS_NUMBER_INT_BAT];
}BATTable_t;

/*****************************************************************************
  Function:     SIP_ParsePAT
  Description:  从Section缓冲区中解析出PAT表
  Input:        pu8SectionBuf      －  Section存放缓冲区
                u16SectionBuffLen  －  Section缓冲区长度
  Output:       *pstPatTabl        － PAT表信息
  Return:       SUCCESS or FAIL
  Others:       输入section从table id开始，不包含pointer_field
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode SIP_ParsePAT(U8 *pu8SectionBuf, U16 u16SectionBuffLen, PATTable_t *pstPatTable);

/*****************************************************************************
  Function:     SIP_ParsePMT
  Description:  从Section缓冲区中解析出PMT表
  Input:        pu8SectionBuf      －  Section存放缓冲区
                u16SectionBuffLen  －  Section缓冲区长度
                U8 Standard        －  标准
  Output:       pstPmtTable        －  PMT表
  Return:       SMP_SUCCESS or FAIL
  Others:       输入section从table id开始，不包含pointer_field
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode SIP_ParsePMT(U8 *pu8SectionBuf, U16 u16SectionBuffLen, PMTTable_t *pstPmtTable);

/*****************************************************************************
  Function:     SIP_ParseCAT
  Description:  从Section缓冲区中解析出CAT表
  Input:        pu8SectionBuf      －  Section存放缓冲区
                u16SectionBuffLen  －  Section缓冲区长度
  Output:       pstCatTable        －  CAT表
  Return:       SUCCESS or FAIL
  Others:       输入section从table id开始，不包含pointer_field
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode SIP_ParseCAT(U8 *pu8SectionBuf, U16 u16SectionBuffLen, CATTable_t  *pstCatTable);

/*****************************************************************************
  Function:     SIP_ParseSDT
  Description:  从Section缓冲区中解析出SDT表
  Input:        pu8SectionBuf      －  Section存放缓冲区
                u16SectionBuffLen  －  Section缓冲区长度
  Output:       pstSdtTable        －  SDT表
  Return:       SMP_SUCCESS or FAIL
  Others:       输入section从table id开始，不包含pointer_field
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode SIP_ParseSDT(U8 *pu8SectionBuf, U16 u16SectionBuffLen, SDTTable_t *pstSdtTable);

/*****************************************************************************
  Function:     SIP_ParseNIT
  Description:  从Section缓冲区中解析出NIT表
  Input:        pu8SectionBuf      －  Section存放缓冲区
                u16SectionBuffLen  －  Section缓冲区长度
  Output:       pstNitTable        －  NIT表
  Return:       SMP_SUCCESS or FAIL
  Others:       输入section从table id开始，不包含pointer_field
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode SIP_ParseNIT(U8 *pu8SectionBuf, U16 u16SectionBuffLen, NITTable_t *pstNitTable);

/*****************************************************************************
  Function:     SIP_ParseBAT
  Description:  从Section缓冲区中解析出BAT表
  Input:        pu8SectionBuf      －  Section存放缓冲区
                u16SectionBuffLen  －  Section缓冲区长度
  Output:       pstBATTable        －  BAT表
  Return:       SUCCESS or FAIL
  Others:       输入section从table id开始，不包含pointer_field
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode SIP_ParseBAT(U8 *pu8SectionBuf, U16 u16SectionBuffLen, BATTable_t *pstBATTable);

/*****************************************************************************
  Function:     FindCADescriptor
  Description:  find CA descriptor according to tag
  Input:        pu8DesBuf        --- input data
                u16DesLength     --- length of pDesBuf
  Output:       pu16CASystemID   --- output system ID
                pu16CAPID        --- output EMM PID or ECM PID
  Return:       u8CANum          --- Number of CA
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
U8 SIP_FindCADescriptor(U8* pu8DesBuf, U16 u16DesLength, U16* pu16CASystemID, U16* pu16CAPID);

wvErrCode SIP_ConstructPAT(const PATTable_t *pstPatTable, U16 u16BufLen, U8 *pu8SectionBuf, U16 *pu16SectionLen);
wvErrCode SIP_ConstructPMT(const PMTTable_t *pstPmtTable, U16 u16BufLen, U8 *pu8SectionBuf, U16 *pu16SectionLen);
wvErrCode SIP_ConstructCAT(const CATTable_t *pstCatTable, U16 u16BufLen, U8 *pu8SectionBuf, U16 *pu16SectionLen);
wvErrCode SIP_ConstructSDT(const SDTTable_t *pstSdtTable, U16 u16BufLen, U8 *pu8SectionBuf, U16 *pu16SectionLen);
wvErrCode SIP_ConstructOtherSDT(const SDTTable_t *pstSdtTable, U16 u16BufLen, U8 *pu8SectionBuf, U16 *pu16SectionLen);
wvErrCode SIP_ConstructBAT(const BATTable_t *pstBATTable, U16 u16BufLen, U8 *pu8SectionBuf, U16 *pu16SectionLen);
void SIP_SendSIServer();
void SIP_Init(void);

#endif

