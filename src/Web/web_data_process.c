#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "AppGlobal.h"

//web
#include "web_data_process.h"
#include "Web_main_process.h"

//others
#include "web_function.h"
#include "UNICODE_Language.h"

//lighttpd
#include "base.h"

//json
#include "cJSON.h"

#include "tuner.h"
#include "BoardMngr.h"
#include "TSProcess.h"
#include "TSConstruct.h"
#include "WV_CI.h"

//获取指针数组的长度
#define ARP_LEN(x)   x , sizeof(x) / sizeof(x[0])

extern S8 * g_ps8RevData;                         //post数据             

/****************************************************************************
 *
 * 名字:
 *      web_getValueByNameFromJson
 *
 * 目标:
 *      从JSON中获取满足条件的数据
 *
 * 用法：
 *
 * 返回值：       
 *      cJSON * -- 数据   
 *
 * 作者:
 *      zhouhuan in 2014-7-19
 *
 ****************************************************************************/
static cJSON * web_getValueByNameFromJson(cJSON * pIn , char * arpNameList[] , int nDeep )
{
    int     nCount  = 0;        
    cJSON * pLoop   = pIn;
    
    while ( pLoop && (nCount < nDeep) )
    {
        pLoop = cJSON_GetObjectItem(pLoop , arpNameList[nCount++]);
    }
    
    return pLoop;
}

static U8 s_aru8ServiceName[UTF8_STRING_MAX_LENGTH + 1] = {0};

/********************************************************************
Function:     web_StringConvertToUTF8
Description:  String Convert To UTF-8
Input:        pcSrc
Output:
Return:       pointer to s_aru8ServiceName
********************************************************************/
const U8 *web_StringConvertToUTF8(const U8 *pu8Src, U32 u32SrcLen)
{
    U32 i = 0;
    U32 j = 0;
    U32 u32Len = 0;
    U16 aru16Name[UTF8_STRING_MAX_LENGTH + 1] = {0};
    U8  aru8Name[UTF8_STRING_MAX_LENGTH * 3]  = {0};
    U8  u8TSStandard = 0;//默认为DVB

    memset(s_aru8ServiceName, 0, sizeof(s_aru8ServiceName));

    if (NULL == pu8Src || 0 == u32SrcLen)
    {
        return s_aru8ServiceName;
    }

    if (SI_MODE_ATSC == u8TSStandard)
    {
        /*
        ---------------------------------------------------------------------------------
        ATSC Standard:
        Program and System Information Protocol for Terrestrial Broadcast and Cable(PSIP)
        ---------------------------------------------------------------------------------
        Virtual Channel Table (VCT)
        short_name:
        The name of the virtual channel, represented as a sequence of one to seven 16-bit
        code values interpreted in accordance with the UTF-16 representation of Unicode character
        data. If the length of the name requires fewer than seven 16-bit code values, this field shall be
        padded out to seven 16-bit code values using the Unicode NUL character (0x0000). Unicode
        character data shall conform to The Unicode Standard, Version 3.0 [13]. The UTF-16
        representation of Unicode character data is in accordance with that defined by [13], which is
        identical to that defined by ISO/IEC 10646-1:2000 Annex C [10]
        ---------------------------------------------------------------------------------
        */

        u32Len = (UTF8_STRING_MAX_LENGTH < u32SrcLen) ? UTF8_STRING_MAX_LENGTH : u32SrcLen;
        memcpy(aru8Name, pu8Src, u32Len);
    }
    else
    {
        /*
        ---------------------------------------------------------------------------------------------------------------------------------------
        First byte value | Character code table                   | Table description
        ---------------------------------------------------------------------------------------------------------------------------------------
        0x01             |  ISO/IEC 8859-5 [27]                   |  Latin/Cyrillic alphabet
        0x02             |  ISO/IEC 8859-6 [28]                   |  Latin/Arabic alphabet
        0x03             |  ISO/IEC 8859-7 [29]                   |  Latin/Greek alphabet
        0x04             |  ISO/IEC 8859-8 [30]                   |  Latin/Hebrew alphabet
        0x05             |  ISO/IEC 8859-9 [31]                   |  Latin alphabet No. 5
        0x06             |  ISO/IEC 8859-10 [32]                  |  Latin alphabet No. 6
        0x07             |  ISO/IEC 8859-11 [33]                  |  Latin/Thai (draft only)
        0x08             |  reserved for future use (see note)    |
        0x09             |  ISO/IEC 8859-13 [34]                  |  Latin alphabet No. 7
        0x0A             |  ISO/IEC 8859-14 [35]                  |  Latin alphabet No. 8 (Celtic)
        0x0B             |  ISO/IEC 8859-15 [36]                  |  Latin alphabet No. 9
        0x0C to 0x0F     |  reserved for future use               |
        0x10             |  ISO/IEC 8859  See table A.4           |
        0x11             |  ISO/IEC 10646 [16]                    |  Basic Multilingual Plane (BMP)
        0x12             |  KSX1001-2004 [44]                     |  Korean Character Set
        0x13             |  GB-2312-1980 [58]                     |  Simplified Chinese Character
        0x14             |  Big5 subset of ISO/IEC 10646 [16]     |  Traditional Chinese
        0x15             |  UTF-8 encoding of ISO/IEC 10646 [16]  |  Basic Multilingual Plane (BMP)
        0x16 to 0x1E     |  reserved for future use               |
        0x1F             |  Described by encoding_type_id         |  Described by 8 bit encoding_type_id conveyed in second byte of the string
        ---------------------------------------------------------------------------------------------------------------------------------------
        NOTE:   The first byte value 0x08 was previously reserved for ISO/IEC 8859-12 which should have covered the
                Devanagri script. ISO/IEC however never published part 12 of the ISO/IEC 8859 series (see
                http://en.wikipedia.org/wiki/ISO/IEC_8859-12). Applications of the current document for the scripts used
                by South-Asian languages should use the BMP of ISO/IEC 10646 [16], where appropriate glyphs are provided.
        ---------------------------------------------------------------------------------------------------------------------------------------
        */


        if ((*pu8Src <= 0x0B) || (0x10 == *pu8Src)) // ISO/IEC 8859
        {
            /* Convert_8859_to_U utf16_to_utf8 , 可能存在某些情况考虑不周 */
            memcpy(s_aru8ServiceName, pu8Src, UTF8_STRING_MAX_LENGTH);
            Convert_8859_to_U(s_aru8ServiceName, aru16Name);
            (void)utf16_to_utf8((unsigned short*)aru16Name, (char*)aru8Name);
            u32Len = strlen((char*)aru8Name);
        }
        else if (0x11 == *pu8Src)   // ISO/IEC 10646 Basic Multilingual Plane || Big5 subset of ISO/IEC 10646
        {
            memcpy(aru16Name, pu8Src + 1, u32SrcLen - 1);
            for (i = 0; i < UTF8_STRING_MAX_LENGTH; i++)
            {
                aru16Name[i] = ntohs(aru16Name[i]);
            }
            (void)UCS2_to_UTF8(aru16Name, aru8Name);
            u32Len = strlen((char*)aru8Name);
        }
        /*
        else if (0x12 == *pu8Src) // Korean Character Set
        {
            // to do ...
        }
        */
        else if (0x13 == *pu8Src) // GB-2312-1980 [58] Simplified Chinese
        {
            memcpy(s_aru8ServiceName, pu8Src + 1, UTF8_STRING_MAX_LENGTH);
            (void)ConvertGBToUTF16(s_aru8ServiceName, aru16Name);
            (void)utf16_to_utf8((unsigned short*)aru16Name, (char*)aru8Name);
            u32Len = strlen((char*)aru8Name);
        }
        else if (0x14 == *pu8Src) // big5
        {
            //memcpy(s_aru8ServiceName, pu8Src + 1, UTF8_STRING_MAX_LENGTH);
            //(void)ConvertBIG5ToUTF16(s_aru8ServiceName,aru16Name);
            //(void)utf16_to_utf8((unsigned short*)aru16Name, (char*)aru8Name);
            //u32Len = strlen((char*)aru8Name);

            memcpy(aru16Name, pu8Src + 1, u32SrcLen - 1);
            for (i = 0; i < UTF8_STRING_MAX_LENGTH; i++)
            {
                aru16Name[i] = ntohs(aru16Name[i]);
            }

            //printf("before UCS2_to_UTF8 u32Len = %d \n",u32SrcLen-1);
            (void)UCS2_to_UTF8(aru16Name, aru8Name);
            u32Len = strlen((char*)aru8Name);
            //printf("after UCS2_to_UTF8 u32Len = %d \n",u32Len);
        }
        else if (0x15 == *pu8Src)
        {
            #if 1
                u32Len = (UTF8_STRING_MAX_LENGTH < u32SrcLen - 1) ? UTF8_STRING_MAX_LENGTH : u32SrcLen - 1;
                memcpy(aru8Name, pu8Src + 1, u32Len);   
            #else
                memcpy(aru16Name, pu8Src + 1, u32SrcLen - 1);
                
                for (i = 0; i < UTF8_STRING_MAX_LENGTH; i++)
                {
                    aru16Name[i] = ntohs(aru16Name[i]);
                }
                
                (void)UCS2_to_UTF8(aru16Name, aru8Name);

                u32Len = strlen((char*)aru8Name);
            #endif
        }
        /* 判断存在失准 */
        else if (IsGBSimplifiedChinese(pu8Src)) // Simplified Chinese
        {
            memcpy(s_aru8ServiceName, pu8Src, UTF8_STRING_MAX_LENGTH);
            (void)ConvertGBToUTF16(s_aru8ServiceName, aru16Name);
            (void)utf16_to_utf8((unsigned short*)aru16Name, (char*)aru8Name);
            u32Len = strlen((char*)aru8Name);
        }
        else
        {
            u32Len = (MAX_SDT_NAME_LENGTH < u32SrcLen) ? MAX_SDT_NAME_LENGTH : u32SrcLen;
            memcpy(aru8Name, pu8Src, u32Len);
        }
    }

    memset(s_aru8ServiceName, 0, sizeof(s_aru8ServiceName));
    
    for (i = 0 , j = 0; i < u32Len; i++)
    {
        if (UTF8_STRING_MAX_LENGTH == j || 0 == aru8Name[i])
        {
            break;
        }

        if (0 != iscntrl(aru8Name[i]))
        {
            /* 将导致显示异常的 (control Character) 替换为 (space) */
            s_aru8ServiceName[j++] = 0x20;
        }
        else
        {
            s_aru8ServiceName[j++] = aru8Name[i];
        }
    }

    return s_aru8ServiceName;
}

