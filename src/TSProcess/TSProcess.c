/**********************************************************************
* Copyright(c), HuiZhou WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName ：TSProcess.c
* Description ：MuxProcess
*
* Version ：1.0.0
* Author ：Momouwei   Reviewer :
* Date   ：2017.02.22
*
**********************************************************************/
#include "TSProcess.h"
#include "TSConstruct.h"
#include "SIProcess.h"
#include "FPGA.h"
#include "WV_log.h"
#include "tools.h"
#include "tuner.h"

//搜台时使用
static U8 s_aru8SectionBuffer[MAX_TS_SCANNER_NUM][MAX_SI_SECTION_LEN];
static PATTable_t s_arstPATInfo[MAX_TS_SCANNER_NUM];
static PMTTable_t s_arstPMTInfo[MAX_TS_SCANNER_NUM];
static CATTable_t s_arstCATInfo[MAX_TS_SCANNER_NUM];
static SDTTable_t s_arstSDTInfo[MAX_TS_SCANNER_NUM];
static NITTable_t s_arstNITInfo[MAX_TS_SCANNER_NUM];
static BATTable_t s_arstBATInfo[MAX_TS_SCANNER_NUM];
static TSInfo s_arstTempTSParam[MAX_TS_SCANNER_NUM];

/*****************************************************************************
  Function:       TSP_GetSectionBuffer
  Description:    获取Section缓冲区，搜流时使用，每一个搜流模块对应一个Section缓冲区
  Input:          u8TSScannerIndex:流搜索模块索引
  Output:         
  Return:         
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
U8 *TSP_GetSectionBuffer(U8 u8TSScannerIndex)
{
    if (u8TSScannerIndex >= MAX_TS_SCANNER_NUM)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex);
        return NULL;
    }
    
    return s_aru8SectionBuffer[u8TSScannerIndex];
}

/*****************************************************************************
  Function:       TSP_GetPATBuffer
  Description:    获取PAT缓冲区，搜流时使用，每一个搜流模块对应一个PAT缓冲区
  Input:          u8TSScannerIndex:流搜索模块索引
  Output:         
  Return:         
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
PATTable_t *TSP_GetPATBuffer(U8 u8TSScannerIndex)
{
    if (u8TSScannerIndex >= MAX_TS_SCANNER_NUM)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex);
        return NULL;
    }
    
    return &s_arstPATInfo[u8TSScannerIndex];
}

/*****************************************************************************
  Function:       TSP_GetPMTBuffer
  Description:    获取PMT缓冲区，搜流时使用，每一个搜流模块对应一个PMT缓冲区
  Input:          u8TSScannerIndex:流搜索模块索引
  Output:         
  Return:         
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
PMTTable_t *TSP_GetPMTBuffer(U8 u8TSScannerIndex)
{
    if (u8TSScannerIndex >= MAX_TS_SCANNER_NUM)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex);
        return NULL;
    }
    
    return &s_arstPMTInfo[u8TSScannerIndex];
}

/*****************************************************************************
  Function:       TSP_GetCATBuffer
  Description:    获取CAT缓冲区，搜流时使用，每一个搜流模块对应一个CAT缓冲区
  Input:          u8TSScannerIndex:流搜索模块索引
  Output:         
  Return:         
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
CATTable_t *TSP_GetCATBuffer(U8 u8TSScannerIndex)
{
    if (u8TSScannerIndex >= MAX_TS_SCANNER_NUM)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex);
        return NULL;
    }
    
    return &s_arstCATInfo[u8TSScannerIndex];
}

/*****************************************************************************
  Function:       TSP_GetSDTBuffer
  Description:    获取SDT缓冲区，搜流时使用，每一个搜流模块对应一个SDT缓冲区
  Input:          u8TSScannerIndex:流搜索模块索引
  Output:         
  Return:         
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
SDTTable_t *TSP_GetSDTBuffer(U8 u8TSScannerIndex)
{
    if (u8TSScannerIndex >= MAX_TS_SCANNER_NUM)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex);
        return NULL;
    }
    
    return &s_arstSDTInfo[u8TSScannerIndex];
}

/*****************************************************************************
  Function:       TSP_GetNITBuffer
  Description:    获取NIT缓冲区，搜流时使用，每一个搜流模块对应一个NIT缓冲区
  Input:          u8TSScannerIndex:流搜索模块索引
  Output:         
  Return:         
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
NITTable_t *TSP_GetNITBuffer(U8 u8TSScannerIndex)
{
    if (u8TSScannerIndex >= MAX_TS_SCANNER_NUM)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex);
        return NULL;
    }
    
    return &s_arstNITInfo[u8TSScannerIndex];
}

/*****************************************************************************
  Function:       TSP_GetBATBuffer
  Description:    获取BAT缓冲区，搜流时使用，每一个搜流模块对应一个BAT缓冲区
  Input:          u8TSScannerIndex:流搜索模块索引
  Output:         
  Return:         
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
BATTable_t *TSP_GetBATBuffer(U8 u8TSScannerIndex)
{
    if (u8TSScannerIndex >= MAX_TS_SCANNER_NUM)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex);
        return NULL;
    }
    
    return &s_arstBATInfo[u8TSScannerIndex];
}

/*****************************************************************************
  Function:       TSP_GetTempTSInfo
  Description:    获取临时的流信息结构指针，搜流时使用，每一个搜流模块对应一个临时的流信息结构
  Input:          u8TSScannerIndex:流搜索模块索引
  Output:         
  Return:         
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
TSInfo *TSP_GetTempTSInfo(U8 u8TSScannerIndex)
{
    if (u8TSScannerIndex >= MAX_TS_SCANNER_NUM)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex);
        return NULL;
    }
    
    return &s_arstTempTSParam[u8TSScannerIndex];
}

/*****************************************************************************
  Function:     TSP_InitTempTSInfo
  Description:  初始化用于搜流的临时流结构信息
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
wvErrCode TSP_InitTempTSInfo(void)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 i = 0;
    TSInfo *pstTempTS = NULL;
    TSInfoInitNum stTSInfoInitNum;
    memset(&stTSInfoInitNum, 0, sizeof(TSInfoInitNum));
    stTSInfoInitNum.u32TSNumber = INIT_TEMP_TS_NUM;
    stTSInfoInitNum.u32ProgramNumber = INIT_TEMP_PROG_NUM;
    stTSInfoInitNum.u32PIDNumber = INIT_TEMP_PID_NUM;
    stTSInfoInitNum.u32TableNumber = INIT_TEMP_TABLE_NUM;
    stTSInfoInitNum.u32DescriptorNumber = INIT_TEMP_DESCRIPTOR_NUM;

    for (i = 0; i < MAX_TS_SCANNER_NUM; i++)
    {
        pstTempTS = TSP_GetTempTSInfo(i);

        enErrCode = TSP_InitTSInfo(pstTempTS, &stTSInfoInitNum);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
                "[%s:%d]TSP_InitTSInfo error\r\n", __FUNCTION__, __LINE__);
            return enErrCode;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_DistributeTSScanner
  Description:    分配流搜索模块
  Input:          u16Channel:通道号
  Output:         u8TSScannerIndex:流搜索模块索引
  Return:         成功时返回SUCCESS，错误时返回对应的错误码
  Others:         逻辑有4个流搜索模块，可以同时搜索4个通道的流
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_DistributeTSScanner(U16 u16Channel, U8 *pu8TSScannerIndex)
{
    if ((MAX_INPUT_CHANNEL_NUM <= u16Channel) || (!pu8TSScannerIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u16Channel[%u],pu8TSScannerIndex[%p]\r\n",
            __FUNCTION__, __LINE__, u16Channel, pu8TSScannerIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    //目前C子板只有4个通道，可以直接每个通道对应一个流搜索模块
    *pu8TSScannerIndex = u16Channel;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_GetAllPIDInChannel
  Description:    获取一个通道的所有PID
  Input:          u16Channel:通道号
                  u8TSScannerIndex:流搜索模块索引
  Output:         pstPIDInfo:搜索到的所有PID
  Return:         成功时返回SUCCESS，错误时返回对应的错误码
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_GetAllPIDInChannel(U16 u16Channel, U8 u8TSScannerIndex, PIDINFO *pstPIDInfo)
{
    U16 i = 0;
    U16 j = 0;
    U32 aru32PIDExistFlag[256] = {0};//每一位代表一个PID,256 * 32 = 8192

    if ((MAX_INPUT_CHANNEL_NUM <= u16Channel)
        || (MAX_TS_SCANNER_NUM <= u8TSScannerIndex) || (!pstPIDInfo))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u16Channel[%u],u8TSScannerIndex[%u],pstPIDInfo[%p]\r\n",
            __FUNCTION__, __LINE__, u16Channel, u8TSScannerIndex, pstPIDInfo);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    //告诉逻辑要获取哪个通道的所有PID
    FPGA_REG_Write(CHANNEL_CONFIG_0 + u8TSScannerIndex, TSP_InputChannel2StreamID(u16Channel));

    //清空PID存在位
    FPGA_REG_Write(PID_EXIST_FLAG_CLEAR_EN_0 + u8TSScannerIndex, 0);
    FPGA_REG_Write(PID_EXIST_FLAG_CLEAR_EN_0 + u8TSScannerIndex, 1);
    FPGA_REG_Write(PID_EXIST_FLAG_CLEAR_EN_0 + u8TSScannerIndex, 0);

    //等待足够长时间
    sleep(1);//后面改成时间可调

    //读取PID存在位
    for (i = 0; i < 256; i++)
    {
        FPGA_REG_Write(PID_EXIST_FLAG_ADDR_0 + u8TSScannerIndex, i);
        FPGA_REG_Read(PID_EXIST_FLAG_READ_BACK_0 + u8TSScannerIndex, &aru32PIDExistFlag[i]);
    }

    pstPIDInfo->u16PIDNum = 0;

    for (i = 0; i < 256; i++)
    {
        if (0 == aru32PIDExistFlag[i])
        {
            continue;
        }

        for (j = 0; j < 32; j++)
        {
            if (0 != (aru32PIDExistFlag[i] & (0x1 << j)))
            {
                pstPIDInfo->aru16PIDList[pstPIDInfo->u16PIDNum] = i * 32 + j;
                pstPIDInfo->u16PIDNum++;
            }
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_IsPIDExist
  Description:    判断一个PID是否在给定的PID链表中
  Input:          u16PID:要判断的PID
                  pstPIDInfo:PID链表
  Output:         
  Return:         TRUE or FALSE
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
BOOL TSP_IsPIDExist(U16 u16PID, PIDINFO *pstPIDInfo)
{
    U16 i = 0;
    if (!pstPIDInfo)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstPIDInfo[%p]\r\n",
            __FUNCTION__, __LINE__, pstPIDInfo);
        return FALSE;
    }

    for (i = 0; i < pstPIDInfo->u16PIDNum; i++)
    {
        if (pstPIDInfo->aru16PIDList[i] == u16PID)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*****************************************************************************
  Function:       TSP_IsExistNewPacket
  Description:    判断是否有新的TS包
  Input:          u8TSScannerIndex:流搜索模块索引
  Output:         
  Return:         TRUE or FALSE
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
BOOL TSP_IsExistNewPacket(U8 u8TSScannerIndex)
{
    U32 u32PacketCount = 0;
    
    if (u8TSScannerIndex >= MAX_TS_SCANNER_NUM)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex);
        return FALSE;
    }

    FPGA_REG_Read(PACKET_NUM_0 + u8TSScannerIndex, &u32PacketCount);

    return (0 == u32PacketCount) ? FALSE : TRUE;
}

/*****************************************************************************
  Function:       TSP_GetTSPacket
  Description:    从逻辑中获取一个TS包（包括8字节包头）
  Input:          u8TSScannerIndex:流搜索模块索引
                  u32Len:aru8TSPacket的长度
  Output:         aru8TSPacket:TS包
  Return:         TRUE or FALSE
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_GetTSPacket(U8 u8TSScannerIndex, U8 aru8TSPacket[], U32 u32Len)
{
    U16 i = 0;
    U16 u16Index = 0;
    U32 u32Packet = 0;

    if ((MAX_TS_SCANNER_NUM <= u8TSScannerIndex) || (!aru8TSPacket)
        || (u32Len < PACKET_LENGTH_WELLAV))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u],aru8TSPacket[%p],u32Len[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex, aru8TSPacket, u32Len);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    for (i = 0; i < PACKET_LENGTH_WELLAV_32BIT; i++)
    {
        FPGA_REG_Write(PACKET_ADDR_0 + u8TSScannerIndex, i);
        FPGA_REG_Read(PACKET_READ_BACK_0 + u8TSScannerIndex, &u32Packet);

        u16Index = 4 * i;

        aru8TSPacket[u16Index + 0] = (u32Packet >> 24) & 0xFF;
        aru8TSPacket[u16Index + 1] = (u32Packet >> 16) & 0xFF;
        aru8TSPacket[u16Index + 2] = (u32Packet >> 8) & 0xFF;
        aru8TSPacket[u16Index + 3] = u32Packet & 0xFF;
    }

    FPGA_REG_Write(READ_PACKET_DONE_FLAG_0 + u8TSScannerIndex, 0);
    FPGA_REG_Write(READ_PACKET_DONE_FLAG_0 + u8TSScannerIndex, 1);
    FPGA_REG_Write(READ_PACKET_DONE_FLAG_0 + u8TSScannerIndex, 0);

    for (i = 0; i < PACKET_LENGTH_WELLAV; i++)
    {
        if (0 == (i % 20) && (i != 0))
        {
            printf("\r\n");
        }
        
        printf("%02X ", aru8TSPacket[i]);
    }
    printf("\r\n");

    return WV_SUCCESS;
}
/*****************************************************************************
  Function:       TSP_FilterPSI
  Description:    过滤PSI/SI
  Input:          u8TSScannerIndex:流搜索模块索引
                  u8Standard:数字电视标准
                  u16PID:要过滤的PID
                  u8TID:要过滤的PID的TableID
                  u8SectionNo:PSI/SI第几个段
                  u16Serviceid:如果是PMT时，对应PMTPID的节目的ServiceID
                  u32Len:SectionBuffer的长度
  Output:         pu8SectionBuffer:获取到的段
  Return:         成功时返回SUCCESS，失败时返回对应错误码
  Others:         目前只处理DVB标准的流
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_FilterPSI(U8 u8TSScannerIndex, U8 u8Standard, U16 u16PID, U8 u8TID, U8 u8SectionNo,
                    U16 u16Serviceid, U8 *pu8SectionBuffer, U32 u32BufLen)

{
    U32 u32TimeOut = 3000;
    U32 u32ErrNum = 0;
    U8 aru8TSPacket[PACKET_LENGTH_WELLAV] = {0};
    U8 *pu8TSPkt = &aru8TSPacket[PACKET_HEADER_LEN];
    U8 u8PayloadUnitStartIndicator = 0;
    U8 u8ContinueCounter = 0;
    U8 u8AdaptationFieldControl = 0;
    U8 u8AdaptationFieldLength = 0;
    U8 u8PointerFiled = 0;
    U8 *pu8Payload = NULL;
    U8 u8PayloadLen = 0;
    U16 u16SectionLen = 0;
    U16 u16CopiedLen = 0;
    U16 u16NeedCopyLen = 0;
    BOOL bFirstPacketFlag = TRUE;
    
    if ((MAX_TS_SCANNER_NUM <= u8TSScannerIndex) || (!pu8SectionBuffer)
        || (u32BufLen > MAX_SI_SECTION_LEN))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u],pu8SectionBuffer[%p],u32BufLen[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex, pu8SectionBuffer, u32BufLen);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    //告诉逻辑要过滤哪个PID的TS包
    FPGA_REG_Write(PID_CONFIG_0 + u8TSScannerIndex, u16PID);

    //告诉逻辑要过滤对应PID的哪个TableID的TS包
    FPGA_REG_Write(TID_CONFIG_0 + u8TSScannerIndex, u8TID);

    //告诉逻辑配置已经更新
    FPGA_REG_Write(CONFIG_CHANGED_FLAG_0 + u8TSScannerIndex, 0);
    FPGA_REG_Write(CONFIG_CHANGED_FLAG_0 + u8TSScannerIndex, 1);
    FPGA_REG_Write(CONFIG_CHANGED_FLAG_0 + u8TSScannerIndex, 0);

    do
    {
        u32TimeOut--;

        if (!TSP_IsExistNewPacket(u8TSScannerIndex))
        {
            (void)usleep(1500);
            continue;
        }

        if(u32ErrNum > 500)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                "[%s:%d]ERR Packet Overflow!\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_TS_ERROR;
        }

        if (WV_SUCCESS != TSP_GetTSPacket(u8TSScannerIndex, aru8TSPacket, PACKET_LENGTH_WELLAV))
        {
            continue;
        }

        /*Check PID*/
        if ((pu8TSPkt[1] & 0x1F) != ((u16PID >> 8) & 0x1F)
            || (pu8TSPkt[2] & 0xFF) != (u16PID & 0xFF))
        {
            u32ErrNum++;
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                "[%s:%d]PID error,PID[%u][%u]\r\n",
                __FUNCTION__, __LINE__, u16PID, ((pu8TSPkt[1] & 0x1f) << 8) | pu8TSPkt[2]);
            continue;
        }
        
        /*Check transport error indicator*/
        if ((pu8TSPkt[1] & 0x80) != 0x00)
        {
            u32ErrNum++;
            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SIP,
                "[%s:%d]Transport Indicator ERR\r\n", __FUNCTION__, __LINE__);
            continue;
        }

        u8PayloadUnitStartIndicator = ((pu8TSPkt[1] & 0x40) == 0x40) ? 1 : 0;

        if (bFirstPacketFlag && (0 == u8PayloadUnitStartIndicator))
        {
            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SIP,
                "[%s:%d]Not The First Packet!\r\n", __FUNCTION__, __LINE__);
            continue;
        }

        u8AdaptationFieldControl = (pu8TSPkt[3] & 0x30) >> 4;

        if ((1 != u8AdaptationFieldControl) && (3 != u8AdaptationFieldControl))
        {
            //不含有净荷数据
            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SIP,
                "[%s:%d]No payload in packet,u8AdaptationFieldControl[%u]\r\n",
                __FUNCTION__, __LINE__, u8AdaptationFieldControl);
            continue;
        }

        pu8Payload = &pu8TSPkt[4];

        u8AdaptationFieldLength = 0;

        if (3 == u8AdaptationFieldControl)//既有适配字段也含有净荷数据
        {
            u8AdaptationFieldLength = pu8Payload[0];

            if (u8AdaptationFieldLength > 182)
            {
                u32ErrNum++;
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                    "[%s:%d]u8AdaptationFieldLength[%u] error!\r\n",
                    __FUNCTION__, __LINE__, u8AdaptationFieldLength);
                continue;
            }

            pu8Payload += (1 + u8AdaptationFieldLength);
        }

        if (1 == u8PayloadUnitStartIndicator)//含有PointerFiled
        {
            u8PointerFiled = pu8Payload[0];
            pu8Payload += (1 + u8PointerFiled);

            if (pu8Payload >= (pu8TSPkt + 188))
            {
                u32ErrNum++;
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                    "[%s:%d]PointerFiled[%u] error,u8AdaptationFieldLength[%u]!\r\n",
                    __FUNCTION__, __LINE__, u8AdaptationFieldLength, u8PointerFiled);
                continue;
            }
        }

        u8PayloadLen = (pu8TSPkt + 188) - pu8Payload;
        
        if (bFirstPacketFlag)
        {
            /*Check TableID & SectionNumber*/
            if ((pu8Payload[0] != u8TID) || (pu8Payload[6] != u8SectionNo))
            {
                u32ErrNum++;
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                    "[%s:%d]u8TID[%u][%u] or u8SectionNo[%u][%u],error!\r\n",
                    __FUNCTION__, __LINE__, u8TID, pu8Payload[0], u8SectionNo, pu8Payload[6]);
                continue;
            }

            u16SectionLen = ((pu8Payload[1] << 8) | pu8Payload[2]) & 0xFFF;

            if (u16SectionLen > 1021)//用宏
            {
                u32ErrNum++;
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                    "[%s:%d]u16SectionLen[%u],error!\r\n",
                    __FUNCTION__, __LINE__, u16SectionLen);
                continue;
            }

            //PMTPID
            if (u16PID > 0x0014)//用宏
            {
                if (u16Serviceid != ((pu8Payload[3] << 8) | pu8Payload[4]))
                {
                    u32ErrNum++;
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                        "[%s:%d]u16Serviceid[%u],error!\r\n",
                        __FUNCTION__, __LINE__, u16Serviceid);
                    continue;
                }
            }

            bFirstPacketFlag = FALSE;
            u8ContinueCounter = pu8TSPkt[3] & 0x0F;
        }
        else
        {
            if (((u8ContinueCounter + 1) & 0x0F) != (pu8TSPkt[3] & 0x0F))
            {
                u32ErrNum++;
                bFirstPacketFlag = TRUE;
                u16CopiedLen = 0;

                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                        "[%s:%d]u8ContinueCounter[%d][%d],error!\r\n",
                        __FUNCTION__, __LINE__, u8ContinueCounter, pu8TSPkt[3] & 0x0F);

                continue;
            }

            u8ContinueCounter = (u8ContinueCounter + 1) & 0x0F;
        }

        if ((u16CopiedLen + u8PayloadLen) > (u16SectionLen + 3))
        {
            u16NeedCopyLen = (u16SectionLen + 3 - u16CopiedLen);
        }
        else
        {
            u16NeedCopyLen = u8PayloadLen;
        }

        memcpy(pu8SectionBuffer + u16CopiedLen, pu8Payload, u16NeedCopyLen);
        u16CopiedLen += u16NeedCopyLen;

        if ((u16SectionLen + 3) == u16CopiedLen)
        {
            if (0 == Common_CRCCalculate(pu8SectionBuffer, u16SectionLen + 3, 0))
            {
                return WV_SUCCESS;
            }
            else
            {
                u32ErrNum++;
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]CRC error!\r\n", __FUNCTION__, __LINE__);
                return WV_ERR_TSP_CRC_ERROR;
            }
        }
        
    } while(u32TimeOut > 0);

    if (u32TimeOut == 0)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Time Out,u16PID[%u],u8TID[%u],u8SectionNo[%u],u16Serviceid[%u]!\r\n",
            __FUNCTION__, __LINE__, u16PID, u8TID, u8SectionNo, u16Serviceid);
        return WV_ERR_TSP_FILTER_PSI_TIMEOUT;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_GetPSILastSectionNum
  Description:    获取表的LastSectionNum
  Input:          u8TSScannerIndex:流搜索模块索引
                  u8Standard:数字电视标准
                  u16PID:要过滤的PID
                  u8TID:要过滤的PID的TableID
  Output:         pu8LastSectionNum:获取到表的LastSectionNum
  Return:         成功时返回SUCCESS，失败时返回对应错误码
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_GetPSILastSectionNum(U8 u8TSScannerIndex, U8 u8Standard,
                                    U16 u16PID, U8 u8TID, U8 *pu8LastSectionNum)
{
    U32 u32TimeOut = 3000;
    U32 u32ErrNum = 0;
    U8 aru8TSPacket[PACKET_LENGTH_WELLAV] = {0};
    U8 *pu8TSPkt = &aru8TSPacket[PACKET_HEADER_LEN];
    U8 u8PayloadUnitStartIndicator = 0;
    U8 u8AdaptationFieldControl = 0;
    U8 u8AdaptationFieldLength = 0;
    U8 u8PointerFiled = 0;
    U8 *pu8Payload = NULL;
    
    if ((MAX_TS_SCANNER_NUM <= u8TSScannerIndex) || (!pu8LastSectionNum))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u],pu8LastSectionNum[%p]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex, pu8LastSectionNum);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    //告诉逻辑要过滤哪个PID的TS包
    FPGA_REG_Write(PID_CONFIG_0 + u8TSScannerIndex, u16PID);

    //告诉逻辑要过滤对应PID的哪个TableID的TS包
    FPGA_REG_Write(TID_CONFIG_0 + u8TSScannerIndex, u8TID);

    //告诉逻辑配置已经更新
    FPGA_REG_Write(CONFIG_CHANGED_FLAG_0 + u8TSScannerIndex, 0);
    FPGA_REG_Write(CONFIG_CHANGED_FLAG_0 + u8TSScannerIndex, 1);
    FPGA_REG_Write(CONFIG_CHANGED_FLAG_0 + u8TSScannerIndex, 0);

    do
    {
        u32TimeOut--;

        if (!TSP_IsExistNewPacket(u8TSScannerIndex))
        {
            (void)usleep(1500);
            continue;
        }

        if(u32ErrNum > 500)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                "[%s:%d]ERR Packet Overflow!\r\n", __FUNCTION__, __LINE__);
            return WV_ERR_TSP_TS_ERROR;
        }

        if (WV_SUCCESS != TSP_GetTSPacket(u8TSScannerIndex, aru8TSPacket, PACKET_LENGTH_WELLAV))
        {
            continue;
        }

        /*Check PID*/
        if ((pu8TSPkt[1] & 0x1F) != ((u16PID >> 8) & 0x1F)
            || (pu8TSPkt[2] & 0xFF) != (u16PID & 0xFF))
        {
            u32ErrNum++;
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                "[%s:%d]PID error,PID[%u][%u]\r\n",
                __FUNCTION__, __LINE__, u16PID, ((pu8TSPkt[1] & 0x1f) << 8) | pu8TSPkt[2]);
            continue;
        }
        
        /*Check transport error indicator*/
        if ((pu8TSPkt[1] & 0x80) != 0x00)
        {
            u32ErrNum++;
            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SIP,
                "[%s:%d]Transport Indicator ERR\r\n", __FUNCTION__, __LINE__);
            continue;
        }

        u8PayloadUnitStartIndicator = ((pu8TSPkt[1] & 0x40) == 0x40) ? 1 : 0;

        if (1 != u8PayloadUnitStartIndicator)
        {
            continue;
        }

        u8AdaptationFieldControl = (pu8TSPkt[3] & 0x30) >> 4;

        if ((1 != u8AdaptationFieldControl) && (3 != u8AdaptationFieldControl))
        {
            //不含有净荷数据
            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SIP,
                "[%s:%d]No payload in packet,u8AdaptationFieldControl[%u]\r\n",
                __FUNCTION__, __LINE__, u8AdaptationFieldControl);
            continue;
        }

        pu8Payload = &pu8TSPkt[4];

        u8AdaptationFieldLength = 0;

        if (3 == u8AdaptationFieldControl)//既有适配字段也含有净荷数据
        {
            u8AdaptationFieldLength = pu8Payload[0];

            if (u8AdaptationFieldLength > 182)
            {
                u32ErrNum++;
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                    "[%s:%d]u8AdaptationFieldLength[%u] error!\r\n",
                    __FUNCTION__, __LINE__, u8AdaptationFieldLength);
                continue;
            }

            pu8Payload += (1 + u8AdaptationFieldLength);
        }

        u8PointerFiled = pu8Payload[0];
        pu8Payload += (1 + u8PointerFiled);

        if (pu8Payload >= (pu8TSPkt + 188))
        {
            u32ErrNum++;
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP,
                "[%s:%d]PointerFiled[%u] error,u8AdaptationFieldLength[%u]!\r\n",
                __FUNCTION__, __LINE__, u8AdaptationFieldLength, u8PointerFiled);
            continue;
        }

        /*Check TableID*/
        if (pu8Payload[0] != u8TID)
        {
            continue;
        }
        
        *pu8LastSectionNum = pu8Payload[7];
        break;

        
    } while(u32TimeOut > 0);

    if (u32TimeOut == 0)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Time Out,u16PID[%u],u8TID[%u]!\r\n",
            __FUNCTION__, __LINE__, u16PID, u8TID);
        return WV_ERR_TSP_FILTER_PSI_TIMEOUT;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_GetPATSection
  Description:    获取PAT表的一个段
  Input:          u8TSScannerIndex:流搜索模块索引
                  u8Standard:数字电视标准
                  u8SectionNumber:PAT第几个段
                  u32Len:SectionBuffer的长度
  Output:         pu8SectionBuffer:获取到的段
  Return:         成功时返回SUCCESS，失败时返回对应错误码
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_GetPATSection(U8 u8TSScannerIndex, U8 u8Standard, U8 u8SectionNumber, U8 *pu8SectionBuffer, U32 u32Len)
{
    wvErrCode enErrCode = WV_SUCCESS;

    if ((MAX_TS_SCANNER_NUM <= u8TSScannerIndex) || (!pu8SectionBuffer)
        || (u32Len < MAX_SI_SECTION_LEN))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u],pu8SectionBuffer[%p],u32Len[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex, pu8SectionBuffer, u32Len);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_FilterPSI(u8TSScannerIndex, u8Standard, PAT_PID, PAT_TID, u8SectionNumber, 0,
                            pu8SectionBuffer, u32Len);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_FilterPSI error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_GetPMTSection
  Description:    获取tPMT表的一个段
  Input:          u8TSScannerIndex:流搜索模块索引
                  u8Standard:数字电视标准
                  u8SectionNumber:PMT第几个段
                  u16PMTPID:PMTPID
                  u16ServiceID:PMTPID对应节目的ServiceID
				  u32Len:SectionBuffer的长度
  Output:         pu8SectionBuffer:获取到的段
  Return:         成功时返回SUCCESS，失败时返回对应错误码

  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_GetPMTSection(U8 u8TSScannerIndex, U8 u8Standard, U8 u8SectionNumber,
                        U16 u16PMTPID, U16 u16ServiceID, U8 *pu8SectionBuffer, U32 u32Len)
{
    wvErrCode enErrCode = WV_SUCCESS;

    if ((MAX_TS_SCANNER_NUM <= u8TSScannerIndex) || (!pu8SectionBuffer)
        || (u32Len < MAX_SI_SECTION_LEN))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u],pu8SectionBuffer[%p],u32Len[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex, pu8SectionBuffer, u32Len);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_FilterPSI(u8TSScannerIndex, u8Standard, u16PMTPID, PMT_TID, u8SectionNumber,
                            u16ServiceID, pu8SectionBuffer, u32Len);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_FilterPSI error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_GetCATSection
  Description:    获取CAT表的一个段
  Input:          u8TSScannerIndex:流搜索模块索引
                  u8Standard:数字电视标准
                  u8SectionNumber:CAT第几个段
                  u32Len:SectionBuffer的长度
  Output:         pu8SectionBuffer:获取到的段
  Return:         成功时返回SUCCESS，失败时返回对应错误码
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_GetCATSection(U8 u8TSScannerIndex, U8 u8Standard, U8 u8SectionNumber, U8 *pu8SectionBuffer, U32 u32Len)
{
    wvErrCode enErrCode = WV_SUCCESS;

    if ((MAX_TS_SCANNER_NUM <= u8TSScannerIndex) || (!pu8SectionBuffer)
        || (u32Len < MAX_SI_SECTION_LEN))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u],pu8SectionBuffer[%p],u32Len[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex, pu8SectionBuffer, u32Len);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_FilterPSI(u8TSScannerIndex, u8Standard, CAT_PID, CAT_TID, u8SectionNumber, 0,
                            pu8SectionBuffer, u32Len);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_FilterPSI error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_GetSDTSection
  Description:    获取SDT表的一个段
  Input:          u8TSScannerIndex:流搜索模块索引
                  u8Standard:数字电视标准
				  u8TID:TableID(SDTActual/SDTOther)
                  u8SectionNumber:SDT第几个段
                  u32Len:SectionBuffer的长度
  Output:         pu8SectionBuffer:获取到的段
  Return:         成功时返回SUCCESS，失败时返回对应错误码
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_GetSDTSection(U8 u8TSScannerIndex, U8 u8Standard, U8 u8TID, U8 u8SectionNumber,
                        U8 *pu8SectionBuffer, U32 u32Len)
{
    wvErrCode enErrCode = WV_SUCCESS;

    if ((MAX_TS_SCANNER_NUM <= u8TSScannerIndex) || (!pu8SectionBuffer)
        || (u32Len < MAX_SI_SECTION_LEN))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u],pu8SectionBuffer[%p],u32Len[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex, pu8SectionBuffer, u32Len);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_FilterPSI(u8TSScannerIndex, u8Standard, SDT_PID, u8TID, u8SectionNumber, 0,
                            pu8SectionBuffer, u32Len);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_FilterPSI error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_GetBATSection
  Description:    获取BAT表的一个段
  Input:          u8TSScannerIndex:流搜索模块索引
                  u8Standard:数字电视标准
                  u8SectionNumber:BAT第几个段
                  u32Len:SectionBuffer的长度
  Output:         pu8SectionBuffer:获取到的段
  Return:         成功时返回SUCCESS，失败时返回对应错误码
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_GetBATSection(U8 u8TSScannerIndex, U8 u8Standard, U8 u8SectionNumber, U8 *pu8SectionBuffer, U32 u32Len)
{
    wvErrCode enErrCode = WV_SUCCESS;

    if ((MAX_TS_SCANNER_NUM <= u8TSScannerIndex) || (!pu8SectionBuffer)
        || (u32Len < MAX_SI_SECTION_LEN))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u],pu8SectionBuffer[%p],u32Len[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex, pu8SectionBuffer, u32Len);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_FilterPSI(u8TSScannerIndex, u8Standard, BAT_PID, BAT_TID, u8SectionNumber, 0,
                            pu8SectionBuffer, u32Len);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_FilterPSI error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_GetNITSection
  Description:    获取NIT表的一个段
  Input:          u8TSScannerIndex:流搜索模块索引
                  u8Standard:数字电视标准
                  u8TID:TableID(NIT_TID_Actual/Other)
                  u8SectionNumber:NIT第几个段
                  u32Len:SectionBuffer的长度
  Output:         pu8SectionBuffer:获取到的段
  Return:         成功时返回SUCCESS，失败时返回对应错误码
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_GetNITSection(U8 u8TSScannerIndex, U8 u8Standard, U8 u8TID,
                        U8 u8SectionNumber, U8 *pu8SectionBuffer, U32 u32Len)
{
    wvErrCode enErrCode = WV_SUCCESS;

    if ((MAX_TS_SCANNER_NUM <= u8TSScannerIndex) || (!pu8SectionBuffer)
        || (u32Len < MAX_SI_SECTION_LEN))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8TSScannerIndex[%u],pu8SectionBuffer[%p],u32Len[%u]\r\n",
            __FUNCTION__, __LINE__, u8TSScannerIndex, pu8SectionBuffer, u32Len);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    enErrCode = TSP_FilterPSI(u8TSScannerIndex, u8Standard, NIT_PID, u8TID, u8SectionNumber, 0,
                            pu8SectionBuffer, u32Len);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_FilterPSI error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        return enErrCode;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SetInputPATInfo
  Description:  把PAT中的节目添加进输入流中
  Input:        u16InTSIndex:流索引
                pstCATTable:PAT表
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_SetInputPATInfo(U16 u16InTSIndex, PATTable_t *pstPATTable, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 u16InProgIndex = INDEX_INVALID_VALUE;
    U16 u16LastProgIndex = INDEX_INVALID_VALUE;
    IndexInfoList stIndexUnusedList;
    Input_TSProgram *pstTSProg = NULL;

    if ((!pstPATTable) || (!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstPATTable[%p],pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstPATTable, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    stIndexUnusedList.u16IndexListNum = 0;
    enErrCode = TSP_GetUnusedInputProgIndexList(&stIndexUnusedList, pstParamTS, pstPATTable->u16ProgramCount);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetUnusedInputProgIndexList Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    enErrCode = TSP_GetInTSLastProgIndex(u16InTSIndex, pstParamTS, &u16LastProgIndex);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSLastProgIndex Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    pstParamTS->pInputTS[u16InTSIndex].u16TransportStreamID = pstPATTable->u16TransportStreamID;

    // 添加节目信息
    for (i = 0; i < pstPATTable->u16ProgramCount; i++)
    {
        u16InProgIndex = stIndexUnusedList.aru16IndexList[i];

        // 第一个节目，ts流的节目偏移需要赋值
        if (0 == i)
        {
            if (INDEX_INVALID_VALUE == u16LastProgIndex)
            {
                pstParamTS->pInputTS[u16InTSIndex].u16ProgramIndex = u16InProgIndex;
            }
            else
            {
                pstParamTS->pInputProgram[u16LastProgIndex].u16NextIndex = u16InProgIndex;
            }
        }

        TSP_InitInputProgram(u16InTSIndex, u16InProgIndex, pstParamTS);

        pstTSProg = &pstParamTS->pInputProgram[u16InProgIndex];

        pstTSProg->u16ServiceID = pstPATTable->arstProgramInfo[i].u16ServiceId;
        pstTSProg->u16PMTPID = pstPATTable->arstProgramInfo[i].u16PMTPid;
        
        pstTSProg->u16NewServiceID = pstTSProg->u16ServiceID;
        pstTSProg->u16NewPMTPID = pstTSProg->u16PMTPID;

        // 最后一个节目,则链表的下一个指向无效位置
        if ((i + 1) == pstPATTable->u16ProgramCount)
        {
            pstTSProg->u16NextIndex = INDEX_INVALID_VALUE;
        }
        else
        {
            pstTSProg->u16NextIndex = stIndexUnusedList.aru16IndexList[i + 1];
        }
    }

    pstParamTS->pInputTS[u16InTSIndex].u16ProgramNum += pstPATTable->u16ProgramCount;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SetInputPMTInfo
  Description:  把PMT中的节目信息添加进输入节目中
  Input:        u16ProgramIndex:节目索引
                pstCMTTable:PMT表
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_SetInputPMTInfo(U16 u16ProgramIndex, const PMTTable_t *pstPMTTable, TSInfo *pstParamTS)
{
    U16 i = 0;
    U16 j = 0;
    U16 u16TotalPIDNum = 0;
    U16 u16UsedIndexNum = 0;
    U16 u16PCRPIDIndex = 0;
    U16 u16ESIndex = 0;
    U16 u16ESECMIndex = 0;
    U16 u16PSECMIndex = 0;
    wvErrCode enErrCode = WV_SUCCESS;

    Input_TSProgram *pstTSProg = NULL;
    Input_TSPID *pstESPID = NULL;
    Input_TSPID *pstECMPID = NULL;
    const PMTESInfo_t *pstPMTESInfo = NULL;

    IndexInfoList stIndexUnusedList;

    // 入参合法性校验
    if ((!pstPMTTable) || (!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16ProgramIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstPMTTable[%p],pstParamTS[%p],u16ProgramIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstPMTTable, pstParamTS, u16ProgramIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    if ((MAX_ES_NUM_IN_PMT < pstPMTTable->u8EsNum)
       || (MAX_CASYS_NUM_IN_PMT < pstPMTTable->u8CaSysNum)
       || (MAX_PROGRAM_INFO_LEN < pstPMTTable->u16ProgInfoLen))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u8EsNum[%u],u8CaSysNum[%u],u16ProgInfoLen[%u]\r\n",
            __FUNCTION__, __LINE__, pstPMTTable->u8EsNum, pstPMTTable->u8CaSysNum, pstPMTTable->u16ProgInfoLen);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    pstTSProg = &pstParamTS->pInputProgram[u16ProgramIndex];

    pstTSProg->u8VersionNumber = pstPMTTable->u8VersionNum;

    if (pstPMTTable->u16PCRPID < 0x1FFF)
    {
        u16TotalPIDNum = 1;
    }

    u16TotalPIDNum += pstPMTTable->u8EsNum + pstPMTTable->u8CaSysNum;

    for (i = 0; i < pstPMTTable->u8EsNum; i++)
    {
        u16TotalPIDNum += pstPMTTable->arstPMTESInfo[i].u8CaSysNum;
    }

    enErrCode = TSP_GetUnusedInputPIDIndexList(&stIndexUnusedList, pstParamTS, u16TotalPIDNum);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetUnusedInputPIDIndexList Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (pstPMTTable->u16PCRPID < 0x1FFF)
    {
        u16PCRPIDIndex = stIndexUnusedList.aru16IndexList[u16UsedIndexNum++];
        TSP_InitInputPID(PID_TYPE_PCRPID, u16ProgramIndex, u16PCRPIDIndex, pstParamTS);
        pstParamTS->pInputPID[u16PCRPIDIndex].u16PID = pstPMTTable->u16PCRPID;
        pstParamTS->pInputPID[u16PCRPIDIndex].u16NewPID = pstPMTTable->u16PCRPID;
        pstTSProg->u16PCRIndex = u16PCRPIDIndex;
        pstTSProg->u16PCRPID = pstPMTTable->u16PCRPID;
    }
    else
    {
        pstTSProg->u16PCRIndex = INDEX_INVALID_VALUE;
        pstTSProg->u16PCRPID = 0x1FFF;
    }
    pstTSProg->u16NewPCRPID = pstTSProg->u16PCRPID;

    pstTSProg->u16ESNumber = pstPMTTable->u8EsNum;
    for (i = 0; i < pstPMTTable->u8EsNum; i++)
    {
        pstPMTESInfo = &pstPMTTable->arstPMTESInfo[i];

        u16ESIndex = stIndexUnusedList.aru16IndexList[u16UsedIndexNum++];

        // 第一个es信息，节目的es偏移需要赋值
        if (0 == i)
        {
            pstTSProg->u16PIDIndex = u16ESIndex;
        }

        TSP_InitInputPID(PID_TYPE_ESPID, u16ProgramIndex, u16ESIndex, pstParamTS);

        pstESPID = &pstParamTS->pInputPID[u16ESIndex];
        pstESPID->u8StreamType = pstPMTESInfo->u8Type;
        pstESPID->u16PID = pstPMTESInfo->u16Pid;
        pstESPID->u16NewPID = pstESPID->u16PID;

        pstESPID->u16ESInfoLength = pstPMTESInfo->u16ESInfoLen;
        if (0 < pstPMTESInfo->u16ESInfoLen)
        {
            memcpy(pstESPID->aru8ESInfo,
                   pstPMTESInfo->aru8ESInfo,
                   pstPMTESInfo->u16ESInfoLen);
        }

        pstESPID->u16ESECMPIDNumber = pstPMTESInfo->u8CaSysNum;
        // ES ECM信息赋值
        for (j = 0; j < pstPMTESInfo->u8CaSysNum; j++)
        {
            u16ESECMIndex = stIndexUnusedList.aru16IndexList[u16UsedIndexNum++];

            // es的第一个ecm信息，es的ecm偏移需要赋值
            if (0 == j)
            {
                pstESPID->u16ESECMPIDIndex = u16ESECMIndex;
            }

            TSP_InitInputPID(PID_TYPE_ES_ECMPID, u16ESIndex, u16ESECMIndex, pstParamTS);

            pstECMPID = &pstParamTS->pInputPID[u16ESECMIndex];

            pstECMPID->u16CASID = pstPMTESInfo->arstCaInfo[j].u16CaSysID;
            pstECMPID->u16PID = pstPMTESInfo->arstCaInfo[j].u16EcmPid;
            pstECMPID->u16NewPID = pstECMPID->u16PID;

            // 最后一个ESECM,则链表的下一个指向无效位置
            if ((j + 1) == pstPMTESInfo->u8CaSysNum)
            {
                pstECMPID->u16NextIndex = INDEX_INVALID_VALUE;
            }
            else
            {
                pstECMPID->u16NextIndex = stIndexUnusedList.aru16IndexList[u16UsedIndexNum];
            }
        }

        // 最后一个ES,则链表的下一个指向无效位置
        if ((i + 1) == pstPMTTable->u8EsNum)
        {
            pstESPID->u16NextIndex = INDEX_INVALID_VALUE;
        }
        else
        {
            pstESPID->u16NextIndex = stIndexUnusedList.aru16IndexList[u16UsedIndexNum];
        }
    }

    // PSECM信息赋值
    pstTSProg->u16PSECMPIDNumber = pstPMTTable->u8CaSysNum;
    for (i = 0; i < pstPMTTable->u8CaSysNum; i++)
    {
        u16PSECMIndex = stIndexUnusedList.aru16IndexList[u16UsedIndexNum++];

        // 第一个es信息，节目的es偏移需要赋值
        if (0 == i)
        {
            pstTSProg->u16PSECMPIDIndex = u16PSECMIndex;
        }

        TSP_InitInputPID(PID_TYPE_PS_ECMPID, u16ProgramIndex, u16PSECMIndex, pstParamTS);

        pstECMPID = &pstParamTS->pInputPID[u16PSECMIndex];

        pstECMPID->u16CASID = pstPMTTable->arstCaInfo[i].u16CaSysID;
        pstECMPID->u16PID = pstPMTTable->arstCaInfo[i].u16EcmPid;
        pstECMPID->u16NewPID = pstECMPID->u16PID;

        // 最后一个PSECM,则链表的下一个指向无效位置
        if ((i + 1) == pstPMTTable->u8CaSysNum)
        {
            pstECMPID->u16NextIndex = INDEX_INVALID_VALUE;
        }
        else
        {
            pstECMPID->u16NextIndex = stIndexUnusedList.aru16IndexList[u16UsedIndexNum];
        }
    }

    // programinfo 赋值
    pstTSProg->u16ProgramInfoLength = pstPMTTable->u16ProgInfoLen;
    if (0 < pstPMTTable->u16ProgInfoLen)
    {
        memcpy(pstTSProg->aru8ProgramInfo,
               pstPMTTable->aru8ProgInfo,
               pstPMTTable->u16ProgInfoLen);
        
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SetInputCATInfo
  Description:  把CAT中的EMM添加进输入流中
  Input:        u16InTSIndex:流索引
                pstCATTable:CAT表
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_SetInputCATInfo(U16 u16InTSIndex, const CATTable_t *pstCATTable, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 u16EMMIndex = INDEX_INVALID_VALUE;
    U16 u16LastEMMIndex = INDEX_INVALID_VALUE;
    U16 u16NextIndex = INDEX_INVALID_VALUE;
    U16 i = 0;

    IndexInfoList stIndexUnusedList;

    if ((!pstCATTable) || (!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstCATTable[%p],pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstCATTable, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    if (0 == pstCATTable->u16CaSysNum)
    {
        return WV_SUCCESS;
    }

    stIndexUnusedList.u16IndexListNum = 0;

    enErrCode = TSP_GetUnusedInputPIDIndexList(&stIndexUnusedList, pstParamTS, pstCATTable->u16CaSysNum);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetUnusedInputPIDIndexList Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    enErrCode = TSP_GetInTSLastEMMPIDIndex(u16InTSIndex, pstParamTS, &u16LastEMMIndex);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSLastEMMPIDIndex Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    // 添加EMM信息
    for (i = 0; i < pstCATTable->u16CaSysNum; i++)
    {
        u16EMMIndex = stIndexUnusedList.aru16IndexList[i];

        if (0 == i)
        {
            if (INDEX_INVALID_VALUE == u16LastEMMIndex)
            {
                pstParamTS->pInputTS[u16InTSIndex].u16EMMPIDIndex = u16EMMIndex;
            }
            else
            {
                pstParamTS->pInputPID[u16LastEMMIndex].u16NextIndex = u16EMMIndex;
            }
        }

        TSP_InitInputPID(PID_TYPE_EMMPID, u16InTSIndex, u16EMMIndex, pstParamTS);

        pstParamTS->pInputPID[u16EMMIndex].u16CASID = pstCATTable->arstCaInfo[i].u16CaSysID;
        pstParamTS->pInputPID[u16EMMIndex].u16PID = pstCATTable->arstCaInfo[i].u16EmmPid;
        pstParamTS->pInputPID[u16EMMIndex].u16NewPID = pstParamTS->pInputPID[u16EMMIndex].u16PID;

        // 最后一个EMM,则链表的下一个指向无效位置
        if ((i + 1) == pstCATTable->u16CaSysNum)
        {
            u16NextIndex = INDEX_INVALID_VALUE;
        }
        else
        {
            u16NextIndex = stIndexUnusedList.aru16IndexList[i + 1];
        }
        
        pstParamTS->pInputPID[u16EMMIndex].u16NextIndex = u16NextIndex;
    }

    pstParamTS->pInputTS[u16InTSIndex].u16EMMPIDNumber += pstCATTable->u16CaSysNum;

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SetInputSDTInfo
  Description:  把SDT中的节目信息添加进节目结构中
  Input:        u16InTSIndex:流索引
                pstSDTTable:SDT表
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_SetInputSDTInfo(U16 u16InTSIndex, const SDTTable_t *pstSDTTable, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 u16ProgramIndex = 0;
    U16 i = 0;
    U16 j = 0;

    const SDTProgramInfo_t *pstSDTProgramInfo = NULL;
    Input_TSProgram *pstTSProg = NULL;

    IndexInfoList stIndexInfoList;

    // 参数合法性校验
    if ((!pstSDTTable) || (MAX_PROGRAMS_NUM_IN_SDT < pstSDTTable->u16ServiceNum)
        || (!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstSDTTable[%p],u16ServiceNum[%u],pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstSDTTable, pstSDTTable->u16ServiceNum, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    stIndexInfoList.u16IndexListNum = 0;

    enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stIndexInfoList);
    if (WV_SUCCESS != enErrCode)
    {
         log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSProgIndexList fail,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    pstParamTS->pInputTS[u16InTSIndex].u16OriginalNetworkID = pstSDTTable->u16OriginalNetworkId;

    // 将SDT信息赋值到对应节目
    for (i = 0; i < pstSDTTable->u16ServiceNum; i++)
    {
        pstSDTProgramInfo = &pstSDTTable->arstProgInfo[i];
        for (j = 0; j < stIndexInfoList.u16IndexListNum; j++)
        {
            u16ProgramIndex = stIndexInfoList.aru16IndexList[j];
            pstTSProg = &pstParamTS->pInputProgram[u16ProgramIndex];

            if (pstSDTProgramInfo->u16ServiceID != pstTSProg->u16ServiceID)
            {
                continue;
            }

            pstTSProg->u8ServiceType = pstSDTProgramInfo->u8ServiceType;
            pstTSProg->u8FreeCAMode = pstSDTProgramInfo->u8FreeCAMode;
            pstTSProg->u8RunningStatus = pstSDTProgramInfo->u8RunningStatus;
            pstTSProg->u8EITScheduleFlag = pstSDTProgramInfo->u8EITScheduleFlag;
            pstTSProg->u8EITPresentFollowingFlag = pstSDTProgramInfo->u8EITPresentFollowingFlag;
            pstTSProg->u8ServiceProviderLen = pstSDTProgramInfo->u8ProviderNameLen;
            pstTSProg->u8ServiceNameLen = pstSDTProgramInfo->u8ServiceNameLen;
            memcpy(pstTSProg->aru8ServiceProvider, pstSDTProgramInfo->aru8ProviderName, MAX_SDT_NAME_LEN);
            memcpy(pstTSProg->aru8ServiceName, pstSDTProgramInfo->aru8ServiceName, MAX_SDT_NAME_LEN);

            pstTSProg->u16SDTInfoLength = pstSDTProgramInfo->u16OtherInfoLen;

            if (0 < pstSDTProgramInfo->u16OtherInfoLen)
            {
                memcpy(pstTSProg->aru8SDTInfo,
                        pstSDTProgramInfo->aru8OtherInfo,
                        pstSDTProgramInfo->u16OtherInfoLen);
            }

            break;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     TSP_SetInputOtherPIDInfo
  Description:  把OtherPID添加进输入TS中
  Input:        u16InTSIndex:在添加到哪个TS
                u16PID:OtherPID
                pstParamTS
  Output:       none
  Return:       void
  Others:
  Create:       Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_SetInputOtherPIDInfo(U16 u16InTSIndex, U16 u16PID, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U16 u16OtherPIDIndex = INDEX_INVALID_VALUE;
    U16 u16LastOtherPIDIndex = INDEX_INVALID_VALUE;

    IndexInfoList stIndexUnusedList;

    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    stIndexUnusedList.u16IndexListNum = 0;

    enErrCode = TSP_GetUnusedInputPIDIndexList(&stIndexUnusedList, pstParamTS, 1);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetUnusedInputPIDIndexList Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    u16OtherPIDIndex = stIndexUnusedList.aru16IndexList[0];
    
    TSP_InitInputPID(PID_TYPE_OTHERPID, u16InTSIndex, u16OtherPIDIndex, pstParamTS);

    pstParamTS->pInputPID[u16OtherPIDIndex].u16PID = u16PID;
    pstParamTS->pInputPID[u16OtherPIDIndex].u16NewPID = u16PID;

    enErrCode = TSP_GetInTSLastOtherPIDIndex(u16InTSIndex, pstParamTS, &u16LastOtherPIDIndex);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSLastOtherPIDIndex Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (INDEX_INVALID_VALUE == u16LastOtherPIDIndex)
    {
        pstParamTS->pInputTS[u16InTSIndex].u16OtherPIDIndex = u16OtherPIDIndex;
        pstParamTS->pInputTS[u16InTSIndex].u16OtherPIDNum = 1;
    }
    else
    {
        pstParamTS->pInputPID[u16LastOtherPIDIndex].u16NextIndex = u16OtherPIDIndex;
        pstParamTS->pInputTS[u16InTSIndex].u16OtherPIDNum++;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_ParseSI
  Description:    解析PSI/SI
  Input:          u16Channel:通道号
                  u8Standard:数字电视标准
                  u8TSScannerIndex:流搜索模块索引
                  u16InTSIndex:解析到的PSI/SI信息存放到哪个流中
  Output:         pstParamTS
  Return:         成功时返回SUCCESS，错误时返回对应的错误码
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_ParseSI(U16 u16Channel, U8 u8Standard, U8 u8TSScannerIndex, U16 u16InTSIndex, TSInfo *pstParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U32 i = 0;
    U32 j = 0;
    U32 k = 0;
    PIDINFO stPIDInfo;
    PIDINFO stParsedPIDInfo;
    U16 u16PID = 0xFFFF;
    U8 *pu8SectionBuffer = NULL;
    U8 u8SectionNum = 0;
    U8 u8LastSectionNum = 0;
    U16 u16InProgIndex = INDEX_INVALID_VALUE;
    U16 u16PMTPID = 0xFFFF;
    U16 u16ServiceID = 0xFFFF;

    PATTable_t *pstPAT = NULL;
    PMTTable_t *pstPMT = NULL;
    CATTable_t *pstCAT = NULL;
    SDTTable_t *pstSDT = NULL;

    IndexInfoList stProgIndexList;

    if ((MAX_INPUT_CHANNEL_NUM <= u16Channel)
        || (MAX_TS_SCANNER_NUM <= u8TSScannerIndex)
        || (!pstParamTS)
        || (pstParamTS->u32InputTSNumber <= u16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u16Channel[%u],u8TSScannerIndex[%u],pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, u16Channel, u8TSScannerIndex, pstParamTS, u16InTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
        "[%s:%d]Start ParseSI,u8TSScannerIndex[%u],u16InTSIndex[%u]\r\n",
        __FUNCTION__, __LINE__, u8TSScannerIndex, u16InTSIndex);

	//获取通道的所有PID
    stPIDInfo.u16PIDNum = 0;
    enErrCode = TSP_GetAllPIDInChannel(u16Channel, u8TSScannerIndex, &stPIDInfo);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetAllPIDInTS Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    if (0 == stPIDInfo.u16PIDNum)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "[%s:%d]No PID\r\n", __FUNCTION__, __LINE__);
        return WV_SUCCESS;
    }

    for (i = 0; i < stPIDInfo.u16PIDNum; i++)
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "aru16PIDList[%u][%u]\r\n", i, stPIDInfo.aru16PIDList[i]);
    }

    if (0x1FFF == stPIDInfo.aru16PIDList[stPIDInfo.u16PIDNum - 1])
    {
        stPIDInfo.u16PIDNum--;
    }

	//解析PAT
    if (PAT_PID != stPIDInfo.aru16PIDList[0])
    {
        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "[%s:%d]No PAT\r\n", __FUNCTION__, __LINE__);

        if (stPIDInfo.u16PIDNum > MAX_OTHERPID_NUM)
        {
            stPIDInfo.u16PIDNum = MAX_OTHERPID_NUM;
        }

        for (i = 0; i < stPIDInfo.u16PIDNum; i++)
        {
            u16PID = stPIDInfo.aru16PIDList[i] & 0x1FFF;
			//将所有的PID设置为Others PID
            enErrCode = TSP_SetInputOtherPIDInfo(u16InTSIndex, u16PID, pstParamTS);
            if (WV_SUCCESS != enErrCode)
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_SetInputOtherPIDInfo Error,enErrCode[%08X]\r\n",
                    __FUNCTION__, __LINE__, enErrCode);
                return enErrCode;       
            }
        }

        return WV_SUCCESS;
    }

    pu8SectionBuffer = TSP_GetSectionBuffer(u8TSScannerIndex);
    pstPAT = TSP_GetPATBuffer(u8TSScannerIndex);

    stParsedPIDInfo.u16PIDNum = 0;
    
    for (i = 0; i < stPIDInfo.u16PIDNum; i++)
    {
        u16PID = stPIDInfo.aru16PIDList[i] & 0x1FFF;

        if (TSP_IsPIDExist(u16PID, &stParsedPIDInfo))
        {
            continue;
        }

        switch (u16PID)
        {
            case PAT_PID:

                log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "PAT Exist\r\n");
                
                enErrCode = TSP_GetPATSection(u8TSScannerIndex, u8Standard, 0, pu8SectionBuffer, MAX_SI_SECTION_LEN);
                if (WV_SUCCESS != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]TSP_GetPAT Error,enErrCode[%08X]\r\n",
                        __FUNCTION__, __LINE__, enErrCode);
                    return enErrCode;       
                }

                enErrCode = SIP_ParsePAT(pu8SectionBuffer, MAX_SI_SECTION_LEN, pstPAT);
                if (WV_SUCCESS != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]SIP_ParsePAT Error,enErrCode[%08X]\r\n",
                        __FUNCTION__, __LINE__, enErrCode);
                    return enErrCode;       
                }

                enErrCode = TSP_SetInputPATInfo(u16InTSIndex, pstPAT, pstParamTS);
                if (WV_SUCCESS != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]TSP_SetInputPATInfo Error,enErrCode[%08X]\r\n",
                        __FUNCTION__, __LINE__, enErrCode);
                    return enErrCode;       
                }

                stParsedPIDInfo.aru16PIDList[stParsedPIDInfo.u16PIDNum++] = PAT_PID;

                break;

            case CAT_PID:

                log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "CAT Exist\r\n");

                pstCAT = TSP_GetCATBuffer(u8TSScannerIndex);
                enErrCode = TSP_GetCATSection(u8TSScannerIndex, u8Standard, 0, pu8SectionBuffer, MAX_SI_SECTION_LEN);
                if (WV_SUCCESS != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]TSP_GetCAT Error,enErrCode[%08X]\r\n",
                        __FUNCTION__, __LINE__, enErrCode);
                    return enErrCode;       
                }

                enErrCode = SIP_ParseCAT(pu8SectionBuffer, MAX_SI_SECTION_LEN, pstCAT);
                if (WV_SUCCESS != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]SIP_ParseCAT Error,enErrCode[%08X]\r\n",
                        __FUNCTION__, __LINE__, enErrCode);
                    return enErrCode;       
                }
                
                enErrCode = TSP_SetInputCATInfo(u16InTSIndex, pstCAT, pstParamTS);
                if (WV_SUCCESS != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]TSP_SetInputCATInfo Error,enErrCode[%08X]\r\n",
                        __FUNCTION__, __LINE__, enErrCode);
                    return enErrCode;       
                }

                stParsedPIDInfo.aru16PIDList[stParsedPIDInfo.u16PIDNum++] = CAT_PID;

                for (j = 0; j < pstCAT->u16CaSysNum; j++)
                {
                    stParsedPIDInfo.aru16PIDList[stParsedPIDInfo.u16PIDNum++] = 
                        pstCAT->arstCaInfo[j].u16EmmPid;
                }

                break;

            case SDT_PID:

                log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "SDT Exist\r\n");

                enErrCode = TSP_GetPSILastSectionNum(u8TSScannerIndex, u8Standard,
                                                        SDT_PID, SDT_TID_ACTUAL, &u8LastSectionNum);
                if (WV_SUCCESS != enErrCode)
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]TSP_GetPSILastSectionNum Error,enErrCode[%08X]\r\n",
                        __FUNCTION__, __LINE__, enErrCode);
                    return enErrCode;       
                }

                pstSDT = TSP_GetSDTBuffer(u8TSScannerIndex);
                for (u8SectionNum = 0; u8SectionNum <= u8LastSectionNum; u8SectionNum++)
                {
                    enErrCode = TSP_GetSDTSection(u8TSScannerIndex, u8Standard, SDT_TID_ACTUAL, u8SectionNum,
                                            pu8SectionBuffer, MAX_SI_SECTION_LEN);
                    if (WV_SUCCESS != enErrCode)
                    {
                        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                            "[%s:%d]TSP_GetSDTSection Error,SectionNum[%u],enErrCode[%08X]\r\n",
                            __FUNCTION__, __LINE__, u8SectionNum, enErrCode);
                        return enErrCode;       
                    }

                    enErrCode = SIP_ParseSDT(pu8SectionBuffer, MAX_SI_SECTION_LEN, pstSDT);
                    if (WV_SUCCESS != enErrCode)
                    {
                        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                            "[%s:%d]SIP_ParseSDT error,enErrCode[%u]\r\n",
                            __FUNCTION__, __LINE__, enErrCode);
                        return enErrCode;
                    }

                    enErrCode = TSP_SetInputSDTInfo(u16InTSIndex, pstSDT, pstParamTS);
                    if (WV_SUCCESS != enErrCode)
                    {
                        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                            "[%s:%d]TSP_SetInputSDTOhterInfo Error,enErrCode[%08X]\r\n",
                            __FUNCTION__, __LINE__, enErrCode);
                        return enErrCode;       
                    }
                }

                stParsedPIDInfo.aru16PIDList[stParsedPIDInfo.u16PIDNum++] = SDT_PID;

                break;

            case NIT_PID:

                log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "NIT Exist\r\n");

                //目前还不分析NIT表

                stParsedPIDInfo.aru16PIDList[stParsedPIDInfo.u16PIDNum++] = NIT_PID;

                break;

            default:
                break;
        }
    }

    stProgIndexList.u16IndexListNum = 0;

    enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stProgIndexList);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInTSProgIndexList Error,u32Ret[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;       
    }

    //PMT
    pstPMT = TSP_GetPMTBuffer(u8TSScannerIndex);
    u16InProgIndex = pstParamTS->pInputTS[u16InTSIndex].u16ProgramIndex;
    for (i = 0; i < pstParamTS->pInputTS[u16InTSIndex].u16ProgramNum; i++)
    {
        if (0 != i)
        {
            u16InProgIndex = pstParamTS->pInputProgram[u16InProgIndex].u16NextIndex;
        }
        
        u16PMTPID =  pstParamTS->pInputProgram[u16InProgIndex].u16PMTPID;
        u16ServiceID = pstParamTS->pInputProgram[u16InProgIndex].u16ServiceID;

        if (!TSP_IsPIDExist(u16PMTPID, &stPIDInfo))
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]Prog[%u],u16ServiceID[%u],u16PMTPID[%u] Not Exist\r\n",
                __FUNCTION__, __LINE__, i, u16ServiceID, u16PMTPID);
            continue;
        }

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "Prog[%u],u16ServiceID[%u],PMT[%u] Exist\r\n",
            i, u16ServiceID, u16PMTPID);

        enErrCode = TSP_GetPMTSection(u8TSScannerIndex, u8Standard, 0,
                                    u16PMTPID, u16ServiceID, pu8SectionBuffer, MAX_SI_SECTION_LEN);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetPMTSection Error,enErrCode[%08X],u16PMTPID[%u],u16ServiceID[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u16PMTPID, u16ServiceID);
            return enErrCode;       
        }

        enErrCode = SIP_ParsePMT(pu8SectionBuffer, MAX_SI_SECTION_LEN, pstPMT);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]SIP_ParsePMT Error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;       
        }
 
        enErrCode = TSP_SetInputPMTInfo(u16InProgIndex, pstPMT, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_SetInputPMTInfo Error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;       
        }

        stParsedPIDInfo.aru16PIDList[stParsedPIDInfo.u16PIDNum++] = pstPMT->u16PCRPID;

        for (j = 0; j < pstPMT->u8EsNum; j++)
        {
            stParsedPIDInfo.aru16PIDList[stParsedPIDInfo.u16PIDNum++] = 
                pstPMT->arstPMTESInfo[j].u16Pid;

            for (k = 0; k < pstPMT->arstPMTESInfo[j].u8CaSysNum; k++)
            {
                stParsedPIDInfo.aru16PIDList[stParsedPIDInfo.u16PIDNum++] = 
                    pstPMT->arstPMTESInfo[j].arstCaInfo[k].u16EcmPid;
            }
        }

        for (j = 0; j < pstPMT->u8CaSysNum; j++)
        {
            stParsedPIDInfo.aru16PIDList[stParsedPIDInfo.u16PIDNum++] = 
                pstPMT->arstCaInfo[j].u16EcmPid;
        }

        stParsedPIDInfo.aru16PIDList[stParsedPIDInfo.u16PIDNum++] = u16PMTPID;
    }

    //OtherPID
    for (i = 0; i < stPIDInfo.u16PIDNum; i++)
    {
        u16PID = stPIDInfo.aru16PIDList[i] & 0x1FFF;
        
        if (TSP_IsPIDExist(u16PID, &stParsedPIDInfo))
        {
            continue;
        }

        enErrCode = TSP_SetInputOtherPIDInfo(u16InTSIndex, u16PID, pstParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_SetInputOtherPIDInfo Error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;       
        }
    }

    return WV_SUCCESS;
}

