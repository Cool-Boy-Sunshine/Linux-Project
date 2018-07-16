#ifndef __MAIN_H__
#define __MAIN_H__

#define LIS_MAX 1024
#define FILE_PATH 1024
#define FILE_SIZE 256
#define LIS_ADDR 16
#define MD5_SIZE 32

#include <stdint.h>
#include "list.h"

extern int daemon_flag;
extern char log_file[FILE_SIZE];
extern char listen_addr[LIS_ADDR];
extern int listen_port;


enum __cli_wait_st_t{
	ST_NLOGIN = 0,
	ST_STOP,
	ST_FHDR,
	ST_BODY,
	ST_FRSP,
};


typedef struct __cli_info_t{
	int         sockfd;
	uint64_t    conn_time;
	int         filefd;
	char		lis_dir[FILE_SIZE];
	char        fname[FILE_SIZE];
	char        fmd5[MD5_SIZE];
	uint64_t    fsize;
	uint64_t    lsize;
	int         wait_statu;
	int			flag;
}cli_info_t;

extern user_list *cli_head;
#endif
