#ifndef __RECV_H__
#define __RECV_H__
#include "main.h"

#define BUFF_SIZE 1024

int recv_data(cli_info_t *node, char *buff, int len);
int send_data(cli_info_t *node, char *buff, int len);
int recv_handle(cli_info_t *node);


#endif
