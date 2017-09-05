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

#include "net/net.h"
#include "report/report.h"
#include "thread/thread.h"
#include "timer/timer.h"

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

void test(void)
{
	char ip_str[20] = {0};
	char mac_addr[40] = {0};
	char net_mask[16] = {0};
	char gateway[16] = {0};
	U32 u32SlotId = 0;
	U32 u32IPAddr = 0;
	char ip_str_base[16] = "192.168.1.100";
	char ip_str_new[16] = {0};
	
	//Timer_MonitorServer();
	//sleep(2);
	//Net_GetIpAddrStr("eth0", ip_str, sizeof(ip_str));
	//Net_GetIpAddrStr("eth1", ip_str, sizeof(ip_str));

	U32 u32CRC = Common_CRCCalculate("hello-world", 11, 0);
	printf("%d\n", u32CRC);

	//获取slot id
   	u32SlotId = PIN_GetSlotID();

	printf("u32SlotId = %u\n", u32SlotId);

	int i = 0;
	Net_GetLocalMac("eth1", mac_addr, sizeof(mac_addr));
	for(i = 0; i < 6; ++i)
	{
		printf("mac[%d] = 0x%x\n", i, mac_addr[i]);
	}

	Net_GetGateWay(gateway, sizeof(gateway));
	printf("gateway: %s\n", gateway);
	
	//Net_GetLocalNetMask("eth1",net_mask, sizeof(net_mask));

	//Net_SetIpAddr2FPGA("eth0");
	//Net_SetMacAddr2FPGA("eth1");

	THREAD_NEW_DETACH(Upgrade_Server, NULL, "Upgrade_Server");

	Net_SetIpAddr("eth0", ip_str_base, sizeof(ip_str_base));
	Net_GetIpAddr("eth0", &u32IPAddr);
	u32IPAddr += PIN_GetSlotID();
	
	u32IPAddr = htonl(u32IPAddr);
	inet_ntop(AF_INET, &u32IPAddr, ip_str_new, sizeof(ip_str_new));
	printf("eth0: %s\n", ip_str_new);
	Net_SetIpAddr("eth0", ip_str_new, sizeof(ip_str_new));

	//Net_GetIpAddrWithDHCP("eth0");

	//IPComm_Server_Service();

	//THREAD_NEW_DETACH(Report_BoardInfo, NULL, "Report_BoardInfo");
	//THREAD_NEW_DETACH(Report_ChannelInfo, NULL, "Report_ChannelInfo");
	
	while(1)
	{
		sleep(100);
	}
}


int main()
{
	printf("main func...\n");

	U32 u32SlotId = 0;
	U32 u32IPAddr = 0;
	char ip_str_base[16] = "192.168.1.100";
	char ip_str_new[16] = {0};
	
    //初始化日志
    log_Init((U8 *)LOG_SAVE_FILE, LOG_SAVE_SIZE);

    //初始化
    (void)i2c_OpenBus(I2C_0);
    (void)i2c_OpenBus(I2C_1);
    FPGA_Init();
    UartComm_init();

    PHY_Init();

    //软件 加载 XC6SLX4芯片，将逻辑提供的bit文件转换为16进制的数组，加载到芯片上
    PSConf_FWConfigGeneralFPGA32Bit(psconf_data_array, sizeof(psconf_data_array) -1, XILINX_PLATFORM);

	//获取slot id
   	PIN_GetSlotID();

	//自动获取ip
	//Net_GetIpAddrWithDHCP("eth0");
	Net_GetIpAddrWithDHCP("eth1");

	//将网口0的ip和mac告诉FPGA
	Net_SetIpAddr("eth0", ip_str_base, sizeof(ip_str_base));
	Net_GetIpAddr("eth0", &u32IPAddr);
	u32IPAddr += PIN_GetSlotID();
	u32IPAddr = htonl(u32IPAddr);
	inet_ntop(AF_INET, &u32IPAddr, ip_str_new, sizeof(ip_str_new));
	printf("eth0: %s\n", ip_str_new);
	Net_SetIpAddr("eth0", ip_str_new, sizeof(ip_str_new));
	
	Net_SetIpAddr2FPGA("eth0");
	Net_SetMacAddr2FPGA("eth0");

	//初始化xml信息
	WebXml_InitDeviceInfo();
	WebXml_InitParamsInfo();
	WebXml_InitProgramDecrypt();
   
    //Resource_Init(Status_GetSlotID());

	//组播通道信息
	THREAD_NEW_DETACH(Report_ChannelInfo, NULL, "Report_ChannelInfo");

	//网络通信
	web_Start();

	//开启升级线程服务
	THREAD_NEW_DETACH(Upgrade_Server, NULL, "Upgrade_Server");
    
    TSP_Init();
	  
    WVCI_Setup();

    usleep(4000*1000);

    Tuner_init();

	THREAD_NEW_DETACH((void *)WVCI_TaskMonitoring, NULL, "WVCI_TaskMonitoring");
	THREAD_NEW_DETACH((void *)WVCI_TaskProcess, NULL, "WVCI_TaskProcess");
	THREAD_NEW_DETACH((void *)Tuner_process, NULL, "Tuner_process");

	#if 0
    pthread_t stCIMonitorThread;
    (void)OS_ThreadCreate(&stCIMonitorThread, (void *)WVCI_TaskMonitoring);

    pthread_t stCITaskThread;
    (void)OS_ThreadCreate(&stCITaskThread, (void *)WVCI_TaskProcess);

    pthread_t stTunerThread;
    (void)OS_ThreadCreate(&stTunerThread, (void *)Tuner_process);
	#endif

	//恢复TS信息
    TSP_RestoreTSMUXInfo();
    
    TSP_ScanTSStart();
	
    while(1)
    {
		usleep(3000*1000);
    }
	
    return 0;
	
}


