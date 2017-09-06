/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName ��resource.c
* Description ��board resource init and  process
* Author    : huada.huang
* Modified  :
* Reviewer  :
* Date      : 2017-03-9
* Record    :
*
**********************************************************************/
#include "appGLobal.h"
#include "tuner.h"
#include "wv_err.h"
#include "boardType.h"
#include "resource.h"
#include "status.h"
#include "wv_log.h"
#include "TSConstruct.h"
#include "PIN.h"
#include "FPGA.h"

/* all board resource */
static TResource_TS  s_stResource[MAX_SLOT_NUM];


U8 boardName[][20]=
{
    "CR-DVBC-00",
    "CR-DVBS2-00",
    "CR-DTMB-00"
};

/*****************************************************************************
  Function:     Resource_Init
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Resource_Init(U8 u8slot)
{
    TUNER_TYPE_enum e_tunerType;
    
    e_tunerType = Tuner_checkType();
    U32 u32LogicVersionNum = 0;
    U32 u32HardwareVersionNum = 0;

	printf("#################RESOUR SloT id = %u\n", u8slot);

    u32LogicVersionNum = FPGA_GetLogicVersion();
    u32HardwareVersionNum = PIN_GetHWVerSion();

    switch(e_tunerType)
    {
        case TUNER_C:
        case TUNER_S2:
        case TUNER_DTMB:
        {
            s_stResource[u8slot].u8SlotID = u8slot;
            s_stResource[u8slot].u8BoardType = CR_DVBC;
            s_stResource[u8slot].u32SoftVersion = SW_VER;
            s_stResource[u8slot].u32LogicVersion = u32LogicVersionNum;
            s_stResource[u8slot].u32HardVersion = u32HardwareVersionNum;
            memcpy(&s_stResource[u8slot].u8BoardDescriptor[0], boardName[e_tunerType], 20);
            s_stResource[u8slot].u8PortNum = DTV4U_TUNER_CHAN_NUM;
            s_stResource[u8slot].u16InputChannelNum = DTV4U_TUNER_CHAN_NUM;
            break;
        }
        default:
        {
            break;
        }
    }

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SYS,
        "Resource_Init:u8slot[%u],LogicVer[%X],HWVer[%X],SWVer[%X]\r\n",
        u8slot,
        u32LogicVersionNum,
        u32HardwareVersionNum,
        SW_VER);

    return WV_SUCCESS;    
}

/*****************************************************************************
  Function:     Resource_GetHandle
  Description:  get Resource handle of slot
  Input:        u8Slot
  Output:       none
  Return:       Resource handle of slot
  Author:       Momouwei 2017.04.13
*****************************************************************************/
TResource_TS *Resource_GetHandle(U8 u8Slot)
{
    if (MAX_SLOT_NUM <= u8Slot)
    {
        return NULL;
    }

    return &s_stResource[u8Slot];
}

