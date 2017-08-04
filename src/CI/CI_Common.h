
#if 0
/******************************************************************************
 * CI_Common.h - CI_Common
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

#ifndef __CI_COMMON_H_
#define __CI_COMMON_H_

#include "CI_BaseType.h"


#define CI_DEFAULT_RETRY_TIMES  (50)



/*
 *--------------------------------------------------------------------------
 * 错误常量
 *--------------------------------------------------------------------------
 */

#define CI_SUCCESS              (0)
#define CI_FAILURE              (-1)
#define CI_ERROR_PARAMS         (-2)
#define CI_ERROR_MEM            (-3)
#define CI_ERROR_COMM           (-4)
#define CI_ERROR_TIMEOUT        (-5)
#define CI_ERROR_RESOURCE_BUSY  (-6)
#define CI_ERROR_CRC            (-7)

/*---------------------------------------------------------------------------
 * 平台选择: 
 *---------------------------------------------------------------------------
 */

#ifdef PLATFORM_IRD_5105

/****************************************************************************
 * include
 ****************************************************************************/


#include <time.h>
#include <ostime.h>
#include <mutex.h>
#include <message.h>
#include <task.h>
#include <semaphor.h>
#include <stdlib.h>
#include <string.h>
#include "stcommon.h"

#include "stsys.h"
#include "sections.h"
#include "stpio.h"
#include "sti2c.h"
#include "stuart.h"


#define STTBX_PRINT
#include "sttbx.h"



/****************************************************************************
 * Macro
 ****************************************************************************/

/*******************************************************************************
 * 
 * ADD BY TOPTIMES
 * 
 *******************************************************************************/
#define ENABLE_TT_DEBUG

#ifdef ENABLE_TT_DEBUG
#define INFO_Print(x)                       HTRACE_INFO(x)
#define ERROR_Print(x)                      HTRACE_ERROR(x)
#define DEBUG_Print(x)                      HTRACE_INFO(x)
#define LOG_Print(x)                        HTRACE_LOG(x)
#define CI_Assert(expression)               HTRACE_ASSERT(expression)
#else
#define INFO_Print(x)
#define ERROR_Print(x)    
#define DEBUG_Print(x)  
#define LOG_Print(x) 
#define CI_Assert(expression)  
#endif


#define CI_I2C_TIMEOUT         (200)


typedef DWORD CI_ErrorCode_t;
typedef void(* pfnCI_PIOPinISR) (U32 Handle, U8 ActiveBits);
typedef int(* pfnCI_SYSIRQISR) (void *pvParams);


/****************************************************************************
 * Type
 ****************************************************************************/
typedef void (CIThreadEntry)(void *pParam);


/****************************************************************************
 * OS-API
 ****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
//  Memory
//
//////////////////////////////////////////////////////////////////////////////
void*       CI_OSMalloc( DWORD dwMemorySize );


//////////////////////////////////////////////////////////////////////////////
//
//  Task
//
//////////////////////////////////////////////////////////////////////////////

HWVHANDLE   CI_OSThreadCreate(const char* pstrName,int nPriority, int nStackSize, CIThreadEntry* pThreadEntry,void* pvArg, DWORD* pdwThreadID);
DWORD       CI_OSThreadDestroy (HWVHANDLE hThread);
DWORD       CI_OSThreadSuspend(HWVHANDLE hThread);
DWORD       CI_OSThreadResume(HWVHANDLE hThread);
DWORD       CI_OSThreadGetID(void);
DWORD       CI_OSThreadSetPriority(HWVHANDLE hThread, int  nPriority);
DWORD       CI_OSThreadGetPriority(HWVHANDLE hThread, int* pnPriority);
void        CI_OSSleep(DWORD dwMilliSeconds);
DWORD       CI_OSThreadSetPriority(HWVHANDLE hThread, int nPriority);
DWORD       CI_OSThreadGetPriority(HWVHANDLE hThread, int* pnPriority);


//////////////////////////////////////////////////////////////////////////////
//
//  Semaphore
//
//////////////////////////////////////////////////////////////////////////////
HWVHANDLE   CI_OSSemaphoreCreate(const char* pstrName,int nInitialCount,int nMaxCount);
DWORD       CI_OSSemaphoreWaitFor(HWVHANDLE hSemaphore, DWORD dwTimeout);
DWORD       CI_OSSemaphoreRelease(HWVHANDLE hSemaphore, int nReleaseCount);
DWORD       CI_OSSemaphoreDestory(HWVHANDLE hSemaphore);



//////////////////////////////////////////////////////////////////////////////
//
//  MsgQueue
//
//////////////////////////////////////////////////////////////////////////////

DWORD CI_OSMsgQueueCreate(const char * pstrName,int nMaxMsgs,int nMsgLen,DWORD dwOptions);
DWORD CI_OSMsgQueueDestroy(HWVHANDLE hMsgQueue);
DWORD CI_OSMsgQueueReceiveMsg(HWVHANDLE hMsgQueue,void* pvMsg,int nMaxMsgBytes,DWORD dwTimeout);
DWORD CI_OSMsgQueueSendMsg(HWVHANDLE hMsgQueue, const void* pvMsg, int nMsgBytes, DWORD dwTimeout);


//////////////////////////////////////////////////////////////////////////////
//
//  I2C : (Just for the CIMax...)
//
//////////////////////////////////////////////////////////////////////////////
CI_ErrorCode_t CI_I2CInitialize(int nIndex);
CI_ErrorCode_t CI_I2COpen(WORD wSlaveAddress);
CI_ErrorCode_t CI_I2CClose(void);
CI_ErrorCode_t CI_I2CWriteWithStop(BYTE ucSubAddress, BYTE *pbBuffer,DWORD dwDataLength );
CI_ErrorCode_t CI_I2CReadWithStop( BYTE ucSubAddress, BYTE *pbBuffer,DWORD dwDataLength );


//////////////////////////////////////////////////////////////////////////////
//
//  PIO : (Just for the CIMax...)
//
//////////////////////////////////////////////////////////////////////////////

CI_ErrorCode_t CI_SetPIORestBit(void);
CI_ErrorCode_t CI_SetPIOforINT(pfnCI_PIOPinISR pfnISR);

int CI_SetSysIRQINT(pfnCI_SYSIRQISR pfnSYSIRQISR);


//////////////////////////////////////////////////////////////////////////////
//
//  FMI : (Just for the CIMax...)
//
//////////////////////////////////////////////////////////////////////////////

CI_ErrorCode_t CI_FMISetup(void);

//////////////////////////////////////////////////////////////////////////////
//
//  DEBUG
//
//////////////////////////////////////////////////////////////////////////////

#endif // PLATFORM_IRD_5105











/*---------------------------------------------------------------------------
 * 平台选择: PLATFORM_IRD_7109
 *---------------------------------------------------------------------------
 */