#if 1
void PrintInESECM(U16 u16InESECMIndex, TSInfo *pstParamTS)
{
    Input_TSPID *pstESECM = NULL;
    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InESECMIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InESECMIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InESECMIndex);
        return;
    }

    pstESECM = &pstParamTS->pInputPID[u16InESECMIndex];

    printf("Slot[%u],Channel[%u],u16Index[%u],u16TSIndex[%u],u16ProgramIndex[%u],u16PIDIndex[%u]\r\n",
        pstESECM->u8SlotID,
        pstESECM->u16ChannelID,
        pstESECM->u16Index,
        pstESECM->u16TSIndex,
        pstESECM->u16ProgramIndex,
        pstESECM->u16PIDIndex);

    printf("            u16CASID[%u],u16PID[%u]\r\n",
        pstESECM->u16CASID,
        pstESECM->u16PID);
}


void PrintInES(U16 u16InESIndex, TSInfo *pstParamTS)
{
    Input_TSPID *pstPID = NULL;
    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InESIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InESIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InESIndex);
        return;
    }

    pstPID = &pstParamTS->pInputPID[u16InESIndex];

    printf("Slot[%u],Channel[%u],u16Index[%u],u16NextIndex[%u]u16TSIndex[%u],u16ProgramIndex[%u]\r\n",
        pstPID->u8SlotID,
        pstPID->u16ChannelID,
        pstPID->u16Index,
        pstPID->u16NextIndex,
        pstPID->u16TSIndex,
        pstPID->u16ProgramIndex);

    printf("        u16PIDIndex[%u],u8PIDType[%u],u8StreamType[%u],u16PID[%u],u8FunctionFlag[%u]\r\n",
        pstPID->u16PIDIndex,
        pstPID->u8StreamType,
        pstPID->u16PID,
        pstPID->u8PIDType,
        pstPID->u8FunctionFlag);

    U32 i = 0;
    U16 u16Index = 0;

    printf("            Start print ESECM Info:u16ESECMPIDNumber[%u]\r\n",pstPID->u16ESECMPIDNumber);
    for (i = 0; i < pstPID->u16ESECMPIDNumber; i++)
    {
        if (0 == i)
        {
            u16Index = pstPID->u16ESECMPIDIndex;
        }
        else
        {
            u16Index = pstParamTS->pInputPID[u16Index].u16NextIndex;
        }
        printf("            EsECM[%u],",i);
        PrintInESECM(u16Index, pstParamTS);
        printf("\r\n");
    }
    printf("            End print ESECM Info\r\n");
}