/********************************************************************
Function:     web_UTF8ToServiceName
Description:  UTF8ToServiceName
Input:        pcSrc
Output:
Return:       pointer to s_aru8ServiceName
********************************************************************/
const U8 *web_UTF8ToServiceName(const U8 *pu8Src, U32 u32SrcLen)
{
    U32  i = 0;
    U16  aru16WString[UTF8_STRING_MAX_LENGTH + 1] = {0};
    bool bOnlyBasicLatinCharacter = true;
    U8   u8FirstByteValue = 0x15;
    U8   u8TSStandard = 0;
    int  nLen = 0;

    memset(s_aru8ServiceName, 0, sizeof(s_aru8ServiceName));

    if (NULL == pu8Src || 0 == u32SrcLen)
    {
        return s_aru8ServiceName;
    }

    if (SI_MODE_ATSC == u8TSStandard)
    {
        goto OnlyBasicLatinCharacter;
    }

    for (i = 0; i < u32SrcLen; i++)
    {
        if (pu8Src[i] < 0x20 || 0x7f < pu8Src[i])
        {
            bOnlyBasicLatinCharacter = false;
            break;
        }
    }

    if (bOnlyBasicLatinCharacter)
    {
        goto OnlyBasicLatinCharacter;
    }

    if (UTF82Unicode(pu8Src, aru16WString))
    {
        if (IsUnicodeCyrillic(aru16WString))
        {
            u8FirstByteValue = 0x01;
        }
        else if (IsUnicodeSimplifiedChinese(aru16WString))
        {
            u8FirstByteValue = 0x13;
        }
        /*
        
        TO DO : 8859-6/7/8/9/10/11/12/13/14/15, 
                (Big5 subset of) ISO/IEC 10646 [16]
                KSX1001-2004
                GB-2312-1980
        */
    }

    s_aru8ServiceName[0] = u8FirstByteValue;

    switch (u8FirstByteValue)
    {
        case 0x01:  // ISO/IEC 8859-5 [27] Latin/Cyrillic alphabet
            (void)Convert_U_to_8859_5(aru16WString, UTF8_STRING_MAX_LENGTH, s_aru8ServiceName + 1);
            break;
        /*
            TO DO: 0x02 ~ 0x14
        */
        case 0x13: // GB-2312-1980 [58]  Simplified Chinese Character
            (void)Convert_U_to_GB(aru16WString, UTF8_STRING_MAX_LENGTH, s_aru8ServiceName + 1);
            break;
        case 0x14:            
            //(void)Convert_U_to_BIG5(aru16WString, UTF8_STRING_MAX_LENGTH, s_aru8ServiceName + 1);            
            //printf("before Convert_UTF8_to_U_TaiWan u32Len = %d \n",u32SrcLen);
            (void)Convert_UTF8_to_U_TaiWan(pu8Src,u32SrcLen,aru16WString,&nLen);
            memcpy(s_aru8ServiceName + 1, aru16WString, UTF8_STRING_MAX_LENGTH);            
            break;
        case 0x15: // UTF-8 encoding of ISO/IEC 10646 [16]  Basic Multilingual Plane (BMP)
        default:
            memcpy(s_aru8ServiceName + 1, pu8Src, (u32SrcLen < UTF8_STRING_MAX_LENGTH - 2) 
                ? u32SrcLen : UTF8_STRING_MAX_LENGTH - 2);
            break;
    }
    
    return s_aru8ServiceName;

OnlyBasicLatinCharacter:

    memset(s_aru8ServiceName, 0, sizeof(s_aru8ServiceName));
    memcpy(s_aru8ServiceName, pu8Src, (u32SrcLen < UTF8_STRING_MAX_LENGTH - 1) 
        ? u32SrcLen : UTF8_STRING_MAX_LENGTH - 1);
    
    return s_aru8ServiceName;
}



/****************************************************************************
 *
 * 名字:
 *      web_processErr
 *
 * 目标:
 *      带错误码的Response 
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      zhouhuan in 2014-7-19
 *
 ****************************************************************************/
wvErrCode web_processErr(buffer * pstWriteBuff , wvErrCode eErrCode , char * pRequest)
{
    cJSON    * pRoot           = NULL;    
    char     * pResponseString = NULL;
    U8       * pu8String       = NULL;

    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    }
    
    //组装数据
    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", eErrCode);

    pu8String = web_GetErrCodeString(eErrCode);
    if(NULL == pu8String)
    {
        cJSON_AddNullToObject(pRoot,"description");
    }
    else
    {
        cJSON_AddStringToObject(pRoot, "description", (char *)pu8String);
    }
    
    pResponseString = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot); 

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);
    
    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;
    
    return WV_SUCCESS;
}

/****************************************************************************
 *
 * 名字:
 *      web_processSuccess
 *
 * 目标:
 *      成功的Response
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      zhouhuan in 2014-7-19
 *
 ****************************************************************************/
wvErrCode web_processSuccess(buffer * pstWriteBuff)
{
    cJSON           * pRoot           = NULL;
    char            * pResponseString = NULL;

    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    }
    
    //组装数据
    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", WV_SUCCESS);
    
    cJSON_AddNullToObject(pRoot, "description");
    
    pResponseString = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot); 

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);
    
    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;
    
    return WV_SUCCESS;
}

/****************************************************************************
 *
 * 名字:
 *      web_processReadReg
 *
 * 目标:
 *      读取寄存器的值
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      zhouhuan in 2017-3-2
 *
 ****************************************************************************/
wvErrCode web_processReadReg(buffer * pstWriteBuff)
{
    cJSON           * pJson           = NULL;
    cJSON           * pRoot           = NULL;
    cJSON           * pTemp           = NULL;
    cJSON           * pDate           = NULL;
    char            * pResponseString = NULL;
    wvErrCode         eErrCode        = WV_SUCCESS;
    U32               u32Address      = 0;
    U32               u32Data         = 0;

    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    }

    //解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }    

    //寄存器地址
    pTemp = cJSON_GetObjectItem(pJson , "address");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
         WEB_LOG("Address = %f",pTemp->valuedouble);
         u32Address = (U32)pTemp->valuedouble;
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    //调用寄存器读取的接口
    web_REG_Read_fortest(u32Address, &u32Data);

    cJSON_Delete(pJson);

    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", eErrCode);
    
    cJSON_AddNullToObject(pRoot, "description");
    
    cJSON_AddItemToObject(pRoot, "data" , pDate = cJSON_CreateObject()); 
    
    //当前获取到的值
    cJSON_AddNumberToObject(pDate , "value" , u32Data);

    pResponseString = cJSON_Print(pRoot); 
    cJSON_Delete(pRoot);

    buffer_copy_string(pstWriteBuff, pResponseString);

    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;    
    
    return WV_SUCCESS;
}

/****************************************************************************
 *
 * 名字:
 *      web_processWriteReg
 *
 * 目标:
 *      设置寄存器的值
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      zhouhuan in 2017-3-2
 *
 ****************************************************************************/
wvErrCode web_processWriteReg(buffer * pstWriteBuff)
{
    cJSON           * pJson           = NULL;
    cJSON           * pTemp           = NULL;
    U32               u32Address      = 0;
    U32               u32Value        = 0;
    
    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    }

    //解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }    

    //寄存器地址
    pTemp = cJSON_GetObjectItem(pJson , "address");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
         WEB_LOG("Address = %f",pTemp->valuedouble);
         u32Address = (U32)pTemp->valuedouble;
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    //设置的值
    pTemp = cJSON_GetObjectItem(pJson , "value");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
         u32Value = (U32)pTemp->valuedouble;
         WEB_LOG("valuedouble = %f,u32Value = %u",pTemp->valuedouble, u32Value);
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    //调用写寄存器的接口
    web_REG_Write_fortest(u32Address, u32Value);

    cJSON_Delete(pJson);
    
    return web_processSuccess(pstWriteBuff);
}

/****************************************************************************
 *
 * 名字:
 *      web_processGetSubboardDetail
 *
 * 目标:
 *      读取子板详情
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      xiazhi.zhang in 2017-4-13
 *
 ****************************************************************************/
wvErrCode web_processGetSubboardDetail(buffer * pstWriteBuff)
{    
    cJSON              * pJson           = NULL;
    cJSON              * pTemp           = NULL;
    cJSON              * pRoot           = NULL; 
    cJSON              * pData           = NULL;
    char               * arpToken[]      = {"token"};
    char               * pResponseString = NULL;
    U8                aru8BoardName[20]  = {0};
    U32                  u32BoardSize    = 20;
    char               * pTempBoardName  = NULL;

    // 入参判断
    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    } 

    // 解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取授权码
    pTemp = web_getValueByNameFromJson(pJson , ARP_LEN(arpToken));
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("token = %s.",pTemp->valuestring);
         //这里要判断授权码的有效性 
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取数据完毕
    cJSON_Delete(pJson);

    web_GetSelfBoardDescriptor(aru8BoardName);
    
    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", WV_SUCCESS);

    cJSON_AddNullToObject(pRoot, "description");

    cJSON_AddItemToObject(pRoot, "data", pData = cJSON_CreateObject());

    cJSON_AddNumberToObject(pData, "boardnumber", 0);

    pTempBoardName = (char *)web_StringConvertToUTF8(aru8BoardName,u32BoardSize);

    cJSON_AddStringToObject(pData, "boardname",pTempBoardName);

    pResponseString = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot); 

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);
    
    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;
    
    return WV_SUCCESS;
}


/****************************************************************************
 *
 * 名字:
 *      web_processGetSubboardTSInfo
 *
 * 目标:
 *      获取子板流信息
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-6
 *
 ****************************************************************************/
