/**********************************************************************
* Copyright (c) 2011, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName : ds2432.c
* Description ：
*
* Version ：1.0.0
* Author ：liwei TT  Reviewer :
* Date ：2011-02-13
* Record : Create
*
* Version ：1.1.0
* Author ：TT  Reviewer : Tidy the code
* Date ：2012-06-04
* Record : Create
*
***********************************************************************/
#include <unistd.h>
#include <string.h>
#include "time.h"
#include "WV_log.h"
#include "DS2432.h"
#include "WV_DS2432.h"

U8      g_DS2432_Index = 0;

unsigned char u8Code_DS2432[8] = {0xaa, 0xbb, 0x11, 0xA2, 0x34, 0x95, 0xCA, 0x22};


static DS2432_AllBoard_t S_gDS2432_AllBoard;

/****************************************************************************
 Function:           DS2432_GetAllBoardDS2432

 Description:        获取所有的2432信息

 Input:              -

 Output:             -
 Return:             -
 Note:               -

 Create by:          hongfu.liu 2014.5.26

 ****************************************************************************/
DS2432_AllBoard_t *DS2432_GetAllBoardDS2432(void)
{
    return &S_gDS2432_AllBoard;
}

/****************************************************************************
 Function:           DS2432_SetOperateIndex

 Description:        根据slot号来获取到需要设置的MIO或EMIO的相关寄存器值

 Input:              u8SlotIndex:      卡槽号

 Output:             -
 Return:             -
 Note:               -

 Modify:             20140526 hongfu.liu

 ****************************************************************************/
void DS2432_SetOperateIndex(U8 u8Index)
{
    g_DS2432_Index = u8Index;
}

/******************************************************************************
 * DS2432_LoadSECRET() - load secret
 * DESCRIPTION: -
 *     load secret into ds2432
 * Input  :
 *              pu8SECRET               密钥指针 (8Bytes)
 *              u32SECRETLen              密钥长度 目前8
 *              u32GPIOBase             1-wrie控制PIO基地址
 *              nProtect                特殊保护位，当为0x55时，表示写入密钥保护，写入后，2432不能再修改密钥
 * Output :
 * Returns:     DS2432_SUCCESS          操作成功
 *              ERROR_DS2432_FAILURE    操作失败
 *
 * modification history
 * --------------------
 * 11 Jun 2012, shouquan.tang written
 * --------------------
 ******************************************************************************/
static U32 DS2432_LoadSECRET(const U8 *pu8SECRET, U32 u32SECRETLen, U32 u32Protect)
{
    // VARS
    enmWVDS2432ErrorType     emErrType = ERR_OK;
    S32     s32PageIndex = 0;
    U8      aru8ChipID[8] = {0};
    U32     u32MaxRetrytime = MAX_TRY_TIMES;

    // INIT
    do
    {
        emErrType = WVDS2432Initialize(s32PageIndex, (U8 *)pu8SECRET, 8);
        u32MaxRetrytime --;
    } while ((u32MaxRetrytime > 0) && (emErrType != ERR_OK) );

    if (emErrType != ERR_OK)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DS2432, "[DS2432_LoadSECRET] WVDS2432Initialize() FAILURE(%d)!", emErrType);
        return ERROR_DS2432_INIT_FAILURE;
    }

    // GET CHIPID
    emErrType = WVDS2432ReadRom(aru8ChipID, 8);
    if (emErrType != ERR_OK)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_LoadSECRET] WVDS2432ReadRom() FAILURE(%d)!", emErrType);
        return ERROR_DS2432_READROM_FAILURE;
    }

    // LOAD SECRET
    emErrType = WVDS2432LoadSecret();
    if (emErrType != ERR_OK)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_LoadSECRET] WVDS2432LoadSecret() FAILURE(%d)!", emErrType);
        return ERROR_DS2432_LOADSEC_FAILURE;
    }

    // PROTECT
    if (u32Protect == 0x55)
    {
        emErrType = WVDS2432ProtectSecret();
        if (emErrType != ERR_OK)
        {
             LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_LoadSECRET] WVDS2432ProtectSecret() FAILURE(%d)!", emErrType);
            return ERROR_DS2432_PROTECTSEC_FAILURE;
        }
    }

    return DS2432_SUCCESS;
}

