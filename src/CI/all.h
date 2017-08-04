
#ifndef __ALL_H_
#define __ALL_H_

/****************************************************************************
 * Include files
 ****************************************************************************/

/****************************************************************************
 * Macro
 ****************************************************************************/
    
// add by shouquan.tang
#ifndef CI_SEND_CI_MESSAGE_CONST
#define CI_SEND_CI_MESSAGE_CONST
// SendCIMessage Tags
#define	CI_MSG_NONE					(0)
#define	CI_MSG_CI_INFO				(1)
#define	CI_MMI_MENU					(2)
#define	CI_MMI_LIST					(3)
#define	CI_MMI_TEXT					(4)
#define	CI_MMI_REQUEST_INPUT		(5)
#define	CI_MMI_CLOSE_MMI_IMM		(9)
#define CI_MMI_CA_PMT_REPLY			(10)
    
// the following codes are used as second byte of a CI_MSG_CI_INFO message
#define	CI_SLOT_EMPTY				(0)
#define	CI_SLOT_MODULE_INSERTED		(1)
#define	CI_SLOT_MODULE_NAME_OK		(2)
#define	CI_SLOT_MODULE_CA_OK		(3)
#endif

#define WAV_CI_PMT_END_FLAG         (0x47)

// error debug
#if 1
#define CI_ErrorPrint(x)            printf x
#else
#define CI_ErrorPrint(x)
#endif

    

// Delays task execution for n milliseconds
#define GURU_Delay(dwTicks)         usleep(dwTicks)
#define GURU_DelayMS(MS)            usleep(MS*1000)
// OS20 tick<->ms conversion for ST20 C1 core
#define CI_OSSleep(MS)              usleep(MS*1000)


#define GURU_GetTickCount()        0

/****************************************************************************
 * ENABLE CI FUNCTION
 ****************************************************************************/
#define ENABLE_CI                       //
#define ENABLE_PCMCIA                   //
//[TT:20090630-Closethisone]//#define ENABLE_AUTOMATIC_PMT_SEND       //

#define WVCI_USE_PRINT_FOR_GUI          // ´ò¿ª´òÓ¡GUI
#define ENABLE_CI_SEMPHORE //2013.01.20
#define ENABLE_IRQ_CONTROL //2013.01.20

/****************************************************************************
 * Ex-Include
 ****************************************************************************/
#include "stdio.h"
#include "string.h"
#include "PCMCIA.h"
#include "CI.h"
#include "WV_CI.h"
#include "AppGlobal.h"
#include "WVCI_PlatformDriver.h"
extern int  min(int a, int b);
extern void SendCIMessage(CI_HANDLE hCI, BYTE *pData, DWORD dwDataLength);

#endif // ALL_H


