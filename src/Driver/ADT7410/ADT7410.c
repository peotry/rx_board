/******************************************************************************

  Copyright (C), 2013,HuiZhou WELLAV Technology Co., Ltd.

 ******************************************************************************
  File Name     : adt7410.c
  Version       : Initial Draft
  Author        : guanghui.chen
  Created       : 2013/12/12
  Last Modified :
  Description   : adt7410 driver'
  Function List :
              adt7410_get_temperature
              adt7410_init
              adt7410_read
              adt7410_write
  History       :
  1.Date        : 2013/12/12
    Author      : guanghui.chen
    Modification: Created file

******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "AppGlobal.h"
#include "ADT7410.h"
#include "xilinx_i2c.h"
#include "WV_log.h"


/*****************************************************************************
  Function:     ADT7410_write
  Description:  write adt7410 register
  Input:        U8 offset
                U8 value
  Output:       none
  Return:       success return 0,and fail return -1
  Author:       guanghui.chen
*****************************************************************************/
static S32 ADT7410_write(U8 offset,U8 value)
{
    S32 ret = 0;

    /* 写总线 */
    i2c_set_addr(I2C_1, AD7410_DEV_ADDR, 0);

    ret = i2c_write(I2C_1, offset, value);

    return ret;
}
/*****************************************************************************
  Function:     ADT7410_read
  Description:  read adt7410
  Input:        U8 offset
                U8 *value
  Output:       none
  Return:       success return 0,and fail return -1
  Author:       guanghui.chen
*****************************************************************************/
static S32 ADT7410_read(U8 offset, U8 *value)
{
    S32 ret = 0;

    /* 读总线 */
    i2c_set_addr(I2C_1, AD7410_DEV_ADDR, 0);
    ret = i2c_read(I2C_1, offset, value);

    return ret;
}

/*****************************************************************************
  Function:     ADT7410_OpenBus
  Description:  init ADT7410
  Input:        None
  Output:       None
  Return:       success return 0,and fail return -1
  Author:       dadi.zeng
*****************************************************************************/
TempErrCode ADT7410_OpenBus(void)
{

    i2c_OpenBus(I2C_1);

    return WV_TEMP_SUCCESS;
}

/*****************************************************************************
  Function:     ds1338_CloseBus
  Description:  close ADT7410
  Input:        None
  Output:       None
  Return:       success return 0
  Author:       dadi.zeng
*****************************************************************************/
TempErrCode ADT7410_CloseBus(void)
{

   (void)i2c_CloseBus(I2C_1);


    return WV_TEMP_SUCCESS;
}

/*****************************************************************************
  Function:     adt740_Detect
  Description:  detect 7410 chip
  Input:        void
  Output:       none
  Return:       Exist return 1,
                fail return 0
  Author:       guanghui.chen
*****************************************************************************/
S32 ADT7410_Detect(void)
{
    S32 ret = 0;
    U8 id   = 0;

    //read chip id
    ret  = ADT7410_read(0x0B, &id);
    if(I2C_FAILURE == ret)
    {
        return AD7410_CHIP_NOEXIST;
    }
    
    //check chip id
    if(ADT7410_MANUFACTURE_ID != (id & ADT7410_MANUFACTURE_ID_MASK))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TEMP, "I2C device  no exist!");
        return AD7410_CHIP_NOEXIST;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TEMP, "I2C device exist!");
        ADT7410_Init();
        return AD7410_CHIP_EXIST;
    }
}

/*****************************************************************************
  Function:     ADT7410_Init
  Description:  init adt7410
  Input:        void
  Output:       none
  Return:       WV_TEMP_SUCCESS
  Author:       guanghui.chen
*****************************************************************************/
TempErrCode ADT7410_Init(void)
{
    //80 degree //0x28
    ADT7410_write(0x04,0x28);     //high temperature most  significant byte
    ADT7410_write(0x05,0x00);     //high temperature least significant byte

    //-10 degree
    ADT7410_write(0x06,0x85);     //low  temperature most  significant byte
    ADT7410_write(0x07,0x00);     //low  temperature least significant byte
    
    return WV_TEMP_SUCCESS;
}

/*****************************************************************************
  Function:     ADT7410_GetTemperature
  Description:  get the temperature of adt7410
  Input:        none
  Output:       the temperature,conver method:
                1.Positive Temperature= ADC Code (dec)/16

                2.Negative Temperature= (ADC Code (dec) - 8192)/16
                  where ADC Codeuses the first 13 MSBs of the data byte,
                  including the sign bit.

                3.Negative Temperature= (ADC Code (dec) ?C 4096)/16
                  where Bit 15 (sign bit) is removed from the ADC cod
  Return:       WV_TEMP_SUCCESS
  Author:       guanghui.chen
*****************************************************************************/
TempErrCode ADT7410_GetTemperature(U32  * pu32temp)
{
    U8 value1 = 0;
    U8 value2 = 0;
    float tmp = 0;

    ADT7410_read(0x00, &value1);
    ADT7410_read(0x01, &value2);

    //normal read
    *pu32temp = (value1 << 5) | (value2 >> 3);   // the highest bit is Sign,13bit mode

    if((*pu32temp) & 0x1000)//Negative Temperature
    {
    	tmp = (float)((*pu32temp)-8192)/16;
		printf("tmp = %f\n", tmp);
    	*pu32temp = (float)((*pu32temp)-8192)/16;
		return WV_TEMP_NEGATIVE;
    }
    else
    {
    	tmp = (float)(*pu32temp)/16;
		printf("tmp = %f\n", tmp);
    	*pu32temp = (float)(*pu32temp)/16;
    }
    return WV_TEMP_SUCCESS;
}

