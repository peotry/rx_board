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



#ifndef USER_DEFINED_FUNCTION_H
#define USER_DEFINED_FUNCTION_H

#ifdef AVL_CPLUSPLUS
extern "C" {
#endif

#include "user_defined_data_type.h"

// For this user defined function, Availink has a different implementation and the parameter list isn't 'avoid'.
#if defined(AVL_I2C_DEFINE)
AVL6381_ErrorCode AVL6381_IBSP_Initialize(System::String^ strServerName0, int iSocketPort0,System::String^ strServerName1, int iSocketPort1);
#else
AVL6381_ErrorCode AVL6381_IBSP_Initialize(void);
#endif

AVL6381_ErrorCode AVL6381_IBSP_Delay(AVL_uint32 uiDelay_ms);
AVL6381_ErrorCode AVL6381_IBSP_Reset(void);
AVL6381_ErrorCode AVL6381_IBSP_I2C_Read(AVL_uint32 u32port, AVL_uint16 uiSlaveAddr,  AVL_puchar pucBuff, AVL_puint16 puiSize);
AVL6381_ErrorCode AVL6381_IBSP_I2C_Write(AVL_uint32 u32port, AVL_uint16 uiSlaveAddr,  AVL_puchar pucBuff, AVL_puint16 puiSize);
AVL6381_ErrorCode AVL6381_IBSP_Dispose(void);
AVL6381_ErrorCode AVL6381_IBSP_InitSemaphore(AVL_psemaphore pSemaphore);
AVL6381_ErrorCode AVL6381_IBSP_ReleaseSemaphore(AVL_psemaphore pSemaphore);
AVL6381_ErrorCode AVL6381_IBSP_WaitSemaphore(AVL_psemaphore pSemaphore);         

#ifdef AVL_CPLUSPLUS
}
#endif

#endif

