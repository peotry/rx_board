/*****************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName WV_user.c
* Description : 用户登录模块
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-28
* Record    :
*
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "WV_log.h"
#include "wv_user.h"

#ifdef RELEASE_VER
#define __func__ "USER"
#endif

ST_USER_INFO g_stUserArr[PE_MAX_USER_NUM];
U32          g_u32UserNum = 0;

/*****************************************************************************
  Function:     user_LoadUser
  Description:  把用户信息从文件导入内存
  Input:        none
  Output:       none
  Return:       WV_ERR_USER_OPEN_FILE_ERROR        --  打开文件错误
                WV_SUCCESS                         --  成功
  Author:       yaoliang.sun
*****************************************************************************/
wvErrCode user_LoadUser()
{
    FILE * fp        = NULL;
    U32    u32I      = 0;
    U8     u8TempLine[MAX_LINE_LENGTH] = { 0 };
    
    memset( g_stUserArr, 0, sizeof( g_stUserArr ) );

    fp = fopen(LOGIN_CONFIG_FILE_PATH, "r");
    if(NULL == fp)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: Open user file error!\n", __func__, __LINE__);
        return WV_ERR_USER_OPEN_FILE_ERROR;
    }

    while(!feof(fp))
    {
        memset((char*)u8TempLine, 0, ARRAY_SIZE(u8TempLine));
        fgets((char *)u8TempLine, ARRAY_SIZE(u8TempLine), fp);
        
        if ( ( NULL != strstr((char *)u8TempLine, ":" ) ) && ( u32I < PE_MAX_USER_NUM ) )
        {
            sscanf( (char*)u8TempLine, "%[^:]:%s", g_stUserArr[u32I].u8UserName, g_stUserArr[u32I].u8UserPasswd );
            LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_USER, "[%s]%d: Name = %s Passwd = %s!\n", __func__, __LINE__, 
               g_stUserArr[u32I].u8UserName, g_stUserArr[u32I].u8UserPasswd );
            g_stUserArr[u32I].isEnable = true;
            u32I++;
        }
    }

    g_u32UserNum = u32I;
    
    fclose(fp);
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     user_SaveUser
  Description:  把用户信息报错到文件
  Input:        none
  Output:       none
  Return:       WV_ERR_USER_OPEN_FILE_ERROR        --  打开文件错误
                WV_SUCCESS                         --  成功
  Author:       yaoliang.sun
*****************************************************************************/
wvErrCode user_SaveUser()
{
    FILE * fp        = NULL;
    U32    u32I      = 0;
    U8     u8TempLine[MAX_LINE_LENGTH] = { 0 };
    
    fp = fopen(LOGIN_CONFIG_FILE_PATH, "w");
    if(NULL == fp)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: Open user file error!\n", __func__, __LINE__);
        return WV_ERR_USER_OPEN_FILE_ERROR;
    }

    for( u32I = 0; u32I < g_u32UserNum; u32I++ )
    {
        if ( !g_stUserArr[u32I].isEnable )
        {
            continue;
        }
        sprintf( (char*)u8TempLine, "%s:%s\n", g_stUserArr[u32I].u8UserName, g_stUserArr[u32I].u8UserPasswd );
        fwrite( u8TempLine, 1, strlen((char*)u8TempLine), fp);
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_USER, "[%s]%d: Line = %s\n", __func__, __LINE__, u8TempLine );
    }
    
    fclose(fp);
    
    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     user_CheckUser
  Description:  检测web登陆用户是否存在于配置文件中
  Input:        const U8 *  pu8UserName        (i)   用户名
                const U8 *  pu8PassWord        (i)   登陆密码
  Output:       none
  Return:       WV_ERR_USER_OPEN_FILE_ERROR        --  打开文件错误
                WV_ERR_USER_LEN_ERROR              --  用户名密码长度错误
                WV_ERR_USER_PWD_ERR                --  用户密码错误
                WV_ERR_USER_NOT_EXIST              --  用户不存在
                WV_SUCCESS                         --  成功
  Author:       dadi.zeng
