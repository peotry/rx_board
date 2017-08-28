/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     IPconfig.c
* Description:  IP manage and configure
* Author    :   ruibin.zhang
* Modified  :
* Reviewer  :
* Date      :   2017-03-04
* Record    :
*
**********************************************************************/

#include "phy/phy.h"
#include "xil_io.h"

#include "WV_log.h"

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/msg.h>

void *s_pGEM1LogicAddress;
void *s_pGEM2LogicAddress;


/*
* function: PHY_RegMap
*
* description: PHY 地址映射
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
void PHY_RegMap(void)
{
    S32 s32Memfd = 0;

    s32Memfd = open("/dev/mem", O_RDWR | O_SYNC);
    if(s32Memfd == -1)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DRIVERS, "Error: Can't open /dev/mem.\n");
        return;
    }

    s_pGEM1LogicAddress = mmap(0, 0xfff, PROT_READ | PROT_WRITE, MAP_SHARED, s32Memfd, GEM1_NET_CONFIG_BASEADRESS);
    if(s_pGEM1LogicAddress == (void *) -1)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DRIVERS, "Error: Can't map the memory to user space.\n");
        return;
    }

    s_pGEM2LogicAddress = mmap(0, 0xfff, PROT_READ | PROT_WRITE, MAP_SHARED, s32Memfd, GEM2_NET_CONFIG_BASEADRESS);
    if(s_pGEM2LogicAddress == (void *) -1)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DRIVERS, "Error: Can't map the memory to user space.\n");
        return;
    }
}


/*
* function: PHY_SetLocalMac
*
* description: 设置mac 地址
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode PHY_SetLocalMac(U8 * pu8MAC, EthType_enum emEthType)
{
    if(NULL == pu8MAC)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DRIVERS, "Error: Param is NULL!");
        return WV_ERR_PARAMS;
    }

    U8 u8Cmd[MAX_COMMAND_LENGTH]     = { 0 };
    U8 pu8MACS[MAX_COMMAND_LENGTH]   = { 0 };

    snprintf( (char*)pu8MACS, MAX_COMMAND_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x",
            pu8MAC[0],pu8MAC[1],pu8MAC[2],pu8MAC[3],pu8MAC[4],pu8MAC[5]);

    snprintf( (char*)u8Cmd, MAX_COMMAND_LENGTH, "ifconfig eth%d down", emEthType);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "u8CMD: %s\n", u8Cmd);
    system( (char*)u8Cmd );

    memset(u8Cmd, 0, MAX_COMMAND_LENGTH);
    snprintf( (char*)u8Cmd, MAX_COMMAND_LENGTH, "ifconfig eth%d hw ether %s", emEthType, pu8MACS);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "u8CMD: %s\n", u8Cmd);
    system( (char*)u8Cmd );

    memset(u8Cmd, 0, MAX_COMMAND_LENGTH);
    snprintf( (char*)u8Cmd, MAX_COMMAND_LENGTH, "ifconfig eth%d up", emEthType);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "u8CMD: %s\n", u8Cmd);
    system( (char*)u8Cmd );

    //TODO
    //写MAC地址到对应逻辑寄存器
    //FPGA_SetMacAddr(pu8MAC, emEthType);

    return WV_SUCCESS;
}


/*
* function: PHY_SetEth
*
* description: 设置网口相关寄存器
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
BOOL PHY_SetEth(EthType_enum emEthType)
{
    U16 PhyReg0   = 0;
    U16 PhyReg8   = 0;
    U32 Status    = 0;

    U16 PhyReg2   = 0;
    U16 PhyReg3   = 0;
    U32 u32Id     = 0;

    //控制Rgmi数据时钟对其方式,要重启PHY才能生效
    Status  = XEmacPs_PhyRead(emEthType, 0, 20, &PhyReg0);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "**** read Eth%d PhyReg0 0x%x\r\n",emEthType, PhyReg0);
    
    PhyReg0 |= 0x7a;
    Status = XEmacPs_PhyWrite(emEthType, 0, 20, PhyReg0);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "**** write Eth%d PhyReg0 0x%x\r\n",emEthType, PhyReg0);

    Status  = XEmacPs_PhyRead(emEthType, 0, 20, &PhyReg0);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "**** read Eth%d PhyReg0 0x%x\r\n",emEthType, PhyReg0);

    //设置PHY为自动协商模式，要重启PHY才能生效
    Status  = XEmacPs_PhyRead(emEthType,0, 0, &PhyReg0);
    PhyReg0 |= 0x1000;
    Status = XEmacPs_PhyWrite(emEthType, 0, 0, PhyReg0);

    Status  = XEmacPs_PhyRead(emEthType, 0, 0, &PhyReg0);
    
    //PHY0最高位为1，则重启PHY
    PhyReg0 |= 0x8000;
    Status = XEmacPs_PhyWrite(emEthType, 0, 0, PhyReg0);

    /*读取当前PHY的速率模式
    bit 15:14
    11 = Reserved
    10 = 1000Mbps
    01 = 100Mbps
    00 = 10Mbps
    */
    Status  = XEmacPs_PhyRead(emEthType, 0, 17, &PhyReg0);

    Status  = XEmacPs_PhyRead(emEthType, 8, 16, &PhyReg8);
    
    if((PhyReg0 & 0xC000) == 0x8000)
    {
        //1000M
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "***** 1000M *****\r\n");
        PhyReg8 |= 0x40;
        PhyReg8 &= ~0x2000;
    }
    else if((PhyReg0 & 0xC000) == 0x4000)
    {
        //100M
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "***** 100M *****\r\n");
        PhyReg8 |= 0x2000;
        PhyReg8 &= ~0x40;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DRIVERS, "***** speed mode error *****\r\n");
        return FALSE;
    }

    //把速率模式写到PHY8
    Status  = XEmacPs_PhyWrite(emEthType,8, 16, PhyReg8);

    Status  = XEmacPs_PhyRead(emEthType, 0, 0, &PhyReg0);
    //PHY0最高位为1，则重启PHY
    PhyReg0 |= 0x8000;
    Status = XEmacPs_PhyWrite( emEthType,0, 0, PhyReg0);
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "***** Eth%d Status = %u *****\r\n", emEthType, Status );

    XEmacPs_PhyRead(emEthType, 0, 2, &PhyReg2);
    XEmacPs_PhyRead(emEthType, 0, 3, &PhyReg3);

    u32Id = PhyReg2;
    u32Id = u32Id << 6;
    u32Id = u32Id | (PhyReg3 >> 10);
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "***** Eth%d chip id 0x%x*****\r\n", emEthType, u32Id );

    XEmacPs_PhyRead(emEthType, 0, 0, &PhyReg0);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "Eth%d Page0 Reg0: 0x%x", emEthType, PhyReg0);

    XEmacPs_PhyRead(emEthType, 0, 1, &PhyReg0);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "Eth%d Page0 Reg1: 0x%x", emEthType, PhyReg0);

    XEmacPs_PhyRead(emEthType, 0, 4, &PhyReg0);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "Eth%d Page0 Reg4: 0x%x", emEthType, PhyReg0);

    XEmacPs_PhyRead(emEthType, 0, 9, &PhyReg0);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "Eth%d Page0 Reg9: 0x%x", emEthType, PhyReg0);
    
    XEmacPs_PhyRead(emEthType, 0, 20, &PhyReg0);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "Eth%d Page0 Reg20: 0x%x", emEthType, PhyReg0);

    XEmacPs_PhyRead(emEthType, 0, 27, &PhyReg0);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "Eth%d Page0 Reg27: 0x%x", emEthType, PhyReg0);


    return TRUE;
}

