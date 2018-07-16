#ifndef __LOG_H__
#define __LOG_H__

#include <zlog.h>

extern zlog_category_t	*log_handle;
#define INFO(...) zlog_info(log_handle, __VA_ARGS__)
//level = 20
#define DBG(...) zlog_debug(log_handle, __VA_ARGS__)			
//level = 80
#define WAR(...) zlog_warn(log_handle, __VA_ARGS__)			
//level = 100
#define ERR(...) zlog_error(log_handle, __VA_ARGS__)			

int open_log(char *conf, char *mode);
void close_log();

#endif
