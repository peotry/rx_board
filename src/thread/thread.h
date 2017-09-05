#ifndef INCLUDE_TOOLS_THREAD_H
#define INCLUDE_TOOLS_THREAD_H

#include <pthread.h>

#include "appGlobal.h"

typedef void *(*ThreadFunc)(void *arg); 

pthread_t  Thread_NewDetach(ThreadFunc func, void *thr_data, const char *funcname, const char *filename, U32 line);

#define THREAD_NEW_DETACH(func, thr_data, funcname) Thread_NewDetach(func, thr_data, funcname, __FILE__, __LINE__)


#endif