/******************************************************************************
 * DS2432_Verification() - verfication
 * DESCRIPTION: -
 *     verfication
 * Input  :
 *          pu8EEPROMData               EEPROM数据指针
 *          nReadEEPROMLen              读取EEPROM长度
 *          pu8SECRET                   密钥指针
 *          nPageIndex                  验证页面索引
 *          u32Random                   随机数
 *          paru8CHIPID                   CHIPID指针
 *          u32GPIOBase                 1-wrie控制PIO基地址
 * Output :
 * Returns:
 *          DS2432_SUCCESS              操作成功
 *          ERROR_DS2432_FAILURE        操作失败
 *          ERROR_DS2432_BAD_PARAMS     错误参数
 *
 * modification history
 * --------------------
 * 10 Jun 2012, shouquan.tang review
 * --------------------
 ******************************************************************************/
U32 DS2432_Verification(U8 *pu8EEPROMData,  S32 s32ReadEEPROMLen,
                        const U8 *pu8SECRET, S32 s32PageIndex,
                        U32 u32Random,
                        U8 *paru8CHIPID)
{
    // VARS
    U8      aru8Random[3] = {0};
    enmWVDS2432ErrorType     emErrType = ERR_OK;
    U32     u32MaxRetrytime = 0;

    // CHECK PARAMS
    if (pu8EEPROMData   == NULL     ||
        pu8SECRET       == NULL     ||
        paru8CHIPID       == NULL     ||
        s32ReadEEPROMLen > 128      ||
        s32ReadEEPROMLen < 0        ||
        s32PageIndex     < 0        ||
        s32PageIndex     > 3)
    {
         LOG_PRINTF(LOG_LEVEL_ERROR,LOG_LEVEL_ERROR, "[DS2432_Verification] BAD PARAMS!! ");
        return ERROR_DS2432_BAD_PARAMS;
    }

    usleep(10000);

    // RANDOM
    aru8Random[0] = u32Random >> 16 & 0xFF;
    aru8Random[1] = u32Random >>  8 & 0xFF;
    aru8Random[2] = u32Random >>  0 & 0xFF;

    // INIT
    u32MaxRetrytime = MAX_TRY_TIMES;
    do
    {
        emErrType = WVDS2432Initialize(s32PageIndex, (U8 *)pu8SECRET, 8);
        u32MaxRetrytime--;
    } while ((u32MaxRetrytime > 0) && (emErrType != ERR_OK) );

    if (emErrType != ERR_OK)
    {
         LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_Verification] WVDS2432Initialize() FAILURE(%d)!! ", emErrType);
        return ERROR_DS2432_INIT_FAILURE;
    }

    // GET CHIPID
    u32MaxRetrytime = MAX_TRY_TIMES;
    do
    {
        emErrType = WVDS2432ReadRom(paru8CHIPID, 8);
        u32MaxRetrytime--;
    } while ((u32MaxRetrytime > 0) && (emErrType != ERR_OK) );
    if (emErrType != ERR_OK)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_Verification] WVDS2432ReadRom() FAILURE(%d)!! ", emErrType);
        return ERROR_DS2432_READROM_FAILURE;
    }

    // READ MEM
    u32MaxRetrytime = MAX_TRY_TIMES;
    do
    {
        emErrType = WVDS2432ReadMemory(0x00, pu8EEPROMData, s32ReadEEPROMLen);
        u32MaxRetrytime--;
    } while ((u32MaxRetrytime > 0) && (emErrType != ERR_OK) );
    if (emErrType != ERR_OK)
    {
         LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_Verification] WVDS2432ReadMemory() FAILURE(%d)!! ", emErrType);
        return ERROR_DS2432_READMEM_FAILURE;
    }

    // VERIFICATION
    u32MaxRetrytime = MAX_TRY_TIMES;
    do
    {
        emErrType = WVDS2432ReadAuthPage(s32PageIndex, aru8Random, 3);
        u32MaxRetrytime--;
    } while ((u32MaxRetrytime > 0) && (emErrType != ERR_OK) );
    if (emErrType != ERR_OK)
    {
        // LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_Verification] WVDS2432ReadAuthPage() FAILURE(%d)!! ", nRet);
        return ERROR_DS2432_READAUTH_FAILURE;
    }

    return DS2432_SUCCESS;
}


