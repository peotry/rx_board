/******************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName PIN.h
* Description : control pin
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-23
* Record    :
*
******************************************************************************/
#ifndef PIN_H
#define PIN_H

#include "appGlobal.h"

//led module 鑳屾澘鐘舵?佹寚绀虹伅
typedef enum{
    LED1    =   29,
    LED2    =   30
}LED_PIN;

typedef enum
{
    HW_VER1 = 37,
    HW_VER2 = 38,
    HW_VER3 = 39,
}HW_VER_PIN;

typedef enum
{
	SLOT_BIT_0 = 21,
	SLOT_BIT_1 = 20,
	SLOT_BIT_2 = 19,
	SLOT_BIT_3 = 18,
	SLOT_BIT_4 = 17,
	SLOT_BIT_5 = 16,
}SLOT_ID_PIN;

void PIN_LEDOrangeLight();
void PIN_LEDGreenLight();
void PIN_LEDOut();

U32 PIN_GetHWVerSion(void);

U32 PIN_GetSlotID(void);


#endif
