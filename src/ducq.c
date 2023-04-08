#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <setjmp.h>
#include <errno.h>



// #include "ducq_def.h"
#include "ducq_i.h"
#include "ducq.h"



#define DUCQ_PUBLISH   'P'
#define DUCQ_SUBSCRIBE 'S'
#define DUCQ_ACK       'A'
#define DUCQ_NACK      'N'


#ifndef DUCQ_MAX_MSG_LEN
#define DUCQ_MAX_MSG_LEN 512
#endif



static
const char *state_str[] = {
	#define list_msg(_, str) str,
	_foreach_state(list_msg)
	#undef list_msg

	"unknown"
};

// const char *ducq_state_tostr_state(ducq_state state) { // for server, encapsulation?
// 	return state_str[state];
// };
// const char *ducq_state_tostr_int(int state) { // for server, encapsulation?
const char *ducq_state_tostr(int state) { // for server, encapsulation?
	if(state < 0)
		 state = -state;

	static const int last = (sizeof(state_str) / sizeof(char*)) -1;
	if(state > last)
		 state = last;
	
	return state_str[state];
};


typedef struct ducq_t {
	const ducq_i *tbl;
} ducq_t;

ducq_state  ducq_conn(ducq_t *ducq) {
	return ducq->tbl->conn(ducq);
}
ducq_state  ducq_send(ducq_t *ducq, void *buf, size_t *size) {
	return ducq->tbl->send(ducq, buf, size);
}
ducq_state  ducq_recv(ducq_t *ducq, void *buf, size_t *size) {
	return ducq->tbl->recv(ducq, buf, size);
}
const char *ducq_id(ducq_t *ducq) {
	return ducq->tbl->id(ducq);
}
ducq_t *ducq_copy(ducq_t *ducq) {
	return ducq->tbl->copy(ducq);
}
bool ducq_eq(ducq_t *a, ducq_t *b) {
	return a->tbl->eq(a, b);
}
ducq_state  ducq_timeout(ducq_t *ducq, int timeout) {
	return ducq->tbl->timeout(ducq, timeout);
}
ducq_state ducq_close(ducq_t *ducq) {
	return ducq->tbl->close(ducq);
}
void ducq_free(ducq_t *ducq) {
	return ducq->tbl->free(ducq);
}



// void ducq_conn(ducq_t *ducq) {
// 	ducq_state state = ducq_ep_conn(ducq->ep);
// 	if(state)
// 		DUCQ_RAISE(ducq, state);
// }
// void ducq_send(ducq_t *ducq, char *ptr, int *n) {
// 	int rc = ducq_ep_send(ducq->ep, ptr, *n);
// 	if( rc < 0)
// 		DUCQ_RAISE(ducq, -rc);
// 	*n = rc;
// }
// void ducq_recv(ducq_t *ducq, char *ptr, int *n) {
// 	int rc = ducq_ep_recv(ducq->ep, ptr, *n);
// 	if( rc < 0)
// 		DUCQ_RAISE(ducq, -rc);
// 	*n = rc;
// }
// char *ducq_id(ducq_t *ducq, char *ptr, int *n) {
// 	return ducq_ep_id(ducq->ep, ptr, *n);
// }
// void ducq_timeout(ducq_t *ducq, int timeout) {
// 	int rc = ducq_ep_timeout(ducq->ep, timeout);
// 	if( rc == -1)
// 		DUCQ_RAISE(ducq, DUCQ_ECOMMLAYER);
// }
// void ducq_close(ducq_t *ducq) {
// 	ducq_state state = ducq_ep_close(ducq->ep);
// 	if(state)
// 		DUCQ_RAISE(ducq, state);
// }

// void ducq_free(ducq_t *ducq) {
// 	if(!ducq) return;
// 	if(ducq->ep)
// 		ducq_ep_free(ducq->ep);
// 	free(ducq);
// }









#define CHECK(func)  if( (state = func) ) return state


