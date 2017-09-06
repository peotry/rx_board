/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName £ºboardType.h
* Description £ºmacro define about board type
* Author    : huada.huang
* Modified  :
* Reviewer  :
* Date      : 2017-03-9
* Record    :
*
**********************************************************************/
#ifndef BOARDTYPE_
#define BOARDTYPE_


#define MACHINE_TYPE    0x1C

typedef enum{
    CR_DVBC = 0, 
	TRANS_BOARD = 1,
	CTRL_BOARD = 2
    
}BOARD_TYPE;
#endif
