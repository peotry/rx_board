/******************************************************************************
 * WV_CI.h - WV_CI
 * 
 * Copyright 2008-2013 HuiZhou WELLAV Technology Co.,Ltd.
 * 
 * DESCRIPTION: - 
 *      WV_CI
 * modification history
 * --------------------
 * 02 Jun 2012, shouquan.tang : 执行第一次代码自审操作
 * 10 Oct 2008, WELLAV-TT written
 * --------------------
 ******************************************************************************/


#ifndef __WV_CI_H_
#define __WV_CI_H_

#include "appGlobal.h"
#include "TSProcess.h"

/****************************************************************************
 * Macro
 ****************************************************************************/
#define WAV_CI_ERROR_BASE               0x00C0
#define WVCI_SUCCESS                    (0)
#define WVCI_FAILURE                    (-1)
#define WVCI_ERROR_PARAMS               (WAV_CI_ERROR_BASE+1)
#define WVCI_ERROR_CRC                  (WAV_CI_ERROR_BASE+2)
#define WVCI_ERROR_BUSY                 (WAV_CI_ERROR_BASE+3)
#define WVCI_ERROR_NO_INITED            (WAV_CI_ERROR_BASE+4)
#define WVCI_ERROR_NO_INSERT            (WAV_CI_ERROR_BASE+5)
#define WVCI_ERROR_TIMEOUT              (WAV_CI_ERROR_BASE+6)
#define WVCI_ERROR_MSG                  (WAV_CI_ERROR_BASE+7)
#define WVCI_ERROR_OVERFLOW             (WAV_CI_ERROR_BASE+8)
#define WVCI_ERROR_EXIT_MMI             (WAV_CI_ERROR_BASE+9)

#define WVCI_SLOT_NAME_SIZE             (20)

#define WVCI_MAX_SLOT_NUM               (4)

#define WVCI_ENABLE                     (1)
#define WVCI_DISABLE                    (0)
#define WVCI_VALID                      (1)
#define WVCI_INVALID                    (0)
#define WVCI_LOCK                       (1)
#define WVCI_UNLOCK                     (0)

#define WVCI_MENU_MAX_ITEMS             (16)
#define WVCI_MENU_MAX_CONTENT           (64)

#define CAM_EVENT_NONE                  (0)
#define CAM_EVENT_INSERTED              (1)
#define CAM_EVENT_REMOVED               (2)



// MMI control commands
#define WVCI_MMI_CTRL_ENTER_MENU                (0x00)
#define WVCI_MMI_CTRL_ESCAPE                    (0x01)
#define WVCI_MMI_CTRL_0_CANCELED_PREV_MENU      (0x02)
#define WVCI_MMI_CTRL_1                         (0x03)
#define WVCI_MMI_CTRL_2                         (0x04)
#define WVCI_MMI_CTRL_3                         (0x05)
#define WVCI_MMI_CTRL_4                         (0x06)
#define WVCI_MMI_CTRL_5                         (0x07)
#define WVCI_MMI_CTRL_6                         (0x08)
#define WVCI_MMI_CTRL_7                         (0x09)
#define WVCI_MMI_CTRL_8                         (0x0a)
#define WVCI_MMI_CTRL_9                         (0x0b)
//#define WVCI_MMI_CTRL_ENTER_OK                  (0x0c)

#define WVCI_MMI_SEND_PIN_CODE                  (0x20)

#define WVCI_MMI_CTRL_ARROW_LEFT                (0x22)
#define WVCI_MMI_CTRL_ARROW_RIGHT               (0x23)
#define WVCI_MMI_CTRL_ARROW_UP                  (0x24)
#define WVCI_MMI_CTRL_ARROW_DOWN                (0x25)
#define WVCI_MMI_CTRL_CANCEL_ACTION             (0x26)
#define WVCI_MMI_CTRL_DEFAULT_ACTION            (0x27)
#define WVCI_MMI_CLOSE                          (0x30)


#define MAX_PIN_NUM_LEN                         (256)
#define WAV_CI_MMI_MAX_LINES_NUM                (20)
#define WAV_CI_MMI_MAX_STRING_SIZE              (1024)
#define WAV_CI_MMI_ROOT_LEVEL                   (0x01)


#ifdef ENABLE_CI_PES_CHECKING
#define CI_PES_MAX_PID_NUM                      (4)

#define CI_PES_SLOT0_PID0                       (0xD1)
#define CI_PES_SLOT0_PID1                       (0xD2)
#define CI_PES_SLOT0_PID2                       (0xD3)
#define CI_PES_SLOT0_PID3                       (0xD4)
#define CI_PES_SLOT0_CLR_STATUS                 (0xD5)
#define CI_PES_SLOT0_STATUS                     (0xDB)

