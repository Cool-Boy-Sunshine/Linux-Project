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
#include "recv.h"
#include "list.h"
#include "parse_conf.h"
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

int check_login(cli_info_t *node)
{
	com_req_t com_req;
	com_req_t com_rsp;
	login_info_t login_req;
	if (recv_data(node, (char *)&com_req, sizeof(com_req_t)) < 0){
		return -1;
	}
	//请求用户登录
	if (com_req.type != REQ_LOGIN){
		return -1;
	}
	if (recv_data(node, (char *)&login_req, com_req.len) < 0){
		return -1;
	}
	//回复用户登录请求
	com_rsp.type = RSP_LOGIN;
	com_rsp.len = 0;
	if (send_data(node, (char *)&com_rsp, sizeof(com_req_t)) < 0){
		return -1;
	}
	return 0;
}


void *login(void *arg)
{
	int serfd, clifd, ret, len, i;
	int epfd, nfds;
	struct epoll_event ev, evs[LIS_MAX];
	struct sockaddr_in ser_addr;
	struct sockaddr_in cli_addr;
	len = sizeof(struct sockaddr_in);
	i = 0;
	int opt = 1;

	serfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serfd < 0){
		ERR("create listen socket error:%s", strerror(errno));
		return NULL;
	}

	setsockopt(serfd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(int));
	setsockopt(serfd, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(int));

	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(listen_port);
	ser_addr.sin_addr.s_addr = inet_addr(listen_addr);

	ret = bind(serfd, (struct sockaddr*)&ser_addr, len);
	if (ret < 0){
		ERR("bind listen addr error:%s", strerror(errno));
		return NULL;
	}
	ret = listen(serfd, LIS_MAX);
	if (ret < 0){
		ERR("listen addr error:%s", strerror(errno));
		return NULL;
	}
	setnonblock(serfd);
	cli_info_t *node = NULL;
	epfd = epoll_create(LIS_MAX);
	node = new_cli(serfd);
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = (void *)node;

	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, serfd, &ev);
	if (ret != 0){
		ERR("epoll ctl add error:%s", strerror(errno));
		return NULL;
	}

	for (i = 0;i<LIS_MAX;i++){
		evs[i].data.ptr = NULL;
	}
	char buff[BUFF_SIZE] = {0};
	int flag = 11;
	file_hdr_t fh;
	int opret = -1;
	while (1){
		nfds = epoll_wait(epfd, evs, LIS_MAX, 3000);
		if (nfds < 0){
			ERR("epoll_wait error:%s", strerror(errno));
			usleep(1000);
			continue;
		}
		else if (nfds == 0){
			continue;
		}
		for (i = 0;i<nfds;i++){
			memset(buff, 0x00, BUFF_SIZE);
			node = (cli_info_t*)evs[i].data.ptr;
			if (node->sockfd == serfd){
				clifd = accept(serfd, (struct sockaddr*)&cli_addr, &len);
				if (clifd < 0){
					ERR("accept error:%s", strerror(errno));
					continue;
				}
				printf("new accept----[%s:%d]\n", inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));
				setnonblock(clifd);
				node = new_cli(clifd);
				if (node == NULL){
					ERR("new client node error");
					close(clifd);
					continue;
				}
				//将客户端节点添加入客户端头部链表
				//cli_head = list_add(cli_head, node);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.ptr = (void *)node;
				ret = epoll_ctl(epfd, EPOLL_CTL_ADD, clifd, &ev);
				if (ret < 0){
					ERR("EPOLL_CTL_ADD client error");
				}
			}else if (evs[i].events & EPOLLIN){
				//check_login(node);
				printf("evs[i].events:[%d]\n", node->wait_statu);
				if (recv_handle(node) == 0){
					printf("recv_handle wait_statu:[%d]\n", node->wait_statu);
					node->wait_statu = ST_FHDR;
				}
				if (node->flag == -1){
					if ((epoll_ctl(epfd, EPOLL_CTL_DEL, node->sockfd, &ev)) < 0){
						ERR("EPOLL_CTL_DEL error");
					}
					INFO("close");
					close(node->sockfd);
				}
			}
		}
	}
	free(node);
	return NULL;
}

int listen_start()
{
	pthread_t tid;
	int ret = 0;
	ret = pthread_create(&tid, NULL, login, NULL);
	if (ret != 0){
		ERR("create login pthread error:%s", strerror(errno));
		return -1;
	}
	return 0;
}
