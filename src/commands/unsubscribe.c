#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#include "../ducq.h"
#include "../ducq_reactor.h"

struct unsub_ctx {
	const char *id;
	int count;
};

static
ducq_loop_t _unsubscribe(ducq_i *ducq, char *route, void *ctx) {
	struct unsub_ctx *unsub = (struct unsub_ctx*) ctx; 
	
	if(strcmp(unsub->id, ducq_id(ducq)) == 0) {
		unsub->count++;
		return DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK;
	}

	return DUCQ_LOOP_CONTINUE;	
}


ducq_state unsubscribe(struct ducq_reactor *reactor, ducq_i *ducq, char *buffer, size_t size) {
	struct unsub_ctx unsub = {
		.id    = ducq_parse_payload(buffer),
		.count = 0
	};

	if (unsub.id == NULL
	|| (strcmp(unsub.id, "") == 0) ) {
		ducq_log(WARNING, "%s,%d unsubscribed", unsub.id, unsub.count);
		return DUCQ_EMSGINV;
	}

	ducq_reactor_loop(reactor, _unsubscribe, &unsub);

	if(unsub.count <= 0 ) {
		ducq_log(WARNING, "%s,%d unsubscribed", unsub.id, unsub.count);
		return DUCQ_ENOTFOUND;
	}

	ducq_log(INFO, "%s,%d unsubscribed", unsub.id, unsub.count);
	return DUCQ_OK;
}



struct ducq_cmd_t command = {
	.name = "unsubscribe",
	.doc  =	"close a client's connection. set payload as id to unsubscribe.",
	.exec = unsubscribe
};