/*****************************************************************************
  Function:     Resource_getSelfHandle
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
TResource_TS *Resource_getSelfHandle()
{
    U8 u8Slot = Status_GetSlotID();

    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "u8Slot[%u]", u8Slot);
    
    return Resource_GetHandle(u8Slot);
}

/*****************************************************************************
  Function:     Resource_SetSelfMac
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
void Resource_SetSelfMac(U8 *pu8MacAddr)
{
    U8 u8Slot = Status_GetSlotID();

    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS,
        "u8Slot[%u],MacAddr[%02X%02X%02X%02X%02X%02X]",
        u8Slot,
        pu8MacAddr[0],
        pu8MacAddr[1],
        pu8MacAddr[2],
        pu8MacAddr[3],
        pu8MacAddr[4],
        pu8MacAddr[5]);

    memcpy(&s_stResource[u8Slot].u8MACAddress[0], pu8MacAddr, 6);
}


/*****************************************************************************
  Function:     Resource_setSelfIpAddr
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
void Resource_setSelfIpAddr(U32 u32Ipaddr)
{
    U8 u8Slot = Status_GetSlotID();

    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS,
        "u8Slot[%u],u32Ipaddr[%d.%d.%d.%d]",
        u8Slot,
        (u32Ipaddr >> 24) & 0xFF,
        (u32Ipaddr >> 16) & 0xFF,
        (u32Ipaddr >>  8) & 0xFF,
        (u32Ipaddr >>  0) & 0xFF);
    
    s_stResource[u8Slot].u32IPAddress = u32Ipaddr;
}


/*****************************************************************************
  Function:     Resource_setSlotResource
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
void Resource_setSlotResource(U32 u32slot, U8 *u8Data)
{
    wvErrCode enErrCode = WV_SUCCESS;
    
    U32 u32OldIP = 0;
    U8 aru8OldMAC[6] = {0};
    
    TResource_TS stResource;
    memcpy(&stResource, (TResource_TS *)u8Data, sizeof(TResource_TS));

    u32OldIP = Resource_getBoardIPAddress(stResource.u8SlotID);
    Resource_getMACAddress(stResource.u8SlotID, aru8OldMAC);

    if ((u32OldIP != stResource.u32IPAddress)
        || (0 != memcmp(aru8OldMAC, stResource.u8MACAddress, 6)))
    {
        enErrCode = TSP_UpdateInputLUTInfo(stResource.u8SlotID,
                                            stResource.u32IPAddress,
                                            stResource.u8MACAddress);
        if (WV_SUCCESS != enErrCode)
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_UpdateInputLUTInfo Error:enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);
        }
    }
    
    memcpy(&s_stResource[stResource.u8SlotID], &stResource, sizeof(TResource_TS));
    //TODO Check
    Resource_Print(stResource.u8SlotID);
}

/*****************************************************************************
  Function:     Resource_SetSelfMac0LicFlag
  Description:  
  Input:        bFlag:TRUE:�Ѿ���Ȩ��FALSE:û��Ȩ
  Output:       none
  Return:       none
  Author:       Momouwei 2017.04.24
*****************************************************************************/
void Resource_SetSelfMac0LicFlag(BOOL bFlag)
{
    U8 u8Slot = Status_GetSlotID();

    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS,
        "u8Slot[%u],bFlag[%d]", u8Slot, bFlag);
    
    s_stResource[u8Slot].u8Mac0LicFlag = bFlag;
}

/*****************************************************************************
  Function:     Resource_setBackBoardIpAddr
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
void Resource_setBackBoardIpAddr(U32 u32Ipaddr)
{
    s_stResource[SLOT_BB].u32IPAddress = u32Ipaddr;
}

/*****************************************************************************
  Function:     Resource_IsOutputBoard
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       Momouwei 2017.04.14
*****************************************************************************/
BOOL Resource_IsOutputBoard(U8 u8BoardType)
{
    //if ((CMP_QAM_16 == u8BoardType) || (CMP_BACKBOARD == u8BoardType))
    {
        return TRUE;
    }

    return FALSE;
}

