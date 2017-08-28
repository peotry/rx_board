/*
 * =====================================================================================
 *
 *       Filename:  xml.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/25/17 13:34:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  linsheng.pan (), life_is_legend@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "wv_xml.h"

#include "appGlobal.h"
#include "Log/wv_log.h"

#include <string.h>

/*
* function: XML_OpenMem
*
* description: 从内存中加载一份xml生成xmldoc
*
* input:  @src_buf:xml缓冲区
*         @buf_len:缓冲区长度
*         @root_name:根节点名字用于简单校验，若是为NULL则忽略
*
* output: @root_node:根节点
*         @root_name_r:返回的根节点名字
*
* return@ 
* success: xmlDocPtr
*    fail: NULL
*
* author: linsheng.pan
*/
xmlDocPtr XML_OpenMem(char *src_buf, int buf_len, const char *root_name, char * root_name_r, int root_name_r_len)
{
    if((!src_buf) || (buf_len < 1))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
        return NULL;
    }


    //从内存中加载xml数据
    /*
     *xml之间的互相驳接--注意
     *xml文件的节点存储跟xmlDoc有关，如字典缓冲,说明节点保存的内容不一定只限定存储在一个节点上,
     可能跟其他有关; 如果对每个xmlDoc读取的时候限制为XML_PARSE_NOBLANKS | XML_PARSE_NODICT,
     而且两个xmlDoc保证都是这样,即可实现xml文件的简单合并.
     *如果在代码去掉这两个限制的话，会在释放文档指针的时候出现错误.
     *xmlDocPtr xml_doc = xmlReadMemory(src_buf, buf_len, NULL, "UTF-8", XML_PARSE_NOBLANKS | XML_PARSE_NODICT);
     */


    xmlDocPtr xml_doc = xmlReadMemory(src_buf, buf_len, NULL, "UTF-8", XML_PARSE_NOBLANKS | XML_PARSE_NODICT);
    if(!xml_doc)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlReadMemory");
        return NULL;
    }

    xmlNodePtr cur_node_tmp = xmlDocGetRootElement(xml_doc);
    if(!cur_node_tmp)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlDocGetRootElement");
        xmlFreeDoc(xml_doc);
        return NULL;
    }

	strncpy(root_name_r, cur_node_tmp->name, root_name_r_len);
	
    if(root_name)
    {
        if(0 != xmlStrcmp(cur_node_tmp->name, (xmlChar *)root_name))
        {
            xmlFreeDoc(xml_doc);
            return NULL;
        }
    }

    return xml_doc;
}

/*
 * function: XML_OpenXmlFile
 *
* description: 获取xml文档的句柄
*
* input:  @filename: xml文档名字
*         @root_name: 检查xml文档合法性
*
* output: @
*
* return: 
*         
*
* author: linsheng.pan
*/
xmlDocPtr XML_OpenXmlFile(const char  *filename, const char *root_name, char *root_name_r, int root_name_r_len)
{
    if((!filename))
    {
       LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL!");
       return NULL;
    }

    xmlDocPtr xml_doc = NULL;

    //xml_doc = xmlReadFile(filename, "UTF-8", XML_PARSE_NOBLANKS);
    xml_doc = xmlParseFile(filename);
    if(!xml_doc)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: read xml file:%s failed", filename);

        return NULL;
    }

    xmlNodePtr cur_node = xmlDocGetRootElement(xml_doc);
    if(!cur_node)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: xml_doc getroot elem failed!");
        XML_FreeDoc(xml_doc);
        return NULL;
    }

    //root name 为空不检查根元素名字
    strncpy(root_name_r, cur_node->name, root_name_r_len);
    if(root_name)
    {
        //检查根节点名字
        if(xmlStrcmp(cur_node->name, (xmlChar *)root_name))
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: xml root name error!");
            XML_FreeDoc(xml_doc);
            return NULL;
        }
    }

   return xml_doc; 
}


/*
* function: XML_FreeDoc 
*
* description: 释放xml文档的xml句柄
*
* input:  @pDoc: xml 文档句柄
*
* output: @
*
* return@ 
* success: WV_SUCCESS
*    fail: WV_ERR_FAILURE
*
* author: linsheng.pan
*/
wvErrCode XML_FreeDoc(xmlDocPtr xml_doc)
{
    if(!xml_doc)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL!");
        return WV_ERR_FAILURE;
    }

    xmlFreeDoc(xml_doc);

    return WV_SUCCESS;
}


