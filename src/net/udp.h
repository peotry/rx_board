/*
 * =====================================================================================
 *
 *       Filename:  udp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/05/17 21:29:03
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lgmark (mk), life_is_legend@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef INCLUDE_NET_UDP_H
#define INCLUDE_NET_UDP_H

#include "err/wv_err.h"

#include <netinet/in.h>

struct BroatcastInfo
{
    int sockfd;
    struct sockaddr_in addr;
    short port;
};

struct UnicastInfo
{
    int sockfd;
    struct sockaddr_in addr;
    short port;
    char arrIp[16];  //255.255.255.255
};

struct MulticastInfo
{
    int sockfd;
    struct sockaddr_in ip_addr;
    short port;
    char arrIp[16]; //227.10.20.80
};


/*
* function: UDP_Sendto
*
* description: UDP 发送数据
*
* input:  @sockfd: 套接字
*         @pstAddr: 目的端地址和端口
*         @buf:发送内容
*         @bufsize:发送内容大小
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode UDP_Sendto(int sockfd, const struct sockaddr_in *pstAddr, const char *buf, int buf_size);


/*
* function: UDP_Recvform
*
* description: UDP接收数据
*
* input:  @sockfd:套接字
*         @pstAddr:发送端的地址和端口
*         @buf:接收到的内容
*         @buf_size: buf 内容的大小
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode UDP_Recvform(int sockfd, struct sockaddr_in *pstAddr, char *buf, int buf_size);


/*
* function: UDP_SendSockfd
*
* description: 创建发送端的套接字
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode UDP_CreateSendSockfd(int *sockfd);


/*
* function: UDP_BroatcastSockfd
*
* description: 创建广播的套接字
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode UDP_BroatcastSockfd(struct BroatcastInfo *pstBroatcastInfo);


/*
* function: UDP_UnicastRecvSockfd
*
* description: 创建单播接收端的套接字
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode UDP_UnicastRecvSockfd(struct UnicastInfo *pstUnicastInfo);
//wvErrCode UDP_UnicastClientSockfd(struct UnicastInfo *pstUnicastInfo);


/*
* function: UDP_MulticastRecvSockfd
*
* description: 创建多播接收套接字
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode UDP_MulticastRecvSockfd(struct MulticastInfo *pstMultiCast);


/*
* function: UDP_MultiAddGroup
*
* description: 加入一个多播组
*
* input:  @group_ip: 多播组地址
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode UDP_MultiAddGroup(struct MulticastInfo *pstMultiCast, const char *group_ip);


/*
* function: UDP_MultiRemoveGroup
*
* description: 退出一个多播组
*
* input:  @group_ip: 多播组地址
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode UDP_MultiDropGroup(struct MulticastInfo *pstMultiCastInfo, const char *group_ip);


#endif /* ifndef INCLUDE_NET_UDP_H */
