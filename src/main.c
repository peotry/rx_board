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

#include "upgrade.h"

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
    /*????????*/
    log_Init((U8 *)LOG_SAVE_FILE, LOG_SAVE_SIZE);

    /*???????????????д???????phyо?*/
    (void)i2c_OpenBus(I2C_0);
    (void)i2c_OpenBus(I2C_1);
    FPGA_Init();
    UartComm_init();
    PHY_Init();

    //软件 加载 XC6SLX4芯片，将逻辑提供的bit文件转换为16进制的数组，加载到芯片上
    PSConf_FWConfigGeneralFPGA32Bit(data_array, sizeof(data_array) -1, XILINX_PLATFORM);


    /*???????????λ???? */
    Status_Init();

    /*???licens????????mac,????????*/
    //BMN_GetBoardLicense();
    //BMN_ParseBoardLicense();

    //TODO
    //IP_setMac();

    /*???????????*/
    Resource_Init(Status_GetSlotID());

	//探测ADT7410是否存在
	#if 0
	ADT7410_Detect();
	U32 temp = 0;

	while(1)
	{
		if(WV_TEMP_NEGATIVE == ADT7410_GetTemperature(&temp))
		{
			printf("Temp: - %u\n", temp);
		}
		else
		{
			printf("Temp: %u\n", temp);
		}
			
		sleep(1);
	}
	#endif

    /*UART ??????????????ip???????????????ip*/
    //UartComm_GetTime();
    //UartComm_GetIpAddr(Status_GetSlotID());

    /*??Χо?????,TUNER??FPGA??CI*/
    WVCI_Setup();

    usleep(4000*1000);

    Tuner_init();

    SIP_Init();

    //????????
    TSP_Init();

    /*???? web ???*/
    //web_Start();

    //??????
    //NMGServer_Start();

    /*????ip??????*/
    //IPServer_Start();

    /*????????????????*/
    //IPComm_SendData2Slot(SLOT_BB, IPCOMM_SEND_RESOUR, (U8*)Resource_getSelfHandle(), sizeof(TResource_TS));

    pthread_t stCIMonitorThread;
    (void)OS_ThreadCreate(&stCIMonitorThread, (void *)WVCI_TaskMonitoring);

    pthread_t stCITaskThread;
    (void)OS_ThreadCreate(&stCITaskThread, (void *)WVCI_TaskProcess);

    pthread_t stTunerThread;
    (void)OS_ThreadCreate(&stTunerThread, (void *)Tuner_process);

    TSP_RestoreTSMUXInfo();

    //???????????SI???????
    SIP_SendSIServer();
    
    TSP_ScanTSStart();

    while(1)
    {
        usleep(30000*1000);
		WVCI_TEST();
		//WVCI_TEST(0);
		//WVCI_TEST(1);
		//WVCI_TEST(2);
		//WVCI_TEST(3);
    }

    return 0;
}