#define CI_PES_SLOT1_PID0                       (0xD6)
#define CI_PES_SLOT1_PID1                       (0xD7)
#define CI_PES_SLOT1_PID2                       (0xD8)
#define CI_PES_SLOT1_PID3                       (0xD9)
#define CI_PES_SLOT1_CLR_STATUS                 (0xDA)
#define CI_PES_SLOT1_STATUS                     (0xDC)
#endif

#define CI_PES_ERROR                        (0)
#define CI_INIT_ERROR                       (1)
#define CI_COMMUNICATION_ERROR              (2)

// ci ts bitrate mode   
#define CI_TS_BITRATE_48Mbps_18             (0)             // clk = 6.000MHz REG_VALUE(0xE0)=108/18=6M
#define CI_TS_BITRATE_56Mbps_16             (1)             // clk = 6.750MHz REG_VALUE(0xE0)=108/16
#define CI_TS_BITRATE_64Mbps_13             (2)             // clk = 8.300MHz REG_VALUE(0xE0)=108/13
#define CI_TS_BITRATE_72Mbps_12             (3)             // clk = 9.000MHz REG_VALUE(0xE0)=108/12
#define CI_TS_BITRATE_80Mbps_10             (4)             // clk = 10.80MHz REG_VALUE(0xE0)=108/10
#define CI_TS_BITRATE_100Mbps_9             (5)             // clk = 12.00MHz REG_VALUE(0xE0)=108/9
#define CI_TS_BITRATE_108Mbps_8             (6)             // clk = 13.50MHz REG_VALUE(0xE0)=108/8  //05.29号的逻辑版本不支持108M

#define MAX_PID_NUM_FOR_CI_MONITOR (8)
#define MAX_CASYSTEM_ID_NUM (32)

/****************************************************************************
 * TYPE
 ****************************************************************************/
typedef struct
{
    // status
    U8      m_u8SlotInserted[WVCI_MAX_SLOT_NUM];
    U8      m_ucSlotInited[WVCI_MAX_SLOT_NUM];    
    //BYTE    m_ucSlot0Name[WVCI_SLOT_NAME_SIZE];
    //BYTE    m_ucSlot0Manufacturer[WVCI_SLOT_NAME_SIZE];
    //BYTE    m_ucSlot1Name[WVCI_SLOT_NAME_SIZE];
    //BYTE    m_ucSlot1Manufacturer[WVCI_SLOT_NAME_SIZE];
    BYTE    m_ucSlotName[WVCI_MAX_SLOT_NUM][WVCI_SLOT_NAME_SIZE];
    BYTE    m_ucSlotManufacturer[WVCI_MAX_SLOT_NUM][WVCI_SLOT_NAME_SIZE];
    U16     m_u16SystemIDNum[WVCI_MAX_SLOT_NUM];
    U16     m_u16SystemIDList[WVCI_MAX_SLOT_NUM][32];    
} WVCI_Status_t; // strcut size is 216bytes


typedef struct
{
	int                         CISlotState;
	BOOL                        CAMCommStatusError;	
}WVAlarm_CI_Status_t;


typedef struct
{
    // STATUS
    BYTE ucSlotNum;                                 // SLOT NUM
	BOOL bMenuValid;                                //
	BOOL bMenuIsClose;                              // ROOT MENU ITEMS
	
    // INFO
    char cTitleText[WVCI_MENU_MAX_CONTENT];         // menu title text
    char cSubTitleText[WVCI_MENU_MAX_CONTENT];      // menu sub title text
    char cBottomText[WVCI_MENU_MAX_CONTENT];        // menu bottom text         
    char cContentText[WVCI_MENU_MAX_ITEMS][WVCI_MENU_MAX_CONTENT];
    int  nChoice_nb;                                // menu choice number when 0xff means null

} WVCI_MenuContent;



typedef struct
{
    U8      slot_id;
    U8      choice_nb;
    U8      message_type;
    U8      mmi_text_valid;
    U8      operate_ret;
    U8      mmi_level;
    U8      u8Blind_Answer;
    U8      u8Answer_Text_Length;
    U8      reserved[2];    
    U16     text_len; 
    U16     item_offset[WAV_CI_MMI_MAX_LINES_NUM];
    U8      text[WAV_CI_MMI_MAX_STRING_SIZE];    
} WAVCI_MMI_Info_t;


typedef enum
{    
    ONLY_SLOT_0 = 0,
    ONLY_SLOT_1,
    ONLY_SLOT_2,
    ONLY_SLOT_3,
    ALL_SLOTS
}WVCI_SLOTMODE_t;

typedef struct
{
    U16 u16Channel;
    U16 u16ServiceID;
    U16 u16PID;
    U16 u16NewPID;
    U8 u8PIDType;
    U8 u8DescrambleStatus;
    U8 aru8ServiceName[MAX_SDT_NAME_LENGTH];
}PID_INFO_4_CI_t;

