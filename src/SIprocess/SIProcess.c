/**********************************************************************
* Copyright(c), HuiZhou WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:    TSConstruct.c
* Description: MuxProcess
*
* Version:     1.0.0
* Author:      Momouwei
* Date:        2017.02.22
*
**********************************************************************/
#include "SIProcess.h"
#include "WV_log.h"
#include "tools.h"
#include "timer.h"
#include "SiSend.h"

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
wvErrCode SIP_ParsePAT(U8 *pu8SectionBuf, U16 u16SectionBuffLen, PATTable_t *pstPatTable)
{
    U16 i = 0;
    U16 u16SectLength   = 0;
    U16 u16ServiceId    = 0;
    U16 u16PMTPid       = 0;
    U16 u16ProgramCount = 0;
    U16 u16Index        = 0;

    U8  *pu8Temp = pu8SectionBuf;
    ProgramInfoInPat_t *pProgInPat = NULL;

    if ((NULL == pu8SectionBuf) || (NULL == pstPatTable))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]Input Error:pu8SectionBuf[%p],pstPatTable[%p]\r\n",
            __FUNCTION__, __LINE__, pu8SectionBuf, pstPatTable);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    // init
    memset(pstPatTable, 0, sizeof(PATTable_t));

    /***************************************************
    Program association section (PAT)
    Syntax No. of bits
    program_association_section( ) {
        table_id                     8
        section_syntax_indicator     1
        '0'                          1
        reserved                     2
        section_length               12
        transport_stream_id          16
        reserved                     2
        version_number               5
        current_next_indicator       1
        section_number               8
        last_section_number          8
        for (j=0;j< N;j++) {
            program_number        16
            reserved              3
            if (program_number=='0' ) {
                network_PID       13
            }
            else {
                program_map_PID   13
            }
        }
        CRC_32 32
    }
    ***************************************************/
    u16SectLength = ((pu8Temp[1]<<8) | pu8Temp[2]) & 0x0fff;

    // 长度校验u16SectLength + 3 表示整个PAT表长度
    if ((u16SectionBuffLen < (u16SectLength + 3))
       || (MAX_SI_SECTION_LEN < (u16SectLength + 3)))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]error,u16SectionBuffLen[%u],u16SectLength[%u]\r\n",
            __FUNCTION__, __LINE__, u16SectionBuffLen, u16SectLength);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    pstPatTable->u16TransportStreamID = (pu8Temp[3]<<8) | pu8Temp[4];
    pstPatTable->u16ProgramCount      = 0;
    pstPatTable->u8VersionNum         = (pu8Temp[5]>>1) & 0x1f;

    /* 节目长度为section_length从transport_stream_id开始计数，减去4字节CRC和5字节信息
       除以4是因为每个节目信息包括4个字节program_number(16)reserved(3)PID(13) */
    u16ProgramCount = (u16SectLength-(5+4))/4;

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SIP,
        "[%s:%d]SecLen[%u],TSID[%u],ProgNum[%u],VerNum[%u],SecNum[%u],LastSecNum[%u]\r\n",
        __FUNCTION__,
        __LINE__,
        u16SectLength,
        pstPatTable->u16TransportStreamID,
        u16ProgramCount,
        pstPatTable->u8VersionNum,
        pu8Temp[6],
        pu8Temp[7]);

    // 偏移到节目信息起始位置 即第一个program_number(16)起始位置
    u16Index = 8;

    // 解析节目信息
    for (i = 0; i < u16ProgramCount; i++)
    {
        u16ServiceId = (pu8Temp[u16Index]<<8)|(pu8Temp[u16Index+1]);

        u16PMTPid = ((pu8Temp[u16Index+2]<<8) | pu8Temp[u16Index+3]) & 0x1fff;

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SIP,
                "Prog[%u],u16ServiceId[%u],u16PMTPid[%u]\r\n",
                i,
                u16ServiceId,
                u16PMTPid);

        // 非network id
        if (0 != u16ServiceId)
        {
            if (MAX_PROGRAMS_NUMBER_IN_PAT <= pstPatTable->u16ProgramCount)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                    "[%s:%d]error,Input program number is more than MAX program number\r\n",
                    __FUNCTION__, __LINE__);
                return WV_ERR_SIP_ERROR_BAD_PARAM;
            }

            pProgInPat = &pstPatTable->arstProgramInfo[pstPatTable->u16ProgramCount];
            pProgInPat->u16PMTPid    = u16PMTPid;
            pProgInPat->u16ServiceId = u16ServiceId;

            pstPatTable->u16ProgramCount++;
        }

        // 4 表示(program_number(16)+reserved(3)+program_map_PID(13))
        u16Index += 4;
    }

    #if 0
    if (0 != DBG_GetDebugFlag(DEBUG_MODULE_SIPROCESS))
    {
        SIP_PATTablePrint(pstPatTable);
    }
    #endif

    return WV_SUCCESS;
}

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
wvErrCode SIP_ParsePMT(U8 *pu8SectionBuf, U16 u16SectionBuffLen, PMTTable_t *pstPmtTable)
{
    U8  i                    = 0;
    U8  u8CANum              = 0;
    U8  u8StreamType         = 0;
    U8  u8VerNum             = 0;
    U16 u16SectLength        = 0;
    U16 u16ProgramNum        = 0;
    U16 u16PCRPID            = 0;
    U16 u16ProgramInfoLength = 0;
    U16 u16ESPID             = 0;
    U16 u16ESInfoLength      = 0;
    U16 u16Index             = 0;
    U16 u16EsInfoEndIdx      = 0;
    U16 aru16CASystemID[MAX_CASYS_NUM_IN_PMT];
    U16 aru16ECMPID[MAX_ECM_NUM_IN_PMT];

    U8  *pu8Temp  = pu8SectionBuf;
    PMTESInfo_t *pstPMTESInfo = NULL;

    // check input param
    if ((NULL == pu8SectionBuf) || (NULL == pstPmtTable))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]Input Error:pu8SectionBuf[%p],pstPmtTable[%p]\r\n",
            __FUNCTION__, __LINE__, pu8SectionBuf, pstPmtTable);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    // init
    memset(pstPmtTable, 0, sizeof(PMTTable_t));

    /***************************************************
    Transport Stream program map section (PMT)
    Syntax No. of bits
    TS_program_map_section( ) {
        table_id                   8
        section_syntax_indicator   1
        '0'                        1
        reserved                   2
        section_length             12
        program_number             16
        reserved                   2
        version_number             5
        current_next_indicator     1
        section_number             8
        last_section number        8
        reserved                   3
        PCR_PID                    13
        reserved                   4
        program_info_length        12
        for (i=0;i<N;i++) {
            descriptor( )
        }
        for (i=0;i<N1;i++) {
            stream_type    8
            reserved       3
            elementary_PID 13
            reserved       4
            ES_info_length 12
            for (j=0;j<N2;j++) {
                descriptor( )
            }
        }
        CRC_32 32
    }
    ***************************************************/
    u16SectLength = ((pu8Temp[1]<<8) | pu8Temp[2]) & 0x0fff;   // section_length  12

    // 长度校验u16SectLength + 3 表示整个PMT表长度
    if ((u16SectionBuffLen < (u16SectLength + 3))
       || (MAX_SI_SECTION_LEN < (u16SectLength + 3)))
    {
       log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]error,u16SectionBuffLen[%d],u16SectLength[%d]\r\n",
            __FUNCTION__, __LINE__, u16SectionBuffLen, u16SectLength);
       return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    u16ProgramNum = (pu8Temp[3]<<8)  | pu8Temp[4];             // program_number  16
    u8VerNum      = (pu8Temp[5]>>1)  & 0x1F;                 // version_number  5
    u16PCRPID     = ((pu8Temp[8]<<8) | pu8Temp[9]) & 0x1fff;   // PCR_PID   13
    u16ProgramInfoLength = ((pu8Temp[10]<<8) | pu8Temp[11]) & 0x0fff; // program_info_length  12

    pstPmtTable->u16PCRPID      = u16PCRPID;
    pstPmtTable->u16ProgInfoLen = u16ProgramInfoLength;
    pstPmtTable->u16ServiceId   = u16ProgramNum;
    pstPmtTable->u8VersionNum   = u8VerNum;
    pstPmtTable->u8CaSysNum     = 0;
    pstPmtTable->u8EsNum        = 0;

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SIP,
        "[%s:%d]SecLen[%u],ProgNum[%u],VerNum[%u],SecNum[%u],LastSecNum[%u],PCRPID[%u],ProgInfoLen[%u]\r\n",
        __FUNCTION__,
        __LINE__,
        u16SectLength,
        u16ProgramNum,
        u8VerNum,
        pu8Temp[6],
        pu8Temp[7],
        u16PCRPID,
        u16ProgramInfoLength);

    if (0 < u16ProgramInfoLength)
    {
        // 超过最大值，返回失败，记录下日志
        if (MAX_PROGRAM_INFO_LEN < u16ProgramInfoLength)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                "[%s:%d]error,u16ProgramInfoLength[%d] out of range\r\n",
                __FUNCTION__, __LINE__, u16ProgramInfoLength);
            return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
        }

        memcpy(pstPmtTable->aru8ProgInfo, &pu8Temp[12], u16ProgramInfoLength);

        u8CANum = SIP_FindCADescriptor(&pu8Temp[12],u16ProgramInfoLength,aru16CASystemID,aru16ECMPID);

        // 超过最大值，返回失败，记录下日志
        if (MAX_CASYS_NUM_IN_PMT < u8CANum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                "[%s:%d]error,u8CANum[%d] out of range\r\n",
                __FUNCTION__, __LINE__, u8CANum);
            return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
        }

        for (i = 0; i < u8CANum; i++)
        {
            pstPmtTable->arstCaInfo[i].u16CaSysID = aru16CASystemID[i];
            pstPmtTable->arstCaInfo[i].u16EcmPid  = aru16ECMPID[i];
        }

        pstPmtTable->u8CaSysNum = u8CANum;
    }

    // 将索引位置偏移到第一个stream_type起始位置
    u16Index = 12 + u16ProgramInfoLength;

    // u16EsInfoEndIdx = SectLength + table_id(8)+section_syntax_indicator(1)+'0'+reserved(2)+section_length(12)-CRC(32)
    u16EsInfoEndIdx = u16SectLength + 3 - 4;

    while(u16Index < u16EsInfoEndIdx)
    {
        // 越界，返回失败，记录错误信息
        if (MAX_ES_NUM_IN_PMT <= pstPmtTable->u8EsNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                "[%s:%d]error,pstPmtTable->u8EsNum[%d] out of range\r\n",
                __FUNCTION__, __LINE__, pstPmtTable->u8EsNum);
            return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
        }

        pstPMTESInfo = &pstPmtTable->arstPMTESInfo[pstPmtTable->u8EsNum];

        // 解析ES PID信息
        u8StreamType    = pu8Temp[u16Index]; // stream_type    8
        u16ESPID        = ((pu8Temp[u16Index+1]<<8) | pu8Temp[u16Index+2]) & 0x1fff; // elementary_PID 13
        u16ESInfoLength = ((pu8Temp[u16Index+3]<<8) | pu8Temp[u16Index+4]) & 0x0fff; // ES_info_length 12

        pstPMTESInfo->u8Type       = u8StreamType;
        pstPMTESInfo->u16Pid       = u16ESPID;
        pstPMTESInfo->u16ESInfoLen = u16ESInfoLength;
        pstPMTESInfo->u8CaSysNum   = 0;

        // 解析ES描述信息
        if (0 < u16ESInfoLength)
        {
            // 越界，返回失败，记录错误信息
            if (MAX_ES_INFO_LEN < u16ESInfoLength)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                    "[%s:%d]error,u16ESInfoLength[%d] out of range\r\n",
                    __FUNCTION__, __LINE__, u16ESInfoLength);
                return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
            }

            memcpy(pstPMTESInfo->aru8ESInfo, &pu8Temp[u16Index+5], u16ESInfoLength);

            // 获取CA信息
            u8CANum = SIP_FindCADescriptor(&pu8Temp[u16Index+5],u16ESInfoLength,aru16CASystemID,aru16ECMPID);

            // 超过最大值，返回失败
            if (MAX_CASYS_NUM_IN_PMT < u8CANum)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                    "[%s:%d]error,ES u8CANum[%d] out of range\r\n",
                    __FUNCTION__, __LINE__, u8CANum);
                return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
            }

            for (i = 0; i < u8CANum; i++)
            {
                pstPMTESInfo->arstCaInfo[i].u16CaSysID = aru16CASystemID[i];
                pstPMTESInfo->arstCaInfo[i].u16EcmPid  = aru16ECMPID[i];
            }

            pstPMTESInfo->u8CaSysNum = u8CANum;
        }

        pstPmtTable->u8EsNum++;

        // 5表示stream_type(8)reserved(3)elementary_PID(13)reserved(4)ES_info_length(12)
        u16Index = u16Index + u16ESInfoLength + 5;
    }

    #if 0
    if (0 != DBG_GetDebugFlag(DEBUG_MODULE_SIPROCESS))
    {
        SIP_PMTTablePrint(pstPmtTable);
    }
    #endif

    return WV_SUCCESS;
}

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
wvErrCode SIP_ParseCAT(U8 *pu8SectionBuf, U16 u16SectionBuffLen, CATTable_t  *pstCatTable)
{
    U8  u8DesTag        = 0;
    U8  u8DesLength     = 0;
    U16 u16SectLength   = 0;
    U16 u16CaSysID      = 0;
    U16 u16EmmPid       = 0;
    U16 u16Index        = 0;
    U16 u16CAInfoEndIdx = 0;
    U8  *pu8Temp        = pu8SectionBuf;

    // check input param
    if ((NULL == pu8SectionBuf) || (NULL == pstCatTable))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]Input Error:pu8SectionBuf[%p],pstCatTable[%p]\r\n",
            __FUNCTION__, __LINE__, pu8SectionBuf, pstCatTable);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    // init
    memset(pstCatTable, 0, sizeof(CATTable_t));

    /***************************************************
    Conditional access section (CAT)
    Syntax No. of bits
    CA_section( ) {
        table_id                 8
        section_syntax_indicator 1
        '0'                      1
        reserved                 2
        section_length           12
        reserved                 18
        version_number           5
        current_next_indicator   1
        section_number           8
        last_section number      8
        for (i=0;i<N;i++) {
            descriptor( )
        }
        CRC_32                   32
    }
    ***************************************************/
    u16SectLength = ((pu8Temp[1]<<8) | pu8Temp[2]) & 0x0fff; // section_length(12)

    // 长度校验u16SectLength + 3 表示整个CAT表长度
    if ((u16SectionBuffLen < (u16SectLength + 3))
       || (MAX_SI_SECTION_LEN < (u16SectLength + 3)))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]error,u16SectionBuffLen[%d],u16SectLength[%d]\r\n",
            __FUNCTION__, __LINE__, u16SectionBuffLen, u16SectLength);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    pstCatTable->u8VersionNum = (pu8Temp[5]>>1) & 0x1f;    // version_number(5)
    pstCatTable->u16CaSysNum  = 0;

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SIP,
        "[%s:%d]SecLen[%u],VerNum[%u],SecNum[%u],LastSecNum[%u]\r\n",
        __FUNCTION__,
        __LINE__,
        u16SectLength,
        pstCatTable->u8VersionNum,
        pu8Temp[6],
        pu8Temp[7]);

    // 偏移到第一个CA的描述起始位置
    u16Index = 8;

    // 计算CA描述信息结束偏移位置
    // =u16SectLength+table_id(8)+section_syntax_indicator(1)+'0'+reserved(2)+section_length(12)-CRC(32)
    u16CAInfoEndIdx = u16SectLength + 3 - 4;

    while (u16Index < u16CAInfoEndIdx)
    {
        u8DesTag    = pu8Temp[u16Index];
        u8DesLength = pu8Temp[u16Index+1];

        if (CA_DESCRIPTOR_TAG == u8DesTag)
        {
            if (MAX_CASYS_NUM_IN_CAT <=  pstCatTable->u16CaSysNum)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                    "[%s:%d]error,pstCatTable->u16CaSysNum[%d] out of range\r\n",
                    __FUNCTION__, __LINE__, pstCatTable->u16CaSysNum);
                return WV_ERR_SIP_ERROR_BAD_PARAM;
            }

            u16CaSysID = (pu8Temp[u16Index+2]<<8)  | pu8Temp[u16Index+3];
            u16EmmPid  = ((pu8Temp[u16Index+4]<<8) | pu8Temp[u16Index+5]) & 0x1fff;

            pstCatTable->arstCaInfo[pstCatTable->u16CaSysNum].u16CaSysID = u16CaSysID;
            pstCatTable->arstCaInfo[pstCatTable->u16CaSysNum].u16EmmPid  = u16EmmPid;
            pstCatTable->u16CaSysNum++;
        }

        // 2表示tag和len共占两个字节
        u16Index = u16Index + u8DesLength + 2;
    }

    for (u16Index = 0; u16Index < pstCatTable->u16CaSysNum; u16Index++)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SIP,
           "[%s:%d]EMM[%u],u16CaSysID[%u],u16EmmPid[%u]\r\n",
           __FUNCTION__,
           __LINE__,
           u16Index,
           pstCatTable->arstCaInfo[u16Index].u16CaSysID,
           pstCatTable->arstCaInfo[u16Index].u16EmmPid);
    }
   

    #if 0
    if (0 != DBG_GetDebugFlag(DEBUG_MODULE_SIPROCESS))
    {
        SIP_CATTablePrint(pstCatTable);
    }
    #endif

    return WV_SUCCESS;
}


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
wvErrCode SIP_ParseSDT(U8 *pu8SectionBuf, U16 u16SectionBuffLen, SDTTable_t *pstSdtTable)
{
    U16 u16SectLength = 0;
    U16 u16ServiceID  = 0;

    //about service_descriptor
    U8  u8DescriptorTag              = 0;
    U8  u8DescriptorLength           = 0;
    U8  u8EITScheduleFlag            = 0;
    U8  u8EITPresentFollowingFlag    = 0;
    U8  u8RunningStatus              = 0;
    U8  u8FreeCAMode                 = 0;
    U8  u8ServiceType                = 0;
    U8  u8ServiceProviderNameLength  = 0;
    U8  u8ServiceNameLength          = 0;
    U16 u16CopyLength                = 0;
    U16 u16LoopLength                = 0;
    U16 u16DescriptorLoopLength      = 0;
    U16 u16Index                     = 0;
    U16 u16SdtInfoEndIdx             = 0;
    U16 u16DescriptorIdx             = 0;

    U8  u8ProviderName[MAX_SDT_NAME_LEN];
    U8  u8ServiceName[MAX_SDT_NAME_LEN];

    U8  *pu8Temp     = pu8SectionBuf;
    U8  *pDescriptor = NULL;

    SDTProgramInfo_t *parstProgInfo = NULL;

    // 参数校验
    if ((NULL == pu8SectionBuf) || (NULL == pstSdtTable))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]Input Error:pu8SectionBuf[%p],pstSdtTable[%p]\r\n",
            __FUNCTION__, __LINE__, pu8SectionBuf, pstSdtTable);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    // init
    memset(pstSdtTable, 0, sizeof(SDTTable_t));
    /***************************************************
    Conditional access section (SDT)
    Syntax No. of bits
    service_description_section( ) {
        table_id                   8
        section_syntax_indicator   1
        reserved_future_use        1
        reserved                   2
        section_length             12
        transport_stream_id        16
        reserved                   2
        vertion_number             5
        current_next_indicator     1
        section number             8
        last_section number        8
        original_network_id        16
        reserved_future_use        8
        for (i=0;i<N;i++) {
            service_id                 16
            reserved_future_use        6
            EIT_schedule_flag          1
            EIT_present_following_flag 1
            runing_status              3
            free_CA_mode               1
            descriptors_loop_length    12
            for (j=0;j<N;j++){
                descriptor()
            }
        }
        CRC_32                     32
    }
    ***************************************************/
    u16SectLength = ((pu8Temp[1]<<8) | pu8Temp[2]) & 0x0fff; // section_length(12)

    // 长度校验u16SectLength + 3 表示整个SDT表长度
    if (u16SectionBuffLen < (u16SectLength + 3))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]error,u16SectionBuffLen[%d],u16SectLength[%d]\r\n",
            __FUNCTION__, __LINE__, u16SectionBuffLen, u16SectLength);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    pstSdtTable->u16SectionLen        = u16SectLength + 3;
    pstSdtTable->u16ServiceNum        = 0;
    pstSdtTable->u16TransportStreamID = (pu8Temp[3]<<8) | pu8Temp[4];
    pstSdtTable->u8VersionNum         = (pu8Temp[5]>>1) & 0x1f;
    pstSdtTable->u8LastSectionNum     = pu8Temp[7];
    pstSdtTable->u16OriginalNetworkId = (pu8Temp[8]<<8) | pu8Temp[9];

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SIP,
            "[%s:%d]SecLen[%u],TSID[%u],VerNum[%u],SecNum[%u],LastSecNum[%u],OrgNetworkID[%u]\r\n",
            __FUNCTION__,
            __LINE__,
            u16SectLength,
            pstSdtTable->u16TransportStreamID,
            pstSdtTable->u8VersionNum,
            pu8Temp[6],
            pu8Temp[7],
            pstSdtTable->u16OriginalNetworkId);


    // 偏移到第一个service_id起始位置
    u16Index = 11;

    // 计算SDT描述信息结束偏移位置
    // =u16SectLength+table_id(8)+section_syntax_indicator(1)+reserved(3)+section_length(12)-CRC(32)
    u16SdtInfoEndIdx = u16SectLength + 3 - 4;

    while (u16Index < u16SdtInfoEndIdx)
    {
        u16ServiceID = (pu8Temp[u16Index]<<8) | pu8Temp[u16Index+1];  // service_id(16)
        u8EITScheduleFlag         = (pu8Temp[u16Index+2]>>1) & 0x01; // EIT_schedule_flag(1)
        u8EITPresentFollowingFlag = pu8Temp[u16Index+2] & 0x01;      // EIT_present_following_flag(1)
        u8RunningStatus           = (pu8Temp[u16Index+3]>>5) & 0x07; // runing_status(3)
        u8FreeCAMode              = (pu8Temp[u16Index+3]>>4) & 0x01; // free_CA_mode(1)
        u16DescriptorLoopLength   = ((pu8Temp[u16Index+3]<<8) | pu8Temp[u16Index+4]) & 0x0fff; // descriptors_loop_length(12)

        parstProgInfo = &pstSdtTable->arstProgInfo[pstSdtTable->u16ServiceNum];
        parstProgInfo->u16OtherInfoLen = 0;
        u16LoopLength = 0;

        // 解析描述信息
        while (u16LoopLength < u16DescriptorLoopLength)
        {
            // 描述结构为 TAG(8)+len(8)+descriptor(len()*8)
            // u16DescriptorIdx表示偏移到descriptor()起始位置
            u16DescriptorIdx   = u16Index+5+u16LoopLength;
            u8DescriptorTag    = pu8Temp[u16DescriptorIdx];
            u8DescriptorLength = pu8Temp[u16DescriptorIdx+1];

            // TAG为SERVICE_DESCRIPTOR_TAG的只有一个，非SERVICE_DESCRIPTOR_TAG可能有多个
            if (u8DescriptorTag != SERVICE_DESCRIPTOR_TAG)
            {
                // 存储的描述信息，需包含 TAG、length
                pDescriptor   = &pu8Temp[u16DescriptorIdx];
                u16CopyLength = u8DescriptorLength + 2;

                // 描述信息越限，记录日志，并返回失败
                if (MAX_SDT_OTHER_INFO_LEN <= (parstProgInfo->u16OtherInfoLen + u16CopyLength))
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                        "[%s:%d]error,other descriptor length:%u out of range\r\n",
                        __FUNCTION__, __LINE__, u16CopyLength);
                    return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
                }

                // save program_info in the TS structure
                memcpy(&parstProgInfo->aru8OtherInfo[parstProgInfo->u16OtherInfoLen], pDescriptor, u16CopyLength);
                parstProgInfo->u16OtherInfoLen += u16CopyLength;
            }
            else
            {
                /* 描述结构为:
                TAG(8)+descriptor_len(8)+service_type(8)+provider_name_len(8)+provider_name(provider_name_len*8)+
                service_name_len(8)+service_name(service_name_len*8)
                */
                u8ServiceType = pu8Temp[u16DescriptorIdx+2];
                u8ServiceProviderNameLength = pu8Temp[u16DescriptorIdx+3];

                // 长度越限，与SMP保持一致，只截取前面部分
                if (u8ServiceProviderNameLength <= MAX_SDT_NAME_LEN)
                {
                    u16CopyLength = u8ServiceProviderNameLength;
                }
                else
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                        "[%s:%d]error,Provider Name Length:%d out of range\r\n",
                        __FUNCTION__, __LINE__, u8ServiceProviderNameLength);

                    u16CopyLength               = MAX_SDT_NAME_LEN;
                    u8ServiceProviderNameLength = MAX_SDT_NAME_LEN;
                }

                memset(u8ProviderName,'\0',MAX_SDT_NAME_LEN);
                memcpy(u8ProviderName, &pu8Temp[u16DescriptorIdx+4],u16CopyLength);
                u8ProviderName[MAX_SDT_NAME_LEN-1]= '\0';

                u8ServiceNameLength = pu8Temp[u16DescriptorIdx+4+u8ServiceProviderNameLength];

                // 长度越限，与SMP保持一致，只截取前面部分
                if (u8ServiceNameLength <= MAX_SDT_NAME_LEN)
                {
                    u16CopyLength = u8ServiceNameLength;
                }
                else
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                        "[%s:%d]error,Service Name Length:%d out of range\r\n",
                        __FUNCTION__, __LINE__, u8ServiceNameLength);

                    u16CopyLength       = MAX_SDT_NAME_LEN;
                    u8ServiceNameLength = MAX_SDT_NAME_LEN;
                }

                memset(u8ServiceName,'\0',MAX_SDT_NAME_LEN);
                memcpy(u8ServiceName, &pu8Temp[u16DescriptorIdx+5+u8ServiceProviderNameLength],u16CopyLength);
                u8ServiceName[MAX_SDT_NAME_LEN-1]= '\0';

                parstProgInfo->u16ServiceID              = u16ServiceID;
                parstProgInfo->u8EITScheduleFlag         = u8EITScheduleFlag;
                parstProgInfo->u8EITPresentFollowingFlag = u8EITPresentFollowingFlag;
                parstProgInfo->u8RunningStatus           = u8RunningStatus;
                parstProgInfo->u8FreeCAMode              = u8FreeCAMode;
                parstProgInfo->u8ServiceType             = u8ServiceType;
                parstProgInfo->u8ServiceNameLen          = u8ServiceNameLength;
                parstProgInfo->u8ProviderNameLen         = u8ServiceProviderNameLength;

                memcpy(parstProgInfo->aru8ProviderName, u8ProviderName, MAX_SDT_NAME_LEN);
                memcpy(parstProgInfo->aru8ServiceName, u8ServiceName,   MAX_SDT_NAME_LEN);
            }

            // 2表示 TAG(8) LENGTH(8)
            u16LoopLength = u16LoopLength + u8DescriptorLength + 2;
        }

        pstSdtTable->u16ServiceNum++;

        // 5表示service_id(16)EIT_schedule_flag(1)EIT_present_following_flag(1)runing_status(3)free_CA_mode(1)descriptors_loop_length(12)
        u16Index = u16Index + u16DescriptorLoopLength + 5;
    }

    #if 0
    if (0 != DBG_GetDebugFlag(DEBUG_MODULE_SIPROCESS))
    {
        SIP_SDTTablePrint(pstSdtTable);
    }
    #endif

    return WV_SUCCESS;
}

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
wvErrCode SIP_ParseNIT(U8 *pu8SectionBuf, U16 u16SectionBuffLen, NITTable_t *pstNitTable)
{
    (void)pu8SectionBuf;
    (void)pstNitTable;
    (void)u16SectionBuffLen;
    // 目前还未有需求，先不需实现

    /***************************************************
    Conditional access section (NIT)
    Syntax No. of bits
    network_information_section( ) {
        table_id                      8
        section_syntax_indicator      1
        reserved_future_use           1
        reserved                      2
        section_length                12
        network_id                    16
        reserved                      2
        vertion_number                5
        current_next_indicator        1
        section number                8
        last_section number           8
        reserved_future_use           4
        network_description_length    12
        for (i=0;i<N;i++) {
            descriptor( )
        }
        reserved_future_use           4
        transport_stream_loop_length  12
        for (i=0;i<N;i++){
            transport_stream_id          16
            original_network_id          16
            reserved_future_use          4
            transport_description_length 12
            for (j=0;j<N;j++){
                descriptor()
            }
        }
        CRC_32                        32
    }
    ***************************************************/



    return WV_SUCCESS;
}

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
wvErrCode SIP_ParseBAT(U8 *pu8SectionBuf, U16 u16SectionBuffLen, BATTable_t *pstBATTable)
{
    U16 u16Index      = 0;
    U16 u16SectLength = 0;
    U16 u16TSLoopLen  = 0;
    U16 u16TSParseLen = 0;

    U8  *pu8Temp      = pu8SectionBuf;

    TSInfoStruct_t *pstTSInfo = NULL;

    // 参数校验
    if ((NULL == pu8SectionBuf) || (NULL == pstBATTable))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]Input Error:pu8SectionBuf[%p],pstBATTable[%p]\r\n",
            __FUNCTION__, __LINE__, pu8SectionBuf, pstBATTable);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    // init
    memset(pstBATTable, 0, sizeof(BATTable_t));

    /*
    Bouquet Association Table (BAT)
    bouquet_association_section(){
    table_id                        8
    section_syntax_indicator        1
    reserved_future_use             1
    reserved                        2
    section_length                  12
    bouquet_id                      16
    reserved                        2
    version_number                  5
    current_next_indicator          1
    section_number                  8
    last_section_number             8
    reserved_future_use             4
    bouquet_descriptors_length      12
    for(i=0;i<N;i++)
    {
      descriptor()
    }

    reserved_future_use             4
    transport_stream_loop_length    12
    for(i=0;i<N;i++)
    {
      transport_stream_id            16
      original_network_id            16
      reserved_future_use            4
      transport_descriptors_length   12
      for(j=0;j<N;j++)
      {
          descriptor()
      }
    }
    CRC_32                              32
    }
    */

    u16SectLength = ((pu8Temp[1]<<8) | pu8Temp[2]) & 0x0fff; // section_length(12)

    // 长度校验u16SectLength + 3 表示整个BAT表长度
    if ((u16SectionBuffLen < (u16SectLength + 3))
       || (MAX_SI_SECTION_LEN < (u16SectLength + 3)))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]error,u16SectionBuffLen[%d],u16SectLength[%d]\r\n",
            __FUNCTION__, __LINE__, u16SectionBuffLen, u16SectLength);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    pstBATTable->u16SectionLen = u16SectLength + 3;

    // bouquet_id(16)
    pstBATTable->u16BouquetId = (pu8Temp[3]<<8) | pu8Temp[4];

    // reserved(2) version_number(5) current_next_indicator(1)
    pstBATTable->u8VersionNum = (pu8Temp[5]>>1) & 0x1f;

    // reserved_future_use(4) bouquet_descriptors_length(12)
    pstBATTable->u16BouquetInfoLen = ((pu8Temp[8]<<8) | pu8Temp[9]) & 0x0fff;

    if (MAX_BOUQUET_INFO_LENGHT < pstBATTable->u16BouquetInfoLen)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]error,BouquetInfoLen:%d out of range\r\n",
            __FUNCTION__, __LINE__, pstBATTable->u16BouquetInfoLen);
        return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
    }

    if (0 < pstBATTable->u16BouquetInfoLen)
    {
        memcpy(pstBATTable->aru8BouquetInfo, &pu8Temp[10], pstBATTable->u16BouquetInfoLen);
    }

    // 偏移到reserved_future_use(4)transport_stream_loop_length(12)起始位置
    u16Index     = 10 + pstBATTable->u16BouquetInfoLen;
    u16TSLoopLen = ((pu8Temp[u16Index]<<8) | pu8Temp[u16Index+1]) & 0x0fff;
    u16Index += 2;

    while (u16TSParseLen < u16TSLoopLen)
    {
        if (MAX_TS_NUMBER_INT_BAT <= pstBATTable->u16TransportStreamNum)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                "[%s:%d]error,u16TransportStreamNum:%d out of range\r\n",
                __FUNCTION__, __LINE__, pstBATTable->u16TransportStreamNum);
            return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
        }

        pstTSInfo = &pstBATTable->arstTSInfo[pstBATTable->u16TransportStreamNum];
        /*
        transport_stream_id            16
        original_network_id            16
        reserved_future_use            4
        transport_descriptors_length   12
        for(j=0;j<N;j++)
        {
          descriptor()
        }
        */
        pstTSInfo->u16TSID = ((pu8Temp[u16Index]<<8) | pu8Temp[u16Index+1]);
        u16Index += 2;

        pstTSInfo->u16OriginalNetId = ((pu8Temp[u16Index]<<8) | pu8Temp[u16Index+1]);
        u16Index += 2;

        pstTSInfo->u16DescriptorsLen = ((pu8Temp[u16Index]<<8) | pu8Temp[u16Index+1]) & 0x0fff;
        u16Index += 2;

        if (MAX_TS_DESCRIPTORS_LENGHT < pstTSInfo->u16DescriptorsLen)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                "[%s:%d]error,TransportStream DescriptorsLen:%d out of range\r\n",
                __FUNCTION__, __LINE__, pstTSInfo->u16DescriptorsLen);
            return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
        }

        if (0 < pstTSInfo->u16DescriptorsLen)
        {
            memcpy(pstTSInfo->aru8Descriptors, &pu8Temp[u16Index], pstTSInfo->u16DescriptorsLen);
        }

        u16Index += pstTSInfo->u16DescriptorsLen;

        u16TSParseLen += (6 + pstTSInfo->u16DescriptorsLen);

        pstBATTable->u16TransportStreamNum++;
    }

    #if 0
    if (0 != DBG_GetDebugFlag(DEBUG_MODULE_SIPROCESS))
    {
        SIP_BATTablePrint(pstBATTable);
    }
    #endif

    return WV_SUCCESS;
}

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
U8 SIP_FindCADescriptor(U8* pu8DesBuf, U16 u16DesLength, U16* pu16CASystemID, U16* pu16CAPID)
{
    U8* pu8Temp = NULL;
    U8  u8Tag = 0;
    U8  u8Len = 0;
    U8  u8CANum = 0;
    U16 u16HadParseLen = 0;

    // check input param
    if ((NULL == pu8DesBuf) || (NULL == pu16CASystemID) || (NULL == pu16CAPID))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]Input Error:pu8DesBuf[%p],pu16CASystemID[%p],pu16CAPID[%p]\r\n",
            __FUNCTION__, __LINE__, pu8DesBuf, pu16CASystemID, pu16CAPID);
        return 0;
    }

    pu8Temp = pu8DesBuf;

    while (u16HadParseLen < u16DesLength)
    {
        u8Tag = pu8Temp[0];
        u8Len = pu8Temp[1];

        if (CA_DESCRIPTOR_TAG == u8Tag)
        {
            if (u8CANum < MAX_CASYS_NUM_IN_PMT)
            {
                pu16CASystemID[u8CANum] = (pu8Temp[2] << 8) | pu8Temp[3];
                pu16CAPID[u8CANum] = ((pu8Temp[4] & 0x1F) << 8) | pu8Temp[5];
            }

            u8CANum++;
        }

        pu8Temp += (2 + u8Len);
        u16HadParseLen += (2 + u8Len);
    }

    return u8CANum;
}

