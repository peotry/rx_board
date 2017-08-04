/*****************************************************************************
* Copyright (c) 2012,WellAV Technologies Ltd. All rights reserved. 
* File Name：       CI_FingerPrint.c
* Description:      
* History:      
                        
<Author>            <Date>              <Version >          <Reason>
Hawayi.Liu          2012/11/22          1.0                 Create
*****************************************************************************/

#include "all.h"
#include "CI_FingerPrint.h"
//#include "../MainDecodeInterface.h"
//#include "../MAIN2DECODER.h"
//#include "../DECODER.h"
//#include "../../Comm/communication.h"

//#include "Ucos_ii.h"
//#include "TRACE/FlashLog.h"
//#include "IRDR2/MAINBOARD.h"

#define FP_DEBUG_ENABLE
//#define FP_SEND2TC_ENABLE

#ifdef FP_DEBUG_ENABLE
#define FP_DEBUG(x) printf x
#else
#define FP_DEBUG(x)
#endif

#if defined(ENABLE_FINGER_PRINT)
static BOOL FingerPrint_Connect(CI_HANDLE hCI, WORD hSession);
static void FingerPrint_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen);
static void FingerPrint_Close(CI_HANDLE hCI, WORD hSession);

extern int Ts_SendFinger(BYTE *pData, U32 u32Length);
extern DWORD GetCurrentTime(void);

//request tag with application ID ??"privatFP"
const BYTE FP_ENQ[12]={0x9F, 0x9A, 0x00, 0x08, 0x70, 0x72, 0x69, 0x76, 0x61, 0x74, 0x46, 0x50};
const RESOURCE_INFO FingerPrint = {RESOURCEID_SAS, FingerPrint_Connect, FingerPrint_Data, FingerPrint_Close};

static FP_Infor stFpInfor;
static FP_Stime stDate;
///////////////////////////////////////////////////////////////////////////////////////////
static DWORD _DateToSecond(WORD wYear,
                              BYTE ucMonth,
                              BYTE ucDay,
                              BYTE ucHour,
                              BYTE ucMintue,
                              BYTE ucSecond)
{
    DWORD wTotalSecond = 0,wTotalDay = 0;
    WORD  wStatrtYear = 1970;
    int   i = wStatrtYear;
    BOOL  btLeapYearFlg = FALSE;

    for (; i <= wYear; i++) //年
    {
         btLeapYearFlg = (i%4 == 0 &&(i%100 || i%400 == 0)) ? TRUE :FALSE;
        if (i < wYear)   
            wTotalDay += (btLeapYearFlg) ? 366 : 365;
    }
    for (i = 1; i < ucMonth; i++)//月
        wTotalDay += (i==1 || i==3 || i==5 || i==7 ||i==8 ||i==10 || i==12) ? 31 : (i != 2)  ? 30 : (btLeapYearFlg) ? 29 : 28;
    wTotalDay += ucDay - 1; //日
    wTotalSecond = ((wTotalDay * 24 + ucHour) * 60 + ucMintue) * 60 + ucSecond;
    return wTotalSecond;
}

static BOOL _SecondToDate(DWORD dwTotalSecond,
                             WORD* pwYear,
                             BYTE* pucMonth,
                             BYTE* pucDay,
                             BYTE* pucHour,
                             BYTE* pucMinute,
                             BYTE* pucSecond)
{
    WORD  wTemyear = 1970;
    DWORD dwSecondCound = 0;
    BYTE  ucMonth,ucDay,ucHour,ucMinute,ucSceond;
    BOOL  btLeapYearFlg = FALSE;
    DWORD dwOnceSecCount=0,dwtSecondSurplus = 0;
    DWORD dwLeapYSec   = 366 * 24 * 3600;
    DWORD dwYearSec    = 365 * 24 * 3600;
    DWORD dwDaySecond  = 24 *3600;
    int   i = wTemyear;

    ucMonth = ucMinute = ucDay = ucHour = ucSceond = 0;
    for (; ; i++)//年
    {
        btLeapYearFlg = (i%4 == 0 &&(i%100 || i%400 == 0)) ? TRUE :FALSE;
        dwOnceSecCount =  (btLeapYearFlg) ? dwLeapYSec : dwYearSec;
        if (dwTotalSecond < (dwtSecondSurplus + dwOnceSecCount) || dwtSecondSurplus > (dwtSecondSurplus + dwOnceSecCount))//数据到FFFFFFFF时，会溢出。
            break;
        dwtSecondSurplus += dwOnceSecCount;
    }
    wTemyear = i;
    dwSecondCound = dwTotalSecond - dwtSecondSurplus;
    dwtSecondSurplus = 0;
    //月
    for (i = 1; i < 13; i++)
    {
        ucDay = (i==1 || i==3 || i==5 || i==7 ||i==8 ||i==10 || i==12) ? 31 : (i != 2)  ? 30 : (btLeapYearFlg) ? 29 : 28;
        dwOnceSecCount = ucDay * dwDaySecond;
        if (dwSecondCound < (dwtSecondSurplus + dwOnceSecCount))
            break;
        dwtSecondSurplus += dwOnceSecCount;
    }
    ucMonth = i;
    dwSecondCound -= dwtSecondSurplus;
    dwtSecondSurplus = 0;
    //日
    dwOnceSecCount = dwDaySecond;
    for (i = 1; i < ucDay + 1; i++)
    {
        if (dwSecondCound < (dwtSecondSurplus + dwOnceSecCount))
            break;
        dwtSecondSurplus += dwOnceSecCount;
    }
    ucDay = i;
    //时、分、秒
    dwSecondCound -= dwtSecondSurplus;
    dwtSecondSurplus = 0;
    ucHour = dwSecondCound / 3600;
    dwSecondCound %= 3600;
    ucMinute = dwSecondCound / 60;
    ucSceond = dwSecondCound % 60;
    //结果
    *pwYear    = wTemyear;
    *pucMonth  = ucMonth;
    *pucDay    = ucDay;
    *pucHour   = ucHour;
    *pucMinute = ucMinute;
    *pucSecond = ucSceond;

    return TRUE;
}

