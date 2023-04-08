#include <stdio.h> // FOR TESTS ONLY
#include <string.h>
#include <stdbool.h>

#include "../ducq_srv_int.h"

static
bool route_cmp(const char *a, const char *b, size_t size) {
	printf("cmp '%s' == '%.*s'\n", a, (int)size, b);
	return strlen(a) == size
			&& strncmp(a, b, size) == 0;
}

static
const char * parse_route(const char *buffer, const char **end) {
	const char *start = strchr(buffer, ' ');
	*end = strchr(start, '\n');

	if(start && *end)
		return start + 1;
	return NULL;
}


ducq_state publish(struct ducq_srv *srv, ducq_t *ducq, char *buffer, size_t size) {
printf("new message :)\n");

	const char *end;
	const char *route = parse_route(buffer, &end);
	if(route == NULL)
		return DUCQ_EMSGINV;

	int count = 0;
	for(ducq_sub *sub = srv->subs; sub; sub = sub->next) {
		if( ! route_cmp(sub->route, route, end-route) )
			continue;

		size_t len = size;
		ducq_state state = ducq_send(sub->ducq, buffer, &len);

		if(state != DUCQ_OK) {
			fprintf(stderr, "ducq_send() failed: %s\n", ducq_state_tostr_state(state));
			// unsubscribe, should be done on next reactor's read==0
		}
		else
			count++;
	}

printf("%d subscriber%s notified\n", count, count > 1 ? "s" : "");

	return DUCQ_OK;
}



struct ducq_cmd_t command = {
	.name = "publish",
	.doc  = "publish a message to a queue.",
	.exec =  publish
};