/*****************************************************************************
  Function:     SIP_GETPMTTableLen
  Description:  获取PMT表的总长度
  Input:        PMTTable_t *pstPmtTable
  Output:       U16 *pu16TableLen
  Return:       success or fail
  Others:
  Create:       2013.09.16 ziteng.yang
*****************************************************************************/
wvErrCode SIP_GETPMTTableLen(const PMTTable_t *pstPmtTable, U16 *pu16TableLen)
{
    U16 i = 0;
    const PMTESInfo_t *pstPMTESInfo = NULL;

    if ((NULL == pstPmtTable) || (NULL == pu16TableLen))
    {
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    if (MAX_ES_NUM_IN_PMT < pstPmtTable->u8EsNum)
    {
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    // table_id(8)到program_info_length(12)共12个字节，再加上4字节CRC
    *pu16TableLen = 12 + 4;
    *pu16TableLen = *pu16TableLen + pstPmtTable->u16ProgInfoLen;

    for (i = 0; i < pstPmtTable->u8EsNum; i++)
    {
        pstPMTESInfo = &pstPmtTable->arstPMTESInfo[i];

        // 5表示stream_type(8)reserved(3)elementary_PID(13)reserved(4)ES_info_length(12)
        *pu16TableLen = *pu16TableLen + pstPMTESInfo->u16ESInfoLen + 5;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     SIP_GETCATTableLen
  Description:  获取CAT表的总长度
  Input:        CATTable_t *pstCatTable
  Output:       U16 *pu16TableLen
  Return:       success or fail
  Others:
  Create:       2013.09.16 ziteng.yang
*****************************************************************************/
wvErrCode SIP_GETCATTableLen(const CATTable_t *pstCatTable, U16 *pu16TableLen)
{
    U32 i = 0;

    if ((NULL == pstCatTable) || (NULL == pu16TableLen))
    {
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    if (MAX_CASYS_NUM_IN_CAT < pstCatTable->u16CaSysNum)
    {
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    // table_id()到last_section number(8)共8个字节，加上CRC
    *pu16TableLen = 8 + 4;

    // EMM
    for (i = 0; i < pstCatTable->u16CaSysNum; i++)
    {
        // 6 表示TAG(8)+LENGTH(8)+cs_system_id(16)+ca_pid(16)
        *pu16TableLen = *pu16TableLen + 6;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     SIP_GetSDTSectionInfo
  Description:  获取SDT表的分段个数以及需要的buff总大小
  Input:        SDTTable_t *pstSdtTable
  Output:       U8 *pu8SectionNum
                U16 *pu16BuffLen  // buff长度
  Return:       success or fail
  Others:
  Create:       2013.09.16 ziteng.yang
*****************************************************************************/
wvErrCode SIP_GetSDTSectionInfo(const SDTTable_t *pstSdtTable, U8 *pu8SectionNum, U16 *pu16BuffLen)
{
    U8   u8SectionNum            = 0;
    U8   u8ProviderNameLength    = 0;
    U8   u8ServiceNameLength     = 0;
    U16  u16SectionBufLen        = 0;  // 用于记录每一个分段的总字节数
    U16  u16ServiceInfoLen       = 0;
    U16  u16DescriptonLoopLength = 0;
    U16  i                       = 0;

    const SDTProgramInfo_t *parstProgInfo = NULL;

    if ((NULL == pstSdtTable) || (NULL == pu8SectionNum))
    {
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    if (MAX_PROGRAMS_NUM_IN_SDT < pstSdtTable->u16ServiceNum)
    {
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    if (0 == pstSdtTable->u16ServiceNum)
    {
        *pu8SectionNum = 0;
        return WV_SUCCESS;
    }

    u8SectionNum = 1;

    // table_id(8)到reserved_future_use(8)总共占11个字节 CRC占4个字节剩余为节目信息
    u16ServiceInfoLen = MAX_SI_SECTION_LEN - 15;
    *pu16BuffLen = 15;

    // 计算SDT分段个数
    for (i = 0; i < pstSdtTable->u16ServiceNum; i++)
    {
        parstProgInfo = &pstSdtTable->arstProgInfo[i];

        /*
        u8ProviderNameLength = (U8)strlen((const char *)parstProgInfo->aru8ProviderName);
        u8ServiceNameLength  = (U8)strlen((const char *)parstProgInfo->aru8ServiceName);
        */
        u8ProviderNameLength = parstProgInfo->u8ProviderNameLen;
        u8ServiceNameLength  = parstProgInfo->u8ServiceNameLen;

        if ((0 == u8ProviderNameLength) && (0 == u8ServiceNameLength)
            && (0 == parstProgInfo->u16OtherInfoLen))
        {
            continue;
        }

        if ((0 != u8ProviderNameLength) || (0 != u8ServiceNameLength))
        {
            u16DescriptonLoopLength = 5; // 5表示descriptor_tag(8)length(8)serv_type(8)provi_name_len(8)serv_name_len(8)
            u16DescriptonLoopLength += u8ProviderNameLength;
            u16DescriptonLoopLength += u8ServiceNameLength;
        }
        else
        {
            u16DescriptonLoopLength = 0;
        }

        u16DescriptonLoopLength += parstProgInfo->u16OtherInfoLen;

        // service_id(16)到descriptors_loop_length(12)共5个字节
        u16SectionBufLen += 5;
        u16SectionBufLen += u16DescriptonLoopLength;

        *pu16BuffLen = *pu16BuffLen + 5 + u16DescriptonLoopLength;


        // 超过最大分段长度，则表示一个section封装完毕
        if (u16ServiceInfoLen < u16SectionBufLen)
        {
            u8SectionNum++;

            // 下次buffenlen计算必须把本次没有封装进去的service信息加上
            u16SectionBufLen = 5 + u16DescriptonLoopLength;

            // 下个section开始，必须加上table_id(8)到reserved_future_use(8)总共占11个字节 CRC占4个字节
            *pu16BuffLen = *pu16BuffLen + 15;
        }
    }

    *pu8SectionNum = u8SectionNum;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     SIP_GetBATSectionInfo
  Description:  获取BAT表的分段个数以及需要的buff总大小
  Input:        BATTable_t *pstBATTable
  Output:       U8  *pu8SectionNum
                U16 *pu16BuffLen  // buff长度
  Return:       success or fail
  Others:
  Create:       2014.07.29 ziteng.yang
*****************************************************************************/
wvErrCode SIP_GetBATSectionInfo(const BATTable_t *pstBATTable, U8 *pu8SectionNum, U16 *pu16BuffLen)
{
    U8  u8SectionNum      = 0;
    U16 u16BuffLen        = 0;  // 用于记录整个BAT需要的buff大小
    U16 u16TSInfoLen      = 0;
    U16 u16SectionBufLen  = 0;  // 用于记录每一个分段的总字节数
    U32 i                 = 0;

    const TSInfoStruct_t *pstTSInfo = NULL;

    // 入参检查
    if ((NULL == pstBATTable) || (NULL == pu8SectionNum) || (NULL == pu16BuffLen))
    {
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    if ((MAX_BOUQUET_INFO_LENGHT < pstBATTable->u16BouquetInfoLen)
       || (MAX_TS_NUMBER_INT_BAT < pstBATTable->u16TransportStreamNum))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]Input param is wrong\r\n", __FUNCTION__, __LINE__);
        return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
    }

    for (i = 0; i < pstBATTable->u16TransportStreamNum; i++)
    {
        pstTSInfo = &pstBATTable->arstTSInfo[i];

        if (MAX_TS_DESCRIPTORS_LENGHT < pstTSInfo->u16DescriptorsLen)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                "[%s:%d]Input param DescriptorsLen is wrong\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
        }
    }

    // table_id到bouquet_descriptors_length共10个字节，加上4字节CRC为14
    u16SectionBufLen = 14;

    // 第一个section才需要加上bouquet_descriptors
    u16SectionBufLen += pstBATTable->u16BouquetInfoLen;

    if (MAX_SI_SECTION_LEN < u16SectionBufLen)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]u16SectionBufLen[%u] out of range\r\n", __FUNCTION__, __LINE__, u16SectionBufLen);
        return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
    }

    // 接下来是遍历所有的ts流的描述信息，并计算长度
    for (i = 0; i < pstBATTable->u16TransportStreamNum; i++)
    {
        pstTSInfo = &pstBATTable->arstTSInfo[i];

        // 每个transport_stream描述长度为6字节加上transport_descriptors_length
        u16TSInfoLen = 6 + pstTSInfo->u16DescriptorsLen;

        // BAT超过一个section长度，则需要另起一个section记录
        if (MAX_SI_SECTION_LEN < (u16SectionBufLen + u16TSInfoLen))
        {
            u16BuffLen += u16SectionBufLen;

            // table_id到bouquet_descriptors_length共10个字节，加上4字节CRC为14
            u16SectionBufLen = 14 + u16TSInfoLen;
            u8SectionNum++;
        }
        else
        {
            u16SectionBufLen += u16TSInfoLen;
        }
    }

    // 还有剩余信息，需要计算进来
    if (0 != u16SectionBufLen)
    {
        u16BuffLen += u16SectionBufLen;
        u8SectionNum++;
    }

    *pu8SectionNum = u8SectionNum;
    *pu16BuffLen   = u16BuffLen;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     SIP_ConstructPAT
  Description:  构造出PAT表Section
  Input:        pstPatTable     －  PAT表结构
                u16BufLen       －  输入buff的大小
  Output:       pu8SectionBuf   －  Section存放缓冲区
                pu16SectionLen  －  Section长度指针
  Return:       SMP_SUCCESS or FAIL
  Others:       构造的section从table id开始，不包含pointer_field;
  Create:       2013.09.16 ziteng.yang
*****************************************************************************/
wvErrCode SIP_ConstructPAT(const PATTable_t *pstPatTable, U16 u16BufLen, U8 *pu8SectionBuf, U16 *pu16SectionLen)
{
    U32 i                = 0;
    U32 u32CRC           = 0;
    U16 u16Index         = 0;
    U16 u16SectionLength = 0;
    U8  *pu8Temp         = pu8SectionBuf;

    if ((!pstPatTable) || (!pu8SectionBuf) || (!pu16SectionLen))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]Input Error,pstPatTable[%p],pu8SectionBuf[%p],pu8SectionBuf[%p]\r\n",
            __FUNCTION__, __LINE__, pstPatTable, pu8SectionBuf, pu8SectionBuf);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    if (MAX_PROGRAMS_NUMBER_IN_PAT < pstPatTable->u16ProgramCount)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]ProgramCount:%d out of range\r\n",
            __FUNCTION__, __LINE__, pstPatTable->u16ProgramCount);
        return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
    }

    *pu16SectionLen = 0;

    // table_id  8bits
    pu8Temp[0] = PAT_TID;

    /* u16SectionLength = transport_stream_id(16)+reserved(2)+version_number(5)+current_next_indicator(1)+
                         section_number(8)+last_section_number(8)+
                         [ProgramCount*(program_number(16)+reserved(3)+program_map_PID(13))] + CRC(32)*/
    u16SectionLength = 5 + (U16)(pstPatTable->u16ProgramCount*4) + 4;

    // 长度校验u16SectLength + 3 表示整个PAT表长度
    if (u16BufLen < (u16SectionLength+3))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]u16BufLen[%u],u16SectionLength[%u],error\r\n",
            __FUNCTION__, __LINE__, u16BufLen, u16SectionLength);
        return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
    }

    // section_syntax_indicaor   1bit    1
    // '0'                       1bit    0
    // reserved                  2bits   xx
    // section_length            12bits
    pu8Temp[1] = ((u16SectionLength>>8) & 0x0f ) | 0xb0;
    pu8Temp[2] = u16SectionLength & 0xff;

    // transport_stream_id 16bit
    pu8Temp[3] = (pstPatTable->u16TransportStreamID>>8) & 0xff;
    pu8Temp[4] = pstPatTable->u16TransportStreamID & 0xff;

    // reserved                  2bit        0b11
    // version_number            5bit        0b00000
    // current_next_indicator    1bit        0b1
    pu8Temp[5] = (0xc1 | ((pstPatTable->u8VersionNum&0x1f)<<1));

    // section_number            8bit
    pu8Temp[6] = 0x00;

    // last_section_number       8bit
    pu8Temp[7] = 0x00;

    // 偏移到节目信息起始位置 即第一个program_number(16)起始位置
    u16Index = 8;

    // add programs
    for (i = 0;i < pstPatTable->u16ProgramCount; i++)
    {
        pu8Temp[u16Index]   = pstPatTable->arstProgramInfo[i].u16ServiceId >> 8;
        pu8Temp[u16Index+1] = pstPatTable->arstProgramInfo[i].u16ServiceId & 0xff;
        pu8Temp[u16Index+2] = ((pstPatTable->arstProgramInfo[i].u16PMTPid >> 8) & 0x1f) | 0xe0;
        pu8Temp[u16Index+3] = pstPatTable->arstProgramInfo[i].u16PMTPid & 0xff;

        // 4 表示(program_number(16)+reserved(3)+program_map_PID(13))
        u16Index += 4;
    }

    // CRC_32 从table_id开始计算，不包括CRC字段
    u32CRC = Common_CRCCalculate(&pu8Temp[0],u16SectionLength+3-4,0);
    pu8Temp[u16Index]   = (u32CRC & 0xff000000) >> 24;
    pu8Temp[u16Index+1] = (u32CRC & 0x00ff0000) >> 16;
    pu8Temp[u16Index+2] = (u32CRC & 0x0000ff00) >> 8;
    pu8Temp[u16Index+3] = u32CRC & 0x000000ff;

    *pu16SectionLen = u16Index+4;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     SIP_ConstructPMT
  Description:  构造出PMT表Section
  Input:        pstPmtTable     －  PMT表结构
                u16BufLen       －  输入buff的大小
  Output:       pu8SectionBuf   －  Section存放缓冲区
                pu16SectionLen  －  Section长度指针
  Return:       SMP_SUCCESS or FAIL
  Others:       构造的section从table id开始，不包含pointer_field;
  Create:       2013.09.16 ziteng.yang
*****************************************************************************/
wvErrCode SIP_ConstructPMT(const PMTTable_t *pstPmtTable, U16 u16BufLen, U8 *pu8SectionBuf, U16 *pu16SectionLen)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 u16SectionLength     = 0;
    U16 u16ProgramInfoLength = 0;
    U16 i                    = 0;
    U16 u16ESNumber          = 0;
    U16 u16ESLength          = 0;
    U32 u32CRC               = 0;
    U16 u16Index             = 0;
    U16 u16PmtTableLen       = 0;
    U8  aru8Start[PACKET_SECTION_LENGTH];

    U8  *pu8Temp    = pu8SectionBuf;
    const PMTESInfo_t *pstPMTESInfo = NULL;

    if ((!pstPmtTable) || (!pu8SectionBuf) || (!pu16SectionLen))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]Input Error,pstPmtTable[%p],pu8SectionBuf[%p],pu8SectionBuf[%p]\r\n",
            __FUNCTION__, __LINE__, pstPmtTable, pu8SectionBuf, pu8SectionBuf);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    memset(aru8Start,0xff,sizeof(aru8Start));

    *pu16SectionLen = 0;

    // 长度校验
    enErrCode = SIP_GETPMTTableLen(pstPmtTable, &u16PmtTableLen);
    if ((WV_SUCCESS != enErrCode) || (u16BufLen < u16PmtTableLen))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]u16BufLen[%u],u16PmtTableLen[%u],enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, u16BufLen, u16PmtTableLen, enErrCode);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    // table_id  8bits
    pu8Temp[0] = PMT_TID;

    // section_syntax_indicaor   1bit    1
    // '0'                       1bit    0
    // reserved                  2bits   xx
    // section_length            12bits

    // program_number           16bit
    pu8Temp[3] = (pstPmtTable->u16ServiceId >> 8 ) & 0xff;
    pu8Temp[4] = pstPmtTable->u16ServiceId & 0xff;

    // reserved                  2bit        0b11
    // version_number            5bit        0b00000
    // current_next_indicator    1bit        0b1
    pu8Temp[5] = (0xc1 | ((pstPmtTable->u8VersionNum&0x1f)<<1));

    // section_number            8bit
    pu8Temp[6] = 0x00;

    // last_section_number       8bit
    pu8Temp[7] = 0x00;

    // reserved      3bit    0bxxx
    // PCR_PID       13bit
    pu8Temp[8] = pstPmtTable->u16PCRPID >> 8 ;
    pu8Temp[8] = (pu8Temp[8] & 0x1f) | 0xe0;
    pu8Temp[9] = pstPmtTable->u16PCRPID & 0xff;

    // reversed              4bit    0bxxxx
    // program_info_length   12bit   0x000
    u16ProgramInfoLength = pstPmtTable->u16ProgInfoLen;
    pu8Temp[10] = ((u16ProgramInfoLength>>8) & 0x0f) | 0xf0;
    pu8Temp[11] = u16ProgramInfoLength & 0xff;

    if (MAX_PROGRAM_INFO_LEN < u16ProgramInfoLength)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]ProgramInfoLength:%d out of range\r\n",
            __FUNCTION__, __LINE__, u16ProgramInfoLength);
        return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
    }

    // add program info
    if (0 < u16ProgramInfoLength)
    {
        memcpy(&pu8Temp[12], pstPmtTable->aru8ProgInfo, u16ProgramInfoLength);
    }

    // 偏移到第一个ES信息起始位置
    u16Index = 12 + u16ProgramInfoLength;

    // start to add elementary stream and es descriptors
    u16ESNumber = pstPmtTable->u8EsNum;
    if (MAX_ES_NUM_IN_PMT < u16ESNumber)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]EsNum:%d out of range\r\n",
            __FUNCTION__, __LINE__, u16ESNumber);
        return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
    }

    for (i=0; i<u16ESNumber; i++)
    {
        pstPMTESInfo = &pstPmtTable->arstPMTESInfo[i];
        //stream type
        pu8Temp[u16Index] = pstPMTESInfo->u8Type;

        // reserved          3bit    0bxxx
        // elementary_PID    13bit
        pu8Temp[u16Index+1] = pstPMTESInfo->u16Pid >> 8;
        pu8Temp[u16Index+1] = (pu8Temp[u16Index+1]   & 0x1f) | 0xe0;
        pu8Temp[u16Index+2] = pstPMTESInfo->u16Pid & 0xff;

        // reserved          4bit
        // es_info_length    12bit
        u16ESLength = pstPMTESInfo->u16ESInfoLen;
        pu8Temp[u16Index+3] = ( (u16ESLength>>8) & 0x0f ) | 0xf0;
        pu8Temp[u16Index+4] = u16ESLength & 0xff;

        if (MAX_ES_INFO_LEN < u16ESLength)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                "[%s:%d]u16ESLength:%d out of range\r\n",
                __FUNCTION__, __LINE__, u16ESLength);
            return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
        }

        // add ES info
        if (0 < u16ESLength)
        {
            memcpy(&pu8Temp[u16Index+5],pstPMTESInfo->aru8ESInfo,u16ESLength);
        }

        // 5表示stream_type(8)reserved(3)elementary_PID(13)reserved(4)ES_info_length(12)
        u16Index = u16Index + u16ESLength + 5;
    }

    // u16SectionLength从program_number(16)开始计算，并包括CRC
    u16SectionLength = (U16)(u16Index - 3) + 4;
    pu8Temp[1]         = ((u16SectionLength >> 8) & 0x0f) | 0xb0;
    pu8Temp[2]         = u16SectionLength & 0xff;

    // CRC_32 从table_id开始计算，不包括CRC字段
    u32CRC = Common_CRCCalculate(&pu8Temp[0], u16SectionLength+3-4,0);
    pu8Temp[u16Index]   = (u32CRC & 0xff000000) >> 24;
    pu8Temp[u16Index+1] = (u32CRC & 0x00ff0000) >> 16;
    pu8Temp[u16Index+2] = (u32CRC & 0x0000ff00) >> 8;
    pu8Temp[u16Index+3] = u32CRC  & 0x000000ff;

    *pu16SectionLen = u16Index+4;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     SIP_ConstructCAT
  Description:  构造出CAT表Section
  Input:        pstCatTable     －  CAT表结构
                u16BufLen       －  输入buff的大小
  Output:       pu8SectionBuf   －  Section存放缓冲区
                pu16SectionLen  －  Section长度指针
  Return:       SMP_SUCCESS or FAIL
  Others:       构造的section从table id开始，不包含pointer_field;
  Create:       2013.09.16 ziteng.yang