/*****************************************************************************
  Function:     Resource_getBoardType
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U8 Resource_getBoardType(U8 u8Slot)
{
    if(u8Slot > SLOT_BB)
    {
        return WV_ERR_PARAMS;
    }
    
    return s_stResource[u8Slot].u8BoardType;
}

/*****************************************************************************
  Function:     Resource_getMACAddress
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
wvErrCode Resource_getMACAddress(U8 u8Slot, U8 *u8mac)
{
    if(u8Slot > SLOT_BB)
    {
        return WV_ERR_PARAMS;
    }

    memcpy(u8mac, s_stResource[u8Slot].u8MACAddress, 6);
    
    return WV_SUCCESS;
}


/*****************************************************************************
  Function:     Resource_getBoardIPAddress
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U32 Resource_getBoardIPAddress(U32 u32Slot)
{
    if(u32Slot > SLOT_BB)
    {
        return WV_ERR_PARAMS;
    }
    
    return s_stResource[u32Slot].u32IPAddress;
}


/*****************************************************************************
  Function:     Resource_getBoardDescriptor
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
wvErrCode Resource_getBoardDescriptor(U8 u8Slot, U8 *pu8Descriptor)
{
    if ((u8Slot > SLOT_BB) || (!pu8Descriptor))
    {
        return WV_ERR_PARAMS;
    }
    memcpy(pu8Descriptor, &s_stResource[u8Slot].u8BoardDescriptor[0], 20);

    return  WV_SUCCESS;
}

/*****************************************************************************
  Function:     Resource_getBoardPortNum
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U8 Resource_getBoardPortNum(U8 u8Slot)
{
    if(u8Slot > SLOT_BB)
    {
        return WV_ERR_PARAMS;
    }
    
    return s_stResource[u8Slot].u8PortNum;
}

/*****************************************************************************
  Function:     Resource_getInputChannelNum
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U16 Resource_getInputChannelNum(U8 u8Slot)
{
    if(u8Slot > SLOT_BB)
    {
        return WV_ERR_PARAMS;
    }
    
    return s_stResource[u8Slot].u16InputChannelNum;
}

/*****************************************************************************
  Function:     Resource_getOutputChannelNum
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U16 Resource_getOutputChannelNum(U8 u8Slot)
{
    if(u8Slot > SLOT_BB)
    {
        return WV_ERR_PARAMS;
    }
    
    return s_stResource[u8Slot].u16OutputChannelNum;
}

/*****************************************************************************
  Function:     Resource_getOutputChannelStatus
  Description:  
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
U8 Resource_getOutputChannelStatus(U8 u8Slot, U8 u8Channel)
{
    if(u8Slot > SLOT_BB)
    {
        return WV_ERR_PARAMS;
    }
    
    return s_stResource[u8Slot].u8OutputChannelStatus[u8Channel];
}

/*****************************************************************************
  Function:     Resource_getOutputChannelStatus
  Description:
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
void Resource_Refresh(void)
{
    SLOT_ID u8slotID = SLOT0;

    //FIX ruibin.zhang ???????? SLOT_BB??????
    for(u8slotID = SLOT0; u8slotID < SLOT_BB; u8slotID ++)
    {
        if(!Status_CheckSubBoardPlugExist(u8slotID))
        {
            memset(&s_stResource[u8slotID], 0, sizeof(TResource_TS));
            LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "=========================", u8slotID);
            LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Refresh slot%d resource!", u8slotID);
            LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "=========================", u8slotID);
        }
    }

}

//debug
void Resource_Print(U8 u8slot)
{
    U8 *pu8data;
    U16 i = 0;
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "*** slot %d resource ***", u8slot);
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "u8SlotID 0x%x", s_stResource[u8slot].u8SlotID);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "BoardType 0x%x", s_stResource[u8slot].u8BoardType);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "MAC %02x:%02x:%02x:%02x:%02x:%02x"
            , s_stResource[u8slot].u8MACAddress[0]
            , s_stResource[u8slot].u8MACAddress[1]
            , s_stResource[u8slot].u8MACAddress[2]
            , s_stResource[u8slot].u8MACAddress[3]
            , s_stResource[u8slot].u8MACAddress[4]
            , s_stResource[u8slot].u8MACAddress[5]);
    
    pu8data = (U8 *)&s_stResource[u8slot].u32IPAddress;
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "IP %d.%d.%d.%d"
              , pu8data[3]
              , pu8data[2]
              , pu8data[1]
              , pu8data[0]);
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "u8BoardDescriptor  %s", s_stResource[u8slot].u8BoardDescriptor);

    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "u8PortNum %d", s_stResource[u8slot].u8PortNum);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "u16InputChannelNum %d", s_stResource[u8slot].u16InputChannelNum);
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "u16OutputChannelNum %d", s_stResource[u8slot].u16OutputChannelNum);

    for (i = 0; i < s_stResource[u8slot].u16OutputChannelNum; i++)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "OutputChannel[%02u],Status[%u]",i,s_stResource[u8slot].u8OutputChannelStatus[i]);
    }
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "***************************");
}


