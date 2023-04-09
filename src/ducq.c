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