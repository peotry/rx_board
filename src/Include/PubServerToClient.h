/**********************************************************************
* Copyright(c), HuiZhou WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName ：PubServerToClient.h
* Description ：common macro define and wellav information of version
*
* Version ：1.0.0
* Author ：tangzhimin   Reviewer :
* Date   ：2010-12-27
* Record : Create
*
**********************************************************************/
#ifndef PUB_SERVER_TO_CLIENT_H_
#define PUB_SERVER_TO_CLIENT_H_

#include "appGlobal.h"

#define   MAX_PARAMETER_PER_PACKET_BUF_LEN          1022

//#define   DEVICE_TYPE_SMP260                          0x00000008
#define   CMD_NET_BROADCAST_REQUEST_RET             0x22222222

#define   CMD_DMP_STATUAS                           0x963800FD
#define   NMS_DMP_QUIT                              0xa1500002 //Client请求退出
#define   NMS_DMP_COMMON_RSP                        0x9638FFFE //公共应答信息

#define   NMS_DMP_GET_MAINBOARD_INFO                0x96380001 //获取主板信息
#define   NMS_DMP_GET_MAINBOARD_INFO_RSP            0x96380002 //获取主板信息的应答
#define   NMS_DMP_GET_SYSTEM_INFO                   0x96380003 //获取主板系统信息
#define   NMS_DMP_GET_SYSTEM_INFO_RSP               0x96380004 //获取主板系统信息信息的应答
#define   NMS_DMP_SET_SYSTEM_INFO                   0x96380005 //设置主板系统信息信息
#define   NMS_DMP_MAINBOARD_REBOOT                  0x96380006 //主板请求重启
#define   NMS_DMP_MAINBOARD_FACTORY_SET             0x96380007 //主板请求恢复出厂设置
#define   NMS_DMP_GET_TS_INFO                       0x96380008 //获取TS信息
#define   NMS_DMP_GET_TS_INFO_RSP                   0x96380009 //获取TS信息的应答
#define   NMS_DMP_SET_TS_INFO                       0x9638000A //设置TS信息
#define   NMS_DMP_UPGRADE_REQUEST                   0x9638000B //升级请求
#define   NMS_DMP_ERASE_FLASH_REQUEST               0x9638000C //擦除FLASH请求
#define   NMS_DMP_REFLESH_FLASH_REQUEST             0x9638000D //刷新FLASH请求
#define   NMS_DMP_SEND_FILE_REQUEST                 0x9638000E //升级请求
#define   NMS_DMP_WRITE_FLASH_RET                   0x9638000F //写FLASH进度
#define   NMS_DMP_GET_SLOT_INFO                     0x96380010 //获取SLOT信息
#define   NMS_DMP_GET_SLOT_INFO_RSP                 0x96380011 //获取SLOT信息的应答
#define   NMS_DMP_GET_SINGLE_TS_INFO                0x96380012 //获取某个TS信息
#define   NMS_DMP_SET_MAINBOARD_INFO                0x96380013 //设置TS信息
#define   NMS_DMP_CLEAR_SINGLE_TS_INFO              0x96380014 //清空某个TS信息

