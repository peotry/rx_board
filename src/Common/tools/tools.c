/******************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName tools.c
* Description : system tools
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-03-03
* Record    :
*
******************************************************************************/
#include "tools.h"
#include "WV_log.h"
#include "SIProcess.h"

/*****************************************************************************
  Function:     Common_CRCCalculate
  Description:  Calculate CRC Value
  Input:        U8 * data       -       需校验的数据句柄
                U32 length      -       数据长度
                U32 CRCValue    -       生成多项式  目前0x04C11DB7 检验效果最好，效率最高
  Output:
  Return:       success     crc值
                fail        0
  Author:       ruibin.zhang
  Modify:
*****************************************************************************/
U32 Common_CRCCalculate(U8 * data, U32 length, U32 CRCValue)
{
    if(NULL == data)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_INIT, "Param is NULL!");
        return 0;
    }

    U32 temp = 0;
    U32 byte_count=0;
    U32 bit_count=0;
    U32 crc = 0;
    U8  tempcrc,tempdata;
    U32 crc_ploy=0x04c11db7;

    if(CRCValue==0)
        crc=0xffffffff;
    else
        crc=CRCValue;
    while(byte_count<length)
    {
        tempcrc=(unsigned char) (crc>>31);
        temp=(7-bit_count);
        bit_count++;
        tempdata=(*data>>temp)&0x1;
        if ((tempdata^tempcrc)!=0)
            crc=(crc<<1^crc_ploy);
        else
            crc=crc<<1;
        if (bit_count>7)
        {
            bit_count=0;
            byte_count++;
            data++;
        }
    }

    return crc;
}


/*****************************************************************************
  Function:     Common_GetSystemAnalysis
  Description:  get system analysis
  Input:        void
  Output:
  Return:       success     1
                fail        0
  Author:       ruibin.zhang
  Modify:
*****************************************************************************/
S32 Common_GetSystemAnalysis()
{
    int cnt = 0;
    FILE *pcmd = NULL;
    char buf[1024];
    char pCPU[5] = {0};
    char pVSZ[5] = {0};
    char pSYSMEM[70] = {0};
    char pSYSCPU[75] = {0};
    char *p = NULL;

    pcmd = popen("top", "r");
    if(NULL == pcmd)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Can not popen top cmd!");
        return TOOLS_FAIL;
    }

    fread(buf, sizeof(char), sizeof(buf), pcmd);
    pclose(pcmd);

    p = strstr(buf, "Mem");
    strncpy(pSYSMEM, p, 70);
    log_printf(LOG_LEVEL_EMERGENCY, LOG_MODULE_SYS, "SYS%s", pSYSMEM);

    p = strstr(buf, "CPU");
    strncpy(pSYSCPU, p, 75);
    log_printf(LOG_LEVEL_EMERGENCY, LOG_MODULE_SYS, "SYS%s\n", pSYSCPU);

    p = strstr(buf, "4U");
    if(p != NULL)
    {
        p--;
        while(strncmp(p," ",1) != 0)
        {
            p--;
        }

        cnt = 0;
        p--;
        while(strncmp(p," ",1) != 0)
        {
            p--;
            cnt++;
        }

        strncpy(pCPU, p+1, cnt);


        while(strncmp(p," ",1) == 0)
        {
            p--;
        }
        while(strncmp(p," ",1) != 0)
        {
            p--;
        }
        while(strncmp(p," ",1) == 0)
        {
            p--;
        }

        cnt = 0;
        while(strncmp(p," ",1) != 0)
        {
            cnt++;
            p--;
        }
        strncpy(pVSZ, p+1, cnt);

    }
    log_printf(LOG_LEVEL_EMERGENCY, LOG_MODULE_SYS, "%%CPU:%s,%%VSZ:%s\n", pCPU, pVSZ);
    return TOOLS_SECCESS;
}

/*****************************************************************************
  Function:     Common_ChangeBit8To32
  Description:  transfer 1byte to 4byte
  Input:        pU8Data         --- input data
                u8Length        --- length of pU8Data
  Output:       pU32Data        --- output data
  Return:       none
  Others:
  Create:       2012.06.08
*****************************************************************************/
void Common_ChangeBit8To32(U32 *pu32Data, const U8 *pu8Data, U8 u8Length)
{
    U8 i = 0;

    if((pu32Data == NULL) || (pu8Data == NULL) || (u8Length > PACKET_LENGTH_WELLAV))
    {
        return;
    }

    for(i=0;i<u8Length/4;i++)
    {
        *pu32Data = ((*pu8Data)       << 24) |
                    ((*(pu8Data + 1)) << 16) |
                    ((*(pu8Data + 2)) <<  8) |
                    (*(pu8Data  + 3));
        pu32Data++;
        pu8Data += 4;
    }
}