/*
* function: XML_RenameConf
*
* description: 把service 的临时写好的配置文件rename成为正式文件，
*              原子操作，防止断电配置损坏，新增、修改都要这样做。
*
* input:  @file_tmp: 临时文件
*         @file:正式文件
*         @svr_id:ser_id 号，不空，则是填充前面两个参数是格式化%s
*
* output: @
*
* return@ 
* success: 
*    fail: 
*
* author: linsheng.pan
*/
static wvErrCode XML_RenameConf(const char *file_tmp, const char *file, const char *ser_id)
{
    if((!file_tmp) || (!file))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: params = NULL");
        return WV_ERR_FAILURE;
    }

    if(!ser_id)
    {
        //没有填充字符，则是直接把file_tmp rename 成 file
        if(rename(file_tmp, file))
        {
            return WV_ERR_FAILURE;
        }
        return WV_SUCCESS;
    }

    char path[MAX_FILENAME_LENGTH] = {0};
    char path_tmp[MAX_FILENAME_LENGTH] = {0};
    int count = snprintf(path, sizeof(path) -1 , file, ser_id);
    if((count < 1) || (count > (sizeof(path) -1)))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: service conf path too long (%s)", path);
        return WV_ERR_FAILURE;
    }

    if(rename(path_tmp, path))
    {
        return WV_ERR_FAILURE;
    }
    
    return WV_SUCCESS;
}

#define TXT_ENCODING "UTF-8"

/*
* function: XML_WriteXmlFile
*
* description: 将一个xml原子写入文件
*
* input:  @path: 目标文件的路径
*         @xml_doc: xml文档句柄
*
* output: @
*
* return@ 
* success: WV_SUCCESS
*    fail: WV_ERR_FAILURE
*
* author: linsheng.pan
*/
wvErrCode XML_WriteXmlFile(const char *path, xmlDocPtr xml_doc)
{
    char path_tmp[MAX_FILENAME_LENGTH] = {0};
    int cnt = snprintf(path_tmp, sizeof(path_tmp)-1, "%s.tmp", path);
    if(cnt < 1 || cnt > (sizeof(path_tmp)-1))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: service conf path too long (%s)", path);
        return WV_ERR_FAILURE;
    }


    if(1 > xmlSaveFormatFileEnc(path_tmp, xml_doc, (const char *) TXT_ENCODING, 1)) 
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: xmlSaveFormatFile  path=%s", path_tmp);
        return WV_ERR_FAILURE;
    }

    return XML_RenameConf(path_tmp, path, NULL);
}


/*
 * function: XML_GetChildNode
 *
 * description:在当前节点cur_node下，查找名为elem_name的节点指针 
 *
 * input:  @cur_node:当前节点
 *         @elem_name:元素名称
 *
 * output: @
*
* return@ 
* success: xmlNodePtr
*    fail: NULL
*
* author: linsheng.pan
*/
xmlNodePtr XML_GetChildNode(xmlNodePtr cur_node, const char *elem_name)
{
    if((!cur_node) || (!elem_name))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
        return NULL;
    }

    xmlNodePtr tmp_node = cur_node->children;
    while(tmp_node)
    {
        if((tmp_node->name) && (!xmlStrcmp(tmp_node->name, (const xmlChar *)elem_name)))
        {
            return tmp_node;
        }
        tmp_node = tmp_node->next;
    }

    return NULL;
}


/*
* function:XML_GetChildNodeValue 
*
* description: 获取当前节点cur_node下，名称为elem_name的值
*
* input:  @cur_node: 当前节点
*         @elem_name: 元素名字
*
* output: @
*
* return@ 
* success: xmlChar
*    fail: NULL
*
* author: linsheng.pan
*/
xmlChar* XML_GetChildNodeValue(xmlNodePtr cur_node,const char *elem_name)
{
    if((!cur_node) || (!elem_name))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
        return NULL;
    }

    xmlNodePtr child_node = XML_GetChildNode(cur_node, elem_name);
    if((!child_node) || (!child_node->children))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: get_child_node do not have child or content, key(%s)", elem_name);
        return NULL;
    }

    return xmlNodeGetContent(child_node->children);
}



/*
* function: XML_GetNodeSet
*
* description: 使用xpath查询节点集
*
* input:  @xml_doc: 需要查询的xml文档句柄
*         @xpath: 表达式
*
* output: @
*
* return@ 
* success: xmlXPathObjectPtr
*    fail: NULL
*
* author: linsheng.pan
*/
xmlXPathObjectPtr XML_GetNodeSet(xmlDocPtr xml_doc, xmlChar *xpath)
{
    if((!xml_doc) || (!xpath))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
        return NULL;
    }

    xmlXPathContextPtr context = NULL;
    xmlXPathObjectPtr result = NULL;

    //创建一个xpath上下文
    context = xmlXPathNewContext(xml_doc);
    if(!context)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlXPathNewContext failed!");
        return NULL;
    }
    //查询xpath表达式
    result = xmlXPathEvalExpression(xpath, context);
    //释放xpath上下文
    xmlXPathFreeContext(context);
    if(!result)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlXPathFreeContext failed!");
        return NULL;
    }
    //判断查询结果是否为空
    
    if(xmlXPathNodeSetIsEmpty(result->nodesetval))
    {
        xmlXPathFreeObject(result);
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_XML, "no result!");
        return NULL;
    }

    return result;
}