#define   NMS_DMP_SUBBOARD_POWEROFF                 0x96380015 //子板断电
#define   NMS_DMP_SUBBOARD_REBOOT                   0x96380016 //子板重启
#define   NMS_DMP_SUBBOARD_FACTORYSET               0x96380017 //子板恢复设置
#define   NMS_DMP_GET_TSOVERIP_PARAM                0x96380018 //获取TSOVERIP参数
#define   NMS_DMP_GET_TSOVERIP_PARAM_RSP            0x96380019 //获取TSOVERIP参数的应答
#define   NMS_DMP_SET_TSOVERIP_PARAM                0x9638001A //设置TSOVERIP参数
#define   NMS_DMP_GET_ASI_PARAM                     0x9638001B //获取ASI参数
#define   NMS_DMP_GET_ASI_PARAM_RSP                 0x9638001C //获取ASI参数的应答
#define   NMS_DMP_SET_ASI_PARAM                     0x9638001D //设置ASI参数
#define   NMS_DMP_GET_AUTHORIZATION                 0x9638001E //获取授权信息
#define   NMS_DMP_GET_AUTHORIZATION_RSP             0x9638001F //获取授权信息的应答
#define   NMS_DMP_SET_AUTHORIZATION                 0x96380020 //设置授权信息
#define   NMS_DMP_GET_DVBS2_PARAM                   0x96380021 //获取DVBS2参数
#define   NMS_DMP_GET_DVBS2_PARAM_RSP               0x96380022 //获取DVBS2参数的应答
#define   NMS_DMP_SET_DVBS2_PARAM                   0x96380023 //设置DVBS2参数
#define   NMS_DMP_GET_DVBC_PARAM                    0x96380024 //获取DVBC参数
#define   NMS_DMP_GET_DVBC_PARAM_RSP                0x96380025 //获取DVBC参数的应答
#define   NMS_DMP_SET_DVBC_PARAM                    0x96380026 //设置DVBC参数
#define   NMS_DMP_GET_8QAM_PARAM                    0x96380027 //获取8QAM参数
#define   NMS_DMP_GET_8QAM_PARAM_RSP                0x96380028 //获取8QAM参数的应答
#define   NMS_DMP_SET_8QAM_PARAM                    0x96380029 //设置8QAM参数
#define   NMS_DMP_GET_CI_PARAM                      0x9638002A //获取CI参数
#define   NMS_DMP_GET_CI_PARAM_RSP                  0x9638002B //获取CI参数的应答
#define   NMS_DMP_SET_CI_PARAM                      0x9638002C //设置CI参数
#define   NMS_DMP_GET_SCR_PARAM                     0x9638002D //获取SCRAMBLER参数
#define   NMS_DMP_GET_SCR_PARAM_RSP                 0x9638002E //获取SCRAMBLER参数的应答
#define   NMS_DMP_SET_SCR_CASYSTEM_PARAM            0x9638002F //设置SCRAMBLER CASYSTEM参数
#define   NMS_DMP_SET_SCR_UNIT_PARAM                0x96380030 //设置SCRAMBLER Unit参数

#define   NMS_DMP_GET_SDENCODER_AV_PARAM            0x96380031 //获取SDENCODER_AV参数
#define   NMS_DMP_GET_SDENCODER_AV_PARAM_RSP        0x96380032 //获取SDENCODER_AV参数的应答
#define   NMS_DMP_SET_SDENCODER_AV_PARAM            0x96380033 //设置SDENCODER_AV参数
#define   NMS_DMP_GET_COFDM_PARAM                   0x96380034 //获取COFDM参数
#define   NMS_DMP_GET_COFDM_PARAM_RSP               0x96380035 //获取COFDM参数的应答
#define   NMS_DMP_SET_COFDM_PARAM                   0x96380036 //设置COFDM参数
#define   NMS_DMP_GET_J83B_PARAM                    0x96380037 //获取J83B参数
#define   NMS_DMP_GET_J83B_PARAM_RSP                0x96380038 //获取J83B参数的应答
#define   NMS_DMP_SET_J83B_PARAM                    0x96380039 //设置J83B参数
#define   NMS_DMP_GET_HDENCODER_AV_PARAM            0x9638003A //获取HDENCODER_AV参数
#define   NMS_DMP_GET_HDENCODER_AV_PARAM_RSP        0x9638003B //获取HDENCODER_AV参数的应答
#define   NMS_DMP_SET_HDENCODER_AV_PARAM            0x9638003C //设置HDENCODER_AV参数

#define   NMS_DMP_GET_TRANSCODERMPEG2TO4_PARAM      0x9638003D //获取TRANSCODERMPEG2TO4参数
#define   NMS_DMP_GET_TRANSCODERMPEG2TO4_PARAM_RSP  0x9638003E //获取TRANSCODERMPEG2TO4参数的应答
#define   NMS_DMP_SET_TRANSCODERMPEG2TO4_PARAM      0x9638003F //设置TRANSCODERMPEG2TO4参数
#define   NMS_DMP_GET_HDENCODER_HDMI_PARAM          0x96380040 //获取HDENCODER_HDMI参数
#define   NMS_DMP_GET_HDENCODER_HDMI_PARAM_RSP      0x96380041 //获取HDENCODER_HDMI参数的应答
#define   NMS_DMP_SET_HDENCODER_HDMI_PARAM          0x96380042 //设置HDENCODER_HDMI参数