*****************************************************************************/
wvErrCode SIP_ConstructCAT(const CATTable_t *pstCatTable, U16 u16BufLen, U8 *pu8SectionBuf, U16 *pu16SectionLen)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i                = 0;
    U16 u16SectionLength = 0;
    U32 u32CRC           = 0;
    U16 u16Index         = 0;
    U16 u16CatTableLen   = 0;

    U8 *pu8Temp = pu8SectionBuf;

    if ((!pstCatTable) || (!pu8SectionBuf) || (!pu16SectionLen))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]Input Error,pstCatTable[%p],pu8SectionBuf[%p],pu8SectionBuf[%p]\r\n",
            __FUNCTION__, __LINE__, pstCatTable, pu8SectionBuf, pu8SectionBuf);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    *pu16SectionLen = 0;

    enErrCode = SIP_GETCATTableLen(pstCatTable, &u16CatTableLen);
    if ((WV_SUCCESS != enErrCode) || (u16BufLen < u16CatTableLen))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]u16BufLen[%u],u16CatTableLen[%u],enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, u16BufLen, u16CatTableLen, enErrCode);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    // table_id  8bits
    pu8Temp[0] = CAT_TID;

    // section_syntax_indicaor   1bit    1
    // '0'                       1bit    0
    // reserved                  2bits   xx
    // section_length            12bits

    // reserved                  16bits
    pu8Temp[3] = 0xff;
    pu8Temp[4] = 0xff;

    // reserved                  2bit        0b11
    // version_number            5bit        0b00000
    // current_next_indicator    1bit        0b1
    pu8Temp[5] = 0xc1 | ((pstCatTable->u8VersionNum&0x1f)<<1);

    // section_number            8bit
    pu8Temp[6] = 0x00;

    // last_section_number       8bit
    pu8Temp[7] = 0x00;

    // 偏移到第一个CA的描述起始位置
    u16Index = 8;

    if (MAX_CASYS_NUM_IN_CAT < pstCatTable->u16CaSysNum)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]CaSysNum:%d out of range\r\n",
            __FUNCTION__, __LINE__, pstCatTable->u16CaSysNum);
        return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
    }

    // add EMM
    for (i = 0; i < pstCatTable->u16CaSysNum; i++)
    {
        pu8Temp[u16Index]   = CA_DESCRIPTOR_TAG;
        pu8Temp[u16Index+1] = 0x04; // 长度固定为4 包括cs_system_id(16) ca_pid(16)

        // add ca system_id
        pu8Temp[u16Index+2] = (pstCatTable->arstCaInfo[i].u16CaSysID >> 8) & 0xff;
        pu8Temp[u16Index+3] = pstCatTable->arstCaInfo[i].u16CaSysID & 0xff;

        // add emm pid
        pu8Temp[u16Index+4] = ((pstCatTable->arstCaInfo[i].u16EmmPid >> 8) & 0x1f) | 0xe0;
        pu8Temp[u16Index+5] = pstCatTable->arstCaInfo[i].u16EmmPid & 0xff;

        // 6 表示TAG(8)+LENGTH(8)+cs_system_id(16)+ca_pid(16)
        u16Index = u16Index + 6;
    }

    // u16SectionLength从reserved(18)开始计算，并包括CRC
    u16SectionLength = (U16)(u16Index - 3) + 4;
    pu8Temp[1] = ((u16SectionLength >> 8) & 0x0f) | 0xb0;
    pu8Temp[2] = u16SectionLength &0xff;

    // CRC_32 从table_id开始计算，不包括CRC字段
    u32CRC = Common_CRCCalculate(&pu8Temp[0], u16SectionLength+3-4, 0);
    pu8Temp[u16Index]   = (u32CRC & 0xff000000) >> 24;
    pu8Temp[u16Index+1] = (u32CRC & 0x00ff0000) >> 16;
    pu8Temp[u16Index+2] = (u32CRC & 0x0000ff00) >> 8;
    pu8Temp[u16Index+3] = u32CRC & 0x000000ff;

    *pu16SectionLen = u16Index+4;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     SIP_ConstructSDT
  Description:  构造出SDT表Section
  Input:        pstSdtTable     －  SDT表结构
                u16BufLen       －  输入buff的大小
  Output:       pu8SectionBuf   －  Section存放缓冲区
                pu16SectionLen  －  Section长度指针
  Return:       SMP_SUCCESS or FAIL
  Others:       构造的section从table id开始，不包含pointer_field;多个section时，
                每个section在pu8SectionBuf里连续存放，需调用者识别
  Create:       2013.09.16 ziteng.yang
