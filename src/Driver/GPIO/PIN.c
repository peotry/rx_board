/******************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName PIN.c
* Description : control pin
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-23
* Record    :
*
******************************************************************************/
#include <unistd.h>
#include "PIN.h"
#include "GPIO.h"
#include "WV_log.h"


/*  led module  */

/*****************************************************************************
  Function:     PIN_LEDOrangeLight
  Description:  Set the orange led light
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
  Modify:
*****************************************************************************/
void PIN_LEDOrangeLight()
{
    GPIO_SetGpio(LED1, 1);
    GPIO_SetGpio(LED2, 0);
}

/*****************************************************************************
 Function:     PIN_LEDGreenLight
 Description:  Set the green led light
 Input:        none
 Output:       none
 Return:       none
 Author:       ruibin.zhang
 Modify:
*****************************************************************************/
void PIN_LEDGreenLight()
{
   GPIO_SetGpio(LED1, 0);
   GPIO_SetGpio(LED2, 1);
}

/*****************************************************************************
 Function:     PIN_LEDOut
 Description:  Set the led light out
 Input:        none
 Output:       none
 Return:       none
 Author:       ruibin.zhang
 Modify:
*****************************************************************************/
void PIN_LEDOut()
{
   GPIO_SetGpio(LED1, 1);
   GPIO_SetGpio(LED2, 1);
}

/*****************************************************************************
 Function:     PIN_GetHWVerSion
 Description:  get hardware versin number
 Input:        none
 Output:       none
 Return:       hardware versin number
 Author:       Momouwei 2017.04.24
 Modify:
*****************************************************************************/
U32 PIN_GetHWVerSion(void)
{
    U32 u32value   = 0;
    U32 u32HWVer   = 0;

    GPIO_GetGpio(HW_VER1, &u32value);
    if (1 == u32value)
    {
        u32HWVer = 1;
    }

    GPIO_GetGpio(HW_VER2, &u32value);
    if (1 == u32value)
    {
        u32HWVer |= (0x1 << 1);
    }

    GPIO_GetGpio(HW_VER3, &u32value);
    if (1 == u32value)
    {
        u32HWVer |= (0x1 << 2);
    }

    return u32HWVer;
}


int g_SlotID = 0;

U32 PIN_GetSlotID(void)
{
	U32 u32Value   = 0;
	U32 u32SlotBit[5] = {0};
	U32 i = 0;

	GPIO_GetGpio(SLOT_BIT_0, &u32SlotBit[0]);
	GPIO_GetGpio(SLOT_BIT_1, &u32SlotBit[1]);
	GPIO_GetGpio(SLOT_BIT_2, &u32SlotBit[2]);
	GPIO_GetGpio(SLOT_BIT_3, &u32SlotBit[3]);
	GPIO_GetGpio(SLOT_BIT_4, &u32SlotBit[4]);
	GPIO_GetGpio(SLOT_BIT_5, &u32SlotBit[5]);

	for(i = 0; i < 5; ++i)
	{
		u32Value |= (u32SlotBit[i] << i);
	}

	u32Value &= 0x3f;

	g_SlotID = u32Value;

	return u32Value;
}


