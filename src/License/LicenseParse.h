/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     LicenseParse.h
* Description:  LicenseParse Include
* Author    :   jie.zhan
* Modified  :
* Reviewer  :
* Date      :   2017-03-28
* Record    :
*
**********************************************************************/

#ifndef __DMP_LICENSEPARSE_H_
#define __DMP_LICENSEPARSE_H_

#include "appGlobal.h"

// Error Code
#define LIC_SUCCESS                      (0)
#define LIC_FAILURE                      (-1)
#define LIC_ERROR_BAD_PARAMS             (-2)
#define LIC_ERROR_CHECKSUM               (-3)
#define LIC_ERROR_COMPANY_SYNC           (-4)
#define LIC_ERROR_NOT_COMPATIBLE         (-5)

// ---- TAG ----
#define TAG_SYNC                         (0xBF)

// (SUBBOARD-TSIP)
#define TAG_TSIP_MAC                     (0x01)
#define TAG_TSIP_CHANNEL                 (0x02)
#define TAG_MODULATOR_CHANNEL            (0x03)
#define TAG_MODULATOR_TYPE               (0x04)
#define TAG_TRANSCODE_TYPE               (0x05)
#define TAG_ENCODER_SDHD                 (0x06) //0��SD 1: HD
#define TAG_DIRECTION                    (0x07) //0:Right->Left 1:Left->Right
#define TAG_RECEIVER_T_CHANNEL           (0x08)
#define TAG_AUDIOENCODE                  (0x09)
#define TAG_ENCODETYPE                   (0x19) //SDMAGNUM

#define TAG_FP_MODEL                     (0x0A) //front panel model
#define TAG_DECODER_SDHD                 (0x0B) //0��SD 1: HD
#define TAG_COMPANY_LOGO                 (0x0C) //booting logo
#define TAG_EIT                          (0x10) //EIT flag

#define TAG_CUSTOMER_NO                  (0x20) //Customer No
#define TAG_MACHINE_TYPE                 (0x21) //MachineType
#define TAG_SOFTWARE_FUNC_VER            (0x22) //Sofeware function Version
#define TAG_TIME_CONSTRAINT              (0x23) //Sofeware Time constraint
#define TAG_CARD_TYPE                    (0x24) //�ӿ����� 1-AC3;2-MAGNUM;3-

#define TAG_TSIP_0IN0OUT_FLAG            (0x25) //TSIP 0in0out Flag

#define TAG_LED_RULE_FLAG                (0x26) //LED Rule: 0 New; 1 Old

#define TAG_QAM_FW_TYPE                  (0x2E) //0:100T       1:150T

#define LIC_TAG_MAX_NUM                  (256)       
#define LIC_TAG_MAX_SIZE                 (21)   // �ݶ������TAG���ݳ���Ϊ21

#define TAG_ENHDMI_PLUS_SDHD             (0x30) //subboard:4HDMI,0��SD 1: HD
#define TAG_ENHDMI_PLUS_AUD              (0x31) //M22 card type: 0:NO AC3 1:AC3
#define TAG_EN4AV_FW_TYPE                (0x32) //M28 chip type: 1:AC3    0:Other   length:1
#define TAG_HDCP_AUTH_FLAG               (0x33) //CHIP9233 HDCP auth:0:OFF 1:ON

//���������ã���Դ��AVS+��H.264�ȸ�ʽ�仯ʱ��������Ҫ�Զ�ʶ����Ƶ��ʽ���ָ���������
#define TAG_VIDEO_DETECTION              (0x35) //0:OFF    1:ON

//���������ã�����Դ������ʱ���Զ��رձ��룻����Դ�ָ�ʱ���Զ��ָ�����
#define TAG_SMART_ENCODE                 (0x36) //0:OFF    1:ON
#define TAG_SCRAMBLER_TYPE               (0x37) //0:AES,  1:DVB_CSA
#define TAG_VIDEO_TYPE                   (0x38)  //0.mpeg2   1.h.264   length:1 
#define TAG_SCR_CA_NUM                   (0x39) //4:4    6:6    length:1
#define TAG_SCR_MAX_UNIT_NUM             (0x3B) //0:75, 1:>75   length:1 

