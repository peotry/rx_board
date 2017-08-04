/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     status.c
* Description:  DEV-4U BackBoard status manage
* Author    :   ruibin.zhang
* Modified  :
* Reviewer  :
* Date      :   2017-03-02
* Record    :
*
**********************************************************************/
#include <string.h>
#include <time.h>

#include "status.h"
#include "wv_err.h"
#include "WV_log.h"
#include "FPGA.h"


#define YEAR_OFFSET     1900
#define MON_OFFSET      1
#define TEM_STR_LEN     128


static ST_WV_STATUS    g_stStatus;


/*****************************************************************************
  Function:     Status_Init
  Description:  Init status
  Input:        none
  Output:       none
  Return:       WV_ERR_FAILURE         --  失败
                WV_SUCCESS             --  成功
  Author:       ruibin.zhang
*****************************************************************************/
wvErrCode Status_Init(void)
{
    memset(&g_stStatus, 0, sizeof(ST_WV_STATUS));

    g_stStatus.u8SlotID = FPGA_GetSlotID();

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     Status_GetStatusHandler
  Description:  get g_stStatus handler
  Input:        none
  Output:       none
  Return:       g_stCurStatus handler
  Author:       ruibin.zhang
*****************************************************************************/
ST_WV_STATUS * Status_GetStatusHandler(void)
{
    return &g_stStatus;
}

/*****************************************************************************
  Function:     Status_GetSlotID
  Description:  get SubBoard Plug SlotID
  Input:        none
  Output:       none
  Return:       SubBoard Plug SlotID
  Author:       ruibin.zhang
*****************************************************************************/
VU8 Status_GetSlotID(void)
{
    return g_stStatus.u8SlotID;
}

/*****************************************************************************
  Function:     Status_SetSlotID
  Description:  set SubBoard Plug SlotID
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void Status_SetSlotID(VU8 u8SlotID)
{
    g_stStatus.u8SlotID = u8SlotID;
}

/*****************************************************************************
  Function:     Status_GetSubBoardPlugStatus
  Description:  get SubBoard Plug Status
  Input:        none
  Output:       none
  Return:       SubBoard Plug Status
  Author:       ruibin.zhang
*****************************************************************************/
VU16 Status_GetSubBoardPlugStatus(void)
{
    return g_stStatus.u16SubBoardplug;
}

/*****************************************************************************
  Function:     Status_SetSubBoardPlugStatus
  Description:  get SubBoard Plug Status
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void Status_SetSubBoardPlugStatus(VU16 u16SubBoardplug)
{
    g_stStatus.u16SubBoardplug = u16SubBoardplug;
}

/*****************************************************************************
  Function:     Status_CheckSubBoardPlugExist
  Description:  check slot SubBoard Plug
  Input:        none
  Output:       none
  Return:       exist 1
                no    0
  Author:       ruibin.zhang
*****************************************************************************/
bool Status_CheckSubBoardPlugExist(SLOT_ID u8slotID)
{
    if(u8slotID > SLOT15)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "SlotID out of range!");
        return false;
    }

    if((Status_GetSubBoardPlugStatus() >> u8slotID) & 0x1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*****************************************************************************
  Function:     Status_GetIPReqflag
  Description:  get IPReqflag
  Input:        none
  Output:       none
  Return:       IPReqflag
  Author:       ruibin.zhang
*****************************************************************************/
bool Status_GetIPReqflag(void)
{
    return g_stStatus.stUartCommflag.IPConifg;
}

/*****************************************************************************
  Function:     Status_SetIPReqflag
  Description:  set IPReqflag
  Input:        bool IPReqflag
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void Status_SetIPReqflag(bool IPReqflag)
{
    g_stStatus.stUartCommflag.IPConifg = IPReqflag;
}

/*****************************************************************************
  Function:     Status_GetIPReqflag
  Description:  get CommFinish
  Input:        none
  Output:       none
  Return:       CommFinish
  Author:       ruibin.zhang
*****************************************************************************/
bool Status_GetCommFinishflag(void)
{
    return g_stStatus.stUartCommflag.CommFinish;
}

/*****************************************************************************
  Function:     Status_SetIPReqflag
  Description:  set CommFinish
  Input:        bool CommFinish
  Output:       none
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void Status_SetCommFinishflag(bool CommFinish)
{
    g_stStatus.stUartCommflag.CommFinish = CommFinish;
}


/*****************************************************************************
  Function:     Status_SetSystemTime
  Description:  set system time
  Input:        u32CurrentTime  -- time value of sencond form 1970.1.1 00:00:00 to now
  Output:       none
  Return:       none
  Author:       huada.huang
*****************************************************************************/
void Status_SetSystemTime(U32 u32CurrentTime)
{
    struct tm  *pstTM           = NULL;
    time_t      stTime          = 0;
    U8          u8TmpString[TEM_STR_LEN] = {0};

    stTime = u32CurrentTime;
   
    pstTM = localtime(&stTime);
    pstTM->tm_year += YEAR_OFFSET;             //  clock start of year
    pstTM->tm_mon  += MON_OFFSET;

    snprintf ((char *)u8TmpString, TEM_STR_LEN, "date -s \"%04d-%02d-%02d %02d:%02d:%02d\"",
               pstTM->tm_year,
               pstTM->tm_mon,
               pstTM->tm_mday,
               pstTM->tm_hour,
               pstTM->tm_min,
               pstTM->tm_sec);


    printf("##set time## %s\r\n", u8TmpString);
    system((const char *)u8TmpString);

    return;
}
