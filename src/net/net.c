/*
 * =====================================================================================
 *
 *       Filename:  net.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/22/17 16:32:49
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  linsheng.pan (), life_is_legend@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "net.h"

#include "appGlobal.h"
#include "WV_log.h"
#include "FPGA.h"

#include <arpa/inet.h>
#include <sys/ioctl.h>

#define MAX_NETWORK_NUM         (16)

static struct ifconf s_stIfconf;
static struct ifreq s_st_arrIfreq[MAX_NETWORK_NUM];

static int Net_FindInterface(const char *eth_name)
{
    if(!eth_name)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return -1;
    }

    int index = -1;
    int interface_num = 0;
    interface_num = s_stIfconf.ifc_len / sizeof(struct ifreq);

    while(interface_num-- > 0)
    {
        if(!strcmp(s_st_arrIfreq[interface_num].ifr_name, eth_name))
        {
            index = interface_num;
            break;
        }
    }

    return index;
}

/*
* function: Net_GetInterfaceInfo
*
* description: 获取所有工作网卡的信息(网)
*
* input:  @
*
* output: @
*
* return: 
*
* author: linsheng.pan
*/
static wvErrCode Net_GetInterfaceInfo(void)
{
    char err_buf[ERR_BUF_LEN] = {0};
    int sockfd = -1;
    int interface_num = 0;

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: %s", err_buf);

        return WV_ERR_SOCKET_OPEN;
    }

    s_stIfconf.ifc_len = sizeof(s_st_arrIfreq);
    s_stIfconf.ifc_buf = (caddr_t)(s_st_arrIfreq);

    if(!ioctl(sockfd, SIOCGIFCONF, &s_stIfconf))
    {
        interface_num = s_stIfconf.ifc_len / sizeof(struct ifreq);
        //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "Network interface : %d", interface_num);

        while(interface_num-- > 0)
        {
            //get mac ip broatcast subnet mask
            //if(!ioctl(sockfd, SIOCGIFFLAGS | SIOCGIFHWADDR | SIOCGIFADDR | SIOCGIFBRDADDR | SIOCGIFNETMASK, &s_st_arrIfreq[interface_num]))
			if(!ioctl(sockfd,  SIOCGIFADDR , &s_st_arrIfreq[interface_num]))
            {
                //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "Get Network[%d] Info Success!", interface_num);
            }
            else
            {
                ERR_STRING(err_buf);
                LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: %s", err_buf);

                CLOSE(sockfd);
                return WV_ERR_SOCKET_SETOPT;
            }
        }
    }
    else
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: %s", err_buf);

        CLOSE(sockfd);
        return WV_ERR_SOCKET_SETOPT;

    }

	CLOSE(sockfd);

    return WV_SUCCESS;
}

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
wvErrCode Net_GetIpAddrStr(const char *eth_name, char *ip_buf, int buf_size)
{
    if((!eth_name) || (!ip_buf))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
    }
    
    int index = -1;

    //获取本地网卡信息
    Net_GetInterfaceInfo();
    //判断是否有指定网口的ip
    if((index = Net_FindInterface(eth_name)) >= 0)
    {
        //将ip的网络序转换为点十制
        inet_ntop(AF_INET, &(((struct sockaddr_in *)&(s_st_arrIfreq[index].ifr_addr))->sin_addr.s_addr), ip_buf, buf_size);
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "%s : %s", eth_name, ip_buf);
    }
    else
    {
        //解锁资源并推出
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: Can't find %s", eth_name);
        return WV_ERR_FAILURE;
    }

    return WV_SUCCESS;
}

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
wvErrCode Net_GetIpAddr(const char *eth_name, unsigned int * ip)
{
    if((!eth_name) || (!ip))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
    }
    
    int index = -1;
	char ip_buf[16] = {0};
	unsigned int val = 0;

    //获取本地网卡信息
    Net_GetInterfaceInfo();
    //判断是否有指定网口的ip
    if((index = Net_FindInterface(eth_name)) >= 0)
    {
        //将ip的网络序转换为点十制
        inet_ntop(AF_INET, &(((struct sockaddr_in *)&(s_st_arrIfreq[index].ifr_addr))->sin_addr.s_addr), ip_buf, sizeof(ip_buf));
		memcpy(&val, &(((struct sockaddr_in *)&(s_st_arrIfreq[index].ifr_addr))->sin_addr.s_addr), sizeof(val));
		*ip = ntohl(val);
		//LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_COMM, "%s : %s", eth_name, ip_buf);
    }
    else
    {
        //解锁资源并推出
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: Can't find %s", eth_name);
        return WV_ERR_FAILURE;
    }

    return WV_SUCCESS;
}


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
wvErrCode Net_SetIpAddr(const char *eth_name, const char *ip_buf, int buf_size)
{
    if((!eth_name) || (!ip_buf))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
    }

    char cmd_str[MAX_LINE_LENGTH] = {0};

    snprintf(cmd_str, sizeof(cmd_str), "ifconfig %s down", eth_name);
    system(cmd_str);
    snprintf(cmd_str, sizeof(cmd_str), "ifconfig %s %s", eth_name, ip_buf);
    system(cmd_str);
    snprintf(cmd_str, sizeof(cmd_str), "ifconfig %s up", eth_name);
    system(cmd_str);

    return WV_SUCCESS;
}


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
wvErrCode Net_SetMacAddr(const char *eth_name, const unsigned char *mac_buf, int buf_size)
{
    if((!eth_name) || (!mac_buf))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_FAILURE;
    }

    char cmd_str[MAX_LINE_LENGTH] = {0};

    snprintf(cmd_str, sizeof(cmd_str), "ifconfig %s hw ether %02x:%02x:%02x:%02x:%02x:%02x", eth_name, mac_buf[0], mac_buf[1], mac_buf[2], mac_buf[3], mac_buf[4], mac_buf[5]);

    system(cmd_str);
    
    return WV_SUCCESS;
}


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
wvErrCode Net_SetIpAddr2FPGA(const char *eth_name)
{
	if(!eth_name)
	{
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_FAILURE;
    }	

	U32 u32IPAddr = 0;

	Net_GetIpAddr(eth_name, &u32IPAddr);

	FPGA_REG_Write(TSIP_IP_ADDR, u32IPAddr);

	return WV_SUCCESS;
}


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
wvErrCode Net_SetMacAddr2FPGA(const char *eth_name)
{
	if(!eth_name)
	{
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_FAILURE;
    }	

	U8 mac_addr[6] = {0};

	Net_GetLocalMac(eth_name, mac_addr, sizeof(mac_addr));

	FPGA_SetMacAddr(mac_addr);

	return WV_SUCCESS;
}



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
wvErrCode Net_BindInterface(int *sockfd, const char *eth_name)
{
    if((!eth_name) && (!sockfd))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
    }

    int ret = 0;
    char err_buf[MAX_ERR_INFO_LEN] = {0};
    struct ifreq netdev;
    memset(&netdev, 0, sizeof(netdev));
    strncpy(netdev.ifr_name, eth_name, sizeof(netdev.ifr_name) - 1);

    ret = setsockopt(*sockfd, SOL_SOCKET, SO_BINDTODEVICE, &netdev, sizeof(netdev));

    if(ret)
    {
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call setsockopt:%s", err_buf);

        return WV_ERR_SOCKET_SETOPT;
    }

    return WV_SUCCESS;
}