*****************************************************************************/
wvErrCode SIP_ConstructSDT(const SDTTable_t *pstSdtTable, U16 u16BufLen, U8 *pu8SectionBuf, U16 *pu16SectionLen)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8   u8ProviderNameLength    = 0;
    U8   u8ServiceNameLength     = 0;
    U8   u8SectionNum            = 0;
    U16  u16ServiceNum           = 0;
    U16  u16SectionLength        = 0;
    U16  u16DescriptonLoopLength = 0;
    U16  i                = 0;
    U16  j                = 0;
    U32  u32CRC           = 0;
    U16  u16Index         = 0;
    U16  u16DescriptorIdx = 0;
    U16  u16SdtTableLen   = 0;

    U8   *pu8Temp = pu8SectionBuf;
    const SDTProgramInfo_t *parstProgInfo = NULL;

    U16 u16SectionBufLen = 0;  // 用于记录每一个分段的总字节数

    if ((!pstSdtTable) || (!pu8SectionBuf) || (!pu16SectionLen))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]Input Error,pstSdtTable[%p],pu8SectionBuf[%p],pu8SectionBuf[%p]\r\n",
            __FUNCTION__, __LINE__, pstSdtTable, pu8SectionBuf, pu8SectionBuf);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    *pu16SectionLen = 0;

    enErrCode = SIP_GetSDTSectionInfo(pstSdtTable, &u8SectionNum, &u16SdtTableLen);
    if ((WV_SUCCESS != enErrCode) || (u16BufLen < u16SdtTableLen))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]u16BufLen[%u],u16SdtTableLen[%u],enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, u16BufLen, u16SdtTableLen, enErrCode);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    // 一个分段都没有，返回失败
    if (0 == u8SectionNum)
    {
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    for (i = 0; i < u8SectionNum; i++)
    {
        // table_id  8bits
        pu8Temp[0] = SDT_TID_ACTUAL;

        // section_syntax_indicaor   1bit    1
        // '0'                       1bit    0
        // reserved                  2bits   xx
        // section_length            12bits

        // transport_stream_id 16bit
        pu8Temp[3] = (pstSdtTable->u16TransportStreamID>>8) & 0xff;
        pu8Temp[4] = pstSdtTable->u16TransportStreamID & 0xff;

        // reserved                  2bit        0b11
        // version_number            5bit        0b00000
        // current_next_indicator    1bit        0b1
        pu8Temp[5] = (0xc1 | ((pstSdtTable->u8VersionNum&0x1f)<<1));

        // section_number            8bit
        pu8Temp[6] = (U8)i;     // 从0开始计数

        // last_section_number       8bit
        pu8Temp[7] = u8SectionNum-1;

        // original_network_id   16bit
        pu8Temp[8] = (pstSdtTable->u16OriginalNetworkId >> 8) & 0xff ;
        pu8Temp[9] = pstSdtTable->u16OriginalNetworkId & 0xff;

        // reserved future used 8bits
        pu8Temp[10] = 0xff;

        // table id 到reserved_future_use(8) 总共11字节, CRC 4个字节
        u16SectionBufLen = 11 + 4;

        // 偏移到第一个service_id起始位置
        u16Index = 11;

        for (j=u16ServiceNum; j<pstSdtTable->u16ServiceNum; j++)
        {
            parstProgInfo = &pstSdtTable->arstProgInfo[j];

            /*
            u8ProviderNameLength = (U8)strlen((const char *)parstProgInfo->aru8ProviderName);
            u8ServiceNameLength  = (U8)strlen((const char *)parstProgInfo->aru8ServiceName);
            */

            u8ProviderNameLength = parstProgInfo->u8ProviderNameLen;
            u8ServiceNameLength  = parstProgInfo->u8ServiceNameLen;

            // 节目、提供商名称长度为0，也需要构造OtherInfoLen
            if ((u8ProviderNameLength == 0) && (u8ServiceNameLength == 0)
               && (0 == parstProgInfo->u16OtherInfoLen))
            {
                continue;
            }

            // 有节目信息才需要构造SERVICE_DESCRIPTOR_TAG
            if ((0 != u8ProviderNameLength) || (0 != u8ServiceNameLength))
            {
                u16DescriptonLoopLength  = 5; // 5表示descriptor_tag(8)length(8)serv_type(8)provi_name_len(8)serv_name_len(8)
                u16DescriptonLoopLength += u8ProviderNameLength;
                u16DescriptonLoopLength += u8ServiceNameLength;
            }
            else
            {
                u16DescriptonLoopLength  = 0;
            }

            u16DescriptonLoopLength += parstProgInfo->u16OtherInfoLen;

            // 超过最大分段长度，则进行此次封装, // service_id(16)到descriptors_loop_length(12)共5个字节
            if (MAX_SI_SECTION_LEN < (u16SectionBufLen + u16DescriptonLoopLength + 5))
            {
                break;
            }

            // service_id(16)到descriptors_loop_length(12)共5个字节
            u16SectionBufLen += 5;
            u16SectionBufLen += u16DescriptonLoopLength;

            //service_id    16bits
            pu8Temp[u16Index] = parstProgInfo->u16ServiceID >> 8;
            pu8Temp[u16Index] = pu8Temp[u16Index] & 0xff;
            pu8Temp[u16Index+1] = parstProgInfo->u16ServiceID & 0xff;

            // reserved_future_use   6bits
            // EIT_schedule_flag     1bit
            // EIT_present_following_flag    1bit
            pu8Temp[u16Index+2] = 0xfc | (U8)(parstProgInfo->u8EITScheduleFlag << 1) | (parstProgInfo->u8EITPresentFollowingFlag);

            // running_status    3bits
            // free_CA_mode      1bit
            // descripton_loop_length  12bits
            pu8Temp[u16Index+3] = ((parstProgInfo->u8RunningStatus & 0x07)<<5)
                                | ((parstProgInfo->u8FreeCAMode  & 0x01)<<4)
                                | ((u16DescriptonLoopLength & 0x0FFF)>>8);

            pu8Temp[u16Index+4] = (u16DescriptonLoopLength & 0x0FFF) & 0xff;

            u16DescriptorIdx = u16Index + 5;

            if ((0 != u8ProviderNameLength) || (0 != u8ServiceNameLength))
            {
                // descriptor, here we only add the service_descriptor to the output TS stream.
                // descriptor_tag
                pu8Temp[u16DescriptorIdx] = SERVICE_DESCRIPTOR_TAG;

                // descriptor_length
                pu8Temp[u16DescriptorIdx+1] = 3 + u8ProviderNameLength + u8ServiceNameLength;

                // service_type
                pu8Temp[u16DescriptorIdx+2] = parstProgInfo->u8ServiceType;


                // service_provider_name_length
                pu8Temp[u16DescriptorIdx+3] = u8ProviderNameLength;


                // 描述信息越界，返回失败，并记录日志
                if (MAX_SDT_NAME_LEN < u8ProviderNameLength)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                        "[%s:%d]u8ProviderNameLength:%d out of range\r\n",
                        __FUNCTION__, __LINE__, u8ProviderNameLength);
                    return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
                }

                if (0 < u8ProviderNameLength)
                {
                    memcpy(&pu8Temp[u16DescriptorIdx+4], parstProgInfo->aru8ProviderName, u8ProviderNameLength);
                }

                u16DescriptorIdx = u16DescriptorIdx+4+u8ProviderNameLength;

                // service_name_length
                pu8Temp[u16DescriptorIdx] = u8ServiceNameLength;
                u16DescriptorIdx++;

                // 描述信息越界，返回失败，并记录日志
                if (MAX_SDT_NAME_LEN < u8ServiceNameLength)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                        "[%s:%d]u8ServiceNameLength:%d out of range\r\n",
                        __FUNCTION__, __LINE__, u8ServiceNameLength);
                    return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
                }

                if (0 < u8ServiceNameLength)
                {
                    memcpy(&pu8Temp[u16DescriptorIdx], parstProgInfo->aru8ServiceName, u8ServiceNameLength);
                }

                u16DescriptorIdx += u8ServiceNameLength;

                if (MAX_SDT_OTHER_INFO_LEN < parstProgInfo->u16OtherInfoLen)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                        "[%s:%d]u16OtherInfoLen:%d out of range\r\n",
                        __FUNCTION__, __LINE__, parstProgInfo->u16OtherInfoLen);
                    return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
                }
            }

            // add other descriptor
            if (0 < parstProgInfo->u16OtherInfoLen)
            {
                memcpy(&pu8Temp[u16DescriptorIdx], parstProgInfo->aru8OtherInfo, parstProgInfo->u16OtherInfoLen);

                u16DescriptorIdx += parstProgInfo->u16OtherInfoLen;
            }

            u16Index = u16DescriptorIdx;
        }

        // u16SectionLength从reserved(18)开始计算，并包括CRC
        u16SectionLength = u16Index + 4 - 3;
        pu8Temp[1] = ((u16SectionLength >> 8) & 0x0f) | 0xf0;
        pu8Temp[2] = u16SectionLength &0xff;

        // CRC_32 从table_id开始计算，不包括CRC字段
        u32CRC = Common_CRCCalculate(&pu8Temp[0], u16SectionLength+3-4,0);
        pu8Temp[u16Index]   = (u32CRC & 0xff000000) >> 24;
        pu8Temp[u16Index+1] = (u32CRC & 0x00ff0000) >> 16;
        pu8Temp[u16Index+2] = (u32CRC & 0x0000ff00) >> 8;
        pu8Temp[u16Index+3] = u32CRC  & 0x000000ff;

        u16Index += 4;

        *pu16SectionLen += u16Index;


        // 保存接下来需要分析的节目偏移号
        u16ServiceNum = j;

        // 需要将下一部分的sdt section信息按顺序写入
        pu8Temp    = &pu8Temp[u16Index];
        u16Index = 0;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     SIP_ConstructOtherSDT
  Description:  构造出Other SDT表Section
  Input:        pstSdtTable     －  SDT表结构
                u16BufLen       －  输入buff的大小
  Output:       pu8SectionBuf   －  Section存放缓冲区
                pu16SectionLen  －  Section长度指针
  Return:       SMP_SUCCESS or FAIL
  Others:       构造的section从table id开始，不包含pointer_field;多个section时，
                每个section在pu8SectionBuf里连续存放，需调用者识别
  Create:       2013.09.16 ziteng.yang
