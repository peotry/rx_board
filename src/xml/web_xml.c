#include "web_xml.h"
#include "wv_xml.h"

#include "err/wv_err.h"
#include "log/wv_log.h"
#include "WV_CI.h"
#include "tuner_c.h"
#include "thread/thread.h"

#include <stdlib.h>
#include <stdio.h>

static char s_arrQAMMode[][STRING_MAX_LEN] = {
		"QAM16",
		"QAM32",
		"QAM64",
		"QAM128",
		"QAM256",
	};

static XmlDeviceInfo s_stXmlDeviceInfo;
static XmlParamsInfo s_arrstXmlParamsInfo[WVCI_MAX_SLOT_NUM];
static XmlProgramDecrypt s_arrstXmlProgramDecrypt[WVCI_MAX_SLOT_NUM];

static ParamsInfo s_arrstParamsInfo[WVCI_MAX_SLOT_NUM];
static ProgramDecrypt s_arrstProgramDecrypt[WVCI_MAX_SLOT_NUM];

struct _CmdMapFunc
{
	char recv_cmd_type[STRING_MAX_LEN];
	wvErrCode (*call_func)(xmlDocPtr , char *, char *);
};

typedef struct _CmdMapFunc CmdMapFunc;
typedef struct _CmdMapFunc * CmdMapFuncPtr;


CmdMapFunc cmd_map_func[] = 
{
	{"GetDeviceInfo", WebXml_GetDeviceInfo},
   	{"Update", WebXml_Update},
   	{"Restart", WebXml_Restart},
	{"Factory", WebXml_Factory},
	{"SaveAllCfg", WebXml_SaveAllCfg},
	{"SetDemodulateInfo", WebXml_SetDemodulateInfo},
	{"GetDemodulateInfo", WebXml_GetDemodulateInfo},
   	{"SetDecryptInfo", WebXml_SetDecryptInfo},
	{"GetDecryptInfo", WebXml_GetDecryptInfo},
	
};


struct _ReplyCmdMapFunc
{
	char recv_cmd_type[STRING_MAX_LEN];
	wvErrCode (*call_func)(xmlDocPtr *, char *, char *);
};

typedef struct _ReplyCmdMapFunc ReplyCmdMapFunc;
typedef struct _ReplyCmdMapFunc * ReplyCmdMapFuncPtr;


ReplyCmdMapFunc reply_cmd_map_func[] = 
{
	{"GetDeviceInfoR", WebXml_GetDeviceInfoR},
   	{"UpdateR", WebXml_UpdateR},
   	{"RestartR", WebXml_RestartR},
	{"FactoryR", WebXml_FactoryR},
	{"SaveAllCfgR", WebXml_SaveAllCfgR},
	{"SetDemodulateInfoR", WebXml_SetDemodulateInfoR},
	{"GetDemodulateInfoR", WebXml_GetDemodulateInfoR},
   	{"SetDecryptInfoR", WebXml_SetDecryptInfoR},
	{"GetDecryptInfoR", WebXml_GetDecryptInfoR},
   	
};



/*
* function: WebXml_GetIpPortFromStr
*
* description:  从[ip:port] 格式的字符串中获取ip 和 port
*
* input:  @str: [ip:port] 格式字符串
*
* output: @
*
* return@ 
* success: 
*    fail: 
*
* author: linsheng.pan
*/
wvErrCode WebXml_GetIpPortFromStr(const char *str, U32 *pu32Ip, U16 * pu16Port)
{
	if(!str)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_PARAMS;
	}

	char ip_addr[16] = {0};
	char port[6] = {0};
	char *ptr = NULL;

	//找到ip地址的字符串
	if(NULL == (ptr = strstr(str, ":")))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: str is error");
		return WV_ERR_PARAMS;
	}
	
	memcpy(ip_addr, str, ptr - str);
	inet_pton(AF_INET, ip_addr, pu32Ip);

	*pu32Ip = ntohl(*pu32Ip);

	//找到port 的字符串
	ptr++;
	strncpy(port, ptr, sizeof(port));
	*pu16Port = atoi(port);

	printf("ip: 0x%x, port: %u\n", *pu32Ip, *pu16Port);
	
	return WV_SUCCESS;
}


/*
* function: WebXml_SetIpPortToStr
*
* description: 将ip 和 port 转换为 [ip:port]格式的字符串
*
* input:  @
*
* output: @
*
* return@ 
* success: 
*    fail: 
*
* author: linsheng.pan
*/
wvErrCode WebXml_SetIpPortToStr(const U32 u32Ip, const U16 u16Port, char *str, int str_len)
{
	if(!str)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_PARAMS;
	}

	const U32 u32IpPortLen = 16 + 1 + 6;
	char ip_str[16] = {0};
	U32 u32IPAddr = 0;

	if(str_len <= u32IpPortLen)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_PARAMS;
	}

	u32IPAddr = htonl(u32Ip);

	inet_ntop(AF_INET, &u32IPAddr, ip_str, sizeof(ip_str));

	printf("%s\n", ip_str);
	snprintf(str, str_len, "%s:%u", ip_str, u16Port);
	

	printf("addr: %s\n", str);

	return WV_SUCCESS;
	
}


/*
* function: WebXml_XmlParams2StructParams
*
* description: 将XML结构体转换为数值结构体
*
* input:  @
*
* output: @
*
* return@ 
* success: 
*    fail: 
*
* author: linsheng.pan
*/
static void WebXml_XmlParams2StructParams(void)
{
	U32 i = 0;
	U32 j = 0;
	U32 u32IpAddr = 0;
	U16 u16Port = 0;
	
	/*
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
	*/

	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		s_arrstParamsInfo[i].u16Channel = atoi(s_arrstXmlParamsInfo[i].channel.value);
		s_arrstParamsInfo[i].u32Frequency = atoi(s_arrstXmlParamsInfo[i].frequency.value);
		s_arrstParamsInfo[i].u32SymbolRate = atoi(s_arrstXmlParamsInfo[i].symbol_rate.value);

		for(j = 0; j < sizeof(s_arrQAMMode)/STRING_MAX_LEN; ++j)
		{
			if(!strcmp(s_arrQAMMode[j], s_arrstXmlParamsInfo[i].modulation.value))
			{
				s_arrstParamsInfo[i].u32Modulation = j;
				break;
			}
		}

		//获取ts ip 和端口
		WebXml_GetIpPortFromStr(s_arrstXmlParamsInfo[i].ts_out_addr.value, &u32IpAddr, &u16Port);
		s_arrstParamsInfo[i].u32TsIpAddr = u32IpAddr;
		s_arrstParamsInfo[i].u16TsPort = u16Port;
		s_arrstParamsInfo[i].u32TsSwitch = atoi(s_arrstXmlParamsInfo[i].ts_out_switch.value);

		//获取monitor ip 和端口
		WebXml_GetIpPortFromStr(s_arrstXmlParamsInfo[i].monitor_addr.value, &u32IpAddr, &u16Port);
		s_arrstParamsInfo[i].u32MonitorIpAddr = u32IpAddr;
		s_arrstParamsInfo[i].u16MonitorPort = u16Port;
		s_arrstParamsInfo[i].u32MonitorSwitch = atoi(s_arrstXmlParamsInfo[i].monitor_switch.value);
		
	}
	
}


