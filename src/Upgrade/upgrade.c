#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <unistd.h>
#include <string.h>


#include "upgrade.h"
//#include "system.h"
#include "appGlobal.h"
#include "WV_log.h"
#include "tools.h"

#include "net/udp.h"
#include "thread/thread.h"

bool g_isUpgrading = false;

static UPGRADE_STATUS s_emUpgradeStatus = UPGRADE_STATUS_NONE;

/*****************************************************************************
  Function:     upgrade_CheckUpgrade
  Description:  校验升级文件包头，提取有效升级文件，并升级
  Input:        U8 * UpgradeFileName  (i)  升级文件名
  Output:       none
  Return:       WV_SUCCESS      -   成功
                WV_ERR_PARAMS   -   失败
  Author:       dadi.zeng
*****************************************************************************/
wvErrCode upgrade_CheckUpgrade(const U8 * UpgradeFileName)
{
    S32  s32Ret = 0;
    FILE * fp = NULL;
    ImageHeader_t  stUpgradeHeader;
    U32 u32ValidDataLen = 0;
    U8  * pu8UpgradeValidArray = NULL;
    wvErrCode enRet = WV_SUCCESS;
    U32 u32Crc = 0;
    U32 u32LocalCrc = 0;
    U8   u8Command[MAX_COMMAND_LENGTH];

    /* 参数校验 */
    if(NULL == UpgradeFileName)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Param is NULL!");
        return WV_ERR_PARAMS;
    }

    /* 清除先前升级残留文件 */
    system("rm /var/upgrade -fr");
    system("rm /var/upgrade.tar");

    /* 打开文件 */
    fp = fopen((char *)UpgradeFileName, "r+");
    if(NULL == fp)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade file [%s] open  error: %s(errno: %d)\n", UpgradeFileName,strerror(errno),errno);
        enRet =  WV_ERR_FILE_CAN_NOT_READE;
        goto upgrade_exit; 
    }

    /* 获得升级文件头部信息并校验 */ 
    s32Ret = fread(&stUpgradeHeader, 1, sizeof(ImageHeader_t), fp);
    if(s32Ret != sizeof(ImageHeader_t))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade file [%s] header can't be read error: %s(errno: %d)\n", UpgradeFileName,strerror(errno),errno);
        enRet =  WV_ERR_FILE_CAN_NOT_READE;
        goto close_upgradefile; 
    }

    if((WV_UPGRADE_SYNC_FLAG != stUpgradeHeader.SyncFlag) ||
       (WV_BOARD_TYPE != stUpgradeHeader.BoardType) ||
       (WV_MACHINE_TYPE != stUpgradeHeader.MachineType))
    {
		printf("SyncFlag = %d\n", stUpgradeHeader.SyncFlag);
		printf("BoardType = %d\n", stUpgradeHeader.BoardType);
		printf("MachineType = %d\n", stUpgradeHeader.MachineType);
		
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade file [%s] header info error!\n", UpgradeFileName);
        enRet =  WV_ERR_UPGRADE_CHECK_HEADER;
        goto close_upgradefile; 
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Upgrade file [%s] header info correct!\n", UpgradeFileName);
    }

    /* 读取有效的升级文件数据 */
    u32ValidDataLen = stUpgradeHeader.DataLength;
    pu8UpgradeValidArray = (U8 *)malloc(u32ValidDataLen);
    if(NULL == pu8UpgradeValidArray)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade file [%s] malloc error!\n", UpgradeFileName);
        enRet =  WV_ERR_MALLOC_FALIURE;
        goto close_upgradefile; 
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Upgrade file [%s] len: %d !", UpgradeFileName, u32ValidDataLen);
    }

    s32Ret = fread(pu8UpgradeValidArray, 1, u32ValidDataLen, fp);
    if(s32Ret != u32ValidDataLen)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade file [%s] valid data can't be read!\n", UpgradeFileName);
        enRet =  WV_ERR_FILE_CAN_NOT_READE;
        goto free_mem; 
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Upgrade file [%s] valid data have been read succefully!", UpgradeFileName);
    }


    /* CRC校验 */
    u32Crc = stUpgradeHeader.DataCRC32;
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Upgrade file CRC value: 0x%x!\n", u32Crc);
    
    u32LocalCrc = (U32)Common_CRCCalculate(pu8UpgradeValidArray, u32ValidDataLen,0);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Upgrade file CRC ret: 0x%x!\n", u32LocalCrc);

    enRet = (u32Crc == u32LocalCrc) ? WV_SUCCESS : WV_ERR_CRC;

    /* 清空原始升级文件,防止空间不够 */
    fclose(fp);
    fp = fopen((char *)UpgradeFileName, "w+");
    if(NULL == fp)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade file [%s] re-open fail!\n", UpgradeFileName);
        return WV_ERR_FILE_NOT_EXISTING;
    }

    /* 剥离出有效数据 */
    if(WV_SUCCESS == enRet)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Upgrade file [%s] CRC successfully!\n", UpgradeFileName);
            
        s32Ret = fwrite(pu8UpgradeValidArray, 1, u32ValidDataLen, fp);
        if(s32Ret != u32ValidDataLen)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade file [%s] valid data can't be write!\n", UpgradeFileName);
            enRet =  WV_ERR_FILE_CAN_NOT_READE;
            goto free_mem;
        }
        else
        {
            LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Upgrade file [%s] valid data can been write successfully!\n", UpgradeFileName);
        }

        /* 解压到指定位置 */
        system("mkdir /var/volatile/upgrade");
        system("ln -s /var/volatile/upgrade /var/upgrade");
        snprintf((char *)u8Command, ARRAY_SIZE(u8Command), "tar xvf %s -C /var/upgrade/", UpgradeFileName);
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "tar commdand:[%s]!\n", u8Command);
        system((char *)u8Command);

        if ( -1 == access( "/var/upgrade/zz.zz", F_OK ) )//解压的升级文件不完整,主要是防止空间不够
        {
            system("rm /var/upgrade -fr");
            system("rm /var/volatile/upgrade -fr");
            enRet = WV_ERR_TAR_UPGRADE_FILE_ERROR;
            goto free_mem;
        }

        /* 执行内部脚本 */
        system("ls -l /var/upgrade/");
        system("chmod +x /var/upgrade/run.sh");
        system("/var/upgrade/run.sh");
        sleep(3);

        /* 清理临时文件，防止多次升级产生的错误 */
        system("rm /var/upgrade -fr");
        system("rm /var/volatile/upgrade -fr");

        /* 延迟时间，防止校验过程中断电 */
        sleep(5);

        enRet = WV_SUCCESS;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade file [%s] CRC failure!\n", UpgradeFileName);
    }

