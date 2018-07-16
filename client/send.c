#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "send.h"
#include "main.h"
#include "login.h"
#include "../include/req.h"
#include "log.h"
#include "calc_md5.h"


int send_data(cli_info_t *node, char *buff, int len)
{
	int recv_len = 0;
	int total_len = 0;  
	while (total_len < len){
		recv_len = send(node->sockfd, buff + total_len, len - total_len, 0); 
		//printf("send_data recv_len = %d\n", recv_len);
		if (recv_len < 0){ 
			if (errno == EAGAIN || errno == EINTR){
				node->flag = -1;
				usleep(1000);
				continue;
			}   
			return -1; 
		}   
		total_len += recv_len;
	}   
	return total_len;
}


int recv_data(cli_info_t *node, char *buff, int len)
{   
	//循环接收数据，知道接收数据长度等于指定长度
	//t_len 已经接收的数据总长度
	//r_len 当前recv函数实际接收的长度
	int recv_len = 0;
	int count = 0;
	while (recv_len < len){
		recv_len = recv(node->flag, buff, len, 0);
		if (errno == EAGAIN || errno == EINTR){
			if (count++ > 5){
				return -1;
			}
			node->flag = -1;
			usleep(1000);
			continue;
		}
		if (recv_len != len){
			return -1;
		}
	}
	return recv_len;
}

int send_handle(cli_info_t *node)
{
	char buff[BUFF_SIZE] = {0};
	int opret;
	int s_len;
	int send_len = BUFF_SIZE;
	char filename[FILE_PATH] = {0};
	//printf("send_handle node->fname[%s]\n", node->fname);
	memset(filename, 0x00, FILE_PATH);
	if (listen_dir[strlen(listen_dir) - 1] != '/'){
		strcat(listen_dir, "/");
	}
	sprintf(filename, "%s%s", listen_dir, node->fname);
	printf("filename:%s\n", filename);
	if ((opret = open(filename, O_RDONLY)) < 0){
		ERR("open file error");
		return -1;
	}
	file_hdr_t fh;
	node->filefd = opret;
	node->wait_statu = ST_FHDR;
	while (1){
		usleep(1000);
		//sleep(1);
		/*
		if (node->wait_statu == ST_NLOGIN){
			if (request_login(node) < 0){
				ERR("request_login error");
				sleep(1);
				continue;
			}
			node->wait_statu = ST_FHDR;
		}else if (node->wait_statu == ST_FHDR){*/
		printf("send flag:[%d]\n", node->wait_statu);
		switch(node->wait_statu){
			case ST_FHDR:{
				calc_md5(node);
				memset(&fh, 0x00, sizeof(file_hdr_t));
				memcpy(fh.fname, node->fname, strlen(node->fname));
				fh.flen = strlen(node->fname);
				strcpy(fh.fmd5, node->fmd5);
				printf("filename[%s], len[%lld], md5[%s]\n", fh.fname, fh.flen, fh.fmd5);
				send_data(node, (char *)&node->wait_statu, sizeof(node->wait_statu));
				send_data(node, (char *)&fh, sizeof(file_hdr_t));
				node->wait_statu = ST_BODY;
				break;
						 }
			case ST_BODY:{
				memset(buff, 0x00, BUFF_SIZE);
				//printf("send file data\n");
				send_data(node, (char *)&node->wait_statu, sizeof(node->wait_statu));
				s_len = 0;
				while (s_len < send_len){
					int re_ret = read(node->filefd, buff, send_len - s_len);
					send_data(node, buff, BUFF_SIZE);
					s_len += re_ret;
					if (re_ret == 0){
						close(opret);
						node->filefd = -1;
						node->wait_statu = ST_FRSP;
						break;
					}
				}
				break;
						 }
			case ST_FRSP:{
				//printf("ST_FRSP send status:%d\n", node->wait_statu);
				com_req_t com_rsp;
				send_data(node, (char *)&node->wait_statu, sizeof(node->wait_statu));
				INFO("send over");
				return 0;
				break;
						 }
		}
	}
}


