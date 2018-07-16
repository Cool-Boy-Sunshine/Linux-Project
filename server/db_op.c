#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "log.h"
#include "main.h"
#include <mysql/mysql.h>
#include <hiredis/hiredis.h>
#include "../include/req.h"

int insert_data(cli_info_t *node)
{
	redisContext *ctx;
	redisReply *reply;
	const char *hostname = "127.0.0.1";
	const int port = 6379;
	int i, len;
	struct timeval timeout = { 1, 500000 }; // 1.5 seconds
	ctx = redisConnectWithTimeout(hostname, port, timeout);
	if (NULL == ctx || ctx->err){
		if (ctx){
			ERR("Connection error: %s\n", ctx->errstr);
			redisFree(ctx);
		}
		else{
			ERR("Connection error: can't allocate redis context\n");
		}
		return -1;
	}
	reply = redisCommand(ctx, "EXISTS %s", node->fmd5);
	if (reply->integer){
		printf("integer = %lld\n", reply->integer);
		reply = redisCommand(ctx, "LLEN %s", node->fmd5);
		printf("integer = %lld\n", reply->integer);
		len = reply->integer;
		for (i = 0;i<len;i++){
			reply = redisCommand(ctx, "LINDEX %s %d", node->fmd5, i);
			if (strcmp(node->fname, reply->str) == 0){
				ERR("md5 is exists,and file name is exists\n");
				return -1;
			}
		}
		ERR("md5 is exists, but file name isn't file name\n");
		reply = redisCommand(ctx, "LPUSH %s %s", node->fmd5, node->fname);
		if (reply->integer > len){
			INFO("insert success");
		}else {
			ERR("insert failed");
		}
		redisCommand(ctx, "SAVE");
	}else {
		printf("md5 isn't exists\n");
		redisCommand(ctx, "LPUSH %s %s", node->fmd5, node->fname);
		redisCommand(ctx, "SAVE");
	}
}

void* db_thread(void *args)
{
	insert_data((cli_info_t *)args);
}

int run_db(cli_info_t *node)
{
	pthread_t tid;
	int ret;
	ret = pthread_create(&tid, NULL, db_thread, (void *)node);
	if (ret != 0){ 
		ERR("create scan pthread error:%s\n", strerror(errno));
		return -1; 
	}   
	return 0;
}