#define   NMS_DMP_CLEAR_POWER_ALARM                 0x96380048 //清空电源报警
#define   NMS_DMP_SAVE_TS                           0x96380049 //保存主板参数

#define   NMS_DMP_GET_BOARD_IMAGE_LIST              0x96380050 //获取BoardImageList信息
#define   NMS_DMP_GET_BOARD_IMAGE_LIST_RSP          0x96380051 //获取BoardImageList信息的应答
#define   NMS_DMP_SCAN_FLASH                        0x96380052 //扫描FLASH获取BoardImageList信息
#define   NMS_DMP_UPGRADE_AUTHORIZATION             0x96380053 //升级授权信息
#define   NMS_DMP_GET_FLASH_DATA                    0x96380060 //获取FLASH数据
#define   NMS_DMP_GET_FLASH_DATA_RSP                0x96380061 //获取FLASH数据的应答
#define   NMS_DMP_SET_TS_RECV_RSP                   0x96380062 //设置TS信息时的接收数据应答
#define   NMS_DMP_SET_TS_WRITE_SRAM_RSP             0x96380063 //设置TS信息时的写SRAM数据应答
#define   NMS_DMP_SET_TS_CONSTRUCT_SI_RSP           0x96380064 //设置TS信息时的构造SI应答
#define   NMS_DMP_CLEAR_ALL_TS                      0x96380065 //ClearAllTS
#define   NMS_DMP_MAINBOARD_LOGIC_REBOOT            0x96380066 //主板请求复位逻辑
#define   NMS_DMP_UPGRADE_FIRMWARE                  0x96380067 //升级H52
#define   NMS_DMP_GET_PSISI                         0x96380068 //PSISI
#define   NMS_DMP_GET_PSISI_RSP                     0x96380069 //PSISI

#define   NMS_DMP_TEST                              0x963800FE //测试命令

// suppliment
#define   NMS_DMP_SET_SCR_IP_PARAM                  0x96380070 //设置SCRAMBLER IP参数
#define   NMS_DMP_GET_REGISTER_VALUE                0x96380071 //GetRegisterValue
#define   NMS_DMP_GET_SRAM_VALUE                    0x96380072 //GetSRAMValue
#define   NMS_DMP_GET_REGISTER_VALUE_RSP            0x96380073 //GetRegisterValue_RSP
#define   NMS_DMP_GET_SRAM_VALUE_RSP                0x96380074 //GetSRAMValue_RSP
#define   NMS_DMP_GET_SDENCODER_AVSDI_PARAM         0x96380075 //获取SDENCODER_AVSDI参数
#define   NMS_DMP_GET_SDENCODER_AVSDI_PARAM_RSP     0x96380076 //获取SDENCODER_AVSDI参数的应答
#define   NMS_DMP_SET_SDENCODER_AVSDI_PARAM         0x96380077 //设置SDENCODER_AVSDI参数
#define   NMS_DMP_GET_RECEIVER_T_PARAM              0x96380078 //获取RECEIVER_T参数
#define   NMS_DMP_GET_RECEIVER_T_PARAM_RSP          0x96380079 //获取RECEIVER_T参数的应答
#define   NMS_DMP_SET_RECEIVER_T_PARAM              0x9638007A //设置RECEIVER_T参数
#define   NMS_DMP_GET_LOG_REQ                       0x9638007B //获取Log参数
#define   NMS_DMP_GET_LOG_ACK                       0x9638007C //获取Log参数应答
#define   NMS_DMP_GET_LOG_RSP                       0x9638007D //发送Log
#define   NMS_DMP_GET_LOG_END                       0x9638007E //获取Log参数结束应答

#define   NMS_DMP_SET_REGISTER_VALUE                0x96380080 //SetRegisterValue
#define   NMS_DMP_SET_SRAM_VALUE                    0x96380081 //SetSRAMValue
#define   NMS_DMP_SET_CI_MMI                        0x96380082 //CI MMI
#define   NMS_DMP_GET_CI_MMI_RSP                    0x96380083 //CI MMI RSP
#define   NMS_DMP_CAT_TEST                          0x96380084 //CAT TEST
#define   NMS_DMP_SET_SCR_BITRATE_PARAM             0x96380085 //设置SCRAMBLER BITRATE参数

