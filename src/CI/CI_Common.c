
#if 0
/******************************************************************************
 * CI_Common.c - CI_Common
 * 
 * Copyright 2008-2013 HuiZhou WELLAV Technology Co.,Ltd.
 * 
 * DESCRIPTION: - 
 *      For all the outside api coversions...
 * modification history
 * --------------------
 * 10 Sep 2008, shouquan.tang written
 * --------------------
 ******************************************************************************/

/****************************************************************************
 * Include files
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "CI_BaseType.h"
#include "CI_Common.h"


#ifdef PLATFORM_IRD_5105


#include "WV_OS.h"

/****************************************************************************
 * Marco
 ****************************************************************************/
#define I2C_DeviceName              "I2C_BACK"

/****************************************************************************
 * Varibles
 ****************************************************************************/
// extern 
extern ST_Partition_t      *SystemPartition;
// gobal
// static 
static BOOL                 s_bIsI2CInited = FALSE;
static HWVHANDLE            s_hI2CHandle = NULL; 
static HWVHANDLE            s_hPIOHandle = NULL;
 char		                s_CIstrOutBuff[1024];
 char                       s_CIstrOutBuffExt[1024]; 

/****************************************************************************
 *                                                                          *
 *                      -- DVB-CI-CMM API Functions --                      *
 *                                                                          *
 ****************************************************************************/

// extern 
extern ST_Partition_t *GetSystemPartition(void);

// static
static DWORD CI_ConvertMilliSeconds2Ticks (DWORD dwMilliSeconds)
{
	DWORD TicksPerSec = ST_GetClocksPerSecond();
	
	return time_plus ( time_now(), (clock_t)(dwMilliSeconds * (TicksPerSec / 1000)));
}

#if 0
#endif

//////////////////////////////////////////////////////////////////////////////
//
//  MEMORY
//
//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * CI_OSMalloc() - Malloc mem
 * DESCRIPTION: - 
 *     Malloc mem 
 * Input  : 
 * Output : 
 * Returns: 
 * 
 * modification history
 * --------------------
 * 10 Sep 2008, shouquan.tang written
 * --------------------
 ******************************************************************************/
void* CI_OSMalloc( DWORD dwMemorySize )
{
    return (void *)WV_OSMalloc(dwMemorySize);
}


/******************************************************************************
 * CI_OSFree() - Free meme
 * DESCRIPTION: - 
 *     Free mem... 
 * Input  : 
 * Output : 
 * Returns: 
 * 
 * modification history
 * --------------------
 * 10 Sep 2008, shouquan.tang written
 * --------------------
 ******************************************************************************/
BOOL CI_OSFree( void* pvMemoryAddr )
{
    return WV_OSFree(pvMemoryAddr);
}


//////////////////////////////////////////////////////////////////////////////
//
//  TASK
//
//////////////////////////////////////////////////////////////////////////////


/*****************************************************************************
 * Function   : CI_OSThreadCreate() - TASK CREATE
 * DESCRIPTION: - 
 *    Task create, for the IR 
 * Input      : 
 * Output     : 
 * Returns    : 
 * 
 * modification history
 * --------------------
 * 01a, 18 DEC 2007, shouquan.tang written
 * --------------------
 *****************************************************************************/
HWVHANDLE CI_OSThreadCreate(const char* pstrName,int nPriority, int nStackSize, CIThreadEntry* pThreadEntry,void* pvArg, DWORD* pdwThreadID)
{
    
	return WV_OSThreadCreate(    pstrName,
                                nPriority,
                                nStackSize,
                                (CIThreadEntry *) pThreadEntry,
                                (void *) pvArg,
                                pdwThreadID);
    
}

void  CI_OSSleep(DWORD dwMilliSeconds)
{
    WV_OSSleep(dwMilliSeconds);
}


//////////////////////////////////////////////////////////////////////////////
//
//  Semaphore
//
//////////////////////////////////////////////////////////////////////////////
HWVHANDLE CI_OSSemaphoreCreate(const char* pstrName,int nInitialCount,int nMaxCount)
{
    return WV_OSSemaphoreCreate( pstrName,
                                nInitialCount,
                                nMaxCount);
}

DWORD CI_OSSemaphoreWaitFor(HWVHANDLE hSemaphore, DWORD dwTimeout)
{
    return WV_OSSemaphoreWaitFor(hSemaphore, dwTimeout);
}

DWORD CI_OSSemaphoreRelease(HWVHANDLE hSemaphore, int nReleaseCount)
{
	return WV_OSSemaphoreRelease(hSemaphore, nReleaseCount);
}

DWORD CI_OSSemaphoreDestory(HWVHANDLE hSemaphore)
{
    return WV_OSSemaphoreDestory(hSemaphore);
}


//////////////////////////////////////////////////////////////////////////////
//
//  Message
//
//////////////////////////////////////////////////////////////////////////////

DWORD CI_OSMsgQueueCreate(const char * pstrName,int nMaxMsgs,int nMsgLen,DWORD dwOptions)
{
    return WV_OSMsgQueueCreate(pstrName, nMaxMsgs, nMsgLen, dwOptions);
}


DWORD CI_OSMsgQueueDestroy(HWVHANDLE hMsgQueue)
{
	DWORD dwRet = CI_FAILURE;
	
	////WVASSERT(hMsgQueue != NULL);

	if (hMsgQueue != 0)
	{
		message_delete_queue ((message_queue_t*)hMsgQueue);
		dwRet = CI_SUCCESS;
	}
	
	////WVASSERT(dwRet == CI_SUCCESS);

	return dwRet;
}

