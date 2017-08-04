/*****************************************************************************
* Copyright (c) 2007,WellAV Technologies Ltd. All rights reserved. 
* File Name£º       Avl6211Manager.h
* Description:              
* History:      
                        
<Author>            <Date>              <Version >             <Reason>
Jiahong.he        2012/6/15              1.0                    Create
*****************************************************************************/
#include "Avl_DVBSx.h"
#include "iTuner.h"
#include "Avl6211_LockSignal.h"
#include "Avl6211Manager.h"

#include "appGlobal.h"


//static Avl6211_SearchParams_t s_tAvl6211CurSearchingParams;
static struct AVL_DVBSx_Chip s_Avl6211Chip[AVL_PORT_NUM] = {
    {
        .m_SlaveAddr = AVL_DVBSx_SA_0,
    },
    {
        .m_SlaveAddr = AVL_DVBSx_SA_1,
    },
    {
        .m_SlaveAddr = AVL_DVBSx_SB_0,
    },
    {
        .m_SlaveAddr = AVL_DVBSx_SB_1
    }
    
  
};
    
static struct AVL_Tuner s_Avl6211Tuner[AVL_PORT_NUM];

extern U8 u8TunerChipType;

static AVL_uint32 s_dwLockFrequency[AVL_PORT_NUM] = {0};
static AVL_int32  s_6211demodState[AVL_PORT_NUM]  = {0}; 



extern void i2c_select(int index);
extern void i2c_init(void); 
extern AVL_DVBSx_ErrorCode Avl6211_Initialize(Avl6211_PortIndex_t ePortIndex, struct AVL_DVBSx_Chip * pAVLChip,struct AVL_Tuner * pTuner);
extern U16  SM_GetStatus(void);
extern AVL_DVBSx_ErrorCode Avl6211_LockSignal(AVL_uint32 u32Port,
                                 AVL_uint32 TunerFrequency,
                                 AVL_uint32 SignalSymbolRate,
                                 AVL_uint32 LNBFrequency,
                                 AVL_uint32 *pNewFrequency,
                                 struct AVL_DVBSx_Chip * pAvl6211Chip,
                                 struct AVL_Tuner * pTuner);
extern AVL_DVBSx_ErrorCode AVL_DVBSx_IRx_GetRFOffset(AVL_uint32 u32Port, AVL_pint16 piRFOffset_100kHz, const struct AVL_DVBSx_Chip * pAVLChip );
extern AVL_DVBSx_ErrorCode Avl6211_GetFECCodeRate(AVL_uint32 u32Port, struct AVL_DVBSx_Chip *pAVLChip, AVL_uchar *pCodeRate, AVL_int32 *pDemodState);
extern AVL_DVBSx_ErrorCode Avl6211_GetPacketErrorRate(AVL_uint32 u32Port, struct AVL_DVBSx_Chip *pAVLChip, AVL_uint32 *uiPER);
extern AVL_DVBSx_ErrorCode Avl6211_GetBitErrorRate(AVL_uint32 u32Port, struct AVL_DVBSx_Chip *pAVLChip, AVL_uint32 *uiBER);
extern AVL_DVBSx_ErrorCode Avl6211_GetSignalQuality(AVL_uint32 u32Port, struct AVL_DVBSx_Chip *pAVLChip, AVL_int32 *SignalQuality);
extern AVL_DVBSx_ErrorCode Avl6211_GetSignalStrength(AVL_uint32 u32Port, struct AVL_DVBSx_Chip *pAVLChip, AVL_int32 *SignalStrength);
extern AVL_DVBSx_ErrorCode AVL_DVBSx_II2C_Read32(AVL_uint32 u32Port, const struct AVL_DVBSx_Chip * pAVLChip, AVL_uint32 uiAddr, AVL_puint32 puiData );
extern AVL_DVBSx_ErrorCode AVL_DVBSx_IRx_GetLockStatus(AVL_uint32 u32Port, AVL_puint16 puiLockStatus, const struct AVL_DVBSx_Chip * pAVLChip );
extern AVL_DVBSx_ErrorCode AVL_DVBSx_IBSP_Delay( AVL_uint32 uiMS );
extern AVL_DVBSx_ErrorCode AVL_DVBSx_IDiseqc_StopContinuous (AVL_uint32 u32Port, struct AVL_DVBSx_Chip * pAVLChip );
extern AVL_DVBSx_ErrorCode AVL_DVBSx_IDiseqc_StartContinuous (AVL_uint32 u32Port, struct AVL_DVBSx_Chip * pAVLChip );


