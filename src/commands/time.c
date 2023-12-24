#include <stdio.h>
#include <string.h>

#include "../ducq_reactor.h"
#include "../ducq_log.h"


ducq_state gettime(struct ducq_reactor *reactor, ducq_i *ducq, char *message, size_t _size) {
	char now[DUCQ_TIMESTAMP_SIZE] = "";
	size_t size = sizeof(now);
	ducq_getnow(now, size);
	ducq_log(INFO, "%s", now);
	return ducq_send(ducq, now, &size);
}



struct ducq_cmd_t command = {
	.name = "time",
	.doc  = "Reply with current time in YYYY-MM-DDTHH:MM:SS format",
	.exec =  gettime
};
