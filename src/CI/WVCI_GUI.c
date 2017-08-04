
/******************************************************************************
 * WVCI_GUI - main
 * 
 * Copyright 2008-2013 HuiZhou WELLAV Technology Co.,Ltd.
 * 
 * DESCRIPTION: - 
 *      
 * modification history
 * --------------------
 * 02 Jun 2012, shouquan.tang : 执行第一次代码自审操作
 * --------------------
 ******************************************************************************/

#include "All.h"

//#include "TRACE/FlashLog.h"
#ifdef WVCI_USE_PRINT_FOR_GUI

#include "WVCI_GUI.h"
#include "WVCI_PlatformDriver.h"


#define WVCI_GUI_InfoPrint(x)       printf x

//#define DEBUG_ENABLE_DUMP_DATA
#define DEBUG_ENABLE_UI_INFO

extern CIMAX_PCMCIA_HANDLESTRUCT_t              g_CIMax_CIHandles[WVCI_MAX_SLOT_NUM]; 
extern U8                                       g_u8MMI_CurrentMenuLevel[WVCI_MAX_SLOT_NUM];
extern U8                                       g_u8MMI_AnswerTextLen;
WAVCI_MMI_Info_t                                g_stMMIInfo;        // menu info


BOOL    bIsInputPINOperate = FALSE;




#ifdef DEBUG_ENABLE_UI_INFO
int WVCI_GUIMessageBox(const BYTE *pucInfo)
{
    // VARS
    U8              u8UIMessage[WVCI_MENU_MAX_CONTENT];
    int             ii = 0;
    int             nInfoLen = 0;

    // CHECK PARAMS
    if (pucInfo == NULL)
    {
        return WVCI_ERROR_PARAMS;
    }

    // 
    memset(u8UIMessage, 0x00, sizeof(u8UIMessage));

    // 获取当前显示信息长度
    for (ii = 0; ii < 42; ii++)     //最大支持显示42个字节
    {
        if (pucInfo[ii] == 0x00     ||
            pucInfo[ii] == '\0'     ||
            pucInfo[ii] == '\r'     ||
            pucInfo[ii] == '\n')
        {
            break;
        }
        ii++;
    }    
    nInfoLen = ii;
    memcpy((char *)u8UIMessage, pucInfo, nInfoLen);
    WVCI_GUI_InfoPrint(("%s \r\n", u8UIMessage));
    return WVCI_SUCCESS;
}
#endif


#define CI_SENDMSG_TYPE_INDEX         (0) 

/******************************************************************************
 * CIMSG_ParseREQUEST_INPUT() - 
 * DESCRIPTION: - Parse REQUEST INPUT PROCESS
 * Input  : u8Slot          slot index    
            pucData         data from mmi which as request-input
            nDataLen        data length
 * Output : 
 * Returns: 
 * 
 ******************************************************************************/
static int CIMSG_ParseREQUEST_INPUT(U8 u8Slot, const BYTE *pucData, int nDataLen)
{
    // vars    
    int         ii          = 0;
    int         tt          = 0;
    int         nNewLine    = 0;
    int         offset      = 0; 

    #if 0
    printf ("================= len:%d ================\r\n", nDataLen);
    for (ii = 0; ii < nDataLen; ii++)
    {
        printf ("%02X ", pucData[ii]);
        if ((ii+1) % 16 == 0) printf ("\r\n");
    }
    printf ("\r\n");
    #endif
    
    g_stMMIInfo.slot_id                 = u8Slot;
    g_stMMIInfo.u8Blind_Answer          = (pucData[4]&0x01);    // blind answer  
    g_stMMIInfo.u8Answer_Text_Length    = pucData[5];           // answer text length        
    offset                              = 0;
    nNewLine                            = 0;
    g_stMMIInfo.item_offset[0]          = offset;
    g_u8MMI_AnswerTextLen               = pucData[5];
    for (ii = 6; ii < nDataLen; ii++)
    {                
        if(pucData[ii] != 0x00)
        {
            g_stMMIInfo.text[offset] = pucData[ii];             
            offset++;
        }
        else
        {   
            // new line
            g_stMMIInfo.text[offset] = 0x00;
            offset++;
            
            nNewLine++;
            g_stMMIInfo.item_offset[nNewLine] = offset;
        }
    }

    // clear other 
    g_stMMIInfo.choice_nb                = 0;        // title
    g_stMMIInfo.item_offset[1]           = offset;   // subtitle
    g_stMMIInfo.item_offset[2]           = offset;   // button
        
    CI_LOG_INF("title    :    %s \r\n", &g_stMMIInfo.text[g_stMMIInfo.item_offset[0]]);            
    CI_LOG_INF("sub_title:    %s \r\n", &g_stMMIInfo.text[g_stMMIInfo.item_offset[1]]);
    for (tt = 0; tt < g_stMMIInfo.choice_nb; tt++)
    {
        CI_LOG_INF("items    :    %s \r\n", &g_stMMIInfo.text[g_stMMIInfo.item_offset[3+tt]]);    
    }
    CI_LOG_INF("button   :    %s \r\n\r\n", &g_stMMIInfo.text[g_stMMIInfo.item_offset[2]]);
   
    return WVCI_SUCCESS;
}



