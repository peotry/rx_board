/******************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName GPIO.c
* Description : GPIO module
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-22
* Record    :
*
******************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "gpio.h"
#include "WV_log.h"

/*****************************************************************************
  Function:     GPIO_GetLinuxGPIONum
  Description:  Get the linux gpio index of a gpio
  Input:        u32GPIONum       -       GPIO index
  Output:       none
  Return:       U32         -       linux gpio index
  Author:       ruibin.zhang
  Note:         XXX 不同的arm平台，计算GPIOnum方法不同，下面为xilinx芯片计算GPIO的方法
  Reference:    Alera-https://support.criticallink.com/redmine/boards/45/topics/4148
                Xilinx-http://www.wiki.xilinx.com/Linux+GPIO+Driver
 *****************************************************************************/
U32 GPIO_GetLinuxGPIONum(U32 u32GPIONum)
{
#ifdef ALTERA
    U32 u32LinuxNum = 0;

    if((u32GPIONum >= 0) && (u32GPIONum <= 28))
    {
        u32LinuxNum = 227 + u32GPIONum;
    }
    else if((u32GPIONum >= 29) && (u32GPIONum <= 57))
    {
        u32LinuxNum = 198 + u32GPIONum - 29;
    }
    else if((u32GPIONum >= 58) && (u32GPIONum <= 66))
    {
        u32LinuxNum = 171 + u32GPIONum - 58;
    }
    else
    {
        u32LinuxNum = 0;
    }

    return u32LinuxNum;
#else
    if(u32GPIONum > MAX_GPIO_CONTTROLLER_NAME_LEN)
    {
        return 0;
    }
    
    U32 u32LinuxNum = 0;

    u32LinuxNum = u32GPIONum + GPIO_BASE_CONTROLER_INDEX;

    return u32LinuxNum;
#endif
}

/*****************************************************************************
  Function:     GPIO_CreateGPIOController
  Description:  Create the linux gpio controller of a gpio
  Input:        u32GPIONum       -       GPIO index
  Output:       none
  Return:       WV_GPIO_PIN_NUM_ERR     -       pin num out of range
                WV_GPIO_SUCCESS         -       success
  Author:       ruibin.zhang
 *****************************************************************************/
GPIOErrCode GPIO_CreateGPIOController(U32 u32GPIONum)
{
    U8  u8GpioExportName[MAX_GPIO_CONTTROLLER_NAME_LEN]     = {0};
    U8  u8GpioControllerDir[MAX_GPIO_CONTTROLLER_NAME_LEN]  = {0};
    U8  u8CommandStr[MAX_GPIO_COMMAN_LEN] = {0};
    U32 u32LinuxPin = 0;
    DIR *pdir = NULL;

    /* find the linux index of gpio */
    u32LinuxPin = GPIO_GetLinuxGPIONum(u32GPIONum);
    if(0 == u32LinuxPin)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_GPIO, "GPIO index %d is error!", u32GPIONum);
        return WV_GPIO_PIN_NUM_ERR;
    }

    /* find export file */
    snprintf((char *)u8GpioExportName, sizeof(u8GpioExportName), "%s", GPIO_EXPORT_PATH);

    /* create gpio-controller by system-command */
    snprintf((char *)u8CommandStr, sizeof(u8CommandStr), "echo %d > %s",  u32LinuxPin,  (char *)u8GpioExportName);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_GPIO, "Command: [%s]", u8CommandStr);
    system((char *)u8CommandStr);

    /* check if creat gpio-controller success*/
    snprintf((char *)u8GpioControllerDir, MAX_GPIO_CONTTROLLER_NAME_LEN, "%s/gpio%d/", GPIO_CONTROLLER_PATH, u32LinuxPin);
    pdir = opendir((char *)u8GpioControllerDir);
    if(NULL == pdir)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_GPIO, "GPIO%d create controller fail!", u32GPIONum);
        return WV_GPIO_CONTROL_CREAT_ERR;
    }
    else
    {
        closedir(pdir);
    }

    return WV_GPIO_SUCCESS;
}

/*****************************************************************************
  Function:     GPIO_SetGPIODirection
  Description:  Set the direction of gpio to input or output
  Input:        u32GPIONum          -       GPIO index
                u32GPIODirection    -       GPIO Direction type
  Output:       none
  Return:       WV_GPIO_PIN_NUM_ERR             -       pin num out of range
                WV_GPIO_DIRECT_FILE_OPEN_ERR    -       direction file can not open
                WV_GPIO_DIRECT_FILE_WRITE_ERR   -       direction file write error
                WV_GPIO_DIRECT_ERR              -       direction set error
                WV_GPIO_SUCCESS                 -       success
  Author:       ruibin.zhang
 *****************************************************************************/
