/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName ??main.c
* Description ??the entry of app.
* Author    : wei.li
* Modified  :
* Reviewer  :
* Date      : 2017-02-21
* Record    :
*
**********************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <fcntl.h>

#include "appGlobal.h"
#include "web.h"
#include "WV_log.h"
#include "FPGA.h"
#include "REG.h"

#include "status.h"
#include "resource.h"
#include "Tuner.h"
#include "WV_CI.h"
#include "UartComm.h"
#include "WV_log.h"
#include "phy.h"
#include "ADT7410.h"
#include "IPComm.h"
#include "uart.h"
#include "PIN.h"
#include "Xilinx_i2c.h"
#include "phy.h"
#include "TSConstruct.h"
#include "parameters.h"
#include "BoardMngr.h"
#include "NetManager.h"
#include "GPIO.h"
#include "psconf/data_hex_array.h"
#include "psconf/psconf.h"
#include "web_xml.h"

#include "upgrade.h"
#include "lut.h"

extern void UartComm_RequestIP(U8 u8SlotID);
extern void UartComm_RecvData(U8 u8slot, ST_WV_UARTCOMM  * pstUartComm);

U8 u8data[2800] = {0};

extern BOOL g_isUpgrading;


U32 OS_ThreadCreate(pthread_t *thread,void *(* thread_function)(void *))
{
    S32 state;
    state = pthread_create(thread,NULL,thread_function,NULL);
    if(state != 0)
    {
        return WV_ERR_FAILURE;
    }

    state = pthread_detach(*thread);
    if(state != 0)
    {
         return WV_ERR_FAILURE;
    }

    return WV_SUCCESS;
}


int main()
{
	printf("main func...\n");
    //初始化日志
    log_Init((U8 *)LOG_SAVE_FILE, LOG_SAVE_SIZE);

    //初始化
    (void)i2c_OpenBus(I2C_0);
    (void)i2c_OpenBus(I2C_1);
    FPGA_Init();
    UartComm_init();

	printf("PHY_Init func...\n");
    PHY_Init();

    //软件 加载 XC6SLX4芯片，将逻辑提供的bit文件转换为16进制的数组，加载到芯片上
    PSConf_FWConfigGeneralFPGA32Bit(psconf_data_array, sizeof(psconf_data_array) -1, XILINX_PLATFORM);

    Status_Init();

	//初始化xml信息
	WebXml_InitDeviceInfo();
	WebXml_InitParamsInfo();
	WebXml_InitProgramDecrypt();
   
    Resource_Init(Status_GetSlotID());

	//网络通信
	web_Start();

	//定时检测温度，定时上报
	//定时发表
	SIP_Init();
    
    TSP_Init();
	  
    WVCI_Setup();

    usleep(4000*1000);

    Tuner_init();

    pthread_t stCIMonitorThread;
    (void)OS_ThreadCreate(&stCIMonitorThread, (void *)WVCI_TaskMonitoring);

    pthread_t stCITaskThread;
    (void)OS_ThreadCreate(&stCITaskThread, (void *)WVCI_TaskProcess);

    pthread_t stTunerThread;
    (void)OS_ThreadCreate(&stTunerThread, (void *)Tuner_process);

	//复用输出
    TSP_RestoreTSMUXInfo();
	
    SIP_SendSIServer();
    
    TSP_ScanTSStart();

	DescrambleProgInfo arstDescrambleProgInfo[100];
	arstDescrambleProgInfo[0].u16Channel = 0;
	arstDescrambleProgInfo[0].u16ServiceID = 302;
	arstDescrambleProgInfo[0].u8CAMIndex = INVALID_CAM_INDEX;

	///TSInfo * pstTS = TSP_GetTSParamHandle();
	
	//usleep(20000*1000);
	//int index = 0;

	//usleep(20000*1000);
	
	//web_ProcessDescramble(arstDescrambleProgInfo , 1, pstTS);
	//TSInfo * pstTS = TSP_GetTSParamHandle();
	//LUT_SetInputLUT(pstTS);
    while(1)
    {
		WVCI_TEST();
        usleep(60000*1000);
		//TSInfo * pstTS = TSP_GetTSParamHandle();
		//WelXml_GetProgramInfoFromTSInfo(pstTS);

		//web_ProcessDescramble(arstDescrambleProgInfo , 1, pstTS);
		//WVCI_TEST();

    }
    return 0;
	
}


