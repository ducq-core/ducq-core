#include <stdio.h>
#include <string.h>

#include "../ducq_reactor.h"


ducq_state PING(struct ducq_reactor *reactor, ducq_i *ducq, char *message, size_t size) {
	const char *payload = ducq_parse_payload(message);
	ducq_log(DEBUG, payload);
	message[1] = 'O';
	return ducq_send(ducq, message, &size);
}



struct ducq_cmd_t command = {
	.name = "PING",
	.doc  = "Reply with a PONG command with unchanged payload.",
	.exec =  PING
};