/*
* function: WebXml_XmlProgram2StructProgram
*
* description: 将xml结构体转换为数值结构体
*
* input:  @
*
* output: @
*
* return@ 
* success: 
*    fail: 
*
* author: linsheng.pan
*/
static void WebXml_XmlProgram2StructProgram(void)
{
	U32 i = 0;
	U32 j = 0;
	U32 u32CAMIndex = 0;
	U32 u32Decrypt = 0;

	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		s_arrstProgramDecrypt[i].u16ChannelID = atoi(s_arrstXmlProgramDecrypt[i].channel.value);
		s_arrstProgramDecrypt[i].u32ProgramNumber = s_arrstXmlProgramDecrypt[i].u32ProgramNumber;
		for(j = 0; j < s_arrstXmlProgramDecrypt[i].u32ProgramNumber; ++j)
		{
			s_arrstProgramDecrypt[i].u16ServiceID[j] = atoi(s_arrstXmlProgramDecrypt[i].service_id[j].value);
			u32Decrypt = atoi(s_arrstXmlProgramDecrypt[i].decrypt[j].value);

			if(0 == u32Decrypt)
			{	//不解扰
				s_arrstProgramDecrypt[i].u8CAMIndex[j] = INVALID_CAM_INDEX;
			}
			else
			{
				//解扰
				s_arrstProgramDecrypt[i].u8CAMIndex[j] = s_arrstProgramDecrypt[i].u16ChannelID;
			}
		}
	}
	
}


/*
* function: WebXml_StructProgram2XmlProgram
*
* description: 将数值结构体转换为xml结构体
*
* input:  @
*
* output: @
*
* return@ 
* success: 
*    fail: 
*
* author: linsheng.pan
*/
static void WebXml_StructProgram2XmlProgram(void)
{
	U32 i = 0;
	U32 j = 0;
	U32 u32ProgramNumber = 0;
	U16 u16ChannelID = 0;
	U8 u8CAMIndex = 0;
	char strChannelId[10] = {0};

	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		//channel 节目数量
		u32ProgramNumber = s_arrstProgramDecrypt[i].u32ProgramNumber;
		s_arrstXmlProgramDecrypt[i].u32ProgramNumber = u32ProgramNumber;

		//channel ID
		snprintf(s_arrstXmlProgramDecrypt[i].channel.value, STRING_MAX_LEN, "%d", s_arrstProgramDecrypt[i].u16ChannelID);

		for(j = 0; j < u32ProgramNumber; ++j)
		{
			//Server ID
			snprintf(s_arrstXmlProgramDecrypt[i].service_id[j].value, STRING_MAX_LEN, "%d", s_arrstProgramDecrypt[i].u16ServiceID[j]);
			u8CAMIndex = s_arrstProgramDecrypt[i].u8CAMIndex[j];
			if(INVALID_CAM_INDEX == u8CAMIndex)
			{
				//不解扰
				strncpy(s_arrstXmlProgramDecrypt[i].decrypt[j].value, "0", STRING_MAX_LEN);
			}
			else
			{
				strncpy(s_arrstXmlProgramDecrypt[i].decrypt[j].value, "1", STRING_MAX_LEN);
			}
		}
	}
}


/*
* function: WebXml_StructParamsXmlParams
*
* description: 将数值结构体转换为xml结构体
*
* input:  @
*
* output: @
*
* return@ 
* success: 
*    fail: 
*
* author: linsheng.pan
*/
static void WebXml_StructParams2XmlParams(void)
{
	//TODO
	/*
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
	*/

	U32 i = 0;

	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		snprintf(s_arrstXmlParamsInfo[i].channel.value, STRING_MAX_LEN, "%u", s_arrstParamsInfo[i].u16Channel);
		snprintf(s_arrstXmlParamsInfo[i].frequency.value, STRING_MAX_LEN, "%u", s_arrstParamsInfo[i].u32Frequency);
		snprintf(s_arrstXmlParamsInfo[i].symbol_rate.value, STRING_MAX_LEN, "%u", s_arrstParamsInfo[i].u32SymbolRate);
		snprintf(s_arrstXmlParamsInfo[i].modulation.value, STRING_MAX_LEN, "%s", s_arrQAMMode[s_arrstParamsInfo[i].u32Modulation]);
		WebXml_SetIpPortToStr(s_arrstParamsInfo[i].u32TsIpAddr, s_arrstParamsInfo[i].u16TsPort, s_arrstXmlParamsInfo[i].ts_out_addr.value, STRING_MAX_LEN);
		snprintf(s_arrstXmlParamsInfo[i].ts_out_switch.value, STRING_MAX_LEN, "%u", s_arrstParamsInfo[i].u32TsSwitch);
		printf("WebXml_StructParams2XmlParams--ts_out_switch[%s] u32TsSwitch[%u]\n", s_arrstXmlParamsInfo[i].ts_out_switch.value, s_arrstParamsInfo[i].u32TsSwitch);
		WebXml_SetIpPortToStr(s_arrstParamsInfo[i].u32MonitorIpAddr, s_arrstParamsInfo[i].u16MonitorPort, s_arrstXmlParamsInfo[i].monitor_addr.value, STRING_MAX_LEN);
		snprintf(s_arrstXmlParamsInfo[i].monitor_switch.value, STRING_MAX_LEN, "%u", s_arrstParamsInfo[i].u32MonitorSwitch);
	}
}


/*
* function: WebXml_InitDeviceInfo
*
* description: 初始化设备信息
*
* input:  @
*
* output: @
*
* return@ 
* success: 
*    fail: 
*
* author: linsheng.pan
*/
wvErrCode WebXml_InitDeviceInfo(void)
{
	LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_XML, "Init DeviceInfo...");

	U8 aru8IPAddr[16] = {0};
	U8 aru8NetMask[16] = {0};
	U8 aru8Gateway[16] = {0};
	char eth_name[16] = "eth1";
	U8 aru8AppVer[16] = {0};
	U8 aru8FPGAVer[16] = {0};

	Net_GetIpAddrStr(eth_name, aru8IPAddr, sizeof(aru8IPAddr));
	Net_GetLocalNetMask(eth_name, aru8NetMask, sizeof(aru8NetMask));
	Net_GetGateWay(aru8Gateway, sizeof(aru8Gateway));
	snprintf(aru8AppVer, sizeof(aru8AppVer), "V%u", SW_VER);
	snprintf(aru8FPGAVer, sizeof(aru8FPGAVer), "V%u", FPGA_GetLogicVersion());
	
	strncpy(s_stXmlDeviceInfo.oem, "wellav", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.card_type, "0", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.card_id, "100", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.eth_name, eth_name, STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.ip_addr, aru8IPAddr, STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.netmask, aru8NetMask, STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.gateway, aru8Gateway, STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.pcb_ver, "V1.0.0", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.fpga_ver, aru8FPGAVer, STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.uboot_ver, "V3.0.0", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.kernel_ver, "V4.0.0", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.app_ver, aru8AppVer, STRING_MAX_LEN);

	//TODO
	//初始化设备数值结构体

	return WV_SUCCESS;
	
}


XmlDeviceInfoPtr WebXml_GetXmlDeviceInfoPtr(void)
{
	return (XmlDeviceInfoPtr)(&s_stXmlDeviceInfo);
}