free_mem:
    free(pu8UpgradeValidArray);
close_upgradefile:
    fclose(fp);   
upgrade_exit:
    return enRet;
}

/*****************************************************************************
  Function:     upgrade_DelNetHeader
  Description:  去掉升级文件包头，提取有效升级文件
  Input:        U8 * UpgradeFileName  (i)  升级文件名
  Output:       none
  Return:       WV_SUCCESS      -   成功
                WV_ERR_PARAMS   -   失败
  Author:       dadi.zeng
*****************************************************************************/
wvErrCode upgrade_DelNetHeader(const U8 * u8UpgradeFileName)
{
    wvErrCode enRet = WV_SUCCESS;
    U8 u8Line[MAX_LINE_LENGTH];
    U8 u8FirstLine[MAX_LINE_LENGTH];
    U32 u32FileSize = 0;
    U8 * pu8ValidArray = NULL;
    U32 u32LineCnt = 0;
    FILE * fp = NULL;
    struct stat stFileStatus;
    bool isFindHeader = false;
    bool isFindSpace  = false;
    bool isFindTail   = false;
    U32  u32ValidStartPosition = 0;
    U32  u32ValidEndPosition = 0;
    U32  u32ValidDataLen = 0;
    U32  u32CurPosition = 0;
    U32  u32FirstLineLen = 0;
    
    bzero((void *)u8Line, ARRAY_SIZE(u8Line));

    /* 参数校验 */
    if(NULL == u8UpgradeFileName)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Null pointer!\n");
        return WV_ERR_PARAMS;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade filename : [%s]!\n", (char *)u8UpgradeFileName);
    }
    
    /* 获得文件大小 */
    stat((char *)u8UpgradeFileName,&stFileStatus);
    u32FileSize = stFileStatus.st_size;

    /* 申请数组空间存放有效数据 */
    pu8ValidArray = (U8 *)malloc(u32FileSize);
    if(NULL == pu8ValidArray)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade file [%s] malloc failed!\n", u8UpgradeFileName);
        enRet =  WV_ERR_MALLOC_FALIURE;
        goto upgrade_exit;
    }

    /* 打开升级文件 */
    fp = fopen((char *)u8UpgradeFileName, "r+"); 
    if(NULL == fp)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade file [%s] can't open!\n", u8UpgradeFileName);
        enRet =  WV_ERR_FILE_CAN_NOT_READE;
        goto free_array;
    }
   
    
    /* 提取有效数据 */
    while(NULL != fgets((char *)u8Line, ARRAY_SIZE(u8Line), fp))
    {
        u32LineCnt++;

        if(u32LineCnt > 50)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Cant't find valid header!\n");
            break;
        }
    
        /* 获得第一行 */
        if(1 == u32LineCnt)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade original first line: [%s]!\n", u8Line);
            memcpy(u8FirstLine, u8Line, ARRAY_SIZE(u8Line));
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade record first line: [%s]!\n", u8FirstLine);
            continue;
        }

        /* 找到头部标志 */
        if(false == isFindHeader)
        {
            if(0 == strstr((char *)u8Line, "filename="))
            {
                //printf("Can't Find header:%s\n", u8Line);
                continue;
            }
            else
            {
                isFindHeader = true;   
                printf("Find header:%s\n", u8Line);
            }
        }

        /* 找到空行 */
        if(false == isFindSpace)
        {
            if(u8Line != (U8 *)strstr((char *)u8Line, "\r\n"))
            {
                //printf("Not space: [%s]", u8Line);
                continue;
            }
            else
            {
                printf("Find space: [%s]", u8Line);
                isFindSpace = true;   
                u32ValidStartPosition = ftell(fp);
                break;
            }
        }
    }

    /* 是否可读 */
    if(0 == u32LineCnt)
    {
         LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade first can't read!\n");
    }

    /* 找到头部标志后， 有效数据从连续2个 /r/n  开始，这表明有个空行，如果没找到空行，则没有找到有效数据 */
    if(false == isFindSpace)
    {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade first line: [%s]!\n", u8FirstLine);
            enRet = WV_ERR_UPGRADE_CHECK_NET_HEADER;
            goto close_file;
    }

    /* 移动文件指针查找，如果再次出现第一行，表明升级文件有效数据结束 */
    u32CurPosition = u32ValidStartPosition;
    u32FirstLineLen = strlen((char *)u8FirstLine);
    LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Position : Cur %d  Total:%d  line_len:%d\n", u32CurPosition, u32FileSize, u32FirstLineLen);
    while(u32CurPosition < u32FileSize - u32FirstLineLen)
    {
        fseek(fp, u32CurPosition, SEEK_SET);
        fread(u8Line, 1, u32FirstLineLen, fp);
        if(0 == memcmp(u8Line, u8FirstLine, u32FirstLineLen-2))
        {
            isFindTail = true;
            u32ValidEndPosition = u32CurPosition - 2;  /* 多了一个\r\n:  格式: \r\n第一样\r\n */ 
            break;
        }
        else
        {
            //printf("Not tail ... ...\n");
        }
        u32CurPosition++;        
    }
    
    if(true == isFindTail)
    {
        u32ValidDataLen = u32ValidEndPosition - u32ValidStartPosition;
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "===>Find tail line, valid len:%d!\n", u32ValidDataLen);
    }
    else
    {
       LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "===>Can't find tail line!\n");
       goto close_file;
    }

    /* 提取有效数据 */
    fseek(fp, u32ValidStartPosition, SEEK_SET);
    fread(pu8ValidArray, 1, u32ValidDataLen, fp);
    fclose(fp);

    /* 清空原先文件将有效数据写入新的文件 */
    fp = fopen((char *)u8UpgradeFileName, "w+");
    if(NULL == fp)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "===>Can't re-open file:%s\n!\n", u8UpgradeFileName);
        enRet = WV_ERR_FILE_CAN_NOT_READE;
        goto free_array;
    }
    fwrite(pu8ValidArray, 1, u32ValidDataLen, fp);
    enRet = WV_SUCCESS;