*****************************************************************************/
wvErrCode SIP_ConstructOtherSDT(const SDTTable_t *pstSdtTable, U16 u16BufLen, U8 *pu8SectionBuf, U16 *pu16SectionLen)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8   u8ProviderNameLength    = 0;
    U8   u8ServiceNameLength     = 0;
    U8   u8SectionNum            = 0;
    U16  u16ServiceNum           = 0;
    U16  u16SectionLength        = 0;
    U16  u16DescriptonLoopLength = 0;
    U16  i                = 0;
    U16  j                = 0;
    U32  u32CRC           = 0;
    U16  u16Index         = 0;
    U16  u16DescriptorIdx = 0;
    U16  u16SdtTableLen   = 0;

    U8   *pu8Temp = pu8SectionBuf;
    const SDTProgramInfo_t *parstProgInfo = NULL;

    U16 u16SectionBufLen = 0;  // 用于记录每一个分段的总字节数

    if ((!pstSdtTable) || (!pu8SectionBuf) || (!pu16SectionLen))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]Input Error,pstSdtTable[%p],pu8SectionBuf[%p],pu8SectionBuf[%p]\r\n",
            __FUNCTION__, __LINE__, pstSdtTable, pu8SectionBuf, pu8SectionBuf);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    *pu16SectionLen = 0;

    enErrCode = SIP_GetSDTSectionInfo(pstSdtTable, &u8SectionNum, &u16SdtTableLen);
    if ((WV_SUCCESS != enErrCode) || (u16BufLen < u16SdtTableLen))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]u16BufLen[%u],u16SdtTableLen[%u],enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, u16BufLen, u16SdtTableLen, enErrCode);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    // 一个分段都没有，返回失败
    if (0 == u8SectionNum)
    {
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    for (i = 0; i < u8SectionNum; i++)
    {
        // table_id  8bits
        pu8Temp[0] = SDT_TID_OTHER;

        // section_syntax_indicaor   1bit    1
        // '0'                       1bit    0
        // reserved                  2bits   xx
        // section_length            12bits

        // transport_stream_id 16bit
        pu8Temp[3] = (pstSdtTable->u16TransportStreamID>>8) & 0xff;
        pu8Temp[4] = pstSdtTable->u16TransportStreamID & 0xff;

        // reserved                  2bit        0b11
        // version_number            5bit        0b00000
        // current_next_indicator    1bit        0b1
        pu8Temp[5] = (0xc1 | ((pstSdtTable->u8VersionNum&0x1f)<<1));

        // section_number            8bit
        pu8Temp[6] = (U8)i;     // 从0开始计数

        // last_section_number       8bit
        pu8Temp[7] = u8SectionNum-1;

        // original_network_id   16bit
        pu8Temp[8] = (pstSdtTable->u16OriginalNetworkId >> 8) & 0xff ;
        pu8Temp[9] = pstSdtTable->u16OriginalNetworkId & 0xff;

        // reserved future used 8bits
        pu8Temp[10] = 0xff;

        // table id 到reserved_future_use(8) 总共11字节, CRC 4个字节
        u16SectionBufLen = 11 + 4;

        // 偏移到第一个service_id起始位置
        u16Index = 11;

        for (j=u16ServiceNum; j<pstSdtTable->u16ServiceNum; j++)
        {
            parstProgInfo = &pstSdtTable->arstProgInfo[j];
            /*
            u8ProviderNameLength = (U8)strlen((const char *)parstProgInfo->aru8ProviderName);
            u8ServiceNameLength  = (U8)strlen((const char *)parstProgInfo->aru8ServiceName);
            */

            u8ProviderNameLength = parstProgInfo->u8ProviderNameLen;
            u8ServiceNameLength  = parstProgInfo->u8ServiceNameLen;

            // 节目、提供商名称长度为0，也需要构造OtherInfoLen
            if ((u8ProviderNameLength == 0) && (u8ServiceNameLength == 0)
               && (0 == parstProgInfo->u16OtherInfoLen))
            {
                continue;
            }

            // 有节目信息才需要构造SERVICE_DESCRIPTOR_TAG
            if ((0 != u8ProviderNameLength) || (0 != u8ServiceNameLength))
            {
                u16DescriptonLoopLength  = 5; // 5表示descriptor_tag(8)length(8)serv_type(8)provi_name_len(8)serv_name_len(8)
                u16DescriptonLoopLength += u8ProviderNameLength;
                u16DescriptonLoopLength += u8ServiceNameLength;
            }
            else
            {
                u16DescriptonLoopLength  = 0;
            }

            u16DescriptonLoopLength += parstProgInfo->u16OtherInfoLen;

            // 超过最大分段长度，则进行此次封装, // service_id(16)到descriptors_loop_length(12)共5个字节
            if (MAX_SI_SECTION_LEN < (u16SectionBufLen + u16DescriptonLoopLength + 5))
            {
                break;
            }

            // service_id(16)到descriptors_loop_length(12)共5个字节
            u16SectionBufLen += 5;
            u16SectionBufLen += u16DescriptonLoopLength;

            //service_id    16bits
            pu8Temp[u16Index] = parstProgInfo->u16ServiceID >> 8;
            pu8Temp[u16Index] = pu8Temp[u16Index] & 0xff;
            pu8Temp[u16Index+1] = parstProgInfo->u16ServiceID & 0xff;

            // reserved_future_use   6bits
            // EIT_schedule_flag     1bit
            // EIT_present_following_flag    1bit
            pu8Temp[u16Index+2] = 0xfc | (U8)(parstProgInfo->u8EITScheduleFlag << 1) | (parstProgInfo->u8EITPresentFollowingFlag);

            // running_status    3bits
            // free_CA_mode      1bit
            // descripton_loop_length  12bits
            pu8Temp[u16Index+3] = ((parstProgInfo->u8RunningStatus & 0x07)<<5)
                                | ((parstProgInfo->u8FreeCAMode  & 0x01)<<4)
                                | ((u16DescriptonLoopLength & 0x0FFF)>>8);

            pu8Temp[u16Index+4] = (u16DescriptonLoopLength & 0x0FFF) & 0xff;

            u16DescriptorIdx = u16Index + 5;

            if ((0 != u8ProviderNameLength) || (0 != u8ServiceNameLength))
            {
                // descriptor, here we only add the service_descriptor to the output TS stream.
                // descriptor_tag
                pu8Temp[u16DescriptorIdx] = SERVICE_DESCRIPTOR_TAG;

                // descriptor_length
                pu8Temp[u16DescriptorIdx+1] = 3 + u8ProviderNameLength + u8ServiceNameLength;

                // service_type
                pu8Temp[u16DescriptorIdx+2] = parstProgInfo->u8ServiceType;


                // service_provider_name_length
                pu8Temp[u16DescriptorIdx+3] = u8ProviderNameLength;


                // 描述信息越界，返回失败，并记录日志
                if (MAX_SDT_NAME_LEN < u8ProviderNameLength)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                        "[%s:%d]u8ProviderNameLength:%d out of range\r\n",
                        __FUNCTION__, __LINE__, u8ProviderNameLength);
                    return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
                }

                if (0 < u8ProviderNameLength)
                {
                    memcpy(&pu8Temp[u16DescriptorIdx+4], parstProgInfo->aru8ProviderName, u8ProviderNameLength);
                }

                u16DescriptorIdx = u16DescriptorIdx+4+u8ProviderNameLength;

                // service_name_length
                pu8Temp[u16DescriptorIdx] = u8ServiceNameLength;
                u16DescriptorIdx++;

                // 描述信息越界，返回失败，并记录日志
                if (MAX_SDT_NAME_LEN < u8ServiceNameLength)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                        "[%s:%d]u8ServiceNameLength:%d out of range\r\n",
                        __FUNCTION__, __LINE__, u8ServiceNameLength);
                    return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
                }

                if (0 < u8ServiceNameLength)
                {
                    memcpy(&pu8Temp[u16DescriptorIdx], parstProgInfo->aru8ServiceName, u8ServiceNameLength);
                }

                u16DescriptorIdx += u8ServiceNameLength;

                if (MAX_SDT_OTHER_INFO_LEN < parstProgInfo->u16OtherInfoLen)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                        "[%s:%d]u16OtherInfoLen:%d out of range\r\n",
                        __FUNCTION__, __LINE__, parstProgInfo->u16OtherInfoLen);
                    return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
                }
            }

            // add other descriptor
            if (0 < parstProgInfo->u16OtherInfoLen)
            {
                memcpy(&pu8Temp[u16DescriptorIdx], parstProgInfo->aru8OtherInfo, parstProgInfo->u16OtherInfoLen);

                u16DescriptorIdx += parstProgInfo->u16OtherInfoLen;
            }

            u16Index = u16DescriptorIdx;
        }

        // u16SectionLength从reserved(18)开始计算，并包括CRC
        u16SectionLength = u16Index + 4 - 3;
        pu8Temp[1] = ((u16SectionLength >> 8) & 0x0f) | 0xf0;
        pu8Temp[2] = u16SectionLength &0xff;

        // CRC_32 从table_id开始计算，不包括CRC字段
        u32CRC = Common_CRCCalculate(&pu8Temp[0], u16SectionLength+3-4,0);
        pu8Temp[u16Index]   = (u32CRC & 0xff000000) >> 24;
        pu8Temp[u16Index+1] = (u32CRC & 0x00ff0000) >> 16;
        pu8Temp[u16Index+2] = (u32CRC & 0x0000ff00) >> 8;
        pu8Temp[u16Index+3] = u32CRC  & 0x000000ff;

        u16Index += 4;

        *pu16SectionLen += u16Index;

        // 保存接下来需要分析的节目偏移号
        u16ServiceNum = j;

        // 需要将下一部分的sdt section信息按顺序写入
        pu8Temp    = &pu8Temp[u16Index];
        u16Index = 0;
    }
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     SIP_ConstructBAT
  Description:  构造出BAT表Section
  Input:        pstBATTable     －  BAT表结构
                u16BufLen       －  输入buff的大小
  Output:       pu8SectionBuf   －  Section存放缓冲区
                pu16SectionLen  －  Section长度指针
  Return:       SUCCESS or FAIL
  Others:       构造的section从table id开始，不包含pointer_field;多个section时，
                每个section在pu8SectionBuf里连续存放，需调用者识别
  Create:       2014.07.29 ziteng.yang
