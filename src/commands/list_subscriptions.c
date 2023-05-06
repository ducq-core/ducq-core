#include <stdio.h>
#include <string.h>

#include "../ducq_srv_int.h"





ducq_state list_subscriptions(struct ducq_srv *srv, ducq_i *ducq, char *message, size_t size) {
	(void) message;
	(void) size;

	char payload[DUCQ_MSGSZ] = "";
	char buffer[DUCQ_MSGSZ]  = "";
	size_t left = DUCQ_MSGSZ;
	char *ptr = payload;
	
	size_t res_size = 0;

	for(ducq_sub *sub = srv->subs; sub; sub = sub->next) {
		size_t len = snprintf(buffer, DUCQ_MSGSZ, "%s,%s\n", sub->id, sub->route);

		if (len > left) {
			res_size = ptr-payload;
			if(ducq_send(ducq, payload, &res_size))
				return ducq_close(ducq);

			left = DUCQ_MSGSZ;
			ptr  = payload;
		}

		memcpy(ptr, buffer, len);
		ptr  += len;
		left -= len;
	}

	res_size = ptr-payload;
	ducq_send(ducq, payload, &res_size); // ignore if send fails, not much to do.
	return ducq_close(ducq);;
}



struct ducq_cmd_t command = {
	.name = "list_subscriptions",
	.doc  = "list all current subscriptions.",
	.exec =  list_subscriptions
};