wvErrCode web_processGetSubboardTSInfo(buffer * pstWriteBuff)
{
    cJSON           * pJson           = NULL;
    cJSON           * pTemp           = NULL;
    cJSON           * pRoot           = NULL;
    cJSON           * pTSInfo         = NULL;
    cJSON           * pTSList         = NULL;
    cJSON           * pTSItem         = NULL;
    cJSON           * pDestList       = NULL;
    cJSON           * pDestItem       = NULL;  
    cJSON           * pProgramList    = NULL;
    cJSON           * pProgramItem    = NULL;
    cJSON           * pProgramDestList= NULL;
    cJSON           * pProgramDestItem= NULL;
    const TSInfo    * pstParamTS      = NULL;
    Dest              stDest;       
    DestInfo        * pDestInfo       = NULL;
    IndexInfoList     stIndexInfoList;
    U32               i               = 0;
    U32               j               = 0;
    U32               k               = 0;
    U16               u16InTSIndex    = 0;
    wvErrCode         eErrCode        = WV_SUCCESS;
    char            * pResponseString = NULL;

    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;
    }

    //解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }    

    //获取授权码
    pTemp = cJSON_GetObjectItem(pJson , "token");
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("token = %s.",pTemp->valuestring);
         //这里要判断授权码的有效性
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取数据完毕
    cJSON_Delete(pJson);

    pstParamTS = (const TSInfo *)web_GetTSPramaHandle();
    if (NULL == pstParamTS)
    {
        WEB_LOG("web_GetTSPrama return null!");
        return WV_ERR_WEB_BAD_PARAM;
    }

    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", WV_SUCCESS);

    cJSON_AddNullToObject(pRoot, "description");

    cJSON_AddItemToObject(pRoot, "data", pTSInfo = cJSON_CreateObject());

    cJSON_AddItemToObject(pTSInfo, "tslist", pTSList = cJSON_CreateArray());

    web_TSPMutexLock();

    for ( i = 0; i < MAX_INPUT_CHANNEL_NUM; i++ )
    {

        // 获取Input TS Index
        eErrCode = (wvErrCode)web_GetInTSIndex(i, (TSInfo *)pstParamTS, &u16InTSIndex);
        if(WV_SUCCESS != eErrCode)
        {
            WEB_LOG("web_GetInTSIndex return 0x%x!", eErrCode);
            continue;
        }

        cJSON_AddItemToArray(pTSList, pTSItem = cJSON_CreateObject());

        cJSON_AddNumberToObject(pTSItem, "port", i);

        cJSON_AddNumberToObject(pTSItem, "channel", 0);

        // 获取TS Dest Info
        memset(&stDest , 0x00, sizeof(Dest));
        eErrCode = web_GetTSDestInfo(u16InTSIndex, (TSInfo *)pstParamTS, &stDest);
        if(WV_SUCCESS != eErrCode)
        {
            WEB_LOG("web_GetTSDestInfo return 0x%x!", eErrCode);
            cJSON_Delete(pRoot);
            goto web_processGetSubboardTSInfo_WEB_ERROR;
        }
        
        cJSON_AddNumberToObject(pTSItem, "destboardnumber", stDest.u8DestSlotNum);

        cJSON_AddItemToObject(pTSItem, "destboardlist", pDestList = cJSON_CreateArray());

        // 遍历slot
        for ( j = 0; j < stDest.u8DestSlotNum; j++ )
        {
            pDestInfo = &stDest.arstDestSlotInfo[j];

            cJSON_AddItemToArray(pDestList, pDestItem = cJSON_CreateObject());
            
            cJSON_AddNumberToObject(pDestItem, "board", pDestInfo->u8Slot);
            
            cJSON_AddStringToObject(pDestItem, "boardname", (char *)pDestInfo->aru8BoardName);
            
            int arnPortList[] = {32};

            cJSON_AddNumberToObject(pDestItem, "portnumber", 1);

            cJSON_AddItemToObject(pDestItem, "portlist", cJSON_CreateIntArray(ARP_LEN(arnPortList)));

            cJSON_AddNumberToObject(pDestItem, "channelnumber", 32);
            
            cJSON_AddItemToObject(pDestItem, "channellist", cJSON_CreateIntArray(ARP_LEN((int *)pDestInfo->aru32Channel)));

            cJSON_AddItemToObject(pDestItem, "bypasslist", cJSON_CreateIntArray(ARP_LEN((int *)pDestInfo->aru32BypassFlag)));
            
        }

        // 获取节目列表
        memset(&stIndexInfoList , 0x00, sizeof(IndexInfoList));
        eErrCode = web_GetInTSProgIndexList(u16InTSIndex, (TSInfo *)pstParamTS, &stIndexInfoList);
        if(WV_SUCCESS != eErrCode)
        {
            WEB_LOG("web_GetInTSProgIndexList return 0x%x!", eErrCode);
            cJSON_Delete(pRoot);
            goto web_processGetSubboardTSInfo_WEB_ERROR;
        }
        
        cJSON_AddNumberToObject(pTSItem, "programnumber", stIndexInfoList.u16IndexListNum);

        cJSON_AddItemToObject(pTSItem, "programlist", pProgramList = cJSON_CreateArray());

        for ( j = 0; j < stIndexInfoList.u16IndexListNum; j++ )
        {
            cJSON_AddItemToArray(pProgramList, pProgramItem = cJSON_CreateObject());

            cJSON_AddNumberToObject(pProgramItem,"index",j);            

            cJSON_AddStringToObject(pProgramItem,"servicename",
                (char *)web_StringConvertToUTF8(pstParamTS->pInputProgram[stIndexInfoList.aru16IndexList[j]].aru8ServiceName
                                                ,pstParamTS->pInputProgram[stIndexInfoList.aru16IndexList[j]].u8ServiceNameLen));
            
            cJSON_AddNumberToObject(pProgramItem,"serviceid",pstParamTS->pInputProgram[stIndexInfoList.aru16IndexList[j]].u16ServiceID);

            // descramble U8     u8FunctionFlag;  U8     u8CAMIndex;           

            memset(&stDest , 0x00, sizeof(Dest));
            eErrCode = web_GetProgDestInfo(stIndexInfoList.aru16IndexList[j], (TSInfo *)pstParamTS, &stDest);
            if(WV_SUCCESS != eErrCode)
            {
                WEB_LOG("web_GetProgDestInfo return 0x%x!",eErrCode);
                cJSON_Delete(pRoot);
                goto web_processGetSubboardTSInfo_WEB_ERROR;
            }

            cJSON_AddNumberToObject(pProgramItem,"descramble",pstParamTS->pInputProgram[stIndexInfoList.aru16IndexList[j]].u8CAMIndex);

            cJSON_AddNumberToObject(pProgramItem,"destboardnumber",stDest.u8DestSlotNum);

            cJSON_AddItemToObject(pProgramItem, "destboardlist", pProgramDestList = cJSON_CreateArray());

            // 遍历slot
            for ( k = 0; k < stDest.u8DestSlotNum; k++ )
            {
                pDestInfo = &stDest.arstDestSlotInfo[k];
    
                cJSON_AddItemToArray(pProgramDestList, pProgramDestItem = cJSON_CreateObject());
                
                cJSON_AddNumberToObject(pProgramDestItem, "board", pDestInfo->u8Slot);
                
                cJSON_AddStringToObject(pProgramDestItem, "boardname", (char *)pDestInfo->aru8BoardName);
                
                cJSON_AddNumberToObject(pProgramDestItem, "portnumber", 4);

                int arnProgramDestPortList[] = {16};

                cJSON_AddItemToObject(pProgramDestItem, "portlist",cJSON_CreateIntArray(ARP_LEN(arnProgramDestPortList)));
                
                cJSON_AddItemToObject(pProgramDestItem, "channellist", cJSON_CreateIntArray(ARP_LEN((int *)pDestInfo->aru32Channel)));

                cJSON_AddItemToObject(pProgramDestItem, "bypasslist", cJSON_CreateIntArray(ARP_LEN((int *)pDestInfo->aru32BypassFlag)));
                
            }
        }
    }

    web_TSPMutexUnlock();

    cJSON_AddNumberToObject(pTSInfo, "version", 100);
    
    pResponseString = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot); 

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);
    
    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;
    
    return WV_SUCCESS;
    
web_processGetSubboardTSInfo_WEB_ERROR:
    web_TSPMutexUnlock();
    return eErrCode;
}

/****************************************************************************
 *
 * 名字:
 *      web_processGetOutputBoardInfo
 *
 * 目标:
 *      获取输出板信息
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-6
 *
 ****************************************************************************/
wvErrCode web_processGetOutputBoardInfo(buffer * pstWriteBuff)
{
    cJSON           * pJson           = NULL;
    cJSON           * pTemp           = NULL;
    cJSON           * pRoot           = NULL;  
    cJSON           * pOutputBoardInfo= NULL;
    cJSON           * pBoardList      = NULL;
    cJSON           * pBoardItem      = NULL;
    U32               i               = 0;  
    U32               j               = 0;
    char            * pResponseString = NULL;
    int               arnPortList[256]   = {0};
    int               arnChannelList[8]= {0};

    U8                aru8BoardName[20] = {0};
    U8                u8PortNum = 0;
    U16               u16ChannelNum = 0;
    U8                u8ChannelStatus = 0; 
    
    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    }
    
    //解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }    

    //获取授权码
    pTemp = cJSON_GetObjectItem(pJson , "token");
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("Token = %s.",pTemp->valuestring);
         //这里要判断授权码的有效性

         cJSON_Delete(pJson);
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", WV_SUCCESS);

    cJSON_AddNullToObject(pRoot, "description");

    cJSON_AddItemToObject(pRoot, "data", pOutputBoardInfo = cJSON_CreateObject());

    cJSON_AddItemToObject(pOutputBoardInfo, "boardlist", pBoardList = cJSON_CreateArray());

    for ( i = 0 ; i < MAX_SLOT_NUM; i++ )
    {
        #if 0
        if (!web_IsSubBoardPlugin(i))
        {
            WEB_LOG("board[%d] Not Plugin!", i);
            continue;
        }
        #endif
        
        if (!web_IsOutputBoard(web_GetBoardType(i)))
        {
            continue;
        }

        u8PortNum = web_GetBoardPortNum(i);
        u16ChannelNum = web_GetOutputChannelNum(i);

        if ((0 == u8PortNum) || (0 == u16ChannelNum))
        {
            continue;
        }

        web_GetBoardDescriptor(i, aru8BoardName);
        aru8BoardName[19] = 0;

        for (j = 0; j < u8PortNum; j++)
        {
            arnPortList[j] = u16ChannelNum / u8PortNum;
        }

        memset(arnChannelList, 0, sizeof(arnChannelList));
        for (j = 0; ((j < u16ChannelNum) && (j < 256)); j++)
        {
            u8ChannelStatus = web_GetOutputChannelStatus(i, j);
            
            if (1 != u8ChannelStatus)
            {
                arnChannelList[j / 32] |= (0x1 << (j % 32));
            }
        }
        
        cJSON_AddItemToArray(pBoardList, pBoardItem = cJSON_CreateObject());
        cJSON_AddNumberToObject(pBoardItem, "board", i);
        cJSON_AddStringToObject(pBoardItem, "boardname", (const char *)aru8BoardName);
        cJSON_AddNumberToObject(pBoardItem, "portnumber", u8PortNum);
        cJSON_AddNumberToObject(pBoardItem, "channelnumber", u16ChannelNum);
        cJSON_AddItemToObject(pBoardItem, "portlist", cJSON_CreateIntArray(arnPortList, u8PortNum));
        cJSON_AddItemToObject(pBoardItem, "channellist", cJSON_CreateIntArray(ARP_LEN(arnChannelList)));
    }
    
    cJSON_AddNumberToObject(pOutputBoardInfo, "version", 100);
    
    pResponseString = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot); 

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);
    
    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;
    
    return WV_SUCCESS;
}


/****************************************************************************
 *
 * 名字:
 *      web_processGetTSDetailStatus
 *
 * 目标:
 *      获取子板的流的详细信息
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      huan.zhou in 2017-4-21
 *
 ****************************************************************************/
wvErrCode web_processGetTSDetailStatus(buffer * pstWriteBuff)
{
    cJSON           * pJson            = NULL;
    cJSON           * pTemp            = NULL;
    cJSON           * pRoot            = NULL;
    cJSON           * pData            = NULL;
    cJSON           * pPIDList         = NULL;
    cJSON           * pPIDItem         = NULL;
    char            * pResponseString  = NULL;
    wvErrCode         eErrorCode       = WV_SUCCESS;
    U16               u16Channel       = 0;
    TSBitrateInfo     stWebTSBitrateInfo;
    U16               i                = 0;
    U8                u8PIDType        = 0;
    U8                aru8Name[MAX_STREAMTYPE_NAME_LENGTH] = {0};

    // 入参判断
    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    }

    // 解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取授权码
    pTemp = cJSON_GetObjectItem(pJson , "token");
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("token = %s.",pTemp->valuestring);
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }
    
    //port
    pTemp = cJSON_GetObjectItem(pJson , "port");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
         WEB_LOG("port = %d.",pTemp->valueint);
         u16Channel = pTemp->valueint;
    }
    
    //channel
    pTemp = cJSON_GetObjectItem(pJson , "channel");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
         WEB_LOG("channel = %d.",pTemp->valueint);
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取数据完毕
    cJSON_Delete(pJson);

    //组装数据
    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", WV_SUCCESS);
    
    cJSON_AddNullToObject(pRoot, "description");
    
    cJSON_AddItemToObject(pRoot, "data" , pData = cJSON_CreateObject());

    memset(&stWebTSBitrateInfo , 0 , sizeof(stWebTSBitrateInfo));

    TSP_MutexLock();
    eErrorCode = web_GetTSBitrateInfo(u16Channel, &stWebTSBitrateInfo);
    TSP_MutexUnlock();
    
    if(WV_SUCCESS != eErrorCode)
    {
        cJSON_Delete(pRoot); 
        
        WEB_LOG("Web_GetTSStatus return 0x%x",eErrorCode);
        return eErrorCode;
    }

    //port
    cJSON_AddNumberToObject(pData, "port", u16Channel);
    
    //channel
    cJSON_AddNumberToObject(pData, "channel", 0);
    
    if(TSP_FLAG_INVALID != stWebTSBitrateInfo.u8ValidFlag)
    {
        //pidnumber
        cJSON_AddNumberToObject(pData, "pidnumber", stWebTSBitrateInfo.u16PIDNum);

        //totalbitrate
        cJSON_AddNumberToObject(pData, "totalbitrate", stWebTSBitrateInfo.u32TotalBitrate / 1000);
        
        //pidlist
        cJSON_AddItemToObject(pData, "pidlist", pPIDList = cJSON_CreateArray());
        
        for( i = 0; i < stWebTSBitrateInfo.u16PIDNum; i++)
        {
            cJSON_AddItemToArray(pPIDList, pPIDItem = cJSON_CreateObject());
            
            //pid
            cJSON_AddNumberToObject(pPIDItem, "pid", stWebTSBitrateInfo.arstPIDBitrateInfo[i].u16PID);
            
            //type
            u8PIDType = stWebTSBitrateInfo.arstPIDBitrateInfo[i].u8PIDType;
            memset(aru8Name, 0, MAX_STREAMTYPE_NAME_LENGTH);
            web_GetPIDTypeDesc(u8PIDType, 0, aru8Name);
            cJSON_AddStringToObject(pPIDItem, "type", (const char *)aru8Name);
            
            //bitrate
            cJSON_AddNumberToObject(pPIDItem, "bitrate", stWebTSBitrateInfo.arstPIDBitrateInfo[i].u32Bitrate / 1000);
            
            //ccerr
            cJSON_AddNumberToObject(pPIDItem, "ccerr", stWebTSBitrateInfo.arstPIDBitrateInfo[i].u8CcNum);

            //samepidnum
            cJSON_AddNumberToObject(pPIDItem, "samepidnum", stWebTSBitrateInfo.arstPIDBitrateInfo[i].u16SamePIDNum);

            //servicename
            cJSON_AddStringToObject(pPIDItem, "servicename", (char *)web_StringConvertToUTF8(stWebTSBitrateInfo.arstPIDBitrateInfo[i].aru8ServiceName,stWebTSBitrateInfo.arstPIDBitrateInfo[i].u8ServiceNameLen));
        }
    }
    else
    {
        //pidnumber
        cJSON_AddNumberToObject(pData, "pidnumber", 0);
        
        //pidlist
        cJSON_AddItemToObject(pData, "pidlist", cJSON_CreateArray());        
    }
    
    pResponseString = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot); 

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);
    
    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;
    
    return WV_SUCCESS;
}