typedef struct
{
    U8 u8CAMInsertFlag;
    U8 u8CAMInitFlag;
    U8 u8CASystemNum;
    U8 u8PIDNum;
    U8 aru8CAMName[WVCI_SLOT_NAME_SIZE];
    U8 aru8Manufacturer[WVCI_SLOT_NAME_SIZE];
    U16 aru16CASystemIDList[MAX_CASYSTEM_ID_NUM];
    PID_INFO_4_CI_t arstPIDInfoList[MAX_PID_NUM_FOR_CI_MONITOR];
    
}WAVCI_Status_4_web_t;

/****************************************************************************
 * VARS
 *****************************************************************************/
// extern 
extern WVCI_Status_t*               g_ptWVCIStatusInst;


//////////////////////////////////////////////////////////////////////////////
//
//  PROTECT FUNCTION : (本部分函数由CI模块内部关联模块调用,外部模块不需调用)
//
//////////////////////////////////////////////////////////////////////////////
int  WVCI_MessageCallback(int nSlotIndex, unsigned char *pData, int nDataLen);
int  WVCI_AutoUpdateModule(unsigned char ucSlotIndex);
void WVCI_ResetMenu(BYTE ucSlotIndex);
int  WVCI_IsModuleInserted(BYTE ucSlotIndex);
int  WVCI_IsModuleInited(BYTE ucSlotIndex);
int  WVCI_SetAutoSendPMTFlag(unsigned char ucSlotIndex);


/****************************************************************************
 *                                                                          *
 *                          -- WV_CI API Functions --                       *
 *                                                                          *
 ****************************************************************************/

#if defined(ENABLE_CI_SEMPHORE)
int WVCI_InitSemphore(void);
int WVCI_GetSemphore(U32 u32TickCnt);
int WVCI_ReleaseSemphore(void);
#endif

#if defined(ENABLE_CI_MONITOR_PMT)
void WVCI_MonitorCaPmt(void);
#endif
/******************************************************************************
 * WVCI_Setup() - CI初始化函数
 * DESCRIPTION: - 
 *          初始化CI模块, 在工程初始化PIO, UART, I2C后可调用 
 * Input  : nCITaskPrior                        CI模块任务优先级
 * Output : None    
 * Returns: WVCI_SUCCESS                        操作成功
            WVCI_FAILURE                        操作失败
 * 
 ******************************************************************************/
int  WVCI_Setup();
void WVCI_InsertCAM(U8 u8Slot);
void WVCI_RemoveCAM(U8 u8Slot);
void WVCI_TaskMonitoring(void *pvParams);
void WVCI_TaskProcess(void *pvParams);
void STATUS_ResetCAM(U8 u8SlotIndex,U8 uErrorCode);
void WVCI_GetCAM_Comm_status(WVAlarm_CI_Status_t * CommStatus,int SlotIndex);
void WVCI_PMTCheck(void);
#ifdef ENABLE_CI_PES_CHECKING
int  WVCI_PES_TSBitrateSetEnable(U8 u8Slot, U8 u8Enable);
U8   WVCI_PES_TSBitrateGetEnable(U8 u8Slot);
int  WVCI_PES_Checking(void);
int  WVCI_PES_SetSlotIndex(int nSlot);
void WVCI_PES_ClearTSMonitor(U16 u16TSSource);
int  WVCI_PES_InitSlot(U8 u8Slot);
int  WVCI_PES_AddCheckPID2Slot(U8 u8Slot, U16 u16PID);
int  WVCI_PES_SetSinglePID(U8 u8Slot, U16 u16PID);
U8   WVCI_PES_GetSlotState(U8 u8Slot);
#endif

int WVCI_GenerateCAPMT(U8 u8Slot, U16 u16InProgIndex, TSInfo *pstParamTS, U8 eMode);
int WVCI_UpdateSetting(WVCI_SLOTMODE_t emSlotMode);
int TSMUX_CI_SetMaxBitrate(U32 u32Bitrate);
wvErrCode WVCI_GetCIStatus(U8 u8Slot, WAVCI_Status_4_web_t *pstCIStatus, TSInfo *pstParamTS);


#ifdef ENABLE_CHECK_DECODER_ESLIST_2DESCRAMBLE                      // 香港客户，旧CAM，单个节目只支持10个ES
BOOL WVCI_IsPlayingServiceInDescrambled(U8 u8Slot);
#endif


WAVCI_MMI_Info_t *WVCI_MMI_GetMenuInfo(void);
int WVCI_MMI_CommandProcess(BYTE u8Slot, BYTE u8MMICmd, BYTE *pucData, int nDataLen, int nTimoutMS);

void WVCI_TEST(void);

#endif // __WV_CI_H_



