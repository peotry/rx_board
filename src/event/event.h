#ifndef INCLUDE_TOOLS_EVENT_H
#define INCLUDE_TOOLS_EVENT_H

#include "appGlobal.h"

#define EVENT_SUCCESS        (0)
#define EVENT_ERR            (-1)

S32 Event_CreateFd(void);
S32 Event_Read(S32 fd);
S32 Event_Write(S32 fd);

#endif
