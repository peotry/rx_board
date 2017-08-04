#include "parameters.h"
#include "wv_err.h"
#include "WV_LOG.h"
#include "Tuner.h"
#include "TSConstruct.h"

wvErrCode PARAM_ReadFile(const char *pcFileName, U8 *pu8Buf, U32 u32Len)
{
    int ret = 0;
    FILE *pFile = NULL;
    long lSize = 0;
    int sNeedReadLen = 0;

    if ((!pcFileName) || (!pu8Buf))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_PARAM,
            "[%s:%d]:Input Error,pcFileName[%p],pu8Buf[%p]\r\n",
            __FUNCTION__, __LINE__, pcFileName, pu8Buf);

        return WV_ERR_PARAMS;
    }

    ret = access(pcFileName, F_OK);
    if (ret < 0)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_PARAM,
            "[%s:%d]:pcFileName[%s] dont exists\r\n",
                    __FUNCTION__, __LINE__, pcFileName);
            
        return WV_ERR_FILE_NOT_EXISTING;
    }

    pFile = fopen(pcFileName, "rb");
    if (!pFile)  
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_PARAM,
            "[%s:%d]:open pcFileName[%s] fail\r\n",
                    __FUNCTION__, __LINE__, pcFileName);
        return WV_ERR_FILE_CAN_NOT_READE;
    }
    
    fseek (pFile, 0, SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);

    sNeedReadLen = (u32Len > lSize) ? lSize : u32Len;

    ret = fread(pu8Buf, 1, sNeedReadLen, pFile);

    fclose(pFile);

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_PARAM,
        "[%s:%d]:fread %s:lSize[%ld],u32Len[%u],sNeedReadLen[%d],ret[%d]\r\n",
        __FUNCTION__, __LINE__, pcFileName, lSize, u32Len, sNeedReadLen, ret);

    if (ret < 0)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_PARAM,
            "[%s:%d]:read pcFileName[%s] error\r\n",
                    __FUNCTION__, __LINE__, pcFileName);
        return WV_ERR_FAILURE;
    }

    return WV_SUCCESS;
}

wvErrCode PARAM_WriteFile(const char *pcFileName, U8 *pu8Buf, U32 u32Len)
{
    int ret = 0;
    FILE *pFile = NULL;

    if ((!pcFileName) || (!pu8Buf))
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_PARAM,
            "[%s:%d]:Input Error,pcFileName[%p],pu8Buf[%p]\r\n",
            __FUNCTION__, __LINE__, pcFileName, pu8Buf);

        return WV_ERR_PARAMS;
    }
 
    pFile = fopen(pcFileName, "wb");
    if (!pFile)  
    {  
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_PARAM,
            "[%s:%d]:open pcFileName[%s] fail\r\n",
                    __FUNCTION__, __LINE__, pcFileName);
        return WV_ERR_FAILURE;
    }
    
    ret = fwrite(pu8Buf, 1, u32Len, pFile);

    fclose(pFile);

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_PARAM,
        "[%s:%d]:fwrite %s:u32Len[%u],ret[%d]\r\n",
        __FUNCTION__, __LINE__, pcFileName, u32Len, ret);

    if (ret < 0)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_PARAM,
            "[%s:%d]:fwrite pcFileName[%s] error,ret[%d]\r\n",
                    __FUNCTION__, __LINE__, pcFileName, ret);
        return WV_ERR_FAILURE;
    }

    return WV_SUCCESS;
}

/*****************************************************************************
  Function:     web_Default
  Description:  »Ö¸´³ö³§ÉèÖÃ
  Input:        
                
  Output:       
  Return:       
  Author:       Momouwei 2017.04.14
*****************************************************************************/
void PARAM_Default(void)
{
    wvErrCode enErrCode = WV_SUCCESS;
    
    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_PARAM,
        "***************** PARAM_Default ****************\r\n");

    TSP_MutexLock();
    enErrCode = TSP_ClearTSConfiguration(TSP_GetTSParamHandle());
    TSP_MutexUnlock();
    
    if (WV_SUCCESS != enErrCode)
    {
        log_printf(LOG_LEVEL_ERROR, LOG_MODULE_TSP,
            "[%s:%d]TSP_ClearTSConfiguration Error:enErrCode[%08X]\r\n",
            __FUNCTION__, __LINE__, enErrCode);
    }

    Tuner_Default();

    log_printf(LOG_LEVEL_DEBUG, LOG_MODULE_PARAM,
        "*************************************************\r\n");
}