DWORD CI_OSMsgQueueReceiveMsg(HWVHANDLE hMsgQueue,void* pvMsg,int nMaxMsgBytes,DWORD dwTimeout)
{
	DWORD	dwRet = CI_ERROR_TIMEOUT;	
	
	////WVASSERT(hMsgQueue != NULL);

	if (hMsgQueue != 0)
	{		
		void*	pTemp = NULL;

		if (dwTimeout == 0)
		{
			pTemp = message_receive_timeout ((message_queue_t*)hMsgQueue, TIMEOUT_IMMEDIATE);
		}
		else if (dwTimeout != 0xffffffff)
		{
			clock_t time = (clock_t)CI_ConvertMilliSeconds2Ticks(dwTimeout);				
			pTemp = message_receive_timeout ((message_queue_t*)hMsgQueue, &time);
		}
		else
		{
			pTemp = message_receive_timeout ((message_queue_t*)hMsgQueue, TIMEOUT_INFINITY);
			if (pTemp == NULL)
			{
				dwRet = CI_FAILURE;
			}				
		}

		if (pTemp != NULL)
		{
			memcpy(pvMsg, pTemp, nMaxMsgBytes);
			message_release ((message_queue_t*)hMsgQueue, pTemp);

			dwRet = CI_SUCCESS;
		}		
	}
	
	////WVASSERT(dwRet != CI_FAILURE);

	return dwRet;
}

DWORD CI_OSMsgQueueSendMsg(HWVHANDLE hMsgQueue, const void* pvMsg, int nMsgBytes, DWORD dwTimeout)
{
	DWORD	dwRet = CI_ERROR_TIMEOUT;

	////WVASSERT(hMsgQueue != 0);

	if (hMsgQueue != 0)
	{
		char *buf = NULL;			

		if (dwTimeout == 0)
		{
			buf = (char*) message_claim_timeout((message_queue_t*)hMsgQueue, TIMEOUT_IMMEDIATE);
		}
		else if(dwTimeout == 0xffffffff)
		{
			buf = (char*) message_claim_timeout((message_queue_t*)hMsgQueue, TIMEOUT_INFINITY);
			if (buf == NULL)
			{
				dwRet = CI_FAILURE;
			}				
		}
		else
		{
			clock_t time = (clock_t)CI_ConvertMilliSeconds2Ticks(dwTimeout);
			buf = (char*) message_claim_timeout((message_queue_t*)hMsgQueue, &time);			
		}		
		
		if (buf != NULL)
		{
			memcpy (buf, pvMsg, nMsgBytes * sizeof (BYTE));
			message_send ((message_queue_t*)hMsgQueue, (void *) buf);
			
			dwRet = CI_SUCCESS;
		}
	}
	
	////WVASSERT(dwRet != CI_FAILURE);

	return dwRet;
}





//////////////////////////////////////////////////////////////////////////////
//
//  I2C : (Just for the CIMax...)
//
//////////////////////////////////////////////////////////////////////////////

static HWVHANDLE    CIMAX_I2CHandle = NULL;

CI_ErrorCode_t CI_I2CInitialize(int  nIndex)
{
    
    return CI_SUCCESS;
}


CI_ErrorCode_t CI_I2COpen(WORD wSlaveAddress)
{
    // vars
    ST_ErrorCode_t      ST_Error = ST_NO_ERROR;
    STI2C_OpenParams_t  tOpenParams;

    // check params
    if (wSlaveAddress == NULL)
    {
        ERROR_Print(("[CI_I2COpen] BAD PARAMS!!!!\r\n"));
        return CI_ERROR_PARAMS;
    }
    
    /* setup params */
    tOpenParams.AddressType         = STI2C_ADDRESS_7_BITS;
    tOpenParams.BaudRate            = STI2C_RATE_NORMAL;
    tOpenParams.BusAccessTimeOut    = STI2C_TIMEOUT_INFINITY;
    tOpenParams.I2cAddress          = (U16)wSlaveAddress;
    
    ST_Error = STI2C_Open(I2C_DeviceName, &tOpenParams, (STI2C_Handle_t *)&s_hI2CHandle);
    if (ST_Error != ST_NO_ERROR)
    {
        ERROR_Print(("[CI_I2COpen] STI2C_Open() ERROR(%08X)!!!!\r\n", ST_Error));
        return CI_FAILURE;
    }


    return CI_SUCCESS;
}


/******************************************************************************
 * WVI2CClose() - I2C API - Close
 * DESCRIPTION: - 
 *          关闭I2C （卸载从设备）
 * Input  : hI2CHandle                          操作句柄
 * Output : None
 * Returns: WVI2C_SUCCESS                       操作成功
            WVI2C_FAILURE                       操作失败
 * 
 * modification history
 * --------------------
 * 05 Mar 2008, shouquan.tang written
 * --------------------
 ******************************************************************************/
CI_ErrorCode_t CI_I2CClose(void)
{
    // vars
    ST_ErrorCode_t      ST_Error = ST_NO_ERROR;
    
    ST_Error = STI2C_Close((STI2C_Handle_t)s_hI2CHandle);
    if (ST_Error != ST_NO_ERROR)
    {
        ERROR_Print(("[CI_I2COpen] STI2C_Close() ERROR(%08X)!!!!\r\n", ST_Error));
        return CI_FAILURE;
    }

    return CI_SUCCESS;
}



