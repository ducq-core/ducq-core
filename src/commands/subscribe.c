#include <string.h>
#include <stdlib.h>

#include "../ducq.h"
#include "../ducq_reactor.h"



#include <stdio.h>
ducq_state subscribe(struct ducq_reactor *reactor, ducq_i *ducq, char *buffer, size_t size) {
	char *end = NULL;
	const char *route = ducq_parse_route(buffer, (const char**) &end);
	if(route == NULL) {
		ducq_send_ack(ducq, DUCQ_EMSGINV);
		ducq_log(WARN, "%s", ducq_state_tostr(DUCQ_EMSGINV) );
		return DUCQ_EMSGINV;
	}

	*end = '\0';
	ducq_state add_state = ducq_reactor_subscribe(reactor, ducq, route);
	if(add_state) ducq_log(ERROR, "%s", ducq_state_tostr(add_state));
	*end = '\n';

	ducq_state ack_state = ducq_send_ack(ducq, add_state);
	if(ack_state) {
		ducq_log(WARN, "%s", ducq_state_tostr(ack_state));
		ducq_reactor_delete(reactor, ducq);
	}
	else ducq_log(INFO,  "%s", route);

	return add_state ? add_state : ack_state;
}


struct ducq_cmd_t command = {
	.name = "subscribe",
	.doc  = "subscribe to a message queue.",
	.exec =  subscribe
};
