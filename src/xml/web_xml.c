#include "web_xml.h"
#include "wv_xml.h"

#include "err/wv_err.h"
#include "log/wv_log.h"

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
static XmlParamsInfo s_arrstXmlParamsInfo[CHANNEL_MAX];
static XmlProgramDecrypt s_arrstXmlProgramDecrypt[CHANNEL_MAX];

static ParamsInfo s_arrstParamsInfo[CHANNEL_MAX];
static ProgramDecrypt s_arrstProgramDecrypt[CHANNEL_MAX];

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


wvErrCode WebXml_InitDeviceInfo(void)
{
	printf("WebXml_InitDeviceInfo ...\n");
	
	strncpy(s_stXmlDeviceInfo.oem, "Bluetop", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.card_type, "0", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.card_id, "100", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.eth_name, "eth1", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.ip_addr, "192.168.10.66", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.netmask, "255.255.0.0", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.gateway, "192.168.10.1", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.pcb_ver, "V1.0.0.0", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.fpga_ver, "V2.1.0.5", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.uboot_ver, "V1.3.5.0", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.kernel_ver, "V9", STRING_MAX_LEN);
	strncpy(s_stXmlDeviceInfo.app_ver, "V2.0.3", STRING_MAX_LEN);

	return WV_SUCCESS;
	
}

XmlDeviceInfoPtr WebXml_GetDeviceInfoPtr(void)
{
	return (XmlDeviceInfoPtr)(&s_stXmlDeviceInfo);
}


wvErrCode WebXml_InitParamsInfo(void)
{
	printf("WebXml_InitParamsInfo ...\n");
	char path_msg[STRING_MAX_LEN] = {0};
	char key_msg[STRING_MAX_LEN] = {0};
	char value_msg[STRING_MAX_LEN] = {0};
	int i = 0;
	
	for(i = 0; i < CHANNEL_MAX; ++i)
	{
		snprintf(path_msg, sizeof(path_msg), "/Msg/Data[%d]/", i + 1);

		snprintf(key_msg, sizeof(key_msg), "%sChannel", path_msg);
		strncpy(s_arrstXmlParamsInfo[i].channel.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "%d", i);
		strncpy(s_arrstXmlParamsInfo[i].channel.value, value_msg, STRING_MAX_LEN);
		printf("Channel[%d]: %s\n", i, value_msg);

		snprintf(key_msg, sizeof(key_msg), "%sFrequency", path_msg);
		strncpy(s_arrstXmlParamsInfo[i].frequency.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "714000");
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
		snprintf(value_msg, sizeof(value_msg), "235.0.1.11:2234");
		strncpy(s_arrstXmlParamsInfo[i].ts_out_addr.value, value_msg, STRING_MAX_LEN);
		printf("TSOut/Addr[%d]: %s\n", i, value_msg);

		snprintf(key_msg, sizeof(key_msg), "%sTSOut/Switch", path_msg);
		strncpy(s_arrstXmlParamsInfo[i].ts_out_switch.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "0");
		strncpy(s_arrstXmlParamsInfo[i].ts_out_switch.value, value_msg, STRING_MAX_LEN);
		printf("TSOut/Switch[%d]: %s\n", i, value_msg);

		snprintf(key_msg, sizeof(key_msg), "%sMonitor/Addr", path_msg);
		strncpy(s_arrstXmlParamsInfo[i].monitor_addr.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "235.0.1.11:2234");
		strncpy(s_arrstXmlParamsInfo[i].monitor_addr.value, value_msg, STRING_MAX_LEN);
		printf("Monitor/Addr[%d]: %s\n", i, value_msg);

		snprintf(key_msg, sizeof(key_msg), "%sMonitor/Switch", path_msg);
		strncpy(s_arrstXmlParamsInfo[i].monitor_switch.key, key_msg, STRING_MAX_LEN);
		snprintf(value_msg, sizeof(value_msg), "0");
		strncpy(s_arrstXmlParamsInfo[i].monitor_switch.value, value_msg, STRING_MAX_LEN);
		printf("Monitor/Switch[%d]: %s\n", i, value_msg);
	}


	return WV_SUCCESS;
}


XmlParamsInfoPtr WebXml_GetParamsInfoPtr(void)
{
	return (XmlParamsInfoPtr)(s_arrstXmlParamsInfo);
}


wvErrCode WebXml_InitProgramDecrypt(void)
{
	
	printf("WebXml_InitProgramDecrypt ...\n");

	char path_msg[STRING_MAX_LEN] = {0};
	char key_msg[STRING_MAX_LEN] = {0};
	char value_msg[STRING_MAX_LEN] = {0};
	int i = 0;
	int j = 0;

	//TODO
	for(i = 0; i < CHANNEL_MAX; ++i)
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

		s_arrstXmlProgramDecrypt[i].u32ProgramNumber = 1;
		
		snprintf(value_msg, sizeof(value_msg), "60%d", i);
		strncpy(s_arrstXmlProgramDecrypt[i].service_id[0].value, value_msg, STRING_MAX_LEN);
		printf("ServiceID[%d]: %s\n", i, value_msg);

		
		snprintf(value_msg, sizeof(value_msg), "0");
		strncpy(s_arrstXmlProgramDecrypt[i].decrypt[0].value, value_msg, STRING_MAX_LEN);
		printf("Decrypt[%d]: %s\n", i, value_msg);
	}
	
	
	return WV_SUCCESS;
}