/******************************************************************************
 * DS2432_WriteEEPROM() - write eeprom
 * DESCRIPTION: -
 *     write eeprom
 * Input  :
 *              pu8Data                 数据指针
 *              nWriteLen               写入长度
 *              pu32ActionWriteLen        实现写入长度指针
 *              u32GPIOBase             1-wire控制PIO基地址
 * Output :
 * Returns:
 *              ERR_OK                  操作成功
 *              ERROR_DS2432_FAILURE    操作失败
 *              ERROR_DS2432_NO_INIT    未初始化
 *              ERROR_DS2432_BAD_PARAMS 参数错误
 *
 * modification history
 * --------------------
 * 10 Jun 2012, shouquan.tang review
 * --------------------
 ******************************************************************************/
static U32 DS2432_WriteEEPROM( const U8  *pu8Data,
                        S32 s32WriteLen,
                        U32 *pu32ActionWriteLen)
{
    // VARS
    enmWVDS2432ErrorType     emErrType = ERR_OK;
    S32     s32PageIndex = 0;
    S32     s32CurrWriteLen = 0;
    S32     s32HadWrittenLen = 0;
    S32     s32MaxRetryTimes = MAX_TRY_TIMES;
    U32     u32Return = DS2432_SUCCESS;

    // CHECK PARAMS
    if (pu8Data == NULL  || pu32ActionWriteLen == NULL ||
        s32WriteLen < 0 || s32WriteLen > 128)
    {
         LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_WriteEEPROM] BAD PARAMS!! ");
        return ERROR_DS2432_BAD_PARAMS;
    }

    // WRITE IN PAGE
    do
    {
        s32CurrWriteLen = s32WriteLen - s32HadWrittenLen;
        if (s32CurrWriteLen > 32)
        {
            s32CurrWriteLen = 32;
        }

        // PageIndex
        s32PageIndex = s32HadWrittenLen / 32;
        emErrType = WVDS2432WriteAuthPage(s32PageIndex, (U8 *)&pu8Data[s32HadWrittenLen], s32CurrWriteLen);
        if (emErrType == ERR_OK)
        {
            s32HadWrittenLen += s32CurrWriteLen;
        }
        else
        {
             LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_WriteEEPROM] WVDS2432WriteAuthPage() FAILURE(%d)!! ", emErrType);
            s32MaxRetryTimes--;
            if (s32MaxRetryTimes < 0)
            {
                u32Return = ERROR_DS2432_WRITEAUTH_FAILURE;
                break;
            }
        }
    } while (s32HadWrittenLen < s32WriteLen);

    *pu32ActionWriteLen = s32HadWrittenLen;

    return u32Return;
}

/******************************************************************************
 * DS2432_Check() - 检查数据校验和
 * DESCRIPTION: -
 *     检查数据校验和
 * Input  :
 *          pData                       数据指针
 * Output :
 * Returns:
 *          DS2432_SUCCESS              操作成功
 *          ERROR_DS2432_BAD_PARAMS     错误参数
 *          ERROR_DS2432_CHECKSUM       检验和异常
 *
 *
 * modification history
 * --------------------
 * 11 Jun 2012, shouquan.tang written
 * --------------------
 ******************************************************************************/
