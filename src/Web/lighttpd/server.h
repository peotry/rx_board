#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdbool.h>
#include "base.h"
#include "AppGlobal.h"

#define LIGHTTPD_PATH WELLAV_APP"tools/lighttpd/"           /* server 路径 */
#define LIBRARY_DIR   LIGHTTPD_PATH"lib"                    /* 加载模块路径 */
#define CONFIG_FILE   LIGHTTPD_PATH"config/lighttpd.conf"   /* 配置文件路径 */
#define MAX_UPGRADE_FILE_SIZE  128 * 1024 * 1024            /* 升级文件大小 128M ，超过限制将被分包 */


//#define USER_FILE     LIGHTTPD_PATH"config/lighttpd.user"

typedef struct {
	char *key;
	char *value;
} two_strings;

typedef enum { CONFIG_UNSET, CONFIG_DOCUMENT_ROOT } config_var_t;

int config_read(server *srv, const char *fn);
int config_set_defaults(server *srv);
buffer *config_get_value_buffer(server *srv, connection *con, config_var_t field);
int lighttpd_shutdown(bool isShutDown);
int lighttpd_configuration(int argc, char **argv);
bool lighttpd_isServerClose(void);

#endif

