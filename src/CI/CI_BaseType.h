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
 * MACRO 定义使用平台环境
 * 
 * 定义下列类型中的一个
 * PLATFORM_STB_5105
 * PLATFORM_STB_7109
 * PLATFORM_IRD_5105
 * PLATFORM_IRD_7109
 *
 * 使用过程中，根据不同项目，定义此常量即可: 如 #define PLATFORM_IRD_5105
 *
 * *******
 *
 * 与硬件平台相关的模块主要有:
 *　CI_Common.c | h         此模块主要负责系统级函数转接
 *  cimax.c | h             此模块跟系统平台相关。主要是片选地址，中断信号源等
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

/* SD-CI 修改历史记录:
 * 
 */

#ifdef PLATFORM_IRD_7109 
#define WVCI_VERSION                   "CI_VERSION_HD_1.2.3cimax"
#endif

/* HD-CI 修改历史记录:
 * [shouquan.tang 2010-07-16] V1.2.3 为了修正插入卡偶尔导致死机。原因估计是参数没有检查导致。
 * [shouquan.tang 2010-07-14] V1.2.2 为了修正名字获取异常问题，增加两个状态更新: Inserted.. Initializing...
 * [shouquan.tang 2010-07-14] V1.2.1 为了修正INT异常问题，把INT的PIO在每次检测到中断后关开一次
 */


#ifdef __cplusplus
}
#endif

#endif      //__CI_BASETYPE_H_

#endif