*****************************************************************************/
wvErrCode user_CheckUser(const U8 * pu8UserName , const U8 * pu8PassWord)
{
    wvErrCode ret      = WV_SUCCESS;
    S32       s32Index = 0;

    ret = user_LoadUser();
    if ( WV_SUCCESS != ret )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: Load user file error!\n", __func__, __LINE__);
        return ret;
    }
    
    if( (NULL == pu8UserName) ||
        (NULL == pu8PassWord) ||
        (strlen((char *)pu8UserName) > MAX_LOGIN_NAME_LEN)  ||
        (strlen((char *)pu8UserName) < MIN_LOGIN_NAME_LEN)  ||
        (strlen((char *)pu8PassWord) > MAX_LOGIN_PASSWD_LEN)||
        (strlen((char *)pu8PassWord) < MIN_LOGIN_PASSWD_LEN) )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: User Name or passwd len error!\n", __func__, __LINE__);
        return WV_ERR_USER_LEN_ERROR;
    }

    //printf( "................\n" );
    
    //system("reboot");
    //wdt_DisableFeedWatchdog();
    
    s32Index = user_FindUser(pu8UserName);

    if ( s32Index < 0 )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: User not exist!\n", __func__, __LINE__);
        return WV_ERR_USER_NOT_EXIST;
    }

    if ( 0 == strcmp( (char*)g_stUserArr[s32Index].u8UserPasswd, (char*)pu8PassWord ) )
    {
        return WV_SUCCESS;
    }

    return WV_ERR_USER_PWD_ERR;
}


/*****************************************************************************
  Function:     user_AddUser
  Description:  添加用户
  Input:        const U8 *  pu8UserName        (i)   用户名
                const U8 *  pu8PassWord        (i)   登陆密码
  Output:       none
  Return:       WV_ERR_USER_OPEN_FILE_ERROR        --  打开文件错误
                WV_ERR_USER_LEN_ERROR              --  用户名密码长度错误
                WV_ERR_USER_FULL                   --  用户量达到上限
                WV_ERR_USER_EXIST                  --  添加的用户已经存在
                WV_SUCCESS                         --  成功
  Author:       yaoliang.sun
*****************************************************************************/
wvErrCode user_AddUser(const U8 * pu8UserName , const U8 * pu8PassWord)
{
    wvErrCode ret = WV_SUCCESS;

    ret = user_LoadUser();
    if ( WV_SUCCESS != ret )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: Load user file error!\n", __func__, __LINE__);
        return ret;
    }
    
    if( (NULL == pu8UserName) ||
        (NULL == pu8PassWord) ||
        (strlen((char *)pu8UserName) > MAX_LOGIN_NAME_LEN)  ||
        (strlen((char *)pu8UserName) < MIN_LOGIN_NAME_LEN)  ||
        (strlen((char *)pu8PassWord) > MAX_LOGIN_PASSWD_LEN)||
        (strlen((char *)pu8PassWord) < MIN_LOGIN_PASSWD_LEN) )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: User Name or passwd len error!\n", __func__, __LINE__);
        return WV_ERR_USER_LEN_ERROR;
    }

    if ( g_u32UserNum >= PE_MAX_USER_NUM )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: User is full!\n", __func__, __LINE__);
        return WV_ERR_USER_FULL;
    }

    if ( user_FindUser(pu8UserName) >= 0 )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: User exist!\n", __func__, __LINE__);
        return WV_ERR_USER_EXIST;
    }

    strcpy( (char*)g_stUserArr[g_u32UserNum].u8UserName, (char*)pu8UserName );
    strcpy( (char*)g_stUserArr[g_u32UserNum].u8UserPasswd, (char*)pu8PassWord );
    g_stUserArr[g_u32UserNum].isEnable = true;
        
    g_u32UserNum++;

    ret = user_SaveUser();
    if ( WV_SUCCESS != ret )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: Save user file error!\n", __func__, __LINE__);
        return ret;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     user_ModifyUser
  Description:  修改密码
  Input:        const U8 *  pu8UserName        (i)   用户名
                const U8 *  pu8PassWord        (i)   登陆密码
  Output:       none
  Return:       WV_ERR_USER_OPEN_FILE_ERROR        --  打开文件错误
                WV_ERR_USER_LEN_ERROR              --  用户名密码长度错误
                WV_ERR_USER_NOT_EXIST              --  用户不存在
                WV_SUCCESS                         --  成功
  Author:       yaoliang.sun
