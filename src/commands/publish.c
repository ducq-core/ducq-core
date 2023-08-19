#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#include "../ducq.h"
#include "../ducq_reactor.h"

struct pub_ctx {
	char *route;
	char *buffer;
	size_t size;
	int count;
};

static
ducq_loop_t _publish(ducq_i *ducq, char *route, void *ctx) {
	struct pub_ctx *msg = (struct pub_ctx*) ctx; 
	size_t len = msg->size;

	if( ! ducq_route_cmp(route, msg->route) )
		return DUCQ_LOOP_CONTINUE;	
	if( ducq_send(ducq, msg->buffer, &len) )
		return DUCQ_LOOP_DELETE;	

	msg->count++;
	return DUCQ_LOOP_CONTINUE;	
}

ducq_state publish(struct ducq_reactor *reactor, ducq_i *ducq, char *buffer, size_t size) {
	struct pub_ctx msg = {
		.route  = NULL,
		.buffer = buffer,
		.size   = size,
		.count  = 0
	};
	const char *end;
	const char *route = ducq_parse_route(buffer, &end);
	ducq_state state =
		! route                                   ? DUCQ_EMSGINV  :
		! (msg.route = strndup(route, end-route)) ? DUCQ_EMEMFAIL : DUCQ_OK;

	if(! ducq_send_ack(ducq, state)) 
		ducq_reactor_delete(reactor, ducq);
	if(state) {
		ducq_log(WARN, "%s,%s", route, ducq_state_tostr(state));
		ducq_close(ducq);
		return state;
	}


	ducq_reactor_loop(reactor, _publish, &msg);

	ducq_log(INFO, "%s,%d notified", route, msg.count);

	free( msg.route ); // discard const
	return state;
}



struct ducq_cmd_t command = {
	.name = "publish",
	.doc  = "publish a message to a queue.",
	.exec =  publish
};