/*****************************************************************************
  Function:         AVLTunerInit
  Description:      Init the avl6211 tuner
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2012/6/15
*****************************************************************************/  
int  AVLTunerInit(Avl6211_PortIndex_t ePortIndex)
{
    AVL_DVBSx_ErrorCode   uRet;

    //init status params
    s_dwLockFrequency[ePortIndex]  = 0;
  
    uRet = Avl6211_Initialize(ePortIndex, &s_Avl6211Chip[ePortIndex], &s_Avl6211Tuner[ePortIndex]);
    if (uRet != AVL_DVBSx_EC_OK)
    {
        printf("[S2TNR]: Avl6211 init port %d error!!\r\n", ePortIndex);
        return AVL_ERROR;
    }

    printf("**** Avl6211  port %d init ok\r\n", ePortIndex);
   
    return AVL_SUCCESS;
}


/*****************************************************************************
  Function:         AVLTunerLockSignal
  Description:      Lock the signal
  Input:            
  Output:           
  Return:           0 - Locked;   1 - unLocked;   -1  - Error
  Others:           
  Create:       2012/6/15
*****************************************************************************/  
int  AVLTunerLockSignal(Avl6211_PortIndex_t ePortIndex, Avl6211_SearchParams_t *pSearchParams)
{
    AVL_DVBSx_ErrorCode  uRet = AVL_SUCCESS;
    AVL_uint32           uRealFrequency;

    s_dwLockFrequency[ePortIndex] = pSearchParams->dwFrequency;


    uRet = Avl6211_LockSignal(ePortIndex,
                              pSearchParams->dwFrequency,
                              pSearchParams->dwSymbolRate,
                              pSearchParams->dwLNBFrequencyKHz,
                              &uRealFrequency,
                              &s_Avl6211Chip[ePortIndex],
                              &s_Avl6211Tuner[ePortIndex]);
                              
    if(AVL_DVBSx_EC_OK == uRet)
    {
        printf(" ~~~ [AVLTunerLockSignal]  uRealFrequency: %d \r\n",uRealFrequency);
        return AVL_SUCCESS;
    }
    else
    {
        printf("avl6211 unlock\r\n");
        return AVL_UNLOCK;
    }
}

/*****************************************************************************
  Function:         AVLTunerGetTunFreqOffset
  Description:      Get tune frequency offset
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2012/7/04
*****************************************************************************/  
int AVLTunerGetTunFeqOffset(Avl6211_PortIndex_t ePortIndex)
{
    AVL_int16 iRFOff;

    AVL_DVBSx_IRx_GetRFOffset(ePortIndex, &iRFOff, &s_Avl6211Chip[ePortIndex]);
    //printf("~~~~~~~~ [AVLTunerGetTunFeqOffset] iRFOff: %d \r\n",iRFOff*100);
    return iRFOff*100;
    
}

/*****************************************************************************
  Function:         AVLTunerGetLockFrequency
  Description:      Get the real frequency of lock now.
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2012/11/15
*****************************************************************************/  
U32 AVLTunerGetLockFrequency(Avl6211_PortIndex_t ePortIndex)
{
    AVL_uint32 u32Freq = 0;
    u32Freq = s_dwLockFrequency[ePortIndex] + (AVL_uint32)AVLTunerGetTunFeqOffset(ePortIndex);
    //printf("==>[%s] u32Freq:%d.\r\n", __FUNCTION__, u32Freq);

    return u32Freq;
}

