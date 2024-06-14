#include <stdio.h>
#include <setjmp.h>	
#include <cmocka.h>

#include <string.h>

#include "../mock_ducq_client.h"
#include "../../src/ducq.h"


extern char MOCK_CLIENT_RECV_BUFFER[];
extern unsigned MOCK_CLIENT_RECV_BUFFER_LEN;


struct ducq_listen_ctx ctx = {};

static
int on_msg(ducq_i * ducq, char *payload, size_t size, void *ctx) {
	return mock();
}
static
int on_proto(ducq_i * ducq, char *payload, size_t size, void *ctx) {
	return mock();
}
static
int on_nack(ducq_i * ducq, char *payload, size_t size, void *ctx) {
	return mock();
}
static
int on_error(ducq_i * ducq, ducq_state state, void *ctx) {
	check_expected(state);
	return mock();
}



int listen_setup(void **state) {
	ctx.on_message  = on_msg;
	ctx.on_protocol = on_proto;
	ctx.on_nack     = on_nack;
	ctx.on_error    = on_error;
	ctx.recv_raw    = true;
	ctx.ctx         = NULL;

	strcpy(MOCK_CLIENT_RECV_BUFFER, "");
	MOCK_CLIENT_RECV_BUFFER_LEN = 0;

	ducq_i *ducq = ducq_new_mock(NULL);
	if (ducq == NULL) {
		return -1;
	}

	*state = ducq;
	return 0;
}

int listen_teardown(void **state) {
	ducq_i *ducq = (ducq_i*) *state;
    ducq_free(ducq);

	return 0;
}

static
void ignore_recv() {
	expect_any(_recv, ducq);
	expect_any(_recv, ptr);
	expect_any(_recv, count);
	will_return(_recv, DUCQ_OK);
}





//			P A R A M E T E R S

static
int on_msg_check_ctx(ducq_i * ducq, char *payload, size_t size, void *ctx) {
	check_expected_ptr(ctx);
	return mock();
}

void listen_pass_ctx_to_callback(void **state) {
	// arrange
	ducq_i *ducq = (ducq_i*) *state;

	char msg[] = "PUB _\n0";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	ctx.on_message = on_msg_check_ctx;

	// mock
	ignore_recv();
	expect_value(on_msg_check_ctx, ctx, ctx.ctx);
	will_return(on_msg_check_ctx, 1);

	// act
	ducq_listen(ducq, &ctx);
}


void listen_return_state(void **state) {
	// arrange
	ducq_i *ducq = (ducq_i*) *state;

	char msg[] = "PUB _\n0";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	ducq_state expected_return = DUCQ_PROTOCOL;

	// mock
	expect_any(_recv, ducq);
	expect_any(_recv, ptr);
	expect_any(_recv, count);
	will_return(_recv, expected_return);

	will_return(on_proto, 1);

	// act
	ducq_state actual_return = ducq_listen(ducq, &ctx);

	// assert
	assert_int_equal(expected_return, actual_return);
}




//			M A N A G E D

void listen_managed_call_on_protocol_PING(void **state) {
	// arrange
	ducq_i *ducq = (ducq_i*) *state;

	char msg[] = "PING _\n0";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	ctx.recv_raw = false;

	// mock
	ignore_recv();
	expect_any(_send, ducq);
	expect_any(_send, buf);
	expect_any(_send, *count);
	will_return(_send, DUCQ_OK);

	will_return(on_proto, 1);

	// act
	ducq_listen(ducq, &ctx);
}

void listen_managed_call_on_protocol_PARTS(void **state) {
	// arrange
	ducq_i *ducq = (ducq_i*) *state;

	char msg[] = "PARTS";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	ctx.recv_raw = false;

	// mock
	ignore_recv();
	will_return(on_proto, 1);

	// act
	ducq_listen(ducq, &ctx);
}

void listen_managed_call_on_protocol_END(void **state) {
	// arrange
	ducq_i *ducq = (ducq_i*) *state;

	char msg[] = "END";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	ctx.recv_raw = false;

	// mock
	ignore_recv();
	will_return(on_proto, 1);

	// act
	ducq_listen(ducq, &ctx);
}

