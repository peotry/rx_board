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


#define CMP_MACHINE_TYPE    0x1a

typedef enum{
    CMP_BACKBOARD           =       0x01,
    CMP_RECEIVER_C_S2       =       0x11,
    CMP_QAM_16              =       0x21,
    CMP_ENCODER_SD_CVBS_6   =       0x31,
    CMP_ENCODER_HD_HDMI_4   =       0x32,
    CMP_RECEIVER_DTMB       =       0x33
    
}BOARD_TYPE;
#endif