close_file:
    fclose(fp);
free_array:
    free(pu8ValidArray);
upgrade_exit:
    return enRet; 
}

/*****************************************************************************
  Function:     upgrade_DelNetHeader
  Description:  去掉升级文件包头，提取有效升级文件
  Input:        U8 * UpgradeFileName  (i)  升级文件名
  Output:       none
  Return:       WV_SUCCESS      -   成功
                WV_ERR_PARAMS   -   失败
  Author:       dadi.zeng
*****************************************************************************/
wvErrCode upgrade_UpgradeSystem(const U8 * u8UpgradeFileName)
{
    wvErrCode enRet = WV_SUCCESS;

    g_isUpgrading = true;

    /* 参数校验 */
    if(NULL == u8UpgradeFileName)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Null pointer!\n");
        g_isUpgrading = false;
        return WV_ERR_PARAMS;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Upgrade file name :[%s]!\n", u8UpgradeFileName);
    }

    /* 校验升级 */
    enRet = upgrade_CheckUpgrade(u8UpgradeFileName);
    if(WV_SUCCESS != enRet)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, " ret: %d!\n", enRet);
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, " ret: %d!\n", enRet);
    }

upgrade_out:
    /* 删除原始的升级文件 */
    unlink((char *)u8UpgradeFileName); 
    
    g_isUpgrading = false;
    

    return enRet;
}