XmlParamsInfoPtr WebXml_GetProgramDecrypt(void)
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
	

	for(i = 0; i < CHANNEL_MAX; ++i)
	{
		u32ProgramNumber[i] = s_arrstXmlProgramDecrypt[i].u32ProgramNumber;
		//一个service id 占一个data
		u32DataIndex += u32ProgramNumber[i];
		for(j = 0; j < u32ProgramNumber[i]; ++j)
		{
			//构造key的路径
			snprintf(key_path, sizeof(key_path), "/Msg/Data[%d]", u32DataIndex + 1);

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
		for(i = 0; i < CHANNEL_MAX; ++i)
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
		for(i = 0; i < CHANNEL_MAX; ++i)
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



wvErrCode WelXml_GetProgramInfoFromTSInfo(TSInfo * pstTS)
{
	if(!pstTS)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return WV_ERR_FAILURE;
	}

	U32 u32InputProgramNumber = pstTS->u32InputProgramNumber;
	U32 u32ProgramIndex = 0;
	U16 u16ChannelID = 0;
	U16 u16ServiceID = 0;
	U8  u8CAMIndex = 0;
	U8  u8ValidFlag = TSP_FLAG_INVALID;
	U32 i = 0;
	U32 j = 0;

	//清除上一下的信息
	memset(&s_arrstProgramDecrypt, 0, sizeof(s_arrstProgramDecrypt));

	//从TSInfo 获取节目信息
	for(i = 0; i < u32InputProgramNumber; ++i)
	{
		u16ChannelID = pstTS->pInputProgram[i].u16ChannelID;
		u16ServiceID = pstTS->pInputProgram[i].u16ServiceID;
		u8CAMIndex = pstTS->pInputProgram[i].u8CAMIndex;
		u8ValidFlag = pstTS->pInputProgram[i].u8ValidFlag;
		if(TSP_FLAG_INVALID == u8ValidFlag)
		{
			continue;
		}

	
		switch(u16ChannelID)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			{
				u32ProgramIndex = s_arrstProgramDecrypt[u16ChannelID].u32ProgramNumber;
			    s_arrstProgramDecrypt[u16ChannelID].u16ServiceID[u32ProgramIndex] = u16ServiceID;
				s_arrstProgramDecrypt[u16ChannelID].u8CAMIndex[u32ProgramIndex] = u8CAMIndex;
				++s_arrstProgramDecrypt[u16ChannelID].u32ProgramNumber;
				break;
			}
			default:
			{
				LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: u16ChannelID = %u", u16ChannelID);
				break;
			}
		}
	}

	//打印获取到的信息
	for(i = 0; i < CHANNEL_MAX; ++i)
	{
		printf("Channel Id: %d\n", i);
		for(j = 0; j < s_arrstProgramDecrypt[i].u32ProgramNumber; ++j)
		{
			printf("ServiceID: %u\n", s_arrstProgramDecrypt[i].u16ServiceID[j]);
			printf("CAMIndex: %u\n", s_arrstProgramDecrypt[i].u8CAMIndex[j]);
		}
	}

	return WV_SUCCESS;
	
}


static void WebXml_GetIpPortFromStr(const char *str, U32 *pu32Ip, U16 * pu16Port)
{
	if(!str)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
		return ;
	}

	char ip_addr[16] = {0};
	char port[6] = {0};
	char *ptr = NULL;

	//找到ip地址的字符串
	ptr = strstr(str, ":");
	memcpy(ip_addr, str, ptr - str);
	inet_pton(AF_INET, ip_addr, pu32Ip);

	//找到port 的字符串
	ptr++;
	strncpy(port, ptr, sizeof(port));
	*pu16Port = atoi(port);

}

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

	for(i = 0; i < CHANNEL_MAX; ++i)
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

		//TODO
		//获取ts ip 和端口
		WebXml_GetIpPortFromStr(s_arrstXmlParamsInfo[i].ts_out_addr.value, &u32IpAddr, &u16Port);
		s_arrstParamsInfo[i].u32TsIpAddr = u32IpAddr;
		s_arrstParamsInfo[i].u16TsPort = u16Port;
		s_arrstParamsInfo[i].u32TsSwitch = atoi(s_arrstXmlParamsInfo[i].ts_out_switch.value);

		//获取monitor ip 和端口
		WebXml_GetIpPortFromStr(s_arrstXmlParamsInfo[i].ts_out_addr.value, &u32IpAddr, &u16Port);
		s_arrstParamsInfo[i].u32MonitorIpAddr = u32IpAddr;
		s_arrstParamsInfo[i].u16MonitorPort = u16Port;
		s_arrstParamsInfo[i].u32MonitorSwitch = atoi(s_arrstXmlParamsInfo[i].monitor_switch.value);
		
	}
	
}


static void WebXml_XmlProgram2StructProgram(void)
{
	U32 i = 0;
	U32 j = 0;
	U32 u32CAMIndex = 0;
	U32 u32Decrypt = 0;

	for(i = 0; i < CHANNEL_MAX; ++i)
	{
		s_arrstProgramDecrypt[i].u16ChannelID = atoi(s_arrstXmlProgramDecrypt[i].channel.value);
		s_arrstProgramDecrypt[i].u32ProgramNumber = atoi(s_arrstXmlProgramDecrypt[i].u32ProgramNumber);
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



static void WebXml_StructProgram2XmlProgram(void)
{
	U32 i = 0;
	U32 j = 0;
	U32 u32ProgramNumber = 0;
	U16 u16ChannelID = 0;
	U8 u8CAMIndex = 0;
	char strChannelId[10] = {0};

	for(i = 0; i < CHANNEL_MAX; ++i)
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