/*
* function: WebXml_InitParamsInfo
*
* description: 初始化参数信息
*
* input:  @
*
* output: @
*
* return@ 
* success: 
*    fail: 
*
* author: linsheng.pan
*/
wvErrCode WebXml_InitParamsInfo(void)
{
	printf("WebXml_InitParamsInfo ...\n");
	char path_msg[STRING_MAX_LEN] = {0};
	char key_msg[STRING_MAX_LEN] = {0};
	char value_msg[STRING_MAX_LEN] = {0};
	int i = 0;
	
	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		snprintf(path_msg, sizeof(path_msg), "/Msg/Data[%d]/", i + 1);

		snprintf(key_msg, sizeof(key_msg), "%sChannel", path_msg);
		strncpy(s_arrstXmlParamsInfo[i].channel.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "%d", i);
		strncpy(s_arrstXmlParamsInfo[i].channel.value, value_msg, STRING_MAX_LEN);
		printf("Channel[%d]: %s\n", i, value_msg);

		snprintf(key_msg, sizeof(key_msg), "%sFrequency", path_msg);
		strncpy(s_arrstXmlParamsInfo[i].frequency.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "208000");
		strncpy(s_arrstXmlParamsInfo[i].frequency.value, value_msg, STRING_MAX_LEN);
		printf("Frequency[%d]: %s\n", i, value_msg);
		printf("#####s_arrstXmlParamsInfo[i].channel.value = %s\n", s_arrstXmlParamsInfo[i].channel.value);


		snprintf(key_msg, sizeof(key_msg), "%sSymbolRate", path_msg);
		strncpy(s_arrstXmlParamsInfo[i].symbol_rate.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "6875");
		strncpy(s_arrstXmlParamsInfo[i].symbol_rate.value, value_msg, STRING_MAX_LEN);
		printf("SymbolRate[%d]: %s\n", i, value_msg);

		snprintf(key_msg, sizeof(key_msg), "%sModulation", path_msg);
		strncpy(s_arrstXmlParamsInfo[i].modulation.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "QAM64");
		strncpy(s_arrstXmlParamsInfo[i].modulation.value, value_msg, STRING_MAX_LEN);
		printf("Modulation[%d]: %s\n", i, value_msg);

		snprintf(key_msg, sizeof(key_msg), "%sTSOut/Addr", path_msg);
		strncpy(s_arrstXmlParamsInfo[i].ts_out_addr.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "227.10.20.9%d:1234", i);
		strncpy(s_arrstXmlParamsInfo[i].ts_out_addr.value, value_msg, STRING_MAX_LEN);
		printf("TSOut/Addr[%d]: %s\n", i, value_msg);

		snprintf(key_msg, sizeof(key_msg), "%sTSOut/Switch", path_msg);
		strncpy(s_arrstXmlParamsInfo[i].ts_out_switch.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "1");
		strncpy(s_arrstXmlParamsInfo[i].ts_out_switch.value, value_msg, STRING_MAX_LEN);
		printf("TSOut/Switch[%d]: %s\n", i, value_msg);

		snprintf(key_msg, sizeof(key_msg), "%sMonitor/Addr", path_msg);
		strncpy(s_arrstXmlParamsInfo[i].monitor_addr.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "227.10.20.8%d:1234", i);
		strncpy(s_arrstXmlParamsInfo[i].monitor_addr.value, value_msg, STRING_MAX_LEN);
		printf("Monitor/Addr[%d]: %s\n", i, value_msg);

		snprintf(key_msg, sizeof(key_msg), "%sMonitor/Switch", path_msg);
		strncpy(s_arrstXmlParamsInfo[i].monitor_switch.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "0");
		strncpy(s_arrstXmlParamsInfo[i].monitor_switch.value, value_msg, STRING_MAX_LEN);
		printf("Monitor/Switch[%d]: %s\n", i, value_msg);
	}

	//初始化数值结构体
	WebXml_XmlParams2StructParams();
	
	return WV_SUCCESS;
}


XmlParamsInfoPtr WebXml_GetXmlParamsInfoPtr(void)
{
	return (XmlParamsInfoPtr)(s_arrstXmlParamsInfo);
}


/*
* function: WebXml_InitProgramDecrypt
*
* description: 
*
* input:  @
*
* output: @
*
* return@ 
* success: 
*    fail: 
*
* author: linsheng.pan
*/
wvErrCode WebXml_InitProgramDecrypt(void)
{
	
	printf("WebXml_InitProgramDecrypt ...\n");

	char path_msg[STRING_MAX_LEN] = {0};
	char key_msg[STRING_MAX_LEN] = {0};
	char value_msg[STRING_MAX_LEN] = {0};
	int i = 0;
	int j = 0;

	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		snprintf(path_msg, sizeof(path_msg), "/Msg/Data[%d]/", i + 1);

		snprintf(key_msg, sizeof(key_msg), "%sChannel", path_msg);
		strncpy(s_arrstXmlProgramDecrypt[i].channel.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "%d", i);
		strncpy(s_arrstXmlProgramDecrypt[i].channel.value, value_msg, STRING_MAX_LEN);
		printf("Channel[%d]: %s\n", i, value_msg);

		for(j = 0; j < MAX_PROGRAM_NUMBER; ++j)
		{
			snprintf(key_msg, sizeof(key_msg), "%sServiceID[%d]", path_msg, j + 1);
			strncpy(s_arrstXmlProgramDecrypt[i].service_id[j].key, key_msg, STRING_MAX_LEN);

			snprintf(key_msg, sizeof(key_msg), "%sDecrypt[%d]", path_msg, j+1);
			strncpy(s_arrstXmlProgramDecrypt[i].decrypt[j].key, key_msg, STRING_MAX_LEN);
		}

		s_arrstXmlProgramDecrypt[i].u32ProgramNumber = 0;
		
		snprintf(value_msg, sizeof(value_msg), "60%d", i);
		strncpy(s_arrstXmlProgramDecrypt[i].service_id[0].value, value_msg, STRING_MAX_LEN);
		printf("ServiceID[%d]: %s\n", i, value_msg);

		
		snprintf(value_msg, sizeof(value_msg), "0");
		strncpy(s_arrstXmlProgramDecrypt[i].decrypt[0].value, value_msg, STRING_MAX_LEN);
		printf("Decrypt[%d]: %s\n", i, value_msg);
	}

	//初始化数值结构体
	WebXml_XmlProgram2StructProgram();
	
	return WV_SUCCESS;
}


XmlParamsInfoPtr WebXml_GetXmlProgramDecrypt(void)
{
	return (XmlParamsInfoPtr)(s_arrstXmlProgramDecrypt);
}


int WebXml_FindSupportCmd(CmdMapFuncPtr cmd_map_ptr, int support_cmd_num, char *cmd)
{
	if((!cmd_map_ptr) || (!cmd))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
		return WV_ERR_FAILURE;
	}
	
	int index = -1;
	U32 i = 0;

	for(i = 0; i < support_cmd_num; ++i)
	{
		if(!strcmp(cmd_map_ptr[i].recv_cmd_type , cmd))
		{
			index = i;
			break;
		}
	}
	
	//不支持
	if(i == support_cmd_num)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: unsport cmd:%s", cmd);
		index = -1;
	}

	return index;
}


wvErrCode WebXml_ExcuseCmdType(char *cmd_type, xmlDocPtr xml_doc_ptr, char *status, char *comment)
{
	if(!cmd_type)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
		return WV_ERR_PARAMS;
	}

	wvErrCode ret = WV_SUCCESS;
	int index = 0;
	
	index = WebXml_FindSupportCmd(cmd_map_func, sizeof(cmd_map_func)/sizeof(CmdMapFunc), cmd_type);

	if(-1 != index)
	{
		ret = cmd_map_func[index].call_func(xml_doc_ptr, status, comment);
	}
	else
	{
		ret = WV_ERR_FAILURE;
	}

	return ret;
}


