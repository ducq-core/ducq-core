#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <setjmp.h>
#include <errno.h>

#include "ducq.h"
#include "ducq_vtbl.h"



static
const char *state_str[] = {
	"ok",
	#define list_msg(_, str) str,
	_foreach_state(list_msg)
	#undef list_msg

	"unknown"
};

const char *ducq_state_tostr(int state) {
	if(state < 0)
		 state = -state;

	static const int last = (sizeof(state_str) / sizeof(char*)) - 1;
	if(state > last)
		 state = last;
	
	return state_str[state];
};



typedef struct ducq_i {
	const ducq_vtbl *tbl;
} ducq_i;

ducq_state  ducq_emit(ducq_i *ducq, const char *command, const char *route, const char *payload, size_t payload_size, bool close) {
	return ducq->tbl->emit(ducq, command, route, payload, payload_size, close);
}
ducq_state  ducq_conn(ducq_i *ducq) {
	return ducq->tbl->conn(ducq);
}
ducq_state  ducq_send(ducq_i *ducq, const void *buf, size_t *size) {
	return ducq->tbl->send(ducq, buf, size);
}
ducq_state  ducq_recv(ducq_i *ducq, void *buf, size_t *size) {
	return ducq->tbl->recv(ducq, buf, size);
}
const char *ducq_id(ducq_i *ducq) {
	return ducq->tbl->id(ducq);
}
ducq_i *ducq_copy(ducq_i *ducq) {
	return ducq->tbl->copy(ducq);
}
bool ducq_eq(ducq_i *a, ducq_i *b) {
	return a->tbl->eq(a, b);
}
ducq_state  ducq_timeout(ducq_i *ducq, int timeout) {
	return ducq->tbl->timeout(ducq, timeout);
}
ducq_state ducq_close(ducq_i *ducq) {
	return ducq->tbl->close(ducq);
}
void ducq_free(ducq_i *ducq) {
	return ducq->tbl->free(ducq);
}


ducq_state ducq_subscribe(ducq_i *ducq, const char *route, ducq_on_msg_f on_msg, void *ctx) {
	DUCQ_CHECK( ducq_conn(ducq) );
	DUCQ_CHECK( ducq_emit(ducq, "subscribe", route, "", 0, true) );
	DUCQ_CHECK( ducq_timeout(ducq, 0) );

	
	char buffer[DUCQ_MSGSZ] = "";
	size_t size = sizeof(buffer);
	DUCQ_CHECK( ducq_recv(ducq, buffer, &size) );
	DUCQ_CHECK( ducq_ack_to_state(buffer) );

	for(;;) {
		DUCQ_CHECK( ducq_recv(ducq, buffer, &size) );
		if(on_msg(buffer, size, ctx))
			break;
	}

	if(ducq_close(ducq))
		return DUCQ_ECLOSE;

	return DUCQ_OK;
}

ducq_state ducq_publish(ducq_i *ducq, char *route, char *payload, size_t size) {
	DUCQ_CHECK( ducq_conn(ducq) );
	DUCQ_CHECK( ducq_emit(ducq, "publish", route, payload, size, true) );


	char recvbuf[DUCQ_MSGSZ] = "";
	size_t len = DUCQ_MSGSZ;
	ducq_state state = ducq_recv(ducq, recvbuf, &len);
	if(state == DUCQ_OK)
		state = ducq_ack_to_state(recvbuf);
	

	if(ducq_close(ducq))
		return DUCQ_ECLOSE;

	return state;
}


const char * ducq_parse_command(const char *buffer, const char **end) {
	const char *command = NULL;
	const char *ptr = strchr(buffer, ' ');

	if(ptr && *ptr)
		command = buffer;
	if(end)
		*end = ptr;

	return command;
}

const char * ducq_parse_route(const char *buffer, const char **end) {
	const char *route = NULL;
	
	if(*buffer != ' ')
		buffer = strchr(buffer, ' ');
	if(!buffer)
		return NULL;
		
	buffer++;
	const char *ptr = strchr(buffer, '\n');

	if(ptr && *ptr)
		route = buffer;
	if(end)
		*end = ptr;

	return route;
}

const char * ducq_parse_payload(const char *buffer) {
	char *ptr = strchr(buffer, '\n');
	return ptr ? ++ptr : NULL;
}

ducq_state ducq_ack_to_state(const char *msg) {
	if(strncmp(msg, "ACK", 3) == 0)
		return DUCQ_OK;
	
	const char *payload = ducq_parse_payload(msg);
	if(!payload)
		return DUCQ_EMSGINV;

	long ack = strtol(payload, NULL, 10);
	if(ack == 0)
		return DUCQ_EMSGINV;

	return ack;
}

static inline  // GNU strchrnul() is non-standard
const char *strchrnull(const char *str, char c) {
	while( *str && *str != c)
			str++;
	return str;
}

bool ducq_route_cmp(const char *sub, const char *pub) {
	while(*sub && *pub) {

		if(*sub == '*') {
			sub++;
			pub = strchrnull(pub, *sub);
		}

		else if(*sub == *pub)
			sub++, pub++;

		else 
			break;
	}

	if(*sub == '*')
		sub++;
		
	return *sub == '\0' 
			&& *pub == '\0';
}


struct ducq_msg ducq_parse_msg(char *message) {
	return (struct ducq_msg) {
		.command = strtok(message, " "),
		.route   = strtok(NULL,   "\n"),
		.payload = strtok(NULL,   "")
	};
}
