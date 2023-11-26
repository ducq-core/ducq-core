#include <stdio.h>
#include <setjmp.h>	
#include <cmocka.h>

#include <string.h>

#include "../mock_ducq_client.h"
#include "../../src/ducq.h"


extern char MOCK_CLIENT_RECV_BUFFER[];
extern unsigned MOCK_CLIENT_RECV_BUFFER_LEN;

struct test_closure {
	int nmsg;
	int nproto;
	int nerror;

	char *msg;
	char *proto;
	char *error;

	int ret_msg;
	int ret_proto;
	int ret_error;
};
static
int on_msg(ducq_i * ducq, char *payload, size_t size, void *ctx) {
	(void)ducq;
	(void)payload;
	(void)size;

	struct test_closure *cl = (struct test_closure*) ctx;

	strcpy(MOCK_CLIENT_RECV_BUFFER, cl->msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(cl->msg);
	cl->nmsg++;
	return cl->ret_msg;
}
static
int on_proto(ducq_i * ducq, char *payload, size_t size, void *ctx) {
	(void)ducq;
	(void)payload;
	(void)size;

	struct test_closure *cl = (struct test_closure*) ctx;

	strcpy(MOCK_CLIENT_RECV_BUFFER, cl->proto);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(cl->proto);
	cl->nproto++;
	return cl->ret_proto;
}
static
int on_error(ducq_i * ducq, char *payload, size_t size, void *ctx) {
	(void)ducq;
	(void)payload;
	(void)size;

	struct test_closure *cl = (struct test_closure*) ctx;

	strcpy(MOCK_CLIENT_RECV_BUFFER, cl->error);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(cl->error);
	cl->nerror++;
	return cl->ret_error;
}

void listen_ok(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	char payload[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;
	ducq_state expected_state = DUCQ_OK;

	struct test_closure closure = {
		.msg       = "NACK 12\n",
		.proto     = "publish route\npayload",
		.error     = "",
		.ret_error = -1

	};

	struct ducq_listen_ctx test_ctx = {
		.on_message  = on_msg,
		.on_protocol = on_proto,
		.on_error    = on_error,
		.ctx         = &closure
	};


	// mock
	char proto[] = "ACK _\n0";
	strcpy(MOCK_CLIENT_RECV_BUFFER, proto);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(proto);
	expect_value_count(_recv, ducq, ducq, 3);
	expect_any_count  (_recv, ptr, 3);
	expect_any_count  (_recv, count, 3);
	will_return_count (_recv, DUCQ_OK, 3);


	// act
	ducq_state actual_state = ducq_listen(ducq, &test_ctx);

	// audit
	assert_int_equal(1, closure.nmsg);
	assert_int_equal(1, closure.nproto);
	assert_int_equal(1, closure.nerror);

	// teardown
	ducq_free(ducq);
}

