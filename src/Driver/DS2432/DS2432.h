/**********************************************************************
* Copyright (c) 2011,HuiZhou WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName ：ds2432.h
* Description ：
*
* Version ：1.0.0
* Author ：liwei TT  Reviewer :
* Date ：2011-02-13
* Record : Create
*
***********************************************************************/
#ifndef DS2432_H_
#define DS2432_H_

#include "appGlobal.h"

#define DS2432_MAX_DATA_LENGTH          (128)

/**********************************************************************
 * macro define slot number
 *********************************************************************/
#define SLOT0                               0
#define SLOT1                               1
#define SLOT2                               2
#define SLOT3                               3
#define SLOT4                               4
#define SLOT5                               5
#define SLOT_MAINBOARD                      6

#define DS2432_SUCCESS                      (0x00000000)

/* DS2432 模块错误码定义 */
#define ERROR_DS2432_START                  (0x00100000)
#define ERROR_DS2432_FAILURE                (ERROR_DS2432_START)
#define ERROR_DS2432_INIT_FAILURE           (ERROR_DS2432_START+1)
#define ERROR_DS2432_READROM_FAILURE        (ERROR_DS2432_START+2)
#define ERROR_DS2432_READMEM_FAILURE        (ERROR_DS2432_START+3)
#define ERROR_DS2432_LOADSEC_FAILURE        (ERROR_DS2432_START+4)
#define ERROR_DS2432_PROTECTSEC_FAILURE     (ERROR_DS2432_START+5)
#define ERROR_DS2432_READAUTH_FAILURE       (ERROR_DS2432_START+6)
#define ERROR_DS2432_WRITEAUTH_FAILURE      (ERROR_DS2432_START+7)
#define ERROR_DS2432_BAD_PARAMS             (ERROR_DS2432_START+8)
#define ERROR_DS2432_NO_INIT                (ERROR_DS2432_START+9)
#define ERROR_DS2432_CHECKSUM_FAILURE       (ERROR_DS2432_START+10)

typedef struct
{
    U8  u8Company[3];                // "WAV" 三个字母常量 -- 公司信息，同时有同步
    U8  u8StructVer;                 // 结构体版本 : 方便数据域更改或算法更新区别
    U8  u8DataCheckSum;              // 数据检验和，本字节之后所有有效数据的校验和
    U8  u8SubboardID;                // Subboard ID ，标志子板类型
    U8  u8HWVersion;                 // 硬件版本
    U8  u8TAGDataLen;                // TAG 元组数据长度
    U32 u32LastUpdateTime;           // 授权更新时间 : bit(31---16):year, bit(15---8):month, bit(7---0):day

    // TAG 域
    U8  u8TAGData[116];              // TAG 数据域
}DS2432_ChipInfo_t;

/* DS2432 data struct */
typedef struct
{
    U8                  u8SlotNumber;          //板号，如果是在主板上的DS2432板号为0xFE
    U8                  u8Reserved[3];
    U8                  u8ChipID[8];                         /* 8 bytes chip id */
    DS2432_ChipInfo_t   stDS2432_ChipInfo;
}DS2432_PerBoard_t;

typedef struct
{
    U32                 u32HadBoardNum;
    DS2432_PerBoard_t   stDS2432_PerBoard[7];
}DS2432_AllBoard_t;

//实际写到文件的信息
typedef struct
{
    U8                  u8ChipID[8];                         /* 8 bytes chip id */
    DS2432_ChipInfo_t   stDS2432_ChipInfo;
}DS2432_WriteToFile_t;


#define Delay_us(US)                            usleep(US)
#define Delay_ms(MS)                            usleep((MS)*1000)

#define MAX_TRY_TIMES   (3)

/****************************************************************************
 Function:           DS2432_SetOperateIndex

 Description:        根据slot号来获取到需要设置的MIO或EMIO的相关寄存器值

 Input:              u8SlotIndex:      卡槽号

 Output:             -
 Return:             -
 Note:               -

 Modify:             20140526 hongfu.liu

 ****************************************************************************/
void DS2432_SetOperateIndex(U8 u8Index);

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
 *          pu8CHIPID                   CHIPID指针
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
                        U8 *pu8CHIPID);

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
U32 DS2432_Check(U8 *pData);

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
U32 DS2432_GetData();

/*****************************************************************************
  Function:     DS2432_InitGlobalValue()
  Description:  初始化DS2432全局变量
  Input:
  Output:       -
  Return:       -

  Description ：copy from DMP 2014.02.18
*****************************************************************************/
void DS2432_InitGlobalValue();
/*****************************************************************************
  Function:     DS2432_UpdateMainboardLicense()
  Description:  初始化主板license
  Input:
  Output:       -
  Return:       -

  Description ：copy from DMP 2014.02.18
*****************************************************************************/
void DS2432_UpdateMainboardLicense(U8* pu8ChipId, U8* pu8Data);

/*****************************************************************************
  Function:     DS2432_UpdateSubLicenseData() - 更新子板license信息
  Description:  获取license信息，并保存到gDS2432_AllBoard变量中
  Input:        u8Slot  --- slot index
  Output:       -
  Return:       DS2432_SUCCESS      		--- success
                ERROR_DS2432_FAILURE        --- input parameter is wrong
  Description ：copy from DMP 2014.02.18
*****************************************************************************/
U32 DS2432_UpdateSubLicenseData(U8 u8Slot, U8* pu8ChipId, U8* pu8Data);

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
U32 DS2432_LoadData(U8 slotID,U8 *DS2432Data);

/****************************************************************************
 Function:           DS2432_GetAllBoardDS2432

 Description:        获取所有的2432信息

 Input:              -

 Output:             -
 Return:             -
 Note:               -

 Create by:          hongfu.liu 2014.5.26

 ****************************************************************************/
DS2432_AllBoard_t *DS2432_GetAllBoardDS2432(void);

#endif /*DS2432_H_*/

