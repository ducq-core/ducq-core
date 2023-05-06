#include <string.h>
#include <stdlib.h>
#include "subscribe.h"

#include <stdio.h>



ducq_state subscribe(struct ducq_srv *srv, ducq_i *ducq, char *buffer, size_t size) {
	const char *end = NULL;
	const char *route = parse_route(buffer, &end);
	if(route == NULL) {
		send_ack(ducq, DUCQ_EMSGINV);
		return DUCQ_EMSGINV;
	}

	ducq_sub *sub = calloc(1, sizeof(ducq_sub));
	if(!sub) return DUCQ_EMEMFAIL;

	if( ! (sub->ducq  = ducq_copy(ducq))           ) goto mem_failed;
	if( ! (sub->route = strndup(route, end-route)) ) goto mem_failed;
	if( ! (sub->id    = ducq_id(sub->ducq))        ) goto mem_failed;


	ducq_state state = send_ack(ducq, DUCQ_OK);
	if(state != DUCQ_OK) {
		ducq_sub_free(sub);
		return state;
	}

	sub->next = srv->subs;
	srv->subs = sub;

	return DUCQ_OK;

	mem_failed:
		send_ack(ducq, DUCQ_EMEMFAIL);
		ducq_sub_free(sub);
		return DUCQ_EMEMFAIL;
}


struct ducq_cmd_t command = {
	.name = "subscribe",
	.doc  = "subscribe to a message queue.",
	.exec =  subscribe
};