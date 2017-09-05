/*
 * =====================================================================================
 *
 *       Filename:  report_board.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/01/17 14:16:52
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  linsheng.pan (), life_is_legend@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "report/report.h"

#include "ADT7410/ADT7410.h"

#include "event/event.h"
#include "timer/timer.h"
#include "net/udp.h"
#include "net/net.h"

#define REPORT_BOARD_CHECK_IP "224.1.1.1"
#define REPORT_BOARD_CHECK_PORT 5001

#define REPORT_CHANNEL_INFO_IP "224.1.1.5"
#define REPORT_CHANNEL_INFO_PORT 5001

static BoardCheckAnalysePacket s_stBoardCheckAnalysePacket;
static ChannelResourceAnalysePacket s_stChannelResourceAnalysePacket;

static int s_BoardInfoFd = -1;
static int s_ChannelInfoFd = -1;

extern int g_SlotID;

void Report_ConstructBoardInfo(void)
{
	U32 u32Temp = 0;
	ADT7410_GetTemperature(&u32Temp);
	
	s_stBoardCheckAnalysePacket.u8Sync = 0x5A;
	s_stBoardCheckAnalysePacket.u8Reserved1 = 0;
	s_stBoardCheckAnalysePacket.u8Reserved2 = 0;
	s_stBoardCheckAnalysePacket.u8Reserved3 = 0;
	s_stBoardCheckAnalysePacket.u8AnalyseDataNum = 1;
	s_stBoardCheckAnalysePacket.u8AnalyseDataSize = sizeof(AnalyseData);

	s_stBoardCheckAnalysePacket.stAnalyseData.u8DataType = DATA_TYPE_BOARD_CHECK;
	s_stBoardCheckAnalysePacket.stAnalyseData.u8DataLength = sizeof(BoardInfo);
	s_stBoardCheckAnalysePacket.stAnalyseData.u8Reserved1 = 0;
	s_stBoardCheckAnalysePacket.stAnalyseData.u8Reserved2 = 0;

	s_stBoardCheckAnalysePacket.stAnalyseData.uniDescriptor.stBoardInfo.u8Chassis = 1;
	s_stBoardCheckAnalysePacket.stAnalyseData.uniDescriptor.stBoardInfo.u8Slot = g_SlotID;
	s_stBoardCheckAnalysePacket.stAnalyseData.uniDescriptor.stBoardInfo.u8Reserved = 0xFF;
	s_stBoardCheckAnalysePacket.stAnalyseData.uniDescriptor.stBoardInfo.u8BoardType = BOARD_TYPE_RECV;
	s_stBoardCheckAnalysePacket.stAnalyseData.uniDescriptor.stBoardInfo.u8ChipTemp = (U8 )u32Temp;
	s_stBoardCheckAnalysePacket.stAnalyseData.uniDescriptor.stBoardInfo.u8Status = BOARD_STATUS_NORMAL;
	s_stBoardCheckAnalysePacket.stAnalyseData.uniDescriptor.stBoardInfo.u8ErrorCode = 0;
	s_stBoardCheckAnalysePacket.stAnalyseData.uniDescriptor.stBoardInfo.u8Reserved = 0;
	
	s_stBoardCheckAnalysePacket.u32CRC = generate_crc32c(&s_stBoardCheckAnalysePacket, sizeof(s_stBoardCheckAnalysePacket) - sizeof(U32));

}


void Report_ConstructChannelInfo(void)
{
	U32 u32Temp = 0;
	U32 i = 0;
	U32 u32IPAddr = 0;

	#if 0
	ADT7410_GetTemperature(&u32Temp);
	Net_GetIpAddr("eth1", &u32IPAddr);
	
	s_stChannelResourceAnalysePacket.u8Sync = 0x5A;
	s_stChannelResourceAnalysePacket.u8Reserved1 = 0;
	s_stChannelResourceAnalysePacket.u8Reserved2 = 0;
	s_stChannelResourceAnalysePacket.u8Reserved3 = 0; 
	s_stChannelResourceAnalysePacket.u8AnalyseDataNum = 4;
	s_stChannelResourceAnalysePacket.u8AnalyseDataSize = sizeof(AnalyseData);

	for(i = 0; i < 4; ++i)
	{
		s_stChannelResourceAnalysePacket.stAnalyseData[i].u8DataType = DATA_TYPE_CHANNEL_RESOURCE;
		s_stChannelResourceAnalysePacket.stAnalyseData[i].u8DataLength = sizeof(ChannelInfo);
		s_stChannelResourceAnalysePacket.stAnalyseData[i].u8Reserved1 = 0;
		s_stChannelResourceAnalysePacket.stAnalyseData[i].u8Reserved2 = 0;
		
		s_stChannelResourceAnalysePacket.stAnalyseData[i].uniDescriptor.stChannelInfo.u8Chassis = 1;
		s_stChannelResourceAnalysePacket.stAnalyseData[i].uniDescriptor.stChannelInfo.u8Slot = g_SlotID;
		s_stChannelResourceAnalysePacket.stAnalyseData[i].uniDescriptor.stChannelInfo.u8Channel = i;
		s_stChannelResourceAnalysePacket.stAnalyseData[i].uniDescriptor.stChannelInfo.u8Reserved1 = 0;
		//TODO
		s_stChannelResourceAnalysePacket.stAnalyseData[i].uniDescriptor.stChannelInfo.aru8IPAddr[0] = u32IPAddr & 0xff;
		s_stChannelResourceAnalysePacket.stAnalyseData[i].uniDescriptor.stChannelInfo.aru8IPAddr[1] = (u32IPAddr >> 8) & 0xff;
		s_stChannelResourceAnalysePacket.stAnalyseData[i].uniDescriptor.stChannelInfo.aru8IPAddr[2] = (u32IPAddr >> 16) & 0xff;
		s_stChannelResourceAnalysePacket.stAnalyseData[i].uniDescriptor.stChannelInfo.aru8IPAddr[3] = (u32IPAddr >> 24) & 0xff;
		s_stChannelResourceAnalysePacket.stAnalyseData[i].uniDescriptor.stChannelInfo.u16Port = 80;
		s_stChannelResourceAnalysePacket.stAnalyseData[i].uniDescriptor.stChannelInfo.u8BoardType = BOARD_TYPE_RECV;
		s_stChannelResourceAnalysePacket.stAnalyseData[i].uniDescriptor.stChannelInfo.var.u8Reserved2 = 0;
	}
	
	s_stChannelResourceAnalysePacket.u32CRC = generate_crc32c(&s_stChannelResourceAnalysePacket, sizeof(s_stBoardCheckAnalysePacket) - sizeof(U32));
	#endif

	Net_GetIpAddr("eth1", &u32IPAddr);
}


void Report_WaitBoardInfoEvent(void)
{
	Event_Read(s_BoardInfoFd);
}

void * Report_NotifyBoardInfoEvent(void *arg)
{
	Event_Write(s_BoardInfoFd);

	return NULL;
}

void Report_WaitChannelInfoEvent(void)
{
	Event_Read(s_ChannelInfoFd);
}

void * Report_NotifyChannelInfoEvent(void * arg)
{
	Event_Write(s_ChannelInfoFd);

	return NULL;
}


void Report_RegisterBoardInfoTimer(void)
{
	struct itimerspec itimer;
    struct TimerTask stTimerTask;

	itimer.it_value.tv_sec = 2;
    itimer.it_value.tv_nsec = 0;
    itimer.it_interval.tv_sec = 2;
    itimer.it_interval.tv_nsec = 0;

	stTimerTask.func = Report_NotifyBoardInfoEvent;
    stTimerTask.arg = NULL;

	Timer_RegisterTask(itimer, &stTimerTask);
}

void Report_RegisterChannelInfoTimer(void)
{
	struct itimerspec itimer;
    struct TimerTask stTimerTask;

	itimer.it_value.tv_sec = 2;
    itimer.it_value.tv_nsec = 0;
    itimer.it_interval.tv_sec = 2;
    itimer.it_interval.tv_nsec = 0;

	stTimerTask.func = Report_NotifyChannelInfoEvent;
    stTimerTask.arg = NULL;

	Timer_RegisterTask(itimer, &stTimerTask);
}



void * Report_BoardInfo(void *arg)
{
	//创建事件通知句柄
	s_BoardInfoFd = Event_CreateFd();
	
	//创建套接字
	int sockfd = -1;
	char group_ip[16] = REPORT_BOARD_CHECK_IP;
    short port = REPORT_BOARD_CHECK_PORT;
    struct sockaddr_in addr;

	bzero(&addr, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, group_ip, &addr.sin_addr.s_addr);
    addr.sin_port = htons(port);

	UDP_CreateSendSockfd(&sockfd);

	//注册定时触发组播的函数
	//Report_RegisterChannelInfoTimer();

	while(1)
	{
		//等待事件通知
		//Report_WaitChannelInfoEvent();
		usleep(500000);
		//构建自检信息
		Report_ConstructBoardInfo();
		//发送组播
		UDP_Sendto(sockfd, (struct sockaddr_in *)(&addr), &s_stBoardCheckAnalysePacket, sizeof(s_stBoardCheckAnalysePacket));
		printf("Send Msg to %s:%u\n", REPORT_BOARD_CHECK_IP, REPORT_BOARD_CHECK_PORT);
	}

	return NULL;
}

void * Report_ChannelInfo(void *arg)
{
	//创建事件通知句柄
	s_ChannelInfoFd = Event_CreateFd();
	
	//创建套接字
	IPInfo stIPInfo;
	int sockfd = -1;
	char group_ip[16] = REPORT_CHANNEL_INFO_IP;
    short port = REPORT_CHANNEL_INFO_PORT;
    struct sockaddr_in addr;

	bzero(&addr, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, group_ip, &addr.sin_addr.s_addr);
    addr.sin_port = htons(port);

	UDP_CreateSendSockfd(&sockfd);

	//注册定时触发组播的函数
	//Report_RegisterChannelInfoTimer();

	while(1)
	{
		//等待事件通知
		//Report_WaitChannelInfoEvent();
		usleep(500000);
		//构建自检信息
		//Report_ConstructChannelInfo();
		Net_GetIpAddr("eth1", &stIPInfo.u32IP);
		stIPInfo.u32Crc = Common_CRCCalculate(&stIPInfo.u32IP, sizeof(U32), 0);
	
		//发送组播
		UDP_Sendto(sockfd, (struct sockaddr_in *)(&addr), &stIPInfo, sizeof(stIPInfo));
		//UDP_Sendto(sockfd, (struct sockaddr_in *)(&addr), &s_stChannelResourceAnalysePacket, sizeof(s_stChannelResourceAnalysePacket));
		//printf("Send Msg to %s:%u\n", REPORT_CHANNEL_INFO_IP, REPORT_CHANNEL_INFO_PORT);
	}

	return NULL;
}



