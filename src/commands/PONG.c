#include <stdio.h>
#include <string.h>

#include "../ducq_reactor.h"


ducq_state PONG(struct ducq_reactor *reactor, ducq_i *ducq, char *message, size_t size) {
	const char *payload = ducq_parse_payload(message);
	ducq_log(DEBUG, payload);
	message[1] = 'O';
	return DUCQ_OK;
}



struct ducq_cmd_t command = {
	.name = "PONG",
	.doc  = "Trap PONG command.",
	.exec =  PONG
};
