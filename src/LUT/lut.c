/*
 * =====================================================================================
 *
 *       Filename:  lut.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/27/17 20:13:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  linsheng.pan (), life_is_legend@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "lut.h"
#include "TSConstruct.h"
#include "WV_log.h"
#include "FPGA.h"


wvErrCode LUT_MultiIpMapMAC(const U32 u32IP, U8 aru8MAC[6])
{
	//多播MAC地址中的高25位是固定的，低23位是可变的。
	aru8MAC[0] = 0x01;
	aru8MAC[1] = 0;
	aru8MAC[2] = 0x5E;

	aru8MAC[3] = ((u32IP >> 16) & 0xef);
	aru8MAC[4] = ((u32IP >> 8) & 0xff);
	aru8MAC[5] = (u32IP & 0xff);
	
	return WV_SUCCESS;
}

wvErrCode LUT_SetInputLUTIPInfo(U8 u8LUTIndex, U32 u32IP, U16 u16UDPPort)
{
	U32 u32MACH = 0;
    U16 u16MACL = 0;
	U8 aru8MAC[6] = {0};
    
    if ((MAX_INPUT_LUT_NUM <= u8LUTIndex) || (!aru8MAC))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:u8LUTIndex[%u],aru8MAC[%p]\r\n",
            __FUNCTION__, __LINE__, u8LUTIndex, aru8MAC);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    FPGA_REG_Write(TSIP_CPU_ADDR, (0x1 << 9) | u8LUTIndex);
    FPGA_REG_Write(TSIP_DAT_H, 0);
    FPGA_REG_Write(TSIP_DAT_L, u32IP);
    FPGA_REG_Write(TSIP_CPU_WREN, 0);
    FPGA_REG_Write(TSIP_CPU_WREN, 1);
    FPGA_REG_Write(TSIP_CPU_WREN, 0);

	//将ip地址映射为mac
	LUT_MultiIpMapMAC(u32IP, aru8MAC);

    u32MACH = (aru8MAC[0] << 24) | (aru8MAC[1] << 16) | (aru8MAC[2] << 8) | aru8MAC[3];
    u16MACL = (aru8MAC[4] << 8) | aru8MAC[5]; 

    FPGA_REG_Write(TSIP_CPU_ADDR, (0x1 << 9) | (0x1 << 8) | u8LUTIndex);
    FPGA_REG_Write(TSIP_DAT_H, u32MACH);
    FPGA_REG_Write(TSIP_DAT_L, (u16MACL << 16) | u16UDPPort);
    FPGA_REG_Write(TSIP_CPU_WREN, 0);
    FPGA_REG_Write(TSIP_CPU_WREN, 1);
    FPGA_REG_Write(TSIP_CPU_WREN, 0);

    return WV_SUCCESS;
}


wvErrCode LUT_OpenStream(U8 u8LUTIndex, U8 u8StreamID)
{
	wvErrCode ret = WV_SUCCESS;

	FPGA_REG_Write(LUT_ADDR, ((u8LUTIndex << 12) | (0x1 << 11) | u8StreamID));
	FPGA_REG_Write(LUT_DATA, 3);
	FPGA_REG_Write(LUT_WRITE_EN, 0);
	FPGA_REG_Write(LUT_WRITE_EN, 1);
	FPGA_REG_Write(LUT_WRITE_EN, 0);

	return ret;
}

wvErrCode LUT_CloseStream(U8 u8LUTIndex, U8 u8StreamID)
{
	wvErrCode ret = WV_SUCCESS;

	FPGA_REG_Write(LUT_ADDR, ((u8LUTIndex << 12) | (0x1 << 11) | u8StreamID));
	FPGA_REG_Write(LUT_DATA, 0);
	FPGA_REG_Write(LUT_WRITE_EN, 0);
	FPGA_REG_Write(LUT_WRITE_EN, 1);
	FPGA_REG_Write(LUT_WRITE_EN, 0);

	return ret;
}


wvErrCode LUT_ClearInputLUT(U8 u8LUTIndex)
{
	U32 i = 0;
	for (i = 0; i < MAX_INPUT_LUT_ENTRY_NUM; i++)
	{
	   FPGA_REG_Write(LUT_ADDR, (u8LUTIndex << 12) | i);
	   FPGA_REG_Write(LUT_DATA, 0);
	   FPGA_REG_Write(LUT_WRITE_EN, 0);
	   FPGA_REG_Write(LUT_WRITE_EN, 1);
	   FPGA_REG_Write(LUT_WRITE_EN, 0);
	}

	FPGA_REG_Write(LUT_SWAP, 0);
	FPGA_REG_Write(LUT_SWAP, 1);
	FPGA_REG_Write(LUT_SWAP, 0);

	return WV_SUCCESS;
}


wvErrCode LUT_GetInputLUTEntry(U8 u8DestSlot, TSInfo *pstParamTS, INPUTLutEntry arstLUTEntry[MAX_INPUT_LUT_ENTRY_NUM], U16 *pu16EntryNum)
{
	wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U16 j = 0;
    U16 k = 0;
    U16 u16InTSIndex = INDEX_INVALID_VALUE;
    U16 u16OutTSIndex = INDEX_INVALID_VALUE;
    U16 u16EntryNum = 0;
    IndexInfoList stIndexList;
    IndexInfoList stESIndexList;
    IndexInfoList stECMIndexList;
    U16 u16SrcTSIndex = INDEX_INVALID_VALUE;
    U16 u16SrcProgIndex = INDEX_INVALID_VALUE;
    U16 u16Index = INDEX_INVALID_VALUE;
    U16 u16PIDIndex = INDEX_INVALID_VALUE;
    U16 u16StreamID = 0;
    INPUTLutEntry stTempEntry;
    U8 u8CamIndex = 0;
    U16 u16Channel = 0;
	PATTable_t stPATTable;
	IndexInfoList stIndexInfoList;

	u8CamIndex = u8DestSlot;
	u16StreamID = TSP_InputChannel2StreamID(u8CamIndex);

	//获取通道所有表的PID
	//PAT
	stTempEntry.u16SrcStream = u16StreamID;
    stTempEntry.u16SrcPID = 0;
	//Entry 是否已经存在
    if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
        && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
    {
        memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
    }
	//SDT
	stTempEntry.u16SrcStream = u16StreamID;
    stTempEntry.u16SrcPID = 17;
	//Entry 是否已经存在
    if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
        && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
    {
        memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
    }

	//PMT
	//找到对应通道的TS结构体
	U16 u16TSIndex = 0;
	for(i = 0; i < MAX_INPUT_CHANNEL_NUM; ++i)
	{
		if(u8DestSlot == pstParamTS->pInputTS[i].u16ChannelID)
		{
			u16TSIndex = i;
			break;
		}
	}
	printf("Ts Index = %u\n", pstParamTS->pInputTS[u16TSIndex].u16Index);
	printf("Channel Id = %u\n", pstParamTS->pInputTS[u16TSIndex].u16ChannelID);
	printf("Program Index = %u\n", pstParamTS->pInputTS[u16TSIndex].u16ProgramIndex);
	TSP_GetInTSProgIndexList(pstParamTS->pInputTS[u16TSIndex].u16Index, pstParamTS, &stIndexInfoList);
	for(j = 0; j < stIndexInfoList.u16IndexListNum; ++j)
	{
		printf("PMT PID = %u\n", pstParamTS->pInputProgram[stIndexInfoList.aru16IndexList[j]].u16PMTPID);
		stTempEntry.u16SrcStream = u16StreamID;
    	stTempEntry.u16SrcPID = pstParamTS->pInputProgram[stIndexInfoList.aru16IndexList[j]].u16PMTPID;
		//Entry 是否已经存在
	    if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
	        && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
	    {
	        memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
	    }
	}

	//获取通道所有PCR和ES PID

	u16Channel = u8DestSlot;
	//for (u16Channel = 0; u16Channel < MAX_INPUT_CHANNEL_NUM; u16Channel++)
    {
        if (WV_SUCCESS != TSP_GetInputTSIndex(u16Channel, pstParamTS, &u16InTSIndex))
        {
            return WV_ERR_FAILURE;
        }

        if (TSP_IfInputTSBypasses2DestSlot(u16InTSIndex, pstParamTS, u8DestSlot))
        {
			return WV_SUCCESS;
        }

        //Prog
        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInTSProgIndexList(u16InTSIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode) 
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetInTSProgIndexList Error:enErrCode[%08X],u16InTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u16InTSIndex);

            return enErrCode;
        }

        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            u16Index = stIndexList.aru16IndexList[i];

            if ((!TSP_IfInputProgOutputs2DestSlot(u16Index, pstParamTS, u8DestSlot)))
            {
                continue;
            }

			//节目送去解扰
            if (TSP_IsProgSet2BeDescrambled(u16Index, pstParamTS))
            {
                continue;
            }

            stTempEntry.u16SrcStream = u16StreamID;
            stTempEntry.u16SrcPID = pstParamTS->pInputProgram[u16Index].u16PCRPID;

            if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
            {
                memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
            }

            stESIndexList.u16IndexListNum = 0;
            enErrCode = TSP_GetInProgESPIDIndexList(u16Index, pstParamTS, &stESIndexList);
            if (WV_SUCCESS != enErrCode) 
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetInProgESPIDIndexList Error:enErrCode[%08X],u16Index[%u]\r\n",
                    __FUNCTION__, __LINE__, enErrCode, u16Index);

                return enErrCode;
            }

            for (j = 0; j < stESIndexList.u16IndexListNum; j++)
            {
                u16PIDIndex = stESIndexList.aru16IndexList[j];
                
                stTempEntry.u16SrcStream = u16StreamID;
                stTempEntry.u16SrcPID = pstParamTS->pInputPID[u16PIDIndex].u16PID;
    
                if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                    && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
                {
                    memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
                }

                stECMIndexList.u16IndexListNum = 0;
                enErrCode = TSP_GetInPIDESECMPIDIndexList(u16PIDIndex, pstParamTS, &stECMIndexList);
                if (WV_SUCCESS != enErrCode) 
                {
                    log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                        "[%s:%d]TSP_GetInPIDESECMPIDIndexList Error:enErrCode[%08X],u16PIDIndex[%u]\r\n",
                        __FUNCTION__, __LINE__, enErrCode, u16PIDIndex);

                    return enErrCode;
                }

                for (k = 0; k < stECMIndexList.u16IndexListNum; k++)
                {
                    u16PIDIndex = stECMIndexList.aru16IndexList[k];
                    
                    stTempEntry.u16SrcStream = u16StreamID;
                    stTempEntry.u16SrcPID = pstParamTS->pInputPID[u16PIDIndex].u16PID;
        
                    if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                        && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
                    {
                        memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
                    }
                }
            }

            stECMIndexList.u16IndexListNum = 0;
            enErrCode = TSP_GetInProgPSECMPIDIndexList(u16Index, pstParamTS, &stECMIndexList);
            if (WV_SUCCESS != enErrCode) 
            {
                log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                    "[%s:%d]TSP_GetInProgPSECMPIDIndexList Error:enErrCode[%08X],u16Index[%u]\r\n",
                    __FUNCTION__, __LINE__, enErrCode, u16Index);

                return enErrCode;
            }

            for (j = 0; j < stECMIndexList.u16IndexListNum; j++)
            {
                u16PIDIndex = stECMIndexList.aru16IndexList[j];
                
                stTempEntry.u16SrcStream = u16StreamID;
                stTempEntry.u16SrcPID = pstParamTS->pInputPID[u16PIDIndex].u16PID;
    
                if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                    && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
                {
                    memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
                }
            }
        }

        //EMM
        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInTSEMMPIDIndexList(u16InTSIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode) 
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetInTSEMMPIDIndexList Error:enErrCode[%08X],u16InTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u16InTSIndex);

            return enErrCode;
        }

        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            u16Index = stIndexList.aru16IndexList[i];
            
            stTempEntry.u16SrcStream = u16StreamID;
            stTempEntry.u16SrcPID = pstParamTS->pInputPID[u16Index].u16PID;

            if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
            {
                memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
            }
        }

        //OtherPID
        stIndexList.u16IndexListNum = 0;
        enErrCode = TSP_GetInTSOtherPIDIndexList(u16InTSIndex, pstParamTS, &stIndexList);
        if (WV_SUCCESS != enErrCode) 
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_GetInTSOtherPIDIndexList Error:enErrCode[%08X],u16InTSIndex[%u]\r\n",
                __FUNCTION__, __LINE__, enErrCode, u16InTSIndex);

            return enErrCode;
        }

        for (i = 0; i < stIndexList.u16IndexListNum; i++)
        {
            u16Index = stIndexList.aru16IndexList[i];
            
            stTempEntry.u16SrcStream = u16StreamID;
            stTempEntry.u16SrcPID = pstParamTS->pInputPID[u16Index].u16PID;

            if (!TSP_IsInputLUTEntryAlreadyExist(&stTempEntry, arstLUTEntry, u16EntryNum)
                && (u16EntryNum < MAX_INPUT_LUT_ENTRY_NUM))
            {
                memcpy(&arstLUTEntry[u16EntryNum++], &stTempEntry, sizeof(INPUTLutEntry));
            }
        }
    }

	*pu16EntryNum = u16EntryNum;

	return WV_SUCCESS;
}

void LUT_SortInputLUTEntry(INPUTLutEntry arstLUTEntry[MAX_INPUT_LUT_ENTRY_NUM], U16 u16EntryNum)
{
	TSP_SortInputLUTEntry(arstLUTEntry, u16EntryNum);
}

wvErrCode LUT_WriteInputLUTEntry(U8 u8LUTIndex, INPUTLutEntry arstLUTEntry[MAX_INPUT_LUT_ENTRY_NUM], U16 u16EntryNum)
{
	U32 i = 0;
	U32 u32LUTData = 0;
	
	for (i = 0; i < u16EntryNum; i++)
    {
        printf("[%4u],u16SrcStream[%3u],u16SrcPID[%u]\r\n",
            i, arstLUTEntry[i].u16SrcStream, arstLUTEntry[i].u16SrcPID);
    }

    for (i = 0; i < u16EntryNum; i++)
    {
        u32LUTData = (1 << 23)
                    | ((arstLUTEntry[i].u16SrcStream & 0x3FF) << 13)
                    | (arstLUTEntry[i].u16SrcPID & 0x1FFF);
        
        FPGA_REG_Write(LUT_ADDR, (u8LUTIndex << 12) | i);
        FPGA_REG_Write(LUT_DATA, u32LUTData);
        FPGA_REG_Write(LUT_WRITE_EN, 0);
        FPGA_REG_Write(LUT_WRITE_EN, 1);
        FPGA_REG_Write(LUT_WRITE_EN, 0);
    }

    for (i = u16EntryNum; i < MAX_INPUT_LUT_ENTRY_NUM; i++)
    {
        FPGA_REG_Write(LUT_ADDR, (u8LUTIndex << 12) | i);
        FPGA_REG_Write(LUT_DATA, 0);
        FPGA_REG_Write(LUT_WRITE_EN, 0);
        FPGA_REG_Write(LUT_WRITE_EN, 1);
        FPGA_REG_Write(LUT_WRITE_EN, 0);
    }

    FPGA_REG_Write(LUT_SWAP, 0);
    FPGA_REG_Write(LUT_SWAP, 1);
    FPGA_REG_Write(LUT_SWAP, 0);

	return WV_SUCCESS;
}


wvErrCode LUT_SetInputLUT(TSInfo *pstParamTS)
{
	printf("######################### LUT_SetInputLUT #####################\n");
    wvErrCode enErrCode = WV_SUCCESS;
    U16 i = 0;
    U8 u8LUTIndex = 0;
    U8 u8DestSlot = 0xFF;
    U32 u32LUTData = 0;
    U32 u32IPAddr = 0;
    U16 u16UDPPort = 0;
    U8 aru8MAC[6] = {0};

    INPUTLutEntry arstLUTEntry[MAX_INPUT_LUT_ENTRY_NUM];
    U16 u16EntryNum = 0;

    if (!pstParamTS)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]Input Error:pstParamTS[%p]\r\n",
            __FUNCTION__, __LINE__, pstParamTS);
        return WV_ERR_TSP_INPUT_PARAM;
    }

    for (u8LUTIndex = 0; u8LUTIndex < MAX_INPUT_LUT_NUM; u8LUTIndex++)
    {
        //u8DestSlot = s_aru8DestSlotRecord[u8LUTIndex];

		u8DestSlot = u8LUTIndex;
		//获取输出的IP 、端口、MAC
		
		u32IPAddr = 0xE30A145A + u8LUTIndex;
		u16UDPPort = 1234;
				
		aru8MAC[0] = 0x01;
		aru8MAC[1] = 0x00;
		aru8MAC[2] = 0x5e;
		aru8MAC[3] = 0x0a;
		aru8MAC[4] = 0x14;
		aru8MAC[5] = 0x5a + u8LUTIndex;

		//设置输出的IP 、端口、MAC
		enErrCode = LUT_SetInputLUTIPInfo(u8LUTIndex, u32IPAddr, u16UDPPort);
		if (WV_SUCCESS != enErrCode) 
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_SetInputLUTIPInfo Error:enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);

            return enErrCode;
        }

		//清除LUT
		LUT_ClearInputLUT(u8LUTIndex);

        u16EntryNum = 0;
        memset(arstLUTEntry, 0, sizeof(arstLUTEntry));
        //获取输入LUT Entry
        enErrCode = LUT_GetInputLUTEntry(u8DestSlot, pstParamTS, arstLUTEntry, &u16EntryNum);
        if (WV_SUCCESS != enErrCode) 
        {
            log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
                "[%s:%d]TSP_SetInputLUTIPInfo Error:enErrCode[%08X]\r\n",
                __FUNCTION__, __LINE__, enErrCode);

            return enErrCode;
        }

		//排序
        TSP_SortInputLUTEntry(arstLUTEntry, u16EntryNum);
		printf("[LUT_SetInputLUT] u16EntryNum = %u\n", u16EntryNum);

        for (i = 0; i < u16EntryNum; i++)
        {
            printf("[%4u],u16SrcStream[%3u],u16SrcPID[%u]\r\n",
                i, arstLUTEntry[i].u16SrcStream, arstLUTEntry[i].u16SrcPID);
        }

		//将Entry 写入LUT
		LUT_WriteInputLUTEntry(u8LUTIndex, arstLUTEntry, u16EntryNum);
    }

	printf("######################### LUT_SetInputLUT End #####################\n");

    return WV_SUCCESS;
}




