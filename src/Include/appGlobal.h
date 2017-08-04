/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName ：appGlobal.h
* Description ：macro define about data type and version.
* Author    : wei.li
* Modified  :
* Reviewer  :
* Date      : 2017-02-21
* Record    :
*
**********************************************************************/
#ifndef APPGLOBAL_H_
#define APPGLOBAL_H_

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "wv_err.h"

#define SW_VER                  0x000005 //软件版本号
                                         //[23-16]:大版本号
                                         //[8-15]:中版本号
                                         //[7-0]:小版本号

#define MAINTIAN_UART_COMM //控制在第一次通过串口获取到IP后是否让背板关闭对应的串口，方便调试，默认关闭。

#define WELLAV_APP              "/wellav_app/"
#define WELLAV_DATA             "/wellav_data/"

#define MAX_FILENAME_LENGTH     512     //最大文件名长度
#define MAX_COMMAND_LENGTH      1000    //最大命令行长度 比如 "rm xx.ts"
#define MAX_LINE_LENGTH         200     //一行最大长度

//打印宏
#define DEBUG_LOG(format, ...)    printf("[%s:%d] " format " \r\n", __FUNCTION__, __LINE__, ## __VA_ARGS__)

/**********************************************************************
 * Data type macro
 *********************************************************************/
typedef unsigned char                   U8;
typedef unsigned short                  U16;
typedef unsigned int                    U32;
typedef unsigned long                   U64;

typedef signed char                     S8;
typedef signed short                    S16;
typedef signed int                      S32;

typedef unsigned char volatile          VU8;
typedef unsigned short volatile         VU16;
typedef unsigned int volatile           VU32;
typedef unsigned long volatile          VU64;

typedef unsigned char   				BYTE;
typedef short           				SHORT;
typedef unsigned short  				WORD;
typedef int             				LONG;
typedef unsigned int    				DWORD;
typedef char            				BOOL;

#define FALSE                           0
#define TRUE                            1

typedef enum{
    SLOT0 = 0,
    SLOT1,
    SLOT2,
    SLOT3,
    SLOT4,
    SLOT5,
    SLOT6,
    SLOT7,
    SLOT8,
    SLOT9,
    SLOT10,
    SLOT11,
    SLOT12,
    SLOT13,
    SLOT14,
    SLOT15,
    SLOT_BB,
    MAX_SLOT_NUM
}SLOT_ID;

#define WEB_LOG(format, ...)    printf("[%s:%d] " format " \r\n", __FUNCTION__, __LINE__, ## __VA_ARGS__)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif/*APPGLOBAL_H_*/