/*****************************************************************************
  Function:         AVLTunerGetPER
  Description:      Get the Packet error rate
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2012/11/15
*****************************************************************************/  
int  AVLTunerGetFECCodeRate(Avl6211_PortIndex_t ePortIndex, U8 *pCodeRate)
{
    AVL_DVBSx_ErrorCode uRet = AVL_DVBSx_EC_OK;

    uRet = Avl6211_GetFECCodeRate(ePortIndex, &s_Avl6211Chip[ePortIndex], pCodeRate, &s_6211demodState[ePortIndex]);
    //printf("==>[%s] pCodeRate:%d.\r\n", __FUNCTION__, *pCodeRate);

    return ( (uRet == AVL_DVBSx_EC_OK) ? AVL_SUCCESS : AVL_ERROR );
}

/*****************************************************************************
  Function:         AVLTunerGetPER
  Description:      Get the Packet error rate
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2012/11/15
*****************************************************************************/  
int  AVLTunerGetPER(Avl6211_PortIndex_t ePortIndex, float *fpuPER)
{
    AVL_DVBSx_ErrorCode uRet = AVL_DVBSx_EC_OK;
    U32  puPER = 0; 

    uRet = Avl6211_GetPacketErrorRate(ePortIndex, &s_Avl6211Chip[ePortIndex], &puPER);

    if (AVL_DVBSx_EC_OK == uRet)
    {
        *fpuPER = (float)((puPER)*1.0e-9); 
    }
    
    //printf("==>[%s] *puPER:%d.\r\n", __FUNCTION__, *puPER);

    return ( (uRet == AVL_DVBSx_EC_OK) ? AVL_SUCCESS : AVL_ERROR );
}

//this funtion is copy from 0900 tuner.
static void GetBerValueExpo(U32 u32Ber, U32 *pu32Value, U32 *pu32Expo)
{
    U32 Cnt = 0; 
    U32 u32TmpBer = u32Ber;

    while (u32TmpBer > 9)
    {
        u32TmpBer = u32TmpBer/10;  
        Cnt++; 
    }

    *pu32Expo = (Cnt > 9) ? 9 : (9 - Cnt); 
    *pu32Value = (u32TmpBer == 0) ? 1 : u32TmpBer; 
}

/*****************************************************************************
  Function:         AVLTunerGetBER_PER
  Description:      Get the bit error rate / Get the Packet error rate
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2012/6/15
*****************************************************************************/  
int  AVLTunerGetBER_PER(Avl6211_PortIndex_t ePortIndex, U32 *pu32BerCnt, U32 *pu32Expo)
{
    AVL_DVBSx_ErrorCode uRet = AVL_DVBSx_EC_OK;
    AVL_uint32 u32BER = 0;

    if (s_6211demodState[ePortIndex] == 2) // s2
    {
        uRet = Avl6211_GetPacketErrorRate(ePortIndex, &s_Avl6211Chip[ePortIndex], &u32BER); 
    }
    else
    {
        uRet = Avl6211_GetBitErrorRate(ePortIndex, &s_Avl6211Chip[ePortIndex], &u32BER);
    }
    
    //printf("==>[%s] u32BER:%d.\r\n", __FUNCTION__, u32BER);
    
    GetBerValueExpo(u32BER, pu32BerCnt, pu32Expo);

    return ( (uRet == AVL_DVBSx_EC_OK) ? AVL_SUCCESS : AVL_ERROR );
}

/*****************************************************************************
  Function:         AVLTunerGetSignalQuality
  Description:      Get the signal quality
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2012/6/15
*****************************************************************************/  
int  AVLTunerGetSignalQuality(Avl6211_PortIndex_t ePortIndex, int  *pnSignalQuality)
{
    AVL_DVBSx_ErrorCode   uRet;

    uRet = Avl6211_GetSignalQuality(ePortIndex, &s_Avl6211Chip[ePortIndex], (AVL_int32 *)pnSignalQuality);

    return ( (uRet == AVL_DVBSx_EC_OK) ? AVL_SUCCESS : AVL_ERROR );
}