*****************************************************************************/
wvErrCode SIP_ConstructBAT(const BATTable_t *pstBATTable, U16 u16BufLen, U8 *pu8SectionBuf, U16 *pu16SectionLen)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U32 u32CRC           = 0;
    U16 u16TotalBuffLen  = 0;  // 记录整个bat表需要的buff大小
    U16 u16SectionBufLen = 0;  // 用于记录每一个分段的总字节数
    U16 u16SectionLength = 0;
    U16 u16TSLoopLength  = 0;
    U16 u16TSInfoNum     = 0;
    U16 u16TSInfoLen     = 0;
    U16 u16Index         = 0;
    U16 i                = 0;
    U16 j                = 0;
    U8  u8SectionNum     = 0;

    U8  *pu8Temp = pu8SectionBuf;
    U8  *pu8TSLoopLength = NULL; // 用于指向transport_stream_loop_length    12

    const TSInfoStruct_t *pstTSInfo = NULL;

    if ((!pstBATTable) || (!pu8SectionBuf) || (!pu16SectionLen))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]Input Error,pstBATTable[%p],pu8SectionBuf[%p],pu8SectionBuf[%p]\r\n",
            __FUNCTION__, __LINE__, pstBATTable, pu8SectionBuf, pu8SectionBuf);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    *pu16SectionLen = 0;

    enErrCode = SIP_GetBATSectionInfo(pstBATTable, &u8SectionNum, &u16TotalBuffLen);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
            "[%s:%d]SIP_GetBATSectionInfo error\r\n", __FUNCTION__, __LINE__);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    // 一个分段都没有或者传入的缓冲区不够，返回失败
    if ((0 == u8SectionNum) || (u16BufLen < u16TotalBuffLen))
    {
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    /*
    Bouquet Association Table (BAT)
    bouquet_association_section(){
    table_id                        8
    section_syntax_indicator        1
    reserved_future_use             1
    reserved                        2
    section_length                  12
    bouquet_id                      16
    reserved                        2
    version_number                  5
    current_next_indicator          1
    section_number                  8
    last_section_number             8
    reserved_future_use             4
    bouquet_descriptors_length      12
    for(i=0;i<N;i++)
    {
      descriptor()
    }

    reserved_future_use             4
    transport_stream_loop_length    12
    for(i=0;i<N;i++)
    {
      transport_stream_id            16
      original_network_id            16
      reserved_future_use            4
      transport_descriptors_length   12
      for(j=0;j<N;j++)
      {
          descriptor()
      }
    }
    CRC_32                              32
    }
    */

    // 根据计算出的section个数进行组装，
    // 因为SIP_GetBATSectionInfo已经有参数的合法性校验，这里可以不需要校验
    for (i = 0; i < u8SectionNum; i++)
    {
        // table_id  8bits
        pu8Temp[0] = BAT_TID;

        // section_syntax_indicaor   1bit    1
        // '0'                       1bit    0
        // reserved                  2bits   xx
        // section_length            12bits

        // bouquet_id                      16
        pu8Temp[3] = (U8)((pstBATTable->u16BouquetId>>8) & 0xff);
        pu8Temp[4] = (U8)((pstBATTable->u16BouquetId) & 0xff);

        // reserved                  2bit        0b11
        // version_number            5bit        0b00000
        // current_next_indicator    1bit        0b1
        pu8Temp[5] = (0xc1 | ((pstBATTable->u8VersionNum&0x1f)<<1));

        // section_number(8)last_section_number(8)
        pu8Temp[6] = (U8)i;
        pu8Temp[7] = (U8)(u8SectionNum - 1);

        //reserved_future_use             4
        //bouquet_descriptors_length      12
        if (0 == i) // 第一个section才记录bouquet_descriptors
        {
            pu8Temp[8] = (U8)((pstBATTable->u16BouquetInfoLen>>8) | 0xf0);
            pu8Temp[9] = (U8)((pstBATTable->u16BouquetInfoLen) & 0xff);

            // descriptor()
            if (0 < pstBATTable->u16BouquetInfoLen)
            {
                memcpy(&pu8Temp[10], pstBATTable->aru8BouquetInfo, pstBATTable->u16BouquetInfoLen);
            }

            // 偏移到reserved_future_use(4)transport_stream_loop_length(12)起始位置
            u16Index = 10 + pstBATTable->u16BouquetInfoLen;
        }
        else
        {
            pu8Temp[8] = 0xf0;
            pu8Temp[9] = 0;

            // 偏移到reserved_future_use(4)transport_stream_loop_length(12)起始位置
            u16Index = 10;
        }

        pu8TSLoopLength = &pu8Temp[u16Index];
        u16TSLoopLength = 0;

        // 跳过reserved_future_use(4)transport_stream_loop_length(12)
        u16Index += 2;

        // 4表示需要加上CRC
        u16SectionBufLen = u16Index + 4;

        for (j = u16TSInfoNum; j < pstBATTable->u16TransportStreamNum; j++)
        {
            pstTSInfo = &pstBATTable->arstTSInfo[j];

            // 6表示transport_stream_id(16)original_network_id(16)
            // reserved_future_use(4)transport_descriptors_length(12)
            u16TSInfoLen = 6 + pstTSInfo->u16DescriptorsLen;

            if (MAX_SI_SECTION_LEN < (u16TSInfoLen + u16SectionBufLen))
            {
                break;
            }

            // transport_stream_id(16)
            pu8Temp[u16Index]   = (U8)((pstTSInfo->u16TSID>>8) & 0xff);
            pu8Temp[u16Index+1] = (U8)((pstTSInfo->u16TSID) & 0xff);
            u16Index += 2;

            // original_network_id(16)
            pu8Temp[u16Index]   = (U8)((pstTSInfo->u16OriginalNetId>>8) & 0xff);
            pu8Temp[u16Index+1] = (U8)((pstTSInfo->u16OriginalNetId) & 0xff);
            u16Index += 2;

            // reserved_future_use(4)transport_descriptors_length(12)
            pu8Temp[u16Index]   = (U8)((pstTSInfo->u16DescriptorsLen>>8) | 0xf0);
            pu8Temp[u16Index+1] = (U8)((pstTSInfo->u16DescriptorsLen) & 0xff);
            u16Index += 2;

            if (0 < pstTSInfo->u16DescriptorsLen)
            {
                memcpy(&pu8Temp[u16Index], pstTSInfo->aru8Descriptors, pstTSInfo->u16DescriptorsLen);
                u16Index += pstTSInfo->u16DescriptorsLen;
            }

            u16TSLoopLength  += u16TSInfoLen;
            u16SectionBufLen += u16TSInfoLen;
        }

        // 重新计算reserved_future_use(4)transport_stream_loop_length(12)
        pu8TSLoopLength[0] = (U8)((u16TSLoopLength>>8) | 0xf0);
        pu8TSLoopLength[1] = (U8)((u16TSLoopLength) & 0xff);

        // u16SectionLength从reserved(18)开始计算，并包括CRC
        u16SectionLength = u16Index + 4 - 3;
        pu8Temp[1] = ((u16SectionLength >> 8) & 0x0f) | 0xf0;
        pu8Temp[2] = u16SectionLength &0xff;

        // CRC_32 从table_id开始计算，不包括CRC字段
        u32CRC = Common_CRCCalculate(&pu8Temp[0], u16SectionLength+3-4,0);
        pu8Temp[u16Index]   = (u32CRC & 0xff000000) >> 24;
        pu8Temp[u16Index+1] = (u32CRC & 0x00ff0000) >> 16;
        pu8Temp[u16Index+2] = (u32CRC & 0x0000ff00) >> 8;
        pu8Temp[u16Index+3] = u32CRC  & 0x000000ff;

        u16Index += 4;

        *pu16SectionLen += u16Index;

        // 保存接下来需要分析的TS流信息号
        u16TSInfoNum = j;

        // 需要将下一部分的bat section信息按顺序写入
        pu8Temp    = &pu8Temp[u16Index];
        u16Index = 0;
    }

    return WV_SUCCESS;
}

