/*****************************************************************************
* Copyright(c)2013, HuiZhou WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName ：SISend.c
* Description ：
*
* Author ：ziteng.yang   Reviewer :
* Date   ：2014-06-06
*
*****************************************************************************/
#include "SiSend.h"
#include "WV_log.h"
#include "WV_err.h"
#include "FPGA.h"
#include "tools.h"

//use to store broadcast SI
static SIPacket_t s_arstSIPacket;
static pthread_mutex_t s_SIPMutex = PTHREAD_MUTEX_INITIALIZER;

/*****************************************************************************
  Function:     SSD_MutexLock
  Description:  锁定流结构互赤锁
  Input:        
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
int SSD_MutexLock(void)
{
    return pthread_mutex_lock(&s_SIPMutex);
}

/*****************************************************************************
  Function:     SSD_MutexUnlock
  Description:  释放流结构互赤锁
  Input:        pTSPMutex
  Output:       
  Return:       
  Others:
  Create:       Momouwei 2017.02.23
*****************************************************************************/
int SSD_MutexUnlock(void)
{
    return pthread_mutex_unlock(&s_SIPMutex);
}

/*****************************************************************************
  Function:     SSD_GetSIPacketParamHandle
  Description:  SSD_GetSIPacketParamHandle
  Input:        none
  Output:       none
  Return:       pointer to s_arstSIPacket
  Author:       Momouwei 2016.12.11
*****************************************************************************/
SIPacket_t *SSD_GetSIPacketParamHandle(void)
{
    return &s_arstSIPacket;
}

/*****************************************************************************
  Function:     SSD_GetSendInterval
  Description:  get Send Interval
  Input:        none
  Output:       none
  Return:       SendInterval
  Author:       ziteng.yang 2014.06.06
*****************************************************************************/
U8 SSD_GetSendInterval()
{
    return s_arstSIPacket.u8SendInterval;
}

/*****************************************************************************
  Function:     SSD_SetSendInterval
  Description:  set Send Interval
  Input:        u8SendInterval -- 发送间隔，100ms为单位
  Output:       none
  Return:       none
  Author:       ziteng.yang 2014.06.06
*****************************************************************************/
void SSD_SetSendInterval(U8 u8SendInterval)
{
    s_arstSIPacket.u8SendInterval = u8SendInterval;
}

/*****************************************************************************
  Function:     SSD_InitBroadcastSI
  Description:  init broadcast SI
  Input:        none
  Output:       none
  Return:       none
  Author:       ziteng.yang 2014.06.06
*****************************************************************************/
void SSD_InitBroadcastSI()
{
    U16 i = 0;

    memset(&s_arstSIPacket, 0, sizeof(s_arstSIPacket));

    for (i = 0; i < MAX_OUTPUT_CHANNEL_NUM; i++)
    {
        s_arstSIPacket.aru16OutTSSIInfo[i] = INDEX_INVALID_VALUE;
    }

    SSD_SetSendInterval(1);

    return;
}

