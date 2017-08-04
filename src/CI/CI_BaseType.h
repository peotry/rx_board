#if 0

/******************************************************************************
 * CI_BaseType.h - CI_BaseType
 * 
 * Copyright 2008-2013 HuiZhou WELLAV Technology Co.,Ltd.
 * 
 * DESCRIPTION: - 
 *      Base typedef
 * modification history
 * --------------------
 * 10 Sep 2008, shouquan.tang written
 * --------------------
 ******************************************************************************/

#ifndef __CI_BASETYPE_H_
#define __CI_BASETYPE_H_

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
 * MACRO ����ʹ��ƽ̨����
 * 
 * �������������е�һ��
 * PLATFORM_STB_5105
 * PLATFORM_STB_7109
 * PLATFORM_IRD_5105
 * PLATFORM_IRD_7109
 *
 * ʹ�ù����У����ݲ�ͬ��Ŀ������˳�������: �� #define PLATFORM_IRD_5105
 *
 * *******
 *
 * ��Ӳ��ƽ̨��ص�ģ����Ҫ��:
 *��CI_Common.c | h         ��ģ����Ҫ����ϵͳ������ת��
 *  cimax.c | h             ��ģ���ϵͳƽ̨��ء���Ҫ��Ƭѡ��ַ���ж��ź�Դ��
 *  PCMCIA.c | h                
 *  WV_CI.c | h
 * 
 * 
 ****************************************************************************/
#define PLATFORM_IRD_7109       // UMH150 IRD STI5105


/****************************************************************************
 * INCLUDE
 ****************************************************************************/
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "math.h"
#include "stddefs.h"


#ifdef PLATFORM_STB_5105
#include "WV_Typedef.h"
#include "WV_HDI_Typedef.h"
#include "WV_SW_CommonType.h"
#endif

#ifdef PLATFORM_STB_7109
#include "stcommon.h"

#include "WV_Typedef.h"
#include "WV_HDI_Typedef.h"
#include "WV_SW_CommonType.h"

#endif

#ifdef PLATFORM_IRD_7109
#include "stcommon.h"

#include "WV_Typedef.h"
#include "WV_HDI_Typedef.h"
#include "WV_SW_CommonType.h"

#endif

#ifdef PLATFORM_IRD_5105

#include "stddefs.h"
#include "stcommon.h"


#include "WV_Typedef.h"
#include "WV_HDI_Typedef.h"
#include "WV_OS.h"
#include "WV_HTRACE.h"

#endif



#ifdef PLATFORM_IRD_5105 
#define WVCI_VERSION                   "CI_VERSION_SD_1.2.0cimax"
#endif

/* SD-CI �޸���ʷ��¼:
 * 
 */

#ifdef PLATFORM_IRD_7109 
#define WVCI_VERSION                   "CI_VERSION_HD_1.2.3cimax"
#endif

/* HD-CI �޸���ʷ��¼:
 * [shouquan.tang 2010-07-16] V1.2.3 Ϊ���������뿨ż������������ԭ������ǲ���û�м�鵼�¡�
 * [shouquan.tang 2010-07-14] V1.2.2 Ϊ���������ֻ�ȡ�쳣���⣬��������״̬����: Inserted.. Initializing...
 * [shouquan.tang 2010-07-14] V1.2.1 Ϊ������INT�쳣���⣬��INT��PIO��ÿ�μ�⵽�жϺ�ؿ�һ��
 */


#ifdef __cplusplus
}
#endif

#endif      //__CI_BASETYPE_H_

#endif

