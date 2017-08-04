/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     LicenseParse.c
* Description:  LicenseParse Interface function
* Author    :   jie.zhan
* Modified  :
* Reviewer  :
* Date      :   2017-03-28
* Record    :
*
**********************************************************************/

#include <stdlib.h>
#include <string.h>
#include "LicenseParse.h"
#include "DS2432.h"
#include "WV_log.h"

extern unsigned char u8Code_DS2432[8];

/*****************************************************************************
  Function:     LIC_ParseLicenseInfo
  Description:  Parse License Infomation
  Input:        pu8LicenseData          --- 需要解析句柄
                pLicenseInfo            --- 存放解析License信息结构体句柄
  Output:       none
  Return:       LIC_ERROR_BAD_PARAMS
                LIC_ERROR_COMPANY_SYNC
                LIC_SUCCESS
  Author:       liuliang 20130913
*****************************************************************************/
S32 LIC_ParseLicenseInfo(const U8* pu8LicenseData, LicenseInfo_t* pLicenseInfo)
{
    // VARS
    S32                     ii          = 0;
    S32                     nOffset     = 0;
    U8                      u8AllTagLen = 0;
    U8                      u8ParsedLen = 0;
    DS2432_ChipInfo_t*      pChipInfo   = NULL;

    U8  TagSync = 0x00;
    U8  TagID   = 0;
    U8  TagLen  = 0;
    U8  TagData[LIC_TAG_MAX_SIZE];
    U32  *pu32SDHDFlag = NULL;

    // CHECK PARAMS
    if ((NULL == pu8LicenseData) || (NULL == pLicenseInfo))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DS2432, "[LIC_ParseLicenseInfo] LIC_ERROR_BAD_PARAMS!!");
        return LIC_ERROR_BAD_PARAMS;
    }

    pChipInfo = (DS2432_ChipInfo_t*)malloc(sizeof(DS2432_ChipInfo_t));
    if(NULL == pChipInfo)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DS2432, "[LIC_ParseLicenseInfo] Malloc failure!!");
        return LIC_ERROR_BAD_PARAMS;
    }

    memcpy(pChipInfo, pu8LicenseData, sizeof(DS2432_ChipInfo_t));
    
    // "wAv" == {0x77, 0x41, 0x76};
    if (pChipInfo->u8Company[0] != 0x77     ||
        pChipInfo->u8Company[1] != 0x41     ||
        pChipInfo->u8Company[2] != 0x76)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DS2432, "[LIC_ParseLicenseInfo] LIC_ERROR_COMPANY_SYNC!!");
        if(NULL != pChipInfo)
        {
            free(pChipInfo);
        }
        
        return LIC_ERROR_COMPANY_SYNC;
    }

    memset(pLicenseInfo, 0x00, sizeof(LicenseInfo_t));

    // chip type
    pLicenseInfo->SubboardID    = pChipInfo->u8SubboardID;
    u8AllTagLen                 = pChipInfo->u8TAGDataLen;
    pLicenseInfo->Date          = pChipInfo->u32LastUpdateTime;
    pLicenseInfo->u8StructVer   = pChipInfo->u8StructVer;

    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DS2432, "[LIC_ParseLicenseInfo]boardType =0x%02x  Date = 0x%08x,u8StructVer= 0x%02x",
                  pLicenseInfo->SubboardID,pLicenseInfo->Date,pLicenseInfo->u8StructVer);

    //for(ii = 0; ii < u8AllTagLen; ii++)
    //{
        //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DS2432, "[LIC_ParseLicenseInfo] u8HWVersion = %d u8TAGData[%d]:0x%x",pChipInfo->u8HWVersion, ii, pChipInfo->u8TAGData[ii]);
    //}
    
    // Parse Each Tag
    if (u8AllTagLen == 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DS2432, "[LIC_ParseLicenseInfo] AllTagLen is 0");
        if(NULL != pChipInfo)
        {
            free(pChipInfo);
        }
        
        return LIC_SUCCESS;
    }
    
    memset(TagData, 0x00, LIC_TAG_MAX_SIZE);

    u8ParsedLen = 0;
    nOffset     = 12;
    while (u8ParsedLen < u8AllTagLen)
    {
        // Find Tag Sync
        TagSync = pu8LicenseData[nOffset];
        if (TagSync == TAG_SYNC)
        {
            TagID   = pu8LicenseData[nOffset+1];
            TagLen  = pu8LicenseData[nOffset+2];
            memcpy(TagData, &pu8LicenseData[nOffset+3], TagLen);

            nOffset         += (TagLen + 3); // tag_sync(1)+tag_id(1)+tag_len(1)+tag_data(n)
            u8ParsedLen     += (TagLen + 3);

            // 把它放到对应的位置去，方便后面比较 Index = tagid
            pLicenseInfo->TagArray[TagID].TagID                 = TagID;
            pLicenseInfo->TagArray[TagID].TagValid              = 0x01;
            pLicenseInfo->TagArray[TagID].TagLen                = TagLen;
            memcpy(pLicenseInfo->TagArray[TagID].TagData, TagData, TagLen);
            pLicenseInfo->TagValidNum++;
        }
        else
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DS2432, "[LIC_ParseLicenseInfo] No found tag sync, should never come into here...");
            nOffset++;
            u8ParsedLen++;
        }
    }

    // Setting Basic Info
    for (ii = 0; ii < LIC_TAG_MAX_NUM; ii++)
    {
        if (pLicenseInfo->TagArray[ii].TagValid != 0x1)
        {
            continue;
        }
        
        switch (pLicenseInfo->TagArray[ii].TagID)
        {
            
            case TAG_TSIP_MAC:/*主板和子板mac tag相同*/
            {
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DS2432, "[LIC_ParseLicenseInfo] TagID = 0x%x\r\n", pLicenseInfo->TagArray[ii].TagID);
                memcpy(pLicenseInfo->Macaddress0, &pLicenseInfo->TagArray[ii].TagData[0], 6);
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DS2432,"[LIC_ParseLicenseInfo] MAC0: %02x-%02x-%02x-%02x-%02x-%02x ",
                                 pLicenseInfo->Macaddress0[0],pLicenseInfo->Macaddress0[1],pLicenseInfo->Macaddress0[2],
                                 pLicenseInfo->Macaddress0[3],pLicenseInfo->Macaddress0[4],pLicenseInfo->Macaddress0[5]
                             );
                if(pLicenseInfo->TagArray[ii].TagLen == 12)
                {
                    memcpy(pLicenseInfo->Macaddress1, &pLicenseInfo->TagArray[ii].TagData[6], 6);
                    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DS2432,"[LIC_ParseLicenseInfo] MAC1: %02x-%02x-%02x-%02x-%02x-%02x ",
                                  pLicenseInfo->Macaddress1[0],pLicenseInfo->Macaddress1[1],pLicenseInfo->Macaddress1[2],
                                  pLicenseInfo->Macaddress1[3],pLicenseInfo->Macaddress1[4],pLicenseInfo->Macaddress1[5]
                                  );
                }
                
                break;
            }
                
            case TAG_FP_MODEL:
            {
                break;
            }

            case TAG_DECODER_SDHD:
            {
                pLicenseInfo->Decode_SDHD = pLicenseInfo->TagArray[ii].TagData[0];
                break;
            }

            case TAG_COMPANY_LOGO:
            {
                memcpy(pLicenseInfo->Company, &pLicenseInfo->TagArray[ii].TagData[0], 10);
                break;
            }

            case TAG_EIT:
            {
                break;
            }

            case TAG_CUSTOMER_NO:
            {
                memcpy(pLicenseInfo->au8CustomerNo, &pLicenseInfo->TagArray[ii].TagData[0], LENGTH_CUSTOMER_NO);
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DS2432,"[LIC_ParseLicenseInfo]customerno: %s", pLicenseInfo->au8CustomerNo);
                break;
            }

            case TAG_MACHINE_TYPE:
            {
                pLicenseInfo->u16MachineType = ((pLicenseInfo->TagArray[ii].TagData[0])<<8)
                                               |(pLicenseInfo->TagArray[ii].TagData[1]);
                break;
            }

            case TAG_SOFTWARE_FUNC_VER:
            {
                pLicenseInfo->u16SwFuncVer = ((pLicenseInfo->TagArray[ii].TagData[0])<<8)
                                               | (pLicenseInfo->TagArray[ii].TagData[1]);
                break;
            }

            case TAG_TIME_CONSTRAINT:
            {
                pLicenseInfo->u32EndTime = ((pLicenseInfo->TagArray[ii].TagData[3])<<24
                                                 | (pLicenseInfo->TagArray[ii].TagData[2])<<16
                                                 |(pLicenseInfo->TagArray[ii].TagData[1])<<8
                                                 |(pLicenseInfo->TagArray[ii].TagData[0]));
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DS2432, "[LIC_ParseLicenseInfo]u32EndTime: 0x%x", pLicenseInfo->u32EndTime);
                break;
            }

            case TAG_TSIP_0IN0OUT_FLAG:
            {
                pLicenseInfo->u8Tsip0in0outFlag = pLicenseInfo->TagArray[ii].TagData[0];
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DS2432, "[LIC_ParseLicenseInfo]TSIP 0IN0OUT: 0x%x", pLicenseInfo->u8Tsip0in0outFlag);
                break;
            }

            case TAG_LED_RULE_FLAG:
            {
                pLicenseInfo->u8LEDRuleFlag = pLicenseInfo->TagArray[ii].TagData[0];
                break;
            }

            case TAG_TSIP_CHANNEL:
            {
                pLicenseInfo->InputChannelNum   = pLicenseInfo->TagArray[ii].TagData[0];
                pLicenseInfo->OutputChannelNum  = pLicenseInfo->TagArray[ii].TagData[1];
                break;
            }

            case TAG_MODULATOR_CHANNEL:
            {
                pLicenseInfo->ModulatorChannelNum = pLicenseInfo->TagArray[ii].TagData[0];
                break;
            }

            case TAG_ENCODER_SDHD:
            {
                pLicenseInfo->Auth_SDHD[0] = pLicenseInfo->TagArray[ii].TagData[0];
                pLicenseInfo->Auth_SDHD[1] = pLicenseInfo->TagArray[ii].TagData[1];

                if (pLicenseInfo->Auth_SDHD[0] == 0x00)
                {
                    *pu32SDHDFlag = 0x01;
                }
                else
                {
                    *pu32SDHDFlag = 0x00;
                }

                break;
            }

            case TAG_DIRECTION:
            {
                pLicenseInfo->Direction = pLicenseInfo->TagArray[ii].TagData[0];
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DS2432, "[LIC_ParseLicenseInfo]Direction: %d", pLicenseInfo->Direction);
                break;
            }

            case TAG_ENCODETYPE:
            {
                pLicenseInfo->AudioEncode = pLicenseInfo->TagArray[ii].TagData[0];
                break;
            }

            case TAG_AUDIOENCODE:
            {
                pLicenseInfo->AudioEncode = pLicenseInfo->TagArray[ii].TagData[0];
                break;
            }

            case TAG_TRANSCODE_TYPE:
            {
                pLicenseInfo->TranscodeType[0] = pLicenseInfo->TagArray[ii].TagData[0];
                pLicenseInfo->TranscodeType[1] = pLicenseInfo->TagArray[ii].TagData[1];
                break;
            }

            case TAG_RECEIVER_T_CHANNEL:
            {
                pLicenseInfo->InputChannelNum = pLicenseInfo->TagArray[ii].TagData[0];
                break;
            }

            case TAG_CARD_TYPE:
            {
                pLicenseInfo->u8EncoderType1 = pLicenseInfo->TagArray[ii].TagData[0];
                if(pLicenseInfo->TagArray[ii].TagLen == 2)
                {
                    pLicenseInfo->u8EncoderType2 = pLicenseInfo->TagArray[ii].TagData[1];
                }
                
                break;
            }

            case TAG_QAM_FW_TYPE:
            {
                pLicenseInfo->u8QAMFWVersion = pLicenseInfo->TagArray[ii].TagData[0];
                break;
            }

            case TAG_ENHDMI_PLUS_SDHD:
            {
                pLicenseInfo->u8HDMIAuth[0] = pLicenseInfo->TagArray[ii].TagData[0];
                pLicenseInfo->u8HDMIAuth[1] = pLicenseInfo->TagArray[ii].TagData[1];
                pLicenseInfo->u8HDMIAuth[2] = pLicenseInfo->TagArray[ii].TagData[2];
                pLicenseInfo->u8HDMIAuth[3] = pLicenseInfo->TagArray[ii].TagData[3];

                break;
            }
            
            case TAG_ENHDMI_PLUS_AUD:
            {
                pLicenseInfo->u8HDMIFWType[0] = pLicenseInfo->TagArray[ii].TagData[0];
                pLicenseInfo->u8HDMIFWType[1] = pLicenseInfo->TagArray[ii].TagData[1];
                break;
            }
            
            case TAG_EN4AV_FW_TYPE:
            {
                pLicenseInfo->u8EN4AVM28FWType = pLicenseInfo->TagArray[ii].TagData[0];
                break;
            }

            case TAG_VIDEO_DETECTION:
            {
                pLicenseInfo->u8VideoDetection = pLicenseInfo->TagArray[ii].TagData[0];
                break;
            }

            case TAG_SMART_ENCODE:
            {
                pLicenseInfo->u8SmartEncode = pLicenseInfo->TagArray[ii].TagData[0];
                break;
            }

            case TAG_HDCP_AUTH_FLAG:
            {
                pLicenseInfo->u8HDCPAuthFlag = pLicenseInfo->TagArray[ii].TagData[0];
                break;
            }
            
            case TAG_SCRAMBLER_TYPE:
            {
                pLicenseInfo->ScramblerType = pLicenseInfo->TagArray[ii].TagData[0];
                break;
            }
            
            case TAG_VIDEO_TYPE:
            {
                pLicenseInfo->u8VideoType   = pLicenseInfo->TagArray[ii].TagData[0];
                break;
            }

            case TAG_SCR_CA_NUM:
            {
                pLicenseInfo->u8CaNumber = pLicenseInfo->TagArray[ii].TagData[0];
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DS2432, "[LIC_ParseLicenseInfo]u8CaNumber: %d", pLicenseInfo->u8CaNumber);
                break;
            }

            case TAG_SCR_MAX_UNIT_NUM:
            {
                pLicenseInfo->u8MaxScrUnitNumLevel = pLicenseInfo->TagArray[ii].TagData[0];
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DS2432, "[LIC_ParseLicenseInfo]u8MaxScrUnitNumLevel: %d", pLicenseInfo->u8MaxScrUnitNumLevel);
                break;
            }

            default:
            {
                LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DS2432,"[LIC_ParseLicenseInfo] not process tag: 0x%02X \r\n", pLicenseInfo->TagArray[ii].TagID);
                break;
            }
        }
    }

    if(NULL != pChipInfo)
    {
        free(pChipInfo);
    }
    
    return LIC_SUCCESS;
}

/*****************************************************************************
  Function:     LIC_ApplyLicenseConfig
  Description:  Apply License Config
  Input:        pstrLicenseInfo       --- License信息句柄
                pu8Params             --- 应用参数句柄       
  Output:       none
  Return:       LIC_SUCCESS
                LIC_ERROR_BAD_PARAMS
  Modify:       jie.zhan
*****************************************************************************/
S32 LIC_ApplyLicenseConfig(LicenseInfo_t* pstrLicenseInfo, U8 *pu8Params)
{
    if((NULL == pstrLicenseInfo) || (NULL == pu8Params))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DS2432, "[LIC_ApplyLicenseConfig] LIC_ERROR_BAD_PARAMS!!");
        return LIC_ERROR_BAD_PARAMS;
    }

    //TODO 根据解析的LincenseInfo_t信息进行应用，如MAC地址
    return LIC_SUCCESS;
}

