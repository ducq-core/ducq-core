#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "mock_ducq_client.h"
#include "unit_tests_cmd.h"

#include "../src/ducq.h"
#include "../src/ducq_reactor.h"


int subscribe_tests_setup(void **state) {
	*state = fix_new("sub");
	return *state == NULL;
}
int subscribe_tests_teardown(void **state) {
	struct fixture *fix = *state;
return fix_free(fix);
}


struct sub_check_ctx {
	ducq_i *ducq;
	char *route;
	int count;
	int match;
};
ducq_loop_t _check_sub_added(ducq_i *ducq, char *actual_route, void *ctx) {
	struct sub_check_ctx *expected = (struct sub_check_ctx*) ctx;

	assert_non_null(ducq);
	assert_non_null(ducq_id(ducq));

	if( expected->ducq && ducq_eq(ducq, expected->ducq) ) {
		assert_ptr_equal(ducq, expected->ducq);
		assert_string_equal(actual_route, expected->route);
		assert_string_equal( ducq_id(ducq), ducq_id(expected->ducq) );
		expected->match++;
	}

	expected->count++;
	return DUCQ_LOOP_CONTINUE;
}


void subscribe_msg_invalide_if_cant_parse_route(void **state) {
	//arrange
	ducq_command_f subscribe = get_command(state);
	
	ducq_state expected_state = DUCQ_EMSGINV;

	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, NULL, mock_log);

	ducq_i *subscriber = ducq_new_mock(NULL);
	char buffer[] = "subroute\npayload";
	size_t size = sizeof(buffer);

	char expected_msg[64] = "";
	size_t count = snprintf(expected_msg, 64, "NACK *\n%d\n%s", expected_state, ducq_state_tostr(expected_state));
	expect_value(_send, ducq, subscriber);
	expect_string(_send, buf, expected_msg);
	expect_value(_send, *count, count);
	will_return(_send, DUCQ_OK);

	expect_string(mock_log, function_name, "sub");
	expect_value(mock_log, level, DUCQ_LOG_WARNING);

	// act
	ducq_state actual_state = subscribe(reactor, subscriber, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	ducq_reactor_free(reactor);
	ducq_free(subscriber); // err, not freed
}



void subscribe_add_subscriber_to_reactor_subs(void **state) {
	//arrange
	ducq_command_f subscribe = get_command(state);

	ducq_reactor *reactor = ducq_reactor_new();
	const char *expected_id = "id";
	ducq_i *subscriber = ducq_new_mock(expected_id);
	ducq_reactor_add_client(reactor, 10, subscriber);

	char buffer[] = "sub ROUTE\npayload";
	size_t size = sizeof(buffer);


	expect_value(_send, ducq, subscriber);
	expect_any(_send, buf);
	expect_any(_send, *count);
	will_return(_send, DUCQ_OK);

	ducq_state expected_state = DUCQ_OK;
	int expected_count = 1;
	int expected_match = 1;

	// act
	ducq_state actual_state = subscribe(reactor, subscriber, buffer, size);
	
	//audit
	assert_int_equal(expected_state, actual_state);
	struct sub_check_ctx ctx = {
		.ducq =  subscriber,
		.route = "ROUTE",
		.count = 0,
		.match = 0
	};
	ducq_reactor_loop(reactor, _check_sub_added, &ctx);
	assert_int_equal(expected_count, ctx.count);
	assert_int_equal(expected_match, ctx.match);

	//teardown
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_reactor_free(reactor);
}



void subscribe_add_second_subscriber_to_reactor_subs(void **state) {
//arrange
	ducq_command_f subscribe = get_command(state);

	ducq_reactor *reactor = ducq_reactor_new();
	const char *expected_id1 = "id1";
	const char *expected_id2 = "id2";
	ducq_i *subscriber1 = ducq_new_mock(expected_id1);
	ducq_i *subscriber2 = ducq_new_mock(expected_id2);
	ducq_reactor_add_client(reactor, 10, subscriber1);
	ducq_reactor_add_client(reactor, 20, subscriber2);

	char buffer[] = "sub ROUTE\npayload";
	size_t size = sizeof(buffer);

	expect_value(_send, ducq, subscriber1);
	expect_value(_send, ducq, subscriber2);
	expect_any_count(_send, buf, 2);
	expect_any_count(_send, *count, 2);
	will_return_count(_send, DUCQ_OK, 2);

	
	ducq_state expected_state = DUCQ_OK;
	int expected_count = 2;
	int expected_match = 1;

	// act
	ducq_state actual_state1 = subscribe(reactor, subscriber1, buffer, size);
	ducq_state actual_state2 = subscribe(reactor, subscriber2, buffer, size);
	
	//audit
	assert_int_equal(expected_state, actual_state1);
	assert_int_equal(expected_state, actual_state2);
	struct sub_check_ctx ctx = {
		.ducq  = subscriber1,
		.route = "ROUTE",
		.count = 0,
		.match = 0
	};
	ducq_reactor_loop(reactor, _check_sub_added, &ctx);
	assert_int_equal(expected_count, ctx.count);
	assert_int_equal(expected_match, ctx.match);
	ctx.ducq = subscriber2;
	ctx.route = "ROUTE";
	ctx.count = 0;
	ctx.match = 0;
	ducq_reactor_loop(reactor, _check_sub_added, &ctx);
	assert_int_equal(expected_count, ctx.count);
	assert_int_equal(expected_match, ctx.match);

	//teardown
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_reactor_free(reactor);
}