U32 DS2432_Check(U8 *pData)
{
    U8  i=0;
    U32 u32Sum=0;
    DS2432_ChipInfo_t *pstDS2432Data = NULL;

    if(pData == NULL)
    {
         LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_Check]Input param wrong!! ");
        return ERROR_DS2432_BAD_PARAMS;
    }

    for(i=5;i<sizeof(DS2432_ChipInfo_t);i++)
    {
        u32Sum += *(pData+i);
    }

    pstDS2432Data = (DS2432_ChipInfo_t *)pData;
    if(pstDS2432Data->u8DataCheckSum != (u32Sum&0xff))
    {
         LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_Check]check sum wrong!! ");
        return ERROR_DS2432_CHECKSUM_FAILURE;
    }

    return DS2432_SUCCESS;
}

/******************************************************************************
 * DS2432_GetData() - 获取license信息
 * DESCRIPTION: -
 *     获取license信息，并保存到gDS2432_AllBoard变量中
 * Input  :
 * Output :
 * Returns:
 *
 * modification history
 * --------------------
 * 11 Jun 2012, shouquan.tang written
 * --------------------
 ******************************************************************************/
U32 DS2432_GetData()
{
    // VARS
    U32         u32ReturnValue = 0;
    U8          u8MaxRetrytime = MAX_TRY_TIMES;
    U8          u8HadBoardNum = 0;

    memset(&S_gDS2432_AllBoard,0x00,sizeof(DS2432_AllBoard_t));

    //get data of mainboard's DS2432
    u8HadBoardNum = 0;
    S_gDS2432_AllBoard.stDS2432_PerBoard[u8HadBoardNum].u8SlotNumber = SLOT_MAINBOARD;
    DS2432_SetOperateIndex(SLOT_MAINBOARD);
    do
    {
        u32ReturnValue = DS2432_Verification(   (U8 *)&S_gDS2432_AllBoard.stDS2432_PerBoard[u8HadBoardNum].stDS2432_ChipInfo,
                                                DS2432_MAX_DATA_LENGTH,
                                                u8Code_DS2432,
                                                0,
                                                23,
                                                (U8 *)S_gDS2432_AllBoard.stDS2432_PerBoard[u8HadBoardNum].u8ChipID);
        if(u32ReturnValue == DS2432_SUCCESS)
        {
            u32ReturnValue = DS2432_Check((U8 *)&S_gDS2432_AllBoard.stDS2432_PerBoard[u8HadBoardNum].stDS2432_ChipInfo);
            if(u32ReturnValue == DS2432_SUCCESS)
            {
                 LOG_PRINTF(LOG_LEVEL_ALARM,LOG_MODULE_DS2432, "[DS2432_GetData] Get baseboard's license success!! (%d)", u32ReturnValue);
            }
            else
            {
                 LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_GetData] Get baseboard's license failure!! (%d)", u32ReturnValue);
            }
        }
        u8MaxRetrytime--;
    }while((u8MaxRetrytime > 0) && (u32ReturnValue != DS2432_SUCCESS));
    u8HadBoardNum++;
    
    S_gDS2432_AllBoard.u32HadBoardNum = u8HadBoardNum;

    return DS2432_SUCCESS;
}

/*****************************************************************************
  Function:     DS2432_InitGlobalValue()
  Description:  初始化DS2432全局变量
  Input:
  Output:       -
  Return:       -

  Description ：copy from DMP 2014.02.18
*****************************************************************************/
void DS2432_InitGlobalValue()
{
    memset(&S_gDS2432_AllBoard, 0x00, sizeof(DS2432_AllBoard_t));
}