/*****************************************************************************
  Function:         AVLTunerGetSignalStrength
  Description:      Get the tuner signal strength
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2012/6/15
*****************************************************************************/  
int  AVLTunerGetSignalStrength(Avl6211_PortIndex_t ePortIndex, int  *pnSignalStrength)
{
    AVL_DVBSx_ErrorCode   uRet;

    uRet = Avl6211_GetSignalStrength(ePortIndex, &s_Avl6211Chip[ePortIndex], (AVL_int32 *)pnSignalStrength);

    return ( (uRet == AVL_DVBSx_EC_OK) ? AVL_SUCCESS : AVL_ERROR );
}


#if 0
/*****************************************************************************
  Function:         AVLTunerGetChipId
  Description:      Get the tuner chip id
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2012/6/15
*****************************************************************************/  
int  AVLTunerGetChipId(Avl6211_PortIndex_t ePortIndex)
{
    AVL_DVBSx_ErrorCode   uRet;

    AVL_uint32 u32RegAddr = 0x006C0030;
    AVL_uint32 u32RegData = 0;//0xFFFFFFFF;

    uRet = AVL_DVBSx_II2C_Read32(&s_Avl6211Chip, u32RegAddr, &u32RegData);
    if (uRet != AVL_DVBSx_EC_OK)
    {
        printf("[AVLTunerGetChipId]AVL_DVBSx_II2C_Read32 fail\r\n");
    }
    else
    {
        printf("[AVLTunerGetChipId]Check chipID:0x%08X.\r\n", u32RegData);
    }
}
#endif


/*****************************************************************************
  Function:         TMGetTunerStatus
  Description:      Get the tuner chip id
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2012/6/15
*****************************************************************************/  
BOOL AVLTunerGetTunerStatus(Avl6211_PortIndex_t ePortIndex)
{
    //hawayi 2012.11.10
    AVL_uchar u8TryTimes = 3;
    AVL_uint16 u16LockStatus = 0;
    AVL_DVBSx_ErrorCode r = AVL_DVBSx_EC_OK;

    do
    {
        //This function should be called to check the lock status of the demod.
        r = AVL_DVBSx_IRx_GetLockStatus(ePortIndex, &u16LockStatus, &s_Avl6211Chip[ePortIndex]);
        if ((AVL_DVBSx_EC_OK == r) && (1 == u16LockStatus))
        {
            break;
        }
        AVL_DVBSx_IBSP_Delay(10); //Wait 10ms for demod to lock the channel.
    }while(--u8TryTimes);

    return (1 == u16LockStatus) ? TRUE : FALSE;
}

/*****************************************************************************
  Function:         Av16211_SetLNB22K
  Description:      set LNB 22K
  Input:            
  Output:           
  Return:           
  Others:           
  Create:       2012/6/15
*****************************************************************************/  

int  Av16211_SetLNB22K(int PortIndex ,unsigned char  ucLNB22KEnable)
{

    if( PortIndex < 0 || PortIndex > 3)
    {
        return AVL_ERROR;
    }
    
    printf("[Av16211_SetLNB22K] tuner index: %d \r\n",PortIndex);
    if(ucLNB22KEnable == 0)    //off
    {
        AVL_DVBSx_IDiseqc_StopContinuous(PortIndex, &s_Avl6211Chip[PortIndex]);
    }

    else if(ucLNB22KEnable == 1)  //on
    {
        AVL_DVBSx_IDiseqc_StartContinuous(PortIndex, &s_Avl6211Chip[PortIndex]);
    }

    return AVL_SUCCESS;
}


int AVL6211_GetDemodState(int PortIndex)
{
    return s_6211demodState[PortIndex]; 
}