wvErrCode WebXml_ExcuseReplyCmdType(char *cmd_type, xmlDocPtr *xml_doc_ptr, char *status, char *comment)
{
	if(!cmd_type)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
		return WV_ERR_PARAMS;
	}

	wvErrCode ret = WV_SUCCESS;
	int index = 0;
	
	index = WebXml_FindSupportCmd(reply_cmd_map_func, sizeof(reply_cmd_map_func)/sizeof(CmdMapFunc), cmd_type);

	if(-1 != index)
	{
		ret = reply_cmd_map_func[index].call_func(xml_doc_ptr, status, comment);
	}
	else
	{
		ret = WV_ERR_FAILURE;
	}

	return ret;
}


wvErrCode WebXml_GetCmdType(xmlDocPtr xml_doc_ptr, char *cmd_type, int cmd_type_len)
{
	if((!xml_doc_ptr) || (!cmd_type))
	{
		LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_XML, "Error: param = NULL");
		return WV_ERR_FAILURE;
	}

	wvErrCode ret = WV_ERR_FAILURE;
	int i = 0;
	xmlNodeSetPtr nodeset_ptr = NULL;
	xmlXPathObjectPtr result_ptr = NULL;
	xmlChar *value = NULL;

	xmlNodePtr xml_node_ptr = xmlDocGetRootElement(xml_doc_ptr);

	//获取cmd type
	result_ptr = XML_GetNodeSet(xml_doc_ptr, (xmlChar *)("//Type"));
	if(result_ptr)
	{
		nodeset_ptr = result_ptr->nodesetval;
		for(i = 0; i < nodeset_ptr->nodeNr; ++i)
		{
			value = xmlNodeListGetString(xml_doc_ptr, nodeset_ptr->nodeTab[i]->xmlChildrenNode, 1);
			strncpy(cmd_type, value, cmd_type_len);
			xmlFree(value);
		}
		xmlXPathFreeObject(result_ptr);
		ret = WV_SUCCESS;
	}

	return ret;
}


wvErrCode WebXml_ParseFile(char *filename, buffer* pstWriteBuff)
{
	if(!filename)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_PARAMS;
	}

	char cmd_type[STRING_MAX_LEN] = {0};
	char cmd_type_reply[STRING_MAX_LEN] = {0};
	char status[STRING_MAX_LEN] = {0};
	char comment[STRING_MAX_LEN] = {0};
	wvErrCode ret = WV_SUCCESS;

	xmlDocPtr reply_xml_doc_ptr = NULL;
	xmlChar   *xmlbuff;
	int buffersize;

	//获取xml的xml_doc_ptr
	xmlDocPtr xml_doc_ptr = XML_OpenXmlFile(filename, NULL, cmd_type, sizeof(cmd_type));
	if(!xml_doc_ptr)
	{
		//返回数据格式错误
		return WV_ERR_FAILURE;
	}

	WebXml_GetCmdType(xml_doc_ptr, cmd_type, sizeof(cmd_type));

	//是否支持 cmd type
	if(-1 != WebXml_FindSupportCmd(cmd_map_func, sizeof(cmd_map_func)/sizeof(CmdMapFunc), cmd_type))
	{
		//根据cmd type 执行对应解析xml数据的函数
		WebXml_ExcuseCmdType(cmd_type, xml_doc_ptr, status, comment);

		//构造reply cmd type
		snprintf(cmd_type_reply, sizeof(cmd_type_reply), "%sR", cmd_type);
		WebXml_ExcuseReplyCmdType(cmd_type_reply, &reply_xml_doc_ptr, status, comment);
		xmlDocDumpFormatMemoryEnc(reply_xml_doc_ptr, &xmlbuff, &buffersize,"UTF-8", 1);
		
		buffer_copy_string(pstWriteBuff,xmlbuff);
		printf("xmlbuff:\n%s\n", xmlbuff);
		xmlFree(xmlbuff);
		
		XML_FreeDoc(xml_doc_ptr);
		XML_FreeDoc(reply_xml_doc_ptr);
		ret = WV_SUCCESS;
	}
	else
	{
		//返回不支持的cmd type
		XML_FreeDoc(xml_doc_ptr);
		ret = WV_ERR_FAILURE;
	}
	
	//应答
	if(WV_SUCCESS == ret)
	{
		//通知相应的线程执行
	}

	return WV_SUCCESS;
}



wvErrCode WebXml_ParseMem(char *str, int str_len, buffer* pstWriteBuff)
{
	if(!str)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_PARAMS;
	}

	char cmd_type[STRING_MAX_LEN] = {0};
	char reply_cmd_type[STRING_MAX_LEN] = {0};
	char status[STRING_MAX_LEN] = {0};
	char comment[STRING_MAX_LEN] = {0};
	wvErrCode ret = WV_SUCCESS;
	char gb2312_buf[2048] = {0};

	xmlDocPtr reply_xml_doc_ptr = NULL;
	xmlChar   *xmlbuff;
	int buffersize;

	//获取xml的xml_doc_ptr
	xmlDocPtr xml_doc_ptr = XML_OpenMem(str, str_len, NULL, cmd_type, sizeof(cmd_type));
	if(!xml_doc_ptr)
	{
		//返回数据格式错误
		printf("Error: call xmlParseMemory\n");
		return WV_ERR_FAILURE;
	}

	WebXml_GetCmdType(xml_doc_ptr, cmd_type, sizeof(cmd_type));

	//是否支持 cmd type
	if(-1 != WebXml_FindSupportCmd(cmd_map_func, sizeof(cmd_map_func)/sizeof(CmdMapFunc), cmd_type))
	{
		//根据cmd type 执行对应解析xml数据的函数
		WebXml_ExcuseCmdType(cmd_type, xml_doc_ptr, status, comment);

		//构造reply cmd type
		snprintf(reply_cmd_type, sizeof(reply_cmd_type), "%sR", cmd_type);
		WebXml_ExcuseReplyCmdType(reply_cmd_type, &reply_xml_doc_ptr, status, comment);
		xmlDocDumpFormatMemoryEnc(reply_xml_doc_ptr, &xmlbuff, &buffersize,"UTF-8", 1);

		//构造Response内容
		buffer_copy_string(pstWriteBuff,xmlbuff);
		printf("xmlbuff:\n%s\n", xmlbuff);
		xmlFree(xmlbuff);
		
		XML_FreeDoc(xml_doc_ptr);
		XML_FreeDoc(reply_xml_doc_ptr);
		ret = WV_SUCCESS;
	}
	else
	{
		//返回不支持的cmd type
		XML_FreeDoc(xml_doc_ptr);
		ret = WV_ERR_FAILURE;
	}
	
	//应答
	if(WV_SUCCESS == ret)
	{
		//通知相应的线程执行
	}

	return WV_SUCCESS;
	
}


wvErrCode WebXml_GetDeviceInfo(xmlDocPtr xml_doc_ptr, char *status, char *comment)
{
	if(!xml_doc_ptr)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: Params = NULL");
		return WV_ERR_PARAMS;
	}

	LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_XML, "WEB--WebXml_GetDeviceInfo...");

	strcpy(status, STATUS_SUCCCESS);
	strcpy(comment, "\0");

	return WV_SUCCESS;
}