// ducq_state ducq_emit(ducq_t *ducq, const char *command, const char *route, const char *payload) {

// }


ducq_state ducq_publish(ducq_t *ducq, const char *route, const char *payload) {
	char buffer[DUCQ_MAX_MSG_LEN];
	ducq_state state = DUCQ_OK;
	size_t size = 0;

	// CHECK(ducq_conn(ducq));

	size = snprintf(buffer, DUCQ_MAX_MSG_LEN, "publish %s\n%s", route, payload);
	size = size < DUCQ_MAX_MSG_LEN ? size : DUCQ_MAX_MSG_LEN;
	CHECK(ducq_send(ducq, buffer, &size));

	size = DUCQ_MAX_MSG_LEN;
	CHECK(ducq_recv(ducq, buffer, &size));
fprintf(stderr, "received:\n%.*s\n", (int)size, buffer);
	if(*buffer != DUCQ_ACK) return DUCQ_NACK;

	// CHECK(ducq_close(ducq));
	return state;
}

ducq_state ducq_subscribe(ducq_t *ducq, const char *route, on_msg_f onmsg, void *cl) {
	ducq_state state = DUCQ_OK;
	char buffer[DUCQ_MAX_MSG_LEN];
	size_t size = 0;

	CHECK(ducq_conn(ducq));
	
	size = snprintf(buffer, DUCQ_MAX_MSG_LEN, "subscribe %s\n", route);
	size = size < DUCQ_MAX_MSG_LEN ? size : DUCQ_MAX_MSG_LEN;
	CHECK(ducq_send(ducq, buffer, &size));

	size = DUCQ_MAX_MSG_LEN;
	CHECK(ducq_timeout(ducq, 1));
	CHECK(ducq_recv(ducq, buffer, &size));

	if(*buffer != DUCQ_ACK) {
		// send to on_msg_f and let user deal handle err ?
fprintf(stderr, "*buffer: '%c'\n", *buffer);
fprintf(stderr, "*DUCQ_ACK: '%c'\n", DUCQ_ACK);
fprintf(stderr, "**buffer != DUCQ_ACK: '%d'\n", *buffer != DUCQ_ACK);
fprintf(stderr, "buffer:\n%s\n", buffer);
		return state;
	}


	ducq_timeout(ducq, 0);
	printf("about to loop...\n");
	for(;;) {
		size = DUCQ_MAX_MSG_LEN;

		CHECK(ducq_recv(ducq, buffer, &size));

		if( onmsg(buffer, size, cl) )
			break;
	}
	

	return state;
}



// int ducq_subscribe(ducq_t *ducq, const char *route, on_msg_f onmsg, void *cl) {
// 	ducq_conn(ducq);

// 	char buffer[DUCQ_MAX_MSG_LEN];
// 	int size = snprintf(buffer, DUCQ_MAX_MSG_LEN, "S %s\n", route);

// 	ducq_send(ducq, buffer, &size);
// 	ducq_timeout(ducq, 10);
// 	int n = DUCQ_MAX_MSG_LEN;
// 	ducq_recv(ducq, buffer, &n);
// 	if(*buffer != DUCQ_ACK) {
// fprintf(stderr, "*buffer: '%c'\n", *buffer);
// fprintf(stderr, "*DUCQ_ACK: '%c'\n", DUCQ_ACK);
// fprintf(stderr, "**buffer != DUCQ_ACK: '%d'\n", *buffer != DUCQ_ACK);
// fprintf(stderr, "buffer:\n%s\n", buffer);
// 		DUCQ_RAISE(ducq, DUCQ_EACK);
// 	}


// 	ducq_timeout(ducq, 0);
// 	for(;;) {
// 		size = DUCQ_MAX_MSG_LEN;
// 		ducq_recv(ducq, buffer, &size);
// 		int rc = onmsg(buffer, size, cl);
// 		if( rc ) return rc;
// 	}
// }