*****************************************************************************/
wvErrCode user_ModifyUser(const U8 * pu8UserName , const U8 * pu8PassWord)
{
    S32 s32Index = 0;

    wvErrCode ret = WV_SUCCESS;

    ret = user_LoadUser();
    if ( WV_SUCCESS != ret )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: Load user file error!\n", __func__, __LINE__);
        return ret;
    }

    printf( "len = %d\n", strlen((char *)pu8PassWord) );
    
    if( (NULL == pu8UserName) ||
        (NULL == pu8PassWord) ||
        (strlen((char *)pu8UserName) > MAX_LOGIN_NAME_LEN)  ||
        (strlen((char *)pu8UserName) < MIN_LOGIN_NAME_LEN)  ||
        (strlen((char *)pu8PassWord) > MAX_LOGIN_PASSWD_LEN)||
        (strlen((char *)pu8PassWord) < MIN_LOGIN_PASSWD_LEN) )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: User Name or passwd len error!\n", __func__, __LINE__);
        return WV_ERR_USER_LEN_ERROR;
    }

    s32Index = user_FindUser(pu8UserName);

    if ( s32Index < 0 )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: User not exist!\n", __func__, __LINE__);
        return WV_ERR_USER_NOT_EXIST;
    }

    strcpy( (char*)g_stUserArr[s32Index].u8UserPasswd, (char*)pu8PassWord );

    ret = user_SaveUser();
    if ( WV_SUCCESS != ret )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: Save user file error!\n", __func__, __LINE__);
        return ret;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     user_DelUser
  Description:  删除用户
  Input:        const U8 * pu8UserName    (i)  用户名
  Output:       none
  Return:       WV_ERR_USER_OPEN_FILE_ERROR        --  打开文件错误
                WV_ERR_USER_NOT_EXIST              --  用户不存在
                WV_SUCCESS                         --  成功
  Author:       yaoliang.sun
*****************************************************************************/
wvErrCode user_DelUser(const U8 * pu8UserName)
{
    S32 s32Index = 0;

    wvErrCode ret = WV_SUCCESS;

    ret = user_LoadUser();
    if ( WV_SUCCESS != ret )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: Load user file error!\n", __func__, __LINE__);
        return ret;
    }

    s32Index = user_FindUser(pu8UserName);

    if ( s32Index < 0 )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: User not exist!\n", __func__, __LINE__);
        return WV_ERR_USER_NOT_EXIST;
    }

    g_stUserArr[s32Index].isEnable = false;

    ret = user_SaveUser();
    if ( WV_SUCCESS != ret )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: Save user file error!\n", __func__, __LINE__);
        return ret;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     user_FindUser
  Description:  查找用户
  Input:        const U8 * pu8UserName    (i)  用户名
  Output:       none
  Return:       -1        --  查找失败
                                                其他      --  用户下标
  Author:       yaoliang.sun
*****************************************************************************/
S32 user_FindUser(const U8 * pu8UserName)
{
    U32 u32I = 0;

    if ( NULL == pu8UserName )
    {
        return -1;
    }

    for ( u32I = 0; u32I < g_u32UserNum; u32I++ )
    {
        if ( 0 == strcmp( (char*)pu8UserName, (char*)g_stUserArr[u32I].u8UserName ) )
        {
            return u32I;
        }
    }

    return -1;
}

/*****************************************************************************
  Function:     user_ShowUser
  Description:  查找用户
  Input:        ST_USER_INFO* pStUserArry    (o)  用户名
                U32 u32MaxNum                (i)  获取用户的上限
                U32 pU32UserNum              (o)  实际用户的个数
  Output:       none
  Return:       WV_ERR_USER_OPEN_FILE_ERROR        --  打开文件错误
                WV_ERR_FAILURE                     --  参数错误
                WV_SUCCESS                         --  成功
  Author:       yaoliang.sun
*****************************************************************************/
wvErrCode user_ShowUser(ST_USER_INFO* pStUserArry, U32 u32MaxNum, U32* pU32UserNum )
{
    wvErrCode ret = WV_SUCCESS;

    ret = user_LoadUser();
    if ( WV_SUCCESS != ret )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: Load user file error!\n", __func__, __LINE__);
        return ret;
    }
    
    if ( NULL == pStUserArry )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: NULL Pointer!\n", __func__, __LINE__);
        return WV_ERR_PARAMS;
    }

    if ( u32MaxNum < g_u32UserNum )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: Params error!\n", __func__, __LINE__);
        return WV_ERR_PARAMS;
    }
    
    memcpy( pStUserArry, g_stUserArr, sizeof( g_stUserArr ) );
    *pU32UserNum = g_u32UserNum;

    ret = user_SaveUser();
    if ( WV_SUCCESS != ret )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_USER, "[%s]%d: Save user file error!\n", __func__, __LINE__);
        return ret;
    }
    
    return WV_SUCCESS;
}
