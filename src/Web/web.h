#ifndef _WV_WEB_H_
#define _WV_WEB_H_

#include "WV_log.h"
#include "appGlobal.h"

#define  WEB_SERVER_PORT 80

typedef enum 
{ 
    WEB_SUCCESS, 
    WEB_FAILURE, 
    WEB_TIMEOUT,
    WEB_RESTART_SUCCESS,
    WEB_RESTART_FAILURE,
} wvWebErrCode;

wvWebErrCode web_Start(void);
wvWebErrCode web_MonitorServer(void);

#endif