/*****************************************************************************
  Function:     DS2432_UpdateMainboardLicense()
  Description:  初始化主板license
  Input:
  Output:       -
  Return:       -

  Description ：copy from DMP 2014.02.18
*****************************************************************************/
void DS2432_UpdateMainboardLicense(U8* pu8ChipId, U8* pu8Data)
{
    if((NULL == pu8ChipId) || (NULL == pu8Data))
    {
        printf("[DS2432_UpdateMainboardLicense]Input params error!");
        return;
    }

    S_gDS2432_AllBoard.u32HadBoardNum = 1;
    S_gDS2432_AllBoard.stDS2432_PerBoard[0].u8SlotNumber = SLOT_MAINBOARD;

    memcpy(S_gDS2432_AllBoard.stDS2432_PerBoard[0].u8ChipID,pu8ChipId,
           sizeof(S_gDS2432_AllBoard.stDS2432_PerBoard[0].u8ChipID));

    memcpy((U8*)&S_gDS2432_AllBoard.stDS2432_PerBoard[0].stDS2432_ChipInfo,pu8Data,
           sizeof(DS2432_ChipInfo_t));
}

/*****************************************************************************
  Function:     DS2432_UpdateSubLicenseData() - 更新子板license信息
  Description:  获取license信息，并保存到gDS2432_AllBoard变量中
  Input:        u8Slot  --- slot index
  Output:       -
  Return:       DS2432_SUCCESS      		--- success
                ERROR_DS2432_FAILURE        --- input parameter is wrong
  Description ：copy from DMP 2014.02.18
*****************************************************************************/
U32 DS2432_UpdateSubLicenseData(U8 u8Slot, U8* pu8ChipId, U8* pu8Data)
{
    U8 i = 0;

    if((u8Slot > SLOT2) || (NULL == pu8ChipId) || (NULL == pu8Data))
    {
        return ERROR_DS2432_BAD_PARAMS;
    }

    if(S_gDS2432_AllBoard.u32HadBoardNum > 4)
    {
        return ERROR_DS2432_FAILURE;
    }

    for(i = 1;i < S_gDS2432_AllBoard.u32HadBoardNum;i++)
    {
        if(u8Slot == S_gDS2432_AllBoard.stDS2432_PerBoard[i].u8SlotNumber)
        {
            memcpy(S_gDS2432_AllBoard.stDS2432_PerBoard[i].u8ChipID,
                   pu8ChipId,
                   sizeof(S_gDS2432_AllBoard.stDS2432_PerBoard[i].u8ChipID)
                   );

            memcpy((U8*)&S_gDS2432_AllBoard.stDS2432_PerBoard[i].stDS2432_ChipInfo,
                   pu8Data,
                   sizeof(DS2432_ChipInfo_t)
                  );

            break;
        }
    }

    /*当前表中没有，添加进来*/
    if(i == S_gDS2432_AllBoard.u32HadBoardNum)
    {
        S_gDS2432_AllBoard.stDS2432_PerBoard[S_gDS2432_AllBoard.u32HadBoardNum].u8SlotNumber = u8Slot;
        memcpy(S_gDS2432_AllBoard.stDS2432_PerBoard[S_gDS2432_AllBoard.u32HadBoardNum].u8ChipID,
               pu8ChipId,
               sizeof(S_gDS2432_AllBoard.stDS2432_PerBoard[S_gDS2432_AllBoard.u32HadBoardNum].u8ChipID)
               );

        memcpy((U8*)&S_gDS2432_AllBoard.stDS2432_PerBoard[S_gDS2432_AllBoard.u32HadBoardNum].stDS2432_ChipInfo,
               pu8Data,
               sizeof(DS2432_ChipInfo_t)
               );

        S_gDS2432_AllBoard.u32HadBoardNum++;
    }

    return DS2432_SUCCESS;
}