GPIOErrCode GPIO_SetGPIODirection(U32 u32GPIONum , GPIODirectionType u32GPIODirection)
{
    U8  u8GpioControllerName[MAX_GPIO_CONTTROLLER_NAME_LEN] = {0};
    U8  u8DirectionString[MAX_GPIO_CONTTROLLER_NAME_LEN] = {0};
    U32 u32LinuxPin = 0;
    S32 fd = -1;
    S32 nRet = -1;

    /* find the linux index of gpio */
    u32LinuxPin = GPIO_GetLinuxGPIONum(u32GPIONum);
    if(0 == u32LinuxPin)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_GPIO, "GPIO index %d is error!", u32GPIONum);
        return WV_GPIO_PIN_NUM_ERR;
    }

    /* find a gpio-controller */
    snprintf((char *)u8GpioControllerName, MAX_GPIO_CONTTROLLER_NAME_LEN, "%s/gpio%d/direction", GPIO_CONTROLLER_PATH, u32LinuxPin);
    fd = open((char *)u8GpioControllerName, O_RDWR);
    if(-1 == fd)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_GPIO, "Can't open gpio [%d] controller [%s]!", u32LinuxPin, u8GpioControllerName);
        return WV_GPIO_DIRECT_FILE_OPEN_ERR;
    }

    /* write input or output of gpio */
    if(GPIO_OUT == u32GPIODirection)
    {
        snprintf((char *)u8DirectionString, sizeof(u8DirectionString), "%s", "out");
    }
    else if(GPIO_IN == u32GPIODirection)
    {
        snprintf((char *)u8DirectionString, sizeof(u8DirectionString), "%s", "in");
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_GPIO, "GPIODirection set %d is error!", u32GPIODirection);
        return WV_GPIO_DIRECT_PARAM_ERR;
    }

    nRet = write(fd, u8DirectionString, sizeof(u8DirectionString));
    if(nRet != sizeof(u8DirectionString))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_GPIO, "Can't write gpio [%d] controller [%s]!", u32LinuxPin, u8GpioControllerName);
        close(fd);
        return WV_GPIO_DIRECT_FILE_WRITE_ERR;
    }

    /* close gpio-controller */
    close(fd);

    return WV_GPIO_SUCCESS;
}

 /*****************************************************************************
  Function:     GPIO_SetGpio
  Description:  Set the gpio out to high or low
  Input:        u32GPIONum        -       GPIO index
                u32GPIOValue      -       GPIO level
  Output:       none
  Return:       WV_GPIO_PIN_NUM_ERR             -       pin num out of range
                WV_GPIO_VALUE_FILE_OPEN_ERR     -       value file can not open
                WV_GPIO_VALUE_FILE_WRITE_ERR    -       value file write error
                WV_GPIO_DIRECT_ERR              -       direction set error
                WV_GPIO_SUCCESS                 -       success
  Author:       dadi.zeng
  Modify:       ruibin.zhang
*****************************************************************************/
GPIOErrCode GPIO_SetGpio(U32 u32GPIONum, U32 u32GPIOValue)
{
    U8  u8GpioControllerDir[MAX_GPIO_CONTTROLLER_NAME_LEN]  = {0};
    U8  u8GpioControllerName[MAX_GPIO_CONTTROLLER_NAME_LEN] = {0};
    U32 u32LinuxPin = 0;
    DIR * pdir      = NULL;
    S32 fd = -1;
    U8  u8value[5] = {0};
    S32 nRet = -1;
    
    /* find the linux index of gpio */
    u32LinuxPin = GPIO_GetLinuxGPIONum(u32GPIONum);
    if(0 == u32LinuxPin)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_GPIO, "GPIO index %d is error!", u32GPIONum);
        return WV_GPIO_PIN_NUM_ERR;
    }

    /* check if need creat gpio-controller*/
    snprintf((char *)u8GpioControllerDir, MAX_GPIO_CONTTROLLER_NAME_LEN, "%s/gpio%d/", GPIO_CONTROLLER_PATH, u32LinuxPin);
    pdir = opendir((char *)u8GpioControllerDir);
    if(NULL == pdir)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_GPIO, "GPIO%d need to creat controller!", u32GPIONum);
        if(WV_GPIO_SUCCESS != GPIO_CreateGPIOController(u32GPIONum))
        {
            return WV_GPIO_CONTROL_CREAT_ERR;
        }
    }
    else
    {
        closedir(pdir);
    }

    /* set gpio direction*/
    if(WV_GPIO_SUCCESS != GPIO_SetGPIODirection(u32GPIONum, GPIO_OUT))
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_GPIO, "GPIO set direction error!");
        return WV_GPIO_DIRECT_ERR;
    }
    
    /* find a gpio-controller */
    snprintf((char *)u8GpioControllerName, MAX_GPIO_CONTTROLLER_NAME_LEN, "%s/gpio%d/value", GPIO_CONTROLLER_PATH, u32LinuxPin);
    fd = open((char *)u8GpioControllerName, O_RDWR);
    if(-1 == fd)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_GPIO, "Can't open gpio [%d] controller [%s]!", u32LinuxPin, u8GpioControllerName);
        return WV_GPIO_VALUE_FILE_OPEN_ERR;
    }

    /* write high or low level of gpio */
    if(0 == u32GPIOValue)
    {
        snprintf((char *)u8value, sizeof(u8value), "%s", "0");
    }
    else 
    {
        snprintf((char *)u8value, sizeof(u8value), "%s", "1");
    }
    nRet = write(fd, u8value, sizeof(u8value));
    if(nRet != sizeof(u8value))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_GPIO, "Can't write gpio [%d] controller [%s]!", u32LinuxPin, u8GpioControllerName);
        close(fd);
        return WV_GPIO_VALUE_FILE_WRITE_ERR;
    }
    
    /* close gpio-controller */
    close(fd);

    return WV_GPIO_SUCCESS;
}

