/**********************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName FPGA.c
* Description : set of other moudles refer to REG moudle
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-20
* Record    :
*
**********************************************************************/
#include "appGlobal.h"
#include "FPGA.h"
#include "REG.h"
#include "WV_log.h"


static char * strTunerBoardType[] =
{
    "DVBC/DTMB",
    "DVBS2"
};

/*****************************************************************************
  Function:     FPGA_Reg_Map
  Description:  map address
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
  Modify:
*****************************************************************************/
void FPGA_REG_Map()
{
    REG_Map();
}


/*****************************************************************************
  Function:     FPGA_Init
  Description:  init FPGA module
  Input:        none
  Output:       none
  Return:       none
  Author:       ruibin.zhang
  Modify:
*****************************************************************************/
void FPGA_Init(void)
{
    FPGA_REG_Map();
}


/*****************************************************************************
  Function:     FPGA_REG_Write
  Description:  write fpga reg
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_REG_Write(U32 u32Address, U32 u32Data)
{
    REG_Write(u32Address, u32Data);
}


/*****************************************************************************
  Function:     FPGA_REG_Read
  Description:  read fpga reg
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_REG_Read(U32 u32Address, U32 *pu32Data)
{
    REG_Read(u32Address,pu32Data);
}

void FPGA_REG_Write_Without_Delay(U32 u32Address, U32 u32Data)
{
    REG_Write_Without_Delay(u32Address, u32Data);
}

void FPGA_REG_Read_Without_Delay(U32 u32Address, U32 *pu32Data)
{
    REG_Read_Without_Delay(u32Address,pu32Data);
}


/*****************************************************************************
  Function:     FPGA_GetLogicVersion
  Description:  get logic version num
  Input:        none
  Output:       none
  Return:       logic version num:
                  [31:24]:Big Version Number
                  [23:16]:Small Version Number
                  [15: 0]:Test Version Number
  Author:       Momouwei 2017.04.24
  Modify:
*****************************************************************************/
U32 FPGA_GetLogicVersion(void)
{
    U32 u32Data = 0;
    U16 u16TestVer = 0;
    
    FPGA_REG_Read(VER_NUM, &u32Data);

    u16TestVer = (u32Data >> 16) & 0xFFFF;

    return (u32Data << 16) | u16TestVer;
}

/*****************************************************************************
  Function:     FPGA_GetSlotID
  Description:  get slot id
  Input:        none
  Output:       none
  Return:       none
  Author:
  Modify:
*****************************************************************************/
U32 FPGA_GetSlotID(void)
{
    U32 u32Data = 0;
    FPGA_REG_Read(REG_SLOT, &u32Data);

    if(u32Data > SLOT15)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_FPGA, "FPGA Get Slot %d\r\n", u32Data);
    }

    return u32Data;
}


/*****************************************************************************
  Function:     FPGA_GetTunerType
  Description:  get tuner type
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
U32 FPGA_GetTunerType(void)
{
    U32 u32Data = 0;
    FPGA_REG_Read(TUNER_TYPE_REG, &u32Data);
	printf("FPGA Tuner Type = %d", u32Data);

    return u32Data;
}


/*****************************************************************************
  Function:     FPGA_SetTunerReset
  Description:  reset tuner
  Input:        u8port   -- tuner port
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_SetTunerReset(U8 u8port)
{
    /*复位*/
    U32 u32Val = 0x0;
    
    REG_Read(TUNER_RESET_REG, &u32Val);

    FPGA_REG_Write(TUNER_RESET_REG, u32Val|(1<<u8port));
    usleep(200000);
    FPGA_REG_Write(TUNER_RESET_REG, u32Val&(~(1<<u8port)));
    usleep(500000);
    FPGA_REG_Write(TUNER_RESET_REG, u32Val|(1<<u8port));

}


