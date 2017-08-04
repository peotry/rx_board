/**********************************************************************
* Copyright(c), HuiZhou WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName ��PubServerToClient.h
* Description ��common macro define and wellav information of version
*
* Version ��1.0.0
* Author ��tangzhimin   Reviewer :
* Date   ��2010-12-27
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
#define   NMS_DMP_QUIT                              0xa1500002 //Client�����˳�
#define   NMS_DMP_COMMON_RSP                        0x9638FFFE //����Ӧ����Ϣ

#define   NMS_DMP_GET_MAINBOARD_INFO                0x96380001 //��ȡ������Ϣ
#define   NMS_DMP_GET_MAINBOARD_INFO_RSP            0x96380002 //��ȡ������Ϣ��Ӧ��
#define   NMS_DMP_GET_SYSTEM_INFO                   0x96380003 //��ȡ����ϵͳ��Ϣ
#define   NMS_DMP_GET_SYSTEM_INFO_RSP               0x96380004 //��ȡ����ϵͳ��Ϣ��Ϣ��Ӧ��
#define   NMS_DMP_SET_SYSTEM_INFO                   0x96380005 //��������ϵͳ��Ϣ��Ϣ
#define   NMS_DMP_MAINBOARD_REBOOT                  0x96380006 //������������
#define   NMS_DMP_MAINBOARD_FACTORY_SET             0x96380007 //��������ָ���������
#define   NMS_DMP_GET_TS_INFO                       0x96380008 //��ȡTS��Ϣ
#define   NMS_DMP_GET_TS_INFO_RSP                   0x96380009 //��ȡTS��Ϣ��Ӧ��
#define   NMS_DMP_SET_TS_INFO                       0x9638000A //����TS��Ϣ
#define   NMS_DMP_UPGRADE_REQUEST                   0x9638000B //��������
#define   NMS_DMP_ERASE_FLASH_REQUEST               0x9638000C //����FLASH����
#define   NMS_DMP_REFLESH_FLASH_REQUEST             0x9638000D //ˢ��FLASH����
#define   NMS_DMP_SEND_FILE_REQUEST                 0x9638000E //��������
#define   NMS_DMP_WRITE_FLASH_RET                   0x9638000F //дFLASH����
#define   NMS_DMP_GET_SLOT_INFO                     0x96380010 //��ȡSLOT��Ϣ
#define   NMS_DMP_GET_SLOT_INFO_RSP                 0x96380011 //��ȡSLOT��Ϣ��Ӧ��
#define   NMS_DMP_GET_SINGLE_TS_INFO                0x96380012 //��ȡĳ��TS��Ϣ
#define   NMS_DMP_SET_MAINBOARD_INFO                0x96380013 //����TS��Ϣ
#define   NMS_DMP_CLEAR_SINGLE_TS_INFO              0x96380014 //���ĳ��TS��Ϣ

#define   NMS_DMP_SUBBOARD_POWEROFF                 0x96380015 //�Ӱ�ϵ�
#define   NMS_DMP_SUBBOARD_REBOOT                   0x96380016 //�Ӱ�����
#define   NMS_DMP_SUBBOARD_FACTORYSET               0x96380017 //�Ӱ�ָ�����
#define   NMS_DMP_GET_TSOVERIP_PARAM                0x96380018 //��ȡTSOVERIP����
#define   NMS_DMP_GET_TSOVERIP_PARAM_RSP            0x96380019 //��ȡTSOVERIP������Ӧ��
#define   NMS_DMP_SET_TSOVERIP_PARAM                0x9638001A //����TSOVERIP����
#define   NMS_DMP_GET_ASI_PARAM                     0x9638001B //��ȡASI����
#define   NMS_DMP_GET_ASI_PARAM_RSP                 0x9638001C //��ȡASI������Ӧ��
#define   NMS_DMP_SET_ASI_PARAM                     0x9638001D //����ASI����
#define   NMS_DMP_GET_AUTHORIZATION                 0x9638001E //��ȡ��Ȩ��Ϣ
#define   NMS_DMP_GET_AUTHORIZATION_RSP             0x9638001F //��ȡ��Ȩ��Ϣ��Ӧ��
#define   NMS_DMP_SET_AUTHORIZATION                 0x96380020 //������Ȩ��Ϣ
#define   NMS_DMP_GET_DVBS2_PARAM                   0x96380021 //��ȡDVBS2����
#define   NMS_DMP_GET_DVBS2_PARAM_RSP               0x96380022 //��ȡDVBS2������Ӧ��
#define   NMS_DMP_SET_DVBS2_PARAM                   0x96380023 //����DVBS2����
#define   NMS_DMP_GET_DVBC_PARAM                    0x96380024 //��ȡDVBC����
#define   NMS_DMP_GET_DVBC_PARAM_RSP                0x96380025 //��ȡDVBC������Ӧ��
#define   NMS_DMP_SET_DVBC_PARAM                    0x96380026 //����DVBC����
#define   NMS_DMP_GET_8QAM_PARAM                    0x96380027 //��ȡ8QAM����
#define   NMS_DMP_GET_8QAM_PARAM_RSP                0x96380028 //��ȡ8QAM������Ӧ��
#define   NMS_DMP_SET_8QAM_PARAM                    0x96380029 //����8QAM����
#define   NMS_DMP_GET_CI_PARAM                      0x9638002A //��ȡCI����
#define   NMS_DMP_GET_CI_PARAM_RSP                  0x9638002B //��ȡCI������Ӧ��
#define   NMS_DMP_SET_CI_PARAM                      0x9638002C //����CI����
#define   NMS_DMP_GET_SCR_PARAM                     0x9638002D //��ȡSCRAMBLER����
#define   NMS_DMP_GET_SCR_PARAM_RSP                 0x9638002E //��ȡSCRAMBLER������Ӧ��
#define   NMS_DMP_SET_SCR_CASYSTEM_PARAM            0x9638002F //����SCRAMBLER CASYSTEM����
#define   NMS_DMP_SET_SCR_UNIT_PARAM                0x96380030 //����SCRAMBLER Unit����

#define   NMS_DMP_GET_SDENCODER_AV_PARAM            0x96380031 //��ȡSDENCODER_AV����
#define   NMS_DMP_GET_SDENCODER_AV_PARAM_RSP        0x96380032 //��ȡSDENCODER_AV������Ӧ��
#define   NMS_DMP_SET_SDENCODER_AV_PARAM            0x96380033 //����SDENCODER_AV����
#define   NMS_DMP_GET_COFDM_PARAM                   0x96380034 //��ȡCOFDM����
#define   NMS_DMP_GET_COFDM_PARAM_RSP               0x96380035 //��ȡCOFDM������Ӧ��
#define   NMS_DMP_SET_COFDM_PARAM                   0x96380036 //����COFDM����
#define   NMS_DMP_GET_J83B_PARAM                    0x96380037 //��ȡJ83B����
#define   NMS_DMP_GET_J83B_PARAM_RSP                0x96380038 //��ȡJ83B������Ӧ��
#define   NMS_DMP_SET_J83B_PARAM                    0x96380039 //����J83B����
#define   NMS_DMP_GET_HDENCODER_AV_PARAM            0x9638003A //��ȡHDENCODER_AV����
#define   NMS_DMP_GET_HDENCODER_AV_PARAM_RSP        0x9638003B //��ȡHDENCODER_AV������Ӧ��
#define   NMS_DMP_SET_HDENCODER_AV_PARAM            0x9638003C //����HDENCODER_AV����

#define   NMS_DMP_GET_TRANSCODERMPEG2TO4_PARAM      0x9638003D //��ȡTRANSCODERMPEG2TO4����
#define   NMS_DMP_GET_TRANSCODERMPEG2TO4_PARAM_RSP  0x9638003E //��ȡTRANSCODERMPEG2TO4������Ӧ��
#define   NMS_DMP_SET_TRANSCODERMPEG2TO4_PARAM      0x9638003F //����TRANSCODERMPEG2TO4����
#define   NMS_DMP_GET_HDENCODER_HDMI_PARAM          0x96380040 //��ȡHDENCODER_HDMI����
#define   NMS_DMP_GET_HDENCODER_HDMI_PARAM_RSP      0x96380041 //��ȡHDENCODER_HDMI������Ӧ��
#define   NMS_DMP_SET_HDENCODER_HDMI_PARAM          0x96380042 //����HDENCODER_HDMI����


#define   NMS_DMP_CLEAR_POWER_ALARM                 0x96380048 //��յ�Դ����
#define   NMS_DMP_SAVE_TS                           0x96380049 //�����������

#define   NMS_DMP_GET_BOARD_IMAGE_LIST              0x96380050 //��ȡBoardImageList��Ϣ
#define   NMS_DMP_GET_BOARD_IMAGE_LIST_RSP          0x96380051 //��ȡBoardImageList��Ϣ��Ӧ��
#define   NMS_DMP_SCAN_FLASH                        0x96380052 //ɨ��FLASH��ȡBoardImageList��Ϣ
#define   NMS_DMP_UPGRADE_AUTHORIZATION             0x96380053 //������Ȩ��Ϣ
#define   NMS_DMP_GET_FLASH_DATA                    0x96380060 //��ȡFLASH����
#define   NMS_DMP_GET_FLASH_DATA_RSP                0x96380061 //��ȡFLASH���ݵ�Ӧ��
#define   NMS_DMP_SET_TS_RECV_RSP                   0x96380062 //����TS��Ϣʱ�Ľ�������Ӧ��
#define   NMS_DMP_SET_TS_WRITE_SRAM_RSP             0x96380063 //����TS��Ϣʱ��дSRAM����Ӧ��
#define   NMS_DMP_SET_TS_CONSTRUCT_SI_RSP           0x96380064 //����TS��Ϣʱ�Ĺ���SIӦ��
#define   NMS_DMP_CLEAR_ALL_TS                      0x96380065 //ClearAllTS
#define   NMS_DMP_MAINBOARD_LOGIC_REBOOT            0x96380066 //��������λ�߼�
#define   NMS_DMP_UPGRADE_FIRMWARE                  0x96380067 //����H52
#define   NMS_DMP_GET_PSISI                         0x96380068 //PSISI
#define   NMS_DMP_GET_PSISI_RSP                     0x96380069 //PSISI

#define   NMS_DMP_TEST                              0x963800FE //��������

// suppliment
#define   NMS_DMP_SET_SCR_IP_PARAM                  0x96380070 //����SCRAMBLER IP����
#define   NMS_DMP_GET_REGISTER_VALUE                0x96380071 //GetRegisterValue
#define   NMS_DMP_GET_SRAM_VALUE                    0x96380072 //GetSRAMValue
#define   NMS_DMP_GET_REGISTER_VALUE_RSP            0x96380073 //GetRegisterValue_RSP
#define   NMS_DMP_GET_SRAM_VALUE_RSP                0x96380074 //GetSRAMValue_RSP
#define   NMS_DMP_GET_SDENCODER_AVSDI_PARAM         0x96380075 //��ȡSDENCODER_AVSDI����
#define   NMS_DMP_GET_SDENCODER_AVSDI_PARAM_RSP     0x96380076 //��ȡSDENCODER_AVSDI������Ӧ��
#define   NMS_DMP_SET_SDENCODER_AVSDI_PARAM         0x96380077 //����SDENCODER_AVSDI����
#define   NMS_DMP_GET_RECEIVER_T_PARAM              0x96380078 //��ȡRECEIVER_T����
#define   NMS_DMP_GET_RECEIVER_T_PARAM_RSP          0x96380079 //��ȡRECEIVER_T������Ӧ��
#define   NMS_DMP_SET_RECEIVER_T_PARAM              0x9638007A //����RECEIVER_T����
#define   NMS_DMP_GET_LOG_REQ                       0x9638007B //��ȡLog����
#define   NMS_DMP_GET_LOG_ACK                       0x9638007C //��ȡLog����Ӧ��
#define   NMS_DMP_GET_LOG_RSP                       0x9638007D //����Log
#define   NMS_DMP_GET_LOG_END                       0x9638007E //��ȡLog��������Ӧ��

#define   NMS_DMP_SET_REGISTER_VALUE                0x96380080 //SetRegisterValue
#define   NMS_DMP_SET_SRAM_VALUE                    0x96380081 //SetSRAMValue
#define   NMS_DMP_SET_CI_MMI                        0x96380082 //CI MMI
#define   NMS_DMP_GET_CI_MMI_RSP                    0x96380083 //CI MMI RSP
#define   NMS_DMP_CAT_TEST                          0x96380084 //CAT TEST
#define   NMS_DMP_SET_SCR_BITRATE_PARAM             0x96380085 //����SCRAMBLER BITRATE����

#define   NMS_DMP_GET_TRANSCODER_DECODE_PARAM       0x96380086 //��ȡTRANSCODER_DECODE����
#define   NMS_DMP_GET_TRANSCODER_DECODE_PARAM_RSP   0x96380087 //��ȡTRANSCODER_DECODE������Ӧ��
#define   NMS_DMP_SET_TRANSCODER_DECODE_PARAM       0x96380088 //����TRANSCODER_DECODE����

#define   NMS_DMP_GET_DECODER_PARAM                 0x96380089 //��ȡDECODE����
#define   NMS_DMP_GET_DECODER_PARAM_RSP             0x9638008A //��ȡDECODE������Ӧ��
#define   NMS_DMP_SET_DECODER_PARAM                 0x9638008B //����DECODE����
#define   NMS_DMP_UPGRADE_TRANS_7162_FIRMWARE       0x9638008C //����ת��7162�̼�
#define   NMS_DMP_UPGRADE_DEC_7162_FIRMWARE         0x9638008D //��������7162�̼�

#define   NMS_DMP_ERASE_SUBBOARD_FLASH_REQUEST      0x96380090 //�����Ӱ�flash����
#define   NMS_DMP_GET_IPQAM_PARAM                   0x96380091 //��ȡIPQAM����
#define   NMS_DMP_GET_IPQAM_PARAM_RSP               0x96380092 //��ȡIPQAM������Ӧ��
#define   NMS_DMP_SET_IPQAM_PARAM                   0x96380093 //����IPQAM����


#define   NMS_DMP_GET_IRDR2_PARAM                   0x963800A0 //��ȡIRDR2����
#define   NMS_DMP_GET_IRDR2_PARAM_RSP               0x963800A1 //��ȡIRDR2������Ӧ��
#define   NMS_DMP_SET_IRDR2_PARAM                   0x963800A2 //����IRDR2����

#define   NMS_DMP_GET_IRDR2_DECODER_PARAM           0x963800A3 //��ȡIRDR2 DECODER����
#define   NMS_DMP_GET_IRDR2_DECODER_PARAM_RSP       0x963800A4 //��ȡIRDR2 DECODER������Ӧ��
#define   NMS_DMP_SET_IRDR2_DECODER_PARAM           0x963800A5 //����IRDR2 DECODER����

#define   NMS_DMP_UPGRADE_IPQAM_QAM_FIRMWARE        0x963800A6  //����IPQAM_QAM�̼�
#define   NMS_DMP_GET_SDENCODER_MAGNUM_PARAM        0x963800A7 //��ȡSDENCODER_MAGNUM����
#define   NMS_DMP_GET_SDENCODER_MAGNUM_PARAM_RSP    0x963800A8 //��ȡSDENCODER_MAGNUM������Ӧ��
#define   NMS_DMP_SET_SDENCODER_MAGNUM_PARAM        0x963800A9 //����SDENCODER_MAGNUM����

#define   NMS_DMP_GET_BATCH_SRAM_VALUE              0x963800AA    //GetBATCHSRAMValue
#define   NMS_DMP_GET_BATCH_SRAM_VALUE_RSP          0x963800AB    //GetBATCHSRAMValue_RSP

#define   NMS_DMP_GET_MPE_PARAM                     0x963800AC //��ȡMPE����
#define   NMS_DMP_GET_MPE_PARAM_RSP                 0x963800AD //��ȡMPE������Ӧ��
#define   NMS_DMP_SET_MPE_PARAM                     0x963800AE //����MPE����

#define   NMS_DMP_GET_ENCODER_MULTIAUDIO_PARAM      0x963800AF //��ȡ_MULTIAUDIO����
#define   NMS_DMP_GET_ENCODER_MULTIAUDIO_PARAM_RSP  0x963800B0 //��ȡ_MULTIAUDIO������Ӧ��
#define   NMS_DMP_SET_ENCODER_MULTIAUDIO_PARAM      0x963800B1 //����_MULTIAUDIO����

#define   NMS_DMP_GET_SDENCODER_AV4_PARAM           0x963800B2 //��ȡSDENCODER_AV4����
#define   NMS_DMP_GET_SDENCODER_AV4_PARAM_RSP       0x963800B3 //��ȡSDENCODER_AV4������Ӧ��
#define   NMS_DMP_SET_SDENCODER_AV4_PARAM           0x963800B4 //����SDENCODER_AV4����

#define   NMS_DMP_GET_RECEIVER_ATSC_PARAM           0x963800B5 //��ȡRECEIVER_ATSC����
#define   NMS_DMP_GET_RECEIVER_ATSC_PARAM_RSP       0x963800B6 //��ȡRECEIVER_ATSC������Ӧ��
#define   NMS_DMP_SET_RECEIVER_ATSC_PARAM           0x963800B7 //����RECEIVER_ATSC����

#define   NMS_DMP_UPGRADE_AC3_FIRMWARE              0x963800B8  //����AC3�̼�

#define   NMS_DMP_GET_RECEIVER_C_T_T2_PARAM         0x963800B9 //��ȡRECEIVER_C_T_T2����
#define   NMS_DMP_GET_RECEIVER_C_T_T2_PARAM_RSP     0x963800BA //��ȡRECEIVER_C_T_T2������Ӧ��
#define   NMS_DMP_SET_RECEIVER_C_T_T2_PARAM         0x963800BB //����RECEIVER_C_T_T2����

#define   NMS_DMP_GET_TRANSCODER_D7PRO_PARAM        0x963800BC //��ȡTRANSCODER_D7PRO����
#define   NMS_DMP_GET_TRANSCODER_D7PRO_PARAM_RSP    0x963800BD //��ȡTRANSCODER_D7PRO������Ӧ��
#define   NMS_DMP_SET_TRANSCODER_D7PRO_PARAM        0x963800BE //����TRANSCODER_D7PRO����
#define   NMS_SAVE_CURTIME_AS_BASETIME              0x963800C7  //���浱ǰʱ����Ϊ��׼
#define   NMS_IS_CURTIME_IS_PERMITTED               0x963800C8  //�жϵ�ǰʱ���Ƿ�����
#define   NMS_LAST_DAYS                             0x963800C9  //ʣ������
#define   NMS_IS_CURTIME_IS_PERMITTED_RSP           0x963800CA //�жϵ�ǰʱ���Ƿ������Ӧ��
#define   NMS_LAST_DAYS_RSP                         0x963800CB //ʣ��������Ӧ��

#define   NMS_DMP_CLEAR_LOG_REQ                     0x963800D1 //���Log����

#define   NMS_DMP_GET_IQAM_PARAM                    0x963800D9 //��ȡIQAM����
#define   NMS_DMP_GET_IQAM_PARAM_RSP                0x963800DA //��ȡIQAM������Ӧ��
#define   NMS_DMP_SET_IQAM_PARAM                    0x963800DB //����IQAM����

//DS3
#define   NMS_DMP_GET_DS3_PARAM                     0x963800DC //��ȡDS3����
#define   NMS_DMP_GET_DS3_PARAM_RSP                 0x963800DD //��ȡDS3������Ӧ��
#define   NMS_DMP_SET_DS3_PARAM                     0x963800DE //����DS3����


///AVS+ transcoder
#define   NMS_DMP_GET_TCAVS_PARAM                   0x963800DF //��ȡTCAVS����
#define   NMS_DMP_GET_TCAVS_PARAM_RSP               0x963800E0 //��ȡTCAVS������Ӧ��
#define   NMS_DMP_SET_TCAVS_PARAM                   0x963800E1 //����TCAVS����
#define   NMS_DMP_UPGRADE_TCAVS_FIRMWARE            0x963800E2 //����TCAVS�Ӱ�Ĺ̼�

//ͳһ���Ӱ��ȡ���������ò����Ѿ������Ӱ�̼�����Ϣ
#define   NMS_GET_SB_PARAM                          0x963800F3 //��ȡ�Ӱ����
#define   NMS_GET_SB_PARAM_RSP                      0x963800F4 //��ȡ�Ӱ������Ӧ��
#define   NMS_SET_SB_PARAM                          0x963800F5 //�����Ӱ����
#define   NMS_UPGRADE_SB_FIRMWARE                   0x963800F6 //�����Ӱ��Ӱ�Ĺ̼�

#define   NMS_DMP_TEST                              0x963800FE //��������

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