wvErrCode upgrade_RecieveFile(const U8 * fileData, U32 u32Datalen)
{    
    FILE * fp = NULL;
    S32 s32Ret = 0;
    //int ret = 0;

    U8 fileName[40] = "/var/volatile/upgradefile";

#if 0
    ret = access(fileName, 0);
    if(ret < 0)
    {
    	printf("***file not exit\r\n");
        //file_creat(filename);
        if(creat(fileName,0755)<0)
        {
             printf("create file %s failure!\n",fileName);
        }
        else
        {
             printf("create file %s success!\n",fileName);
        }
    }

#endif
    fp = fopen((char *)fileName, "w+");
    if(NULL == fp)
    {
        //LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Upgrade file [%s] re-open fail!\n", UpgradeFileName);
    	printf("Upgrade file [%s] re-open fail!\n", fileName);
        return WV_ERR_FILE_NOT_EXISTING;
    }

   
    //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Upgrade file [%s] CRC successfully!\n", UpgradeFileName);
        printf("u32Datalen %d\r\n", u32Datalen);
    s32Ret = fwrite(fileData, 1, u32Datalen, fp);


    fclose(fp);

    return WV_SUCCESS;
}


static UpgradeInfo s_stUpgradeInfo;

#define UPGRADE_SERVER_PORT (6666)
#define UPGRADE_FILE_NAME   ("/var/volatile/upgradefile")


