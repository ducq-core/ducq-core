#include <stdio.h>
#include <string.h>

#include "../ducq_srv_int.h"



ducq_state list_subscriptions(struct ducq_srv *srv, ducq_i *ducq, char *message, size_t size) {
	(void) message;
	(void) size;

	ducq_log(INFO, "");

	char payload[DUCQ_MSGSZ] = "";
	ducq_state state = DUCQ_OK;

	for(ducq_sub *sub = srv->subs; sub; sub = sub->next) {
		size_t len = snprintf(payload, DUCQ_MSGSZ, "%s,%s\n", sub->id, sub->route);
		
		state = ducq_send(ducq, payload, &len);
		if(state) break;
	}

	ducq_close(ducq);
	return state;
}



struct ducq_cmd_t command = {
	.name = "list_subscriptions",
	.doc  = "list all current subscriptions as csv: <id>,<route>\\n.",
	.exec =  list_subscriptions
};