/****************************************************************************
 *
 * 名字:
 *      web_processClearAllErr
 *
 * 目标:
 *      清除流的错误计数
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      huan.zhou in 2017-4-21
 *
 ****************************************************************************/
wvErrCode web_processClearAllErr(buffer * pstWriteBuff)
{
    cJSON           * pJson            = NULL;
    cJSON           * pTemp            = NULL;
    
    // 入参判断
    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    }

    // 解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取授权码
    pTemp = cJSON_GetObjectItem(pJson , "token");
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("token = %s.",pTemp->valuestring);
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取数据完毕
    cJSON_Delete(pJson);
    
    web_ClearPIDCc();

    return web_processSuccess(pstWriteBuff);
}

/****************************************************************************
 *
 * 名字:
 *      web_processSetTsOutput
 *
 * 目标:
 *      设置流去向
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-6
 *
 ****************************************************************************/
wvErrCode web_processSetTsOutput(buffer * pstWriteBuff)
{
    cJSON           * pJson           = NULL;
    cJSON           * pTemp           = NULL;
    cJSON           * pChild          = NULL;
    cJSON           * pTempChild      = NULL;
    cJSON           * pTempChildChild = NULL;
    wvErrCode         eErrCode        = WV_SUCCESS;
    U8                u8Port          = 0;
    U16               u16InTSIndex    = 0;
    const TSInfo    * pstParamTS      = NULL;
    Dest              stDest;
    U8                u8Count         = 0; 
    U8                u8TempCount     = 0;
    U8                i               = 0;
    U32               j               = 0;

    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    }

    pstParamTS = (const TSInfo *)web_GetTSPramaHandle();
    if (NULL == pstParamTS)
    {
        WEB_LOG("web_GetTSPrama return null!");
        return WV_ERR_WEB_BAD_PARAM;
    }

    //解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }    

    //获取授权码
    pTemp = cJSON_GetObjectItem(pJson , "token");
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("Token = %s.",pTemp->valuestring);
         //这里要判断授权码的有效性

         //cJSON_Delete(pJson);
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    //获取流信息版本
    pTemp = cJSON_GetObjectItem(pJson , "tsversion");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
         WEB_LOG("TSVersion = %d.",pTemp->valueint);
         //这里要判断版本号是否一致

         //cJSON_Delete(pJson);
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    //获取输出板信息版本
    pTemp = cJSON_GetObjectItem(pJson , "destversion");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
         WEB_LOG("DestVersion = %d.",pTemp->valueint);
         //这里要判断版本号是否一致

         //cJSON_Delete(pJson);
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    //获取端口号
    pTemp = cJSON_GetObjectItem(pJson , "port");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
         WEB_LOG("Port = %d.",pTemp->valueint);
         u8Port = (U8)pTemp->valueint;
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }
    
    memset(&stDest , 0x00, sizeof(Dest));
    
    //获取去向列表
    pTemp = cJSON_GetObjectItem(pJson , "destboardlist");
    if ((NULL != pTemp) && (cJSON_Array == pTemp->type))
    {
        u8Count = (U8)cJSON_GetArraySize(pTemp);

        for( i = 0; i < u8Count; i++ )
        {
            pChild = cJSON_GetArrayItem(pTemp,(int)i);
            if ( NULL == pChild )
            {
                continue;
            }

            //获取去向板号
            pTempChild = cJSON_GetObjectItem(pChild , "board");
            if ((NULL != pTempChild ) && (cJSON_Number == pTempChild->type))
            {
                WEB_LOG("DestSlotList[%d].board = %d.", i, pTempChild->valueint);
                stDest.arstDestSlotInfo[stDest.u8DestSlotNum].u8Slot = (U8) pTempChild->valueint;
            }

            // 获取去向通道
            pTempChild = cJSON_GetObjectItem(pChild , "channellist");
            if ((NULL != pTempChild) && (cJSON_Array == pTempChild->type))
            { 
                u8TempCount = (U8)cJSON_GetArraySize(pTempChild);
        
                for( j = 0; j < u8TempCount; j++ )
                {
                    pTempChildChild = cJSON_GetArrayItem(pTempChild,(int)j);
                    if ( (NULL != pTempChildChild) && (cJSON_Number == pTempChildChild->type) )
                    {
                        stDest.arstDestSlotInfo[stDest.u8DestSlotNum].aru32Channel[j] = (U32)pTempChildChild->valueint;
                    }
                }    
            }

            // 获取去向直通标记
            pTempChild = cJSON_GetObjectItem(pChild , "bypasslist");
            if ((NULL != pTempChild) && (cJSON_Array == pTempChild->type))
            {

                u8TempCount = (U8)cJSON_GetArraySize(pTempChild);
        
                for( j = 0; j < u8TempCount; j++ )
                {
                    pTempChildChild = cJSON_GetArrayItem(pTempChild,(int)j);
                    if ( (NULL != pTempChildChild) && (cJSON_Number == pTempChildChild->type) )
                    {
                        stDest.arstDestSlotInfo[stDest.u8DestSlotNum].aru32BypassFlag[j] = (U32)pTempChildChild->valueint;
                    }
                }  
            }
            
            stDest.u8DestSlotNum++;
        }
    }

    // 获取数据完毕
    cJSON_Delete(pJson);

    web_TSPMutexLock();

    eErrCode = web_GetInTSIndex(u8Port, (TSInfo *)pstParamTS, &u16InTSIndex);
    if(WV_SUCCESS != eErrCode)
    {
        WEB_LOG("web_GetInTSIndex return 0x%x!",eErrCode);
        web_TSPMutexUnlock();
        return eErrCode;
    }
    
    //调用设置流去向的接口    
    eErrCode = web_ProcessTSDest(u16InTSIndex, &stDest, (TSInfo *)pstParamTS);
    if(WV_SUCCESS != eErrCode)
    {
        WEB_LOG("web_ProcessTSDest return 0x%x!",eErrCode);
        web_TSPMutexUnlock();
        return eErrCode;
    }

    web_TSPMutexUnlock();
    
    return web_processSuccess(pstWriteBuff);
}

/****************************************************************************
 *
 * 名字:
 *      web_processSetProgramOutput
 *
 * 目标:
 *      设置节目去向
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-6
 *
 ****************************************************************************/
wvErrCode web_processSetProgramOutput(buffer * pstWriteBuff)
{
    cJSON           * pJson           = NULL;
    cJSON           * pTemp           = NULL; 
    cJSON           * pChild          = NULL;
    cJSON           * pGrandChild     = NULL;
    cJSON           * pGrandChildChild= NULL;
    cJSON           * pTempChild      = NULL;
    cJSON           * pTempGrandChild = NULL;
    wvErrCode         eErrCode        = WV_SUCCESS;
    U8                u8Port          = 0;
    U16               u16ServiceID    = 0;
    U16               u16InTSIndex    = 0;
    U16               u16InProgIndex  = 0;
    U16               u16CAMCount     = 0;
    const TSInfo    * pstParamTS      = NULL;
    DescrambleProgInfo arstDescrambleProgInfo[100];
    Dest              stDest;
    U32               u32Count        = 0;
    U32               u32ProgramCount = 0;
    U32               u32TempCount    = 0;
    U32               i               = 0;
    U32               j               = 0;
    U32               k               = 0;

    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    }

    pstParamTS = (const TSInfo*)web_GetTSPramaHandle();
    if (NULL == pstParamTS)
    {
        WEB_LOG("web_GetTSPrama return null!");
        return WV_ERR_WEB_BAD_PARAM;
    }

    //解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }    

    //获取授权码
    pTemp = cJSON_GetObjectItem(pJson , "token");
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("Token = %s.",pTemp->valuestring);
         //这里要判断授权码的有效性

         //cJSON_Delete(pJson);
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    //获取流信息版本
    pTemp = cJSON_GetObjectItem(pJson , "tsversion");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
         WEB_LOG("TSVersion = %d.",pTemp->valueint);
         //这里要判断版本号是否一致

         //cJSON_Delete(pJson);
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    //获取输出板信息版本
    pTemp = cJSON_GetObjectItem(pJson , "destversion");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
         WEB_LOG("DestVersion = %d.",pTemp->valueint);
         //这里要判断版本号是否一致

         //cJSON_Delete(pJson);
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }
    
    web_TSPMutexLock();

    // 获取节目信息 
    pTemp = cJSON_GetObjectItem(pJson , "programlist");
    if ((NULL != pTemp ) && (cJSON_Array == pTemp->type))
    {
         
         memset(&arstDescrambleProgInfo , 0x00, sizeof(DescrambleProgInfo)*100);
         u32ProgramCount = (U32)cJSON_GetArraySize(pTemp);  
         
         for (i = 0; i < u32ProgramCount; i++)
         {
            pChild = cJSON_GetArrayItem(pTemp,(int)i);
            if ( NULL == pChild )
            {
                continue;
            }
            //获取端口号
            pTempChild = cJSON_GetObjectItem(pChild , "port");
            if ((NULL != pTempChild ) && (cJSON_Number == pTempChild->type))
            {
                 WEB_LOG("port = %d.",pTempChild->valueint);
                 u8Port = (U8)pTempChild->valueint;
            }
            else
            {
                continue;
            }

            //获取通道号
            pTempChild = cJSON_GetObjectItem(pChild , "channel");
            if ((NULL != pTempChild ) && (cJSON_Number == pTempChild->type))
            {
                 WEB_LOG("channel = %d.",pTempChild->valueint);
            }
            
            //获取节目ID
            pTempChild = cJSON_GetObjectItem(pChild , "serviceid");
            if ((NULL != pTempChild ) && (cJSON_Number == pTempChild->type))
            {
                 WEB_LOG("serviceid = %d.",pTempChild->valueint);
                 u16ServiceID = (U16)pTempChild->valueint;
            }
            else
            {
                continue;
            }

            //获取解扰状态
            pTempChild = cJSON_GetObjectItem(pChild , "descramble");
            if ((NULL != pTempChild ) && (cJSON_Number == pTempChild->type))
            {
                 WEB_LOG("descramble = %d.",pTempChild->valueint);
                 arstDescrambleProgInfo[u16CAMCount].u8CAMIndex = (U8)pTempChild->valueint;
                 arstDescrambleProgInfo[u16CAMCount].u16Channel = u8Port;
                 arstDescrambleProgInfo[u16CAMCount].u16ServiceID = u16ServiceID;
                 u16CAMCount++;
            }
            
            //获取去向列表
            
            pTempChild = cJSON_GetObjectItem(pChild , "destboardlist");
            if ((NULL != pTempChild ) && (cJSON_Array == pTempChild->type))
            {
                memset(&stDest , 0x00, sizeof(Dest));
                u32Count = (U32)cJSON_GetArraySize(pTempChild);
                for( j = 0; j < u32Count; j++ )
                {
                    pGrandChild = cJSON_GetArrayItem(pTempChild,(int)j);
                    if ( NULL == pGrandChild )
                    {
                        continue;
                    }
                    
                    //获取去向板号
                    pTempGrandChild = cJSON_GetObjectItem(pGrandChild , "board");
                    if ((NULL != pTempGrandChild ) && (cJSON_Number == pTempGrandChild->type))
                    {
                        WEB_LOG("DestList[%d].board = %d.",stDest.u8DestSlotNum,pTempGrandChild->valueint);
                        stDest.arstDestSlotInfo[stDest.u8DestSlotNum].u8Slot = (U8)pTempGrandChild->valueint;
                    }

                    //获取去向通道号
                    pTempGrandChild = cJSON_GetObjectItem(pGrandChild , "channellist");
                    if ((NULL != pTempGrandChild ) && (cJSON_Array == pTempGrandChild->type))
                    {
                        u32TempCount = (U32)cJSON_GetArraySize(pTempGrandChild);
                
                        for( k = 0; k < u32TempCount; k++ )
                        {
                            pGrandChildChild = cJSON_GetArrayItem(pTempGrandChild,(int)k);
                            if ( (NULL != pGrandChildChild) && (cJSON_Number == pGrandChildChild->type) )
                            {
                                stDest.arstDestSlotInfo[stDest.u8DestSlotNum].aru32Channel[k] = (U32)pGrandChildChild->valueint;
                            }
                        }    
                    }
                    stDest.u8DestSlotNum++;            
                 }
                
                eErrCode = web_GetInTSIndex(u8Port, (TSInfo *)pstParamTS, &u16InTSIndex);
                if(WV_SUCCESS != eErrCode)
                {
                    WEB_LOG("web_GetInTSIndex return 0x%x!",eErrCode);

                    cJSON_Delete(pJson);
                    web_TSPMutexUnlock();
                    return eErrCode;
                }

                eErrCode = web_GetInProgIndex(u16InTSIndex, u16ServiceID, (TSInfo *)pstParamTS, &u16InProgIndex);
                if(WV_SUCCESS != eErrCode)
                {
                    WEB_LOG("web_GetInProgIndex return 0x%x!",eErrCode);

                    cJSON_Delete(pJson);
                    web_TSPMutexUnlock();
                    return eErrCode;
                }
                
                //调用设置节目去向的接口    
                eErrCode = web_ProcessProgDest(u16InProgIndex, &stDest, (TSInfo *)pstParamTS);
                if(WV_SUCCESS != eErrCode)
                {
                    WEB_LOG("web_ProcessProgDest return 0x%x!",eErrCode);

                    cJSON_Delete(pJson);
                    web_TSPMutexUnlock();
                    return eErrCode;
                }
            }
        }
    }
    else
    {
        cJSON_Delete(pJson);
        web_TSPMutexUnlock();
        return WV_ERR_WEB_POST_PARAM;
    }
            
    cJSON_Delete(pJson);

    if(u16CAMCount != 0)
    {
        //调用设置节目解扰的接口
        eErrCode = web_ProcessDescramble(arstDescrambleProgInfo, u16CAMCount, (TSInfo *)pstParamTS);
        if(WV_SUCCESS != eErrCode)
        {
            WEB_LOG("web_ProcessDescramble return 0x%x!",eErrCode);
            web_TSPMutexUnlock();
            return eErrCode;
        }
    }

    web_TSPMutexUnlock();
    
    return web_processSuccess(pstWriteBuff);
}


