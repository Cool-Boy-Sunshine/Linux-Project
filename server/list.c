#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include "log.h"
#include "list.h"



user_list *list_add(user_list *head, void *data)
{
	user_list *list;
	user_list *last;

	list = (user_list*)malloc(sizeof(user_list));
	if (NULL == list) return NULL;
	list->pnext = NULL;
	list->data = data;
	if (NULL == head) {
		list->prev = NULL;
		return list;
	}
	last = head;
	while (last->pnext) last = last->pnext;
	last->pnext = list;
	list->prev = last;
	return head;
}

user_list *list_del(user_list *head, void *data)
{


}

user_list *list_destory(user_list *head)
{
	user_list *list;
	while (head->prev) head = head->prev;
	list = head;
	while (list)
	{
		head = head->pnext;
		free(list);
		list = head;
	}
	return NULL;
}
