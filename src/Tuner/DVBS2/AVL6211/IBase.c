/*
 *           Copyright 2012 Availink, Inc.
 *
 *  This software contains Availink proprietary information and
 *  its use and disclosure are restricted solely to the terms in
 *  the corresponding written license agreement. It shall not be 
 *  disclosed to anyone other than valid licensees without
 *  written permission of Availink, Inc.
 *
 */


///$Date: 2012-3-8 21:47 $
///
#include "IBase.h"
#include "II2C.h"
#include "IBSP.h"
#include "IRx.h"
#include "ITuner.h"
#include "II2CRepeater.h"
#include "IBlindScan.h"

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_SetPLL(AVL_uint32 u32Port, const struct AVL_DVBSx_PllConf * pPLLConf, struct AVL_DVBSx_Chip * pAVLChip )
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;

    r = AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, pll_clkf_map_addr, pPLLConf->m_uiClkf);
    r |= AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, pll_bwadj_map_addr, pPLLConf->m_uiClkf);
    r |= AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, pll_clkr_map_addr, pPLLConf->m_uiClkr);
    r |= AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, pll_od_map_addr, pPLLConf->m_uiPllod);
    r |= AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, pll_od2_map_addr, pPLLConf->m_uiPllod2);
    r |= AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, pll_od3_map_addr, pPLLConf->m_uiPllod3);
    r |= AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, pll_softvalue_en_map_addr,   1);
    r |= AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, reset_register_addr, 0);
    printf("****ack\r\n");
    AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, reset_register_addr, 1);  //this is a reset, do not expect an ACK from the chip.
    pAVLChip->m_DemodFrequency_10kHz = pPLLConf->m_DmodFrequency_10kHz;
    pAVLChip->m_FecFrequency_10kHz =  pPLLConf->m_FecFrequency_10kHz;
    pAVLChip->m_MpegFrequency_10kHz = pPLLConf->m_MpegFrequency_10kHz;

    return(r);
}
/// @endcond

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_DownloadFirmware(AVL_uint32 u32Port, AVL_puchar pFirmwareData,  const struct AVL_DVBSx_Chip * pAVLChip)
{
    AVL_uint32 uiSize, uiDataSize;
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
    AVL_uint32 i1;

    r = AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, core_reset_b_reg, 0);

    uiDataSize = DeChunk32(pFirmwareData);
    i1 = 4;
    while( i1 < uiDataSize )
    {
        uiSize = DeChunk32(pFirmwareData+i1);
        i1 += 4;
        r |= AVL_DVBSx_II2C_Write(u32Port, pAVLChip, pFirmwareData+i1+1, (AVL_uint16)(uiSize+3));
        i1 += 4 + uiSize;
    }
    printf("****fw  done\r\n");
    r |= AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, 0x00000000, 0x00003ffc);
    r |= AVL_DVBSx_II2C_Write16(u32Port, pAVLChip, core_ready_word_addr, 0x0000);
    r |= AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, error_msg_addr, 0x00000000);
    r |= AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, error_msg_addr+4, 0x00000000);
    AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, core_reset_b_reg, 1);  //this is a reset, do not expect an ACK from the chip.

    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_GetStatus(AVL_uint32 u32Port, struct AVL_DVBSx_Chip * pAVLChip )
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
    AVL_uint16 uiReadValue;
    AVL_uint32 uiTemp;

    r = AVL_DVBSx_II2C_Read32(u32Port, pAVLChip, core_reset_b_reg, &uiTemp);
    r |= AVL_DVBSx_II2C_Read16(u32Port, pAVLChip, core_ready_word_addr, &uiReadValue);
    if( (AVL_DVBSx_EC_OK == r) )
    {
        if( (0 == uiTemp) || (uiReadValue != 0xA55A) )
        {
            r = AVL_DVBSx_EC_GeneralFail;
        }
    }

    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_GetVersion(AVL_uint32 u32Port, struct AVL_DVBSx_VerInfo * pVerInfo, const struct AVL_DVBSx_Chip * pAVLChip )
{
    AVL_uint32 uiTemp;
    AVL_uchar ucBuff[4];
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;

    r =  AVL_DVBSx_II2C_Read32(u32Port, pAVLChip, rom_ver_addr, &uiTemp);
    if( AVL_DVBSx_EC_OK == r )
    {
        Chunk32(uiTemp, ucBuff);
        pVerInfo->m_Chip.m_Major = ucBuff[0];
        pVerInfo->m_Chip.m_Minor = ucBuff[1];
        pVerInfo->m_Chip.m_Build = ucBuff[2];
        pVerInfo->m_Chip.m_Build = ((AVL_uint16)((pVerInfo->m_Chip.m_Build)<<8)) + ucBuff[3];
        r |=  AVL_DVBSx_II2C_Read32(u32Port, pAVLChip, rc_patch_ver_addr, &uiTemp);
        if( AVL_DVBSx_EC_OK == r )
        {
            Chunk32(uiTemp, ucBuff);
            pVerInfo->m_Patch.m_Major = ucBuff[0];
            pVerInfo->m_Patch.m_Minor = ucBuff[1];
            pVerInfo->m_Patch.m_Build = ucBuff[2];
            pVerInfo->m_Patch.m_Build = ((AVL_uint16)((pVerInfo->m_Patch.m_Build)<<8)) + ucBuff[3];

            pVerInfo->m_API.m_Major = AVL_DVBSx_API_VER_MAJOR;
            pVerInfo->m_API.m_Minor = AVL_DVBSx_API_VER_MINOR;
            pVerInfo->m_API.m_Build = AVL_DVBSx_API_VER_BUILD;
        }
    }
    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_Initialize(AVL_uint32 u32Port, const struct AVL_DVBSx_PllConf * pPLLConf, AVL_puchar pInitialData,  struct AVL_DVBSx_Chip * pAVLChip )
{   
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
    r |= AVL_DVBSx_IBase_SetPLL(u32Port, pPLLConf, pAVLChip );
    r |= AVL_DVBSx_IBSP_Delay(100);     // this delay is critical 
    r |= AVL_DVBSx_IBase_DownloadFirmware(u32Port, pInitialData, pAVLChip);

    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_Halt(AVL_uint32 u32Port, struct AVL_DVBSx_Chip * pAVLChip )
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;

    r |= AVL_DVBSx_IBase_SendRxOP(u32Port, OP_RX_HALT, pAVLChip);    

    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_Sleep(AVL_uint32 u32Port, struct AVL_DVBSx_Chip * pAVLChip )
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;

    r |= AVL_DVBSx_IBase_SendRxOP(u32Port, OP_RX_SLEEP, pAVLChip);

    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_Wake(AVL_uint32 u32Port, struct AVL_DVBSx_Chip * pAVLChip )
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;

    r |= AVL_DVBSx_IBase_SendRxOP(u32Port, OP_RX_WAKE, pAVLChip);

    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_SetFunctionalMode(AVL_uint32 u32Port, const struct AVL_DVBSx_Chip * pAVLChip, enum AVL_DVBSx_FunctionalMode enumFunctionalMode)
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;

    r = AVL_DVBSx_II2C_Write16(u32Port, pAVLChip, rc_functional_mode_addr, (AVL_uint16)enumFunctionalMode);
#if 0
//disable the freq_offset in firmware.
    if(enumFunctionalMode == AVL_DVBSx_FunctMode_Demod)
    {
        r |= AVL_DVBSx_II2C_Write16(pAVLChip, 0x2642, 400);

    }
    else if(enumFunctionalMode == AVL_DVBSx_FunctMode_BlindScan)
    {
        r |= AVL_DVBSx_II2C_Write16(pAVLChip, 0x2642, 0);
    }
#endif  
    r |=AVL_DVBSx_II2C_Write16(u32Port, pAVLChip, rc_iq_mode_addr,0);

    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_GetFunctionalMode(AVL_uint32 u32Port, enum AVL_DVBSx_FunctionalMode * pFunctionalMode,  const struct AVL_DVBSx_Chip * pAVLChip)
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;    
    AVL_uint16 uiTemp;

    r = AVL_DVBSx_II2C_Read16(u32Port, pAVLChip, rc_functional_mode_addr, &uiTemp);
    *pFunctionalMode = (enum AVL_DVBSx_FunctionalMode)(uiTemp & 0x0001);    

    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_SendRxOP(AVL_uint32 u32Port, AVL_uchar ucOpCmd, struct AVL_DVBSx_Chip * pAVLChip )
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
    AVL_uchar pucBuff[2];
    AVL_uint16 uiTemp;
    enum AVL_DVBSx_Sleep_Wake_Status sleep_wake_status;
    const AVL_uint16 uiTimeDelay = 10;
    AVL_uint16 uiMaxRetries = 20;

    r = AVL_DVBSx_IBSP_WaitSemaphore(&(pAVLChip->m_semRx));
    r |= AVL_DVBSx_IBase_GetChipStatus(u32Port, &sleep_wake_status, pAVLChip);
    if(r == AVL_DVBSx_EC_OK)
    {
        if(sleep_wake_status == AVL_DVBSx_Sleep_Mode)
        {
            if((ucOpCmd != OP_RX_WAKE) && (ucOpCmd != OP_RX_HALT) && (ucOpCmd != OP_RX_SLEEP))
            {
                AVL_DVBSx_IBSP_ReleaseSemaphore(&(pAVLChip->m_semRx));
                r = AVL_DVBSx_EC_InSleepMode;
                return(r);
            }
        }

        do
        {
            r = AVL_DVBSx_IBase_GetRxOPStatus(u32Port, pAVLChip);
            if( AVL_DVBSx_EC_OK == r )
            {
                break;
            }
            AVL_DVBSx_IBSP_Delay(uiTimeDelay);
            uiMaxRetries--;

        }while(uiMaxRetries != 0);

        if( AVL_DVBSx_EC_OK == r )
        {
            pucBuff[0] = 0;
            pucBuff[1] = ucOpCmd;
            uiTemp = DeChunk16(pucBuff);
            r |= AVL_DVBSx_II2C_Write16(u32Port, pAVLChip, rx_cmd_addr, uiTemp);   
        }
    }
    r |= AVL_DVBSx_IBSP_ReleaseSemaphore(&(pAVLChip->m_semRx));

    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_GetRxOPStatus(AVL_uint32 u32Port, const struct AVL_DVBSx_Chip * pAVLChip )
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
    AVL_uchar pBuff[2] = {0,0};

    r = AVL_DVBSx_II2C_Read(u32Port, pAVLChip, rx_cmd_addr, pBuff, 2);
    if( AVL_DVBSx_EC_OK == r )
    {
        if( 0 != pBuff[1] )
        {
            r = AVL_DVBSx_EC_Running;
        }
    }

    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_SetGPIODir(AVL_uint32 u32Port, AVL_uchar ucDir, const struct AVL_DVBSx_Chip * pAVLChip )
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
    AVL_uint32 uiTemp = (AVL_uint32)(ucDir & 0x7);

    r = AVL_DVBSx_II2C_Write32(u32Port, pAVLChip, gpio_reg_enb, uiTemp);

    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_SetGPIOVal(AVL_uint32 u32Port, AVL_uchar ucVal, const struct AVL_DVBSx_Chip * pAVLChip )
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
    AVL_uint32 uiTemp;
    
    uiTemp = (AVL_uint32)(ucVal & 0x7);
    r = AVL_DVBSx_II2C_Write32(u32Port,pAVLChip, gpio_data_reg_out, uiTemp);

    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_GetGPIOVal(AVL_uint32 u32Port, AVL_puchar pucVal, const struct AVL_DVBSx_Chip * pAVLChip )
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
    AVL_uint32 uiTemp;

    r = AVL_DVBSx_II2C_Read32(u32Port,pAVLChip, gpio_data_in_to_reg, &uiTemp);
    *pucVal = (AVL_uchar)uiTemp;
    
    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_GetChipStatus(AVL_uint32 u32Port, enum AVL_DVBSx_Sleep_Wake_Status * pChipStatus, const struct AVL_DVBSx_Chip * pAVLChip )
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;
    AVL_uint16 uiStatus;

    r = AVL_DVBSx_II2C_Read16(u32Port,pAVLChip, rp_sleep_wake_status_addr, &uiStatus);
    if( AVL_DVBSx_EC_OK == r )
    {
        if((enum AVL_DVBSx_Sleep_Wake_Status)(uiStatus) == AVL_DVBSx_Sleep_Mode)
        {
            *pChipStatus = AVL_DVBSx_Sleep_Mode;

        }
        else if((enum AVL_DVBSx_Sleep_Wake_Status)(uiStatus) == AVL_DVBSx_Wake_Mode)
        {
            *pChipStatus = AVL_DVBSx_Wake_Mode;
        }
        else
        {
            r = AVL_DVBSx_EC_GeneralFail;
        }
    }

    return(r);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBase_SetSpectrumPolarity(AVL_uint32 u32Port, enum AVL_DVBSx_SpectrumPolarity enumSpectrumPolarity, const struct AVL_DVBSx_Chip * pAVLChip )
{
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;

    r = AVL_DVBSx_II2C_Write16(u32Port, pAVLChip, rc_blind_scan_tuner_spectrum_inversion_addr, (AVL_uint16)enumSpectrumPolarity);

    return(r);
}