/****************************************************************************
 *
 * 名字:
 *      web_processClearAllTS
 *
 * 目标:
 *      清除所有配置
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      huan.zhou in 2017-4-20
 *
 ****************************************************************************/
wvErrCode web_processClearAllTS(buffer * pstWriteBuff)
{
    cJSON              * pJson           = NULL;
    cJSON              * pTemp           = NULL;
    char               * arpToken[]      = {"token"};
    wvErrCode            eErrCode        = WV_SUCCESS;

    // 入参判断
    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    } 

    // 解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取授权码
    pTemp = web_getValueByNameFromJson(pJson , ARP_LEN(arpToken));
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("Token = %s.",pTemp->valuestring);
         //这里要判断授权码的有效性 
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取数据完毕
    cJSON_Delete(pJson);

    web_TSPMutexLock();
    eErrCode = web_ClearAllTS();
    web_TSPMutexUnlock();
    if (WV_SUCCESS != eErrCode)
    {
        WEB_LOG("web_ClearAllTS return 0x%x!", eErrCode);
        return eErrCode;
    }
    
    return web_processSuccess(pstWriteBuff);
}


/****************************************************************************
 *
 * 名字:
 *      web_processGetSubboardParam
 *
 * 目标:
 *      获取子板参数
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-8
 *
 ****************************************************************************/
wvErrCode web_processGetSubboardParam(buffer * pstWriteBuff)
{
    cJSON              * pJson           = NULL;
    cJSON              * pTemp           = NULL;
    cJSON              * pRoot           = NULL; 
    cJSON              * pData           = NULL;
    cJSON              * pPortList       = NULL;
    cJSON              * pPortParam      = NULL;
    cJSON              * pChannels       = NULL;
    cJSON              * pChannelParam   = NULL;
    wvErrCode            eErrCode        = WV_SUCCESS;
    U8                   u8Port          = 0;
    char               * pResponseString = NULL;
    Tuner_C_Params_t     stTunerParam;
    Tune_S2_Params_t     stS2TunerParam;
    TUNER_TYPE_enum      eTunerType      = TUENR_TYPE_UNKNOW;

    // 入参判断
    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    } 

    // 解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取授权码
    pTemp = cJSON_GetObjectItem(pJson , "token");
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("token = %s.",pTemp->valuestring);
         //这里要判断授权码的有效性 
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取数据完毕
    cJSON_Delete(pJson);

    
    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", WV_SUCCESS);

    cJSON_AddNullToObject(pRoot, "description");

    cJSON_AddItemToObject(pRoot, "data", pData = cJSON_CreateObject());

    eErrCode = Web_getTunerType(&eTunerType);
    if(WV_SUCCESS != eErrCode)
    {
        WEB_LOG("Web_getTunerType return 0x%x!", eErrCode);
        cJSON_Delete(pRoot);
        return eErrCode;
    }

    cJSON_AddNumberToObject(pData,"tunertype",eTunerType);

    cJSON_AddItemToObject(pData,"portlist",pPortList = cJSON_CreateArray());

    for( u8Port = 0; u8Port < DTV4U_TUNER_CHAN_NUM; u8Port++ ) 
    {        
        if(TUNER_C == eTunerType)
        {
            memset(&stTunerParam, 0, sizeof(Tuner_C_Params_t));
            eErrCode = web_GetCTunerParam(u8Port, &stTunerParam);
            if(WV_SUCCESS != eErrCode)
            {
                WEB_LOG("web_GetCTunerParam return 0x%x!", eErrCode);
                cJSON_Delete(pRoot);
                return eErrCode;
            }
        }
        else if(TUNER_S2 == eTunerType)
        {
            memset(&stS2TunerParam, 0, sizeof(Tune_S2_Params_t));
            eErrCode = web_GetS2TunerParam(u8Port, &stS2TunerParam);
            if(WV_SUCCESS != eErrCode)
            {
                WEB_LOG("web_GetS2TunerParam return 0x%x!", eErrCode);
                cJSON_Delete(pRoot);
                return eErrCode;
            }
        }
        else if(TUNER_DTMB == eTunerType)
        {
            memset(&stTunerParam, 0, sizeof(Tuner_C_Params_t));
            eErrCode = web_GetDTMBTunerParam(u8Port, &stTunerParam);
            if(WV_SUCCESS != eErrCode)
            {
                WEB_LOG("web_GetDTMBTunerParam return 0x%x!", eErrCode);
                cJSON_Delete(pRoot);
                return eErrCode;
            }
        }
        
        cJSON_AddItemToArray(pPortList, pPortParam = cJSON_CreateObject());

        cJSON_AddNumberToObject(pPortParam, "port", u8Port);

        cJSON_AddNumberToObject(pPortParam, "channelnumber", 1);

        cJSON_AddItemToObject(pPortParam, "channels", pChannels = cJSON_CreateArray());

        cJSON_AddItemToArray(pChannels, pChannelParam = cJSON_CreateObject());
        
        cJSON_AddNumberToObject(pChannelParam, "channelindex", 0);

        if(TUNER_C == eTunerType)
        {
            //当是tuner c时返回内容

            cJSON_AddNumberToObject(pChannelParam, "qammode", stTunerParam.u32QamMode);

            cJSON_AddNumberToObject(pChannelParam, "symbolrate", stTunerParam.u32SymbolRate);

            cJSON_AddNumberToObject(pChannelParam, "frequency", stTunerParam.u32FrequencyKHz);

            //cJSON_AddNumberToObject(pChannelParam, "mode", stTunerParam.u32Mode);
        }
        else if(TUNER_S2 == eTunerType)
        {
            //当是tuner s2时返回内容
            
            //symbolrate
            cJSON_AddNumberToObject(pChannelParam, "symbolrate", stS2TunerParam.u32SymbolRate);
            
            //satellitefrequency
            cJSON_AddNumberToObject(pChannelParam, "satellitefrequency", stS2TunerParam.u32FrequencyKHz);
            
            //lnbfrequency
            cJSON_AddNumberToObject(pChannelParam, "lnbfrequency", stS2TunerParam.u32LNBFrequencyKHz);
            
            //lnbpowersupport
            cJSON_AddNumberToObject(pChannelParam, "lnbpowersupport", stS2TunerParam.u32LNBVoltage);
            
            //lnb22khz      
            cJSON_AddNumberToObject(pChannelParam, "lnb22khz", stS2TunerParam.u32LNB22k);
        }
        else if(TUNER_DTMB == eTunerType)
        {
            cJSON_AddNumberToObject(pChannelParam, "frequency", stTunerParam.u32FrequencyKHz);

            cJSON_AddNumberToObject(pChannelParam, "qammode", stTunerParam.u32QamMode);
        }
    }

    pResponseString = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot); 

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);
    
    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;
    
    return WV_SUCCESS;

}

/****************************************************************************
 *
 * 名字:
 *      web_processSetSubboardParam
 *
 * 目标:
 *      设置子板参数
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-8
 *
 ****************************************************************************/