/******************************************************************************
 * CIMSG_ParseMenu() - 
 * DESCRIPTION: - Parse MENU DATA PROCESS
 * Input  : u8Slot          slot index    
            pucData         data from mmi which as menu 
            nDataLen        data length
 * Output : 
 * Returns: 
 * 
 ******************************************************************************/
static int CIMSG_ParseMenu(int nSlotIndex, const BYTE *pucData, int nDataLen)
{
    // VARS
    //BYTE        ucMessageInfo[30][80];
    int         ii          = 0;
    int         tt          = 0;
    int         nNewLine    = 0;
    int         offset      = 0; 
    
    g_stMMIInfo.slot_id     = nSlotIndex;
    g_stMMIInfo.choice_nb  = (pucData[5] - 3);  
    offset              = 0;
    nNewLine            = 0;
    g_stMMIInfo.item_offset[0] = offset;
    for (ii = 6; ii < nDataLen; ii++)
    {                
        if(pucData[ii] != 0x00)
        {
            g_stMMIInfo.text[offset] = pucData[ii];             
            offset++;
        }
        else
        {   
            // new line
            g_stMMIInfo.text[offset] = 0x00;
            offset++;
            
            nNewLine++;
            g_stMMIInfo.item_offset[nNewLine] = offset;
        }
    }
    
    CI_LOG_INF("title    :    %s \r\n", &g_stMMIInfo.text[g_stMMIInfo.item_offset[0]]);            
    CI_LOG_INF("sub_title:    %s \r\n", &g_stMMIInfo.text[g_stMMIInfo.item_offset[1]]);
    for (tt = 0; tt < g_stMMIInfo.choice_nb; tt++)
    {
        CI_LOG_INF("items    :    %s \r\n", &g_stMMIInfo.text[g_stMMIInfo.item_offset[3+tt]]);    
    }
    CI_LOG_INF("button   :    %s \r\n\r\n", &g_stMMIInfo.text[g_stMMIInfo.item_offset[2]]);
   
    return WVCI_SUCCESS;
}



