#include "event/event.h"

#include "WV_log.h"

#include <errno.h>
#include <unistd.h>
#include <sys/eventfd.h>

S32 Event_CreateFd(void)
{
	S32 fd = eventfd(0, 0);

	if(fd < 0)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: Event_CreateFd Failed!!");
	}

	return fd;
}


S32 Event_Read(S32 fd)
{
	S32 ret = 0;
	S32 num = 0;
    S32 timeout = 5;
	U64 u64Val = 0;
	char err_buf[ERR_BUF_LEN] = {0};

again:
	num = read(fd, &u64Val, sizeof(u64Val));
    if((num < 0) && (EINTR == errno))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: read fail by EINTR");
        if(--timeout < 0)
        {
            ret = EVENT_ERR;
        }
        else
        {
            goto again;
        }
    }
    else if((num <= 0) && ( EINTR != errno))
    {
		ERR_STRING(err_buf);
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: %s", err_buf);
        ret = EVENT_ERR;
    }
    else
    {
        ret = EVENT_SUCCESS;
    }

	return ret;
}


S32 Event_Write(S32 fd)
{
	S32 ret = 0;
	S32 num = 0;
	U64 u64Val = 1;
	char err_buf[ERR_BUF_LEN] = {0};

	if(fd < 0)
	{
		printf("fd < 0\n");
		return 0;
	}
	num = write(fd, &u64Val, sizeof(u64Val));
	if(num != sizeof(u64Val))
	{
		ERR_STRING(err_buf);
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Error: %s!", err_buf);
		ret = EVENT_ERR;
	}

	return ret;
}



