/******************************************************************************

  Copyright (C), 2013,HuiZhou WELLAV TechnologyCo., Ltd.

 ******************************************************************************
  File Name     : adt7410.h
  Version       : Initial Draft
  Author        : guanghui.chen
  Created       : 2013/12/12
  Last Modified :
  Description   : adt7410.c header file
  Function List :
  History       :
  1.Date        : 2013/12/12
    Author      : guanghui.chen
    Modification: Created file

******************************************************************************/

#ifndef __ADT7410_H__
#define __ADT7410_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define AD7410_DEV_ADDR 0x4B

#define AD7410_CHIP_NOEXIST     0
#define AD7410_CHIP_EXIST       1

#define ADT7410_MANUFACTURE_ID 0xC8
#define ADT7410_MANUFACTURE_ID_MASK 0xF8 //[7:3]  11001  R  Manufacture ID  Contains the manufacturer identification numbe

#define AD7410_LOWEST_TEMP_VALUE   0x1C90 //-55 degree
#define AD7410_WARNING_TEMP_VALUE  0x4E0  //78 degree  
#define AD7410_POWEROFF_TEMP_VALUE 0x520  //82 degree 

#define AD7410_NORMAL_RECONGNIZE_LIMIT 0x960  //根据芯片手册，最大工作温度为150度


#define POWEROFF_THRESHOLD  3

typedef enum
{
    WV_TEMP_SUCCESS          =       0x0,

    WV_TEMP_PARAMS_ERR       =       0x1,
    WV_TEMP_SYNC_ERR         =       0x2,
    WV_TEMP_NEGATIVE         =       0x3,

}TempErrCode;

TempErrCode ADT7410_OpenBus(void);
TempErrCode ADT7410_CloseBus(void);
TempErrCode ADT7410_Init(void);
S32 ADT7410_Detect(void);
TempErrCode ADT7410_GetTemperature(U32  * pu32temp);

void ADT7410_example(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __ADT7410_H__ */


