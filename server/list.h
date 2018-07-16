#ifndef __LIST_H__
#define __LIST_H__

typedef struct user_list
{
	char *data;
	struct user_list *pnext;
	struct user_list *prev;
}user_list;


user_list *user_list_add(user_list *head, void *data);
user_list *user_list_del(user_list *head, void *data);
user_list *user_list_destory(user_list *head);


#endif