#define   NMS_DMP_GET_TRANSCODER_DECODE_PARAM       0x96380086 //获取TRANSCODER_DECODE参数
#define   NMS_DMP_GET_TRANSCODER_DECODE_PARAM_RSP   0x96380087 //获取TRANSCODER_DECODE参数的应答
#define   NMS_DMP_SET_TRANSCODER_DECODE_PARAM       0x96380088 //设置TRANSCODER_DECODE参数

#define   NMS_DMP_GET_DECODER_PARAM                 0x96380089 //获取DECODE参数
#define   NMS_DMP_GET_DECODER_PARAM_RSP             0x9638008A //获取DECODE参数的应答
#define   NMS_DMP_SET_DECODER_PARAM                 0x9638008B //设置DECODE参数
#define   NMS_DMP_UPGRADE_TRANS_7162_FIRMWARE       0x9638008C //升级转码7162固件
#define   NMS_DMP_UPGRADE_DEC_7162_FIRMWARE         0x9638008D //升级解码7162固件

#define   NMS_DMP_ERASE_SUBBOARD_FLASH_REQUEST      0x96380090 //擦除子板flash请求
#define   NMS_DMP_GET_IPQAM_PARAM                   0x96380091 //获取IPQAM参数
#define   NMS_DMP_GET_IPQAM_PARAM_RSP               0x96380092 //获取IPQAM参数的应答
#define   NMS_DMP_SET_IPQAM_PARAM                   0x96380093 //设置IPQAM参数


#define   NMS_DMP_GET_IRDR2_PARAM                   0x963800A0 //获取IRDR2参数
#define   NMS_DMP_GET_IRDR2_PARAM_RSP               0x963800A1 //获取IRDR2参数的应答
#define   NMS_DMP_SET_IRDR2_PARAM                   0x963800A2 //设置IRDR2参数

#define   NMS_DMP_GET_IRDR2_DECODER_PARAM           0x963800A3 //获取IRDR2 DECODER参数
#define   NMS_DMP_GET_IRDR2_DECODER_PARAM_RSP       0x963800A4 //获取IRDR2 DECODER参数的应答
#define   NMS_DMP_SET_IRDR2_DECODER_PARAM           0x963800A5 //设置IRDR2 DECODER参数

#define   NMS_DMP_UPGRADE_IPQAM_QAM_FIRMWARE        0x963800A6  //升级IPQAM_QAM固件
#define   NMS_DMP_GET_SDENCODER_MAGNUM_PARAM        0x963800A7 //获取SDENCODER_MAGNUM参数
#define   NMS_DMP_GET_SDENCODER_MAGNUM_PARAM_RSP    0x963800A8 //获取SDENCODER_MAGNUM参数的应答
#define   NMS_DMP_SET_SDENCODER_MAGNUM_PARAM        0x963800A9 //设置SDENCODER_MAGNUM参数

#define   NMS_DMP_GET_BATCH_SRAM_VALUE              0x963800AA    //GetBATCHSRAMValue
#define   NMS_DMP_GET_BATCH_SRAM_VALUE_RSP          0x963800AB    //GetBATCHSRAMValue_RSP

#define   NMS_DMP_GET_MPE_PARAM                     0x963800AC //获取MPE参数
#define   NMS_DMP_GET_MPE_PARAM_RSP                 0x963800AD //获取MPE参数的应答
#define   NMS_DMP_SET_MPE_PARAM                     0x963800AE //设置MPE参数

#define   NMS_DMP_GET_ENCODER_MULTIAUDIO_PARAM      0x963800AF //获取_MULTIAUDIO参数
#define   NMS_DMP_GET_ENCODER_MULTIAUDIO_PARAM_RSP  0x963800B0 //获取_MULTIAUDIO参数的应答
#define   NMS_DMP_SET_ENCODER_MULTIAUDIO_PARAM      0x963800B1 //设置_MULTIAUDIO参数

#define   NMS_DMP_GET_SDENCODER_AV4_PARAM           0x963800B2 //获取SDENCODER_AV4参数
#define   NMS_DMP_GET_SDENCODER_AV4_PARAM_RSP       0x963800B3 //获取SDENCODER_AV4参数的应答
#define   NMS_DMP_SET_SDENCODER_AV4_PARAM           0x963800B4 //设置SDENCODER_AV4参数