wvErrCode WebXml_Update(xmlDocPtr xml_doc_ptr, char *status, char *comment)
{
	if(!xml_doc_ptr)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: Params = NULL");
		return WV_ERR_PARAMS;
	}

	LOG_PRINTF(LOG_LEVEL_ALARM, LOG_MODULE_XML, "WEB--Update.....");

	//TODO
	//执行相应的指令

	strcpy(status, STATUS_SUCCCESS);
	strcpy(comment, "\0");
	
	return WV_SUCCESS;
}


static void * WebXml_SystemRestart(void *arg)
{
	sleep(3);
	system("reboot");

	return NULL;
}

wvErrCode WebXml_Restart(xmlDocPtr xml_doc_ptr, char *status, char *comment)
{
	if(!xml_doc_ptr)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: Params = NULL");
		return WV_ERR_PARAMS;
	}

	LOG_PRINTF(LOG_LEVEL_ALARM, LOG_MODULE_XML, "WEB--Restart.....");

	//TODO
	//执行相应的指令
	//2.重启后回复执行结果。
	//3.板卡重启必须在1分钟之内完成。

	THREAD_NEW_DETACH(WebXml_SystemRestart, NULL, "WebXml_SystemRestart");

	strcpy(status, STATUS_SUCCCESS);
	strcpy(comment, "\0");
	
	return WV_SUCCESS;
}



wvErrCode WebXml_Factory(xmlDocPtr xml_doc_ptr, char *status, char *comment)
{
	if(!xml_doc_ptr)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: Params = NULL");
		return WV_ERR_PARAMS;
	}

	LOG_PRINTF(LOG_LEVEL_ALARM, LOG_MODULE_XML, "WEB--Factory.....");

	//TODO
	//执行相应打指令
	PARAM_Default();
	WebXml_InitAll();

	strcpy(status, STATUS_SUCCCESS);
	strcpy(comment, "\0");
	
	return WV_SUCCESS;
}


wvErrCode WebXml_SaveAllCfg(xmlDocPtr xml_doc_ptr, char *status, char *comment)
{
	if(!xml_doc_ptr)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: Params = NULL");
		return WV_ERR_PARAMS;
	}

	LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_XML, "WEB--SaveAllCfg.....");

	//TODO
	//执行相应的指令

	strcpy(status, STATUS_SUCCCESS);
	strcpy(comment, "\0");
	
	return WV_SUCCESS;
}


wvErrCode WebXml_SetDemodulateInfo(xmlDocPtr xml_doc_ptr, char *status, char *comment)
{
	if(!xml_doc_ptr)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: Params = NULL");
		return WV_ERR_PARAMS;
	}

	LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_XML, "WEB--SetDemodulateInfo.....");

	int i = 0;
	XmlKeyValuePtr key_value_ptr = (XmlKeyValuePtr)s_arrstXmlParamsInfo;
	int item_num = sizeof(s_arrstXmlParamsInfo)/sizeof(XmlKeyValue);
	printf("item = %d\n", item_num);

	//从XML提取信息到 s_arrstXmlParamsInfo
	for(i = 0; i < item_num; ++i)
	{
		if(WV_SUCCESS != XML_XpathGetValue(xml_doc_ptr, (xmlChar *)(key_value_ptr->key), key_value_ptr->value, STRING_MAX_LEN))
		{
			LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: %s can't find", key_value_ptr->key);
			return WV_ERR_FAILURE;
		}
		printf("value: %s\n", key_value_ptr->value);
		key_value_ptr++;
	}

	//将xml结构体[s_arrstXmlParamsInfo], 转换为数值结构体
	WebXml_XmlParams2StructParams();

	//将tuner参数提供出来设置到tuner参数
	WebXml_SetParamsInfoToTunerParams();

	//设置IP 输出
	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		LUT_SetInputLUTIPInfo(i, s_arrstParamsInfo[i].u32TsIpAddr, s_arrstParamsInfo[i].u16TsPort);
		if(s_arrstParamsInfo[i].u32TsSwitch)
		{
			LUT_OpenStream(i, TSP_InputChannel2StreamID(i));
		}
		else
		{
			LUT_ClearInputLUT(i);
			LUT_CloseStream(i, TSP_InputChannel2StreamID(i));
		}
	}	
	
	strcpy(status, STATUS_SUCCCESS);
	strcpy(comment, "\0");
	
	return WV_SUCCESS;
}


wvErrCode WebXml_GetDemodulateInfo(xmlDocPtr xml_doc_ptr, char *status, char *comment)
{
	if(!xml_doc_ptr)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: Params = NULL");
		return WV_ERR_PARAMS;
	}

	LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_XML, "WEB--GetDemodulateInfo....");

	strcpy(status, STATUS_SUCCCESS);
	strcpy(comment, "\0");
	
	return WV_SUCCESS;
}


wvErrCode WebXml_SetDecryptInfo(xmlDocPtr xml_doc_ptr, char *status, char *comment)
{
	if(!xml_doc_ptr)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: Params = NULL");
		return WV_ERR_PARAMS;
	}

	LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_XML, "WEB--SetDecryptInfo.....");

	U32 i = 0;
	U32 j = 0;
	U32 u32ProgramNumber[4] = {0};
	U32 u32DataIndex = 0;
	char key[STRING_MAX_LEN] = {0};
	char key_path[STRING_MAX_LEN] = {0};
	

	//从xml信息提取到 s_arrstXmlProgramDecrypt
	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		u32ProgramNumber[i] = s_arrstXmlProgramDecrypt[i].u32ProgramNumber;
		//一个service id 占一个data
		
		for(j = 0; j < u32ProgramNumber[i]; ++j)
		{
			printf("u32DataIndex = %u\n", u32DataIndex + j + 1);
			//构造key的路径
			snprintf(key_path, sizeof(key_path), "/Msg/Data[%d]", u32DataIndex + j + 1);

			snprintf(key, sizeof(key), "%s/Channel", key_path);
			if(WV_SUCCESS != XML_XpathGetValue(xml_doc_ptr, (xmlChar *)key, s_arrstXmlProgramDecrypt[i].channel.value, STRING_MAX_LEN))
			{
				LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: %s can't find", key);
				return WV_ERR_FAILURE;
			}

			snprintf(key, sizeof(key), "%s/ServiceID",key_path);
			if(WV_SUCCESS != XML_XpathGetValue(xml_doc_ptr, (xmlChar *)key, s_arrstXmlProgramDecrypt[i].service_id[j].value, STRING_MAX_LEN))
			{
				LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: %s can't find", key);
				return WV_ERR_FAILURE;
			}

			snprintf(key, sizeof(key), "%s/Decrypt",key_path);
			if(WV_SUCCESS != XML_XpathGetValue(xml_doc_ptr, (xmlChar *)key, s_arrstXmlProgramDecrypt[i].decrypt[j].value, STRING_MAX_LEN))
			{
				LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: %s can't find", key);
				return WV_ERR_FAILURE;
			}
		}

		u32DataIndex += u32ProgramNumber[i];
	}

	//将xml结构体转换到数值结构体
	WebXml_XmlProgram2StructProgram();

	//将设置解扰信息到TSINFO结构体
	TSInfo *pstTS = TSP_GetTSParamHandle();
	WebXml_SetProgramInfoToTSInfo(pstTS);

	//设置调用设置解扰的函数
	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		WVCI_UpdateSetting(i);
	}

	strcpy(status, STATUS_SUCCCESS);
	strcpy(comment, "\0");
	
	return WV_SUCCESS;
}