wvErrCode Upgrade_WriteRawDataToFile(void * pData, int data_len, const char * file_name)
{
	if(!file_name)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: param = NULL");
		return WV_ERR_PARAMS;
	}

	FILE *fp = NULL;
	wvErrCode ret = WV_SUCCESS;

	fp = fopen(file_name, "w+");
    if(NULL == fp)
    {
    	printf("Upgrade file [%s] re-open fail!\n", file_name);
        return WV_ERR_FILE_NOT_EXISTING;
    }

  
    printf("u32Datalen %d\r\n", data_len);
    fwrite(pData, 1, data_len, fp);

    fclose(fp);

	return WV_SUCCESS;
}


void * Upgrade_Recvfile(void *arg)
{
	S32 sockfd = *((int *)arg);
	U8 recv_buf[1536] = {0};
	U8 send_buf[1536] = {0};
	S32 num = 0;
	U8 u8Cmd = 0;
	void *p_Data = NULL;
	
	bool isUpgrade = false;
	char err_buf[ERR_BUF_LEN] = {0};
	U32 u32Offset = 0;
	
	while(1)
	{
		if((num = read(sockfd, recv_buf, sizeof(recv_buf))) <= 0)
		{
			if((EINTR == errno) && (num < 0))
			{
				LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: call accept failed by EINTR!");
				continue;
			}
			else if(num < 0)
			{
				ERR_STRING(err_buf);
				LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: call read: %s!", err_buf);
			}
			else 
			{
				//connection close
				CLOSE(sockfd);
				break;
			}
		}

		//申请空间		
		if(!isUpgrade)
		{
			isUpgrade = true;
			p_Data = malloc(UPGRADE_FILE_SIZE);
			if(!p_Data)
			{
				ERR_STRING(err_buf);
				LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: call malloc faild!");
				goto err;
			}
			LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Upgrade start ...");
		}

		memcpy(p_Data + u32Offset, recv_buf, num);
		u32Offset += num;
		
	}

	//将数据写入文件
	Upgrade_WriteRawDataToFile(p_Data, u32Offset, UPGRADE_FILE_NAME);
	FREE(p_Data);
	
	upgrade_UpgradeSystem(UPGRADE_FILE_NAME);
	CLOSE(sockfd);

	LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Upgrade Successfully!");
	
	return NULL;

	err:
		LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Error: Upgrade Failed!");
		CLOSE(sockfd);
		return NULL;
	
}


void Upgrade_HandleSocketConnect(int sockfd)
{
	int num = 0;
	char recv_buf[1024] = {0};
	
	while((num = read(sockfd, recv_buf, sizeof(recv_buf))) > 0)
	{
		printf("num = %d, %s\n", num, recv_buf);
	}

	printf("close sockfd\n");
}

void * Upgrade_Server(void *arg)
{
	int sockfd = -1;
	int connfd = -1;
    struct sockaddr_in ser_addr;
	char err_buf[ERR_BUF_LEN] = {0};

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        ERR_STRING(err_buf);
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: call socket %s!", err_buf);

		return NULL;
    }

    bzero(&ser_addr, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(6666);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd, &ser_addr, sizeof(ser_addr));

    listen(sockfd, 128);

	while(1)
    {
        connfd = accept(sockfd, (struct sockaddr *)NULL, NULL);
		if(connfd < 0)
		{
			if(EINTR == errno)
			{
				LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Error: call accept failed by EINTR!");
				continue;
			}
			else
			{
				ERR_STRING(err_buf);
				LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "Error: call accept %s!", err_buf);
				continue;
			}
		}
        
        THREAD_NEW_DETACH(Upgrade_Recvfile, &connfd, "Upgrade_Recvfile");
		sleep(2);
    }

	return NULL;
}



UPGRADE_STATUS Upgrade_GetUpgradeStatus(void)
{
	return s_emUpgradeStatus;
}

void Upgrade_SetUpgradeStatus(UPGRADE_STATUS emUpgradeStatus)
{
	s_emUpgradeStatus = emUpgradeStatus;
}


