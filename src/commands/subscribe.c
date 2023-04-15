#include <string.h>
#include <stdlib.h>
#include "subscribe.h"



ducq_state subscribe(struct ducq_srv *srv, ducq_i *ducq, char *buffer, size_t size) {
	const char *end = NULL;
	const char *route = parse_route(buffer, &end);
	if(route == NULL)
		return DUCQ_EMSGINV;

	ducq_sub *sub = malloc(sizeof(ducq_sub));
	if(!sub) return DUCQ_EMEMFAIL;
	sub->ducq  = ducq_copy(ducq);
	sub->route = strndup(route, end-route);
	sub->id    = ducq_id(ducq);

	if(sub->ducq && sub->route && sub->id) {
		sub->next = srv->subs;
		srv->subs = sub;
		return DUCQ_OK;
	}

	if(sub->ducq)  ducq_free(sub->ducq);
	if(sub->route) free(sub->route);
	free(sub);
	return DUCQ_EMEMFAIL;
}


struct ducq_cmd_t command = {
	.name = "subscribe",
	.doc  = "subscribe to a message queue.",
	.exec =  subscribe
};