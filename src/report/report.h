/*
 * =====================================================================================
 *
 *       Filename:  report_board.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/01/17 14:17:15
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  linsheng.pan (), life_is_legend@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef _INCLUDE_REPORT_REPORT_H
#define _INCLUDE_REPORT_REPORT_H 

#include "appGlobal.h"

//主动上报信息
enum _BoardType
{
	BOARD_TYPE_RECV,
	BOARD_TYPE_TRANS,
	BOARD_TYPE_MONITOR,
};

typedef enum _BoardType BoardType;


struct _BoardInfo
{
	U8 u8Chassis;
	U8 u8Slot;
	U8 u8BoardType;
	U8 u8ChipTemp;
	U8 u8Status;
	U8 u8ErrorCode;
	U8 u8Reserved;
};

typedef struct _BoardInfo BoardInfo;
typedef struct _BoardInfo * BoardInfoPtr;

union ChannelType
{
	U8 u8ChannelType;
	U8 u8Reserved2;
};

struct _ChannelInfo
{
	U8 u8Chassis;
	U8 u8Slot;
	U8 u8Channel;
	U8 u8Reserved1;
	U8 aru8IPAddr[4];
	U16 u16Port;
	U8 u8BoardType;
	union ChannelType var;
};

typedef struct _ChannelInfo ChannelInfo;
typedef struct _ChannelInfo * ChannelInfoPtr;

union Descriptor
{
	BoardInfo stBoardInfo;
	ChannelInfo stChannelInfo;
};

struct _AnalyseData
{
	U8 u8DataType;
	U8 u8DataLength;
	U8 u8Reserved1;
	U8 u8Reserved2;
	union Descriptor uniDescriptor;
};

typedef struct _AnalyseData AnalyseData;
typedef struct _AnalyseData * AnalyseDataPtr;

typedef enum
{
	DATA_TYPE_BOARD_CHECK = 1,
	DATA_TYPE_CHANNEL_RESOURCE = 2,
	DATA_TYPE_CHANNEL_RF_RESULT = 3,
	DATA_TYPE_TR290_RESULT = 4,
	DATA_TYPE_AV_ERROR_STATUS = 5,
	DATA_TYPE_CORRECTING_TIME = 6,
}DATA_TYPE;

typedef enum
{
	BOARD_STATUS_NORMAL = 0, 
	BOARD_STATUS_ABNORMAL = 1,
}BOARD_STATUS;

struct _BoardCheckAnalysePacket
{
	U8 u8Sync;
	U8 u8Reserved1;
	U8 u8Reserved2;
	U8 u8Reserved3;
	U8 u8AnalyseDataNum;
	U8 u8AnalyseDataSize;
	AnalyseData stAnalyseData;
	U32 u32CRC;
};

typedef struct _BoardCheckAnalysePacket BoardCheckAnalysePacket;
typedef struct _BoardCheckAnalysePacket * BoardCheckAnalysePacketPtr;

struct _ChannelResourceAnalysePacket
{
	U8 u8Sync;
	U8 u8Reserved1;
	U8 u8Reserved2;
	U8 u8Reserved3;
	U8 u8AnalyseDataNum;
	U8 u8AnalyseDataSize;
	AnalyseData stAnalyseData[4];
	U32 u32CRC;
};

typedef struct _ChannelResourceAnalysePacket ChannelResourceAnalysePacket;
typedef struct _ChannelResourceAnalysePacket * ChannelResourceAnalysePacketPtr;

struct _IPInfo
{
	U32 u32IP;
	U32 u32Crc;
};

typedef struct _IPInfo IPInfo;
typedef struct _IPInfo * IPInfoPtr;

void Report_ConstructBoardInfo(void);
void Report_ConstructChannelInfo(void);

void * Report_BoardInfo(void *arg);
void * Report_ChannelInfo(void *arg);



#endif /* ifndef _INCLUDE_REPORT_REPORT_H */