/*
* function: XML_NewXmlDoc
*
* description: 新建立一个空的xml_doc, 并返回跟节点指针
*
* input:  @root_node: 生成的根节点指针
*         @root_name: 根节点名字
*
* output: @
*
* return@ 
* success: xmlDocPtr
*    fail: NULL
*
* author: linsheng.pan
*/
xmlDocPtr XML_NewXmlDoc(xmlNodePtr *root_node, const char *root_name)
{
    if(!root_name)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
        return NULL;
    }

    xmlDocPtr xml_doc = NULL;
    xmlNodePtr root_node_tmp = NULL;

    //创建xml_doc
    xml_doc = xmlNewDoc((xmlChar*) "1.0");
    if(!xml_doc)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlNewDoc");
        return NULL;
    }

    //创建根节点
    root_node_tmp = xmlNewNode(NULL, (xmlChar *) root_name);
    if(!root_node_tmp) 
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlNewNode");
        XML_FreeDoc(xml_doc);
        return NULL;
    }

    //将xml_doc 和 根节点关联起来
    xmlDocSetRootElement(xml_doc, root_node_tmp);
    *root_node = root_node_tmp;

    return xml_doc;
}



/*
* function: XML_NewNode
*
* description: 创建一个节点
*
* input:  @key_name:
*         @value:
*
* output: @
*
* return@ 
* success: xmlNodePtr
*    fail: NULL
*
* author: linsheng.pan
*/
xmlNodePtr XML_NewNode(const char *key_name, const char *value)
{
    if((!key_name))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
        return NULL;
    }

    xmlNodePtr new_node = xmlNewNode(NULL, (xmlChar *)key_name);
    if(!new_node)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlNewNode");
        return NULL;
    }

    if(value)
    {
        xmlNodePtr content = xmlNewText((xmlChar *)value);
        if(!content)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlNewText");
            xmlFreeNode(new_node);
            return NULL;
        }

        if(!xmlAddChild(new_node, content))
        {
            xmlFreeNode(new_node);
            xmlFreeNode(content);
            return NULL;
        }
    }

    return new_node;
}


/*
* function: XML_AddNewChildNode
*
* description: 在father_node节点下增加一个节点，不管里面有没有重名
*
* input:  @father_node: 父亲节点
*         @key_name:
*         @value:
*
* output: @
*
* return@ 
* success: xmlNodePtr
*    fail: NULL
*
* author: linsheng.pan
*/
xmlNodePtr XML_AddNewChildNode(xmlNodePtr father_node, const char *key_name, const char *value)
{
    xmlNodePtr node = XML_NewNode(key_name, value);
    if(!node)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call XML_NewNode");
        return NULL;
    }

    if(!xmlAddChild(father_node, node))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlAddChild");
        xmlFreeNodeList(node);
        return NULL;
    }

    return node;
}


/*
* function: XML_UpdateChildNode
*
* description: 新增或更新key_name节点，若是存在key_name节点则覆盖老的
*
* input:  @father_node: 父亲节点
*         @key_name:
*         @value:
*
* output: @
*
* return@ 
* success: 
*    fail: 
*
* author: linsheng.pan
*/
wvErrCode XML_UpdateChildNode(xmlNodePtr father_node, const char *key_name, const char *value)
{
    if((!father_node) || (!key_name))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
        return WV_ERR_FAILURE;
    }

    xmlNodePtr old_child_node = XML_GetChildNode(father_node, key_name);

    if(old_child_node)
    {
        //存在同名节点，则替换
        xmlNodePtr new_child_node = XML_NewNode(key_name, value);
        if(!new_child_node)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call XML_NewNode");
            return WV_ERR_FAILURE;
        }

        if(xmlReplaceNode(old_child_node, new_child_node))
        {
            //释放旧的节点
            xmlFreeNodeList(old_child_node);
            return WV_SUCCESS;
        }
        else
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlReplaceNode");
            //释放新节点
            xmlFreeNodeList(new_child_node);

            return WV_ERR_FAILURE;
        }
    }
    else
    {
        //不存在同名节点，则加入到父亲节点下
        if(!xmlNewTextChild(father_node, NULL, (xmlChar *)key_name, (xmlChar *)value))
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: xmlNewTextChild");
            return WV_ERR_FAILURE;
        }
    }
    
    return WV_SUCCESS;
}