/*****************************************************************************
  Function:     FPGA_SetMacAddr
  Description:  set mac addr to logic for filter
  Input:        u8port   -- tuner port
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_SetMacAddr(U8 *u8MacAddr)
{

    U32 u32MacAddrH = 0;
    U32 u32MacAddrL = 0;

    u32MacAddrH = (u8MacAddr[0] << 16)|(u8MacAddr[1] << 8)|(u8MacAddr[2]);
    u32MacAddrL = (u8MacAddr[3] << 16)|(u8MacAddr[4] << 8)|(u8MacAddr[5]);

    FPGA_REG_Write(TSIP_MAC_ADDH, u32MacAddrH);
    FPGA_REG_Write(TSIP_MAC_ADDL, u32MacAddrL);
}


/*****************************************************************************
  Function:     FPGA_GetMacAddr
  Description:  get mac addr
  Input:        *u8MacAddr   -- mac addr arry
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_GetMacAddr(U8 *u8MacAddr)
{
    U32 u32MacAddrH = 0;
    U32 u32MacAddrL = 0;

    FPGA_REG_Read(TSIP_MAC_ADDH, &u32MacAddrH);
    FPGA_REG_Read(TSIP_MAC_ADDL, &u32MacAddrL);

    u8MacAddr[0] = (u32MacAddrH >> 16) & 0xff;
    u8MacAddr[1] = (u32MacAddrH >> 8) & 0xff;
    u8MacAddr[2] = (u32MacAddrH >> 0) & 0xff;

    u8MacAddr[3] = (u32MacAddrL >> 16) & 0xff;
    u8MacAddr[4] = (u32MacAddrL >> 8) & 0xff;
    u8MacAddr[5] = (u32MacAddrL >> 0) & 0xff;
}


/*****************************************************************************
  Function:     FPGA_setTsipAddr
  Description:  get mac addr
  Input:        *u8TsipAddr  -- ip addr arry
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setTsipAddr(U8 *u8TsipAddr)
{
    U32 u32IpAddr = 0;
    u32IpAddr = (u8TsipAddr[3] << 24)|(u8TsipAddr[2] << 16)|(u8TsipAddr[1] << 8)|(u8TsipAddr[0]);

    FPGA_REG_Write(TSIP_IP_ADDR, u32IpAddr);

}



/*****************************************************************************
  Function:     FPGA_readCiInterrupt
  Description:  read CI interrupt event
  Input:        *u32IntStatus   -- poit of read date
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_readCiInterrupt(U32 *u32IntStatus)
{
    FPGA_REG_Read(CI_INT, u32IntStatus);
}


/*****************************************************************************
  Function:     FPGA_checkCiRegister
  Description:  check ci register
  Input:        *u32RegStatus   -- read date
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_checkCiRegister(U32 *u32RegStatus)
{
    FPGA_REG_Read(CI_CDD, u32RegStatus);
}


/*****************************************************************************
  Function:     FPGA_setCiPower
  Description:  set si Power
  Input:        slot   -- slot to set
                u8Status -- 1:power on; 0 : power off
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPower(U8 slot, U8 u8Status)
{
    U32 value = 0;
    
    if(slot > WVCI_MAX_SLOT_NUM)
    {
        return;
    }

    FPGA_REG_Read(CI_POW, &value);
    
    if(CI_POW_OFF == u8Status)
    {
        printf("**power off 0x%x \r\n", value&(~(1 << slot)) );
        FPGA_REG_Write(CI_POW, value&(~(1 << slot)));
    }
    else
    {
        printf("**power on 0x%x \r\n", value|(1 << slot) );
        FPGA_REG_Write(CI_POW, value|((1 << slot)));
    }

}


/*****************************************************************************
  Function:     FPGA_CiReset
  Description:  reset CI
  Input:        u8slot -- slot to reset
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_CiReset(U8 u8Slot)
{
    U32 u32value = 0;

    FPGA_REG_Read(CI_RST, &u32value);
    FPGA_REG_Write(CI_RST, u32value|(0x1 << u8Slot) );
    usleep(50);
    FPGA_REG_Write(CI_RST, u32value & (~(0x1 << u8Slot)));
    usleep(20);

}


/*****************************************************************************
  Function:     FPGA_setCiSelect
  Description:  set CI select register
  Input:        *u32sel   -- CI 0 or 1
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiSelect(U32 u32sel)
{
    if( u32sel > WVCI_MAX_SLOT_NUM)
    {
        return;
    }

    FPGA_REG_Write(CI_SEL, (1 << u32sel));
}


/*****************************************************************************
  Function:     FPGA_clearCiSelect
  Description:  clear CI select register
  Input:        u32sel   -- CI 0 or 1
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_clearCiSelect(U32 u32sel)
{
    FPGA_REG_Write(CI_SEL, 0);
}


/*****************************************************************************
  Function:     FPGA_CiReadEnable
  Description:  enable ci read IO/MEM
  Input:        u32Command   -- 100: IO read   110: mem read
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_CiReadEnable()
{
    FPGA_REG_Write(CI_RDEN, 1);
}


/*****************************************************************************
  Function:     FPGA_CiWriteAddr
  Description:  write addr to read or write
  Input:        u32Addr   -- addr for read or write data
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_CiWriteAddr(U32 u32Addr)
{
    FPGA_REG_Write(CI_ADDR, u32Addr);
}


/*****************************************************************************
  Function:     FPGA_CiReadCmd
  Description:  write read command reg
  Input:        u32Command   -- IO read  or MEM read
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_CiReadCmd(U32 u32Command)
{
    if( (u32Command != IO_READ) && (u32Command != MEM_READ) )
    {
        return;
    }

    FPGA_REG_Write(CI_CMD, u32Command);
}


/*****************************************************************************
  Function:     FPGA_CiReadDisable
  Description:  disable ci read
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_CiReadDisable(void)
{
    FPGA_REG_Write(CI_CMD, 0x0);
    FPGA_REG_Write(CI_RDEN, 0);
}


/*****************************************************************************
  Function:     FPGA_CiWriteEnable
  Description:  enable ci write IO/MEM
  Input:        u32Command   -- 101: IO read   111: mem read
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_CiWriteEnable(U32 u32Command)
{
    if( (u32Command != IO_WRITE) && (u32Command != MEM_WRITE) )
    {
        return;
    }

    FPGA_REG_Write(CI_CMD, u32Command);
    FPGA_REG_Write(CI_WREN, 1);
}


/*****************************************************************************
  Function:     FPGA_CiWriteDisable
  Description:  disable ci write
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_CiWriteDisable(void)
{
    FPGA_REG_Write(CI_CMD, 0x0);
    FPGA_REG_Write(CI_WREN, 0);
}


/*****************************************************************************
  Function:     FPGA_CiWrite
  Description:  CI write
  Input:        u32Addr -- the addr to write
                u32data -- data to write
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_CiWrite(U32 u32Addr, U32 u32data)
{

    FPGA_REG_Write(CI_ADDR, u32Addr);
    FPGA_REG_Write(CI_DATA, u32data);
}


/*****************************************************************************
  Function:     FPGA_CiRead
  Description:  CI read
  Input:        *u32data  -- data
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_CiRead(U32 *u32data)
{
    FPGA_REG_Read(CI_RDBK, u32data);
}


/*****************************************************************************
  Function:     FPGA_setCiClk
  Description:  set ci clock
  Input:        u32ClockValue  -- clock  value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiClk(U32 u32ClockValue)
{
    FPGA_REG_Write(CI_CLK_PAR , u32ClockValue);
}


void FPGA_setCAMbypassOn(U8 u8Slot)
{
	U32 u32ReadVal = 0;
    U32 u32Val = 0;
    u32Val = 0x1 << u8Slot;

    FPGA_REG_Read(CI_BYPASS, &u32ReadVal);

    u32ReadVal &= (~u32Val);
    
    FPGA_REG_Write(CI_BYPASS, u32ReadVal);

}


void FPGA_setCAMbypassOff(U8 u8Slot)
{
    U32 u32ReadVal = 0;
    U32 u32Val = 0;
    u32Val = 0x1 << u8Slot;

    FPGA_REG_Read(CI_BYPASS, &u32ReadVal);

    u32ReadVal |= u32Val;
    
    FPGA_REG_Write(CI_BYPASS, u32ReadVal);
}


/*****************************************************************************
  Function:     FPGA_setCAM0bypassOn
  Description:  CI set TS bypass cam 0
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCAM0bypassOn()
{
    FPGA_REG_Write(CI0_BYPASS, 0x0);
}

/*****************************************************************************
  Function:     FPGA_setCAM0bypassOff
  Description:  CI set TS to cam 0
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCAM0bypassOff()
{
    FPGA_REG_Write(CI0_BYPASS, 0x1);
}


/*****************************************************************************
  Function:     FPGA_setCAM1bypassOn
  Description:  CI set ts bypass cam 1
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCAM1bypassOn()
{
    FPGA_REG_Write(CI1_BYPASS, 0x0);
}


/*****************************************************************************
  Function:     FPGA_setCAM1bypassOff
  Description:  set ts to cam 1
  Input:        none
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCAM1bypassOff()
{
    FPGA_REG_Write(CI1_BYPASS, 0x1);
}

void FPGA_setCISid(U8 u8Slot, U32 u32StreamID)
{
    U32 u32ReadVal = 0;
    U32 u32Val = 0;

    u32Val = u32StreamID << (u8Slot * 8);

    FPGA_REG_Read(CI_SID_SEL, &u32ReadVal);

    u32ReadVal |= u32Val;
    
    FPGA_REG_Write(CI_SID_SEL, u32ReadVal);
    
}


/*****************************************************************************
  Function:     FPGA_setCI0Sid
  Description:  set stream to ci0
  Input:        u32Sid  -- stream id
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCI0Sid(U32 u32StreamID)
{
    FPGA_REG_Write(CI0_SID_SEL, u32StreamID);
}

/*****************************************************************************
  Function:     FPGA_setCI1Sid
  Description:  set stream to ci1
  Input:        u32Sid  -- stream id
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCI1Sid(U32 u32StreamID)
{
    FPGA_REG_Write(CI1_SID_SEL, u32StreamID);
}

/*****************************************************************************
  Function:     FPGA_getCiPidDetect0_A
  Description:  get ci pid detect 0 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect0_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT0_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect1_A
  Description:  get ci pid detect 1 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect1_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT1_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect2_A
  Description:  get ci pid detect 2 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect2_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT2_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect3_A
  Description:  get ci pid detect 3 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect3_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT3_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect4_A
  Description:  get ci pid detect 4 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect4_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT4_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect5_A
  Description:  get ci pid detect 5 of slot 0
  Input:        *u32Pid   -- point of pid valuet
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect5_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT5_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect6_A
  Description:  get ci pid detect 6 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect6_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT6_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect7_A
  Description:  get ci pid detect 7 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect7_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT7_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect0_B
  Description:  get ci pid detect 0 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect0_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT0_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect1_B
  Description:  get ci pid detect 1 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect1_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT1_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect2_B
  Description:  get ci pid detect 2 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect2_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT2_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect3_B
  Description:  get ci pid detect 3 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect3_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT3_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect4_B
  Description:  get ci pid detect 4 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect4_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT4_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect5_B
  Description:  get ci pid detect 5 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect5_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT5_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect6_B
  Description:  get ci pid detect 6 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect6_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT6_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect7_B
  Description:  get ci pid detect 7 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCiPidDetect7_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI_PID_DETECT7_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect0_A
  Description:   set ci pid detect 0 of slot 0
  Input:       *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect0_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT0_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect1_A
  Description:  set ci pid detect 1 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect1_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT1_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect2_A
  Description:   set ci pid detect 2 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect2_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT2_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect3_A
  Description:   set ci pid detect 3 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect3_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT3_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect4_A
  Description:   set ci pid detect 4 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect4_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT4_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect5_A
  Description:   set ci pid detect 5 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect5_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT5_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect6_A
  Description:   set ci pid detect 6 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect6_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT6_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect7_A
  Description:  set ci pid detect 7 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect7_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT7_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect0_B
  Description:  set ci pid detect 0 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect0_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT0_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect1_B
  Description:  set ci pid detect 1 of slot 1
  Input:       *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect1_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT1_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect2_B
  Description:  set ci pid detect 2 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect2_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT2_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect3_B
  Description:  set ci pid detect 3 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect3_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT3_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect4_B
  Description:  set ci pid detect 4 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect4_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT4_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect5_B
  Description:  set ci pid detect 5 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect5_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT5_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect6_B
  Description:  set ci pid detect 6 of slot 1
  Input:       *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect6_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT6_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect7_B
  Description:  set ci pid detect 7 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCiPidDetect7_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI_PID_DETECT7_B, *u32Pid);
}





/*****************************************************************************
  Function:     FPGA_getCiPidDetect0_A
  Description:  get ci pid detect 0 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect0_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT0_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect1_A
  Description:  get ci pid detect 1 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect1_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT1_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect2_A
  Description:  get ci pid detect 2 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect2_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT2_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect3_A
  Description:  get ci pid detect 3 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect3_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT3_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect4_A
  Description:  get ci pid detect 4 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect4_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT4_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect5_A
  Description:  get ci pid detect 5 of slot 0
  Input:        *u32Pid   -- point of pid valuet
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect5_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT5_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect6_A
  Description:  get ci pid detect 6 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect6_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT6_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect7_A
  Description:  get ci pid detect 7 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect7_A(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT7_A, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect0_B
  Description:  get ci pid detect 0 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect0_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT0_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect1_B
  Description:  get ci pid detect 1 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect1_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT1_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCi2PidDetect2_B
  Description:  get ci pid detect 2 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect2_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT2_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect3_B
  Description:  get ci pid detect 3 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect3_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT3_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect4_B
  Description:  get ci pid detect 4 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect4_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT4_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect5_B
  Description:  get ci pid detect 5 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect5_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT5_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect6_B
  Description:  get ci pid detect 6 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect6_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT6_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_getCiPidDetect7_B
  Description:  get ci pid detect 7 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getCi2PidDetect7_B(U32 *u32Pid)
{
    FPGA_REG_Read(CI2_PID_DETECT7_B, u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect0_A
  Description:   set ci pid detect 0 of slot 0
  Input:       *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect0_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT0_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect1_A
  Description:  set ci pid detect 1 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect1_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT1_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect2_A
  Description:   set ci pid detect 2 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect2_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT2_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect3_A
  Description:   set ci pid detect 3 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect3_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT3_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect4_A
  Description:   set ci pid detect 4 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect4_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT4_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect5_A
  Description:   set ci pid detect 5 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect5_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT5_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect6_A
  Description:   set ci pid detect 6 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect6_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT6_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect7_A
  Description:  set ci pid detect 7 of slot 0
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect7_A(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT7_A, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect0_B
  Description:  set ci pid detect 0 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect0_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT0_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect1_B
  Description:  set ci pid detect 1 of slot 1
  Input:       *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect1_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT1_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect2_B
  Description:  set ci pid detect 2 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect2_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT2_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect3_B
  Description:  set ci pid detect 3 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect3_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT3_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect4_B
  Description:  set ci pid detect 4 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect4_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT4_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect5_B
  Description:  set ci pid detect 5 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect5_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT5_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect6_B
  Description:  set ci pid detect 6 of slot 1
  Input:       *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect6_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT6_B, *u32Pid);
}


/*****************************************************************************
  Function:     FPGA_setCiPidDetect7_B
  Description:  set ci pid detect 7 of slot 1
  Input:        *u32Pid   -- point of pid value
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_setCi2PidDetect7_B(U32 *u32Pid)
{
    FPGA_REG_Write(CI2_PID_DETECT7_B, *u32Pid);
}







/*****************************************************************************
  Function:     FPGA_getTunerTotalBitRate
  Description:  get tuner total bitrate
  Input:        u8port   -- port of tuner
                *u32totalBitrate -- point to data 
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getTunerTotalBitRate(U8 u8port, U32 *u32totalBitrate)
{
    U32 u32Value = 0;
    
    FPGA_REG_Write(STR_MMNT_TIMER, CNT_135_M);
    FPGA_REG_Write(STR_MMNT_ADDR1, TUNER_TS_INDEX + u8port);

    FPGA_REG_Read(STR_MMNT_TOTLR, &u32Value);
    
    *u32totalBitrate = PACKET_2_BIT(u32Value);
}


/*****************************************************************************
  Function:     FPGA_getTunerEffectBitRate
  Description:  get tuner effect bitrate
  Input:        u8port   -- port of tuner
                *u32EffectBitrate -- point to data 
  Output:       none
  Return:       none
  Author:       huada.huang
  Modify:
*****************************************************************************/
void FPGA_getTunerEffectBitRate(U8 u8port, U32 *u32EffectBitrate)
{
    U32 u32Value = 0 ;

    FPGA_REG_Write(STR_MMNT_TIMER, CNT_135_M);
    FPGA_REG_Write(STR_MMNT_ADDR1, TUNER_TS_INDEX + u8port);

    FPGA_REG_Read(STR_MMNT_EFFER, &u32Value);
    
    *u32EffectBitrate = PACKET_2_BIT(u32Value);
}