#define   NMS_DMP_GET_RECEIVER_ATSC_PARAM           0x963800B5 //获取RECEIVER_ATSC参数
#define   NMS_DMP_GET_RECEIVER_ATSC_PARAM_RSP       0x963800B6 //获取RECEIVER_ATSC参数的应答
#define   NMS_DMP_SET_RECEIVER_ATSC_PARAM           0x963800B7 //设置RECEIVER_ATSC参数

#define   NMS_DMP_UPGRADE_AC3_FIRMWARE              0x963800B8  //升级AC3固件

#define   NMS_DMP_GET_RECEIVER_C_T_T2_PARAM         0x963800B9 //获取RECEIVER_C_T_T2参数
#define   NMS_DMP_GET_RECEIVER_C_T_T2_PARAM_RSP     0x963800BA //获取RECEIVER_C_T_T2参数的应答
#define   NMS_DMP_SET_RECEIVER_C_T_T2_PARAM         0x963800BB //设置RECEIVER_C_T_T2参数

#define   NMS_DMP_GET_TRANSCODER_D7PRO_PARAM        0x963800BC //获取TRANSCODER_D7PRO参数
#define   NMS_DMP_GET_TRANSCODER_D7PRO_PARAM_RSP    0x963800BD //获取TRANSCODER_D7PRO参数的应答
#define   NMS_DMP_SET_TRANSCODER_D7PRO_PARAM        0x963800BE //设置TRANSCODER_D7PRO参数
#define   NMS_SAVE_CURTIME_AS_BASETIME              0x963800C7  //保存当前时间作为基准
#define   NMS_IS_CURTIME_IS_PERMITTED               0x963800C8  //判断当前时间是否允许
#define   NMS_LAST_DAYS                             0x963800C9  //剩余天数
#define   NMS_IS_CURTIME_IS_PERMITTED_RSP           0x963800CA //判断当前时间是否允许的应答
#define   NMS_LAST_DAYS_RSP                         0x963800CB //剩余天数的应答

#define   NMS_DMP_CLEAR_LOG_REQ                     0x963800D1 //清除Log参数

#define   NMS_DMP_GET_IQAM_PARAM                    0x963800D9 //获取IQAM参数
#define   NMS_DMP_GET_IQAM_PARAM_RSP                0x963800DA //获取IQAM参数的应答
#define   NMS_DMP_SET_IQAM_PARAM                    0x963800DB //设置IQAM参数

//DS3
#define   NMS_DMP_GET_DS3_PARAM                     0x963800DC //获取DS3参数
#define   NMS_DMP_GET_DS3_PARAM_RSP                 0x963800DD //获取DS3参数的应答
#define   NMS_DMP_SET_DS3_PARAM                     0x963800DE //设置DS3参数


///AVS+ transcoder
#define   NMS_DMP_GET_TCAVS_PARAM                   0x963800DF //获取TCAVS参数
#define   NMS_DMP_GET_TCAVS_PARAM_RSP               0x963800E0 //获取TCAVS参数的应答
#define   NMS_DMP_SET_TCAVS_PARAM                   0x963800E1 //设置TCAVS参数
#define   NMS_DMP_UPGRADE_TCAVS_FIRMWARE            0x963800E2 //升级TCAVS子板的固件

//统一的子板获取参数和设置参数已经升级子板固件的消息
#define   NMS_GET_SB_PARAM                          0x963800F3 //获取子板参数
#define   NMS_GET_SB_PARAM_RSP                      0x963800F4 //获取子板参数的应答
#define   NMS_SET_SB_PARAM                          0x963800F5 //设置子板参数
#define   NMS_UPGRADE_SB_FIRMWARE                   0x963800F6 //升级子板子板的固件

#define   NMS_DMP_TEST                              0x963800FE //测试命令

typedef struct Srv2ClientParameterRsp
{
    U32        dwParameterTotelLen;
    U16        wParameterSendTotelCnt;
    U16        wParameterSendIndex;
    U16        wParameterSendLen;
    U8         arucParameterBuf[MAX_PARAMETER_PER_PACKET_BUF_LEN];
}Srv2ClientParameterRsp_t;

// Msg body definition
typedef struct
{
    U32      dwRslt;
}Srv2ClientCommonRsp;

#endif/*PUB_SERVER_TO_CLIENT_H_*/

