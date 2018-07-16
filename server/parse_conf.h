#ifndef __INIT_OPT_H__
#define __INIT_OPT_H__
#include "main.h"


extern char log_file[FILE_SIZE];
extern char listen_addr[LIS_ADDR];
extern int listen_port;
extern int login_timeout;
extern char listen_dir[FILE_SIZE];

int parse_conf(char *path);

#endif