wvErrCode WebXml_GetDecryptInfo(xmlDocPtr xml_doc_ptr, char *status, char *comment)
{
	if(!xml_doc_ptr)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: Params = NULL");
		return WV_ERR_PARAMS;
	}

	LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_XML, "WEB--GetDecryptInfo....");

	strcpy(status, STATUS_SUCCCESS);
	strcpy(comment, "\0");
	
	return WV_SUCCESS;
}


wvErrCode WebXml_GetDeviceInfoR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment)
{
	printf("WebXml_GetDeviceInfoR...\n");
	if(!status)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
		return WV_ERR_FAILURE;
	}

	xmlNodePtr root_node = NULL;
	xmlNodePtr data_node = NULL;
	xmlNodePtr ip_info_node = NULL;
	xmlNodePtr ver_info_node = NULL;

	*xml_doc_ptr = XML_NewXmlDoc(&root_node, "Msg");

	if(!strcmp("Success", status))
	{
		//Type
		XML_AddNewChildNode(root_node, "Type", "GetDeviceInfoR");
		//Status
		XML_AddNewChildNode(root_node, "Status", status);
		//Data
		data_node = XML_AddNewChildNode(root_node, "Data", NULL);
			//OEM
			XML_AddNewChildNode(data_node, "OEM", s_stXmlDeviceInfo.oem);
			//CardType
			XML_AddNewChildNode(data_node, "CardType", s_stXmlDeviceInfo.card_type);
			//CardID
			XML_AddNewChildNode(data_node, "CardID", s_stXmlDeviceInfo.card_id);
			//IPInfo
			ip_info_node = XML_AddNewChildNode(data_node, "IPInfo", NULL);
				//NICName
				XML_AddNewChildNode(ip_info_node, "NICName", s_stXmlDeviceInfo.eth_name);
				//IP
				XML_AddNewChildNode(ip_info_node, "IP", s_stXmlDeviceInfo.ip_addr);
				//Netmask
				XML_AddNewChildNode(ip_info_node, "Netmask", s_stXmlDeviceInfo.netmask);
				//Gateway
				XML_AddNewChildNode(ip_info_node, "Gateway", s_stXmlDeviceInfo.gateway);
			//VerInfo
			ver_info_node = XML_AddNewChildNode(data_node, "VerInfo", NULL);
				//PcbVer
				XML_AddNewChildNode(ver_info_node, "PcbVer", s_stXmlDeviceInfo.pcb_ver);
				//FpgaVer
				XML_AddNewChildNode(ver_info_node, "FpgaVer", s_stXmlDeviceInfo.fpga_ver);
				//UbootVer
				XML_AddNewChildNode(ver_info_node, "UbootVer", s_stXmlDeviceInfo.uboot_ver);
				//KernelVer
				XML_AddNewChildNode(ver_info_node, "KernelVer", s_stXmlDeviceInfo.kernel_ver);
				//AppVer
				XML_AddNewChildNode(ver_info_node, "AppVer", s_stXmlDeviceInfo.app_ver);
			
	}
	else
	{
		//Type
		XML_AddNewChildNode(root_node, "Type", "GetDeviceInfoR");
		//Status
		XML_AddNewChildNode(root_node, "Status", status);
		//ErrorCode 1000
		XML_AddNewChildNode(root_node, "ErrorCode", "1000");
		//Comment
		XML_AddNewChildNode(root_node, "Comment", comment);
	}
	
	return WV_SUCCESS;
}

wvErrCode WebXml_UpdateR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment)
{
	if((!status) || (!comment))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_FAILURE;
	}

	xmlNodePtr root_node = NULL;
	xmlNodePtr data_node = NULL;

	*xml_doc_ptr = XML_NewXmlDoc(&root_node, "Msg");

	//Type
	XML_AddNewChildNode(root_node, "Type", "UpdateR");
	//Status
	XML_AddNewChildNode(root_node, "Status", status);

	if(strcmp(STATUS_SUCCCESS, status))
	{
		//ErrorCode
		XML_AddNewChildNode(root_node, "ErrorCode", XML_ERROR_CODE_UPDATE);
		//Comment
		XML_AddNewChildNode(root_node, "Comment", comment);
	}
	
	return WV_SUCCESS;
}


wvErrCode WebXml_RestartR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment)
{
	if((!status) || (!comment))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_FAILURE;
	}

	xmlNodePtr root_node = NULL;
	xmlNodePtr data_node = NULL;

	*xml_doc_ptr = XML_NewXmlDoc(&root_node, "Msg");

	//Type
	XML_AddNewChildNode(root_node, "Type", "RestartR");
	//Status
	XML_AddNewChildNode(root_node, "Status", status);

	if(strcmp(STATUS_SUCCCESS, status))
	{
		//ErrorCode
		XML_AddNewChildNode(root_node, "ErrorCode", XML_ERROR_CODE_RESTART);
		//Comment
		XML_AddNewChildNode(root_node, "Comment", comment);
	}
	
	return WV_SUCCESS;
}


wvErrCode WebXml_FactoryR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment)
{
	if((!status) || (!comment))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_FAILURE;
	}

	xmlNodePtr root_node = NULL;
	xmlNodePtr data_node = NULL;

	*xml_doc_ptr = XML_NewXmlDoc(&root_node, "Msg");

	//Type
	XML_AddNewChildNode(root_node, "Type", "FactoryR");
	//Status
	XML_AddNewChildNode(root_node, "Status", status);

	if(strcmp(STATUS_SUCCCESS, status))
	{
		//ErrorCode
		XML_AddNewChildNode(root_node, "ErrorCode", XML_ERROR_CODE_FACTORY);
		//Comment
		XML_AddNewChildNode(root_node, "Comment", comment);
	}
	
	return WV_SUCCESS;
}



wvErrCode WebXml_SaveAllCfgR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment)
{
	if((!status) || (!comment))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_FAILURE;
	}

	xmlNodePtr root_node = NULL;
	xmlNodePtr data_node = NULL;

	*xml_doc_ptr = XML_NewXmlDoc(&root_node, "Msg");

	//Type
	XML_AddNewChildNode(root_node, "Type", "SaveAllCfgR");
	//Status
	XML_AddNewChildNode(root_node, "Status", status);

	if(strcmp(STATUS_SUCCCESS, status))
	{
		//ErrorCode
		XML_AddNewChildNode(root_node, "ErrorCode", XML_ERROR_CODE_SAVE_CFG);
		//Comment
		XML_AddNewChildNode(root_node, "Comment", comment);
	}
	
	return WV_SUCCESS;
}

wvErrCode WebXml_SetDemodulateInfoR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment)
{
	if((!status) || (!comment))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_FAILURE;
	}

	xmlNodePtr root_node = NULL;
	xmlNodePtr data_node = NULL;

	*xml_doc_ptr = XML_NewXmlDoc(&root_node, "Msg");

	//Type
	XML_AddNewChildNode(root_node, "Type", "SetDemodulateInfoR");
	//Status
	XML_AddNewChildNode(root_node, "Status", status);

	if(strcmp(STATUS_SUCCCESS, status))
	{
		//ErrorCode
		XML_AddNewChildNode(root_node, "ErrorCode", XML_ERROR_CODE_SET_DEMODULATE);
		//Comment
		XML_AddNewChildNode(root_node, "Comment", comment);
	}

	return WV_SUCCESS;
}