CI_ErrorCode_t CI_I2CWriteWithStop(BYTE ucSubAddress, BYTE *pbBuffer, DWORD dwDataLength )
{
    // vars 
    ST_ErrorCode_t      ST_Error = ST_NO_ERROR;
    U32                 ActLength = 0;
    BYTE                *pucBuff = NULL;

    // check params
    if ( pbBuffer == NULL || dwDataLength < 1)
    {
        ERROR_Print(("[CI_I2CWriteWithStop] BAD PARAMS!!!!\r\n"));
        return CI_ERROR_PARAMS;
    }

    if (s_hI2CHandle == NULL)
    {
        ERROR_Print(("[CI_I2CWriteWithStop] NO OPEN ERROR!!!!\r\n"));
        return CI_ERROR_PARAMS;
    }

    // mem
    pucBuff = CI_OSMalloc(dwDataLength+1);
    if (pucBuff == NULL)
    {
        ERROR_Print(("[CI_I2CWriteWithStop] MEM ERROR!!!!\r\n"));
        return CI_ERROR_MEM;
    }

    pucBuff[0] = ucSubAddress;
    memcpy(&pucBuff[1], pbBuffer, (size_t)dwDataLength);
    
    // action
    ST_Error = STI2C_Write((STI2C_Handle_t)s_hI2CHandle, 
                           pucBuff, 
                           dwDataLength+1, 
                           (U32)CI_I2C_TIMEOUT, 
                           &ActLength);
    if (ST_Error != ST_NO_ERROR         ||
        ActLength != dwDataLength+1)
    {
        CI_OSFree(pucBuff);
        ERROR_Print(("[CI_I2CWriteWithStop] WRITE ACTION ERROR(%X)!!!!\r\n", ST_Error));
        return CI_FAILURE;
    }

    CI_OSFree(pucBuff);
    return CI_SUCCESS;
}