void PrintInPSECM(U16 u16InPSECMIndex, TSInfo *pstParamTS)
{
    Input_TSPID *pstPSECM = NULL;
    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InPSECMIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InPSECMIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InPSECMIndex);
        return;
    }

    pstPSECM = &pstParamTS->pInputPID[u16InPSECMIndex];

    printf("Slot[%u],Channel[%u],u16Index[%u],u16TSIndex[%u],u16ProgramIndex[%u],u8PIDType[%u]\r\n",
        pstPSECM->u8SlotID,
        pstPSECM->u16ChannelID,
        pstPSECM->u16Index,
        pstPSECM->u16TSIndex,
        pstPSECM->u16ProgramIndex,
        pstPSECM->u8PIDType);

    printf("u16CASID[%u],u16PID[%u]\r\n",
        pstPSECM->u16CASID,
        pstPSECM->u16PID);
}

void PrintInProg(U16 u16InProgIndex, TSInfo *pstParamTS)
{
    Input_TSProgram *pstProg = NULL;
    if ((!pstParamTS) || (pstParamTS->u32InputProgramNumber <= u16InProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InProgIndex);
        return;
    }

    pstProg = &pstParamTS->pInputProgram[u16InProgIndex];

    printf("Slot[%u],Channel[%u],u16Index[%u],u16TSIndex[%u]\r\n",
        pstProg->u8SlotID,
        pstProg->u16ChannelID,
        pstProg->u16Index,
        pstProg->u16TSIndex);

    printf("    u16ServiceID[%u],u16PMTPID[%u],u16PCRPID[%u],u16PCRIndex[%u],u8FunctionFlag[%02X]\r\n",
        pstProg->u16ServiceID,
        pstProg->u16PMTPID,
        pstProg->u16PCRPID,
        pstProg->u16PCRIndex,
        pstProg->u8FunctionFlag);
    
    printf("    aru8ServiceName[%s],aru8ServiceProvider[%s],u8ServiceType[%u],u8VersionNumber[%u]\r\n",
        pstProg->aru8ServiceName,
        pstProg->aru8ServiceProvider,
        pstProg->u8ServiceType,
        pstProg->u8VersionNumber);

    U32 i = 0;
    U16 u16Index = 0;

    for (i = 0; i < MAX_DEST; i++)
    {
        printf("    DestSlot[%u],DestChannel[%08X][%08X][%08X][%08X][%08X][%08X][%08X][%08X]\r\n",
            pstProg->aru8DestSlot[i],
            pstProg->aru32DestChannel[i][0],
            pstProg->aru32DestChannel[i][1],
            pstProg->aru32DestChannel[i][2],
            pstProg->aru32DestChannel[i][3],
            pstProg->aru32DestChannel[i][4],
            pstProg->aru32DestChannel[i][5],
            pstProg->aru32DestChannel[i][6],
            pstProg->aru32DestChannel[i][7]);
    }

    printf("        Start print ES Info:u16ESNumber[%u]\r\n",pstProg->u16ESNumber);
    for (i = 0; i < pstProg->u16ESNumber; i++)
    {
        if (0 == i)
        {
            u16Index = pstProg->u16PIDIndex;
        }
        else
        {
            u16Index = pstParamTS->pInputPID[u16Index].u16NextIndex;
        }
                printf("        Es[%u],",i);
        PrintInES(u16Index, pstParamTS);
    }
    printf("        End print ES Info\r\n\r\n");

    printf("        Start print PSECM Info:u16PSECMPIDNumber[%u]\r\n",pstProg->u16PSECMPIDNumber);
    for (i = 0; i < pstProg->u16PSECMPIDNumber; i++)
    {
        if (0 == i)
        {
            u16Index = pstProg->u16PSECMPIDIndex;
        }
        else
        {
            u16Index = pstParamTS->pInputPID[u16Index].u16NextIndex;
        }
        printf("        PSECM[%u]",i);
        PrintInPSECM(u16Index, pstParamTS);
    }
    printf("        End print PSECM Info\r\n");
}