wvErrCode WebXml_GetDemodulateInfoR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment)
{
	if((!status) || (!comment))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_FAILURE;
	}

	xmlNodePtr root_node = NULL;
	xmlNodePtr data_node = NULL;
	xmlNodePtr out_node = NULL;
	xmlNodePtr monitor_node = NULL;
	int i = 0;

	//从tuner 结构体提取数据
	WebXml_GetParamsInfoFromTunerParams();

	
	//从数值结构体转化为xml结构体
	WebXml_StructParams2XmlParams();
	
	//xml结构体到xml文件
	*xml_doc_ptr = XML_NewXmlDoc(&root_node, "Msg");

	//Type
	XML_AddNewChildNode(root_node, "Type", "GetDemodulateInfoR");
	//Status
	XML_AddNewChildNode(root_node, "Status", status);

	if(strcmp(STATUS_SUCCCESS, status))
	{
		//ErrorCode
		XML_AddNewChildNode(root_node, "ErrorCode", XML_ERROR_CODE_GET_DEMODULATE);
		//Comment
		XML_AddNewChildNode(root_node, "Comment", comment);
	}
	else
	{
		for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
		{
			//Data
			data_node = XML_AddNewChildNode(root_node, "Data", NULL);
				//Channel
				XML_AddNewChildNode(data_node, "Channel", s_arrstXmlParamsInfo[i].channel.value);
				//Frequency
				XML_AddNewChildNode(data_node, "Frequency", s_arrstXmlParamsInfo[i].frequency.value);
				printf("##############s_arrstXmlParamsInfo[i].channel.value = %s\n", s_arrstXmlParamsInfo[i].channel.value);
				//SymbolRate
				XML_AddNewChildNode(data_node, "SymbolRate", s_arrstXmlParamsInfo[i].symbol_rate.value);
				//Modulation
				XML_AddNewChildNode(data_node, "Modulation", s_arrstXmlParamsInfo[i].modulation.value);
				//Out
				out_node = XML_AddNewChildNode(data_node, "TSOut", NULL);
					//Addr
					XML_AddNewChildNode(out_node, "Addr", s_arrstXmlParamsInfo[i].ts_out_addr.value);
					//Switch
					XML_AddNewChildNode(out_node, "Switch", s_arrstXmlParamsInfo[i].ts_out_switch.value);
				//Monitor
				monitor_node = XML_AddNewChildNode(data_node, "Monitor", NULL);
					//Addr
					XML_AddNewChildNode(monitor_node, "Addr", s_arrstXmlParamsInfo[i].monitor_addr.value);
					//Switch
					XML_AddNewChildNode(monitor_node, "Switch", s_arrstXmlParamsInfo[i].monitor_switch.value);
		}

	}

	
	return WV_SUCCESS;
}

wvErrCode WebXml_SetDecryptInfoR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment)
{
	
	if((!status) || (!comment))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_FAILURE;
	}

	xmlNodePtr root_node = NULL;
	xmlNodePtr data_node = NULL;

	*xml_doc_ptr = XML_NewXmlDoc(&root_node, "Msg");

	//Type
	XML_AddNewChildNode(root_node, "Type", "SetDecryptInfoR");
	//Status
	XML_AddNewChildNode(root_node, "Status", status);

	if(strcmp(STATUS_SUCCCESS, status))
	{
		//ErrorCode
		XML_AddNewChildNode(root_node, "ErrorCode", XML_ERROR_CODE_SET_DECRYPT);
		//Comment
		XML_AddNewChildNode(root_node, "Comment", comment);
	}
	
	return WV_SUCCESS;
}


wvErrCode WebXml_GetDecryptInfoR(xmlDocPtr *xml_doc_ptr, const char *status, const char *comment)
{
	if((!status) || (!comment))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_FAILURE;
	}

	U32 i = 0;
	U32 j = 0;
	xmlNodePtr root_node = NULL;
	xmlNodePtr data_node = NULL;
	U32 u32ProgramNumber = 0;

	//从TSINFO获取解扰信息
	TSInfo *pstTS = TSP_GetTSParamHandle();
	WebXml_GetProgramInfoFromTSInfo(pstTS);

	//将数值结构体转换为xml结构体
	WebXml_StructProgram2XmlProgram();

	//xml结构体转换为xml文件
	*xml_doc_ptr = XML_NewXmlDoc(&root_node, "Msg");

	//Type
	XML_AddNewChildNode(root_node, "Type", "GetDecryptInfoR");
	//XML_AddNewChildNode(root_node, "Type", "获取解调信息");
	//Status
	XML_AddNewChildNode(root_node, "Status", status);

	if(strcmp(STATUS_SUCCCESS, status))
	{
		//ErrorCode
		XML_AddNewChildNode(root_node, "ErrorCode", XML_ERROR_CODE_GET_DECRYPT);
		//Comment
		XML_AddNewChildNode(root_node, "Comment", comment);
	}
	else
	{
		for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
		{
			
			u32ProgramNumber = s_arrstXmlProgramDecrypt[i].u32ProgramNumber;
			for(j = 0; j < u32ProgramNumber; ++j)
			{
				//Data
				data_node = XML_AddNewChildNode(root_node, "Data", NULL);
					//Channel
					XML_AddNewChildNode(data_node, "Channel", s_arrstXmlProgramDecrypt[i].channel.value);
					//ServiceID
					XML_AddNewChildNode(data_node, "ServiceID", s_arrstXmlProgramDecrypt[i].service_id[j].value);
					//Decrypt
					XML_AddNewChildNode(data_node, "Decrypt", s_arrstXmlProgramDecrypt[i].decrypt[j].value);
			}
		}
	}

	
	return WV_SUCCESS;
}