CI_ErrorCode_t CI_I2CReadWithStop( BYTE ucSubAddress, BYTE *pbBuffer,DWORD dwDataLength )
{
    // vars 
    ST_ErrorCode_t      ST_Error = ST_NO_ERROR;
    U32                 ActLength = 0;
    BYTE                ucTempSubAddress[1];

    // check params
    if ( pbBuffer == NULL || dwDataLength < 1)
    {
        return CI_ERROR_PARAMS;
    }

    if (s_hI2CHandle == NULL)
    {
        return CI_ERROR_PARAMS;
    }

    ucTempSubAddress[0] = ucSubAddress;
    ST_Error = STI2C_Write((STI2C_Handle_t)s_hI2CHandle,
                            ucTempSubAddress,
                            1,
                            (U32)CI_I2C_TIMEOUT,
                            &ActLength);
    if (ST_Error != ST_NO_ERROR         ||
        ActLength != 1)
    {
        ERROR_Print(("[CI_I2CReadWithStop] READ ACTION1 ERROR(%08X)!!!!\r\n", ST_Error));
        return CI_FAILURE;
    }

    ////CI_OSSleep(1);
    ST_Error = STI2C_Read((STI2C_Handle_t)s_hI2CHandle, 
                          pbBuffer, 
                          dwDataLength, 
                          (U32)CI_I2C_TIMEOUT, 
                          &ActLength);
    if (ST_Error != ST_NO_ERROR         ||
        ActLength != dwDataLength)
    {
        ERROR_Print(("[CI_I2CReadWithStop] READ ACTION2 ERROR(%08X)!!!!\r\n", ST_Error));
        return CI_FAILURE;
    }

    return CI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//
//  GPIO : (Just for the CIMax...)
//
//////////////////////////////////////////////////////////////////////////////
CI_ErrorCode_t CI_SetPIORestBit(void)
{
    // vars
    ST_ErrorCode_t          ST_Error = ST_NO_ERROR;
    STPIO_OpenParams_t      tPIOOpenParams;
    STPIO_Handle_t          hPIOHandle;
    
    //
    tPIOOpenParams.IntHandler = NULL;
    tPIOOpenParams.ReservedBits = PIO_BIT_4;
    tPIOOpenParams.BitConfigure[4] = STPIO_BIT_OUTPUT;
    //
    ST_Error = STPIO_Open("PIO2", &tPIOOpenParams,&hPIOHandle);
    if (ST_Error != ST_NO_ERROR)
    {
        ERROR_Print(("[CI_SetPIORestBit] STPIO_Open() ERROR(%08X)!!!!\r\n", ST_Error));
        return CI_FAILURE;
    }

    ST_Error = STPIO_Write(hPIOHandle, 0xFF);    
    if (ST_Error != ST_NO_ERROR)
    {
        ERROR_Print(("[CI_SetPIORestBit] STPIO_Write() ERROR(%08X)!!!!\r\n", ST_Error));
        return CI_FAILURE;
    }
    
    CI_OSSleep(2);           // current set 1 ms  reset time  >= ...?
    
    ST_Error = STPIO_Write(hPIOHandle, 0x00);
    if (ST_Error != ST_NO_ERROR)
    {
        ERROR_Print(("[CI_SetPIORestBit] PIO RESET BIT ERROR(%08X)!!!!\r\n", ST_Error));
        return CI_FAILURE;
    }
    
    return CI_SUCCESS;
}



// We are enable the CIMax interrupt ...
CI_ErrorCode_t CI_SetPIOforINT(pfnCI_PIOPinISR pfnISR)
{
    // vars
    STPIO_OpenParams_t      tPIOOpenParams;
    STPIO_Compare_t         tPIOComparams;
    ST_ErrorCode_t          ST_Error = ST_NO_ERROR;
    
    // check params
    if (pfnISR == NULL)
    {
        return CI_FAILURE;
    }

    // PIO2_3
    tPIOOpenParams.ReservedBits     = PIO_BIT_3;
    tPIOOpenParams.IntHandler       = pfnISR;
    tPIOOpenParams.BitConfigure[3]  = STPIO_BIT_INPUT;

    ST_Error = STPIO_Open("PIO2", &tPIOOpenParams, &s_hPIOHandle);
    if (ST_Error != ST_NO_ERROR)
    {
        ERROR_Print(("[CI_SetPIOforINT] STPIO_Open() ERROR(0x%08X)", ST_Error));
        return CI_FAILURE;
    }

    tPIOComparams.CompareEnable     = PIO_BIT_3;
    tPIOComparams.ComparePattern    = 0x08; // 0000 1000  
    ST_Error = STPIO_SetCompare(s_hPIOHandle, &tPIOComparams);
    if (ST_Error != ST_NO_ERROR)
    {
        ERROR_Print(("\r\n[CI_SetPIOforINT] STPIO_SetCompare() ERROR(0x%08X)", ST_Error));
        return CI_FAILURE;
    }
    
    return CI_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
//
//  SYS_IRQ: FOR MODULE DECTECT
//
//////////////////////////////////////////////////////////////////////////////
#include "stos.h"

int CI_SetSysIRQINT(pfnCI_SYSIRQISR pfnSYSIRQISR)
{
    // VARS
    int     nRet = 0;

    // CHECK PARAMS
    if (pfnSYSIRQISR == NULL)
    {
        printf ("\r\n[CI_SetSysIRQINT] BAD PARAMS!!!!\r\n");
        return CI_FAILURE;
    }
    return CI_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
//
// FMI:
//
//////////////////////////////////////////////////////////////////////////////
CI_ErrorCode_t CI_FMISetup(void)
{
        // vars
#define CI_FMI_CONFIG_BASE_ADDRESS       0x20200000
#define CI_FMI_BUFF_BASE_ADDRESS         0x202FF800

#define CI_FMI_GENCFG                   CI_FMI_CONFIG_BASE_ADDRESS+0x0028

#define CI_BANK0_TOP_ADDRESS            CI_FMI_BUFF_BASE_ADDRESS+0x0020
#define CI_BANK1_TOP_ADDRESS            CI_FMI_BUFF_BASE_ADDRESS+0x0030
#define CI_BANK2_TOP_ADDRESS            CI_FMI_BUFF_BASE_ADDRESS+0x0040
#define CI_BANK3_TOP_ADDRESS            CI_FMI_BUFF_BASE_ADDRESS+0x0050
#define CI_FMI_CLKENABLE                CI_FMI_CONFIG_BASE_ADDRESS+0x0068

    U32 dwValue = 0x00;

    dwValue = 0x60000000;
    STSYS_WriteRegDev32LE(CI_FMI_GENCFG, dwValue);
    dwValue = STSYS_ReadRegDev32LE(CI_FMI_GENCFG);
    printf ("\n[CI_FMISetup] CI_FMI_GENCFG(0x%08X)\n", dwValue);

    dwValue = 0x00;
    dwValue = STSYS_ReadRegDev32LE(CI_BANK0_TOP_ADDRESS);
    printf ("\n[CI_FMISetup] CI_BANK0_TOP_ADDRESS(0x%08X)\n", dwValue);
    
    dwValue = STSYS_ReadRegDev32LE(CI_BANK1_TOP_ADDRESS);
    printf ("\n[CI_FMISetup] CI_BANK1_TOP_ADDRESS(0x%08X)\n", dwValue);

    dwValue = STSYS_ReadRegDev32LE(CI_BANK2_TOP_ADDRESS);
    printf ("\n[CI_FMISetup] CI_BANK2_TOP_ADDRESS(0x%08X)\n", dwValue);
    
    dwValue = STSYS_ReadRegDev32LE(CI_BANK3_TOP_ADDRESS);
    printf ("\n[CI_FMISetup] CI_BANK3_TOP_ADDRESS(0x%08X)\n", dwValue);
    
    //dwValue = 0xFFFFFFFF;
    //STSYS_WriteRegDev32LE(CI_FMI_CLKENABLE, dwValue);
    //dwValue = STSYS_ReadRegDev32LE(CI_FMI_CLKENABLE);
    //printf ("\n[CI_FMISetup] CI_FMI_CLKENABLE(0x%08X)\n", dwValue);
    
    return CI_SUCCESS;
} 
 

//////////////////////////////////////////////////////////////////////////////
//
//  DEBUG
//
//////////////////////////////////////////////////////////////////////////////

#endif // #ifdef PLATFORM_IRD_5105




#ifdef PLATFORM_IRD_7109 
/****************************************************************************
 * Include files
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "stcommon.h"

#include "WV_Typedef.h"
#include "WV_HDI_Typedef.h"
#include "WV_SW_CommonType.h"
#include "WV_I2C.h"
#include "WV_GPIO.h"
#include "CI_Common.h"
#include "WV_HTRACE.h"


/****************************************************************************
 * Marco
 ****************************************************************************/
#define I2C_DeviceName              "I2C_BACK"


/****************************************************************************
 * Varibles
 ****************************************************************************/
// extern 
extern ST_Partition_t      *SystemPartition;
// gobal
// static 
//static BOOL                 s_bIsI2CInited = FALSE;
//static HWVHANDLE            s_hI2CHandle = NULL; 
//static HWVHANDLE            s_hPIOHandle = NULL;
 char		            s_CIstrOutBuff[1024];
 char                 s_CIstrOutBuffExt[1024]; 

/****************************************************************************
 *                                                                          *
 *                      -- DVB-CI-CMM API Functions --                      *
 *                                                                          *
 ****************************************************************************/

// extern 
extern ST_Partition_t *GetSystemPartition(void);

// static
static DWORD CI_ConvertMilliSeconds2Ticks (DWORD dwMilliSeconds)
{
	DWORD TicksPerSec = ST_GetClocksPerSecond();
	
	return time_plus ( time_now(), (clock_t)(dwMilliSeconds * (TicksPerSec / 1000)));
}

#if 0
#endif

//////////////////////////////////////////////////////////////////////////////
//
//  MEMORY
//
//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * CI_OSMalloc() - Malloc mem
 * DESCRIPTION: - 
 *     Malloc mem 
 * Input  : 
 * Output : 
 * Returns: 
 * 
 * modification history
 * --------------------
 * 10 Sep 2008, shouquan.tang written
 * --------------------
 ******************************************************************************/
void* CI_OSMalloc( DWORD dwMemorySize )
{
    return (void *)WVMalloc(dwMemorySize);
}


/******************************************************************************
 * CI_OSFree() - Free meme
 * DESCRIPTION: - 
 *     Free mem... 
 * Input  : 
 * Output : 
 * Returns: 
 * 
 * modification history
 * --------------------
 * 10 Sep 2008, shouquan.tang written
 * --------------------
 ******************************************************************************/
BOOL CI_OSFree( void* pvMemoryAddr )
{
    return WVFree(pvMemoryAddr);
}


//////////////////////////////////////////////////////////////////////////////
//
//  TASK
//
//////////////////////////////////////////////////////////////////////////////


/*****************************************************************************
 * Function   : CI_OSThreadCreate() - TASK CREATE
 * DESCRIPTION: - 
 *    Task create, for the IR 
 * Input      : 
 * Output     : 
 * Returns    : 
 * 
 * modification history
 * --------------------
 * 01a, 18 DEC 2007, shouquan.tang written
 * --------------------
 *****************************************************************************/
HWVHANDLE CI_OSThreadCreate(const char* pstrName,int nPriority, int nStackSize, CIThreadEntry* pThreadEntry,void* pvArg, DWORD* pdwThreadID)
{
    
	return WVOSThreadCreate(    pstrName,
                                nPriority,
                                nStackSize,
                                (CIThreadEntry *) pThreadEntry,
                                (void *) pvArg,
                                pdwThreadID);
    
}

void  CI_OSSleep(DWORD dwMilliSeconds)
{
    WVSleep(dwMilliSeconds);
}


//////////////////////////////////////////////////////////////////////////////
//
//  MsgQueue
//
//////////////////////////////////////////////////////////////////////////////
HWVHANDLE CI_OSSemaphoreCreate(const char* pstrName,int nInitialCount,int nMaxCount)
{
    return WVOSSemaphoreCreate( pstrName,
                                nInitialCount,
                                nMaxCount);
}

DWORD CI_OSSemaphoreWaitFor(HWVHANDLE hSemaphore, DWORD dwTimeout)
{
    return WVOSSemaphoreWaitFor(hSemaphore, dwTimeout);
}

DWORD CI_OSSemaphoreRelease(HWVHANDLE hSemaphore, int nReleaseCount)
{
	return WVOSSemaphoreRelease(hSemaphore, nReleaseCount);
}

DWORD CI_OSSemaphoreDestory(HWVHANDLE hSemaphore)
{
    return WVOSSemaphoreDestory(hSemaphore);
}




//////////////////////////////////////////////////////////////////////////////
//
//  Message
//
//////////////////////////////////////////////////////////////////////////////

DWORD CI_OSMsgQueueCreate(const char * pstrName,int nMaxMsgs,int nMsgLen,DWORD dwOptions)
{
    return WVOSMsgQueueCreate(pstrName, nMaxMsgs, nMsgLen, dwOptions);
}


DWORD CI_OSMsgQueueDestroy(HWVHANDLE hMsgQueue)
{
	DWORD dwRet = CI_FAILURE;
	
	////HTRACE_ASSERT(hMsgQueue != NULL);

	if (hMsgQueue != 0)
	{
		message_delete_queue ((message_queue_t*)hMsgQueue);
		dwRet = CI_SUCCESS;
	}
	
	////HTRACE_ASSERT(dwRet == CI_SUCCESS);

	return dwRet;
}

DWORD CI_OSMsgQueueReceiveMsg(HWVHANDLE hMsgQueue,void* pvMsg,int nMaxMsgBytes,DWORD dwTimeout)
{
	DWORD	dwRet = CI_ERROR_TIMEOUT;	
	
	////HTRACE_ASSERT(hMsgQueue != NULL);

	if (hMsgQueue != 0)
	{		
		void*	pTemp = NULL;

		if (dwTimeout == 0)
		{
			pTemp = message_receive_timeout ((message_queue_t*)hMsgQueue, TIMEOUT_IMMEDIATE);
		}
		else if (dwTimeout != 0xffffffff)
		{
			clock_t time = (clock_t)CI_ConvertMilliSeconds2Ticks(dwTimeout);				
			pTemp = message_receive_timeout ((message_queue_t*)hMsgQueue, &time);
		}
		else
		{
			pTemp = message_receive_timeout ((message_queue_t*)hMsgQueue, TIMEOUT_INFINITY);
			if (pTemp == NULL)
			{
				dwRet = CI_FAILURE;
			}				
		}

		if (pTemp != NULL)
		{
			memcpy(pvMsg, pTemp, nMaxMsgBytes);
			message_release ((message_queue_t*)hMsgQueue, pTemp);

			dwRet = CI_SUCCESS;
		}		
	}
	
	////HTRACE_ASSERT(dwRet != CI_FAILURE);

	return dwRet;
}

DWORD CI_OSMsgQueueSendMsg(HWVHANDLE hMsgQueue, const void* pvMsg, int nMsgBytes, DWORD dwTimeout)
{
	DWORD	dwRet = CI_ERROR_TIMEOUT;

	////HTRACE_ASSERT(hMsgQueue != 0);

	if (hMsgQueue != 0)
	{
		char *buf = NULL;			

		if (dwTimeout == 0)
		{
			buf = (char*) message_claim_timeout((message_queue_t*)hMsgQueue, TIMEOUT_IMMEDIATE);
		}
		else if(dwTimeout == 0xffffffff)
		{
			buf = (char*) message_claim_timeout((message_queue_t*)hMsgQueue, TIMEOUT_INFINITY);
			if (buf == NULL)
			{
				dwRet = CI_FAILURE;
			}				
		}
		else
		{
			clock_t time = (clock_t)CI_ConvertMilliSeconds2Ticks(dwTimeout);
			buf = (char*) message_claim_timeout((message_queue_t*)hMsgQueue, &time);			
		}		
		
		if (buf != NULL)
		{
			memcpy (buf, pvMsg, nMsgBytes * sizeof (BYTE));
			message_send ((message_queue_t*)hMsgQueue, (void *) buf);
			
			dwRet = CI_SUCCESS;
		}
	}
	
	////HTRACE_ASSERT(dwRet != CI_FAILURE);

	return dwRet;
}




//////////////////////////////////////////////////////////////////////////////
//
//  I2C : (Just for the CIMax...)
//
//////////////////////////////////////////////////////////////////////////////

static HWVHANDLE    CIMAX_I2CHandle = NULL;

CI_ErrorCode_t CI_I2CInitialize(int  nIndex)
{
    
    return CI_SUCCESS;
}


CI_ErrorCode_t CI_I2COpen(WORD wSlaveAddress)
{
    WVI2C_Error_t   WV_Error = WVI2C_SUCCESS;
    
    WV_Error = WVI2COpen(1, wSlaveAddress,&CIMAX_I2CHandle);
    //STB// WV_Error = WVI2COpen(0, wSlaveAddress,&CIMAX_I2CHandle);
    if (WV_Error != WVI2C_SUCCESS)
    {
        ERROR_Print(("CIMAX I2C Open failure!!!!\r\n"));
        return CI_FAILURE;
    }

    INFO_Print(("[CI_COMMON] CI_I2COpen() SUCCESS!!\r\n"));
    return CI_SUCCESS;
}


/******************************************************************************
 * WVI2CClose() - I2C API - Close
 * DESCRIPTION: - 
 *          关闭I2C （卸载从设备）
 * Input  : hI2CHandle                          操作句柄
 * Output : None
 * Returns: WVI2C_SUCCESS                       操作成功
            WVI2C_FAILURE                       操作失败
 * 
 * modification history
 * --------------------
 * 05 Mar 2008, shouquan.tang written
 * --------------------
 ******************************************************************************/
CI_ErrorCode_t CI_I2CClose(void)
{
    return CI_SUCCESS;
}


CI_ErrorCode_t CI_I2CWriteWithStop(BYTE ucSubAddress, BYTE *pbBuffer, DWORD dwDataLength )
{
    // VARS 
    WVI2C_Error_t       WV_Error = WVI2C_SUCCESS;
    BYTE                *pucBuff = NULL;

    // CHECK PARAMS
    if ( pbBuffer == NULL || dwDataLength < 1 || CIMAX_I2CHandle == NULL)
    {
        ERROR_Print(("[CI_COMMON][CI_I2CWriteWithStop] BAD PARAMS!!!!\r\n"));
        return CI_ERROR_PARAMS;
    }

    // MEM
    pucBuff = CI_OSMalloc(dwDataLength+1);
    if (pucBuff == NULL)
    {
        ERROR_Print(("[CI_COMMON][CI_I2CWriteWithStop] MEM ERROR!!!!\n"));
        return CI_ERROR_MEM;
    }

    pucBuff[0] = ucSubAddress;
    memcpy(&pucBuff[1], pbBuffer, (size_t)dwDataLength);
    
    // ACTION
    WV_Error = WVI2CWriteWithStop(CIMAX_I2CHandle, pucBuff, dwDataLength+1);
    if (WV_Error != WVI2C_SUCCESS)
    {
        ERROR_Print(("[CI_COMMON][CI_I2CWriteWithStop] WRITE I2C DATA ERROR(%08X)!!!!\r\n", WV_Error));
        CI_OSFree(pucBuff);
        return WV_Error;
    }
        

    CI_OSFree(pucBuff);
    return CI_SUCCESS;
}


CI_ErrorCode_t CI_I2CReadWithStop( BYTE ucSubAddress, BYTE *pbBuffer,DWORD dwDataLength )
{
    // VARS 
    WVI2C_Error_t       WV_Error = WVI2C_SUCCESS;
    BYTE                ucTempSubAddress[1];

    // CHECK PARAMS
    if ( pbBuffer == NULL || dwDataLength < 1 || CIMAX_I2CHandle == NULL)
    {
        ERROR_Print(("[CI_COMMON][CI_I2CReadWithStop] BAD PARAMS!!!!\r\n"));
        return CI_ERROR_PARAMS;
    }


    ucTempSubAddress[0] = ucSubAddress;

    WV_Error = WVI2CWriteWithStop(CIMAX_I2CHandle, ucTempSubAddress, 1);
    if (WV_Error != WVI2C_SUCCESS)
    {
        ERROR_Print(("[CI_COMMON][CI_I2CReadWithStop] WRITE I2C ERROR(%08X)\r\n", WV_Error));
        return WV_Error;
    }

    WV_Error = WVI2CReadWithStop(CIMAX_I2CHandle, pbBuffer, dwDataLength);
    if (WV_Error != WVI2C_SUCCESS)
    {
        ERROR_Print(("[CI_COMMON][CI_I2CReadWithStop] READ I2C ERROR(%08X)\r\n", WV_Error));
        return WV_Error;
    }
    
    return CI_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//
// FMI:
//
//////////////////////////////////////////////////////////////////////////////

#include "stsys.h"

CI_ErrorCode_t CI_FMISetup(void)
{
        // vars
#define CI_FMI_CONFIG_BASE_ADDRESS       0x20200000
#define CI_FMI_BUFF_BASE_ADDRESS         0x202FF800

#define CI_FMI_GENCFG                   CI_FMI_CONFIG_BASE_ADDRESS+0x0028

#define CI_BANK0_TOP_ADDRESS            CI_FMI_BUFF_BASE_ADDRESS+0x0020
#define CI_BANK1_TOP_ADDRESS            CI_FMI_BUFF_BASE_ADDRESS+0x0030
#define CI_BANK2_TOP_ADDRESS            CI_FMI_BUFF_BASE_ADDRESS+0x0040
#define CI_BANK3_TOP_ADDRESS            CI_FMI_BUFF_BASE_ADDRESS+0x0050
#define CI_FMI_CLKENABLE                CI_FMI_CONFIG_BASE_ADDRESS+0x0068

    U32 dwValue = 0x00;

    dwValue = 0x60000000;
    STSYS_WriteRegDev32LE(CI_FMI_GENCFG, dwValue);
    dwValue = STSYS_ReadRegDev32LE(CI_FMI_GENCFG);
    printf ("\n[CI_FMISetup] CI_FMI_GENCFG(0x%08X)\n", dwValue);

    dwValue = 0x00;
    dwValue = STSYS_ReadRegDev32LE(CI_BANK0_TOP_ADDRESS);
    printf ("\n[CI_FMISetup] CI_BANK0_TOP_ADDRESS(0x%08X)\n", dwValue);
    
    dwValue = STSYS_ReadRegDev32LE(CI_BANK1_TOP_ADDRESS);
    printf ("\n[CI_FMISetup] CI_BANK1_TOP_ADDRESS(0x%08X)\n", dwValue);

    dwValue = STSYS_ReadRegDev32LE(CI_BANK2_TOP_ADDRESS);
    printf ("\n[CI_FMISetup] CI_BANK2_TOP_ADDRESS(0x%08X)\n", dwValue);
    
    dwValue = STSYS_ReadRegDev32LE(CI_BANK3_TOP_ADDRESS);
    printf ("\n[CI_FMISetup] CI_BANK3_TOP_ADDRESS(0x%08X)\n", dwValue);
    
    //dwValue = 0xFFFFFFFF;
    //STSYS_WriteRegDev32LE(CI_FMI_CLKENABLE, dwValue);
    //dwValue = STSYS_ReadRegDev32LE(CI_FMI_CLKENABLE);
    //printf ("\n[CI_FMISetup] CI_FMI_CLKENABLE(0x%08X)\n", dwValue);
    
    return CI_SUCCESS;
} 


//////////////////////////////////////////////////////////////////////////////
//
//  GPIO : (Just for the CIMax...)
//
//////////////////////////////////////////////////////////////////////////////
CI_ErrorCode_t CI_SetPIORestBit(void)
{
    // HARDWARE RESET DONE
#if 1
    WVGPIOErrorType     WV_ERROR = WV_GPIO_SUCCESS;

    BYTE                ucPIOIndex = 36;

    WV_ERROR = WVGPIOOpen(ucPIOIndex, WV_GPIO_OUPUT);

    // SET LOW
    WV_ERROR = WVGPIOWrite(ucPIOIndex, 1);

    WVSleep(2);
    
    WV_ERROR = WVGPIOWrite(ucPIOIndex, 0);

    if (WV_ERROR != WV_GPIO_SUCCESS)
    {
        ERROR_Print(("[CI_SetPIORestBit] ACTION FAILURE!!"));
        return CI_FAILURE;
    }
#endif    
    
    return CI_SUCCESS;
}



// We are enable the CIMax interrupt ...
CI_ErrorCode_t CI_SetPIOforINT(pfnCI_PIOPinISR pfnISR)
{
    // VARS
    WVGPIOErrorType             WV_ERROR = WV_GPIO_SUCCESS;
    BYTE                        ucPIOIndex = 39;  // SD: 19;  HD //39;


    // CHECK PARAMS
    if (pfnISR == NULL)
    {
        return CI_ERROR_PARAMS;
    }

#if 0
// 测试硬件的PIO是否正常

    WV_ERROR = WVGPIOOpen(ucPIOIndex, WV_GPIO_OUPUT);
    if (WV_ERROR != WV_GPIO_SUCCESS)
    {
        ERROR_Print(("[CI_SetPIOforINT]OPEN PIO FAILURE!!!!(%d, 0x%X)!!!!\r\n", ucPIOIndex, WV_ERROR));
        return CI_FAILURE;
    }


// 循环写
    while (1)
    {
        task_delay(ST_GetClocksPerSecond());
        WVGPIOWrite(ucPIOIndex, 0);
        task_delay(ST_GetClocksPerSecond());
        WVGPIOWrite(ucPIOIndex, 1);

        INFO_Print(("[CI_SetPIOforINT] loop!! \r\n"));
    }

#endif
    
    WV_ERROR = WVGPIOEnableInterrupt(ucPIOIndex, WV_GPIO_INT_LOW, pfnISR);
    if (WV_ERROR != WV_GPIO_SUCCESS)
    {
        ERROR_Print(("[CI_COMMON][CI_SetPIOforINT] SET PIO INTTERRUPT FAILURE(0x%X)!!!!\r\n", WV_ERROR));
        return CI_FAILURE;
    }

    INFO_Print(("[CI_COMMON][CI_SetPIOforINT] SET PIO INTTERRUPT SUCCESS!!\r\n"));
    return CI_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
//
//  SYS_IRQ: FOR MODULE DECTECT
//
//////////////////////////////////////////////////////////////////////////////
#include "stos.h"

int CI_SetSysIRQINT(pfnCI_SYSIRQISR pfnSYSIRQISR)
{
    // VARS

    // CHECK PARAMS
    if (pfnSYSIRQISR == NULL)
    {
        printf ("\r\n[CI_SetSysIRQINT] BAD PARAMS!!!!\r\n");
        return CI_FAILURE;
    }
#if 0
    // 
    pCI_INT_HANDLE = interrupt_handle(ST7109_EXT_INT_IN0_INTERRUPT);
    if (pCI_INT_HANDLE == NULL)
    {
        printf ("\r\n[CI_SetSysIRQINT] GET HANDLE ERROR!!!!\r\n");
        return CI_FAILURE;
    }


    //
    nRet = interrupt_install(pCI_INT_HANDLE, NULL, (interrupt_handler_t)pfnSYSIRQISR, pCI_INT_HANDLE);
    if (nRet != 0)
    {
        printf ("\r\n[CI_SetSysIRQINT]INT INSTALL ERROR(%d)!!!!\r\n", nRet);
        return CI_FAILURE;
    }
#endif

#if 0// SYS ISR NOT OK RIGHT NOW
    /* Attempt to install the interrupt handler */
    STOS_InterruptLock();


    nRet = STOS_InterruptInstall(EXTERNAL_0_INTERRUPT,
                          EXTERNAL_0_INTERRUPT_LEVEL,
                          STOS_INTERRUPT_CAST(pfnSYSIRQISR),
                          EXTERNAL_0_INTERRUPT,
                          NULL);

    if (nRet != STOS_SUCCESS)
    {
        printf ("[..] STOS_InterruptInstall() ERROR (%08X)", nRet);
        return CI_FAILURE;
    }
    
    nRet = STOS_InterruptEnable(EXTERNAL_0_INTERRUPT, EXTERNAL_0_INTERRUPT_LEVEL);
    if (nRet != STOS_SUCCESS)
    {
        printf ("[..] STOS_InterruptEnable() ERROR (%08X)", nRet);
        return CI_FAILURE;
    }

    STOS_InterruptUnlock(); /* Re-enable interrupts */
#endif
    return CI_SUCCESS;
}




//////////////////////////////////////////////////////////////////////////////
//
//  DEBUG
//
//////////////////////////////////////////////////////////////////////////////
static int s_semflag = 0;

void CI_COMMONPrint(int nDebugLevel, char *format, ...)
{
    // vars
	va_list Argument;

    if (s_semflag)
    {
        return;
    }

    s_semflag = 1;

    
	va_start(Argument, format);
	vsprintf(s_CIstrOutBuff, format, Argument);
	va_end(Argument);
    
    //STTBX_Print(("%s", s_strOutBuff));
    if (nDebugLevel == ERROR_LEVEL)
    {
        HTRACE_ERROR(("%s", s_CIstrOutBuff));
    }
    else if (nDebugLevel == INFO_LEVEL)
    {
        HTRACE_INFO(("%s", s_CIstrOutBuff));
    }
    

    s_semflag = 0;
}

void CI_PrintBuff(int nDebugLevel)
{
    if (s_semflag)
    {
        return;
    }

    s_semflag = 1;

    //WVTRACE(nDebugLevel, "%s", s_CIstrOutBuffExt);
    if (nDebugLevel == ERROR_LEVEL)
    {
        HTRACE_ERROR(("%s", s_CIstrOutBuffExt));
    }
    else if (nDebugLevel == INFO_LEVEL)
    {
        HTRACE_INFO(("%s", s_CIstrOutBuffExt));
    }

    s_semflag = 0;
}


void CI_Print2String(char *format, ...)
{
    // vars    
	va_list Argument; 
    
    if (s_semflag)
    {
        return;
    }

    s_semflag = 1;
         
	va_start(Argument, format);
	vsprintf(s_CIstrOutBuffExt, format, Argument);
	va_end(Argument);    

    s_semflag = 0;
}

#endif      // PLATFORM_IRD_7109

///////////////////////
// eof

#endif 

