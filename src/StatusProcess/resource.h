/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName ��resource.h
* Description ��board resource struct
* Author    : huada.huang
* Modified  :
* Reviewer  :
* Date      : 2017-03-9
* Record    :
*
**********************************************************************/
#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "appGlobal.h"


#define MAX_USER_NUM 6

typedef struct
{
    U8 userName[20];
    U8 password[20];
    U32 u32Level; //Ȩ��
}TUserInfo;


typedef struct
{
    U32     u32Version;                 //�ṹ��汾��

    //user info
    U32 u8UserNum;
    TUserInfo User[MAX_USER_NUM];

    //���Ӱ�����������Ϣ
    U8      u8SlotID;
    U8      u8BoardType;                                                                                                            //�����Ӱ�/����Ӱ�/�����Ӱ�/����
    U8      u8MACAddress[6];            //������Ӱ��MAC��ַ��Ψһ��ʶ��
    U32     u32SoftVersion;
    U32     u32LogicVersion;
    U32     u32HardVersion;
    U32     u32IPAddress;               //������Ӱ��IP��ַ
    U8      u8BoardDescriptor[20];      //�ַ��������Ӱ����ƣ���QAM/Encode

    //�ɱ������������Ϣ
    U8      u8ChangeStatus;
    U8      u8OldBoardType;                                                                                                         //�����Ӱ�/����Ӱ�/�����Ӱ�/����
    U8      u8OldMACAddress[6];         //������Ӱ��MAC��ַ��Ψһ��ʶ��
    U32     u32OldSoftVersion;
    U32     u32OldLogicVersion;
    U32     u32OldHardVersion;
    U32     u32OldIPAddress;            //������Ӱ��IP��ַ

    U8      u8PortNum;                                                                                                                //Ӳ���˿�����
    U8      u8Mac0LicFlag;
    U8      u8Res0[2];                                                                                                                   //Ӳ���˿�����
    U16     u16InputChannelNum;         //����ͨ������
    U16     u16OutputChannelNum;        //���ͨ������
    U8      u8OutputChannelStatus[256]; //���ͨ��״̬  ʹ��/��ռ�õ�

    U8      u8Res1[420];
}TResource_TS;   //sizeof(TResource_TS) = 1K




U32 Resource_Init(U8 u8slot);

TResource_TS * Resource_getSelfHandle();

void Resource_SetSelfMac(U8 *u8MacAddr);

void Resource_setSelfIpAddr(U32 u32Ipaddr);
void Resource_SetSelfMac0LicFlag(BOOL bFlag);
void Resource_setSlotResource(U32 u32slot, U8 *u8Data);

void Resource_setBackBoardIpAddr(U32 u32Ipaddr);

BOOL Resource_IsOutputBoard(U8 u8BoardType);
U8 Resource_getBoardType(U8 u8Slot);
wvErrCode Resource_getMACAddress(U8 u8Slot, U8 *u8mac);
U32 Resource_getBoardIPAddress(U32 u32Slot);
wvErrCode Resource_getBoardDescriptor(U8 u8Slot, U8  *u8Discroption);
U8 Resource_getBoardPortNum(U8 u8Slot);
U16 Resource_getInputChannelNum(U8 u8Slot);
U16 Resource_getOutputChannelNum(U8 u8Slot);
U8 Resource_getOutputChannelStatus(U8 u8Slot, U8 u8channel);
void Resource_Refresh(void);
//debug
void Resource_Print(U8 u8slot);


#endif