#ifdef PLATFORM_IRD_7109



#ifndef STTBX_PRINT
#define STTBX_PRINT
#endif

/****************************************************************************
 * include
 ****************************************************************************/

#include "sttbx.h"
// 针对7109
#include "WV_OS.h"
//#include "WV_Debug.h"

#include "WV_HTRACE.h"

#include "WV_I2C.h"


/****************************************************************************
 * Macro
 ****************************************************************************/
// debug
#define CI_DEBUG_STATUS
// debug use uart -- comment it for use dcu
#define DEBUG_USE_UART

extern char                     s_CIstrOutBuff[1024];
extern char                     s_CIstrOutBuffExt[1024];

void CI_Print2String(char *format, ...);
void CI_PrintBuff(int nDebugLevel);
void CI_COMMONPrint(int nDebugLevel, char *format, ...);


/*******************************************************************************
 * 
 * ADD BY TOPTIMES
 * 
 *******************************************************************************/
#define ENABLE_TT_DEBUG

#ifdef ENABLE_TT_DEBUG
#define INFO_Print(x)                       HTRACE_INFO(x)
#define ERROR_Print(x)                      HTRACE_ERROR(x)
#define DEBUG_Print(x)                      HTRACE_INFO(x)
#define LOG_Print(x)                        HTRACE_LOG(x)
#define CI_Assert(expression)               HTRACE_ASSERT(expression)
#else
#define INFO_Print(x)
#define ERROR_Print(x)    
#define DEBUG_Print(x)  
#define LOG_Print(x) 
#define CI_Assert(expression)  
#endif


#define CI_I2C_TIMEOUT         (1000)

#define CI_MODULE_BASE          0x00000000
#define CI_MODULE_MEM           0x00001000
#define CI_MODULE_I2C           0x00002000
#define CI_MODULE_CIMAX         0x00003000
#define CI_MODULE_EXTEND        0x0000F000


/****************************************************************************
 * Enum
 ****************************************************************************/
typedef enum
{
    // BASE 
    CI_SUCCESS1 = CI_MODULE_BASE,
    CI_FAILURE2, 

    // OS
    CI_OS_TIMEOUT,

    // MEM
    CI_MEM_BASE = CI_MODULE_MEM,
    CI_MEM_ERROR,
    
    // I2C
    CI_I2C_BASE = CI_MODULE_I2C,
    CI_I2C_NO_INIT,
    CI_I2C_BUSY,
    CI_I2C_NOACK,
    CI_I2C_NOT_SUPPORT,
    CI_I2C_TIMOUT,
    CI_I2C_BAD_PARAM,
    CI_I2C_MAX_HANDLE,
    CI_I2C_HAD_INIT,
    CI_I2C_NO_OPEN,

    // CIMax
    CI_CIMAX_BASE = CI_MODULE_CIMAX,
    
    CI_NO_SPECIFY
}CI_RETRUN_t;

typedef DWORD CI_ErrorCode_t;
typedef void(* pfnCI_PIOPinISR) (U32 Handle, U8 ActiveBits);
typedef int(* pfnCI_SYSIRQISR) (void *pvParams);


/****************************************************************************
 * Type
 ****************************************************************************/
