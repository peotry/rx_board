/*****************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName WV_user.h
* Description : 用户登录模块
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-28
* Record    :
*
*****************************************************************************/
#ifndef WV_USER_H_
#define WV_USER_H_

#include <stdbool.h>
#include "AppGlobal.h"


#define PE_MAX_USER_NUM         10

#define MAX_LOGIN_NAME_LEN      20
#define MIN_LOGIN_NAME_LEN      2
#define MAX_LOGIN_PASSWD_LEN    20
#define MIN_LOGIN_PASSWD_LEN    2

#define LOGIN_CONFIG_FILE_PATH  WELLAV_DATA"auth.user"//WELLAV_APP"tools/lighttpd/config/lighttpd.user"


typedef struct
{
    U8   u8UserName[MAX_LOGIN_NAME_LEN+1];
    U8   u8UserPasswd[MAX_LOGIN_PASSWD_LEN+1];
    bool isEnable;
} ST_USER_INFO;

S32       user_FindUser(const U8 * pu8UserName);
wvErrCode user_LoadUser();
wvErrCode user_SaveUser();
wvErrCode user_CheckUser(const U8 * pu8UserName , const U8 * pu8PassWord);
wvErrCode user_AddUser(const U8 * pu8UserName , const U8 * pu8PassWord);
wvErrCode user_ModifyUser(const U8 * pu8UserName , const U8 * pu8PassWord);
wvErrCode user_DelUser(const U8 * pu8UserName);
wvErrCode user_ShowUser(ST_USER_INFO* pStUserArry, U32 u32MaxNum, U32* pU32UserNum );

#endif