static void _MjdToYMD(WORD Mjd,WORD *Year,BYTE *Month,BYTE *Day/*,BYTE *WeekDay*/)
{
    long  Mjd32;        /* 32bit MJD */
    long  y, m, k;  /* Y', M', K */

    Mjd32 = (long)Mjd;  /* Set 32 bit MJD */
    if( Mjd32 < 15079 ) /* MJD Lower than 1900/3/1 ? */
        Mjd32 += 0x10000;/* Adjust MJD */

    y = ( Mjd32 * 100 - 1507820 ) / 36525;                  /* Calculate Y', M' */
    m = ( Mjd32 * 10000 - 149561000 - ( ( y * 36525 ) / 100 ) * 10000 ) / 306001;
    *Day = (BYTE)( Mjd32 - 14956 - ( ( y * 36525 ) / 100 ) - ( ( m * 306001 ) / 10000 ) );/* Calculate Day */

    k = ( ( m == 14 ) || ( m == 15 ) ) ? 1 : 0;                 /* If M'=14 or M'=15 then K=1 else K=0 */
    *Year = (WORD)( y + k ) + 1900;                             /* Calculate Year */
    *Month = (BYTE)( m - 1 - k * 12 );                          /* Calculate Month */
    //*WeekDay = (BYTE)( ( ( Mjd32 + 2 ) % 7 ) + 1 );               /* Calculate Week Day */
}

static void _FP_SendMessage(CI_HANDLE hCI, const BYTE *pData, WORD wLen)
{
    FP_DEBUG(("[%s] wLen:%d.\r\n", __FUNCTION__, (int)wLen));

    if(hCI->wFP_Session){
        static BYTE u8MsgCnt = 0;
        BYTE u8Offset = 0; 
        WORD wLenField = wLen + 3;
        
        BYTE *p8Msg = malloc(wLenField+6);//Tag:3 bytes; Length Field:3 bytes max;
        if (p8Msg != NULL){
            p8Msg[0] = (BYTE)(TAG_SAS_ASYNC_MSG >> 16);
            p8Msg[1] = (BYTE)(TAG_SAS_ASYNC_MSG >> 8);
            p8Msg[2] = (BYTE)(TAG_SAS_ASYNC_MSG >> 0);
            if (wLenField < 128){
                p8Msg[3] = (BYTE)(wLenField & 0x00FF);
                u8Offset = 1; 
            }
            else if (wLenField < 256){
                p8Msg[3] = 0x80;
                p8Msg[4] = (BYTE)(wLenField & 0x00FF);
                u8Offset = 2; 
            }
            else {
                p8Msg[3] = 0x80;
                p8Msg[4] = (BYTE)((wLenField>>8) & 0x00FF);
                p8Msg[5] = (BYTE)(wLenField & 0x00FF);
                u8Offset = 3; 
            }
            p8Msg[u8Offset+3] = u8MsgCnt++;
            p8Msg[u8Offset+4] = (BYTE)((wLen>>8) & 0x00FF);
            p8Msg[u8Offset+5] = (BYTE)(wLen & 0x00FF);

            memcpy(p8Msg+u8Offset+6, pData, wLen);
            wLenField += u8Offset + 3;
            Send_SPDU_Data(hCI, hCI->CiSessions[hCI->wFP_Session].bTransportConnID, hCI->CiSessions[hCI->wFP_Session].wSessionID, p8Msg, wLenField);        
            free(p8Msg);
        }
        else{
            CI_LOG_ERR("[Finger](%s) Malloc memory error!!!\r\n", __FUNCTION__);
        }
    }
    else {
        CI_LOG_ERR("[Finger](%s) error,no session!!!\r\n", __FUNCTION__);
    }
}   