void PrintInEMM(U16 u16InEMMIndex, TSInfo *pstParamTS)
{
    Input_TSPID *pstEMM = NULL;
    if ((!pstParamTS) || (pstParamTS->u32InputPIDNumber <= u16InEMMIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InEMMIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InEMMIndex);
        return;
    }

    pstEMM = &pstParamTS->pInputPID[u16InEMMIndex];

    printf("Slot[%u],Channel[%u],u16Index[%u],u16TSIndex[%u],u16CASID[%u],u16PID[%u]\r\n",
        pstEMM->u8SlotID,
        pstEMM->u16ChannelID,
        pstEMM->u16Index,
        pstEMM->u16TSIndex,
        pstEMM->u16CASID,
        pstEMM->u16PID);
}

void PrintInputTS(U16 u16InTSIndex, TSInfo *pstParamTS)
{
    U32 i = 0;
    U16 u16Index = 0;
    U16 u16ProgIndex = 0;
    Input_TSStream *pstInTS = NULL;
    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16InTSIndex);
        return;
    }

    pstInTS = &pstParamTS->pInputTS[u16InTSIndex];

    printf("\r\nStart print u16InTSIndex[%u] Info########################\r\n",u16InTSIndex);

    printf("Slot[%u],Channel[%u],u16Index[%u],u16NetID[%u],TSID[%u],OrgID[%u]\r\n",
        pstInTS->u8SlotID,
        pstInTS->u16ChannelID,
        pstInTS->u16Index,
        pstInTS->u16NetworkID,
        pstInTS->u16TransportStreamID,
        pstInTS->u16OriginalNetworkID);

    for (i = 0; i < MAX_DEST; i++)
    {
        printf("DestSlot[%u]\r\nDestChannel[%08X][%08X][%08X][%08X][%08X][%08X][%08X][%08X]\r\n",
            pstInTS->aru8DestSlot[i],
            pstInTS->aru32DestChannel[i][0],
            pstInTS->aru32DestChannel[i][1],
            pstInTS->aru32DestChannel[i][2],
            pstInTS->aru32DestChannel[i][3],
            pstInTS->aru32DestChannel[i][4],
            pstInTS->aru32DestChannel[i][5],
            pstInTS->aru32DestChannel[i][6],
            pstInTS->aru32DestChannel[i][7]);

        printf(" BypassFlag[%08X][%08X][%08X][%08X][%08X][%08X][%08X][%08X]\r\n",
            pstInTS->aru32BypassFlag[i][0],
            pstInTS->aru32BypassFlag[i][1],
            pstInTS->aru32BypassFlag[i][2],
            pstInTS->aru32BypassFlag[i][3],
            pstInTS->aru32BypassFlag[i][4],
            pstInTS->aru32BypassFlag[i][5],
            pstInTS->aru32BypassFlag[i][6],
            pstInTS->aru32BypassFlag[i][7]);
    }
    
    printf("ProgNum[%u],EMMNum[%u],OtherPIDNum[%u]\r\n",
        pstInTS->u16ProgramNum,
        pstInTS->u16EMMPIDNumber,
        pstInTS->u16OtherPIDNum);

    printf("    Start print Prog Info:u16ProgramNum[%u]\r\n",pstInTS->u16ProgramNum);
    for (i = 0; i < pstInTS->u16ProgramNum; i++)
    {
        if (0 == i)
        {
            u16ProgIndex = pstInTS->u16ProgramIndex;
        }
        else
        {
            u16ProgIndex = pstParamTS->pInputProgram[u16ProgIndex].u16NextIndex;
        }
        printf("    Prog[%u]",i);
        PrintInProg(u16ProgIndex, pstParamTS);
    }
    printf("    End print Prog Info\r\n\r\n");
    

    printf("    Start print Emm Info:u16EMMPIDNumber[%u]\r\n",pstInTS->u16EMMPIDNumber);
    for (i = 0; i < pstInTS->u16EMMPIDNumber; i++)
    {
        if (0 == i)
        {
            u16Index = pstInTS->u16EMMPIDIndex;
        }
        else
        {
            u16Index = pstParamTS->pInputPID[u16Index].u16NextIndex;
        }
        printf("    Emm[%u],",i);
        PrintInEMM(u16Index, pstParamTS);
        printf("\r\n");
    }
    printf("    End print Emm\r\n\r\n");

    printf("    Start print OtherPID Info:u16OtherPIDNum[%u]\r\n",pstInTS->u16OtherPIDNum);
    for (i = 0; i < pstInTS->u16OtherPIDNum; i++)
    {
        if (0 == i)
        {
            u16Index = pstInTS->u16OtherPIDIndex;
        }
        else
        {
            u16Index = pstParamTS->pInputPID[u16Index].u16NextIndex;
        }
        printf("    OtherPID[%u],u16OtherPIDIndex[%u],PID[%u]\r\n",
            i,
            u16Index,
            pstParamTS->pInputPID[u16Index].u16PID);
    }
    printf("    End print OtherPID\r\n");
    printf("End print u16InTSIndex[%u] Info**********************************\r\n\r\n",u16InTSIndex);
}
#endif