void subscribe_subscribe_inexistent_return_not_found(void **state) {
	//arrange
	ducq_command_f subscribe = get_command(state);

	ducq_reactor *reactor = ducq_reactor_new();
	const char *expected_id1 = "id1";
	const char *expected_id2 = "id2";
	ducq_i *connected    = ducq_new_mock(expected_id1);
	ducq_i *notconnected = ducq_new_mock(expected_id2);
	ducq_reactor_add_client(reactor, 10, connected);
	char msg[] = "sub ROUTE\npayload";
	size_t size_msg = sizeof(msg);

	char buffer_ok[] = "ACK";
	size_t size_ok = sizeof(buffer_ok);

	char buffer_error[DUCQ_MSGSZ] = "";
	size_t size_error = snprintf(buffer_error, DUCQ_MSGSZ, "NACK *\n%d\n%s", DUCQ_ENOTFOUND, ducq_state_tostr(DUCQ_ENOTFOUND));

	
	expect_value(_send, ducq, connected);
	expect_string(_send, buf, buffer_ok);
	expect_value(_send, *count, size_ok-1);
	will_return(_send, DUCQ_OK);

	expect_value(_send, ducq, notconnected);
	expect_string(_send, buf, buffer_error);
	expect_value(_send, *count, size_error);
	will_return(_send, DUCQ_OK);

	
	ducq_state expected_existent_state   = DUCQ_OK;
	ducq_state expected_inexistent_state = DUCQ_ENOTFOUND;
	int expected_count = 1;
	int expected_match = 1;

	// act
	ducq_state actual_existent_state   = subscribe(reactor,    connected, msg, size_msg);
	ducq_state actual_inexistent_state = subscribe(reactor, notconnected, msg, size_msg);

	//audit
	assert_int_equal(expected_existent_state,   actual_existent_state);
	assert_int_equal(expected_inexistent_state, actual_inexistent_state);
	struct sub_check_ctx ctx = {
		.ducq  = connected,
		.route = "ROUTE",
		.count = 0,
		.match = 0
	};
	ducq_reactor_loop(reactor, _check_sub_added, &ctx);
	assert_int_equal(expected_count, ctx.count);
	assert_int_equal(expected_match, ctx.match);

	//teardown
	ducq_free(notconnected);
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_reactor_free(reactor);
}



void subscribe_send_ack_fail_cleans_up(void **state) {
//arrange
	ducq_command_f subscribe = get_command(state);

	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, NULL, mock_log);

	ducq_i *subscriber = ducq_new_mock(NULL);
	ducq_reactor_add_client(reactor, 10, subscriber);

	char buffer[] = "sub ROUTE\npayload";
	size_t size = sizeof(buffer);

	
	expect_value(_send, ducq, subscriber);
	expect_any(_send, buf);
	expect_any(_send, *count);
	will_return(_send, DUCQ_EWRITE);

	expect_string(mock_log, function_name, "sub");
	expect_value(mock_log, level, DUCQ_LOG_WARNING);


	expect_value(_close, ducq, subscriber);
	will_return(_close, DUCQ_OK);

	ducq_state expected_state = DUCQ_EWRITE;
	int expected_count = 0;
	int expected_match = 0;

	// act
	ducq_state actual_state = subscribe(reactor, subscriber, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);
	struct sub_check_ctx ctx = {
		.ducq  = subscriber,
		.route = "ROUTE",
		.count = 0,
		.match = 0
	};
	ducq_reactor_loop(reactor, _check_sub_added, &ctx);
	assert_int_equal(expected_count, ctx.count);
	assert_int_equal(expected_match, ctx.match);

	//teardown
	ducq_reactor_free(reactor);
}
