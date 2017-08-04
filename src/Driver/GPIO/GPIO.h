/******************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName GPIO.h
* Description : GPIO module
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-22
* Record    :
*
******************************************************************************/
#ifndef _GPIO_H_
#define _GPIO_H_

#include "AppGlobal.h"


#define GPIO_CONTROLLER_PATH                "/sys/class/gpio"
#define LED_CONTROLLER_PATH                 "/sys/class/leds"
#define GPIO_EXPORT_PATH                    "/sys/class/gpio/export"
#define MAX_GPIO_CONTTROLLER_NAME_LEN       128
#define MAX_GPIO_COMMAN_LEN                 128
#define GPIO_BASE_CONTROLER_INDEX           906  //   base = ARCH_NR_GPIOS - ZYNQ_GPIO_NR_GPIOS = 1024 - 118 = 906

typedef enum{
    WV_GPIO_SUCCESS                         =               0X0,

    WV_GPIO_PIN_NUM_ERR                     =               0X1,    // pin num out of range

    WV_GPIO_DIRECT_FILE_OPEN_ERR            =               0X2,    // direction file can not open
    WV_GPIO_DIRECT_FILE_WRITE_ERR           =               0X3,    // direction file write error
    WV_GPIO_DIRECT_PARAM_ERR                =               0x4,    // the direction param error
    WV_GPIO_DIRECT_ERR                      =               0x5,    // the direction set error

    WV_GPIO_VALUE_FILE_OPEN_ERR             =               0X6,    // value file can not open
    WV_GPIO_VALUE_FILE_WRITE_ERR            =               0X7,    // value file write error
    WV_GPIO_VALUE_FILE_READ_ERR             =               0X8,    // value file read error

    WV_GPIO_CONTROL_CREAT_ERR               =               0X9,    // control file create error

}GPIOErrCode;

typedef enum{
    GPIO_OUT            =           0X0,
    GPIO_IN             =           0X1,    
}GPIODirectionType;

GPIOErrCode GPIO_SetGpio(U32 u32GPIONum, U32 u32GPIOValue);
GPIOErrCode GPIO_GetGpio(U32 u32GPIONum,  U32 * u32GPIOValue);


#endif