void SIP_Init(void)
{
    SSD_InitBroadcastSI();
}

/*****************************************************************************
  Function:     SIPacket_Service
  Description:  SI Packet pthread 
  Input:        none
  Output:       none
  Return:       
  Author:       lipeng
*****************************************************************************/
void SIPacket_Service()
{
    U32 u32lastTimerCnt = 0;
    while(1)
    {
        if(GetTimeCnt() != u32lastTimerCnt)
        {
            SSD_MutexLock();
            SSD_SendBroadcastSI();
            SSD_MutexUnlock();

            u32lastTimerCnt = GetTimeCnt();
        }
        
        usleep(10000);
    }
}

/*****************************************************************************
  Function:     SIPacket_Start
  Description:  
  Input:        none
  Output:       none
  Return:       
  Author:       lipeng
*****************************************************************************/
U32 SIPacket_Start(void)
{
    int res = 0;
    pthread_t SIPacket_thread;

    res = pthread_create(&SIPacket_thread, NULL, (void*)SIPacket_Service, NULL);
    if(-1 == res)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "SIPacket thread create error");
        return WV_ERR_COMM_PTHREAD_CREAT;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "SIPacket create successfully!");
    }

    res = pthread_detach(SIPacket_thread);
    if(-1 == res)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "SIPacket thread detach error");
        return WV_ERR_COMM_PTHREAD_CREAT;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "SIPacket pthread detach successfully!");
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     SIP_Init
  Description:  start SI Table and Timer pthread
  Input:        none
  Output:       none
  Return:       
  Author:       lipeng
*****************************************************************************/
void SIP_SendSIServer()
{
    Timer_Start(100 * 1000);//100ms
    SIPacket_Start();
}