/*
* function: XML_ReplaceCopyNode
*
* description: 用new_node节点的副本去替换old_node
*
* input:  @new_node:需要拷贝的节点
*         @old_node:被替换的节点
*
* output: @
*
* return@ 
* success: WV_SUCCESS
*    fail: WV_ERR_FAILURE
*
* author: linsheng.pan
*/
wvErrCode XML_ReplaceCopyNode(xmlNodePtr old_node, xmlNodePtr new_node)
{
    if((!old_node) || (!new_node))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
        return WV_ERR_FAILURE;
    }

    xmlNodePtr new_node_tmp = xmlCopyNode(new_node, 1);
    if(!new_node_tmp)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlCopyNode");
        return WV_ERR_FAILURE;
    }

    //把新的节点链直接替换老的节点
    if(!xmlReplaceNode(old_node, new_node_tmp))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlReplaceNode");

        xmlFreeNodeList(new_node_tmp);
        
        return WV_ERR_FAILURE;
    }

    //替换成功释放旧的节点
    xmlFreeNodeList(old_node);

    return WV_SUCCESS;
}


/*
* function: XML_AddChildCopyNode
*
* description: 将child_node节点拷贝一份加入到father_node下
*
* input:  @child_node: 需要拷贝添加到father_node下的节点
*         @father_node: 父节点
*
* output: @
*
* return@ 
* success: WV_SUCCESS
*    fail: WV_ERR_FAILURE
*
* author: linsheng.pan
*/
wvErrCode XML_AddChildCopyNode(xmlNodePtr father_node, xmlNodePtr child_node)
{
    if((!child_node) || (!father_node))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
        return WV_ERR_FAILURE;
    }

    xmlNodePtr child_node_tmp = xmlCopyNode(child_node, 1);
    if(!child_node_tmp)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlCopyNode");
        return WV_ERR_FAILURE;
    }

    if(!xmlAddChild(father_node, child_node_tmp))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: call xmlAddChild");
        xmlFreeNodeList(child_node_tmp);
        return WV_ERR_FAILURE;
    }

    return WV_SUCCESS;
}


/*
* function: XML_UpdateChildCopyNode
*
* description: 把child_node节点更新到father_node节点下，若节点在则替换原来节点，若不存在则新增
*
* input:  @child_node:孩子节点
*         @father_node:父亲节点
*
* output: @
*
* return@ 
* success: WV_SUCCESS
*    fail: WV_ERR_FAILURE
*
* author: linsheng.pan
*/
wvErrCode XML_UpdateChildCopyNode(xmlNodePtr father_node, xmlNodePtr child_node)
{
    if((!father_node) || (!child_node))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
        return WV_ERR_FAILURE;
    }


    char *key_name = (char *)child_node->name;
    xmlNodePtr child_node_old = XML_GetChildNode(father_node, key_name);
    if(child_node_old)
    {
       return  XML_ReplaceCopyNode(child_node_old, child_node);
    }
    else
    {
        return XML_AddChildCopyNode(child_node, father_node);
    }
}




wvErrCode XML_XpathGetValue(xmlDocPtr *xml_doc, xmlChar *xpath, char * str_value, int str_value_len)
{
	if((!xml_doc) || (!xpath) || (!str_value))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_XML, "Error: param = NULL");
		return WV_ERR_PARAMS;
	}

	xmlNodeSetPtr nodeset_ptr = NULL;
	xmlXPathObjectPtr xpath_result_ptr = NULL;
	xmlChar * value = NULL;
	wvErrCode ret = WV_SUCCESS;

	xpath_result_ptr = XML_GetNodeSet(xml_doc, xpath);
	if(xpath_result_ptr)
	{
		nodeset_ptr = xpath_result_ptr->nodesetval;
		//若不是唯一的值，返回错误
		if(1 != nodeset_ptr->nodeNr)
		{
			ret = WV_ERR_FAILURE;
			xmlXPathFreeObject(xpath_result_ptr);
		}
		else
		{
			value = xmlNodeListGetString(xml_doc, nodeset_ptr->nodeTab[0]->xmlChildrenNode, 1);
			strncpy(str_value, value, str_value_len);
			xmlFree(value);
			xmlXPathFreeObject(xpath_result_ptr);
		}
	}
	else
	{
		ret = WV_ERR_FAILURE;
	}

	return ret;
}