wvErrCode web_processSetSubboardParam(buffer * pstWriteBuff)
{
    cJSON              * pJson           = NULL;
    cJSON              * pTemp           = NULL;
    cJSON              * pChild          = NULL;
    cJSON              * pTempChild      = NULL;
    cJSON              * pChildChild     = NULL;
    cJSON              * pTempChildChild = NULL;
    wvErrCode            eErrCode        = WV_SUCCESS;
    U8                   u8Count         = 0;
    U8                   u8Port          = 0;
    U8                   i               = 0;
    Tuner_C_Params_t     stTunerParam;
    Tune_S2_Params_t     stS2TunerParam;
    TUNER_TYPE_enum      eTunerType      = TUENR_TYPE_UNKNOW;

    // 入参判断
    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    } 

    // 解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取授权码
    pTemp = cJSON_GetObjectItem(pJson , "token");
    if ((NULL != pTemp ) && (cJSON_String == pTemp->type))
    {
         WEB_LOG("token = %s.",pTemp->valuestring);
         //这里要判断授权码的有效性 
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }
    
    eErrCode = Web_getTunerType(&eTunerType);
    if(WV_SUCCESS != eErrCode)
    {
        WEB_LOG("Web_getTunerType return 0x%x!", eErrCode);
        cJSON_Delete(pJson);
        return eErrCode;
    }
    
    // 获取端口列表
    pTemp = cJSON_GetObjectItem(pJson , "portlist");
    if ((NULL != pTemp ) && (cJSON_Array == pTemp->type))
    {
         u8Count = (U32)cJSON_GetArraySize(pTemp);
         for (i = 0; i < u8Count; i++)
         {
            pChild = cJSON_GetArrayItem(pTemp,(int)i);
            if ( NULL == pChild )
            {
                continue;
            }

            // 获取端口号
            pTempChild = cJSON_GetObjectItem(pChild , "port");
            if ((NULL != pTempChild ) && (cJSON_Number == pTempChild->type))
            {
                WEB_LOG("port = %d.", pTempChild->valueint);
                u8Port = pTempChild->valueint;
            }
            else
            {
                continue;
            }
            
            if(TUNER_C == eTunerType)
            {
                memset(&stTunerParam, 0, sizeof(Tuner_C_Params_t));
                eErrCode = web_GetCTunerParam(u8Port, &stTunerParam);
                if(WV_SUCCESS != eErrCode)
                {
                    WEB_LOG("web_GetCTunerParam return 0x%x!", eErrCode);
                    cJSON_Delete(pJson);
                    return eErrCode;
                }
                
                // 获取通道列表
                pTempChild = cJSON_GetObjectItem(pChild , "channels");
                if ((NULL != pTempChild ) && (cJSON_Array == pTempChild->type))
                {
                    pChildChild = cJSON_GetArrayItem(pTempChild, 0);
                    if ( NULL == pChildChild)
                    {
                        continue;
                    }
                    
                    // 获取星座
                    pTempChildChild = cJSON_GetObjectItem(pChildChild, "qammode");
                    if ((NULL != pTempChildChild ) && (cJSON_Number == pTempChildChild->type))
                    {
                        WEB_LOG("port[%d].channels[0].qammode = %d.", u8Port, pTempChildChild->valueint);
                        stTunerParam.u32QamMode = pTempChildChild->valueint;
                    }

                    // 获取符号率
                    pTempChildChild = cJSON_GetObjectItem(pChildChild, "symbolrate");
                    if ((NULL != pTempChildChild ) && (cJSON_Number == pTempChildChild->type))
                    {
                        WEB_LOG("port[%d].channels[0].symbolrate = %d.", u8Port, pTempChildChild->valueint);
                        stTunerParam.u32SymbolRate = pTempChildChild->valueint;
                    }

                    // 获取频点
                    pTempChildChild = cJSON_GetObjectItem(pChildChild, "frequency");
                    if ((NULL != pTempChildChild ) && (cJSON_Number == pTempChildChild->type))
                    {
                        WEB_LOG("port[%d].channels[0].frequency = %d.", u8Port, pTempChildChild->valueint);
                        stTunerParam.u32FrequencyKHz = pTempChildChild->valueint;
                    }

                    // 获取模式
                    //pTempChildChild = cJSON_GetObjectItem(pChildChild, "mode");
                    //if ((NULL != pTempChildChild ) && (cJSON_Number == pTempChildChild->type))
                    //{
                    //    WEB_LOG("port[%d].channels[0].mode = %d.", u8Port, pTempChildChild->valueint);
                    //    stTunerParam.u32Mode = pTempChildChild->valueint;
                    //}
                    
                }

                eErrCode = web_SetCTunerParam(u8Port, &stTunerParam);
                if(WV_SUCCESS != eErrCode)
                {
                    WEB_LOG("web_SetCTunerParam return 0x%x!", eErrCode);
                    cJSON_Delete(pJson);
                    return eErrCode;
                }
            }
            else if(TUNER_S2 == eTunerType)
            {
                memset(&stS2TunerParam, 0, sizeof(Tune_S2_Params_t));
                eErrCode = web_GetS2TunerParam(u8Port, &stS2TunerParam);
                if(WV_SUCCESS != eErrCode)
                {
                    WEB_LOG("web_GetS2TunerParam return 0x%x!", eErrCode);
                    cJSON_Delete(pJson);
                    return eErrCode;
                }
                
                // 获取通道列表
                pTempChild = cJSON_GetObjectItem(pChild , "channels");
                if ((NULL != pTempChild ) && (cJSON_Array == pTempChild->type))
                {
                    pChildChild = cJSON_GetArrayItem(pTempChild, 0);
                    if ( NULL == pChildChild)
                    {
                        continue;
                    }

                    //symbolrate
                    pTempChildChild = cJSON_GetObjectItem(pChildChild, "symbolrate");
                    if ((NULL != pTempChildChild ) && (cJSON_Number == pTempChildChild->type))
                    {
                        WEB_LOG("port[%d].channels[0].symbolrate = %d.", u8Port, pTempChildChild->valueint);
                        stS2TunerParam.u32SymbolRate = pTempChildChild->valueint;
                    }
                    
                    //satellitefrequency
                    pTempChildChild = cJSON_GetObjectItem(pChildChild, "satellitefrequency");
                    if ((NULL != pTempChildChild ) && (cJSON_Number == pTempChildChild->type))
                    {
                        WEB_LOG("port[%d].channels[0].satellitefrequency = %d.", u8Port, pTempChildChild->valueint);
                        stS2TunerParam.u32FrequencyKHz = pTempChildChild->valueint;
                    }
                    
                    //lnbfrequency
                    pTempChildChild = cJSON_GetObjectItem(pChildChild, "lnbfrequency");
                    if ((NULL != pTempChildChild ) && (cJSON_Number == pTempChildChild->type))
                    {
                        WEB_LOG("port[%d].channels[0].lnbfrequency = %d.", u8Port, pTempChildChild->valueint);
                        stS2TunerParam.u32LNBFrequencyKHz = pTempChildChild->valueint;
                    }
                    
                    //lnbpowersupport 13v 18v off
                    pTempChildChild = cJSON_GetObjectItem(pChildChild, "lnbpowersupport");
                    if ((NULL != pTempChildChild ) && (cJSON_Number == pTempChildChild->type))
                    {
                        WEB_LOG("port[%d].channels[0].lnbpowersupport = %d.", u8Port, pTempChildChild->valueint);
                        stS2TunerParam.u32LNBVoltage = pTempChildChild->valueint;
                    }
                    
                    //lnb22khz  off on
                    pTempChildChild = cJSON_GetObjectItem(pChildChild, "lnb22khz");
                    if ((NULL != pTempChildChild ) && (cJSON_Number == pTempChildChild->type))
                    {
                        WEB_LOG("port[%d].channels[0].lnb22khz = %d.", u8Port, pTempChildChild->valueint);
                        stS2TunerParam.u32LNB22k = pTempChildChild->valueint;
                    }                    
                }

                eErrCode = web_SetS2TunerParam(u8Port, &stS2TunerParam);
                if(WV_SUCCESS != eErrCode)
                {
                    WEB_LOG("web_SetS2TunerParam return 0x%x!", eErrCode);
                    cJSON_Delete(pJson);
                    return eErrCode;
                }
            }
            else if(TUNER_DTMB == eTunerType)
            {
                memset(&stTunerParam, 0, sizeof(Tuner_C_Params_t));
                eErrCode = web_GetDTMBTunerParam(u8Port, &stTunerParam);
                if(WV_SUCCESS != eErrCode)
                {
                    WEB_LOG("web_GetDTMBTunerParam return 0x%x!", eErrCode);
                    cJSON_Delete(pJson);
                    return eErrCode;
                }
                
                // 获取通道列表
                pTempChild = cJSON_GetObjectItem(pChild , "channels");
                if ((NULL != pTempChild ) && (cJSON_Array == pTempChild->type))
                {
                    pChildChild = cJSON_GetArrayItem(pTempChild, 0);
                    if ( NULL == pChildChild)
                    {
                        continue;
                    }
                    
                    pTempChildChild = cJSON_GetObjectItem(pChildChild, "qammode");
                    if ((NULL != pTempChildChild ) && (cJSON_Number == pTempChildChild->type))
                    {
                        WEB_LOG("port[%d].channels[0].qammode = %d.", u8Port, pTempChildChild->valueint);
                        stTunerParam.u32QamMode = pTempChildChild->valueint;
                    }

                    // 获取频点
                    pTempChildChild = cJSON_GetObjectItem(pChildChild, "frequency");
                    if ((NULL != pTempChildChild ) && (cJSON_Number == pTempChildChild->type))
                    {
                        WEB_LOG("port[%d].channels[0].frequency = %d.", u8Port, pTempChildChild->valueint);
                        stTunerParam.u32FrequencyKHz = pTempChildChild->valueint;
                    }                    
                }

                eErrCode = web_SetDTMBTunerParam(u8Port, &stTunerParam);
                if(WV_SUCCESS != eErrCode)
                {
                    WEB_LOG("web_SetDTMBTunerParam return 0x%x!", eErrCode);
                    cJSON_Delete(pJson);
                    return eErrCode;
                }
            }
         }
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }   

    // 获取数据完毕
    cJSON_Delete(pJson);

    return web_processSuccess(pstWriteBuff);
}

/****************************************************************************
 *
 * 名字:
 *      web_processDefault
 *
 * 目标:
 *      恢复出厂设置
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-7
 *
 ****************************************************************************/
wvErrCode web_processDefault(buffer * pstWriteBuff)
{
    
    cJSON           * pRoot           = NULL;
    cJSON           * pData           = NULL;
    char            * pResponseString = NULL;
    wvErrCode         eErrorCode      = WV_SUCCESS;

    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    }
    
    eErrorCode = web_Default();
    if(WV_SUCCESS != eErrorCode)
    {
        WEB_LOG("web_Default return 0x%x!",eErrorCode);
        return eErrorCode;
    }
    
    //组装数据
    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", WV_SUCCESS);
    
    cJSON_AddNullToObject(pRoot, "description");
    
    cJSON_AddItemToObject(pRoot, "data" , pData = cJSON_CreateObject());

    //delay
    cJSON_AddNumberToObject(pData,"defaultDelay", DEFAULT_WAIT_TIME_S);
    
    pResponseString = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot); 

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);
    
    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;
    
    return WV_SUCCESS;
}

/****************************************************************************
 *
 * 名字:
 *      web_processReboot
 *
 * 目标:
 *      重启
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-7
 *
 ****************************************************************************/
wvErrCode web_processReboot(buffer * pstWriteBuff)
{
    cJSON           * pRoot           = NULL;
    cJSON           * pData           = NULL;
    char            * pResponseString = NULL;
    wvErrCode         eErrorCode      = WV_SUCCESS;

    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    }
    
    eErrorCode = web_Reboot();
    if(WV_SUCCESS != eErrorCode)
    {
        WEB_LOG("Web_Reboot return 0x%x!",eErrorCode);
        return eErrorCode;
    }
    
    //组装数据
    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", WV_SUCCESS);
    
    cJSON_AddNullToObject(pRoot, "description");
    
    cJSON_AddItemToObject(pRoot, "data" , pData = cJSON_CreateObject());

    //delay
    cJSON_AddNumberToObject(pData,"delay", REBOOT_WAIT_TIME_S);
    
    pResponseString = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot); 

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);
    
    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;
    
    return WV_SUCCESS;
}


/****************************************************************************
 *
 * 名字:
 *      web_processExportLogs
 *
 * 目标:
 *      导出日志
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      jia.lin in 2017-3-22
 *
 ****************************************************************************/
wvErrCode web_processExportLogs(buffer * pstWriteBuff)
{
    cJSON           * pJson            = NULL;
    cJSON           * pTemp            = NULL;
    cJSON           * pRoot            = NULL;
    cJSON           * pData            = NULL;
    wvErrCode       eErrCode           = WV_SUCCESS;
    char            arUrl[MAX_FILENAME_LENGTH] = {0};
    char            * pResponseString  = NULL;

    // 入参判断
    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    }

    // 解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取授权码
    pTemp = cJSON_GetObjectItem(pJson , "token");
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("token = %s.",pTemp->valuestring);
         // validate token
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取数据完毕
    cJSON_Delete(pJson);

    eErrCode = Web_ExportLogs((U8 *)arUrl);
    if (WV_SUCCESS != eErrCode)
    {
        WEB_LOG("web_ExportLogs return 0x%x!", eErrCode);
        return eErrCode;
    }

    // 开始组装应答数据
    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", WV_SUCCESS);

    cJSON_AddNullToObject(pRoot, "description");

    cJSON_AddItemToObject(pRoot, "data", pData = cJSON_CreateObject());

    cJSON_AddStringToObject(pData, "uri", arUrl);

    pResponseString = cJSON_Print(pRoot);

    // 组装应答数据完毕
    cJSON_Delete(pRoot);

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);

    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;

    return WV_SUCCESS;

}

