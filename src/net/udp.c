/*
 * =====================================================================================
 *
 *       Filename:  udp.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/05/17 22:36:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lgmark
 *          Email:  life_is_legend@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "udp.h"

#include "appGlobal.h"
#include "WV_log.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


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
wvErrCode UDP_Sendto(int sockfd, const struct sockaddr_in *pstAddr, const char *buf, int buf_size)
{
    if((!pstAddr) || (!buf))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
    }

    int ret = 0;
    char err_buf[ERR_BUF_LEN] = {0};

    if((ret = sendto(sockfd, buf, buf_size, 0, (struct sockaddr *)(pstAddr),sizeof(*pstAddr))) < 0)
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call sendto: %s", err_buf);
        return WV_ERR_SOCKET_SEND;
    }
    
    return WV_SUCCESS;

}


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
wvErrCode UDP_Recvform(int sockfd, struct sockaddr_in *pstAddr, char *buf, int buf_size)
{
    if((!pstAddr) || (!buf) || (!buf_size))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
    }

    int ret = 0;
    socklen_t len = 0; 
    char err_buf[ERR_BUF_LEN] = {0};

    if((ret = recvfrom(sockfd, buf, buf_size, 0, (struct sockaddr *)(pstAddr), &len)) < 0)
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call recvfrom: %s", err_buf);
        return WV_ERR_SOCKET_RECV;
    }

    return WV_SUCCESS;

}


wvErrCode UDP_CreateSendSockfd(int *sockfd)
{
    if(!sockfd)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
    }

    char err_buf[MAX_ERR_INFO_LEN] = {0};
    *sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if(*sockfd < 0)
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call socket: %s", err_buf);
        return WV_ERR_SOCKET_OPEN;
    }
    
    return WV_SUCCESS;

}



/*
* function: UDP_BroatcastSockfd
*
* description: 创建广播的socket，设置socket的广播属性
*
* input:  @pstBroatcastInfo->port: 设置广播监听的端口
*
* output: @socket: socket 句柄
*         @struct socketaddr_in: socket 地址的相关信息
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode UDP_BroatcastSockfd(struct BroatcastInfo *pstBroatcastInfo)
{
    if(!pstBroatcastInfo)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
    }

    char err_buf[ERR_BUF_LEN] = {0};
    const int opt = -1;

    if( -1 == (pstBroatcastInfo->sockfd = socket(AF_INET, SOCK_DGRAM, 0)))
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call socket: %s", err_buf);
        return WV_ERR_SOCKET_OPEN;
    }

    bzero(&(pstBroatcastInfo->addr), sizeof(struct sockaddr_in));
    pstBroatcastInfo->addr.sin_family = AF_INET;
    pstBroatcastInfo->addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    pstBroatcastInfo->addr.sin_port = htons(pstBroatcastInfo->port);

    //设置广播属性
    if(-1 == (setsockopt(pstBroatcastInfo->sockfd, SOL_SOCKET, SO_BROADCAST, (void *)&opt, sizeof(opt))))
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call setsockopt: %s", err_buf);
        CLOSE(pstBroatcastInfo->sockfd);
        return WV_ERR_SOCKET_SETOPT;
    }

    if(-1 == bind(pstBroatcastInfo->sockfd,(struct sockaddr *) (&(pstBroatcastInfo->addr)), sizeof(pstBroatcastInfo->addr)))
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call bind: %s", err_buf);
        CLOSE(pstBroatcastInfo->sockfd);
        return WV_ERR_SOCKET_BIND;
    }

    return WV_SUCCESS;
}



/*
* function: UDP_UnicastServerSockfd
*
* description: 创建UDP Server 单播socket 句柄
*
* input:  @pstUnicastInfo->ip_addr: ip 地址
*         @pstUnicastInfo->port: 端口
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
wvErrCode UDP_UnicastServerSockfd(struct UnicastInfo *pstUnicastInfo)
{
    if(!pstUnicastInfo)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
    }

    long ip_addr = 0;
    char err_buf[ERR_BUF_LEN] = {0};

#if 0
    if(1 != inet_pton(AF_INET, pstUnicastInfo->arrIp, &ip_addr))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: ip addr is error");
        return WV_ERR_SOCKET_IPADDR;
    }
#endif

    if( -1 == (pstUnicastInfo->sockfd = socket(AF_INET, SOCK_DGRAM, 0)))
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call socket: %s", err_buf);
        return WV_ERR_SOCKET_OPEN;
    }

    bzero(&(pstUnicastInfo->addr), sizeof(struct sockaddr_in));
    pstUnicastInfo->addr.sin_family = AF_INET;
    pstUnicastInfo->addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(-1 == bind(pstUnicastInfo->sockfd,(struct sockaddr *) (&(pstUnicastInfo->addr)), sizeof(pstUnicastInfo->addr)))
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call bind: %s", err_buf);
        CLOSE(pstUnicastInfo->sockfd);
        return WV_ERR_SOCKET_BIND;
    }

    return WV_SUCCESS;
}



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
wvErrCode UDP_MulticastRecvSockfd(struct MulticastInfo *pstMultiCastInfo)
{
    if(!pstMultiCastInfo)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
    }

    struct sockaddr_in addr;
    char err_buf[MAX_ERR_INFO_LEN] = {0};
    int val = 0;
    //创建套接字
    if((pstMultiCastInfo->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call socket : %s", err_buf);
        return WV_ERR_SOCKET_SETOPT;
    }

    //设置多个socket可以绑定相同的端口
    if(setsockopt(pstMultiCastInfo->sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call setsockopt: %s", err_buf);
        CLOSE(pstMultiCastInfo->sockfd);
        return WV_ERR_SOCKET_SETOPT;
    }
    
    //绑定端口
    memset(&(pstMultiCastInfo->ip_addr), 0, sizeof(struct sockaddr_in));
    pstMultiCastInfo->ip_addr.sin_family = AF_INET;
    pstMultiCastInfo->ip_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    pstMultiCastInfo->ip_addr.sin_port = htons(pstMultiCastInfo->port);

    if(bind(pstMultiCastInfo->sockfd,(struct sockaddr *) &(pstMultiCastInfo->ip_addr), sizeof(struct sockaddr)) < 0)
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call bind:%s", err_buf);
        CLOSE(pstMultiCastInfo->sockfd);
        
        return WV_ERR_SOCKET_BIND;
    }

    return WV_SUCCESS;
}


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
wvErrCode UDP_MultiAddGroup(struct MulticastInfo *pstMultiCastInfo, const char *group_ip)
{
    if((!pstMultiCastInfo) || (!group_ip))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
    }
    
    struct ip_mreq mreq;
    char err_buf[MAX_ERR_INFO_LEN] = {0};

    //组播地址
    inet_pton(AF_INET, group_ip, &(mreq.imr_multiaddr.s_addr));
    //指定接收数据网卡
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    //加入多播组
    if(setsockopt(pstMultiCastInfo->sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call setsockopt: %s", err_buf);

        return WV_ERR_SOCKET_SETOPT;
    }

    return WV_SUCCESS;
}


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
wvErrCode UDP_MultiDropGroup(struct MulticastInfo *pstMultiCastInfo, const char *group_ip)
{
    if((!pstMultiCastInfo) || (!group_ip))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
    }
    
    struct ip_mreq mreq;
    char err_buf[MAX_ERR_INFO_LEN] = {0};

    //组播地址
    inet_pton(AF_INET, group_ip, &(mreq.imr_multiaddr.s_addr));
    //指定接收数据网卡
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    //加入多播组
    if(setsockopt(pstMultiCastInfo->sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call setsockopt: %s", err_buf);

        return WV_ERR_SOCKET_SETOPT;
    }

    return WV_SUCCESS;
}