/*****************************************************************************
  Function:       TSP_CopyInputPID
  Description:    把搜索到的PID从临时流结构体中复制到系统流结构体中
  Input:          u16SrcTSIndex:源流索引
                  u16SrcProgIndex:源节目索引
                  u16SrcPIDIndex:源PID索引
                  u16InPIDIndex:输入PID索引
                  pstParamTS:
                  u16TempInPIDIndex:临时流结构体中的输入PID索引
                  pstTempParamTS:
  Output:         
  Return:         成功时返回SUCCESS，错误时返回对应的错误码
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_CopyInputPID(U16 u16SrcTSIndex,
                            U16 u16SrcProgIndex,
                            U16 u16SrcPIDIndex,
                            U16 u16InPIDIndex,
                            TSInfo *pstParamTS,
                            U16 u16TempInPIDIndex,
                            TSInfo *pstTempParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U32 i = 0;
    Input_TSPID *pstInputPID = NULL;
    Input_TSPID *pstTempInputPID = NULL;
    U16 u16Index = INDEX_INVALID_VALUE;
    U16 u16TempIndex = INDEX_INVALID_VALUE;

    IndexInfoList stUnusedIndexList;

    if ((!pstParamTS)
        || (pstParamTS->u32InputTSNumber <= u16SrcTSIndex)
        || (pstParamTS->u32InputPIDNumber <= u16InPIDIndex)
        || (!pstTempParamTS)
        || (pstTempParamTS->u32InputPIDNumber <= u16TempInPIDIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16SrcTSIndex[%u],pstTempParamTS[%p],u16TempInPIDIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16SrcTSIndex, pstTempParamTS, u16TempInPIDIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    pstInputPID = &pstParamTS->pInputPID[u16InPIDIndex];
    pstTempInputPID = &pstTempParamTS->pInputPID[u16TempInPIDIndex];

    memcpy(pstInputPID, pstTempInputPID, sizeof(Input_TSPID));

    pstInputPID->u8ValidFlag = TSP_FLAG_VALID;
    pstInputPID->u16Index = u16InPIDIndex;
    pstInputPID->u16TSIndex = u16SrcTSIndex;
    pstInputPID->u16ProgramIndex = u16SrcProgIndex;
    pstInputPID->u16PIDIndex = u16SrcPIDIndex;

    if (PID_TYPE_ESPID != pstInputPID->u8PIDType)
    {
        return WV_SUCCESS;
    }

    stUnusedIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetUnusedInputPIDIndexList(&stUnusedIndexList,
                                                pstParamTS,
                                                pstTempInputPID->u16ESECMPIDNumber);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetUnusedInputProgIndexList Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    for (i = 0; i < pstTempInputPID->u16ESECMPIDNumber; i++)
    {
        u16Index = stUnusedIndexList.aru16IndexList[i];
        
        if (0 == i)
        {
            pstInputPID->u16ESECMPIDIndex = u16Index;
            u16TempIndex = pstTempInputPID->u16ESECMPIDIndex;
        }
        else
        {
            u16TempIndex = pstTempParamTS->pInputPID[u16TempIndex].u16NextIndex;
        }

        enErrCode = TSP_CopyInputPID(u16SrcTSIndex,
                                        u16SrcProgIndex,
                                        u16SrcPIDIndex,
                                        u16Index,
                                        pstParamTS,
                                        u16TempIndex,
                                        pstTempParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_CopyInputPID Error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;
        }

        if ((i + 1) < pstTempInputPID->u16ESECMPIDNumber)
        {
            pstParamTS->pInputPID[u16Index].u16NextIndex = stUnusedIndexList.aru16IndexList[i + 1];
        }
        else
        {
            pstParamTS->pInputPID[u16Index].u16NextIndex = INDEX_INVALID_VALUE;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_CopyInputTS
  Description:    把搜索到的节目从临时流结构体中复制到系统流结构体中
  Input:          u16SrcTSIndex:输入流索引
                  u16InProgIndex:节目索引
                  pstParamTS:
                  u16TempInProgIndex:临时流结构体中的输入节目索引
                  pstTempParamTS:
  Output:         
  Return:         成功时返回SUCCESS，错误时返回对应的错误码
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_CopyInputProg(U16 u16SrcTSIndex,
                            U16 u16InProgIndex,
                            TSInfo *pstParamTS,
                            U16 u16TempInProgIndex,
                            TSInfo *pstTempParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U32 i = 0;
    Input_TSProgram *pstInputProg = NULL;
    Input_TSProgram *pstTempInputProg = NULL;
    U16 u16Index = INDEX_INVALID_VALUE;
    U16 u16TempIndex = INDEX_INVALID_VALUE;
    U16 u16PCRIndex = INDEX_INVALID_VALUE;
    U16 u16TotalPIDNum = 0;
    U16 u16UsedPIDNum = 0;

    IndexInfoList stUnusedIndexList;

    if ((!pstParamTS)
        || (pstParamTS->u32InputTSNumber <= u16SrcTSIndex)
        || (pstParamTS->u32InputProgramNumber <= u16InProgIndex)
        || (!pstTempParamTS)
        || (pstTempParamTS->u32InputProgramNumber <= u16TempInProgIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16SrcTSIndex[%u],u16InProgIndex[%u],pstTempParamTS[%p],u16TempInProgIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS, u16SrcTSIndex, u16InProgIndex, pstTempParamTS, u16TempInProgIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    pstInputProg = &pstParamTS->pInputProgram[u16InProgIndex];
    pstTempInputProg = &pstTempParamTS->pInputProgram[u16TempInProgIndex];

    memcpy(pstInputProg, pstTempInputProg, sizeof(Input_TSProgram));
    pstInputProg->u16Index = u16InProgIndex;
    pstInputProg->u16TSIndex = u16SrcTSIndex;

    u16TotalPIDNum = 1 + pstTempInputProg->u16ESNumber + pstTempInputProg->u16PSECMPIDNumber;

    stUnusedIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetUnusedInputPIDIndexList(&stUnusedIndexList,
                                            pstParamTS,
                                            u16TotalPIDNum);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetUnusedInputProgIndexList Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    for (i = 0; i < u16TotalPIDNum; i++)
    {
        pstParamTS->pInputPID[stUnusedIndexList.aru16IndexList[i]].u8ValidFlag = TSP_FLAG_VALID;
    }

    u16TempIndex = pstTempInputProg->u16PCRIndex;

    if (INDEX_INVALID_VALUE != u16TempIndex)
    {
        u16PCRIndex = stUnusedIndexList.aru16IndexList[u16UsedPIDNum++];

        pstInputProg->u16PCRIndex = u16PCRIndex;

        enErrCode = TSP_CopyInputPID(u16SrcTSIndex,
                                        u16InProgIndex,
                                        INDEX_INVALID_VALUE,
                                        u16PCRIndex,
                                        pstParamTS,
                                        u16TempIndex,
                                        pstTempParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_CopyInputPID Error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;
        }
    }
    else
    {
        //由于没有PCR，所以要删除多申请的一个PID索引，删除最后申请的一个。
        pstParamTS->pInputPID[stUnusedIndexList.aru16IndexList[u16TotalPIDNum - 1]].u8ValidFlag = TSP_FLAG_INVALID; 
    }

    for (i = 0; i < pstTempInputProg->u16ESNumber; i++)
    {
        u16Index = stUnusedIndexList.aru16IndexList[u16UsedPIDNum++];
        
        if (0 == i)
        {
            pstInputProg->u16PIDIndex = u16Index;
            u16TempIndex = pstTempInputProg->u16PIDIndex;
        }
        else
        {
            u16TempIndex = pstTempParamTS->pInputPID[u16TempIndex].u16NextIndex;
        }

        enErrCode = TSP_CopyInputPID(u16SrcTSIndex,
                                        u16InProgIndex,
                                        INDEX_INVALID_VALUE,
                                        u16Index,
                                        pstParamTS,
                                        u16TempIndex,
                                        pstTempParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_CopyInputPID Error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;
        }

        if ((i + 1) < pstTempInputProg->u16ESNumber)
        {
            pstParamTS->pInputPID[u16Index].u16NextIndex = stUnusedIndexList.aru16IndexList[u16UsedPIDNum];
        }
        else
        {
            pstParamTS->pInputPID[u16Index].u16NextIndex = INDEX_INVALID_VALUE;
        }
    }

    for (i = 0; i < pstTempInputProg->u16PSECMPIDNumber; i++)
    {
        u16Index = stUnusedIndexList.aru16IndexList[u16UsedPIDNum++];
        
        if (0 == i)
        {
            pstInputProg->u16PSECMPIDIndex = u16Index;
            u16TempIndex = pstTempInputProg->u16PSECMPIDIndex;
        }
        else
        {
            u16TempIndex = pstTempParamTS->pInputPID[u16TempIndex].u16NextIndex;
        }

        enErrCode = TSP_CopyInputPID(u16SrcTSIndex,
                                        u16InProgIndex,
                                        INDEX_INVALID_VALUE,
                                        u16Index,
                                        pstParamTS,
                                        u16TempIndex,
                                        pstTempParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_CopyInputPID Error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;
        }

        if ((i + 1) < pstTempInputProg->u16PSECMPIDNumber)
        {
            pstParamTS->pInputPID[u16Index].u16NextIndex = stUnusedIndexList.aru16IndexList[u16UsedPIDNum];
        }
        else
        {
            pstParamTS->pInputPID[u16Index].u16NextIndex = INDEX_INVALID_VALUE;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_CopyInputTS
  Description:    把搜索到的流从临时流结构体中复制到系统流结构体中
  Input:          u16InTSIndex:输入流索引
                  pstParamTS:
                  u16TempInTSIndex:临时流结构体中的输入流索引
                  pstTempParamTS:
  Output:         
  Return:         成功时返回SUCCESS，错误时返回对应的错误码
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_CopyInputTS(U16 u16InTSIndex, TSInfo *pstParamTS, U16 u16TempInTSIndex, TSInfo *pstTempParamTS)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U32 i = 0;
    Input_TSStream *pstInputTS = NULL;
    Input_TSStream *pstTempInputTS = NULL;
    U16 u16Index = INDEX_INVALID_VALUE;
    U16 u16TempIndex = INDEX_INVALID_VALUE;
    U16 u16UsedPIDNum = 0;

    IndexInfoList stUnusedIndexList;
    
    if ((!pstParamTS) || (pstParamTS->u32InputTSNumber <= u16InTSIndex)
        || (!pstTempParamTS) || (pstTempParamTS->u32InputTSNumber <= u16TempInTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,pstParamTS[%p],u16InTSIndex[%u],pstTempParamTS[%p],u16TempInTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pstParamTS,u16InTSIndex, pstTempParamTS, u16TempInTSIndex);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    pstInputTS = &pstParamTS->pInputTS[u16InTSIndex];
    pstTempInputTS = &pstTempParamTS->pInputTS[u16TempInTSIndex];
        
    memcpy(pstInputTS, pstTempInputTS, sizeof(Input_TSStream));
    pstInputTS->u8ValidFlag = TSP_FLAG_VALID;
    pstInputTS->u16Index = u16InTSIndex;

    stUnusedIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetUnusedInputProgIndexList(&stUnusedIndexList, pstParamTS, pstTempInputTS->u16ProgramNum);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetUnusedInputProgIndexList Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    for (i = 0; i < pstTempInputTS->u16ProgramNum; i++)
    {
        u16Index = stUnusedIndexList.aru16IndexList[i];

        if (0 == i)
        {
            pstInputTS->u16ProgramIndex = u16Index;
            u16TempIndex = pstTempInputTS->u16ProgramIndex;
        }
        else
        {
            u16TempIndex = pstTempParamTS->pInputProgram[u16TempIndex].u16NextIndex;
        }

        enErrCode = TSP_CopyInputProg(u16InTSIndex, u16Index, pstParamTS, u16TempIndex, pstTempParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_CopyInputProg Error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;
        }

        if ((i + 1) < pstTempInputTS->u16ProgramNum)
        {
            pstParamTS->pInputProgram[u16Index].u16NextIndex = stUnusedIndexList.aru16IndexList[i + 1];
        }
        else
        {
            pstParamTS->pInputProgram[u16Index].u16NextIndex = INDEX_INVALID_VALUE;
        }
    }

    stUnusedIndexList.u16IndexListNum = 0;
    enErrCode = TSP_GetUnusedInputPIDIndexList(&stUnusedIndexList,
                                                pstParamTS,
                                                pstTempInputTS->u16EMMPIDNumber + pstTempInputTS->u16OtherPIDNum);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetUnusedInputPIDIndexList Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    for (i = 0; i < pstTempInputTS->u16EMMPIDNumber; i++)
    {
        u16Index = stUnusedIndexList.aru16IndexList[u16UsedPIDNum++];

        if (0 == i)
        {
            pstInputTS->u16EMMPIDIndex = u16Index;
            u16TempIndex = pstTempInputTS->u16EMMPIDIndex;
        }
        else
        {
            u16TempIndex = pstTempParamTS->pInputPID[u16TempIndex].u16NextIndex;
        }

        enErrCode = TSP_CopyInputPID(u16InTSIndex,
                                        INDEX_INVALID_VALUE,
                                        INDEX_INVALID_VALUE,
                                        u16Index,
                                        pstParamTS,
                                        u16TempIndex,
                                        pstTempParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_CopyInputPID Error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;
        }

        if ((i + 1) < pstTempInputTS->u16EMMPIDNumber)
        {
            pstParamTS->pInputPID[u16Index].u16NextIndex = stUnusedIndexList.aru16IndexList[u16UsedPIDNum];
        }
        else
        {
            pstParamTS->pInputPID[u16Index].u16NextIndex = INDEX_INVALID_VALUE;
        }
    }

    for (i = 0; i < pstTempInputTS->u16OtherPIDNum; i++)
    {
        u16Index = stUnusedIndexList.aru16IndexList[u16UsedPIDNum++];

        if (0 == i)
        {
            pstInputTS->u16OtherPIDIndex = u16Index;
            u16TempIndex = pstTempInputTS->u16OtherPIDIndex;
        }
        else
        {
            u16TempIndex = pstTempParamTS->pInputPID[u16TempIndex].u16NextIndex;
        }

        enErrCode = TSP_CopyInputPID(u16InTSIndex,
                                        INDEX_INVALID_VALUE,
                                        INDEX_INVALID_VALUE,
                                        u16Index,
                                        pstParamTS,
                                        u16TempIndex,
                                        pstTempParamTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_CopyInputPID Error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            return enErrCode;
        }

        if ((i + 1) < pstTempInputTS->u16OtherPIDNum)
        {
            pstParamTS->pInputPID[u16Index].u16NextIndex = stUnusedIndexList.aru16IndexList[u16UsedPIDNum];
        }
        else
        {
            pstParamTS->pInputPID[u16Index].u16NextIndex = INDEX_INVALID_VALUE;
        }
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:       TSP_ScanTS
  Description:    搜流
  Input:          u16Channel:通道号
                  u8Standard:数字电视标准
  Output:         
  Return:         成功时返回SUCCESS，错误时返回对应的错误码
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
wvErrCode TSP_ScanTS(U16 u16Channel, U8 u8Standard)
{
    wvErrCode enErrCode = WV_SUCCESS;
    U8 u8TSScannerIndex = 0;
    U16 u16InTSIndex = INDEX_INVALID_VALUE;
    U16 u16TempInTSIndex = INDEX_INVALID_VALUE;
    TSInfo *pstTS = NULL;
    TSInfo *pstTempTS = NULL;
    IndexInfoList stIndexList;

    //根据通道来分配一个流搜索模块，逻辑有4个流搜索模块，可以同时进行4个流的搜台
    enErrCode = TSP_DistributeTSScanner(u16Channel, &u8TSScannerIndex);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_DistributeTSScanner Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;
    }

    //获取到的流存放在一个临时的流信息结构中，下面再比较不同
    pstTempTS = TSP_GetTempTSInfo(u8TSScannerIndex);
    if (!pstTempTS)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetTempTSInfo Error\r\n", __FUNCTION__, __LINE__);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    TSP_ClearTSInfo(pstTempTS);

    enErrCode = TSP_GetUnusedInputTSIndexList(&stIndexList, pstTempTS, 1);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetUnusedInputTSIndexList Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;       
    }
    
    u16TempInTSIndex = stIndexList.aru16IndexList[0];

    TSP_InitInputTS(u16TempInTSIndex, u16Channel, pstTempTS);

    enErrCode = TSP_ParseSI(u16Channel, u8Standard, u8TSScannerIndex, u16TempInTSIndex, pstTempTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_ParseSI Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        return enErrCode;       
    }

    pstTS = TSP_GetTSParamHandle();

    TSP_MutexLock();
    
    enErrCode = TSP_GetInputTSIndex(u16Channel, pstTS, &u16InTSIndex);
    if (WV_ERR_TSP_NOT_EXIST == enErrCode)
    {
        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetUnusedInputTSIndexList(&stIndexList, pstTS, 1);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetUnusedInputTSIndexList Error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            goto TSP_ERROR;
        }

        u16InTSIndex = stIndexList.aru16IndexList[0];
        
        enErrCode = TSP_CopyInputTS(u16InTSIndex, pstTS, u16TempInTSIndex, pstTempTS);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_CopyInputTS Error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            goto TSP_ERROR;
        }

        PrintInputTS(u16InTSIndex, pstTS);

        enErrCode = TSP_StoreTSConfiguration();
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_StoreTSConfiguration Error,enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
            goto TSP_ERROR;
        }
        
        TSP_MutexUnlock();
        return WV_SUCCESS;
    }
    else if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_GetInputTSIndex Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        goto TSP_ERROR;
    }
    
    #if 0
    enErrCode = TSP_CompareTSInfo(u16InTSIndex, pstTS, u16TempInTSIndex, pstTempTS);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_CompareTSInfo Error,enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
        goto TSP_ERROR;       
    }
    #endif
    
    TSP_MutexUnlock();
    return WV_SUCCESS;

TSP_ERROR:    
    TSP_MutexUnlock();
    return enErrCode;    
}

/*****************************************************************************
  Function:       TSP_IsReadyToScanTS
  Description:    判断是否可以搜台
  Input:          u16Channel:通道号
  Output:         
  Return:         TRUE or FALSE
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
BOOL TSP_IsReadyToScanTS(U16 u16Channel)
{
    if (MAX_INPUT_CHANNEL_NUM <= u16Channel)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u16Channel[%u]\r\n",
            __FUNCTION__, __LINE__, u16Channel);
        return FALSE;
    }

    return Tuner_isChannelLock(u16Channel);
}


/*****************************************************************************
  Function:       TSP_IsNeedToScanTS
  Description:    判断是否需要搜台
  Input:          u16Channel:通道号
  Output:         
  Return:         TRUE or FALSE
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
BOOL TSP_IsNeedToScanTS(U16 u16Channel)
{
    if (MAX_INPUT_CHANNEL_NUM <= u16Channel)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error,u16Channel[%u]\r\n",
            __FUNCTION__, __LINE__, u16Channel);
        return FALSE;
    }

    if (TSP_IsFirstScanTS(u16Channel) || Tuner_IsLockFreqChange(u16Channel))
    {
        return TRUE;
    }

    return FALSE;
}

/*****************************************************************************
  Function:       TSP_ScanTSRoutine
  Description:    搜台线程处理例程
  Input:          pArg:通道号
  Output:         
  Return:         
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
void *TSP_ScanTSRoutine(void *pArg)
{
    U8 u8Channel = 0;
    wvErrCode enErrCode = WV_SUCCESS;

    u8Channel = *((U8 *)pArg);

    while (1)
    {
        sleep(5);
        
        if (!TSP_IsNeedToScanTS(u8Channel))
        {
            #if 0
            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
                "[%s:%d]No Need To Scan u8Channel[%u]\r\n",
                __FUNCTION__, __LINE__, u8Channel);
            #endif
            
            continue;
        }

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
                "[%s:%d]Need To Scan u8Channel[%u]\r\n",
                __FUNCTION__, __LINE__, u8Channel);

        enErrCode = TSP_DelInputTSByChannel(u8Channel);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_DelInputTSByChannel Error,enErrCode[%08X],u8Channel[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u8Channel);
        }
        
        if (!TSP_IsReadyToScanTS(u8Channel))
        {
            #if 0
            log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
                "[%s:%d]Not Ready To Scan u8Channel[%u]\r\n",
                __FUNCTION__, __LINE__, u8Channel);
            #endif
            
            continue;
        }

        log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_TSP,
                "[%s:%d]Ready To Scan u8Channel[%u]\r\n",
                __FUNCTION__, __LINE__, u8Channel);

        enErrCode = TSP_ScanTS(u8Channel, SI_MODE_DVB);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_ScanTS Error,enErrCode[%08X],u8Channel[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u8Channel);
            continue;
        }

		//清除Tuner锁定发生变化的标志
        Tuner_ClearLockFreqChangeFlag(u8Channel);
        TSP_SetClearTSFlag(u8Channel, FALSE);

		/*
		
		//选择输出流号	
		LUT_OpenStream(u8Channel, TSP_InputChannel2StreamID(u8Channel));
			
		U8 u8LUTIndex = 0;
		U8 u8DestSlot = 0;
		U16 u16UDPPort = 0;
		U32 u32IPAddr = 0;
		u32IPAddr = 0xE30A145A + u8Channel;
		u16UDPPort = 1234;

		//TODO
		//设置输出的IP 、端口、MAC
		LUT_SetInputLUTIPInfo(u8Channel, u32IPAddr, u16UDPPort);
	
		*/
    }

    pthread_exit(NULL);
}

