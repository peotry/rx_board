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
///
/// @file
/// @brief Implements the functions declared in IBSP.h. 
/// 
#include <unistd.h>

#include "IBSP.h"
#include "xilinx_i2c.h"


#define IBSP_I2C_TIMEOUT 1000


extern int i2c_write_msg(int index, unsigned char address, unsigned char *value, unsigned char len);
extern int i2c_receive_msg(int index, unsigned char address, unsigned char *value, unsigned char len);



/// The following table illustrates a set of PLL configuration values to operate AVL6211 in two modes:
// Standard performance mode.
// High performance mode

/// Please refer to the AVL6211 channel receiver datasheet for detailed information on highest symbol rate 
/// supported by the demod in both these modes.

///For more information on other supported clock frequencies and PLL settings for higher symbol rates, please 
///contact Availink.

/// Users can remove unused elements from the following array to reduce the SDK footprint size.

/* //hawayi 2012.05.16 move to file Avl6211_LockSignal.c
const struct AVL_DVBSx_PllConf pll_conf[] =
{
    // The following set of PLL configuration at different reference clock frequencies refer to demod operation
    // in standard performance mode.
     {503,  1, 7, 4, 2,  4000, 11200, 16800, 25200} ///< Reference clock 4 MHz,   Demod clock 112 MHz, FEC clock 168 MHz, MPEG clock 252 MHz
    ,{447,  1, 7, 4, 2,  4500, 11200, 16800, 25200} ///< Reference clock 4.5 MHz, Demod clock 112 MHz, FEC clock 168 MHz, MPEG clock 252 MHz
    ,{503,  4, 7, 4, 2, 10000, 11200, 16800, 25200} ///< Reference clock 10 MHz,  Demod clock 112 MHz, FEC clock 168 MHz, MPEG clock 252 MHz
    ,{503,  7, 7, 4, 2, 16000, 11200, 16800, 25200} ///< Reference clock 16 MHz,  Demod clock 112 MHz, FEC clock 168 MHz, MPEG clock 252 MHz
    ,{111,  2, 7, 4, 2, 27000, 11200, 16800, 25200} ///< Reference clock 27 MHz,  Demod clock 112 MHz, FEC clock 168 MHz, MPEG clock 252 MHz
    
    // The following set of PLL configuration at different reference clock frequencies refer to demod operation
    // in high performance mode. 
    ,{566,  1, 7, 4, 2,  4000, 12600, 18900, 28350} /// < Reference clock 4 MHz,   Demod clock 126 MHz, FEC clock 189 MHz, MPEG clock 283.5 MHz
    ,{503,  1, 7, 4, 2,  4500, 12600, 18900, 28350} ///< Reference clock 4.5 MHz, Demod clock 126 MHz, FEC clock 189 MHz, MPEG clock 283.5 MHz
    ,{566,  4, 7, 4, 2, 10000, 12600, 18900, 28350} ///< Reference clock 10 MHz,  Demod clock 126 MHz, FEC clock 189 MHz, MPEG clock 283.5 MHz
    ,{566,  7, 7, 4, 2, 16000, 12600, 18900, 28350} ///< Reference clock 16 MHz,  Demod clock 126 MHz, FEC clock 189 MHz, MPEG clock 283.5 MHz
    ,{377,  8, 7, 4, 2, 27000, 12600, 18900, 28350} ///< Reference clock 27 MHz,  Demod clock 126 MHz, FEC clock 189 MHz, MPEG clock 283.5 MHz

};

const AVL_uint16 pll_array_size = sizeof(pll_conf)/sizeof(struct AVL_DVBSx_PllConf);
*/

AVL_DVBSx_ErrorCode AVL_DVBSx_IBSP_Initialize(void)
{
    return(AVL_DVBSx_EC_OK);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBSP_Dispose(void)
{
    return(AVL_DVBSx_EC_OK);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBSP_Delay( AVL_uint32 uiMS )
{
    usleep(uiMS*1000);
    
    return(AVL_DVBSx_EC_OK);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBSP_I2COpen(WVHDII2CIndex nIndex, AVL_uint16 uiSlaveAddr)
{
    return(AVL_DVBSx_EC_OK);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBSP_I2CRead(AVL_uint32 u32Port, const struct AVL_DVBSx_Chip * pAVLChip, AVL_puchar pucBuff, AVL_puint16 puiSize)
{
    AVL_int32          uiSize = (AVL_int32)*puiSize;

    if((0 == u32Port)||(1 == u32Port))
    {
        i2c_receive_msg(I2C_0, pAVLChip->m_SlaveAddr, pucBuff, uiSize);
    }
    else
    {
        i2c_receive_msg(I2C_1, pAVLChip->m_SlaveAddr, pucBuff, uiSize);
    }
    return(AVL_DVBSx_EC_OK);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBSP_I2CWrite(AVL_uint32 u32Port, const struct AVL_DVBSx_Chip * pAVLChip, AVL_puchar pucBuff, AVL_puint16 puiSize)
{
    AVL_int32          uiSize = (AVL_int32)*puiSize;
    
    if((0 == u32Port)||(1 == u32Port))
    {
        i2c_write_msg(I2C_0, pAVLChip->m_SlaveAddr, pucBuff, uiSize);
    }
    else
    {
        i2c_write_msg(I2C_1, pAVLChip->m_SlaveAddr, pucBuff, uiSize);   
    }
    return(AVL_DVBSx_EC_OK);
}


AVL_DVBSx_ErrorCode AVL_DVBSx_IBSP_InitSemaphore(AVL_psemaphore pAVLSemaphore)
{
    //AVL_int32 state = AVL_DVBSx_EC_OK;
  /*  
    state = sem_init(pAVLSemaphore,0,1);
    if(state != 0)
    {
        return(AVL_DVBSx_EC_GeneralFail);
    }
    
    //printf("[%s] OK!\n", __FUNCTION__);
*/
    return(AVL_DVBSx_EC_OK);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBSP_WaitSemaphore(AVL_psemaphore pAVLSemaphore)
{
    //AVL_int32 state = AVL_DVBSx_EC_OK;
  /*  
    state = sem_wait(pAVLSemaphore);
    if(state != 0)
    {
        return AVL_DVBSx_EC_GeneralFail;
    }
 */
    return(AVL_DVBSx_EC_OK);
}

AVL_DVBSx_ErrorCode AVL_DVBSx_IBSP_ReleaseSemaphore(AVL_psemaphore pAVLSemaphore)
{
    //AVL_int32 state = AVL_DVBSx_EC_OK;
 /*
    state = sem_post(pAVLSemaphore);

    if(state != 0)
    {
        return AVL_DVBSx_EC_GeneralFail;
    }
    */
    return(AVL_DVBSx_EC_OK);
}


