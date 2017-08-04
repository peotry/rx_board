#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <syslog.h>
#include <time.h>
#include <pthread.h>

#include "AppGlobal.h"

wvErrCode PARAM_ReadFile(const char *pcFileName, U8 *pu8Buf, U32 u32Len);
wvErrCode PARAM_WriteFile(const char *pcFileName, U8 *pu8Buf, U32 u32Len);
void PARAM_Default(void);

#endif

