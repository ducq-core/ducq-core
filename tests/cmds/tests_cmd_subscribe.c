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



void subscribe_msg_invalide_if_cant_parse_route(void **state) {
//arrange
	command_f subscribe = get_command(state);
	
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
	command_f subscribe = get_command(state);

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
	char  expected_route[] = "ROUTE";

	// act
	ducq_state actual_state = subscribe(srv, subscriber, buffer, size);
	ducq_sub *sub = srv->subs;
	char *actual_route = sub->route;
	const char *actual_id = sub->id;

	//audit
	assert_int_equal(expected_state, actual_state);
	assert_ptr_not_equal(sub->ducq, subscriber);
	assert_string_equal(actual_route, expected_route);
	assert_string_equal(expected_id, actual_id);

	//teardown
	ducq_free(subscriber);
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
}



void subscribe_add_second_subscriber_to_srv_subs(void **state) {
//arrange
	command_f subscribe = get_command(state);

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
	char  expected_route[] = "ROUTE";

	// act
	ducq_state actual_state1 = subscribe(srv, subscriber1, buffer, size);
	ducq_state actual_state2 = subscribe(srv, subscriber2, buffer, size);
	ducq_sub *sub2 = srv->subs;
	ducq_sub *sub1 = sub2->next;
	char *actual_route1 = sub1->route;
	char *actual_route2 = sub2->route;
	const char *actual_id1 = sub1->id;
	const char *actual_id2 = sub2->id;

	//audit
	assert_int_equal(expected_state, actual_state1);
	assert_int_equal(expected_state, actual_state2);
	assert_string_equal(actual_route1, expected_route);
	assert_string_equal(actual_route2, expected_route);
	assert_string_equal(expected_id1, actual_id1);
	assert_string_equal(expected_id2, actual_id2);

	//teardown
	ducq_free(subscriber1);
	ducq_free(subscriber2);
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
}


void subscribe_add_second_subscriber_makes_copy(void **state) {
//arrange
	command_f subscribe = get_command(state);

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
	char  expected_route[] = "ROUTE";

	// act
	ducq_state actual_state1 = subscribe(srv, subscriber1, buffer, size);
	ducq_free(subscriber1);
	ducq_state actual_state2 = subscribe(srv, subscriber2, buffer, size);
	ducq_free(subscriber2);
	ducq_sub *sub2 = srv->subs;
	ducq_sub *sub1 = sub2->next;
	char *actual_route1 = sub1->route;
	char *actual_route2 = sub2->route;
	const char *actual_id1 = sub1->id;
	const char *actual_id2 = sub2->id;

	//audit
	assert_int_equal(expected_state, actual_state1);
	assert_int_equal(expected_state, actual_state2);
	assert_string_equal(actual_route1, expected_route);
	assert_string_equal(actual_route2, expected_route);
	assert_string_equal(expected_id1, actual_id1);
	assert_string_equal(expected_id2, actual_id2);

	//teardown
	// ducq_free(subscriber1);
	// ducq_free(subscriber2);
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
}


void subscribe_mem_error_cleans_up(void **state) {
//arrange
	command_f subscribe = get_command(state);

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
	expect_value(mock_log, level, DUCQ_LOG_ERROR);

	ducq_state expected_state = DUCQ_EMEMFAIL;

	// act
	ducq_state actual_state = subscribe(srv, subscriber, buffer, size);
	
	//audit
	assert_int_equal(expected_state, actual_state);
	assert_null(srv->subs);

	//teardown
	ducq_srv_free(srv);
	ducq_free(subscriber);
}




void subscribe_send_ack_fail_cleans_up(void **state) {
//arrange
	command_f subscribe = get_command(state);

	ducq_srv *srv = ducq_srv_new();
	ducq_srv_set_log(srv, NULL, mock_log);

	ducq_i *subscriber = ducq_new_mock(NULL);
	char buffer[] = "subscribe ROUTE\npayload";
	size_t size = sizeof(buffer);

	will_return(_copy, subscriber);
	
	expect_value(_send, ducq, subscriber);
	expect_any(_send, buf);
	expect_any(_send, *count);
	will_return(_send, DUCQ_EWRITE);

	expect_string(mock_log, function_name, "subscribe");
	expect_value(mock_log, level, DUCQ_LOG_WARN);


	expect_value(_close, ducq, subscriber);
	will_return(_close, DUCQ_OK);

	ducq_state expected_state = DUCQ_EWRITE;

	// act
	ducq_state actual_state = subscribe(srv, subscriber, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);
	assert_null(srv->subs);

	//teardown
	ducq_srv_free(srv);
}