wvErrCode WebXml_GetProgramInfoFromTSInfo(TSInfo * pstTS)
{
	if(!pstTS)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_FAILURE;
	}
	
	U32 i = 0;
	U32 j = 0;
	IndexInfoList  stIndexInfoList[300];
	U32 u32ChannelIndex = 0;
	U32 u32TSIndex = 0;
	U32 u32ProgramIndex = 0;

	
	//清除上一次的信息
	memset(&s_arrstProgramDecrypt, 0, sizeof(s_arrstProgramDecrypt));

	//获取到对应channel的流的所有节目index
	for(u32ChannelIndex =0; u32ChannelIndex <  pstTS->u32InputTSNumber; ++u32ChannelIndex)
	{
		stIndexInfoList[u32ChannelIndex].u16IndexListNum = 0;
		/*
		printf("######### u32InputTSNumber = %u\n", pstTS->u32InputTSNumber);
		printf("########## u8ValidFlag= %u\n", pstTS->pInputTS[u32ChannelIndex].u8ValidFlag);
		printf("u16ChannelID = %u\n", pstTS->pInputTS[u32ChannelIndex].u16ChannelID);
		TSP_GetInTSProgIndexList(u32ChannelIndex, pstTS, &stIndexInfoList[u32ChannelIndex]);
		for(i = 0; i < stIndexInfoList[u32ChannelIndex].u16IndexListNum; ++i)
		{
			printf("[Service ID] = %u\n",  pstTS->pInputProgram[stIndexInfoList[u32ChannelIndex].aru16IndexList[i]].u16ServiceID);
			printf("[CAM Index] = %u\n", pstTS->pInputProgram[stIndexInfoList[u32ChannelIndex].aru16IndexList[i]].u8CAMIndex);
			printf("[Service Name] = %s\n", pstTS->pInputProgram[stIndexInfoList[u32ChannelIndex].aru16IndexList[i]].aru8ServiceName);
		}
		*/	
		for(u32TSIndex = 0; u32TSIndex < pstTS->u32InputTSNumber; ++u32TSIndex)
		{
			if((u32ChannelIndex == pstTS->pInputTS[u32TSIndex].u16ChannelID) && (pstTS->pInputTS[u32TSIndex].u8ValidFlag))
			{
				TSP_GetInTSProgIndexList(u32TSIndex, pstTS, &stIndexInfoList[u32ChannelIndex]);
				break;
			}
		}
		
	}

	//获取channel的所有service id 、节目名字 、 CAM Index、节目数量
	for(u32ChannelIndex = 0; u32ChannelIndex < WVCI_MAX_SLOT_NUM; ++u32ChannelIndex)
	{
		s_arrstProgramDecrypt[u32ChannelIndex].u16ChannelID = u32ChannelIndex;
		s_arrstProgramDecrypt[u32ChannelIndex].u32ProgramNumber = stIndexInfoList[u32ChannelIndex].u16IndexListNum;
		for(i = 0; i < stIndexInfoList[u32ChannelIndex].u16IndexListNum; ++i)
		{
			u32ProgramIndex = stIndexInfoList[u32ChannelIndex].aru16IndexList[i];
			s_arrstProgramDecrypt[u32ChannelIndex].u16ServiceID[i] = pstTS->pInputProgram[u32ProgramIndex].u16ServiceID;
			s_arrstProgramDecrypt[u32ChannelIndex].u8CAMIndex[i] = pstTS->pInputProgram[u32ProgramIndex].u8CAMIndex;
			strncpy(s_arrstProgramDecrypt[u32ChannelIndex].u8ServiceName[i],pstTS->pInputProgram[u32ProgramIndex].aru8ServiceName, 
				sizeof(s_arrstProgramDecrypt[u32ChannelIndex].u8ServiceName[i]));
		}
	}

	//打印获取到的信息
	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		printf("Channel Id: %d\n", i);
		for(j = 0; j < s_arrstProgramDecrypt[i].u32ProgramNumber; ++j)
		{
			printf("[ServiceID]: %u\n", s_arrstProgramDecrypt[i].u16ServiceID[j]);
			printf("[CAMIndex]: %u\n", s_arrstProgramDecrypt[i].u8CAMIndex[j]);
			printf("[Service Name]: %s\n", s_arrstProgramDecrypt[i].u8ServiceName[j]);
		}
	}

	return WV_SUCCESS;
	
}


wvErrCode WebXml_SetProgramInfoToTSInfo(TSInfo * pstTS)
{
	if(!pstTS)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_FAILURE;
	}

	U32 i = 0;
	U32 j = 0;
	IndexInfoList  stIndexInfoList[WVCI_MAX_SLOT_NUM];
	U32 u32ChannelIndex = 0;
	U32 u32TSIndex = 0;
	U32 u32ProgramIndex = 0;
	
	TSP_MutexLock();

	//获取到对应channel的流的所有节目index
	for(u32ChannelIndex =0; u32ChannelIndex < WVCI_MAX_SLOT_NUM; ++u32ChannelIndex)
	{
		stIndexInfoList[u32ChannelIndex].u16IndexListNum = 0;
		for(u32TSIndex = 0; u32TSIndex < WVCI_MAX_SLOT_NUM; ++u32TSIndex)
		{
			if(u32ChannelIndex == pstTS->pInputTS[u32TSIndex].u16ChannelID)
			{
				TSP_GetInTSProgIndexList(u32TSIndex, pstTS, &stIndexInfoList[u32ChannelIndex]);
				break;
			}
		}
	}

	//设置解扰
	for(u32ChannelIndex = 0; u32ChannelIndex < WVCI_MAX_SLOT_NUM; ++u32ChannelIndex)
	{		
		for(i = 0; i < s_arrstProgramDecrypt[u32ChannelIndex].u32ProgramNumber; ++i)
		{
			//节目列表index
			u32ProgramIndex = stIndexInfoList[u32ChannelIndex].aru16IndexList[i];
			for(j = 0; j < s_arrstProgramDecrypt[u32ChannelIndex].u32ProgramNumber; ++j)
			{
				//service id 一致，对应节目设置解扰
				if(s_arrstProgramDecrypt[u32ChannelIndex].u16ServiceID[j] == pstTS->pInputProgram[u32ProgramIndex].u16ServiceID)
				{
					pstTS->pInputProgram[u32ProgramIndex].u8CAMIndex = s_arrstProgramDecrypt[u32ChannelIndex].u8CAMIndex[j];
				}
			}
		}
	}
	
	TSP_MutexUnlock();

	return WV_SUCCESS;
}



wvErrCode WebXml_GetParamsInfoFromTunerParams(void)
{
	int i = 0;
	int tuner_map[WVCI_MAX_SLOT_NUM] = {3,2,1,0};
	Tuner_C_Params_t stTunerCParams;
	
	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		Tuner_c_GetLockParam(tuner_map[i], &stTunerCParams);
		s_arrstParamsInfo[i].u16Channel = i;
		s_arrstParamsInfo[i].u32Frequency = stTunerCParams.u32FrequencyKHz;
		s_arrstParamsInfo[i].u32Modulation = stTunerCParams.u32QamMode;
		s_arrstParamsInfo[i].u32SymbolRate = stTunerCParams.u32SymbolRate;
	}

	return WV_SUCCESS;
}


wvErrCode WebXml_SetParamsInfoToTunerParams(void)
{
	int i = 0;
	int tuner_map[WVCI_MAX_SLOT_NUM] = {3,2,1,0};
	Tuner_C_Params_t stTunerCParams;
	
	for(i = 0; i < WVCI_MAX_SLOT_NUM; ++i)
	{
		stTunerCParams.u32FrequencyKHz = s_arrstParamsInfo[i].u32Frequency;
		stTunerCParams.u32QamMode = s_arrstParamsInfo[i].u32Modulation;
		stTunerCParams.u32SymbolRate = s_arrstParamsInfo[i].u32SymbolRate;
		printf("################ stTunerCParams.u32FrequencyKHz = %u\n", stTunerCParams.u32FrequencyKHz);

		Tuner_c_SetLockParam(tuner_map[i], &stTunerCParams);
	}

	return WV_SUCCESS;
}



ParamsInfoPtr  WebXml_GetParamsInfoPtr(U8 u8Index)
{
	if(u8Index >= WVCI_MAX_SLOT_NUM)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = %u\n", u8Index);
		return NULL;
	}

	return (ParamsInfoPtr)(&s_arrstParamsInfo[u8Index]);
}


ProgramDecryptPtr  WebXml_GetProgramDecryptPtr(U8 u8Index)
{
	if(u8Index >= WVCI_MAX_SLOT_NUM)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = %u\n", u8Index);
		return NULL;
	}

	return (ProgramDecryptPtr)(&s_arrstProgramDecrypt[u8Index]);
}


wvErrCode WebXml_InitAll(void)
{
	WebXml_InitDeviceInfo();
	WebXml_InitParamsInfo();
	WebXml_InitProgramDecrypt();

	return WV_SUCCESS;
}





