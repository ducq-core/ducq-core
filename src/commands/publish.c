#include <string.h>
#include <stdbool.h>

#include "../ducq_srv_int.h"

static
bool route_cmp(const char *a, const char *b, size_t size) {
	return strlen(a) == size
			&& strncmp(a, b, size) == 0;
}




ducq_state publish(struct ducq_srv *srv, ducq_i *ducq, char *buffer, size_t size) {
	const char *end;
	const char *route = parse_route(buffer, &end);
	if(route == NULL) {
		send_ack(ducq, DUCQ_EMSGINV);
		ducq_close(ducq);
		return DUCQ_EMSGINV;
	}


	send_ack(ducq, DUCQ_OK);
	
	ducq_sub *sub = srv->subs;
	while(sub) {
		ducq_sub *next = sub->next;
		if( route_cmp(sub->route, route, end-route) ) {
			size_t len = size;
			if( ducq_send(sub->ducq, buffer, &len) )
				ducq_srv_unsubscribe(srv, sub->ducq);
		}
		sub = next;
	}

	return ducq_close(ducq);;
}



struct ducq_cmd_t command = {
	.name = "publish",
	.doc  = "publish a message to a queue.",
	.exec =  publish
};