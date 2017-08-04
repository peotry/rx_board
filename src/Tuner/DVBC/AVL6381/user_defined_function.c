/*
 *           Copyright 2007-2015 Availink, Inc.
 *
 *  This software contains Availink proprietary information and
 *  its use and disclosure are restricted solely to the terms in
 *  the corresponding written license agreement. It shall not be 
 *  disclosed to anyone other than valid licensees without
 *  written permission of Availink, Inc.
 *
 */
#include <unistd.h>
#include "AVL6381_API.h"
#include "xilinx_i2c.h"
#include <semaphore.h>


#ifndef NULL
#define NULL    ((void *)0)
#endif

extern Avl6381_PortIndex_t gsPortIndex;

extern int i2c_write_msg(int index, unsigned char address, unsigned char *value, unsigned char len);
extern int i2c_receive_msg(int index, unsigned char address, unsigned char *value, unsigned char len);



AVL6381_ErrorCode AVL6381_IBSP_Reset(void)
{
    return(AVL6381_EC_OK);
}

AVL6381_ErrorCode AVL6381_IBSP_Delay(AVL_uint32 uiDelay_ms)
{

    usleep(uiDelay_ms*1000);
    
    return(AVL6381_EC_OK);
}

AVL6381_ErrorCode AVL6381_IBSP_I2C_Read(U32 u32port, AVL_uint16 usSlaveAddr,  AVL_puchar pucBuff, AVL_puint16 pusSize)
{
    AVL_int32   uiSize = (AVL_int32)*pusSize;
    
    if((0 == u32port)||(1 == u32port))
    {
        i2c_receive_msg(I2C_0, usSlaveAddr, pucBuff, uiSize);
    }
    else
    {
        i2c_receive_msg(I2C_1, usSlaveAddr, pucBuff, uiSize);
    }
   
    
    return(AVL6381_EC_OK);
}


AVL6381_ErrorCode AVL6381_IBSP_I2C_Write(U32 u32port, AVL_uint16 usSlaveAddr,  AVL_puchar pucBuff,  AVL_puint16  pusSize)
{
    AVL_int32   uiSize = (AVL_int32)*pusSize;
    S32 ret = 0;

    if((0 == u32port)||(1 == u32port))
    {
       ret = i2c_write_msg(I2C_0, usSlaveAddr, pucBuff, uiSize);
    }
    else
    {
       ret = i2c_write_msg(I2C_1, usSlaveAddr, pucBuff, uiSize);
    }

    if( ret != 0 )
    {
        return(AVL6381_EC_I2C_FAIL);
    }
    
    return(AVL6381_EC_OK);
}



AVL6381_ErrorCode AVL6381_IBSP_Initialize(void)
{
    return(AVL6381_EC_OK);
}



AVL6381_ErrorCode AVL6381_IBSP_InitSemaphore(AVL_psemaphore pSemaphore)
{
    S32 state;
    
    state = sem_init(pSemaphore,0,1);

    if(state != 0)
    {
        return AVL6381_EC_WARNING;
    }
    
    return(AVL6381_EC_OK);
}

AVL6381_ErrorCode AVL6381_IBSP_ReleaseSemaphore(AVL_psemaphore pSemaphore)
{
    S32 state;
    state = sem_post(pSemaphore);

    if(state != 0)
    {
        return AVL6381_EC_WARNING;
    }
    
    return(AVL6381_EC_OK);
}

AVL6381_ErrorCode AVL6381_IBSP_WaitSemaphore(AVL_psemaphore pSemaphore)
{
    S32 state;
    
    state = sem_wait(pSemaphore);
    if(state != 0)
    {
        return AVL6381_EC_WARNING;
    }
    
    return(AVL6381_EC_OK);
}

AVL6381_ErrorCode AVL6381_IBSP_Dispose(void)
{
    return(AVL6381_EC_OK);
}


