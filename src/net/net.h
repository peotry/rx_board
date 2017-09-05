/*
 * =====================================================================================
 *
 *       Filename:  net.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/22/17 16:13:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  linsheng.pan (), life_is_legend@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef INCLUDE_NET_NET_H
#define INCLUDE_NET_NET_H 

#include "err/wv_err.h"
#include <net/if.h>

/*
* function: 获取本机指定网卡的IP
*
* description: Net_GetIpAddr
*
* input:  @eth_name: 网卡的名字
*         @ip_buf: ip的字符串
*         @buf_size: 字符串的长度
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode Net_GetIpAddr(const char *eth_name, unsigned int *ip);

wvErrCode Net_GetIpAddrStr(const char *eth_name, char *ip_buf, int buf_size);



/*
* function: Net_GetMacAddr
*
* description: 获取本机指定网卡的MAC地址
*
* input:  @eth_name: 网卡名字
*         @mac_buf: 存储mac地址
*         @buf_size: mac地址的长度
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode Net_GetMacAddr(const char *eth_name, unsigned char *mac_buf, int buf_size);


/*
* function: Net_SetIpAddr
*
* description: 设置网卡的IP
*
* input:  @eth_name:网卡名
*         @ip_buf:ip地址("192.168.1.1")
*         @buf_size: ip地址的长度
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode Net_SetIpAddr(const char *eth_name, const char *ip_buf, int buf_size);



/*
* function: Net_SetMacAddr
*
* description:给指定网卡设置mac地址
*
* input:  @eth_name:网卡名
*         @mac_buf:mac地址
*         @buf_size:mac地址长度
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode Net_SetMacAddr(const char *eth_name, const unsigned char *mac_buf, int buf_size);


/*
* function: Net_SetIpAddr2FPGA
*
* description: 将IP地址告诉FPGA
*
* input:  @eth_name:网卡名
*         @ip_buf:ip地址("192.168.1.1")
*         @buf_size:地址长度
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode Net_SetIpAddr2FPGA(const char *eth_name);


/*
* function: Net_SetMacAddr2FPGA
*
* description: 将Mac地址告诉FPGA
*
* input:  @eth_name:网卡名
*         @mac_buf:mac地址
*         @buf_size:mac地址的长度
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode Net_SetMacAddr2FPGA(const char *eth_name);


/*
* function: Net_BindInterface
*
* description: 将套接字绑定网卡
*
* input:  @sockfd: 一个已经打开的套接字
*         @eth_name: 网卡名字
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode Net_BindInterface(int *sockfd, const char *eth_name);

wvErrCode Net_GetIpAddrWithDHCP(const char * eth_name);

wvErrCode Net_GetLocalNetMask(const char * eth_name, char * net_mask_str, int str_len);

wvErrCode Net_GetLocalMac(const char *eth_name, char * mac_addr, int str_len);


wvErrCode Net_GetGateWay(char *gateway, int str_len);


#endif /* ifndef INCLUDE_NET_NET_H */
