#include <string.h>
#include <stdlib.h>

#include "../ducq.h"
#include "../ducq_srv.h"
#include "../ducq_srv_int.h"




ducq_state subscribe(struct ducq_srv *srv, ducq_i *ducq, char *buffer, size_t size) {
	const char *end = NULL;
	const char *route = ducq_parse_route(buffer, &end);
	if(route == NULL) {
		send_ack(ducq, DUCQ_EMSGINV);
		ducq_log(WARN, "%s", ducq_state_tostr(DUCQ_EMSGINV) );
		return DUCQ_EMSGINV;
	}

	ducq_sub *sub = calloc(1, sizeof(ducq_sub));
	if(!sub) return DUCQ_EMEMFAIL;

	if( ! (sub->ducq  = ducq_copy(ducq))           ) goto mem_failed;
	if( ! (sub->route = strndup(route, end-route)) ) goto mem_failed;
	if( ! (sub->id    = ducq_id(sub->ducq))        ) goto mem_failed;


	ducq_state state = send_ack(ducq, DUCQ_OK);
	if(state != DUCQ_OK) {
		ducq_log(WARN, "%s,%s,%s", sub->id, sub->route, ducq_state_tostr(state));
		ducq_sub_free(sub);
		return state;
	}

	sub->next = srv->subs;
	srv->subs = sub;


	ducq_log(INFO, "%s", sub->route);
	return DUCQ_OK;

	mem_failed:
		ducq_log(ERROR, "%s", ducq_state_tostr(DUCQ_EMEMFAIL));
		send_ack(ducq, DUCQ_EMEMFAIL);
		ducq_sub_free(sub);
		return DUCQ_EMEMFAIL;
}


struct ducq_cmd_t command = {
	.name = "subscribe",
	.doc  = "subscribe to a message queue.",
	.exec =  subscribe
};