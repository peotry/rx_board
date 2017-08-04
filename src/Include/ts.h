/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName ：ts.h
* Description ：macro define about ts standard.
* Author    : wei.li
* Modified  :
* Reviewer  :
* Date      : 2017-02-21
* Record    :
*
**********************************************************************/
#ifndef TS_H_
#define TS_H_

enum
{
    SI_MODE_UNKNOWN,
    SI_MODE_DVB,
    SI_MODE_ATSC,
    SI_MODE_SCTE,
};

/*******************************************************************
*PID Constant
*******************************************************************/
#define PAT_PID                 0x0000      //节目关联表
#define CAT_PID                 0x0001      //条件接收表
#define NIT_PID                 0x0010      //网络信息表
#define SDT_PID                 0x0011      //节目业务描述表
#define BAT_PID                 0x0011      //节目业务群关联表
#define ST_PID                  0x0010      //填充表
#define ST_PID_2                0x0011      //填充表
#define NULL_PID                0x1FFF      //空表

#define TDT_PID                 0x0014      //时间日期表
#define TOT_PID                 0x0014
#define EIT_PID                 0x0012      //节目事件表
#define DIT_PID                 0x001E
#define SIT_PID                 0x001F

#define MAX_RESERVE_PID         0x0020

#define BASE_PID                0x1FFB

#define MAX_PID_NUMBER          0x1FFF

/*******************************************************************
*TABLEID Constant
*******************************************************************/
//DVB 
#define PAT_TID                 0x00        //节目关联表
#define CAT_TID                 0x01        //条件接收表
#define PMT_TID                 0x02        //节目映射表
#define NIT_TID_ACTUAL          0x40        //网络信息表.实际
#define NIT_TID_OTHER           0x41        //网络信息表.其它
#define SDT_TID_ACTUAL          0x42        //节目业务描述表.实际
#define SDT_TID_OTHER           0x46        //节目业务描述表.其它
#define BAT_TID                 0x4A

#define TDT_TID                 0x70
#define TOT_TID                 0x73
#define EIT_TID_ACTUAL          0x4E
#define EIT_TID_OTHER           0x4F
#define EIT_TID_ACTUAL_SCHEDULE 0x50        //from 0x50 to 0x5f
#define EIT_TID_OTHER_SCHEDULE  0x60        //from 0x60 to 0x6f

//ATSC standard
#define MGT_TID                 0xC7
#define RRT_TID                 0xCA
#define EIT_TID_ATSC            0xCB
#define ETT_TID                 0xCC
#define STT_TID                 0xCD
#define TVCT_TID                0xC8
#define CVCT_TID                0xC9

//ATSC MGT TABLE TYPE
#define TVCT_TYPE               0x0000      // for MGT type
#define CVCT_TYPE               0x0002      // for MGT type
#define EIT_TYPE                0x0100      // for MGT type
#define EEIT_TYPE               0x0200      // for MGT type
#define RRT_TYPE                0x0300      // for MGT type

//SCTE
#define PMT_PROGRAM_INFO        0xC0
#define PMT_PROGRAM_NAME        0xC1
#define NIT_NETWORK_INFO        0xC2
#define NIT_NETWORK_TEXT        0xC3
#define NIT_VIRTUAL_CHANNEL     0xC4
#define NIT_SYSTEM_TIME         0xC5


/*******************************************************************
*TAG Constant
*******************************************************************/
#define VIDEO_STREAM_DESCRIPTOR_TAG                 0x02
#define AUDIO_STREAM_DESCRIPTOR_TAG                 0x03
#define HIERARCHY_DESCRIPTOR_TAG                    0x04
#define REGISTRATION_DESCRIPTOR_TAG                 0x05
#define DATA_STREAM_ALIGNMENT_DESCRIPTOR_TAG        0x06
#define TARGET_BACKGROUND_GRID_DESCRIPTOR_TAG       0x07
#define VIDEO_WINDOW_DESCRIPTOR_TAG                 0x08
#define CA_DESCRIPTOR_TAG                           0x09
#define ISO_639_LANGUAGE_DESCRIPTOR_TAG             0x0A
#define SYSTEM_CLOCK_DESCRIPTOR_TAG                 0x0B
#define MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR_TAG 0x0C
#define COPYRIGHT_DESCRIPTOR_TAG                    0x0D
#define MAXIMUM_BITRATE_DESCRIPTOR_TAG              0x0E
#define PRIVATE_DATA_INDICATOR_DESCRIPTOR_TAG       0x0F
#define SMOOTHING_BUFFER_DESCRIPTOR_TAG             0x10
#define STD_DESCRIPTOR_TAG                          0x11
#define IBP_DESCRIPTOR_TAG                          0x12

