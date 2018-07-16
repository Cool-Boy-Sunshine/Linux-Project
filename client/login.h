#ifndef __LOGIN_H__
#define __LOGIN_H__
#include "main.h"

cli_info_t* new_cli(int fd);
void setnonblock(int sockfd);
int request_login(cli_info_t *node);
void *login(void *arg);
int connect_start();

#endif