// -------------------------------------------------------------
static void _FP_ParseData(CI_HANDLE hCI, const BYTE *pData, DWORD dwLength)
{
    WORD wOffset = 0;
    WORD wLen = 0;
    static FP_Status eFP_Stt = FPST_FP_SUCCESS;

    FP_DEBUG(("==>[%s] Line:%d DataTypeID:0x%02X dwLength:%d.\r\n", __FUNCTION__, __LINE__, pData[wOffset], (int)dwLength));
    //wDataTypeLen = pData[wOffset+1]<<8 + pData[wOffset+2];

    switch (pData[wOffset]) //data type ID
    {
        case DTID_FP_AUTH:
        {
            eFP_Stt = FPST_AUTH_SUCCESS;
            //pDataType->st_Auth.wAuthProtID = (pData[wOffset+3]<<8) + pData[wOffset+4];
            wLen = (pData[wOffset+5]<<8) + pData[wOffset+6];

            WORD i;
            FP_DEBUG(("==>Auth data:"));
            for (i=0; i<dwLength; i++){
                FP_DEBUG(("0x%02X ", pData[i]));
                if (((i%10) == 0 && i != 0) || (i == dwLength-1))
                {
                    FP_DEBUG(("\r\n"));
                }
            }
        #if 1 //test
            BYTE u8SendBuf[12+5] = {0};
            
            u8SendBuf[0] = CMD_AUTHENTICATION_RESPONSE; //command ID
            memcpy(&u8SendBuf[1], &(stFpInfor.wCaSystemID), 2);
            memcpy(&u8SendBuf[3], &(stFpInfor.dwTransID), 4);

            u8SendBuf[7] = 0x02; //two data types:auth and status.
            memcpy(&u8SendBuf[8], &stDate, 5);  // dtid_fingerprint_auth content
            u8SendBuf[5+8] = DTID_FP_STATUS;  // dtid_fingerprint_status
            u8SendBuf[5+9] = 0x00;          // status length high byte
            u8SendBuf[5+10] = 0x01;         // status length low byte
            u8SendBuf[5+11] = eFP_Stt;      // status content
            _FP_SendMessage(hCI, u8SendBuf, 5+12);
        #else
            BYTE *pSendBuf = (BYTE*)malloc(8+dwLength+4); //Header:8 bytes; Auth:dwLength bytes; status:4 bytes.
            if (pSendBuf != NULL){
                pSendBuf[0] = CMD_AUTHENTICATION_RESPONSE; //command ID
                memcpy(pSendBuf+1, &(stFpInfor.wCaSystemID), 2);
                memcpy(pSendBuf+3, &(stFpInfor.dwTransID), 4);

                pSendBuf[7] = 0x02; //two data types:auth and status.
                memcpy(pSendBuf+8, pData, dwLength);    // dtid_fingerprint_auth content
                pSendBuf[dwLength+8] = DTID_FP_STATUS;  // dtid_fingerprint_status
                pSendBuf[dwLength+9] = 0x00;            // status length high byte
                pSendBuf[dwLength+10] = 0x01;           // status length low byte
                pSendBuf[dwLength+11] = eFP_Stt;        // status content
                _FP_SendMessage(hCI, pSendBuf, dwLength+12);
                free(pSendBuf);
            }
            else{
                FP_DEBUG(("==>[%s] Line:%d,Malloc memory error!!!\r\n", __FUNCTION__, __LINE__));
            }
        #endif  

            break;
        }
        case DTID_FP_SERVICE_ID:
        {
        #if 1
            WORD i;
            stFpInfor.Msg.wServiceNum = (pData[wOffset+3]<<8) + pData[wOffset+4];
            for (i=0; i < stFpInfor.Msg.wServiceNum; i++) {
                stFpInfor.Msg.wServiceID[i] = (pData[wOffset+5+i*2]<<8) + pData[wOffset+6+i*2];
                FP_DEBUG(("stFpInfor.Msg.wServiceID[%d]:0x%04X.\r\n", (int)i, stFpInfor.Msg.wServiceID[i]));
            }
        #endif
            FP_DEBUG(("DTID_FP_SERVICE_ID wServiceNum:%d.\r\n", stFpInfor.Msg.wServiceNum));
            break;
        }
        case DTID_FP_STRING:
        {
            eFP_Stt = FPST_AUTH_SUCCESS;
            wLen = pData[wOffset+3];

            FP_DEBUG(("DTID_FP_STRING wLen:%d\r\n", (int)wLen));
            if (wLen > FP_STRING_MAX_LENGTH){
                CI_LOG_ERR("[Finger](%s) String wLen:%d is over flow!\r\n", __FUNCTION__, (int)wLen);
                eFP_Stt = FPST_FP_LENGTH_ERROR;
            }
            else if (wLen == 0){
                CI_LOG_WARR("[Finger](%s) No Finger data!\r\n", __FUNCTION__);
                eFP_Stt = FPST_NO_FP;
            }
            else{
                stFpInfor.Msg.pStr = (BYTE*)malloc(wLen);
                if (stFpInfor.Msg.pStr != NULL){
                    memset(stFpInfor.Msg.pStr, 0, wLen);
                    memcpy(stFpInfor.Msg.pStr, &pData[wOffset+4], wLen);
                    stFpInfor.Msg.pStr[wLen] = '\0';
                    stFpInfor.Msg.u8StrLen = wLen + 1;
                    CI_LOG_DEBG("[Finger](%s) Finger data:[%s].\r\n", __FUNCTION__, stFpInfor.Msg.pStr);
                    eFP_Stt = FPST_FP_SUCCESS;
                }
                else{
                    CI_LOG_WARR("[Finger](%s) Line:%d,Malloc memory error!\r\n", __FUNCTION__, __LINE__);
                    stFpInfor.Msg.u8StrLen = 0;
                    eFP_Stt = FPST_FP_LENGTH_ERROR;
                }
            }

            break;
        }
        case DTID_FP_LOGO:
        {
            WORD wLogoID = (pData[wOffset+3]<<8) | pData[wOffset+4];
            WORD wLogoLen = (pData[wOffset+5]<<8) | pData[wOffset+6];
            
            stFpInfor.Msg.pLogo = (BYTE*)malloc(wLogoLen);
            if (stFpInfor.Msg.pLogo != NULL){
                memset(stFpInfor.Msg.pLogo, 0, wLogoLen);
                memcpy(stFpInfor.Msg.pLogo, &pData[wOffset+7], wLogoLen);
                stFpInfor.Msg.wLogoLen = wLogoLen;
                eFP_Stt = FPST_FP_SUCCESS;
            #if 0   
                WORD i;
                FP_DEBUG(("==>Logo data:"));
                for (i=0; i<wLogoLen; i++){
                    if ((i%0x10) == 0)
                    {
                        FP_DEBUG(("\r\n"));
                    }
                    FP_DEBUG(("0x%02X,", stFpInfor.Msg.pLogo[i]));
                }
                FP_DEBUG(("\r\n"));
            #endif  
            }
            else{
                CI_LOG_WARR("[Finger](%s) Line:%d,Malloc memory error!\r\n", __FUNCTION__, __LINE__);
                stFpInfor.Msg.wLogoLen = 0;
                eFP_Stt = FPST_FP_LENGTH_ERROR;
            }
            CI_LOG_DEBG("[Finger](%s) wLogoID:0x%04X; wLogoLen:%d.\r\n", __FUNCTION__, wLogoID, (int)wLogoLen);
            break;
        }
        case DTID_FP_PIXEL:
        {
            stFpInfor.Msg.Rect.height = pData[wOffset+3];
            FP_DEBUG(("DTID_FP_PIXEL length:%d.\r\n", stFpInfor.Msg.Rect.height));
            if (stFpInfor.Msg.Rect.height > 128) //ST7162 support 128 pixels max only.
            {
                stFpInfor.Msg.Rect.height = 128;
            }
            break;
        }
        case DTID_FP_START_TIME:
        {
            WORD year  = 0;
            BYTE month = 0;
            BYTE day   = 0;
            WORD wMJD  = 0;
            DWORD dwUTC = 0, dwCurTime = 0;
            BYTE hour, minute, second;

            U8 i;
            for (i=0; i<5; i++){
                FP_DEBUG(("st_Stime[%d]:0x%02X.\r\n", i, pData[wOffset+3+i]));
            }
            
            wMJD = (pData[wOffset+3]<<8) | pData[wOffset+4];
            dwUTC = (pData[wOffset+5]<<16) | (pData[wOffset+6]<<8) | pData[wOffset+7];

            //Get current date:year,month and day.
            if (wMJD == 0xFFFF){ 
            //  dwCurTime = OSTimeGet()/OS_TICKS_PER_SEC + GetTimeFix();
                dwCurTime = OSTimeGet()/OS_TICKS_PER_SEC;
                FP_DEBUG(("dwCurTime:0x%08X.\r\n", dwCurTime));
                _SecondToDate(dwCurTime, &year, &month, &day, &hour, &minute, &second);
            }
            else {
                _MjdToYMD(wMJD, &year, &month, &day); 
            }
            
            if (dwUTC != 0xFFFFFF){
                hour = pData[wOffset+5];
                minute = pData[wOffset+6];
                second = pData[wOffset+7];
            }
            
            stFpInfor.Msg.dwStartTime = _DateToSecond(year, month, day, hour, minute, second); 
            FP_DEBUG(("dwCurTime 2:0x%08X.\r\n", dwCurTime));
            FP_DEBUG(("stFpInfor.Msg.dwStartTime:0x%08X.\r\n", stFpInfor.Msg.dwStartTime));
            break;
        }
        case DTID_FP_DURATION:
        {
            //duration //32 bits
            stFpInfor.Msg.dwDuration = (pData[wOffset+3]<<24) | (pData[wOffset+4]<<16) | (pData[wOffset+5]<<8) | pData[wOffset+6];
            FP_DEBUG(("duration data:0x%08X.\r\n", stFpInfor.Msg.dwDuration));
            break;
        }
        case DTID_FP_LOCATION:
        {
            U8 u8Lc_form = pData[wOffset+3];

            FP_DEBUG(("DTID_FP_LOCATION length:%d.\r\n", (int)dwLength));
            if (u8Lc_form == 0x01){
                stFpInfor.Msg.Rect.left   = (pData[wOffset+4] << 8) + pData[wOffset+5]; //position_X
                stFpInfor.Msg.Rect.top    = (pData[wOffset+6] << 8) + pData[wOffset+7]; //position_Y
            }
            else if (u8Lc_form == 0x02){
                stFpInfor.Msg.Rect.left   = pData[wOffset+4] | 0x80000000;  //Quadrant information
            
            CI_LOG_DEBG("[Finger](%s) Position left:%d top:%d right:%d bottom:%d.\r\n", __FUNCTION__,\
                stFpInfor.Msg.Rect.left, stFpInfor.Msg.Rect.top, stFpInfor.Msg.Rect.width, stFpInfor.Msg.Rect.height);
            BYTE u8SendBuf[12] = {0};       //Header:8 bytes; status:4 bytes.
            u8SendBuf[0] = CMD_FINGERPRINT_RESPONSE; //command ID
            memcpy(&u8SendBuf[1], &(stFpInfor.wCaSystemID), 2);
            memcpy(&u8SendBuf[3], &(stFpInfor.dwTransID), 4);

            u8SendBuf[7] = 0x01;            // one data type.
            u8SendBuf[8] = DTID_FP_STATUS;  // dtid_fingerprint_status
            u8SendBuf[9] = 0x00;            // status length high byte
            u8SendBuf[10] = 0x01;           // status length low byte
            u8SendBuf[11] = eFP_Stt; // status content
            _FP_SendMessage(hCI, u8SendBuf, 12);

            //FP_DEBUG(("Send message length:%d.\r\n", sizeof(FP_Msg)));
            break;
        }
        case DTID_FP_STATUS:
        {
            eFP_Stt = pData[wOffset+3];
            FP_DEBUG(("DTID_FP_STATUS status:%d.\r\n", eFP_Stt));
            break;
        }
        
        default:
            break;
    }
}

// Application Info
BOOL FingerPrint_Connect(CI_HANDLE hCI, WORD hSession)
{
    FP_DEBUG(("[%s] Line:%d.\r\n", __FUNCTION__, __LINE__));
    hCI->wFP_Session = hSession;
    Send_SPDU_Data(hCI,hCI->CiSessions[hSession].bTransportConnID,hCI->CiSessions[hSession].wSessionID,FP_ENQ,sizeof(FP_ENQ));

    return TRUE;
}

// -------------------------------------------------------------
void FingerPrint_Data(CI_HANDLE hCI, WORD hSession, const BYTE *pData, DWORD dwLen)
{
    WORD wLen = 0;
    DWORD dwTag;
    int nOffset = 3;
    nOffset += CI_DecodeLengthField(&pData[nOffset],&wLen);

    dwTag = (pData[0]<<16) | (pData[1]<<8) | pData[2];
    FP_DEBUG(("[%s] dwTag:0x%08X nOffset:%d.\r\n", __FUNCTION__, (int)dwTag, nOffset));

    switch (dwTag)
    {
        case TAG_SAS_ASYNC_MSG:
        {
            BYTE i, u8MsgCnt = pData[nOffset];
            WORD wMsgLen = (pData[nOffset+1]<<8) + pData[nOffset+2];

            if (stFpInfor.Msg.pStr != NULL){
                free(stFpInfor.Msg.pStr);
                stFpInfor.Msg.pStr = NULL;
            }
            if (stFpInfor.Msg.pLogo != NULL){
                free(stFpInfor.Msg.pLogo);
                stFpInfor.Msg.pLogo = NULL;
            }
            memset(&stFpInfor, 0, sizeof(FP_Infor));
            FP_DEBUG(("Message_nb:0x%02X.\r\n", u8MsgCnt));
            FP_DEBUG(("Message_length:0x%04X.\r\n", wMsgLen));
            
            stFpInfor.u8CmdID = pData[nOffset+3];
            stFpInfor.wCaSystemID = (pData[nOffset+4]<<8) + pData[nOffset+5];
            stFpInfor.dwTransID = (pData[nOffset+6]<<24) + (pData[nOffset+7]<<16) + (pData[nOffset+8]<<8) + pData[nOffset+9];
            stFpInfor.u8DataTypeNum = pData[nOffset+10];
            
            if (stFpInfor.u8DataTypeNum > FP_DAT_TYPE_MAX_NUM){
                CI_LOG_ERR("[Finger](%s) u8DataNB:%d over flow!\r\n", __FUNCTION__, stFpInfor.u8DataTypeNum);
                return;
            }

            nOffset += 11;
            WORD wDataLen = 0;
            for (i=0; i<stFpInfor.u8DataTypeNum; i++)
            {
                wDataLen = (pData[nOffset+1]<<8) + pData[nOffset+2] + 3;
                _FP_ParseData(hCI, &(pData[nOffset]), wDataLen);
                nOffset += wDataLen;
            }
            FP_DEBUG(("==>Line:[%d] stFpInfor.u8CmdID:0x%02X.\r\n", __LINE__, stFpInfor.u8CmdID));
            if (stFpInfor.u8CmdID == CMD_FINGERPRINT_SEND){
                stFpInfor.Msg.bNewStr = TRUE;
                FP_DEBUG(("==>Line:[%d] stFpInfor.Msg.bNewStr:%d.\r\n", __LINE__, stFpInfor.Msg.bNewStr));
            }

            break;
        }

        default:
        {
            CI_LOG_WARR("[Finger](%s) Unknown tag:0x%08X!\r\n", __FUNCTION__, (int)dwTag);
            break;
        }
    }
}

// -------------------------------------------------------------
void FingerPrint_Close(CI_HANDLE hCI,WORD hSession)
{
    FP_DEBUG(("[%s] Line:%d.\r\n", __FUNCTION__, __LINE__));
    hCI->wFP_Session = 0;
}

void WVCI_SetMJD_UTC(BYTE *pDate, BYTE u8Len)
{
    if (pDate == NULL || u8Len > 5){
        return ;
    }

    memcpy(&stDate, pDate, u8Len);
}

void WVCI_ShowFingerPrint(BOOL bShow)
{
    WORD wMsgLen = 0;
    WORD wOffset = 0;

    if (bShow){
        wMsgLen = 2 + sizeof(RECT) + 1 + stFpInfor.Msg.u8StrLen + 2 + stFpInfor.Msg.wLogoLen;
        
        BYTE *pBuffer = (BYTE*)malloc(wMsgLen);
        if (pBuffer != NULL){
            FP_DEBUG(("[%s] start to show finger...\r\n", __FUNCTION__));
            CI_LOG_INF("[Finger](%s) Finger Msg Len:[%d].\r\n", __FUNCTION__, (int)wMsgLen);
            pBuffer[0] = (BYTE)((wMsgLen >> 8) & 0x00FF);
            pBuffer[1] = (BYTE)((wMsgLen >> 0) & 0x00FF);

            //RECT data
            wOffset = 2;
            memcpy(pBuffer+wOffset, &stFpInfor.Msg.Rect, sizeof(RECT));

            //String data
            wOffset += sizeof(RECT);
            pBuffer[wOffset++] = stFpInfor.Msg.u8StrLen;
            if (stFpInfor.Msg.pStr != NULL){
                memcpy(pBuffer+wOffset, stFpInfor.Msg.pStr, stFpInfor.Msg.u8StrLen);
                wOffset += stFpInfor.Msg.u8StrLen;
            }
            
            //Logo data
            pBuffer[wOffset++] = (BYTE)((stFpInfor.Msg.wLogoLen >> 8) & 0x00FF);
            pBuffer[wOffset++] = (BYTE)((stFpInfor.Msg.wLogoLen >> 0) & 0x00FF);
            if (stFpInfor.Msg.pLogo != NULL){
                memcpy(pBuffer+wOffset, stFpInfor.Msg.pLogo, stFpInfor.Msg.wLogoLen);
                wOffset += stFpInfor.Msg.wLogoLen;
            }
    #if 0
            {
                WORD i;
                FP_DEBUG(("==>pBuffer data:"));
                for (i=0; i<wMsgLen; i++){
                    if ((i%0x10) == 0)
                    {
                        FP_DEBUG(("\r\n"));
                    }
                    FP_DEBUG(("0x%02X,", pBuffer[i]));
                }
                FP_DEBUG(("\r\n"));
            }
    #endif  
            // send to decoder
            Ts_SendFinger(pBuffer, wMsgLen);
        #if defined(FP_SEND2TC_ENABLE) //send to transcoder
            COMM_DownSoftware(COMM_MAINCMD_FINGER, SLOT1, pBuffer, wMsgLen);            
        #endif  
            free(pBuffer);
        }
    }
    else{
        CI_LOG_INF("[Finger](%s) start to hide finger.\r\n", __FUNCTION__);

        BYTE u8Buffer[4] = {0x00, 0x04};
        // send to decoder
        Ts_SendFinger(u8Buffer, sizeof(u8Buffer));
    #if defined(FP_SEND2TC_ENABLE) //send to transcoder
        COMM_DownSoftware(COMM_MAINCMD_FINGER, SLOT1, u8Buffer, sizeof(u8Buffer));          
    #endif
    }
}

#if 0 //test function
static BYTE u8FingerMsg[] = {
#if 0
    0x00, 0x25, 0x13, 0x01, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x10, 0x30, 
    0x31, 0x39, 0x20, 0x30, 0x30, 0x30, 0x30, 0x20, 0x31, 0x30, 
    0x30, 0x30, 0x20, 0x39, 0x00, 0x00, 0x00
#else
    0x03, 0xE5, 0x13, 0x01, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00, 
    0x03, 0xD0,
    0x42, 0x4D, 0xD0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2E, 0x01, 0x00, 0x00, 0x28, 0x00, 
    0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x15, 0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0xA2, 0x02, 0x00, 0x00, 0x12, 0x0B, 0x00, 0x00, 0x12, 0x0B, 0x00, 0x00, 0x3E, 0x00, 
    0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 
    0x01, 0x00, 0x02, 0x07, 0x07, 0x00, 0x04, 0x0D, 0x0D, 0x00, 0x04, 0x0C, 0x0C, 0x00, 0x01, 0x03, 
    0x03, 0x00, 0x0E, 0x2E, 0x2F, 0x00, 0x0B, 0x24, 0x25, 0x00, 0x18, 0x4D, 0x4F, 0x00, 0x17, 0x4A, 
    0x4C, 0x00, 0x0E, 0x2D, 0x2E, 0x00, 0x0A, 0x22, 0x23, 0x00, 0x49, 0xEE, 0xF6, 0x00, 0x48, 0xEB, 
    0xF3, 0x00, 0x42, 0xD8, 0xDF, 0x00, 0x41, 0xD4, 0xDB, 0x00, 0x40, 0xD1, 0xD8, 0x00, 0x3D, 0xC8, 
    0xCE, 0x00, 0x3A, 0xBE, 0xC4, 0x00, 0x30, 0x9D, 0xA2, 0x00, 0x2E, 0x97, 0x9C, 0x00, 0x2A, 0x8A, 
    0x8E, 0x00, 0x23, 0x72, 0x76, 0x00, 0x22, 0x6F, 0x73, 0x00, 0x18, 0x4E, 0x51, 0x00, 0x0F, 0x31, 
    0x33, 0x00, 0x0A, 0x21, 0x22, 0x00, 0x08, 0x1A, 0x1B, 0x00, 0x4C, 0xF7, 0xFF, 0x00, 0x4C, 0xF6, 
    0xFE, 0x00, 0x4B, 0xF2, 0xFA, 0x00, 0x4A, 0xF1, 0xF9, 0x00, 0x4A, 0xEF, 0xF7, 0x00, 0x49, 0xED, 
    0xF5, 0x00, 0x48, 0xEA, 0xF2, 0x00, 0x48, 0xE9, 0xF1, 0x00, 0x48, 0xE8, 0xF0, 0x00, 0x46, 0xE3, 
    0xEA, 0x00, 0x45, 0xE0, 0xE7, 0x00, 0x44, 0xDD, 0xE4, 0x00, 0x42, 0xD7, 0xDE, 0x00, 0x40, 0xD0, 
    0xD7, 0x00, 0x40, 0xCF, 0xD6, 0x00, 0x3F, 0xCD, 0xD4, 0x00, 0x3B, 0xC0, 0xC6, 0x00, 0x3A, 0xBC, 
    0xC2, 0x00, 0x3A, 0xBB, 0xC1, 0x00, 0x37, 0xB2, 0xB8, 0x00, 0x37, 0xB1, 0xB7, 0x00, 0x35, 0xAC, 
    0xB2, 0x00, 0x35, 0xAB, 0xB1, 0x00, 0x2F, 0x99, 0x9E, 0x00, 0x26, 0x7B, 0x7F, 0x00, 0x1F, 0x65, 
    0x68, 0x00, 0x1D, 0x5D, 0x60, 0x00, 0x15, 0x43, 0x45, 0x00, 0x12, 0x39, 0x3B, 0x00, 0x08, 0x19, 
    0x1A, 0x00, 0x05, 0x10, 0x11, 0x00, 0x06, 0x12, 0x13, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x02, 0x19, 
    0x2F, 0x25, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 
    0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x25, 0x2F, 0x19, 0x02, 0x00, 0x00, 0x09, 0x20, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x20, 0x09, 0x00, 0x00, 0x2E, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x2E, 0x00, 0x00, 0x25, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x25, 0x00, 0x00, 0x0D, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x0D, 0x00, 0x00, 0x22, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x22, 0x00, 0x00, 0x22, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x22, 0x00, 0x00, 0x22, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x23, 0x12, 0x12, 0x23, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x22, 0x00, 0x00, 0x22, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1E, 0x34, 0x1B, 0x06, 0x06, 0x1B, 0x34, 
    0x1E, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x22, 0x00, 0x00, 0x22, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1F, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x17, 0x1F, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x22, 0x00, 0x00, 0x22, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x21, 0x37, 0x00, 0x00, 0x03, 0x1C, 0x1C, 0x03, 0x00, 
    0x00, 0x37, 0x21, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x22, 0x00, 0x00, 0x22, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x27, 0x0A, 0x00, 0x00, 0x38, 0x33, 0x26, 0x26, 0x33, 0x38, 
    0x00, 0x00, 0x0A, 0x27, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x22, 0x00, 0x00, 0x22, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x29, 0x39, 0x00, 0x00, 0x36, 0x0E, 0x1D, 0x1D, 0x1D, 0x1D, 0x0E, 
    0x36, 0x00, 0x00, 0x39, 0x29, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x22, 0x00, 0x00, 0x22, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x2C, 0x0B, 0x00, 0x02, 0x35, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1D, 0x35, 0x02, 0x00, 0x0B, 0x2C, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x22, 0x00, 0x00, 0x0D, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x13, 0x0C, 0x00, 0x03, 0x16, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1D, 0x1D, 0x16, 0x03, 0x00, 0x0C, 0x13, 0x1D, 0x1D, 0x1D, 0x1D, 0x0D, 0x00, 0x00, 0x23, 0x1D, 
    0x1D, 0x1D, 0x32, 0x3C, 0x00, 0x05, 0x14, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x14, 0x05, 0x00, 0x3C, 0x32, 0x1D, 0x1D, 0x1D, 0x23, 0x00, 0x00, 0x10, 0x1D, 
    0x1D, 0x15, 0x04, 0x00, 0x3A, 0x31, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x30, 0x3A, 0x00, 0x04, 0x15, 0x1D, 0x1D, 0x10, 0x00, 0x00, 0x18, 0x11, 
    0x17, 0x03, 0x00, 0x08, 0x2D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x2D, 0x08, 0x00, 0x03, 0x17, 0x11, 0x18, 0x00, 0x00, 0x02, 0x3B, 
    0x00, 0x00, 0x07, 0x2B, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x2A, 0x07, 0x00, 0x00, 0x3B, 0x02, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x1A, 0x28, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 
    0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x1D, 0x28, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x09, 0x0F, 0x24, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 
    0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x24, 0x0F, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
};

static BOOL _TestFingerFunc(void)
{
    static int nCnt = 0;
    static BOOL bShow = FALSE;

    nCnt ++;
    if ((++nCnt % 3) == 0){
        if (!bShow){
            bShow = TRUE;
            FP_DEBUG(("[%s] start to show finger...\r\n", __FUNCTION__));
            Ts_SendFinger(u8FingerMsg, sizeof(u8FingerMsg));
        #if defined(FP_SEND2TC_ENABLE) //send to transcoder
            COMM_DownSoftware(COMM_MAINCMD_FINGER, SLOT1, u8FingerMsg, sizeof(u8FingerMsg));            
        #endif  
        }
        else {
            bShow = FALSE;
            FP_DEBUG(("[%s] start to hide finger...\r\n", __FUNCTION__));

            BYTE u8Buffer[4] = {0x00, 0x04};
            Ts_SendFinger(u8Buffer, sizeof(u8Buffer));
        #if defined(FP_SEND2TC_ENABLE) //send to transcoder
            COMM_DownSoftware(COMM_MAINCMD_FINGER, SLOT1, u8Buffer, sizeof(u8Buffer));          
        #endif
        }
    }

    return TRUE;
}
#endif

void WVCI_CheckFingerPrint(void)
{
    DWORD dwCurTime = OSTimeGet()/OS_TICKS_PER_SEC + GetCurrentTime();
//  DWORD dwCurTime = OSTimeGet()/OS_TICKS_PER_SEC;
    DECO_Main_Params_t *hDecoMainParams = NULL; 
    static WORD wCurServiceID = 0;

#if 0 //test
    return _TestFingerFunc();
#endif

    hDecoMainParams = MBOARD_GetDecoderParamsHandle();
    if(hDecoMainParams == NULL)
    {
        FP_DEBUG(("[%s] GET DECODE MAIN PARAMS FAILURE !!!!\r\n", __FUNCTION__));
        return ;
    }

    if (wCurServiceID != hDecoMainParams->ProgramConfig[0].PlayingService.wServiceId){
        wCurServiceID = hDecoMainParams->ProgramConfig[0].PlayingService.wServiceId;
        stFpInfor.Msg.bNewStr = TRUE;
    }
    
    //FP_DEBUG(("dwCurTime:0x%08X.\r\n", dwCurTime));
    //Terminate time is arrived.
    if (dwCurTime >= stFpInfor.Msg.dwStartTime + (stFpInfor.Msg.dwDuration/100)){
        //Close FP window if the flag of display is TRUE.
        if (stFpInfor.Msg.bShowOn){
            stFpInfor.Msg.bShowOn = FALSE;
            WVCI_ShowFingerPrint(stFpInfor.Msg.bShowOn);
        }
    }
    //Start time is arrived.
    else if (dwCurTime >= stFpInfor.Msg.dwStartTime)
    {
        WORD wIdx = 0;
        //FP_DEBUG(("stFpInfor.Msg.wServiceNum:%d.\r\n", stFpInfor.Msg.wServiceNum));
        for (wIdx=0; wIdx<stFpInfor.Msg.wServiceNum; wIdx++)
        {
            if (stFpInfor.Msg.wServiceID[wIdx] == wCurServiceID || 
                (stFpInfor.Msg.wServiceID[wIdx] == 0xFFFF && stFpInfor.Msg.wServiceNum == 0x01))//all services should be showed FP
            {
                //check if new FP or not?
                if (stFpInfor.Msg.bNewStr)
                {
                    CI_LOG_INF("[Finger](%s) Show FP CurServiceID:0x%04X.\r\n", __FUNCTION__, wCurServiceID);
                    stFpInfor.Msg.bNewStr = FALSE;
                    stFpInfor.Msg.bShowOn = TRUE;
                    WVCI_ShowFingerPrint(stFpInfor.Msg.bShowOn);
                }
                break;
            }
        }

        //No match service ID;Close it if the flag of display is TRUE.
        if (wIdx >= stFpInfor.Msg.wServiceNum){
            CI_LOG_WARR("[Finger](%s) No match Service ID!\r\n", __FUNCTION__);
            if (stFpInfor.Msg.bShowOn){
                stFpInfor.Msg.bShowOn = FALSE;
                WVCI_ShowFingerPrint(stFpInfor.Msg.bShowOn);
            }
        }
    }
}

#endif // SMART_CARD_ENABLE