/******************************************************************************
 * DS2432_LoadData() - 装载EEPROM信息
 * DESCRIPTION: -
 *     装载EEPROM信息，用于授权数据更新
 * Input  :
 *          slotID                  索引号
 *          DS2432Data              数据指针
 * Output :
 * Returns:
 *          DS2432_SUCCESS          操作成功
 *          ERROR_DS2432_FAILURE    操作失败
 *          ERROR_DS2432_BAD_PARAMS 参数错误
 * modification history
 * --------------------
 * 11 Jun 2012, shouquan.tang written
 * --------------------
 ******************************************************************************/
U32 DS2432_LoadData(U8 slotID,U8 *DS2432Data)
{
    U32     u32ReadLen = 128;
    U32     u32PageIndex = 0;
    U32     u32Random = 100;
    U32     u32ActionWriteLen = 0;
    U32     u32ReturnValue = DS2432_SUCCESS;
    U8      u8MaxRetrytime = MAX_TRY_TIMES;

    U8      aru8CHIPID[8];
    U8      aru8TempData[256];

    memset(aru8CHIPID,0x00,sizeof(aru8CHIPID));
    memset(aru8TempData,0x00,sizeof(aru8TempData));

    /*don't move this statement, set global value*/
    DS2432_SetOperateIndex(slotID);
    //load secret
    u32ReturnValue = DS2432_LoadSECRET(u8Code_DS2432,8,0);
    if(u32ReturnValue != DS2432_SUCCESS)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_LoadData]load secret is wrong!! ");
        return u32ReturnValue;
    }

    //verification
    memset(aru8CHIPID,0x00,sizeof(aru8CHIPID));
    u32ReturnValue = DS2432_Verification( aru8TempData,
                                u32ReadLen,
                                u8Code_DS2432,
                                u32PageIndex,
                                u32Random,
                                aru8CHIPID);
    if(u32ReturnValue != DS2432_SUCCESS)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_LoadData]verification is wrong!! ");
        return u32ReturnValue;
    }

    u32ReturnValue = DS2432_WriteEEPROM(DS2432Data,128,&u32ActionWriteLen);
    if(u32ReturnValue != DS2432_SUCCESS)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR,LOG_MODULE_DS2432, "[DS2432_LoadData]WriteEEPROM failed! ");
        return u32ReturnValue;
    }

    //DS2432_SetOperateIndex(slotID);
    do
    {
        u32ReturnValue = DS2432_Verification(aru8TempData,DS2432_MAX_DATA_LENGTH,
                                             u8Code_DS2432,0,23,aru8CHIPID);
        #if 0
        {
            int     nRet = 0;
            int     tt = 0;

            nRet = memcmp(DS2432Data, aru8TempData, 128);
            printf ("##debug## nRet: %d \r\n\r\n", nRet);
            printf (" --------------------------------------------------- \r\n");
            for (tt = 0; tt < 128; tt++)
            {
                printf ("%02X ", DS2432Data[tt]);
                if ((tt+1)%16 == 0) printf ("\r\n");
            }
            printf (" =================================================== \r\n");
            for (tt = 0; tt < 128; tt++)
            {
                printf ("%02X ", aru8TempData[tt]);
                if ((tt+1)%16 == 0) printf ("\r\n");
            }
            printf ("\r\n");
        }
        #endif
        if(u32ReturnValue == DS2432_SUCCESS)
        {
            u32ReturnValue = DS2432_Check(aru8TempData);
            if(u32ReturnValue != DS2432_SUCCESS)
            {
                (void)DS2432_WriteEEPROM(DS2432Data,128,&u32ActionWriteLen);
            }
        }
        u8MaxRetrytime--;
    }while((u8MaxRetrytime > 0) && (u32ReturnValue != DS2432_SUCCESS));

    if (u32ReturnValue == DS2432_SUCCESS)
    {
         LOG_PRINTF(LOG_LEVEL_ALARM, LOG_MODULE_DS2432, "[DS2432_LoadData]load data success !!");
    }
    else
    {
         LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DS2432, "[DS2432_LoadData]load data failure !!(%x)", u32ReturnValue);
    }

    return u32ReturnValue;
}


