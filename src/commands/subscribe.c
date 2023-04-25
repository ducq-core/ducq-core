#include <string.h>
#include <stdlib.h>
#include "subscribe.h"

#include <stdio.h>

// static
// ducq_state _close_client(ducq_sub *sub, ducq_i *ducq, ducq_state state) {
// 	ducq_sub_free(sub);
// 	return state;
// }



ducq_state subscribe(struct ducq_srv *srv, ducq_i *ducq, char *buffer, size_t size) {
	const char *end = NULL;
	const char *route = parse_route(buffer, &end);
	if(route == NULL) {
		send_ack(ducq, DUCQ_EMSGINV);
		return DUCQ_EMSGINV;
	}

	ducq_sub *sub = malloc(sizeof(ducq_sub));
	if(!sub) return DUCQ_EMEMFAIL;

	sub->ducq  = ducq_copy(ducq);
	sub->route = strndup(route, end-route);
	sub->id    = ducq_id(ducq);

	bool ok = sub->ducq && sub->route && sub->id;
	if(! ok) {
		if(sub->ducq)
			send_ack(ducq, DUCQ_EMEMFAIL);
		ducq_sub_free(sub);
		return DUCQ_EMEMFAIL;
	}

	ducq_state state = send_ack(ducq, DUCQ_OK);
	if(state != DUCQ_OK) {
		ducq_sub_free(sub);
		return state;
	}
		


	sub->next = srv->subs;
	srv->subs = sub;

	return DUCQ_OK;
}


struct ducq_cmd_t command = {
	.name = "subscribe",
	.doc  = "subscribe to a message queue.",
	.exec =  subscribe
};