/*****************************************************************************
  Function:     SSD_AddBroadcastSI
  Description:  transfer SI data to SI buffer
  Input:        pu8Section        --- data of SI
                u16BuffLength     --- length of data
                u16OutTSIndex     --- index of Output TS's index
                u16PID            --- PID of SI
                u16TempPID        --- temp pid of si
  Output:       none
  Return:       SMP_SUCCESS or fail
  Author:       ziteng.yang 2014.06.09
*****************************************************************************/
U32 SSD_AddBroadcastSI(U8 *pu8Section, U16 u16BuffLength, U16 u16OutTSIndex, U16 u16PID)
{
    U32 u32Rtn                = WV_SUCCESS;
    U16 u16SectionLength      = 0;
    U16 u16AllLength          = 0;
    U16 u16CopyLength         = 0;
    U16 u16TempLength         = 0;
    U16 u16LastSIPacketIndex  = 0;
    U16 u16SIInfoIndex        = 0;
    U16 u16TSPacketIndex      = 0;
    U16 u16TSPacketCount      = 0;
    U16 u16StreamID           = 0;
    U8 u8SlotID           = 0;
    U8  u8AllPacketNum        = 0;
    U8  u8SectionPacketNum    = 0;
    U8  u8TotalSectionNum     = 0;
    U8  u8ContinuousCount     = 0;
    U8  u8SectionNum          = 0;
    //U8  u8LastSectionNum      = 0;
    //U8  u8SectionCount        = 0;
    U8  u8OverOneSectionTotalNum   = 0;
    U8  u8OverOneSectionCount      = 0;
    U8  i                          = 0;

    U8  *pu8Temp              = NULL;
    U8  *pu8Start             = NULL;

    SIPacketInfo_t *pstSIPacketInfo = NULL;
    TSPacketInfo_t *pstTSPacketInfo = NULL;

    TSInfo *pstParamTS = TSP_GetTSParamHandle();
    static U8 aru8Packet[PACKET_LENGTH_WELLAV];
    static U16 aru16TSPacketList[MAX_TSPACKET_ONE_SI];

    if ((!pu8Section) || (MAX_INPUT_CHANNEL_NUM <= u16OutTSIndex))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP, 
            "[%s:%d]Input error:pu8Section[%p],u16OutTSIndex[%u]\r\n",
            __FUNCTION__, __LINE__, pu8Section, u16OutTSIndex);
        return WV_ERR_SIP_ERROR_BAD_PARAM;
    }

    u16StreamID = TSP_CICamIndex2StreamID(pstParamTS->pOutputTS[u16OutTSIndex].u16ChannelID);
    u8SlotID = pstParamTS->pOutputTS[u16OutTSIndex].u8SlotID;

    pu8Start      = pu8Section;
    u16TempLength = u16BuffLength;

    // caculate packet number
    while (0 < u16TempLength)
    {
        pu8Temp = pu8Start;
        u8TotalSectionNum++;

        //find section length
        u16SectionLength = ((*(pu8Temp+1))<<8 | (*(pu8Temp+2))) & 0x0fff;
        u8SectionNum      = *(pu8Temp+6);
        //u8LastSectionNum  = *(pu8Temp+7);
        u16AllLength     = u16SectionLength + 3;

        // 跨段section需要有发包间隔以解决部分机顶盒buff不够问题
        if (0 != u8SectionNum)
        {
            // 最大发包间隔先限定不能大于200
            if (200 < (u8OverOneSectionTotalNum + 1))
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP, 
                    "[%s:%d]u16OutTSIndex[%u],u16PID[%u],u8OverOneSectionTotalNum[%u] out of range\r\n",
                    __FUNCTION__, __LINE__, u16OutTSIndex, u16PID, u8OverOneSectionTotalNum);

                return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
            }

            u8OverOneSectionTotalNum++;
        }

        //add point_field
        u8AllPacketNum += (u16AllLength+1)/PACKET_DATA_LENGTH + (((u16AllLength+1)%PACKET_DATA_LENGTH)?1:0);

        //more than one section
        if(u16TempLength >= u16AllLength)
        {
            u16TempLength -= u16AllLength;
            pu8Start      += u16AllLength;
        }
        else
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP, 
                "[%s:%d]u16OutTSIndex[%u],u16PID[%u],u16AllLength[%u],u16TempLength[%u] error\r\n",
                __FUNCTION__, __LINE__, u16OutTSIndex, u16PID, u16AllLength, u16TempLength);
            
            return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
        }
    }

    // 资源校验
    if (SI_BUFFER_COUNT <= s_arstSIPacket.u16SIInfoNum)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP, 
                "[%s:%d]u16OutTSIndex[%u],u16PID[%u],u16SIInfoNum[%u] overflow\r\n",
                __FUNCTION__, __LINE__, u16OutTSIndex, u16PID, s_arstSIPacket.u16SIInfoNum);
        return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
    }

    if (SI_BUFFER_COUNT < (s_arstSIPacket.u16TSPacketNum + u8AllPacketNum))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP, 
            "[%s:%d]u16OutTSIndex[%u],u16PID[%u],u16TSPacketNum[%u][%u], error\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex, u16PID, s_arstSIPacket.u16TSPacketNum, u8AllPacketNum);

        return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
    }

    if (MAX_TSPACKET_ONE_SI < u8AllPacketNum)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP, 
            "[%s:%d]u16OutTSIndex[%u],u16PID[%u],AllPacketNum[%u], error\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex, u16PID, u8AllPacketNum);
        return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
    }

    // 获取SI和ts报文资源
    u32Rtn = SSD_GetUnuseSIPacketIndexList(1, &u16SIInfoIndex);
    if (WV_SUCCESS != u32Rtn)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP, 
            "[%s:%d]SSD_GetUnuseSIPacketIndexList error:u16OutTSIndex[%u],u16PID[%u]\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex, u16PID);

        return u32Rtn;
    }

    u32Rtn = SSD_GetUnuseTSPacketIndexList(u8AllPacketNum, aru16TSPacketList);
    if (WV_SUCCESS != u32Rtn)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP, 
            "[%s:%d]SSD_GetUnuseTSPacketIndexList error:u16OutTSIndex[%u],u16PID[%u],u8AllPacketNum[%u]\r\n",
            __FUNCTION__, __LINE__, u16OutTSIndex, u16PID, u8AllPacketNum);
        return u32Rtn;
    }

    SSD_InitSIPacketInfo(u16OutTSIndex, u16SIInfoIndex, u16PID);

    pstSIPacketInfo = &s_arstSIPacket.arstSIPacketInfo[u16SIInfoIndex];
    pstSIPacketInfo->u16OutTSIndex             = u16OutTSIndex;
    pstSIPacketInfo->u16TSPacketIndex          = u16TSPacketIndex;
    pstSIPacketInfo->u16PID                    = u16PID;
    pstSIPacketInfo->u8TotalSectionNum         = u8TotalSectionNum;
    pstSIPacketInfo->u8OverOneSectionTotalNum  = u8OverOneSectionTotalNum;
    pstSIPacketInfo->u8PacketNum               = u8AllPacketNum;
    pstSIPacketInfo->u8SendFlag                = SSD_FLAG_TRUE;

    s_arstSIPacket.u16SIInfoNum++;

    pu8Start = pu8Section;
    while (u16BuffLength > 0)
    {
        pu8Temp = pu8Start;

        //find section length
        u16SectionLength  = ((*(pu8Temp+1))<<8 | (*(pu8Temp+2))) & 0x0fff;
        u8SectionNum      = *(pu8Temp+6);
        //u8LastSectionNum  = *(pu8Temp+7);
        u16AllLength      = u16SectionLength + 3;
        u16CopyLength     = u16AllLength;

        if (0 != u8SectionNum)
        {
            u8OverOneSectionCount++;
        }

        //copy data to buffer
        u8SectionPacketNum = (u16AllLength+1)/PACKET_DATA_LENGTH + (((u16AllLength+1)%PACKET_DATA_LENGTH)?1:0);
        for (i = 0; i < u8SectionPacketNum; i++)
        {
            u16TSPacketIndex = aru16TSPacketList[u16TSPacketCount];
            pstTSPacketInfo  = &s_arstSIPacket.arstTSPacketInfo[u16TSPacketIndex];

            SSD_InitTSPacketInfo(u16TSPacketIndex);

            if (0 == u16TSPacketCount)
            {
                pstSIPacketInfo->u16TSPacketIndex = u16TSPacketIndex;
            }

            // 最后一个ts packet
            if (u8AllPacketNum <= (u16TSPacketCount + 1))
            {
                pstTSPacketInfo->stList.u16NextIndex = INDEX_INVALID_VALUE;
            }
            else
            {
                pstTSPacketInfo->stList.u16NextIndex = aru16TSPacketList[u16TSPacketCount + 1];
            }

            memset(aru8Packet, 0xff, sizeof(aru8Packet));
            
           //伟乐模式下的196字节前的8字节
            memset(aru8Packet, 0xff, sizeof(aru8Packet));
            aru8Packet[0] = (u8SlotID << 2) | ((u16StreamID >> 14) & 0x3);
            aru8Packet[1] = (U8)(u16StreamID & 0xff);
            aru8Packet[2] = u8AllPacketNum;
            aru8Packet[6] = u8OverOneSectionTotalNum;  // 记录跨段section的总个数
            aru8Packet[7] = u8OverOneSectionCount;     // 记录前面多少个跨段section

            aru8Packet[8]  = 0x47;
            aru8Packet[9]  = (U8)(0x40 | (u16PID>>8));
            aru8Packet[10] = (U8)(u16PID & 0xff);
            aru8Packet[11] = 0x10 + u8ContinuousCount;

            u8ContinuousCount++;
            u8ContinuousCount = u8ContinuousCount%16;

            if(i == 0)
            {
                aru8Packet[12] = 0;   //add point_field

                if(u16CopyLength >= 183)
                {
                    memcpy(&aru8Packet[13],pu8Temp,183);
                    u16CopyLength -= 183;
                    pu8Temp       += 183;
                }
                else
                {
                    memcpy(&aru8Packet[13],pu8Temp,u16CopyLength);
                }
            }
            else
            {
                /*payload unit start indicator*/
                aru8Packet[9] = aru8Packet[9] & 0xbf;

                if(u16CopyLength >= 184)
                {
                    memcpy(&aru8Packet[12],pu8Temp,184);
                    u16CopyLength -= 184;
                    pu8Temp += 184;
                }
                else
                {
                    memcpy(&aru8Packet[12],pu8Temp,u16CopyLength);
                }
            }

            Common_ChangeBit8To32(pstTSPacketInfo->aru32TSPacket, aru8Packet, PACKET_LENGTH_WELLAV);

            u16TSPacketCount++;
            s_arstSIPacket.u16TSPacketNum++;
        }

        //more than one section
        if(u16BuffLength >= u16AllLength)
        {
            u16BuffLength -= u16AllLength;
            pu8Start      += u16AllLength;
            //u8SectionCount++;
        }
        else
        {
            // 按理这里需要回退，但是由于上面的计算和校验已经处理过，此处不会走到，因此不做回退
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP, 
                "[%s:%d]u16OutTSIndex[%u],u16PID[%u],SI data is wrong\r\n",
                __FUNCTION__, __LINE__, u16OutTSIndex, u16PID);
            return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
        }
    }

    // 将si信息存入aru16OutTSSIInfo
    u16LastSIPacketIndex = SSD_GetLastSIPacketIndex(u16OutTSIndex);

    if (SI_BUFFER_COUNT <= u16LastSIPacketIndex)
    {
        s_arstSIPacket.aru16OutTSSIInfo[u16OutTSIndex] = u16SIInfoIndex;
    }
    else
    {
        s_arstSIPacket.arstSIPacketInfo[u16LastSIPacketIndex].stList.u16NextIndex = u16SIInfoIndex;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     SSD_SendBroadcastSI
  Description:  Send SI
  Input:        none
  Output:       none
  Return:       none
  Author:       ziteng.yang 2014.06.09
*****************************************************************************/
void SSD_SendBroadcastSI(void)
{
    U8  u8OverOneSectionTotalNum   = 0;
    U8  u8OverOneSectionCount      = 0;
    //U8  u8LastSectionNumber = 0;
    //U8  u8SectionNumber     = 0;
    U16 u16SIInfoIndex      = 0;
    U16 u16TSPacketIndex    = 0;
    U16 i                   = 0;

    SIPacketInfo_t *pstSIPacketInfo = NULL;
    TSPacketInfo_t *pstTSPacketInfo = NULL;

    for (i = 0; i < MAX_OUTPUT_CHANNEL_NUM; i++)
    {
        if (SI_BUFFER_COUNT <= s_arstSIPacket.aru16OutTSSIInfo[i])
        {
            continue;
        }
        
        // 遍历ts下的所有si信息，
        u16SIInfoIndex = s_arstSIPacket.aru16OutTSSIInfo[i];
        while (SI_BUFFER_COUNT > u16SIInfoIndex)
        {
            //printf("SSD_SendBroadcastSI u16SIInfoIndex[%d]\n", u16SIInfoIndex);
            pstSIPacketInfo = &s_arstSIPacket.arstSIPacketInfo[u16SIInfoIndex];

            // 遍历si下的所有ts信息，并依次发送TS包
            u16TSPacketIndex = pstSIPacketInfo->u16TSPacketIndex;
            while (SI_BUFFER_COUNT > u16TSPacketIndex)
            {
                pstTSPacketInfo  = &s_arstSIPacket.arstTSPacketInfo[u16TSPacketIndex];
                //printf("SSD_SendBroadcastSI u16TSPacketIndex[%d]\n", u16TSPacketIndex);

                // 没有发包间隔，则连续发送
                if (0 == s_arstSIPacket.u8SendInterval)
                {
                    pstTSPacketInfo->aru32TSPacket[2] = (pstTSPacketInfo->aru32TSPacket[2]&0xfffffff0)
                         + (((pstTSPacketInfo->aru32TSPacket[2]&0xf)+(pstTSPacketInfo->aru32TSPacket[0]>>8))&0xf);

                    FPGA_SendSIPacket2Logic(pstTSPacketInfo->aru32TSPacket, PACKET_LENGTH_WELLAV_32BIT);
                }
                else // 有发包间隔则按section 按间隔发送
                {
                    u8OverOneSectionTotalNum = (pstTSPacketInfo->aru32TSPacket[1]>>8) & 0xff;

                    if(0 == u8OverOneSectionTotalNum)
                    {
                        pstTSPacketInfo->aru32TSPacket[2] = (pstTSPacketInfo->aru32TSPacket[2]&0xfffffff0)
                              + (((pstTSPacketInfo->aru32TSPacket[2]&0xf)+(pstTSPacketInfo->aru32TSPacket[0]>>8))&0xf);

                        FPGA_SendSIPacket2Logic(pstTSPacketInfo->aru32TSPacket, PACKET_LENGTH_WELLAV_32BIT);
                    }
                    else
                    {
                        u8OverOneSectionCount = pstTSPacketInfo->aru32TSPacket[1] & 0xff;

                        if ((pstSIPacketInfo->u16SISendSection % (u8OverOneSectionTotalNum + 1)) == u8OverOneSectionCount)
                        {
                            pstTSPacketInfo->aru32TSPacket[2] = (pstTSPacketInfo->aru32TSPacket[2]&0xfffffff0)
                                + (((pstTSPacketInfo->aru32TSPacket[2]&0xf)+(pstTSPacketInfo->aru32TSPacket[0]>>8))&0xf);

                            FPGA_SendSIPacket2Logic(pstTSPacketInfo->aru32TSPacket, PACKET_LENGTH_WELLAV_32BIT);
                        }
                    }
                }

                u16TSPacketIndex = pstTSPacketInfo->stList.u16NextIndex;
            }

            if (0 == s_arstSIPacket.u8SendInterval)
            {
                pstSIPacketInfo->u16SISendSection = 0;
            }
            else
            {
                pstSIPacketInfo->u16SISendSection++;
                if(pstSIPacketInfo->u16SISendSection > pstSIPacketInfo->u8OverOneSectionTotalNum)
                {
                    pstSIPacketInfo->u16SISendSection = 0;
                }
            }

            u16SIInfoIndex = pstSIPacketInfo->stList.u16NextIndex;
        }
    }
}

/*****************************************************************************
  Function:     SSD_InitTSPacketInfo
  Description:  init TS Packet info
  Input:        u16TSPacketIndex
  Output:       none
  Return:       none
  Author:       ziteng.yang 2014.06.09
*****************************************************************************/
void SSD_InitTSPacketInfo(U16 u16TSPacketIndex)
{
    TSPacketInfo_t *pstTSPacketInfo = NULL;

    if (SI_BUFFER_COUNT <= u16TSPacketIndex)
    {
        return;
    }

    pstTSPacketInfo = &s_arstSIPacket.arstTSPacketInfo[u16TSPacketIndex];

    pstTSPacketInfo->stList.u8ValidFlag  = SSD_FLAG_TRUE;
    pstTSPacketInfo->stList.u16Index     = u16TSPacketIndex;
    pstTSPacketInfo->stList.u16NextIndex = INDEX_INVALID_VALUE;

    memset(pstTSPacketInfo->aru32TSPacket, 0xff, sizeof(pstTSPacketInfo->aru32TSPacket));

    return;
}

/*****************************************************************************
  Function:     SSD_InitSIPacketInfo
  Description:  init SI info
  Input:        u16OutTSIndex
                u16SIPacketIndex
                u16Pid
  Output:       none
  Return:       none
  Author:       ziteng.yang 2014.06.09
*****************************************************************************/
void SSD_InitSIPacketInfo(U16 u16OutTSIndex, U16 u16SIPacketIndex, U16 u16Pid)
{
    SIPacketInfo_t *pstSIPacketInfo = NULL;

    if (SI_BUFFER_COUNT <= u16SIPacketIndex)
    {
        return;
    }

    pstSIPacketInfo = &s_arstSIPacket.arstSIPacketInfo[u16SIPacketIndex];

    memset(pstSIPacketInfo, 0, sizeof(SIPacketInfo_t));

    pstSIPacketInfo->stList.u8ValidFlag  = SSD_FLAG_TRUE;
    pstSIPacketInfo->stList.u16Index     = u16SIPacketIndex;
    pstSIPacketInfo->stList.u16NextIndex = INDEX_INVALID_VALUE;

    pstSIPacketInfo->u16OutTSIndex    = u16OutTSIndex;
    pstSIPacketInfo->u16TSPacketIndex = INDEX_INVALID_VALUE;
    pstSIPacketInfo->u16PID           = u16Pid;
    pstSIPacketInfo->u16SITempPid     = INDEX_INVALID_VALUE;;

    return;
}

/*****************************************************************************
  Function:     SSD_GetUnuseTSPacketIndexList
  Description:  get unuse ts packet index list
  Input:        u16UnusePacketNum
  Output:       aru16UnuseList
  Return:       none
  Author:       ziteng.yang 2014.06.09
*****************************************************************************/
U32 SSD_GetUnuseTSPacketIndexList(U16 u16UnusePacketNum, U16 aru16UnuseList[])
{
    U16 i = 0;
    U16 u16UnuseTSPacket = 0;
    TSPacketInfo_t *pstTSPacketInfo = NULL;

    if (0 == u16UnusePacketNum)
    {
        return WV_SUCCESS;
    }

    if (SI_BUFFER_COUNT < (s_arstSIPacket.u16TSPacketNum + u16UnusePacketNum))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP, 
            "[%s:%d] get UnusePacketNum:%d not enough",u16UnusePacketNum);
        return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
    }

    for (i = 0; i < SI_BUFFER_COUNT; i++)
    {
        pstTSPacketInfo = &s_arstSIPacket.arstTSPacketInfo[i];

        if (SSD_FLAG_TRUE == pstTSPacketInfo->stList.u8ValidFlag)
        {
            continue;
        }

        aru16UnuseList[u16UnuseTSPacket] = i;
        u16UnuseTSPacket++;

        if (u16UnuseTSPacket == u16UnusePacketNum)
        {
            return WV_SUCCESS;
        }
    }

    return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
}