void listen_managed_call_on_nack(void **state) {
	// arrange
	ducq_i *ducq = (ducq_i*) *state;

	char msg[] = "NACK _\n0";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	ctx.recv_raw = false;

	// mock
	ignore_recv();
	will_return(on_nack, 1);

	// act
	ducq_listen(ducq, &ctx);
}

void listen_managed_call_on_error(void **state) {
	// arrange
	ducq_i *ducq = (ducq_i*) *state;

	char msg[] = "PING _\n0";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	ctx.recv_raw = false;

	// mock
	expect_any(_recv, ducq);
	expect_any(_recv, ptr);
	expect_any(_recv, count);
	will_return(_recv, DUCQ_EREAD);

	expect_value(on_error, state, DUCQ_EREAD);
	will_return(on_error, 1);

	// act
	ducq_listen(ducq, &ctx);
}

void listen_managed_error_on_sending_pong_call_on_error(void **state) {
	// arrange
	ducq_i *ducq = (ducq_i*) *state;

	char msg[] = "PING _\n0";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	ctx.recv_raw = false;

	// mock
	ignore_recv();
	expect_any(_send, ducq);
	expect_any(_send, buf);
	expect_any(_send, *count);
	will_return(_send, DUCQ_EWRITE);

	expect_value(on_error, state, DUCQ_EWRITE);
	will_return(on_error, 1);

	// act
	ducq_listen(ducq, &ctx);
}




//			R A W

void listen_raw_proto_call_on_message(void **state) {
	// arrange
	ducq_i *ducq = (ducq_i*) *state;

	char msg[] = "PING _\n0";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	ctx.recv_raw = true;

	// mock
	ignore_recv();
	will_return(on_msg, 1);

	// act
	ducq_listen(ducq, &ctx);
}

void listen_raw_nack_call_on_message(void **state) {
	// arrange
	ducq_i *ducq = (ducq_i*) *state;

	char msg[] = "NACK _\n0";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	ctx.recv_raw = true;

	// mock
	ignore_recv();
	will_return(on_msg, 1);

	// act
	ducq_listen(ducq, &ctx);
}

void listen_raw_error_call_on_error(void **state) {
	// arrange
	ducq_i *ducq = (ducq_i*) *state;

	char msg[] = "PING _\n0";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	ctx.recv_raw = true;

	// mock
	expect_any(_recv, ducq);
	expect_any(_recv, ptr);
	expect_any(_recv, count);
	will_return(_recv, DUCQ_EREAD);

	expect_value(on_error, state, DUCQ_EREAD);
	will_return(on_error, 1);

	// act
	ducq_listen(ducq, &ctx);
}





//			L O O P


static
int on_msg_loop(ducq_i * ducq, char *payload, size_t size, void *ctx) {
	function_called();

	char *msg = "PING _\n0";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	return 0;
}
static
int on_proto_loop(ducq_i * ducq, char *payload, size_t size, void *ctx) {
	function_called();

	char *msg = "NACK _\n0";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	return 0;
}
static
int on_nack_loop(ducq_i * ducq, char *payload, size_t size, void *ctx) {
	function_called();

	char *msg = "";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	return 0;
}
static
int on_error_loop(ducq_i * ducq, ducq_state state, void *ctx) {
	function_called();
	return 1;
}

void listen_loops_until_return_is_not_zero(void **state) {
	// arrange
	ducq_i *ducq = (ducq_i*) *state;

	char msg[] = "PUB _\n0";
	strcpy(MOCK_CLIENT_RECV_BUFFER, msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(msg);

	ctx.on_message  = on_msg_loop;
	ctx.on_protocol = on_proto_loop;
	ctx.on_nack     = on_nack_loop;
	ctx.on_error    = on_error_loop;
	ctx.recv_raw    = false;

	// mock
	expect_any_always(_recv, ducq);
	expect_any_always(_recv, ptr);
	expect_any_always(_recv, count);
	will_return_count(_recv, DUCQ_OK,    3);
	will_return_count(_recv, DUCQ_EREAD, 1);

	expect_any_always (_send, ducq);
	expect_any_always (_send, buf);
	expect_any_always (_send, *count);
	will_return_always(_send, DUCQ_OK);

	expect_function_call(on_msg_loop);
	expect_function_call(on_proto_loop);
	expect_function_call(on_nack_loop);
	expect_function_call(on_error_loop);

	// act
	ducq_listen(ducq, &ctx);
}