typedef void (CIThreadEntry)(void *pParam);


/****************************************************************************
 * OS-API
 ****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
//  Memory
//
//////////////////////////////////////////////////////////////////////////////
void*       CI_OSMalloc( DWORD dwMemorySize );
BOOL        CI_OSFree( void* pvMemoryAddr );


//////////////////////////////////////////////////////////////////////////////
//
//  Task
//
//////////////////////////////////////////////////////////////////////////////

HWVHANDLE   CI_OSThreadCreate(const char* pstrName,int nPriority, int nStackSize, CIThreadEntry* pThreadEntry,void* pvArg, DWORD* pdwThreadID);
DWORD       CI_OSThreadDestroy (HWVHANDLE hThread);
DWORD       CI_OSThreadSuspend(HWVHANDLE hThread);
DWORD       CI_OSThreadResume(HWVHANDLE hThread);
DWORD       CI_OSThreadGetID(void);
DWORD       CI_OSThreadSetPriority(HWVHANDLE hThread, int  nPriority);
DWORD       CI_OSThreadGetPriority(HWVHANDLE hThread, int* pnPriority);
void        CI_OSSleep(DWORD dwMilliSeconds);
DWORD       CI_OSThreadSetPriority(HWVHANDLE hThread, int nPriority);
DWORD       CI_OSThreadGetPriority(HWVHANDLE hThread, int* pnPriority);


//////////////////////////////////////////////////////////////////////////////
//
//  Semaphore
//
//////////////////////////////////////////////////////////////////////////////
HWVHANDLE   CI_OSSemaphoreCreate(const char* pstrName,int nInitialCount,int nMaxCount);
DWORD       CI_OSSemaphoreWaitFor(HWVHANDLE hSemaphore, DWORD dwTimeout);
DWORD       CI_OSSemaphoreRelease(HWVHANDLE hSemaphore, int nReleaseCount);
DWORD       CI_OSSemaphoreDestory(HWVHANDLE hSemaphore);



//////////////////////////////////////////////////////////////////////////////
//
//  MsgQueue
//
//////////////////////////////////////////////////////////////////////////////

DWORD CI_OSMsgQueueCreate(const char * pstrName,int nMaxMsgs,int nMsgLen,DWORD dwOptions);
DWORD CI_OSMsgQueueDestroy(HWVHANDLE hMsgQueue);
DWORD CI_OSMsgQueueReceiveMsg(HWVHANDLE hMsgQueue,void* pvMsg,int nMaxMsgBytes,DWORD dwTimeout);
DWORD CI_OSMsgQueueSendMsg(HWVHANDLE hMsgQueue, const void* pvMsg, int nMsgBytes, DWORD dwTimeout);

//////////////////////////////////////////////////////////////////////////////
//
//  I2C : (Just for the CIMax...)
//
//////////////////////////////////////////////////////////////////////////////
CI_ErrorCode_t CI_I2CInitialize(int nIndex);
CI_ErrorCode_t CI_I2COpen(WORD wSlaveAddress);
CI_ErrorCode_t CI_I2CClose(void);
CI_ErrorCode_t CI_I2CWriteWithStop(BYTE ucSubAddress, BYTE *pbBuffer,DWORD dwDataLength );
CI_ErrorCode_t CI_I2CReadWithStop( BYTE ucSubAddress, BYTE *pbBuffer,DWORD dwDataLength );


//////////////////////////////////////////////////////////////////////////////
//
//  PIO : (Just for the CIMax...)
//
//////////////////////////////////////////////////////////////////////////////

CI_ErrorCode_t CI_SetPIORestBit(void);
CI_ErrorCode_t CI_SetPIOforINT(pfnCI_PIOPinISR pfnISR);

int CI_SetSysIRQINT(pfnCI_SYSIRQISR pfnSYSIRQISR);


//////////////////////////////////////////////////////////////////////////////
//
//  FMI : (Just for the CIMax...)
//
//////////////////////////////////////////////////////////////////////////////

CI_ErrorCode_t CI_FMISetup(void);

//////////////////////////////////////////////////////////////////////////////
//
//  DEBUG
//
//////////////////////////////////////////////////////////////////////////////




//#ifdef 0 //WIN32
#if 0
	#include <assert.h>
        
	#define CIASSERT			assert
	#define CITRACE				WVPrint
#else
	#ifdef CI_DEBUG_STATUS
            //when use the debug mode ,we can chose the debug level
		#define CIASSERT(expression)  \
            do{ \
                if(!(expression)) \
                printf("Assertion: \"%s\" failed, in file %s, line %d\n", \
					#expression, __FILE__, __LINE__); \
            }while(0)

        #ifdef DEBUG_USE_UART
        //// UART
        #define CITRACE        printf
        #else
        //// DCU
		#define CITRACE		   printf
        #endif
	#else
		#define CIASSERT
		#define CITRACE        
	#endif
#endif



#endif //PLATFORM_IRD_7109











#endif  // __CI_COMMON_H_

#endif