/*****************************************************************************
  Function:     SSD_GetUnuseSIPacketIndexList
  Description:  get unuse si packet index list
  Input:        u16UnusePacketNum
  Output:       aru16UnuseList
  Return:       none
  Author:       ziteng.yang 2014.06.09
*****************************************************************************/
U32 SSD_GetUnuseSIPacketIndexList(U16 u16UnusePacketNum, U16 aru16UnuseList[])
{
    U16 i                = 0;
    U16 u16UnuseSIPacket = 0;
    SIPacketInfo_t *pstSIPacketInfo = NULL;

    if (0 == u16UnusePacketNum)
    {
        return WV_SUCCESS;
    }

    if (SI_BUFFER_COUNT < (s_arstSIPacket.u16SIInfoNum + u16UnusePacketNum))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SIP, 
            "[%s:%d] get UnusePacketNum:%d not enough",u16UnusePacketNum);
        return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
    }

    for (i = 0; i < SI_BUFFER_COUNT; i++)
    {
        pstSIPacketInfo = &s_arstSIPacket.arstSIPacketInfo[i];

        if (SSD_FLAG_TRUE == pstSIPacketInfo->stList.u8ValidFlag)
        {
            continue;
        }

        aru16UnuseList[u16UnuseSIPacket] = i;
        u16UnuseSIPacket++;

        if (u16UnuseSIPacket == u16UnusePacketNum)
        {
            return WV_SUCCESS;
        }
    }

    return WV_ERR_SIP_ERROR_OUT_OF_RANGE;
}

