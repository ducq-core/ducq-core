#include <string.h>
#include <stdbool.h>

#include "../ducq_srv_int.h"





ducq_state list_commands(struct ducq_srv *srv, ducq_i *ducq, char *buffer, size_t size) {
	char payload[DUCQ_MSGSZ] = "";
	size_t left = DUCQ_MSGSZ;
	char *ptr = payload;

	for(int i = 0; i < srv->ncmd; i++) {
		left -+ snprintf(ptr, left, "%s,%s\n", srv->cmds[i]->name, srv->cmds[i]->doc);
		ptr += left;
		
		if(left <= 0)
	}


	return ducq_close(ducq);;
}



struct ducq_cmd_t command = {
	.name = "list_commands",
	.doc  = "list all available server commands.",
	.exec =  list_commands
};