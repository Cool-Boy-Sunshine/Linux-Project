#ifndef __LOGIN_H__
#define __LOGIN_H__
#include "main.h"


cli_info_t* new_cli(int fd);
void setnonblock(int sockfd);
void *login(void *arg);
int listen_start();

#endif