/*****************************************************************************
  Function:     GPIO_GetGpio
  Description:  Get the input level of gpio
  Input:        u32GPIONum        -       GPIO index              
  Output:       u32GPIOValue      -       GPIO level
  Return:       WV_GPIO_PIN_NUM_ERR             -       pin num out of range
                WV_GPIO_VALUE_FILE_OPEN_ERR     -       value file can not open
                WV_GPIO_VALUE_FILE_READ_ERR     -       value file read error
                WV_GPIO_DIRECT_ERR              -       direction set error
                WV_GPIO_SUCCESS                 -       success
  Author:       dadi.zeng
  Modify:       ruibin.zhang
*****************************************************************************/
GPIOErrCode GPIO_GetGpio(U32 u32GPIONum,  U32 * u32GPIOValue)
{
    U8  u8GpioControllerDir[MAX_GPIO_CONTTROLLER_NAME_LEN]  = {0};
    U8  u8GpioControllerName[MAX_GPIO_CONTTROLLER_NAME_LEN] = {0};
    U32 u32LinuxPin = 0;
    DIR * pdir      = NULL;
    S32 fd = -1;
    U8  u8value[5] = {0};
    S32 nRet = -1;
    
    /* find the linux index of gpio */
    u32LinuxPin = GPIO_GetLinuxGPIONum(u32GPIONum);
    if(0 == u32LinuxPin)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_GPIO, "GPIO index %d is error!", u32GPIONum);
        return WV_GPIO_PIN_NUM_ERR;
    }

    /* check if need creat gpio-controller*/
    snprintf((char *)u8GpioControllerDir, MAX_GPIO_CONTTROLLER_NAME_LEN, "%s/gpio%d/", GPIO_CONTROLLER_PATH, u32LinuxPin);
    pdir = opendir((char *)u8GpioControllerDir);
    if(NULL == pdir)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_GPIO, "GPIO%d need to creat controller!", u32GPIONum);
        if(WV_GPIO_SUCCESS != GPIO_CreateGPIOController(u32GPIONum))
        {
            return WV_GPIO_CONTROL_CREAT_ERR;
        }
    }
    else
    {
        closedir(pdir);
    }

    /* set gpio direction*/
    if(WV_GPIO_SUCCESS != GPIO_SetGPIODirection(u32GPIONum, GPIO_IN))
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_GPIO, "GPIO set direction error!");
        return WV_GPIO_DIRECT_ERR;
    }
    
    /* find a gpio-controller */
    snprintf((char *)u8GpioControllerName, MAX_GPIO_CONTTROLLER_NAME_LEN, "%s/gpio%d/value", GPIO_CONTROLLER_PATH, u32LinuxPin);
    fd = open((char *)u8GpioControllerName, O_RDWR);
    if(-1 == fd)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_GPIO, "Can't open gpio [%d] controller [%s]!", u32LinuxPin, u8GpioControllerName);
        return WV_GPIO_VALUE_FILE_OPEN_ERR;
    }

    /* read input gpio-level */
    nRet = read(fd, u8value, sizeof(u8value));
    if(nRet != 2)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_GPIO, "Can't read gpio [%d] controller [%s]!", u32LinuxPin, u8GpioControllerName);
        close(fd);
        return WV_GPIO_VALUE_FILE_READ_ERR;
    }
    else
    {
        * u32GPIOValue = (U32)atoi((char *)u8value);
        //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_GPIO, "read gpio [%d] value: [%d]!", u32LinuxPin, *u32GPIOValue);
    }

    /* close gpio-controller */
    close(fd);

    return WV_GPIO_SUCCESS;
}

