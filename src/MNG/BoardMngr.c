/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     BoardMngr.c
* Description:  Board Manager Function
* Author    :   jie.zhan
* Modified  :
* Reviewer  :
* Date      :   2017-03-28
* Record    :
*
**********************************************************************/

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "BoardMngr.h"
#include "WV_log.h"
#include "wv_err.h"
#include "IPComm.h"

DS2432_PerBoard_t g_stBoardDS2432;
LicenseInfo_t g_stSBLicenseInfo;

extern unsigned char u8Code_DS2432[8];

/*****************************************************************************
  Function:     BMN_GetDS2432_Handle
  Description:  获取DS2432控制句柄
  Input:        none
  Output:       none
  Return:       g_stBoardDS2432变量地址
  Author:       jie.zhan
*****************************************************************************/
DS2432_PerBoard_t * BMN_GetDS2432_Handle()
{   
    return &g_stBoardDS2432;
}

/*****************************************************************************
  Function:     BMN_GetDS2432_Handle
  Description:  获取DS2432控制句柄
  Input:        none
  Output:       none
  Return:       g_stSBLicenseInfo变量地址
  Author:       jie.zhan
*****************************************************************************/
LicenseInfo_t * BMN_GetLicenseInfo_Handle()
{   
    return &g_stSBLicenseInfo;
}

/*****************************************************************************
  Function:     Get_BoardLicense
  Description:  获取板License的Chip ID和数据保存到全局变量中
  Input:        none
  Output:       none
  Return:       u32ReturnValue
  Author:       jie.zhan
*****************************************************************************/
U32 BMN_GetBoardLicense(void)
{
    U32 u32ReturnValue      = 0;
    U8  u8MaxRetrytime      = 3;
    U8  aru8DS2432ChipID[8] = {0};
    U8  aru8DS2432Data[DS2432_MAX_DATA_LENGTH] = {0};

    DS2432_SetOperateIndex(SLOT_MAINBOARD);
    
    do
    {
        u32ReturnValue = DS2432_Verification(aru8DS2432Data, DS2432_MAX_DATA_LENGTH, u8Code_DS2432,
                                     0, 23, aru8DS2432ChipID);

        (void)DS2432_UpdateMainboardLicense(aru8DS2432ChipID, aru8DS2432Data);
        
        if(u32ReturnValue == DS2432_SUCCESS)
        {
            u32ReturnValue = DS2432_Check(aru8DS2432Data);
            if(u32ReturnValue == DS2432_SUCCESS)
            {
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DS2432, "[DS2432_GetData] Check License Success!! (%d)", u32ReturnValue);
            }
        }
        
        u8MaxRetrytime--;

    }while((u8MaxRetrytime > 0) && (u32ReturnValue != DS2432_SUCCESS));

    if(u32ReturnValue != DS2432_SUCCESS)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_GetData] Get baseboard's license failure!! (%d)", u32ReturnValue);
        return u32ReturnValue;
    }

    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DS2432, "[DS2432_GetData] Get baseboard's license success!! (%d)", u32ReturnValue);
    
    g_stBoardDS2432.u8SlotNumber = SLOT_MAINBOARD;
    memcpy(g_stBoardDS2432.u8ChipID, aru8DS2432ChipID, sizeof(aru8DS2432ChipID));
    memcpy(&(g_stBoardDS2432.stDS2432_ChipInfo), aru8DS2432Data, sizeof(DS2432_ChipInfo_t));

    printf("BMN_GetBoardLicense:\r\n");
    printf("ChipID[%02X%02X%02X%02X%02X%02X%02X%02X]\r\n",
        aru8DS2432ChipID[0],
        aru8DS2432ChipID[1],
        aru8DS2432ChipID[2],
        aru8DS2432ChipID[3],
        aru8DS2432ChipID[4],
        aru8DS2432ChipID[5],
        aru8DS2432ChipID[6],
        aru8DS2432ChipID[7]);

    printf("u8Company[%c%c%c],u8StructVer[%u],u8DataCheckSum[%02X],u8SubboardID[%u],u8HWVersion[%u]\r\n",
        g_stBoardDS2432.stDS2432_ChipInfo.u8Company[0],
        g_stBoardDS2432.stDS2432_ChipInfo.u8Company[1],
        g_stBoardDS2432.stDS2432_ChipInfo.u8Company[2],
        g_stBoardDS2432.stDS2432_ChipInfo.u8StructVer,
        g_stBoardDS2432.stDS2432_ChipInfo.u8DataCheckSum,
        g_stBoardDS2432.stDS2432_ChipInfo.u8SubboardID,
        g_stBoardDS2432.stDS2432_ChipInfo.u8HWVersion);

    printf("u8TAGDataLen[%d],u32LastUpdateTime[%u-%u-%u]\r\n",
        g_stBoardDS2432.stDS2432_ChipInfo.u8TAGDataLen,
        (g_stBoardDS2432.stDS2432_ChipInfo.u32LastUpdateTime >> 16) & 0xFFFF,
        (g_stBoardDS2432.stDS2432_ChipInfo.u32LastUpdateTime >> 8) & 0xFF,
        g_stBoardDS2432.stDS2432_ChipInfo.u32LastUpdateTime & 0xFF);

    printf("u8TAGData:\r\n");

    U16 i = 0;
    U16 j = 0;
    for (i = 0; i < 116; i++)
    {
        printf("%02X ", g_stBoardDS2432.stDS2432_ChipInfo.u8TAGData[i]);
       j++;

       if (0 == (j%16))
       {
            printf("\r\n");
       }
    }

    printf("\r\n");

    return u32ReturnValue;
}

/*****************************************************************************
  Function:     Parse_BoardLicense
  Description:  解析Lisence信息到变量g_stSBLicenseInfo
  Input:        none
  Output:       none
  Return:       Sucess Or Error Code
  Author:       jie.zhan
*****************************************************************************/
U32 BMN_ParseBoardLicense(void)
{
    U32 u32ReturnValue = 0;

    u32ReturnValue = LIC_ParseLicenseInfo((U8*)&(g_stBoardDS2432.stDS2432_ChipInfo), &(g_stSBLicenseInfo));
    if(u32ReturnValue != DS2432_SUCCESS)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[SBM_ParseSubLicense] Parse License Fail!");
        return u32ReturnValue;
    }

    return DS2432_SUCCESS;
}

/*****************************************************************************
  Function:     BMN_Reboot
  Description:  重启
  Input:        
                
  Output:       
  Return:       
  Author:       Momouwei 2017.04.14
*****************************************************************************/
wvErrCode BMN_Reboot(void)
{
    wvErrCode enErrCode = WV_SUCCESS;

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "********Reboot******\r\n");
    
    enErrCode = IPComm_SendCmd2Slot(SLOT_BB, IPCOMM_REQ_REBOOT);
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP, 
            "[%s:%d]IPComm_SendCmd2Slot error,enErrCode[%u]\r\n",
            __FUNCTION__, __LINE__, enErrCode);

        return enErrCode;
    }
    
    return WV_SUCCESS;
}