/*****************************************************************************
  Function:     SSD_GetLastSIPacketIndex
  Description:  get output ts's last si packet info index
  Input:        u16OutTSIndex
  Output:       none
  Return:       LastSIPacketIndex
  Author:       ziteng.yang 2014.06.09
*****************************************************************************/
U16 SSD_GetLastSIPacketIndex(U16 u16OutTSIndex)
{
    U16 u16SIIndex       = 0;
    SIPacketInfo_t *pstSIPacketInfo = NULL;

    if (MAX_OUTPUT_CHANNEL_NUM <= u16OutTSIndex)
    {
        return INDEX_INVALID_VALUE;
    }

    u16SIIndex = s_arstSIPacket.aru16OutTSSIInfo[u16OutTSIndex];
    if (SI_BUFFER_COUNT <= u16SIIndex)
    {
        return INDEX_INVALID_VALUE;
    }

    // 轮询ts的si报文列表，获取最后一个si报文信息的索引值
    while (SI_BUFFER_COUNT > u16SIIndex)
    {
        pstSIPacketInfo = &s_arstSIPacket.arstSIPacketInfo[u16SIIndex];

        if (SI_BUFFER_COUNT <= pstSIPacketInfo->stList.u16NextIndex)
        {
            return pstSIPacketInfo->stList.u16Index;
        }

        u16SIIndex = pstSIPacketInfo->stList.u16NextIndex;
    }

    return INDEX_INVALID_VALUE;
}

