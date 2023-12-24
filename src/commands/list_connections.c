#include <stdio.h>
#include <string.h>

#include "../ducq_reactor.h"
#include "../ducq_log.h"


static
ducq_loop_t _list(ducq_i *client, char *route, void *ctx) {
	ducq_i *ducq = (ducq_i *)ctx;

	char  buffer[DUCQ_MSGSZ];
	size_t size = snprintf(buffer, sizeof(buffer), "%s%s%s\n", 
		ducq_id(client),
		route ? ","   : "",
		route ? route : ""
	);
	
	return ducq_send(ducq, buffer, &size)
		? DUCQ_LOOP_BREAK
		: DUCQ_LOOP_CONTINUE;
}

static
ducq_state _list_connections(ducq_reactor *reactor, ducq_i *ducq) {
	DUCQ_CHECK( ducq_parts(ducq) );
	if( ducq_reactor_loop(reactor, _list, ducq) )
		return DUCQ_EWRITE;
	DUCQ_CHECK( ducq_end(ducq) );

	return DUCQ_OK;
}

ducq_state list_connections(struct ducq_reactor *reactor, ducq_i *ducq, char *message, size_t size) {
	(void) message;
	(void) size;

	ducq_state state = _list_connections(reactor, ducq);
	
	enum ducq_log_level level = state ? DUCQ_LOG_WARNING : DUCQ_LOG_INFO;
	ducq_reactor_log(reactor, level, __func__, ducq_id(ducq),
		"%s", ducq_state_tostr(state));

	return state;
}



struct ducq_cmd_t command = {
	.name = "list_connections",
	.doc  = "list all current connections as csv: <id>,<route>\\n.",
	.exec =  list_connections
};
