#include <stdio.h>
#include <string.h>

#include "../ducq_srv_int.h"





ducq_state list_commands(struct ducq_srv *srv, ducq_i *ducq, char *message, size_t size) {
	(void) message;
	(void) size;

	char payload[DUCQ_MSGSZ] = "";
	ducq_state state = DUCQ_OK;

	for(int i = 0; i < srv->ncmd; i++) {
		struct ducq_cmd_t *cmd = srv->cmds[i];
		size_t len = snprintf(payload, DUCQ_MSGSZ, "%s,%s\n", cmd->name, cmd->doc);
		
		state = ducq_send(ducq, payload, &len);
		if(state) break;
	}

	ducq_close(ducq);
	return state;
}





struct ducq_cmd_t command = {
	.name = "list_commands",
	.doc  = "list all available server commands as csv: <name>,<doc>\\n.",
	.exec =  list_commands
};