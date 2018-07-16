#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <malloc.h>
#include <time.h>
#include <fcntl.h>
#include "login.h"
#include "log.h"
#include "main.h"
#include "list.h"
//#include "scan.h"
#include "send.h"
#include "parse_conf.h"
#include "my_inotify.h"
#include "../include/req.h"


user_list *cli_head = NULL;


cli_info_t* new_cli(int fd)
{
	cli_info_t *node = NULL;
	node = (cli_info_t*)malloc(sizeof(cli_info_t));
	if (node == NULL){
		return NULL;
	}
	memset(node, 0x00, sizeof(cli_info_t));
	node->sockfd = fd;
	node->conn_time = time(NULL);
	node->wait_statu = ST_FHDR;
	return node;
}


void setnonblock(int sockfd)
{
	int flag = -1;
	flag = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_GETFL, flag | O_NONBLOCK);
}





int request_login(cli_info_t *node)
{
	com_req_t com_req;
	com_req_t com_rsp;
	login_info_t login_req;
	memcpy(login_req.username, "sunshine", strlen("sunshine"));
	memcpy(login_req.password, "123456", strlen("123456"));

	com_req.type = REQ_LOGIN;
	com_req.len = sizeof(com_req_t);
	//发送请求登陆命令
	if (send_data(node, (char *)&com_req, com_req.len) < 0){
		ERR("send REQ_LOGIN error");
		return -1;
	}
	//发送用户名和密码信息
	if (send_data(node, (char *)&login_req, sizeof(login_info_t)) < 0){
		ERR("send login_req error");
		return -1;
	}

	//接收回复信息
	if (recv_data(node, (char *)&com_rsp, com_req.len) < 0){
		ERR("recv com_rsp error");
		return -1;
	}
	if (com_rsp.type != RSP_LOGIN){
		ERR("login error");
		return -1;
	}
	return 0;
}




void *login(void *arg)
{
	int serfd, ret, len, i;
	int epfd, nfds;
	struct epoll_event ev, evs[LIS_MAX];
	struct sockaddr_in ser_addr;
	struct sockaddr_in cli_addr;
	len = sizeof(struct sockaddr_in);
	i = 0;
	int opt = 1;

	serfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serfd < 0){
		ERR("create client socket error:%s", strerror(errno));
		return NULL;
	}

	setsockopt(serfd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(int));
	setsockopt(serfd, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(int));

	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(listen_port);
	ser_addr.sin_addr.s_addr = inet_addr(listen_addr);

	cli_addr.sin_family = AF_INET;
	cli_addr.sin_port = htons(6000);
	cli_addr.sin_addr.s_addr = inet_addr("192.168.121.128");

	ret = bind(serfd, (struct sockaddr*)&cli_addr, len);
	if (ret < 0){
		ERR("bind client addr error:%s", strerror(errno));
		return NULL;
	}
	ret = connect(serfd, (struct sockaddr*)&ser_addr, len);
	if (ret < 0){
		ERR("connect error:%s", strerror(errno));
		return NULL;
	}
	setnonblock(serfd);
	cli_info_t *node;
	node = new_cli(serfd);
	if (run_scan(node) < 0){
		ERR("create scan pthread error");
		return NULL;
	}
	INFO("scaning...");
	while (1);
	return NULL;
}

int connect_start()
{
	pthread_t tid;
	int ret;
	ret = pthread_create(&tid, NULL, login, NULL);
	if (ret != 0){
		ERR("create client login pthread error:%s", strerror(errno));
		return -1;
	}
	return 0;
}
