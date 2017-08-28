#ifndef INCLUDE_XML_WEB_XML_H
#define INCLUDE_XML_WEB_XML_H

#include "wv_xml.h"
#include "libxml/xmlmemory.h"
#include "libxml/parser.h"
#include "libxml/xpath.h"
#include "libxml/tree.h"

#include "base.h"

#include "TSProcess.h"

typedef unsigned char U8;
typedef unsigned short U16;

#define STATUS_SUCCCESS ("Success")
#define STATUS_FAILURE  ("Failure")

//主动上报信息
enum _BoardType
{
	BOARD_TYPE_RECV,
	BOARD_TYPE_TRANS,
	BOARD_TYPE_MONITOR,
};

typedef enum _BoardType BoardType;

//错误码
#define XML_ERROR_CODE_DEVINFO    ("001")
#define XML_ERROR_CODE_UPDATE     ("002")
#define XML_ERROR_CODE_RESTART    ("003")
#define XML_ERROR_CODE_FACTORY    ("004")
#define XML_ERROR_CODE_SAVE_CFG   ("005")
#define XML_ERROR_CODE_SET_DEMODULATE  ("006")
#define XML_ERROR_CODE_GET_DEMODULATE  ("007")
#define XML_ERROR_CODE_SET_DECRYPT  ("008")
#define XML_ERROR_CODE_GET_DECRYPT  ("009")

struct _BoardInfo
{
	U8 chassis;
	U8 slot;
	U8 board_type;
	U8 chip_temp;
	U8 status;
	U8 err_code;
	U8 reserved;
};

typedef struct _BoardInfo BoardInfo;
typedef struct _BoardInfo * BoardInfoPtr;


struct _ChannelResource
{
	U8 chassis;
	U8 slot;
	U8 channel;
	U8 reserved1;
	U8 ip_addr[4];
	U16 port;
	U8 reserved2;
};

typedef struct _ChannelResource ChannelResource;
typedef struct _ChannelResource * ChannelResourcePtr;

struct _CorrectingTime
{
	U16 year;
	U8 month;
	U8 day;
	U8 hour;
	U8 minute;
	U8 second;
	U8 reserved;
};

typedef struct _CorrectingTime CorrectingTime;
typedef struct _CorrectingTime * CorrectingTimePtr;


//xml通信
#define STRING_MAX_LEN  (256)
#define CHANNEL_MAX     (4)


struct _XmlKeyValue
{
	char key[STRING_MAX_LEN];
	char value[STRING_MAX_LEN];
};


typedef struct _XmlKeyValue XmlKeyValue;
typedef struct _XmlKeyValue * XmlKeyValuePtr;


struct _XmlDeviceInfo
{
	char oem[STRING_MAX_LEN];
	char card_type[STRING_MAX_LEN];
	char card_id[STRING_MAX_LEN];
	char eth_name[STRING_MAX_LEN];
	char ip_addr[STRING_MAX_LEN];
	char netmask[STRING_MAX_LEN];
	char gateway[STRING_MAX_LEN];
	char pcb_ver[STRING_MAX_LEN];
	char fpga_ver[STRING_MAX_LEN];
	char uboot_ver[STRING_MAX_LEN];
	char kernel_ver[STRING_MAX_LEN];
	char app_ver[STRING_MAX_LEN];
};

typedef struct _XmlDeviceInfo XmlDeviceInfo;
typedef struct _XmlDeviceInfo * XmlDeviceInfoPtr;


//参数设置
struct _ParamsInfo
{
	U16 u16Channel;
	U32 u32Frequency;
	U32 u32SymbolRate;
	U32 u32Modulation;
	U32 u32TsIpAddr;
	U16 u16TsPort;
	U32 u32TsSwitch;
	U32 u32MonitorIpAddr;
	U16 u16MonitorPort;
	U32 u32MonitorSwitch;
};

typedef struct _ParamsInfo ParamsInfo;
typedef struct _ParamsInfo * ParamsInfoPtr;


struct _XmlParamsInfo
{
	XmlKeyValue channel;
	XmlKeyValue frequency;
	XmlKeyValue symbol_rate;
	XmlKeyValue modulation;
	XmlKeyValue ts_out_addr;
	XmlKeyValue ts_out_switch;
	XmlKeyValue monitor_addr;
	XmlKeyValue monitor_switch;
};

typedef struct _XmlParamsInfo XmlParamsInfo;
typedef struct _XmlParamsInfo * XmlParamsInfoPtr;

#define MAX_PROGRAM_NUMBER      (1024)


//节目解解扰信息
struct _ProgramDecrypt 
{
	U32 u32ProgramNumber;
	U16 u16ChannelID;
	U16 u16ServiceID[MAX_PROGRAM_NUMBER];
	U8 u8CAMIndex[MAX_PROGRAM_NUMBER];
};

typedef struct _ProgramDecrypt ProgramDecrypt;
typedef struct _ProgramDecrypt * ProgramDecryptPtr;

struct _XmlProgramDecrypt
{
	U32 u32ProgramNumber;
	XmlKeyValue channel;
	XmlKeyValue service_id[MAX_PROGRAM_NUMBER];
	XmlKeyValue decrypt[MAX_PROGRAM_NUMBER];
};

typedef struct _XmlProgramDecrypt XmlProgramDecrypt;
typedef struct _XmlProgramDecrypt * XmlProgramDecryptPtr;

wvErrCode WebXml_ParseFile(char *filename, buffer* pstWriteBuff);
wvErrCode WebXml_ParseMem(char *str, int str_len, buffer * pstWriteBuff);

wvErrCode WebXml_GetDeviceInfo(xmlDocPtr xml_doc_ptr, char *status, char *comment);
wvErrCode WebXml_Update(xmlDocPtr xml_doc_ptr, char *status, char *comment);
wvErrCode WebXml_Restart(xmlDocPtr xml_doc_ptr, char *status, char *comment);
wvErrCode WebXml_Factory(xmlDocPtr xml_doc_ptr, char *status, char *comment);
wvErrCode WebXml_SaveAllCfg(xmlDocPtr xml_doc_ptr, char *status, char *comment);
wvErrCode WebXml_SetDemodulateInfo(xmlDocPtr xml_doc_ptr, char *status, char *comment);
wvErrCode WebXml_GetDemodulateInfo(xmlDocPtr xml_doc_ptr, char *status, char *comment);
wvErrCode WebXml_SetDecryptInfo(xmlDocPtr xml_doc_ptr, char *status, char *comment);
wvErrCode WebXml_GetDecryptInfo(xmlDocPtr xml_doc_ptr, char *status, char *comment);

wvErrCode WebXml_GetDeviceInfoR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment);
wvErrCode WebXml_UpdateR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment);
wvErrCode WebXml_RestartR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment);
wvErrCode WebXml_FactoryR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment);
wvErrCode WebXml_SaveAllCfgR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment);
wvErrCode WebXml_SetDemodulateInfoR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment);
wvErrCode WebXml_GetDemodulateInfoR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment);
wvErrCode WebXml_SetDecryptInfoR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment);
wvErrCode WebXml_GetDecryptInfoR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment);

wvErrCode WelXml_GetProgramInfoFromTSInfo(TSInfo * pstTS);

#endif