/****************************************************************************
 *
 * 名字:
 *      web_processDebug
 *
 * 目标:
 *      调试接口
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      huan.zhou 2017-3-23
 *
 ****************************************************************************/
wvErrCode web_processDebug(buffer * pstWriteBuff)
{
    cJSON           * pJson            = NULL;
    cJSON           * pTemp            = NULL;
    cJSON           * pRoot            = NULL;
    cJSON           * pParam           = NULL;
    //wvErrCode       eErrCode           = WV_SUCCESS;
    char            * pResponseString  = NULL;
    U8                u8Type           = 0;
    int               i                = 0;
    U32               u32ParamsLen     = 0;
    int               arnParms[MAX_PARAMS_LENGTH]   = {0};
    int               arnResults[MAX_PARAMS_LENGTH] = {0};

    // 入参判断
    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    }

    // 解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取授权码
    pTemp = cJSON_GetObjectItem(pJson , "token");
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("token = %s.",pTemp->valuestring);
         // validate token
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取类型
    pTemp = cJSON_GetObjectItem(pJson , "type");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
         WEB_LOG("type = %d.",pTemp->valueint);
         u8Type = (U8)pTemp->valueint;
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }


    memset(arnParms,0,sizeof(int)*MAX_PARAMS_LENGTH);
    
    // 获取参数列表
    pTemp = cJSON_GetObjectItem(pJson , "params");
    if ((NULL != pTemp ) && (cJSON_Array == pTemp->type))
    {
         u32ParamsLen = cJSON_GetArraySize(pTemp);

         for(i = 0; i < u32ParamsLen; i++)
         {
            pParam = cJSON_GetArrayItem(pTemp,i);
            if(NULL == pParam)
            {
                continue;
            }
            
            arnParms[i] = pParam->valueint;
         }
         
    }

    switch(u8Type)
    {
        case 0://LUT

            if (arnParms[0] < MAX_INPUT_LUT_NUM)
            {
                web_DebugPrintLUTInfo(arnParms[0]);
            }
            else
            {
                web_DebugPrintCIOutputLUTInfo();
            }

            //arnResults赋值
            
            break;
        default:
            break;
    }

    // 获取数据完毕
    cJSON_Delete(pJson);

    // 开始组装应答数据
    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", WV_SUCCESS);

    cJSON_AddNullToObject(pRoot, "description");

    cJSON_AddItemToObject(pRoot, "data", cJSON_CreateIntArray(ARP_LEN(arnResults)));    

    pResponseString = cJSON_Print(pRoot);

    // 组装应答数据完毕
    cJSON_Delete(pRoot);

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);

    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;

    return WV_SUCCESS;
}

/****************************************************************************
 *
 * 名字:
 *      web_processGetInputTSStatus
 *
 * 目标:
 *      获取输入流状态
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      xiazhi.zhang in 2017-3-21
 *
 ****************************************************************************/
wvErrCode web_processGetInputTSStatus(buffer * pstWriteBuff)
{
    cJSON              * pJson           = NULL;
    cJSON              * pTemp           = NULL;
    cJSON              * pRoot           = NULL; 
    cJSON              * pData           = NULL;
    cJSON              * pPortParam      = NULL;
    cJSON              * pProgramlist    = NULL;
    cJSON              * pProgram        = NULL;
    wvErrCode            eErrCode        = WV_SUCCESS;
    U8                   u8Port          = 0;   
    char               * pResponseString = NULL;
    const TSInfo       * pstParamTS      = NULL;
    U16                  u16InTSIndex    = 0;
    U32                  i               = 0;
    IndexInfoList        stIndexInfoList;
    Tuner_stat           stTunerStatues;

    // 入参判断
    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    } 

    // 解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取授权码
    pTemp = cJSON_GetObjectItem(pJson , "token");
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("token = %s.",pTemp->valuestring);
         //这里要判断授权码的有效性 
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取数据完毕
    cJSON_Delete(pJson);

    pstParamTS = (const TSInfo *)web_GetTSPramaHandle();
    if (NULL == pstParamTS)
    {
        WEB_LOG("web_GetTSPrama return null!");
        return WV_ERR_WEB_BAD_PARAM;
    }

    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", WV_SUCCESS);

    cJSON_AddNullToObject(pRoot, "description");

    cJSON_AddItemToObject(pRoot, "data", pData = cJSON_CreateArray());

    web_TSPMutexLock();

    for( u8Port = 0; u8Port < DTV4U_TUNER_CHAN_NUM; u8Port++ ) 
    {
        memset(&stTunerStatues, 0, sizeof(Tuner_stat));
        eErrCode = web_GetTunerStatus(u8Port, &stTunerStatues);
        if(WV_SUCCESS != eErrCode)
        {
            WEB_LOG("web_GetTunerStatus return 0x%x!", eErrCode);
            continue;
        }
     
        cJSON_AddItemToArray(pData, pPortParam = cJSON_CreateObject());

        cJSON_AddNumberToObject(pPortParam, "port", stTunerStatues.port);

        cJSON_AddNumberToObject(pPortParam, "channel", stTunerStatues.channel);
        
        cJSON_AddNumberToObject(pPortParam, "totalbitrate", stTunerStatues.totalbitrate);
        
        cJSON_AddNumberToObject(pPortParam, "effectivebitrate", stTunerStatues.effectivebitrate);

        if(true == stTunerStatues.isLock)
        {
            cJSON_AddNumberToObject(pPortParam, "lockstatus", 1);
        }
        else
        {
            cJSON_AddNumberToObject(pPortParam, "lockstatus", 0);
        }
        
        cJSON_AddNumberToObject(pPortParam, "bercnt", stTunerStatues.uBerCnt);

        cJSON_AddNumberToObject(pPortParam, "berexponent", stTunerStatues.uBerExponent);

        cJSON_AddNumberToObject(pPortParam, "cn", stTunerStatues.s32C_N);

        cJSON_AddNumberToObject(pPortParam, "frequencytune", stTunerStatues.frequencytune);

        cJSON_AddNumberToObject(pPortParam, "rflevel", stTunerStatues.s32RFLevel);

        cJSON_AddItemToObject(pPortParam, "programlist", pProgramlist = cJSON_CreateArray());

        // 获取Input TS Index
        eErrCode = web_GetInTSIndex(u8Port, (TSInfo *)pstParamTS, &u16InTSIndex);
        if(WV_SUCCESS != eErrCode)
        {
            WEB_LOG("web_GetInTSIndex return 0x%x!", eErrCode);
            continue;
        }

        // 获取节目列表
        memset(&stIndexInfoList , 0x00, sizeof(IndexInfoList));
        eErrCode = web_GetInTSProgIndexList(u16InTSIndex, (TSInfo *)pstParamTS, &stIndexInfoList);
        if(WV_SUCCESS != eErrCode)
        {
            WEB_LOG("web_GetInTSProgIndexList return 0x%x!", eErrCode);
            continue;
        }

        for( i = 0; i<stIndexInfoList.u16IndexListNum; i++)
        {
            cJSON_AddItemToArray(pProgramlist, pProgram = cJSON_CreateObject());

            cJSON_AddStringToObject(pProgram,"servicename",
                (char *)web_StringConvertToUTF8(pstParamTS->pInputProgram[stIndexInfoList.aru16IndexList[i]].aru8ServiceName
                                                ,pstParamTS->pInputProgram[stIndexInfoList.aru16IndexList[i]].u8ServiceNameLen));
            cJSON_AddNumberToObject(pProgram, "serviceid", pstParamTS->pInputProgram[stIndexInfoList.aru16IndexList[i]].u16ServiceID);
        }
    }

    web_TSPMutexUnlock();
    
    pResponseString = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot); 

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);
    
    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;
    
    return WV_SUCCESS;
}

/****************************************************************************
 *
 * 名字:
 *      web_processGetInputProgramStatus
 *
 * 目标:
 *      获取节目信息
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      xiazhi.zhang in 2017-3-21
 *
 ****************************************************************************/