wvErrCode Net_GetIpAddrWithDHCP(const char * eth_name)
{
	if(!eth_name)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
    }

	char cmd[128] = {0};
	snprintf(cmd, sizeof(cmd), "udhcpc -i %s", eth_name);
	system(cmd);

	return WV_SUCCESS;
}


wvErrCode Net_GetLocalNetMask(const char * eth_name, char * net_mask_str, int str_len)  
{  
	if((!eth_name) || (!net_mask_str))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
	}

	const int ip_str_len = 16;

	if(str_len < ip_str_len)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: str_len = %d",  str_len);
        return WV_ERR_PARAMS;
	}
	
    int sock_netmask;  
    char err_buf[ERR_BUF_LEN] = {0}; 
  
    struct ifreq ifr_mask;  
    struct sockaddr_in *net_mask;  
          
    sock_netmask = socket( AF_INET, SOCK_STREAM, 0 );  
    if( -1 == sock_netmask)  
    {  
		ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call socket:%s", err_buf); 
        return WV_ERR_FAILURE;  
    }  
      
    memset(&ifr_mask, 0, sizeof(ifr_mask));     
    strncpy(ifr_mask.ifr_name, eth_name, sizeof(ifr_mask.ifr_name )-1);     
  
    if((ioctl( sock_netmask, SIOCGIFNETMASK, &ifr_mask ) ) < 0 )   
    {  
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call socket:%s", err_buf);
		CLOSE(sock_netmask);
        return WV_ERR_FAILURE;   
    }  
      
    net_mask = ( struct sockaddr_in * )&( ifr_mask.ifr_netmask );  
    strncpy(net_mask_str, inet_ntoa( net_mask -> sin_addr), str_len);  
      
    printf("local netmask:%s\n",net_mask_str);      
      
    CLOSE(sock_netmask); 

	return WV_SUCCESS;
}  