#define NETWORK_NAME_DESCRIPTOR_TAG                 0x40
#define SERVICE_LIST_DESCRIPTOR_TAG                 0x41
#define STUFFING_DESCRIPTOR_TAG                     0x42
#define SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR_TAG    0x43
#define CABLE_DELIVERY_SYSTEM_DESCRIPTOR_TAG        0x44
#define VBI_DATA_DESCRIPTOR_TAG                     0x45
#define VBI_TELETEXT_DESCRIPTOR_TAG                 0x46
#define BOUQUET_NAME_DESCRIPTOR_TAG                 0x47
#define SERVICE_DESCRIPTOR_TAG                      0x48
#define LINKAGE_DESCRIPTOR_TAG                      0x4A
#define STREAM_IDENTIFIER_DESCRIPTOR_TAG            0x52
#define TELETEXT_DESCRIPTOR_TAG                     0x56
#define SUBTITLE_DESCRIPTOR_TAG                     0x59
#define TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR_TAG  0x5A
#define PRIVATE_DATA_SPECIFIER_DESCRIPTOR_TAG       0x5F
#define AC3_DESCRIPTOR_TAG                          0x6A
#define ENHANCED_AC3_DESCRIPTOR_TAG                 0x7A
#define AAC_DESCRIPTOR_TAG                          0x7C
#define LCN_DESCRIPTOR_TAG                          0x83
#define DRA_DESCRIPTOR_TAG                          0xA0
#define AVS_VIDEO_DESCRIPTOR_TAG                    0x3F

/*******************************************************************
*Stream type Constant
*******************************************************************/
#define STREAM_TYPE_VIDEO1          0x01 //ISO/IEC 11172     视频
#define STREAM_TYPE_VIDEO           0x02 //ISO/IEC 13818-2   视频
#define STREAM_TYPE_AUDIO1          0x03 //ISO/IEC 11172     音频
#define STREAM_TYPE_AUDIO           0x04 //ISO/IEC 13818-3   音频
#define STREAM_TYPE_PRIVATE1        0x05 //私有分段
#define STREAM_TYPE_PRIVATE         0x06 //私有数据
#define STREAM_TYPE_AUDIO_DRA       0x06 //DRA音频
#define STREAM_TYPE_TELETEXT        0x56 //teletext
#define STREAM_TYPE_VIDEO_H264      0x1B //H264 视频
#define STREAM_TYPE_VIDEO_H265      0x24 //H265 视频
#define STREAM_TYPE_VIDEO_MPEG4     0x10 //MPEG4 视频
#define STREAM_TYPE_AUDIO_AC3       0x81 //AC3 音频
#define STREAM_TYPE_VIDEO_ATSC      0x80 //ATSC 视频
#define STREAM_TYPE_MPEG2_AAC       0x0F //Mpeg-2 AAC：stream_type为0x0f
#define STREAM_TYPE_MPEG4_AAC       0x11 //Mpeg-4 AAC：stream_type为0x11
#define STREAM_TYPE_AC3_PLAS        0x84 //AC3-Plus：stream_type为0x84
#define STREAM_TYPE_PRIVATE2        0x05 //PRIVATE type
#define STREAM_TYPE_VIDEO_AVS       0x42 //AVS 视频
#define STREAM_TYPE_DVB_AUDIO_AC3   0x06 //PRIVATE type
#define STREAM_TYPE_USER_PRIVATE    0x80 //User Private
#define STREAM_TYPE_DTS             0x82 //DTS：stream_type为0x82
#define STREAM_TYPE_DOLBY_TRUEHD    0x83 //Dolby TrueHD：stream_type为0x83
#define STREAM_TYPE_DTS_HD          0x85 //DTS_HD：stream_type为0x85
#define STREAM_TYPE_DTS_MA          0x86 //DTS-MA：stream_type为0x86
#define STREAM_TYPE_AC3_PLUS_SEC    0xA1 //AC3-Plus_SEC：steam_type为0xa1
#define STREAM_TYPE_DTS_HD_SEC      0xA2 //DTS_HD_SEC：stream_type为0xa2

#define STREAM_TYPE_ECM             0x09 //ECM type
#define STREAM_TYPE_EMM             0x09 //EMM type


#endif/*TS_H_*/

