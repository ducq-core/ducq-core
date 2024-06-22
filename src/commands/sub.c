#include <ducq.h>
#include <string.h>
#include <stdlib.h>

#include "../ducq.h"
#include "../ducq_reactor.h"



#include <stdio.h>
ducq_state sub(struct ducq_reactor *reactor, ducq_i *ducq, char *buffer, size_t size) {
	const char *payload = ducq_parse_payload(buffer);
	char *end = NULL;
	const char *route = ducq_parse_route(buffer, (const char**) &end);
	if(route == NULL) {
		ducq_send_ack(ducq, DUCQ_EMSGINV);
		ducq_log(WARNING, "%s", ducq_state_tostr(DUCQ_EMSGINV) );
		return DUCQ_EMSGINV;
	}
	*end = '\0';

	ducq_state add_state = ducq_reactor_subscribe(reactor, ducq, route);
	if(add_state) ducq_log(ERROR, "%s", ducq_state_tostr(add_state));


	ducq_state ack_state = DUCQ_OK;
	if(add_state != DUCQ_OK) {
		ack_state = ducq_send_ack(ducq, add_state);
	}
	else if(payload && (strcmp(payload, "last") == 0) ) {
		const char *last = ducq_get_last_msg(reactor, route);
		size_t len = strlen(last);
		ack_state = ducq_send(ducq, last, &len);
	}
	else {
		ack_state = ducq_send_ack(ducq, DUCQ_OK);
	}

	if(ack_state) {
		ducq_log(WARNING, "%s", ducq_state_tostr(ack_state));
		ducq_reactor_delete(reactor, ducq);
	} else {
		ducq_log(INFO, "%s", route);
	}

	*end = '\n';
	return add_state ? add_state : ack_state;
}


struct ducq_cmd_t command = {
	.name = "sub",
	.doc  =
		"subscribe to a message queue. "
		"send 'last' as payload to received last published message "
		"on this route (if any).",
	.exec =  sub
};
