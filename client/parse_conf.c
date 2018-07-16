#include <stdio.h>
#include <unistd.h>
#include <libconfig.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include "log.h"
#include "parse_conf.h"
#include "main.h"

char log_file[FILE_SIZE] = {0};
char listen_addr[LIS_ADDR] = {0};
int listen_port = -1;
int login_timeout = -1;
char listen_dir[FILE_SIZE] = {0};

//read conf.cfg
int parse_conf(char *path)
{
	config_t conf;
	if (NULL == path){
		printf("\nconfig path is null\n");
		return -1;
	}
	config_init(&conf);
	if (!config_read_file(&conf, path)){
		printf("\nread config_file error!!!\n");
		return -1;
	}
	const char *ptr = NULL;
	//path
	if (!config_lookup_string(&conf, "conf.log_file", &ptr)){
		printf("\nobtain the log_file_path error!!!\n");
		return -2;
	}
	memcpy(log_file, ptr, strlen(ptr));
	//IP addr
	if (!config_lookup_string(&conf, "conf.listen_addr",&ptr)){
		printf("\nobtain the log_listen_addr error!!!\n");
		return -3;
	}
	memcpy(listen_addr, ptr, strlen(ptr));
	//port
	if (!config_lookup_int(&conf, "conf.listen_port", &listen_port)) {
		printf("\nobtain listen port error!!!\n");
		return -4;
	}
	//timeout
	if (!config_lookup_int(&conf, "conf.cli_timeout", &login_timeout)) {
		printf("\nobtain login_timeout error!!!\n");
		return -5;
	}
	//dir
	if (!config_lookup_string(&conf, "conf.listen_dir", &ptr)) {
		printf("\nobtain listen_dir error!!!\n");
		return -6;
	}
	memcpy(listen_dir, ptr, strlen(ptr));
	printf("log:[%s],addr:[%s], port[%d], timeout[%d],listen_dir[%s]\n", log_file, listen_addr, listen_port, login_timeout, listen_dir);
	config_destroy(&conf);
	return 0;
}