/*****************************************************************************
  Function:     FPGA_GetPIDBitRate
  Description:  获取PID的码率，时钟频率和流ID必须事先设置好(在获取整个流或节目的所有PID时，为了节省时间)
  Input:        u16PID
  Output:       none
  Return:       PID的码率
  Author:       Momouwei 2017.04.17
*****************************************************************************/
U32 FPGA_GetPIDBitRate(U16 u16PID)
{
    U32 u32Value = 0;

    FPGA_REG_Write(STR_MMNT_ADDR0, u16PID);
    FPGA_REG_Read(STR_MMNT_PID_RATE, &u32Value);
    
    return PACKET_2_BIT(u32Value);
}

/*****************************************************************************
  Function:     FPGA_SendSIPacket2Logic
  Description:  send data to subboard
  Input:        pu32Data   : the data will be send
                u32DataLen : data length
  Output:       none
  Return:       none
  Others:
  Create:       lipeng 20170319
*****************************************************************************/
void FPGA_SendSIPacket2Logic(U32 *pu32Data, U32 u32DataLen)
{
    U32 i = 0;
    
    if (!pu32Data)
    {
       return;
    }
    
    for (i = 0; i < u32DataLen; i++)
    {
        FPGA_REG_Write_Without_Delay(PSI_CPU_ADDR, i);
        FPGA_REG_Write_Without_Delay(PSI_CPU_DATA, pu32Data[i]);
        FPGA_REG_Write_Without_Delay(PSI_CPU_WREN, 0);
        FPGA_REG_Write_Without_Delay(PSI_CPU_WREN, 1);
        FPGA_REG_Write_Without_Delay(PSI_CPU_WREN, 0);
    }
    
    FPGA_REG_Write_Without_Delay(PSI_CPU_OVER, 0);
    FPGA_REG_Write_Without_Delay(PSI_CPU_OVER, 1);
    FPGA_REG_Write_Without_Delay(PSI_CPU_OVER, 0);
}


