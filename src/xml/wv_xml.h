/*
 * =====================================================================================
 *
 *       Filename:  xml.h
 *
 *    Description:  
 *    
 *
 *        Version:  1.0
 *        Created:  07/25/17 13:33:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  linsheng.pan (), life_is_legend@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef INCLUDE_XML_XML_H
#define INCLUDE_XML_XML_H 
#ifdef __cplusplus
extern "C" {
#endif

#include "err/wv_err.h"

#include "libxml/xmlmemory.h"
#include "libxml/parser.h"
#include "libxml/xpath.h"
#include "libxml/tree.h"

//解析xml

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
*
* return@ 
* success: 
*    fail: 
*
* author: linsheng.pan
*/
xmlDocPtr XML_OpenMem(char *src_buf, int buf_len, const char *root_name, char *root_name_r, int root_name_r_len);


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
xmlDocPtr XML_OpenXmlFile(const char  *filename, const char *root_name, char *root_name_r, int root_name_r_len);


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
wvErrCode XML_FreeDoc(xmlDocPtr xml_doc);


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
wvErrCode XML_WriteXmlFile(const char *path, xmlDocPtr xml_doc);


/*
* function: XML_GetChildNode
*
* description:在当前节点cur_node下，查找名为elem_name的节点指针 
*
* input:  @cur_node:当前节点
*         @elem_name:元素名称
*
* output: @

* return@ 
* success: xmlNodePtr
*    fail: NULL
*
* author: linsheng.pan
*/
xmlNodePtr XML_GetChildNode(xmlNodePtr cur_node, const char *elem_name);


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
xmlChar* XML_GetChildNodeValue(xmlNodePtr cur_node, const char *elem_name);

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
xmlXPathObjectPtr XML_GetNodeSet(xmlDocPtr xml_doc, xmlChar *xpath);

//生成xml

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
xmlDocPtr XML_NewXmlDoc(xmlNodePtr *root_node, const char *root_name);



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
xmlNodePtr XML_NewNode(const char *key_name, const char *value);


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
xmlNodePtr XML_AddNewChildNode(xmlNodePtr father_node, const char *key_name, const char *value);


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
wvErrCode XML_UpdateChildNode(xmlNodePtr father_node, const char *key_name, const char *value);

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
wvErrCode XML_ReplaceCopyNode(xmlNodePtr old_node, xmlNodePtr new_node);


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
wvErrCode XML_AddChildCopyNode(xmlNodePtr child_node, xmlNodePtr father_node);


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
wvErrCode XML_UpdateChildCopyNode(xmlNodePtr child_node, xmlNodePtr father_node);


wvErrCode XML_XpathGetValue(xmlDocPtr *xml_doc, xmlChar *xpath, char * str_value, int str_value_len);


//xml编码格式转换

#ifdef __cplusplus
}
#endif
#endif /* ifndef INCLUDE_XML_XML_H */

