#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <setjmp.h>
#include <errno.h>

#include "ducq.h"
#include "ducq_vtbl.h"



static
const char *state_str[][2] = {
	{"ok", "ok"},

	#define list_msg(code, str) {#code, str},
	_foreach_state(list_msg)
	#undef list_msg
};

static
ducq_state _sanitize_state(int state) {
	if(state < 0)
		 state = -state;

	static const int last = (sizeof(state_str) / sizeof(char*) / 2) - 1;
	if(state > last)
		 state = last;

	return state;
};

const char *ducq_state_tostr(int state) {
	state = _sanitize_state(state);
	return state_str[state][1];
};

const char *ducq_state_short(int state) {
	state = _sanitize_state(state);
	const char *ptr = state_str[state][0];
	if(state != DUCQ_OK)
		ptr += 5;
	return ptr;
};




typedef struct ducq_i {
	const ducq_vtbl *tbl;
} ducq_i;

ducq_state  ducq_conn(ducq_i *ducq) {
	return ducq
		? ducq->tbl->conn(ducq)
		: DUCQ_ENULL;
}
ducq_state  ducq_send(ducq_i *ducq, const void *buf, size_t *size) {
	return ducq
		? ducq->tbl->send(ducq, buf, size)
		: DUCQ_ENULL;
}
ducq_state ducq_parts(ducq_i *ducq) {
	return ducq
		? ducq->tbl->parts(ducq)
		: DUCQ_ENULL;
}
ducq_state ducq_end(ducq_i *ducq) {
	return ducq
		? ducq->tbl->end(ducq)
		: DUCQ_ENULL;
}
ducq_state  ducq_recv(ducq_i *ducq, char *buf, size_t *size) {
	return ducq
		? ducq->tbl->recv(ducq, buf, size)
		: DUCQ_ENULL;
}
const char *ducq_id(ducq_i *ducq) {
	return ducq
		? ducq->tbl->id(ducq)
		: NULL;
}
ducq_i *ducq_copy(ducq_i *ducq) {
	return ducq
		? ducq->tbl->copy(ducq)
		: NULL;
}
bool ducq_eq(ducq_i *a, ducq_i *b) {
	return  a 
		? a->tbl->eq(a, b)
		: false;
}
ducq_state  ducq_timeout(ducq_i *ducq, int timeout) {
	return ducq
		? ducq->tbl->timeout(ducq, timeout)
		: DUCQ_ENULL;
}
ducq_state ducq_close(ducq_i *ducq) {
	return ducq
		? ducq->tbl->close(ducq)
		: DUCQ_ENULL;
}
void ducq_free(ducq_i *ducq) {
	if(ducq)
		ducq->tbl->free(ducq);
}






ducq_state ducq_emit(ducq_i *ducq, const char *command, const char *route, const char *payload, size_t size) {
	char msg[DUCQ_MSGSZ];
	size = snprintf(msg, DUCQ_MSGSZ, "%s %s\n%.*s", command, route, (int)size, payload);

	if(size >= DUCQ_MSGSZ)
		return DUCQ_EMSGSIZE;

	return ducq_send(ducq, msg, &size);
}

ducq_state ducq_send_ack(ducq_i *ducq, ducq_state state) {
	char msg[128];

	size_t size = 0;
	if (state == DUCQ_OK) {
		size = 	snprintf(msg, 128, "ACK");
	} else {
		size = snprintf(msg, 128, "NACK *\n%d\n%s",
			state, ducq_state_tostr(state) );
	}

	return ducq_send(ducq, msg, &size);
}

ducq_state  ducq_receive(ducq_i *ducq, char *msg, size_t *size) {
	size_t _size = *size;
	ducq_state state = ducq_recv(ducq, msg, size);

	if(state == DUCQ_ETIMEOUT) {
		if(_size >= 4 && strncmp(msg, "PING", 4) == 0)
			return DUCQ_ETIMEOUT;
		*size = snprintf(msg, _size, "PING _\nping");
		DUCQ_CHECK( ducq_send(ducq, msg, size) );
		return DUCQ_PROTOCOL;
	}
	if(state)
		return state;

		
	// protocol
	if(strncmp(msg, "ACK", 3) == 0){
		return DUCQ_PROTOCOL;
	}
	if(strncmp(msg, "PARTS", 5) == 0){
		return DUCQ_PROTOCOL;
	}
	if(strncmp(msg, "END", 3) == 0){
		return DUCQ_PROTOCOL;
	}
	if(strncmp(msg, "NACK", 4) == 0) {
		return DUCQ_NACK;
	}
	if(strncmp(msg, "PING", 4) == 0) {
		msg[1] = 'O';
		DUCQ_CHECK( ducq_send(ducq, msg, size) );
		return DUCQ_PROTOCOL;
	}
	if(strncmp(msg, "PONG", 4) == 0) {
		return DUCQ_PROTOCOL;
	}

	// message
	return DUCQ_OK;
}

ducq_state ducq_listen(ducq_i *ducq, struct ducq_listen_ctx *ctx) {
	char msg[DUCQ_MSGSZ] = "";
	size_t size          = DUCQ_MSGSZ;
	ducq_state state     = DUCQ_OK;

	ducq_state (*recv)(ducq_i*, char*, size_t*) = ctx->recv_raw
		? ducq_recv
		: ducq_receive;

	for(;;) {
		size  = DUCQ_MSGSZ;
		state = recv(ducq, msg, &size);

		if(state >= DUCQ_ERROR) {
			if(!ctx->on_error) continue;
			if( ctx->on_error(ducq, state, ctx->ctx) )
				break;
			continue;
		}

		ducq_on_msg_f callback = 
			state == DUCQ_OK       ? ctx->on_message  :
			state == DUCQ_PROTOCOL ? ctx->on_protocol :
			state == DUCQ_NACK     ? ctx->on_nack     : NULL;
		if(!callback) continue;
		if( callback(ducq, msg, size, ctx->ctx) )
			break;
	}

	return state;
}


ducq_state ducq_pub(ducq_i *ducq, const char *route, const char *payload, size_t size) {
	DUCQ_CHECK( ducq_emit(ducq, "pub", route, payload, size) );

	char msg[DUCQ_MSGSZ] = "";
	size = DUCQ_MSGSZ;
	ducq_state state = ducq_receive(ducq, msg, &size);
	return state <= DUCQ_PROTOCOL ? DUCQ_OK : state;
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
	if(!sub || !pub) return false;

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
