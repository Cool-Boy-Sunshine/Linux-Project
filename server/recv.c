#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "recv.h"
#include "log.h"
#include "main.h"
#include "login.h"
#include "parse_conf.h"
#include "db_op.h"
#include "../include/req.h"

int recv_data(cli_info_t *node, char *buff, int len)
{
	int recv_len = 0;
	int count = 0; 
	while (recv_len < len){
		recv_len = recv(node->sockfd, buff, len, 0);
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



int send_data(cli_info_t *node, char *buff, int len)
{   
	int recv_len = 0;
	int total_len = 0;  
	while (total_len < len){
		recv_len = send(node->sockfd, buff + total_len, len - total_len, 0); 
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


int recv_handle(cli_info_t *node)
{
	int flag = 1;
	char buff[BUFF_SIZE] = {0};
	file_hdr_t fh;
	int opret = -1;
	if (access(listen_dir, F_OK) < 0){
		mkdir(listen_dir, 0777);
	}
	memset(buff, 0x00, BUFF_SIZE);
	char filename[FILE_PATH] = {0};
	int recv_len;
	while (1){
		recv_len = recv_data(node, (char *)&flag, sizeof(int));
		if (flag >= ST_FHDR && flag <= ST_FRSP){
			break;
		}
	}
	printf("flag = %d\n", flag);
	switch(flag){
		case ST_FHDR:{
			node->wait_statu = ST_FHDR;
			recv_len = recv_data(node, (char *)&fh, sizeof(file_hdr_t));
			memset(filename, 0x00, FILE_PATH);
			if (listen_dir[strlen(listen_dir) - 1] != '/'){
				strcat(listen_dir, "/");
			}
			sprintf(filename, "%s%s", listen_dir, fh.fname);
			printf("md5[%s]\n", fh.fmd5);
			strcpy(node->fname, fh.fname);
			strcpy(node->fmd5, fh.fmd5);
			//run_db(node);

			if ((opret = open(filename, O_CREAT | O_RDWR | O_TRUNC | O_APPEND, 0666)) < 0){
				ERR("oepn filename error");
				return -1;
			}
			node->filefd = opret;
			break;
					 }
		case ST_BODY:{
			node->wait_statu = ST_BODY;
			recv_len = recv_data(node, buff, BUFF_SIZE);
			if (write(node->filefd, buff, strlen(buff)) < 0){
				ERR("write error:%s", strerror(errno));
				return -1;
			}
			break;
					 }
		case ST_FRSP:{
			node->wait_statu = ST_FRSP;
			INFO("recv over");
			close(node->filefd);
			return 0;
			break;
					 }
	}
}
