/*****************************************************************************
* Copyright(c)2013, HuiZhou WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName ��SISend.h
* Description ��
*
* Author ��ziteng.yang   Reviewer :
* Date   ��2014-06-06
*
*****************************************************************************/
#ifndef SISEND_H_
#define SISEND_H_

#include "TSconstruct.h"


#define SSD_FLAG_TRUE            (1)
#define SSD_FLAG_FALSE           (0)

#define MAX_TSPACKET_ONE_SI      (30)
#define SI_BUFFER_COUNT          (3000)


typedef struct
{
    U8  u8ValidFlag;
    U8  aru8Res[3];
    U16 u16Index;
    U16 u16NextIndex;

}SIList_t;

typedef struct
{
    SIList_t stList;
    U32 aru32TSPacket[PACKET_LENGTH_WELLAV_32BIT]; // TS��������

}TSPacketInfo_t;

// broadcast SI
typedef struct
{
    SIList_t stList;
    U16 u16OutTSIndex;
    U16 u16TSPacketIndex; // TS������ʼλ��
    U16 u16PID;
    U16 u16SISendSection; // ���ڼ�¼��ǰsi���͵ڼ���section
    U16 u16SITempPid;
    U8  u8TotalSectionNum;         // ���ڼ�¼�ܹ���section����
    U8  u8OverOneSectionTotalNum;  /* ���ڼ�¼��ε�section������
                                      ����ĳtable sectionΪ0���ǿ�θ���Ϊ0;
                                      ĳtable sectionNumΪ2���ǿ�θ���Ϊ2
                                   */
    U8  u8PacketNum;
    U8  u8SendFlag;
    U8  u8TSContinueCount; // ����PID�������Լ���
    U8  aru8Res;

}SIPacketInfo_t;

typedef struct
{
    U8  u8SendInterval; // ���ͼ������100msΪ��λ
    U16 u16TSPacketNum;       // TS�����ܸ���
    U16 u16SIInfoNum;    // SI������Ϣ����
    TSPacketInfo_t arstTSPacketInfo[SI_BUFFER_COUNT]; // ts������Ϣ
    SIPacketInfo_t arstSIPacketInfo[SI_BUFFER_COUNT]; // si��Ϣ
    U16 aru16OutTSSIInfo[MAX_OUTPUT_CHANNEL_NUM]; // ����ָ���outTS�ĵ�һ��SIPacketInfo��λ��

}SIPacket_t;

/*****************************************************************************
  Function:     SSD_GetSIPacketParamHandle
  Description:  SSD_GetSIPacketParamHandle
  Input:        none
  Output:       none
  Return:       pointer to s_arstSIPacket
  Author:       Momouwei 2016.12.11
*****************************************************************************/
SIPacket_t *SSD_GetSIPacketParamHandle(void);

/*****************************************************************************
  Function:     SSD_InitTSPacketInfo
  Description:  init TS Packet info
  Input:        u16TSPacketIndex
  Output:       none
  Return:       none
  Author:       ziteng.yang 2014.06.09
*****************************************************************************/
void SSD_InitTSPacketInfo(U16 u16TSPacketIndex);

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
void SSD_InitSIPacketInfo(U16 u16OutTSIndex, U16 u16SIPacketIndex, U16 u16Pid);

/*****************************************************************************
  Function:     SSD_GetUnuseTSPacketIndexList
  Description:  get unuse ts packet index list
  Input:        u16UnusePacketNum
  Output:       aru16UnuseList
  Return:       none
  Author:       ziteng.yang 2014.06.09
*****************************************************************************/
U32 SSD_GetUnuseTSPacketIndexList(U16 u16UnusePacketNum, U16 aru16UnuseList[]);

/*****************************************************************************
  Function:     SSD_GetUnuseSIPacketIndexList
  Description:  get unuse si packet index list
  Input:        u16UnusePacketNum
  Output:       aru16UnuseList
  Return:       none
  Author:       ziteng.yang 2014.06.09
*****************************************************************************/
U32 SSD_GetUnuseSIPacketIndexList(U16 u16UnusePacketNum, U16 aru16UnuseList[]);

/*****************************************************************************
  Function:     SSD_GetLastSIPacketIndex
  Description:  get output ts's last si packet info index
  Input:        u16OutTSIndex
  Output:       none
  Return:       LastSIPacketIndex
  Author:       ziteng.yang 2014.06.09
*****************************************************************************/
U16 SSD_GetLastSIPacketIndex(U16 u16OutTSIndex);

/*****************************************************************************
  Function:     SSD_GetLastTSPacketIndex
  Description:  get output si's last ts packet index
  Input:        u16SIPacketIndex
  Output:       none
  Return:       LastTSPacketIndex
  Author:       ziteng.yang 2014.06.09
*****************************************************************************/
U16 SSD_GetLastTSPacketIndex(U16 u16SIPacketIndex);
void SSD_SendBroadcastSI(void);
U32 SSD_AddBroadcastSI(U8 *pu8Section, U16 u16BuffLength, U16 u16OutTSIndex, U16 u16PID);
int SSD_MutexLock(void);
int SSD_MutexUnlock(void);
void SSD_InitBroadcastSI();

#endif /* SISEND_H_ */