static int CIMSG_ParseInfo(const BYTE *pucData)
{
    // vars    
    BYTE        ucMessageInfoType       = 0x00;
    BYTE        ucCI_MSG_CI_INFO[48]    = {0};   
    BYTE        u8Slot                  = 0;
    int         kk                      = 0;

    // check params
    if (NULL == pucData)
    {
        CI_LOG_ERR("[CI] Line:%d, bad params!\r\n", __LINE__);  
        return WVCI_ERROR_PARAMS;
    }
               
    ucMessageInfoType           = pucData[1];
    u8Slot                      = pucData[2];

    // check params
    if (u8Slot >= WVCI_MAX_SLOT_NUM)
    {
        CI_LOG_ERR("[CI] Line:%d,  bad params!\r\n", __LINE__);  
        return WVCI_ERROR_PARAMS;        
    }
    
    switch(ucMessageInfoType)
    {                
        case CI_SLOT_EMPTY:
        {
            g_ptWVCIStatusInst->m_u16SystemIDNum[u8Slot] = 0;
            g_ptWVCIStatusInst->m_ucSlotInited[u8Slot] = 0;

            memset(&g_ptWVCIStatusInst->m_ucSlotName[u8Slot],          0x00,       WVCI_SLOT_NAME_SIZE);                        
            memcpy(&g_ptWVCIStatusInst->m_ucSlotName[u8Slot],          "EMPTY",    WVCI_SLOT_NAME_SIZE);
            memset(&g_ptWVCIStatusInst->m_ucSlotManufacturer[u8Slot],  0x00,       WVCI_SLOT_NAME_SIZE);                        
            memcpy(&g_ptWVCIStatusInst->m_ucSlotManufacturer[u8Slot],  "NULL ",    WVCI_SLOT_NAME_SIZE);
            g_ptWVCIStatusInst->m_u16SystemIDNum[u8Slot] = 0;
            for (kk = 0; kk < 32; kk++)
            {
                g_ptWVCIStatusInst->m_u16SystemIDList[u8Slot][kk] = 0;
            }
            
            //TS_MUX_CheckParams();   // 当CAM拔出时，使TS bypass

            #ifdef DEBUG_ENABLE_UI_INFO
            snprintf((char *)ucCI_MSG_CI_INFO, sizeof(ucCI_MSG_CI_INFO), "CI MODULE(%d) EMPTY", pucData[2]);
            WVCI_GUIMessageBox(ucCI_MSG_CI_INFO);    
            CI_LOG_INF("CI MODULE(%d) EMPTY\r\n",pucData[2]);
            #endif
            
            break;
        }

        case CI_SLOT_MODULE_INSERTED:
        {
            memset(&g_ptWVCIStatusInst->m_ucSlotName[u8Slot], 0x00, WVCI_SLOT_NAME_SIZE);                        
            memcpy(&g_ptWVCIStatusInst->m_ucSlotName[u8Slot], "Initializing...", WVCI_SLOT_NAME_SIZE);
           
            #ifdef DEBUG_ENABLE_UI_INFO
            snprintf((char *)ucCI_MSG_CI_INFO, sizeof(ucCI_MSG_CI_INFO), "CI MODULE(%d) INSERTED", pucData[2]);
            WVCI_GUIMessageBox(ucCI_MSG_CI_INFO); 
            CI_LOG_INF("CI MODULE(%d) INSERTED\r\n", u8Slot);
            #endif
            break;
        }

        case CI_SLOT_MODULE_NAME_OK:
        {            
            memset(g_ptWVCIStatusInst->m_ucSlotName[u8Slot], 0, WVCI_SLOT_NAME_SIZE);
            memcpy(g_ptWVCIStatusInst->m_ucSlotName[u8Slot],          &CiSlot[u8Slot].pszModuleName, (WVCI_SLOT_NAME_SIZE-1));
            memcpy(g_ptWVCIStatusInst->m_ucSlotManufacturer[u8Slot],  (char *)g_CIMax_CIHandles[u8Slot].tPCMCIAInfo.pszManufacturerName, (WVCI_SLOT_NAME_SIZE-1));

            #ifdef DEBUG_ENABLE_UI_INFO
            snprintf((char *)ucCI_MSG_CI_INFO, sizeof(ucCI_MSG_CI_INFO), "CI MODULE(%d) NAME(%s)", 
                            pucData[2],
                            CiSlot[u8Slot].pszModuleName);
            WVCI_GUIMessageBox(ucCI_MSG_CI_INFO);    
            CI_LOG_INF("CI MODULE(%d) NAME(%s)\r\n",
                                                    u8Slot,
                                                    CiSlot[u8Slot].pszModuleName);
            #endif
            break;
        }

        case CI_SLOT_MODULE_CA_OK:
        {
            WVCI_SetAutoSendPMTFlag(u8Slot); // 自动更新PMT
            g_ptWVCIStatusInst->m_ucSlotInited[u8Slot] = 1;
            //TS_MUX_CheckParams();   // 当CAM插入正常时，使TS其通过CAM
            if (u8Slot == 0)
            {
                g_ptWVCIStatusInst->m_u16SystemIDNum[u8Slot] = CiSlot[u8Slot].wNumCASystems;
                for (kk = 0; kk < CiSlot[u8Slot].wNumCASystems; kk++)
                {                    
                    g_ptWVCIStatusInst->m_u16SystemIDList[u8Slot][kk] = CiSlot[u8Slot].wCASystemIDs[kk];                
                }
            }
            else
            {
                g_ptWVCIStatusInst->m_u16SystemIDNum[u8Slot] = CiSlot[u8Slot].wNumCASystems;
                for (kk = 0; kk < CiSlot[u8Slot].wNumCASystems; kk++)
                {                    
                    g_ptWVCIStatusInst->m_u16SystemIDList[u8Slot][kk] = CiSlot[u8Slot].wCASystemIDs[kk];                
                }
            }
            #ifdef DEBUG_ENABLE_UI_INFO
            snprintf((char *)ucCI_MSG_CI_INFO, sizeof(ucCI_MSG_CI_INFO), "CI MODULE(%d) CA OK", u8Slot);
            WVCI_GUIMessageBox(ucCI_MSG_CI_INFO);  
            CI_LOG_INF("CI MODULE(%d) CA OK", u8Slot);
            #endif

            break;
        }

        default:
        {
            CI_LOG_WARR("[WVCI_GUIParseMessage] unknown msg: %d\r\n", CI_MSG_CI_INFO);
            break;     
        }
    }
    
    return WVCI_SUCCESS;
}



