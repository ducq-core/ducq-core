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
#include "../src/ducq_srv.h"
#include "../src/ducq_srv_int.h"


int subscribe_tests_setup(void **state) {
	*state = fix_new("subscribe");
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
		assert_ptr_not_equal(ducq, expected->ducq);
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

	ducq_srv *srv = ducq_srv_new();
	ducq_srv_set_log(srv, NULL, mock_log);

	ducq_i *subscriber = ducq_new_mock(NULL);
	char buffer[] = "subscriberoute\npayload";
	size_t size = sizeof(buffer);

	char expected_msg[64] = "";
	size_t count = snprintf(expected_msg, 64, "NACK *\n%d\n%s", expected_state, ducq_state_tostr(expected_state));
	expect_value(_send, ducq, subscriber);
	expect_string(_send, buf, expected_msg);
	expect_value(_send, *count, count);
	will_return(_send, DUCQ_OK);

	expect_string(mock_log, function_name, "subscribe");
	expect_value(mock_log, level, DUCQ_LOG_WARN);

	// act
	ducq_state actual_state = subscribe(srv, subscriber, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	ducq_srv_free(srv);
	ducq_free(subscriber); // err, not freed
}



void subscribe_add_subscriber_to_srv_subs(void **state) {
	//arrange
	ducq_command_f subscribe = get_command(state);

	ducq_srv *srv = ducq_srv_new();
	const char *expected_id = "id";
	ducq_i *subscriber = ducq_new_mock(expected_id);
	char buffer[] = "subscribe ROUTE\npayload";
	size_t size = sizeof(buffer);

	will_return(_copy, ducq_new_mock(ducq_id(subscriber)));

	expect_value(_send, ducq, subscriber);
	expect_any(_send, buf);
	expect_any(_send, *count);
	will_return(_send, DUCQ_OK);

	ducq_state expected_state = DUCQ_OK;
	int expected_count = 1;
	int expected_match = 1;

	// act
	ducq_state actual_state = subscribe(srv, subscriber, buffer, size);
	
	//audit
	assert_int_equal(expected_state, actual_state);
	struct sub_check_ctx ctx = {
		.ducq =  subscriber,
		.route = "ROUTE",
		.count = 0,
		.match = 0
	};
	ducq_srv_loop(srv, _check_sub_added, &ctx);
	assert_int_equal(expected_count, ctx.count);
	assert_int_equal(expected_match, ctx.match);

	//teardown
	ducq_free(subscriber);
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
}



void subscribe_add_second_subscriber_to_srv_subs(void **state) {
//arrange
	ducq_command_f subscribe = get_command(state);

	ducq_srv *srv = ducq_srv_new();
	const char *expected_id1 = "id1";
	const char *expected_id2 = "id2";
	ducq_i *subscriber1 = ducq_new_mock(expected_id1);
	ducq_i *subscriber2 = ducq_new_mock(expected_id2);
	char buffer[] = "subscribe ROUTE\npayload";
	size_t size = sizeof(buffer);

	will_return(_copy, ducq_new_mock(ducq_id(subscriber1)));
	will_return(_copy, ducq_new_mock(ducq_id(subscriber2)));
	
	expect_value(_send, ducq, subscriber1);
	expect_value(_send, ducq, subscriber2);
	expect_any_count(_send, buf, 2);
	expect_any_count(_send, *count, 2);
	will_return_count(_send, DUCQ_OK, 2);

	
	ducq_state expected_state = DUCQ_OK;
	int expected_count = 2;
	int expected_match = 1;

	// act
	ducq_state actual_state1 = subscribe(srv, subscriber1, buffer, size);
	ducq_state actual_state2 = subscribe(srv, subscriber2, buffer, size);
	
	//audit
	assert_int_equal(expected_state, actual_state1);
	assert_int_equal(expected_state, actual_state2);
	struct sub_check_ctx ctx = {
		.ducq  = subscriber1,
		.route = "ROUTE",
		.count = 0,
		.match = 0
	};
	ducq_srv_loop(srv, _check_sub_added, &ctx);
	assert_int_equal(expected_count, ctx.count);
	assert_int_equal(expected_match, ctx.match);
	ctx.ducq = subscriber2;
	ctx.route = "ROUTE";
	ctx.count = 0;
	ctx.match = 0;
	ducq_srv_loop(srv, _check_sub_added, &ctx);
	assert_int_equal(expected_count, ctx.count);
	assert_int_equal(expected_match, ctx.match);

	//teardown
	ducq_free(subscriber1);
	ducq_free(subscriber2);
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
}


void subscribe_add_second_subscriber_makes_copy(void **state) {
	//arrange
	ducq_command_f subscribe = get_command(state);

	ducq_srv *srv = ducq_srv_new();
	const char *expected_id1 = "id1";
	const char *expected_id2 = "id2";
	ducq_i *subscriber1 = ducq_new_mock(expected_id1);
	ducq_i *subscriber2 = ducq_new_mock(expected_id2);
	char buffer[] = "subscribe ROUTE\npayload";
	size_t size = sizeof(buffer);

	will_return(_copy, ducq_new_mock(ducq_id(subscriber1)));
	will_return(_copy, ducq_new_mock(ducq_id(subscriber2)));
	
	expect_value(_send, ducq, subscriber1);
	expect_value(_send, ducq, subscriber2);
	expect_any_count(_send, buf, 2);
	expect_any_count(_send, *count, 2);
	will_return_count(_send, DUCQ_OK, 2);

	
	ducq_state expected_state = DUCQ_OK;
	int expected_count = 2;
	int expected_match = 0;

	// act
	ducq_state actual_state1 = subscribe(srv, subscriber1, buffer, size);
	ducq_state actual_state2 = subscribe(srv, subscriber2, buffer, size);
	ducq_free(subscriber1);
	ducq_free(subscriber2);

	//audit
	assert_int_equal(expected_state, actual_state1);
	assert_int_equal(expected_state, actual_state2);
	struct sub_check_ctx ctx = {
		.ducq  = NULL,
		.route = "ROUTE",
		.count = 0,
		.match = 0
	};
	ducq_srv_loop(srv, _check_sub_added, &ctx);
	assert_int_equal(expected_count, ctx.count);
	ctx.count = 0;
	ctx.match = 0;
	ducq_srv_loop(srv, _check_sub_added, &ctx);
	assert_int_equal(expected_count, ctx.count);
	assert_int_equal(expected_match, ctx.match);

	//teardown
	// ducq_free(subscriber1);
	// ducq_free(subscriber2);
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
}


void subscribe_mem_error_cleans_up(void **state) {
	//arrange
	ducq_command_f subscribe = get_command(state);

	ducq_srv *srv = ducq_srv_new();
	ducq_srv_set_log(srv, NULL, mock_log);

	ducq_i *subscriber = ducq_new_mock(NULL);
	char buffer[] = "subscribe ROUTE\npayload";
	size_t size = sizeof(buffer);

	will_return(_copy, NULL);

	char expected_res[100] = "";
	size_t reslen = snprintf(expected_res, 100, "NACK *\n%d\n%s", DUCQ_EMEMFAIL, ducq_state_tostr(DUCQ_EMEMFAIL));
	expect_value(_send, ducq, subscriber);
	expect_string(_send, buf, expected_res);
	expect_value(_send, *count, reslen);
	will_return(_send, DUCQ_OK);

	expect_string(mock_log, function_name, "subscribe");
	expect_value(mock_log, level, DUCQ_LOG_ERROR); // mem fail
	expect_string(mock_log, function_name, "subscribe");
	expect_value(mock_log, level, DUCQ_LOG_INFO );

	ducq_state expected_state = DUCQ_EMEMFAIL;
	int expected_count = 0;
	int expected_match = 0;

	// act
	ducq_state actual_state = subscribe(srv, subscriber, buffer, size);
	
	//audit
	assert_int_equal(expected_state, actual_state);
	struct sub_check_ctx ctx = {
		.ducq  = NULL,
		.count = 0,
		.match = 0
	};
	ducq_srv_loop(srv, _check_sub_added, &ctx);
	assert_int_equal(expected_count, ctx.count);
	assert_int_equal(expected_match, ctx.match);

	//teardown
	ducq_srv_free(srv);
	ducq_free(subscriber);
}




void subscribe_send_ack_fail_cleans_up(void **state) {
//arrange
	ducq_command_f subscribe = get_command(state);

	ducq_srv *srv = ducq_srv_new();
	ducq_srv_set_log(srv, NULL, mock_log);

	ducq_i *subscriber = ducq_new_mock(NULL);
	char buffer[] = "subscribe ROUTE\npayload";
	size_t size = sizeof(buffer);

	ducq_i *copy = ducq_new_mock(ducq_id(subscriber));
	will_return(_copy, copy);
	
	expect_value(_send, ducq, subscriber);
	expect_any(_send, buf);
	expect_any(_send, *count);
	will_return(_send, DUCQ_EWRITE);

	expect_string(mock_log, function_name, "subscribe");
	expect_value(mock_log, level, DUCQ_LOG_WARN);


	expect_value(_close, ducq, copy);
	will_return(_close, DUCQ_OK);

	ducq_state expected_state = DUCQ_EWRITE;
	int expected_count = 0;
	int expected_match = 0;

	// act
	ducq_state actual_state = subscribe(srv, subscriber, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);
	struct sub_check_ctx ctx = {
		.ducq  = subscriber,
		.route = "ROUTE",
		.count = 0,
		.match = 0
	};
	ducq_srv_loop(srv, _check_sub_added, &ctx);
	assert_int_equal(expected_count, ctx.count);
	assert_int_equal(expected_match, ctx.match);

	//teardown
	ducq_srv_free(srv);
	ducq_free(subscriber);
}
