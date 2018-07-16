#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <openssl/md5.h>
#include "main.h"
#include "log.h"
#include "calc_md5.h"


int calc_md5(cli_info_t *node)
{
	MD5_CTX ctx;
	char filename[FILE_PATH] = {0};
	unsigned char buff[1024] = {0};
	char test[32] = {0};
	FILE *fp = NULL;
	int len = 0, i;
	unsigned char md5[16] = {0};

	memset(filename, 0x00, FILE_PATH);
	sprintf(filename, "%s%s", listen_dir, node->fname);
	fp = fopen(filename, "rb");
	if (fp == NULL){
		ERR("fopen error");
		return -1;
	}
	MD5_Init(&ctx);
	while((len = fread(buff, 1, sizeof(buff), fp)) > 0){
		MD5_Update(&ctx, buff, len);
		memset(buff, 0 ,sizeof(buff));
	}
	MD5_Final(md5, &ctx);  
	int j = 0;
	for (i = 0;i<16;i++, j += 2){ 
		//printf("%02x", md5[i]);
		snprintf(&test[j], sizeof(test), "%02x", (unsigned int)md5[i]);
	}   
	strcpy(node->fmd5, test);
	//printf("%s\n", test);
	if (fp){
		fclose(fp);
	}
	return 0;
}
