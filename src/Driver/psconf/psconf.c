/**********************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName   PSCONF.c
* Description : The Module of Control QAM160T
* Author    : jie.zhan
* Modified  :
* Reviewer  :
* Date      : 2017-02-26
* Record    :
*
**********************************************************************/
#include "psconf/psconf.h"

#include "fpga/FPGA.h"
#include "wv_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*****************************************************************************
  Function:     PSConf_FWConfigGeneralFPGA32Bit
  Description:  配置FPGA接口，7Z020将数据通过逻辑接口传输给160T
  Input:        u8PucData            --- 写数据buffer
                nDataLen             --- 数据长度
                nPlatform            --- ALTERA或者XILINX平台
  Output:       none
  Return:       WV_SUCCESS           --- 成功
                WV_ERR_FAILURE       --- 返回失败
                WV_ERR_PARAMS        --- 参数错误
  Author:       shouquan.tang
  Modify:       jie.zhan
*****************************************************************************/
wvErrCode PSConf_FWConfigGeneralFPGA32Bit( U8 *u8PucData, U32 nDataLen, U32 nPlatform)
{
    if(NULL == u8PucData)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DRIVERS, "Input Parameter is NULL. \n");
        return WV_ERR_PARAMS;
    }

    // VARS
    U32    ii = 0;
    U32    jj = 0;
    U32    u32Addr       = 0;
    U32    Send32BitData = 0;
    U32    u32Offset     = 0;

    U32    u32MaxContinueBusy = 0;
    U32    u32TempData        = 0;
    U32    u32PlatformMask    = 0;
    U32    u32SubBoardIndex   = 0;
    U32    u32CCount          = 0;

    U8     aru8TempData[4];

    memset((U8 *)aru8TempData, 0, sizeof(aru8TempData));

    //平台选择
    if (nPlatform == ALTERA_PLATFORM)
    {
        u32PlatformMask = PSCONF_PLATFORM_ALTERA_MASK;
    }
    else
    {
        u32PlatformMask = PSCONF_PLATFORM_XILINX_MASK;
    }

    //默认槽号是0
    u32PlatformMask = u32PlatformMask | 0x01000000;

    // 复位PSCONFIG
    (void)FPGA_REG_Write(REG_PSCONF_ADDR, 0);  //reset psconfig
    (void)usleep(1);

    // 开始初始化
    (void)FPGA_REG_Write(REG_PSCONF_ADDR, (PSCONF_START_MASK | u32PlatformMask));
    (void)usleep(200000);
    u32Addr = 0;

    // Send 256 Unit
    for (ii = 0; ii < PSCONF_SENDUNIT_LENGTH; ii++)
    {
        //防止数组越界处理
        if ((u32Offset + 4) <= nDataLen)
        {
            Send32BitData = ((U32)u8PucData[u32Offset+3] << 24)| ((U32)u8PucData[u32Offset+2] << 16)| ((U32)u8PucData[u32Offset+1] << 8) | u8PucData[u32Offset];
        }
        else if (u32Offset >= nDataLen)
        {
            Send32BitData = 0;
        }
        else
        {
            for (jj = 0; jj < 4; jj++)
            {
                if ((u32Offset+jj) < nDataLen)
                {
                    aru8TempData[jj] = u8PucData[u32Offset+jj];
                }
                else
                {
                    aru8TempData[jj] = 0;
                }
            }

            Send32BitData = ((U32)aru8TempData[3] << 24)| ((U32)aru8TempData[2] << 16)| ((U32)aru8TempData[1] << 8) | aru8TempData[0];
        }
        u32Offset += 4;

        (void)FPGA_REG_Write(REG_PSCONF_DATA, Send32BitData);
        (void)FPGA_REG_Write(REG_PSCONF_ADDR, (u32Addr | PSCONF_START_MASK | u32PlatformMask));
        (void)FPGA_REG_Write(REG_PSCONF_ENABLE, 1);
        (void)FPGA_REG_Write(REG_PSCONF_ENABLE, 0);
        u32Addr++;
    }

    do
    {
        // Check The First 0~127 Block Free Or Not
        u32MaxContinueBusy = 0;
        do
        {
            u32MaxContinueBusy++;
            if (u32MaxContinueBusy > 10000)
            {
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "PSCONF_ERROR_NO_ACK --> REG_PSCONF_BUSY!! \n");
                return WV_ERR_CONFIG_TIMEOUOT;
            }
            
            (void)FPGA_REG_Read(REG_PSCONF_BUSY, &u32TempData);
        } while ((u32TempData & 0x01) == 0x01); // 1: free  0: busy
        
        // Send 128 Unit
        for (ii = 0; ii < (PSCONF_SENDUNIT_LENGTH / 2); ii++)
        {
            // 防止数组越界处理
            if ((u32Offset + 4) <= nDataLen)
            {
                Send32BitData = ((U32)u8PucData[u32Offset+3] << 24)| ((U32)u8PucData[u32Offset+2] << 16)| ((U32)u8PucData[u32Offset+1] << 8) | u8PucData[u32Offset];
            }
            else if (u32Offset >= nDataLen)
            {
                Send32BitData = 0;
            }
            else
            {
                for (jj = 0; jj < 4; jj++)
                {
                    if ((u32Offset+jj) < nDataLen)
                    {
                        aru8TempData[jj] = u8PucData[u32Offset+jj];
                    }
                    else
                    {
                        aru8TempData[jj] = 0;
                    }
                }

                Send32BitData = ((U32)aru8TempData[3] << 24)| ((U32)aru8TempData[2] << 16)| ((U32)aru8TempData[1] << 8) | aru8TempData[0];
            }
            u32Offset += 4;

            (void)FPGA_REG_Write(REG_PSCONF_DATA, Send32BitData);
            (void)FPGA_REG_Write(REG_PSCONF_ADDR, (u32Addr | PSCONF_START_MASK | u32PlatformMask));
            (void)FPGA_REG_Write(REG_PSCONF_ENABLE, 1);
            (void)FPGA_REG_Write(REG_PSCONF_ENABLE, 0);
            u32Addr++;
        }

        // Check The First 128~255 Block Free Or Not
        u32MaxContinueBusy = 0;
        do
        {
            u32MaxContinueBusy++;
            if (u32MaxContinueBusy > 10000)
            {
                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "PSCONF_ERROR_NO_ACK --> REG_PSCONF_BUSY!! \n");
                return WV_ERR_CONFIG_TIMEOUOT;
            }
            (void)FPGA_REG_Read(REG_PSCONF_BUSY, &u32TempData);
            
        } while ((u32TempData & 0x02) == 0x02); // 1: free  0: busy
        
        // Send 128 Unit
        for (ii = (PSCONF_SENDUNIT_LENGTH / 2); ii < PSCONF_SENDUNIT_LENGTH; ii++)
        {
            //防止数组越界处理
            if ((u32Offset + 4) <= nDataLen)
            {
                Send32BitData = ((U32)u8PucData[u32Offset+3] << 24)| ((U32)u8PucData[u32Offset+2] << 16)| ((U32)u8PucData[u32Offset+1] << 8) | u8PucData[u32Offset];
            }
            else if (u32Offset >= nDataLen)
            {
                Send32BitData = 0;
            }
            else
            {
                for (jj = 0; jj < 4; jj++)
                {
                    if ((u32Offset+jj) < nDataLen)
                    {
                        aru8TempData[jj] = u8PucData[u32Offset+jj];
                    }
                    else
                    {
                        aru8TempData[jj] = 0;
                    }
                }

                Send32BitData = ((U32)aru8TempData[3] << 24)| ((U32)aru8TempData[2] << 16)| ((U32)aru8TempData[1] << 8) | aru8TempData[0];
            }
            u32Offset += 4;

            (void)FPGA_REG_Write(REG_PSCONF_DATA, Send32BitData);
            (void)FPGA_REG_Write(REG_PSCONF_ADDR, (u32Addr | PSCONF_START_MASK | u32PlatformMask));
            (void)FPGA_REG_Write(REG_PSCONF_ENABLE, 1);
            (void)FPGA_REG_Write(REG_PSCONF_ENABLE, 0);
            u32Addr++;
        }

		//打印加载160T信息
		u32CCount++;
		if(0 == u32CCount % 500)
		{
			LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "nDataLen = %d u32Offset = %d\n", nDataLen, u32Offset);
		}

		if(0xFFFFFFFF == u32CCount)
		{
			u32CCount = 0;
		}

    } while (u32Offset < nDataLen);

    // 获取下载完成状态
    (void)usleep(2000);
    (void)FPGA_REG_Read(REG_PSCONF_DONE, &u32TempData);
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "nConfigDone (0x%02X)!! \n", u32TempData);

    if (((u32TempData >> u32SubBoardIndex) & 0x01) == 0x01)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_DRIVERS, "Download Firmware Success!(%d) \n", nDataLen);
        return WV_SUCCESS;
    }
    
    LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_DRIVERS, "PSCONF_FAILURE nConfigDone!! \n", nDataLen);
    return WV_ERR_FAILURE;
}