wvErrCode Net_GetLocalMac(const char *eth_name, char * mac_addr, int str_len)  
{  
	if((!eth_name) || (!mac_addr))
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
	}

	if(str_len < 6)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: str_len= %d\n", str_len);
        return WV_ERR_PARAMS;
	}
	
    int sock_mac;  
    struct ifreq ifr_mac;  
	char err_buf[ERR_BUF_LEN] = {0};
      
    sock_mac = socket( AF_INET, SOCK_STREAM, 0 );  
    if(sock_mac == -1)  
    {  
		ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call socket:%s", err_buf); 
        return WV_ERR_FAILURE;  
    }  
      
    memset(&ifr_mac,0,sizeof(ifr_mac));     
    strncpy(ifr_mac.ifr_name, eth_name, sizeof(ifr_mac.ifr_name)-1);     
  
    if((ioctl( sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)  
    {  
        ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call socket:%s", err_buf);
		CLOSE(sock_mac);
        return WV_ERR_FAILURE;   
    }    

	memcpy(mac_addr, ifr_mac.ifr_hwaddr.sa_data, str_len);
      
    CLOSE(sock_mac); 

	return WV_SUCCESS;
}  


wvErrCode Net_GetGateWay(char *gateway_str, int str_len)  
{  
	if(!gateway_str)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: param = NULL");
        return WV_ERR_PARAMS;
	}

	if(str_len < 16)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: str_len = %d",  str_len);
        return WV_ERR_PARAMS;
	}

    FILE *fp;  
    char buf[512];  
    char cmd[128];  
    char *tmp;  
	char err_buf[ERR_BUF_LEN] = {0};
  
    strncpy(cmd, "ip route", sizeof(cmd));  
    fp = popen(cmd, "r");  
    if(!fp)  
    {  
		ERR_STRING(err_buf);
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: call popen:%s", err_buf);

		return WV_ERR_FAILURE;
    }  

	
    while(fgets(buf, sizeof(buf), fp) != NULL)  
    {  
        tmp =buf;  
        while(*tmp && isspace(*tmp)) 
        {
            ++tmp;  
        }
		
        if(strncmp(tmp, "default", strlen("default")) == 0)  
        {
            break;  
        }
    }  
	
    sscanf(buf, "%*s%*s%s", gateway_str);         
    pclose(fp);  
      
    return WV_SUCCESS;  
}  