wvErrCode web_processGetInputProgramStatus(buffer * pstWriteBuff)
{
    cJSON              * pJson           = NULL;
    cJSON              * pTemp           = NULL;
    cJSON              * pRoot           = NULL; 
    cJSON              * pData           = NULL;
    cJSON              * pPids           = NULL;
    cJSON              * pPid            = NULL;
    wvErrCode            eErrCode        = WV_SUCCESS;
    U8                   u8Port          = 0;
    U8                   u8PidIndex      = 0;  
    U8                   u8ESECMPIDIndex = 0;
    char               * pResponseString = NULL;
    TSInfo             * pstParamTS      = NULL;
    Input_TSProgram    * pstInputProg    = NULL;
    Input_TSPID        * pstInputPID     = NULL;
    U16                  u16ServiceID    = 0;
    U16                  u16InTSIndex    = INDEX_INVALID_VALUE;
    U16                  u16InProgIndex  = INDEX_INVALID_VALUE;
    IndexInfoList        stPIDIndexList;
    IndexInfoList        stESECMPIDIndexList;
    U8                   u8PIDType                              =  0;
    U8                   aru8Name[MAX_STREAMTYPE_NAME_LENGTH]   = {0};

    // 入参判断
    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    } 

    // 解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取授权码
    pTemp = cJSON_GetObjectItem(pJson , "token");
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("token = %s.",pTemp->valuestring);
         //这里要判断授权码的有效性 
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    //获取端口号
    pTemp = cJSON_GetObjectItem(pJson , "port");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
         WEB_LOG("Port = %d.",pTemp->valueint);
         u8Port = (U8)pTemp->valueint;
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    //获取通道号
    pTemp = cJSON_GetObjectItem(pJson , "channel");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
         WEB_LOG("Channel = %d.",pTemp->valueint);
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    //获取节目id
    pTemp = cJSON_GetObjectItem(pJson , "serviceid");
    if ((NULL != pTemp ) && (cJSON_Number == pTemp->type))
    {
        u16ServiceID = pTemp->valueint;
         WEB_LOG("Serviceid = %d.",pTemp->valueint);
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取数据完毕
    cJSON_Delete(pJson);

    pstParamTS = web_GetTSPramaHandle();
    if (NULL == pstParamTS)
    {
        WEB_LOG("web_GetTSPrama return null!");
        return WV_ERR_WEB_BAD_PARAM;
    }

    web_TSPMutexLock();

    web_SetStreamIDBeforeGetPIDBitrate(u8Port);

    eErrCode = web_GetInTSIndex(u8Port, (TSInfo *)pstParamTS, &u16InTSIndex);
    if (WV_SUCCESS != eErrCode)
    {
        WEB_LOG("web_GetInTSIndex return 0x%x!", eErrCode);
        cJSON_Delete(pRoot);
        goto web_processGetInputProgramStatus_WEB_ERROR;
    }

    eErrCode = web_GetInProgIndex(u16InTSIndex, u16ServiceID, pstParamTS, &u16InProgIndex);
    if (WV_SUCCESS != eErrCode)
    {
        WEB_LOG("web_GetInProgIndex return 0x%x!", eErrCode);
        cJSON_Delete(pRoot);
        goto web_processGetInputProgramStatus_WEB_ERROR;
    }
    
    pstInputProg = &pstParamTS->pInputProgram[u16InProgIndex];
    
    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", WV_SUCCESS);

    cJSON_AddNullToObject(pRoot, "description");

    cJSON_AddItemToObject(pRoot, "data", pData = cJSON_CreateObject());

    cJSON_AddNumberToObject(pData, "channelindex", 1);

    cJSON_AddStringToObject(pData,"servicename",
                (char *)web_StringConvertToUTF8(pstInputProg->aru8ServiceName
                                                ,pstInputProg->u8ServiceNameLen));

    cJSON_AddNumberToObject(pData, "scrambled", web_IsProgSet2BeDescrambled(u16InProgIndex, pstParamTS));

    cJSON_AddItemToObject(pData, "pids", pPids = cJSON_CreateArray());
    
    //PCRPID
    cJSON_AddItemToArray(pPids, pPid = cJSON_CreateObject());
    cJSON_AddNumberToObject(pPid, "pid", pstInputProg->u16PCRPID);

    u8PIDType = PCR_PID;
    memset(aru8Name,0,MAX_STREAMTYPE_NAME_LENGTH);
    web_GetPIDTypeDesc(u8PIDType, 0, aru8Name);
    cJSON_AddStringToObject(pPid, "type", (const char *)aru8Name);
    
    cJSON_AddNumberToObject(pPid, "bitrate", web_GetPIDBitRate(pstInputProg->u16PCRPID));
    
    //PMTPID
    cJSON_AddItemToArray(pPids, pPid = cJSON_CreateObject());
    cJSON_AddNumberToObject(pPid, "pid", pstInputProg->u16PMTPID);
    
    u8PIDType = PMT_TABLE_PID;
    memset(aru8Name,0,MAX_STREAMTYPE_NAME_LENGTH);
    web_GetPIDTypeDesc(u8PIDType, 0, aru8Name);
    cJSON_AddStringToObject(pPid, "type", (const char *)aru8Name);
    cJSON_AddNumberToObject(pPid, "bitrate", web_GetPIDBitRate(pstInputProg->u16PMTPID));
    
    //ESPID 
    memset(&stPIDIndexList,0,sizeof(stPIDIndexList));
    eErrCode = web_GetInProgESPIDIndexList(u16InProgIndex, pstParamTS, &stPIDIndexList);
    if (WV_SUCCESS != eErrCode)
    {
        WEB_LOG("web_GetInProgESPIDIndexList return 0x%x!", eErrCode);
        cJSON_Delete(pRoot);
        goto web_processGetInputProgramStatus_WEB_ERROR;
    }

    for( u8PidIndex = 0; u8PidIndex < stPIDIndexList.u16IndexListNum; u8PidIndex++)
    {
        pstInputPID = &pstParamTS->pInputPID[stPIDIndexList.aru16IndexList[u8PidIndex]];
        
        cJSON_AddItemToArray(pPids, pPid = cJSON_CreateObject());
        cJSON_AddNumberToObject(pPid, "pid", pstInputPID->u16PID);

        u8PIDType = web_GetPIDType(pstInputPID->u16Index, pstParamTS);
        memset(aru8Name,0,MAX_STREAMTYPE_NAME_LENGTH);
        web_GetPIDTypeDesc(u8PIDType, pstInputPID->u8StreamType, aru8Name);
        cJSON_AddStringToObject(pPid, "type", (const char *)aru8Name);
        cJSON_AddNumberToObject(pPid, "bitrate", web_GetPIDBitRate(pstInputPID->u16PID));

        //ESECMPID
        memset(&stESECMPIDIndexList,0,sizeof(stESECMPIDIndexList));
        eErrCode = web_GetInPIDESECMPIDIndexList(stPIDIndexList.aru16IndexList[u8PidIndex], pstParamTS, &stESECMPIDIndexList);
        if (WV_SUCCESS != eErrCode)
        {
            WEB_LOG("web_GetInPIDESECMPIDIndexList return 0x%x!", eErrCode);
            cJSON_Delete(pRoot);
            goto web_processGetInputProgramStatus_WEB_ERROR;
        }

        for( u8ESECMPIDIndex = 0; u8ESECMPIDIndex < stESECMPIDIndexList.u16IndexListNum; u8ESECMPIDIndex++)
        {
            pstInputPID = &pstParamTS->pInputPID[stESECMPIDIndexList.aru16IndexList[u8ESECMPIDIndex]];
            
            cJSON_AddItemToArray(pPids, pPid = cJSON_CreateObject());
            cJSON_AddNumberToObject(pPid, "pid", pstInputPID->u16PID);

            u8PIDType = ESECM_PID;
            memset(aru8Name,0,MAX_STREAMTYPE_NAME_LENGTH);
            web_GetPIDTypeDesc(u8PIDType, 0, aru8Name);
            cJSON_AddStringToObject(pPid, "type", (const char *)aru8Name);
            cJSON_AddNumberToObject(pPid, "bitrate", web_GetPIDBitRate(pstInputPID->u16PID));
        }
    }

    //PSECMPID
    memset(&stPIDIndexList,0,sizeof(stPIDIndexList));
    eErrCode = web_GetInProgPSECMPIDIndexList(u16InProgIndex, pstParamTS, &stPIDIndexList);
    if (WV_SUCCESS != eErrCode)
    {
        WEB_LOG("web_GetInProgPSECMPIDIndexList return 0x%x!", eErrCode);
        cJSON_Delete(pRoot);
        goto web_processGetInputProgramStatus_WEB_ERROR;
    }

    for( u8PidIndex = 0; u8PidIndex < stPIDIndexList.u16IndexListNum; u8PidIndex++)
    {
        pstInputPID = &pstParamTS->pInputPID[stPIDIndexList.aru16IndexList[u8PidIndex]];
        
        cJSON_AddItemToArray(pPids, pPid = cJSON_CreateObject());
        cJSON_AddNumberToObject(pPid, "pid", pstInputPID->u16PID);

        u8PIDType = PSECM_PID;
        memset(aru8Name,0,MAX_STREAMTYPE_NAME_LENGTH);
        web_GetPIDTypeDesc(u8PIDType, 0, aru8Name);
        cJSON_AddStringToObject(pPid, "type", (const char *)aru8Name);
        cJSON_AddNumberToObject(pPid, "bitrate", web_GetPIDBitRate(pstInputPID->u16PID));
    }

    web_TSPMutexUnlock();

    pResponseString = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot); 

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);
    
    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;
    
    return WV_SUCCESS;

web_processGetInputProgramStatus_WEB_ERROR:
    web_TSPMutexUnlock();
    return eErrCode;
}

/****************************************************************************
 *
 * 名字:
 *      web_processGetSubBoardCIDetail
 *
 * 目标:
 *      获取节目信息
 *
 * 用法：
 *
 * 返回值： 
 *
 *      WV_SUCCESS             -处理成功
 *      其它                   -处理失败 
 *
 * 作者:
 *      xiazhi.zhang in 2017-5-11
 *
 ****************************************************************************/
wvErrCode web_processGetSubBoardCIDetail(buffer * pstWriteBuff)
{
    cJSON              * pJson           = NULL;
    cJSON              * pTemp           = NULL;
    cJSON              * pRoot           = NULL; 
    cJSON              * pData           = NULL;
    cJSON              * pCIStatus       = NULL;
    cJSON              * pProgramList    = NULL;
    cJSON              * pProgram        = NULL;
    char               * pResponseString = NULL;
    U32                  U32index        = 2;
    U32                  i               = 0;
    U32                  j               = 0;
    wvErrCode            eErrCode        = WV_SUCCESS;
    TSInfo             * pstParamTS      = NULL;
    WAVCI_Status_4_web_t stCIStatus;
    int                  arnCASystemIDList[MAX_CASYSTEM_ID_NUM] = {0};

    // 入参判断
    if ( NULL == pstWriteBuff )
    {
        WEB_LOG("Parameter Err!");
        return WV_ERR_WEB_BAD_PARAM;    
    } 

    // 解析数据
    pJson = cJSON_Parse((const char *)g_ps8RevData);
    if (!pJson) 
    {
        WEB_LOG("Error before: [%s]",cJSON_GetErrorPtr());
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取授权码
    pTemp = cJSON_GetObjectItem(pJson , "token");
    if ((NULL != pTemp ) && (cJSON_String== pTemp->type))
    {
         WEB_LOG("token = %s.",pTemp->valuestring);
         //这里要判断授权码的有效性 
    }
    else
    {
        cJSON_Delete(pJson);
        return WV_ERR_WEB_POST_PARAM;
    }

    // 获取数据完毕
    cJSON_Delete(pJson);

    pstParamTS = web_GetTSPramaHandle();
    if (NULL == pstParamTS)
    {
        WEB_LOG("web_GetTSPrama return null!");
        return WV_ERR_WEB_BAD_PARAM;
    }
    
    pRoot = cJSON_CreateObject();

    cJSON_AddNumberToObject(pRoot, "code", WV_SUCCESS);

    cJSON_AddNullToObject(pRoot, "description");

    cJSON_AddItemToObject(pRoot, "data", pData = cJSON_CreateArray());

    web_TSPMutexLock();
  
    for ( i = 0; i < U32index; i++ )
    {
        cJSON_AddItemToArray(pData, pCIStatus = cJSON_CreateObject());
        
        eErrCode = web_GetCIStatus( i, &stCIStatus ,pstParamTS);
        if (WV_SUCCESS != eErrCode)
        {
            WEB_LOG("web_GetCIStatus return 0x%x!", eErrCode);
            continue;
        }

        cJSON_AddNumberToObject(pCIStatus, "index", i);
        cJSON_AddNumberToObject(pCIStatus, "insertstatus", stCIStatus.u8CAMInsertFlag);
        cJSON_AddNumberToObject(pCIStatus, "initstatus", stCIStatus.u8CAMInitFlag);
        
        cJSON_AddStringToObject(pCIStatus,"manufacturer",
                (char *)web_StringConvertToUTF8(stCIStatus.aru8Manufacturer , WVCI_SLOT_NAME_SIZE));
        
        cJSON_AddStringToObject(pCIStatus,"camname",
                (char *)web_StringConvertToUTF8(stCIStatus.aru8CAMName , WVCI_SLOT_NAME_SIZE));

        for( j = 0; j < stCIStatus.u8CASystemNum; j++ )
        {
            arnCASystemIDList[j] = stCIStatus.aru16CASystemIDList[j];
        }
        
        cJSON_AddItemToObject(pCIStatus, "casystemidlist", 
                cJSON_CreateIntArray(arnCASystemIDList, stCIStatus.u8CASystemNum));

        cJSON_AddItemToObject(pCIStatus, "programlist", pProgramList = cJSON_CreateArray());

        for ( j = 0; j < stCIStatus.u8PIDNum; j++ )
        {
         
            cJSON_AddItemToArray(pProgramList, pProgram = cJSON_CreateObject());
            
            cJSON_AddNumberToObject(pProgram, "pid", stCIStatus.arstPIDInfoList[j].u16PID);
            cJSON_AddNumberToObject(pProgram, "type", stCIStatus.arstPIDInfoList[j].u8PIDType);
            cJSON_AddNumberToObject(pProgram, "port", stCIStatus.arstPIDInfoList[j].u16Channel);
            cJSON_AddNumberToObject(pProgram, "channel", 1);
            cJSON_AddNumberToObject(pProgram, "serviceid", stCIStatus.arstPIDInfoList[j].u16ServiceID);
            cJSON_AddNumberToObject(pProgram, "status", stCIStatus.arstPIDInfoList[j].u8DescrambleStatus);
            cJSON_AddStringToObject(pProgram,"servicename",
                (char *)web_StringConvertToUTF8(stCIStatus.arstPIDInfoList[j].aru8ServiceName , MAX_SDT_NAME_LENGTH));
        }

    }
    
    web_TSPMutexUnlock();

    pResponseString = cJSON_Print(pRoot);  
    cJSON_Delete(pRoot); 

    //将组装好的数据写到response中去
    buffer_copy_string(pstWriteBuff, pResponseString);
    
    free(pResponseString);  /* Print to text, Delete the cJSON, print it, release the string. */
    pResponseString = NULL;
    
    return WV_SUCCESS;
}


