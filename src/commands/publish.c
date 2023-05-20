#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#include "../ducq.h"
#include "../ducq_srv_int.h"



ducq_state publish(struct ducq_srv *srv, ducq_i *ducq, char *buffer, size_t size) {
	ducq_state state = DUCQ_OK;
	const char *end;
	const char *route = ducq_parse_route(buffer, &end);

	if( !route) 
		state = DUCQ_EMSGINV;
	else if( ! (route = strndup(route, end-route)) )
		state = DUCQ_EMEMFAIL;
	if(state) {
		ducq_log(WARN, "%s,%s", route, ducq_state_tostr(state));

		send_ack(ducq, state);
		ducq_close(ducq);
		return state;
	}


	send_ack(ducq, DUCQ_OK);

	int count = 0;
	ducq_sub *next = NULL;
	for(ducq_sub *sub = srv->subs; sub; sub = next) {
		next = sub->next;

		if( ! ducq_route_cmp(sub->route, route) )
			continue;

		size_t len = size;
		if( ducq_send(sub->ducq, buffer, &len) )
			ducq_srv_unsubscribe(srv, sub->ducq);
		else
			count++;
	}

	ducq_log(INFO, "%s,%d notified", route, count);

	free( (void*) route ); // discard const
	return ducq_close(ducq);
}



struct ducq_cmd_t command = {
	.name = "publish",
	.doc  = "publish a message to a queue.",
	.exec =  publish
};