#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>


int main(int argc, char *argv[])
{
	int fd;
	struct stat st;
	fd = open("./log.c", O_RDONLY);
	if (fd < 0){
		printf("open error\n");
		return -1;
	}
	//fstat(fd, &st);
	if (stat("./log.c", &st) < 0){
		printf("get file size error\n");
	}
	printf("file size:[%ld]\n", st.st_size);
	return 0;
}