/******************************************************************************
 * WVCI_GUIParseMessage() - 
 * DESCRIPTION: - Parse CI-STACK Callback Message PROCESS
 * Input  : u8Slot          slot index    
            pucData         data from mmi, which as message
            nDataLen        data length
 * Output : 
 * Returns: 
 * 
 ******************************************************************************/
int WVCI_GUIParseMessage(U8 u8Slot, const BYTE *pucData, int nDataLen)
{       
    // VARS

    // CHECK PARAMS
    if (u8Slot >= WVCI_MAX_SLOT_NUM)
    {
        CI_LOG_ERR("[WVCI_GUIParseMessage] u8Slot(%d), --PARAMS INVALID!!!! \r\n", u8Slot);
        return WVCI_ERROR_PARAMS;
    }

    if (pucData == NULL     ||
        nDataLen <= 0)
    {
        CI_LOG_ERR("pucData(%08X), nDataLen(%d), --PARAMS INVALID!!!! \r\n", pucData, nDataLen);
        return WVCI_ERROR_PARAMS;
    }

    bIsInputPINOperate = FALSE;

    // 以下是分析具体的数据
    g_stMMIInfo.slot_id      = u8Slot;
    g_stMMIInfo.message_type = pucData[0];
    g_stMMIInfo.text_len     = nDataLen;
    switch (g_stMMIInfo.message_type)
    {
        case CI_MSG_NONE:
        {
            CI_LOG_INF("[WVCI_GUIParseMessage] CI_MSG_NONE: \r\n");
            break;
        }
        case CI_MSG_CI_INFO:
        {   
            CI_LOG_INF("[WVCI_GUIParseMessage] CI_MSG_CI_INFO: \r\n");
            CIMSG_ParseInfo(pucData);
            break;
        }
        case CI_MMI_MENU:
        {            
            CI_LOG_INF("[WVCI_GUIParseMessage] CI_MMI_MENU: \r\n");
            CIMSG_ParseMenu(u8Slot, pucData, nDataLen);
            if (WAV_CI_MMI_ROOT_LEVEL == g_u8MMI_CurrentMenuLevel[u8Slot])
            {
                //g_stMMIInfo.mmi_text_valid = 0x01;
                CI_LOG_INF("[WVCI_GUIParseMessage] Slot:%d, no user operate mmi, force close!\r\n", u8Slot);
                CI_MMI_Close(u8Slot);
            }
            else 
            {
                g_stMMIInfo.mmi_text_valid = 0x01;
            }
            break;
        }
        case CI_MMI_LIST:
        {            
            CI_LOG_INF("[WVCI_GUIParseMessage] CI_MMI_LIST: \r\n");
            CIMSG_ParseMenu(u8Slot, pucData, nDataLen);   
            g_stMMIInfo.mmi_text_valid = 0x01;
            
            break;
        }
        case CI_MMI_TEXT:
        {            
            CI_LOG_INF("[WVCI_GUIParseMessage] CI_MMI_TEXT: \r\n");            
            g_stMMIInfo.mmi_text_valid = 0x01;
            break;
        }
        case CI_MMI_REQUEST_INPUT:
        {   
            CI_LOG_INF("[WVCI_GUIParseMessage] CI_MMI_REQUEST_INPUT: \r\n");
            CIMSG_ParseREQUEST_INPUT(u8Slot, pucData, nDataLen);            
            g_stMMIInfo.mmi_text_valid = 0x01;
            bIsInputPINOperate = TRUE;            
            break;
        }
        case CI_MMI_CLOSE_MMI_IMM:
        {
            CI_LOG_INF("[WVCI_GUIParseMessage] CI_MMI_CLOSE_MMI_IMM: \r\n");
            if (g_u8MMI_CurrentMenuLevel[u8Slot] != WAV_CI_MMI_ROOT_LEVEL)
            {
                g_u8MMI_CurrentMenuLevel[u8Slot] = WAV_CI_MMI_ROOT_LEVEL;
            }
            break;
        }
        case CI_MMI_CA_PMT_REPLY:
        {
            CI_LOG_INF("[WVCI_GUIParseMessage] CI_MMI_CA_PMT_REPLY: \r\n");
            break;
        }
        
        default:
        {
            CI_LOG_ERR("[WVCI_GUIParseMessage] UNKNOW CI MESSAGE TYPE(%d), PLEASE CHECK!!!! \r\n", g_stMMIInfo.message_type);
            break;
        }
    }    

    return WVCI_SUCCESS;
}

#endif