/*****************************************************************************
  Function:       TSP_ScanTSStart
  Description:    启动4个搜台线程，每个线程定时搜一个通道的流
  Input:          
  Output:         
  Return:         
  Others:         
  Create:         Momouwei 2017.02.17
*****************************************************************************/
int TSP_ScanTSStart(void)
{
    int res = 0;
    U8 i = 0;
    pthread_attr_t stThreadAttr;
    pthread_t arstScanTSThreadID[MAX_TS_SCANNER_NUM];
    static U8 aru8Arg[MAX_TS_SCANNER_NUM] = {0};
    
    res = pthread_attr_init(&stThreadAttr);
    if(0 != res)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, "pthread_attr_init error\r\n");
        return res;
    }

    res = pthread_attr_setdetachstate(&stThreadAttr, PTHREAD_CREATE_DETACHED);
    if(0 != res)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, "pthread_attr_setdetachstate error\r\n");
        return res;
    }

    for (i = 0; i < MAX_TS_SCANNER_NUM; i++)
    {
        aru8Arg[i] = i;

        res = pthread_create(&arstScanTSThreadID[i], &stThreadAttr, TSP_ScanTSRoutine, (void *)&aru8Arg[i]);
        if(0 == res)
        {
            LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSP, "Create scan TS thread[%u] successfully!\r\n", i);
        }
        else
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, "pthread_create error, i[%u]\r\n", i);
            return res;
        }
    }

    return 0;
}