/*****************************************************************************
  Function:     PHY_Init
  Description:  PHY芯片初始化
  Input:        none
  Output:       none
  Return:       none
  Author:       jie.zhan
*****************************************************************************/
void PHY_Init(void)
{
    //映射PHY MAP地址
    PHY_RegMap();
    
    //设置网卡eth0和eth1
    PHY_SetEth(ETH0);
    PHY_SetEth(ETH1);
}

/*****************************************************************************
  Function:     PHY_SetIP
  Description:  设置网卡0的IP地址
  Input:        none
  Output:       none
  Return:       none
  Author:       jie.zhan
*****************************************************************************/
void PHY_SetIP(U32 u32IP)
{
    U8 u8IPBuf[60] = {0};
    U8 *u8data     = (U8*)&u32IP;
    sprintf((char *)&u8IPBuf[0],"ifconfig eth0 %d.%d.%d.%d netmask 255.255.255.0", u8data[3], u8data[2], u8data[1], u8data[0]);
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "***** Debug = %s *****\r\n", u8IPBuf);

    //配置eth0
    system("ifconfig eth0 down");
    system((char *)u8IPBuf);
    system("ifconfig eth0 up");
}

//TODO 自动获取IP，还需要进行优化
void IPconfig_GetFreeIP(U32 * pu32Ip)
{
    if(NULL == pu32Ip)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DRIVERS, "Input Parameter is NULL. \n");
        return ;
    }
    
    //192.168.3.15
    *pu32Ip = 0xc0a8030f;
}

