#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/inotify.h>
#include "log.h"
#include "main.h"
#include "../include/req.h"
#include "send.h"
#include "my_inotify.h"
#include "parse_conf.h"

int parse_filename(char *name)
{
	int len = strlen(name);
	char *num = "4913";
	for (int i = 0;i<len - 1;i++){
		if (name[0] == '.' || name[len - 1] == '~'){
			return 0;
		} else if (!strcmp(name, num)){
			return 0;
		}
	}
	return 1;
}


int read__inotify_fd(cli_info_t *node, int fd)
{
	int ret;
	char event_buf[1024];
	int event_size;
	int event_pos = 0;
	struct inotify_event *event;
	char *move_from = 0;

	ret = read(fd, event_buf, sizeof(event_buf));

	if(ret < (int)sizeof(*event)) {
		if(errno == EINTR)
		  return 0;
		printf("could not get event, %s\n", strerror(errno));
		return -1;
	}
	while(ret >= (int)sizeof(*event)) {
		event = (struct inotify_event *)(event_buf + event_pos);
		if (parse_filename(event->name)){
			if(event->len) {
				if(event->mask & IN_DELETE) {
					INFO("delete file: %s\n", event->name);
				} else if (event->mask & IN_CREATE){
					INFO("create file:%s\n", event->name);
					strcpy(node->fname, event->name);
					send_handle(node);
				}
			}
		}
		event_size = sizeof(*event) + event->len;
		ret -= event_size;
		event_pos += event_size;
	}
	return 0;
}



void *scan_thread(void *args)
{   
	int inotify_fd;
	int events = 0;
	int ret = 0;
	inotify_fd = inotify_init();
	if (inotify_fd < 0){
		ERR("inotify_init error\n");
		return NULL;
	}
	if (!events){
		events = IN_ALL_EVENTS;
	}
	if (access(listen_dir, F_OK) < 0){
		mkdir(listen_dir, 0777);
	}
	ret = inotify_add_watch(inotify_fd, listen_dir, events);
	while (1){
		read__inotify_fd((cli_info_t *)args, inotify_fd);
	}
	return NULL;
}

int run_scan(cli_info_t *node)
{
	pthread_t tid;
	int ret;
	ret = pthread_create(&tid, NULL, scan_thread, (void *)node);
	if (ret != 0){
		ERR("create scan pthread error:%s\n", strerror(errno));
		return -1;
	}
	return 0;

}

