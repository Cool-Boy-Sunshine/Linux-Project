#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <libconfig.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "main.h"
#include "parse_conf.h"
#include "parse_parm.h"
#include "init_signal.h"
#include "log.h"
#include "login.h"

char *conf_path = "/home/sunshine/myproject/etc/strans.cfg";

int main(int argc, char *argv[])
{
	int ret = 0;
	//解析参数信息
	init_opt(argc, argv);
	//判断是否进入守护进程
	if (is_daemon)
	{
		daemon(1, 0);
	}
	//初始化信号
	init_signal();
	//读取件
	parse_conf(conf_path);
	//启动日志模配置文块
	if (is_daemon){
		if (open_log(log_file, "s_f_cat") != 1){
		  return -1;
		}	
	}else{
		if ((ret = open_log(log_file, "o_cat")) != 1){
			return -1;
		}
	}
	ret = listen_start();
	if (ret < 0){
		ERR("listen_start error");
		return -1;
	}
	INFO("listen...");
	//char dir[256] = "./dir";
	//run_scan();
	//printf("scaning...\n");
	while(1);
	return 0;
}
