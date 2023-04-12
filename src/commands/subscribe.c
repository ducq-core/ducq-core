#include <stdio.h> // for tests only!!!

#include <string.h>
#include <stdlib.h>
#include "subscribe.h"

static
const char *_parse_route(char *buffer, const char **end) {
	       strtok(buffer, " ");
	return strtok(NULL,  "\n");
}


// void list_subs(ducq_sub *sub) {
// 	printf("listing all subs:\n");
// 	while(sub) {
// 		printf("\t'%s' listening on '%s'\n", sub->id, sub->route);
// 		sub = sub->next;
// 	}
// }

ducq_state subscribe(struct ducq_srv *srv, ducq_i *ducq, char *buffer, size_t size) {
	// printf("new subscriber :)\n");

	const char *end = NULL;
	const char *route = _parse_route(buffer, &end);
	if(route == NULL)
		return DUCQ_EMSGINV;

	ducq_sub *sub = malloc(sizeof(ducq_sub));
	if(!sub) return DUCQ_EMEMFAIL;

	sub->ducq  = ducq_copy(ducq);
	sub->route = strndup(route, end-route);
	sub->id    = ducq_id(sub->ducq);
	
	if(!sub->ducq || !sub->route || !sub->id) {
		if(sub->ducq)  ducq_free(sub->ducq);
		if(sub->route) free(sub->route);
		free(sub);
		return DUCQ_EMEMFAIL;
	}

	sub->next  = srv->subs;
	srv->subs = sub;


// list_subs(srv->subs);


	return DUCQ_OK;
}



struct ducq_cmd_t command = {
	.name = "subscribe",
	.doc  = "subscribe to a message queue.",
	.exec =  subscribe
};