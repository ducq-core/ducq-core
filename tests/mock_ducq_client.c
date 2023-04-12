#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mock_ducq_client.h"
#include "../src/ducq_vtbl.h"


#define MAX_ID 10
static unsigned next_id = 0;
char MOCK_CLIENT_RECV_BUFFER[BUFSIZ] = {};
unsigned MOCK_CLIENT_RECV_BUFFER_LEN = 0;

typedef struct mock_ducq_client {
	ducq_vtbl *tbl;
	char id[MAX_ID];
} mock_ducq_client;


static
ducq_state _conn(ducq_i *ducq) {
	check_expected(ducq);
	return mock();
}


static
const char *_id(ducq_i *ducq) {
	mock_ducq_client *mock_ducq = (mock_ducq_client *)ducq;
	return mock_ducq->id;
}
static
ducq_state _timeout(ducq_i *ducq, int timeout) {
	check_expected(ducq);
	check_expected(timeout);
	return mock();
}




static
ducq_state _recv(ducq_i *ducq, char *ptr, size_t *count) {
	check_expected(ducq);
	check_expected(ptr);
	check_expected(count);

	if(count) {
		if(*count > MOCK_CLIENT_RECV_BUFFER_LEN)
			 *count = MOCK_CLIENT_RECV_BUFFER_LEN;
		memcpy(ptr, MOCK_CLIENT_RECV_BUFFER, *count);
	}

	return mock();
}


static
ducq_state _send(ducq_i *ducq, const void *buf, size_t *count) {
	check_expected(ducq);
	check_expected(buf);
	check_expected(count);
	return mock();
}

static
ducq_i *_copy(ducq_i * ducq) {
	return (ducq_i*) mock();
}

static
bool _eq(ducq_i *a, ducq_i *b) {
	mock_ducq_client *mock_ducq_a = (mock_ducq_client *)a;
	mock_ducq_client *mock_ducq_b = (mock_ducq_client *)b;

	return mock_ducq_a->tbl == mock_ducq_b->tbl
			&& mock_ducq_a->id  == mock_ducq_b->id;
}

static
ducq_state _close(ducq_i *ducq) {
	check_expected(ducq);
	return mock();
}

static
void _free (ducq_i *ducq) {
	if(ducq) free(ducq);
}




static ducq_vtbl table = {
	.conn    = _conn,
	.close   = _close,
	.id      = _id,
	.recv    = _recv,
	.send    = _send,
	.copy    = _copy,
	.eq      = _eq,
	.timeout = _timeout,
	.free    = _free
};

ducq_i *ducq_new_mock() {
	mock_ducq_client *cli = malloc(sizeof(mock_ducq_client));
	if(!cli) return NULL;

	cli->tbl = &table;
	snprintf(cli->id, MAX_ID, "%d", next_id++);

	return (ducq_i *) cli;
}


#undef MAX_ID