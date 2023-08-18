#include <stdio.h>
#include <string.h>

#include "../ducq_srv.h"


struct list_ctx {
	char  *buffer;
	size_t left;
};
static
ducq_loop_t _list(ducq_i *ducq, char *route, void *ctx) {
	struct list_ctx *list = (struct list_ctx*) ctx;

	size_t len = snprintf(list->buffer, list->left, "%s,%s\n", ducq_id(ducq), route);

	list->buffer += len;
	list->left   -= len;
	
	return list->left > 0 ? DUCQ_LOOP_CONTINUE : DUCQ_LOOP_BREAK;
}

ducq_state list_subscriptions(struct ducq_srv *srv, ducq_i *ducq, char *message, size_t size) {
	(void) message;
	(void) size;

	ducq_log(INFO, "");

	char payload [DUCQ_MSGSZ] = "";
	struct list_ctx list = {
		.buffer = payload,
		.left = DUCQ_MSGSZ
	};
	ducq_srv_loop(srv, _list, &list);
	size_t len = DUCQ_MSGSZ - list.left;

	ducq_send(ducq, payload, &len);
	return ducq_close(ducq);
}



struct ducq_cmd_t command = {
	.name = "list_subscriptions",
	.doc  = "list all current subscriptions as csv: <id>,<route>\\n.",
	.exec =  list_subscriptions
};
