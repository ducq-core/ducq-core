#include <stdio.h>
#include <time.h>

#include "../ducq_reactor.h"


ducq_state epoch(struct ducq_reactor *reactor, ducq_i *ducq, char *message, size_t _size) {
	char now[25] = "";
	size_t size = sizeof(now);
	size = snprintf(now, size, "%ld", time(NULL));
	ducq_log(INFO, "%s", now);
	return ducq_send(ducq, now, &size);
}



struct ducq_cmd_t command = {
	.name = "epoch",
	.doc  = "Reply with current time in epoch format",
	.exec =  epoch
};