/*****************************************************************************
  Function:     SSD_GetLastTSPacketIndex
  Description:  get output si's last ts packet index
  Input:        u16SIPacketIndex
  Output:       none
  Return:       LastTSPacketIndex
  Author:       ziteng.yang 2014.06.09
*****************************************************************************/
U16 SSD_GetLastTSPacketIndex(U16 u16SIPacketIndex)
{
    U16 u16TSPacketIndex = 0;
    SIPacketInfo_t *pstSIPacketInfo = NULL;
    TSPacketInfo_t *pstTSPacketInfo = NULL;

    if (SI_BUFFER_COUNT <= u16SIPacketIndex)
    {
        return INDEX_INVALID_VALUE;
    }

    pstSIPacketInfo = &s_arstSIPacket.arstSIPacketInfo[u16SIPacketIndex];

    if ((SSD_FLAG_TRUE != pstSIPacketInfo->stList.u8ValidFlag)
       || (SI_BUFFER_COUNT <= pstSIPacketInfo->u16TSPacketIndex))
    {
        return INDEX_INVALID_VALUE;
    }

    u16TSPacketIndex = pstSIPacketInfo->u16TSPacketIndex;

    while (SI_BUFFER_COUNT > u16TSPacketIndex)
    {
        pstTSPacketInfo = &s_arstSIPacket.arstTSPacketInfo[u16TSPacketIndex];

        if (SI_BUFFER_COUNT <= pstTSPacketInfo->stList.u16NextIndex)
        {
            return pstTSPacketInfo->stList.u16Index;
        }

        u16TSPacketIndex = pstTSPacketInfo->stList.u16NextIndex;
    }

    return INDEX_INVALID_VALUE;
}


