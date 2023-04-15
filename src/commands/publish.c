#include <stdio.h> // FOR TESTS ONLY
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
	if(route == NULL)
		return DUCQ_EMSGINV;

	for(ducq_sub *sub = srv->subs; sub; sub = sub->next) {
		if( ! route_cmp(sub->route, route, end-route) )
			continue;

		size_t len = size;
		ducq_send(sub->ducq, buffer, &len);
			// ignore error, unsubscribe, should be done on next reactor's read or keepalive
	}

	return DUCQ_OK;
}



struct ducq_cmd_t command = {
	.name = "publish",
	.doc  = "publish a message to a queue.",
	.exec =  publish
};