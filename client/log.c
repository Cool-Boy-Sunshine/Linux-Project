#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

zlog_category_t *log_handle = NULL;


int open_log(char *conf, char *mode)
{
	printf("open_log:%s\n", conf);
	if (!conf)
	{
		fprintf(stdout, "\nlog_conf_file is null!!!\n");
		return -1;
	}
	if (zlog_init(conf))
	{
		fprintf(stdout, "\nzlog_init error!!!\n");
		return -2;
	}
	log_handle = zlog_get_category(mode);
	if (NULL == log_handle)
	{
		fprintf(stdout, "\nzlog_get_category error!!!\n");
		return -3;
	}
	return 1;
}


void close_log()
{
	zlog_fini();
}