#define LENGTH_CUSTOMER_NO (10)

// ��ȡLIC������Ϣ
typedef struct
{
    U8      TagID;                          // tagID
    U8      TagValid;                       // valid
    U8      TagLen;                         // data len
    U8      TagData[LIC_TAG_MAX_SIZE];      // data buff
} Tag_t;

typedef struct
{
    U8      SubboardID;                     // subboard type
    U8      IsDefaultSetting;               // default setting, no any valid tag
    U8      TagValidNum;                    // ��¼��ЧTag����
    U32     Date;
    U8      u8StructVer;
    U8      Reserved[1];                    //
    Tag_t   TagArray[LIC_TAG_MAX_NUM];      // ��Ϊ���е�TAG���ݲ���ܴ�������ֱ��ʹ�����飬���˷Ѽ�K�ڴ棬���ڴ���--4K

    // the follow is the basic info
    U8      Macaddress0[6];                 // mac address 0
    U8      Macaddress1[6];                 // mac address 1
    U8      InputChannelNum;                // input channel number
    U8      OutputChannelNum;               // output channel number
    U8      ModulatorChannelNum;            // Modulator channel number
    U8      Auth_SDHD[2];                   // encoder support SD or HD
    U8      TranscodeType[2];               // transcode type 1:H264 2:MPEG
    U8      Direction;                      // Direction 0:Right->Left 1:Left->Right
    U8      AudioEncode;                    // 0:video and audio can encode at the same time;1:just audio can encode;
                                            //
    //new add model, company, decode sd/hd info
    U8      Model[20];                      //�ͺ�
    U8      Decode_SDHD;                    //�Ƿ�֧�ָ��壬1:֧�ָ������
                                            // 0:ֻ֧�ֱ���
    U8      Company[10];                    //��˾��
    U8      au8CustomerNo[LENGTH_CUSTOMER_NO];             //�ͻ�����
    U16     u16MachineType;                 //����
    U16     u16SwFuncVer;                   //������ܰ汾
    U32     u32EndTime;                     //��Ȩ��ֹʱ��
    U8      u8EncoderType1;                 //ͨ����������
    U8      u8EncoderType2;                 //ͨ����������
    U8      u8Tsip0in0outFlag;              //tsipͨ��0��0����־
    U8      u8LEDRuleFlag;                  //ǰ����ƹ����־ 0:Ĭ��ֵ�µ�ƹ淶 1:�ϵ�ƹ淶
    U8      u8QAMFWVersion;                 //QAM�̼��汾    0:150T 1:100T
    U8      u8HDMIAuth[4];                  //subboard:4HDMI,0��SD 1: HD
    U8      u8HDMIFWType[2];                //M22 card type: 0:NO AC3 1:AC3
    U8      u8EN4AVM28FWType;               //M28 chip type: 0:NO AC3 1:AC3
    U8      u8VideoDetection;               // �Զ�ʶ����Ƶ��ʽ
    U8      u8SmartEncode;                  // �Զ�����
    U8      u8HDCPAuthFlag;                 //chip9233 HDCP auth:0:OFF 1:ON
    U8      ScramblerType;                  //�����Ż���������
    U8      u8VideoType;                    //0:ֻ֧��Mpeg2  1:֧��mpeg2��H.264    
    U8      u8CaNumber;                     //�����Ż�ca����
    U8      u8MaxScrUnitNumLevel;           //�����Ż������Լ��ŵĽ�Ŀ���ȼ���0:75,  1:512
} LicenseInfo_t;


S32 LIC_ParseLicenseInfo(const U8* pu8LicenseData, LicenseInfo_t* pLicenseInfo);
S32 LIC_IsCompatibleVersion(const U8 * pu8NewLicenseData, const U8 * pu8OldLicenseData);
S32 LIC_ApplyLicenseConfig(LicenseInfo_t* pstrLicenseInfo, U8 *pu8Params);
U8 LIC_GetEITEnablFlag();

#endif /*__DMP_LICENSEPARSE_H_*/

