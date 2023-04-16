#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests_srv_parse.h"


#include "../src/ducq_srv.h"
#include "../src/ducq_srv_int.h"
#include "mock_ducq_client.h"


void srv_ctor_dtor_no_leak(void **state) {
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_free(srv);
}


void srv_parse_command_ok(void **state) {
	// arange
	char msg[] = "COMMAND route\npayload";
	char expected_command[] = "COMMAND";
	char *expected_command_end = msg + strlen(expected_command);

	// act
	const char *actual_command_end = NULL;
	const char *actual_command = parse_command(msg, &actual_command_end);

	// audit
	assert_ptr_equal(expected_command_end, actual_command_end);
	assert_memory_equal(expected_command, actual_command, actual_command_end-actual_command);
}

void srv_parse_command_no_space_returns_null(void **state) {
	// arange
	char msg[] = "COMMANDroute\npayload";

	// act
	const char *actual_command_end = NULL;
	const char *actual_command = parse_command(msg, &actual_command_end);

	// audit
	assert_null(actual_command);
	assert_null(actual_command_end);
}

void srv_parse_route_ok(void **state) {
	// arange
	char msg[] = "command ROUTE\npayload";
	char expected_route[] = "ROUTE";
	char *expected_route_end = strchr(msg, '\n');

	// act
	const char *actual_route_end = NULL;
	const char *actual_route = parse_route(msg, &actual_route_end);

	// audit
	assert_ptr_equal(expected_route_end, actual_route_end);
	assert_memory_equal(expected_route, actual_route, actual_route_end-actual_route);
}

void srv_parse_route_after_parse_command_ok(void **state) {
	// arange
	char msg[] = "command ROUTE\npayload";
	char expected_route[] = "ROUTE";
	char *expected_route_end = strchr(msg, '\n');

	// act
	const char *actual_route_end = NULL;
	const char *command      = parse_command(msg, &actual_route_end);
	const char *actual_route = parse_route(actual_route_end, &actual_route_end);

	// audit
	assert_ptr_equal(expected_route_end, actual_route_end);
	assert_memory_equal(expected_route, actual_route, actual_route_end-actual_route);
}

void srv_parse_route_no_space_err(void **state) {
	// arange
	char msg[] = "commandROUTE\npayload";

	// act
	const char *actual_route_end = NULL;
	const char *actual_route = parse_route(msg, &actual_route_end);

	// audit
	assert_null(actual_route);
	assert_null(actual_route_end);
}

void srv_parse_route_no_newline_err(void **state) {
	// arange
	char msg[] = "command ROUTEpayload";

	// act
	const char *actual_route_end = NULL;
	const char *actual_route = parse_route(msg, &actual_route_end);

	// audit
	assert_null(actual_route);
	assert_null(actual_route_end);
}


void srv_parse_send_ack_ok(void **state) {
	// arange
	char expected_message[] = "ACK *\n0\nok";
	ducq_i *ducq = ducq_new_mock();
	ducq_state expected_state = DUCQ_OK;

	will_return(_send, expected_state);
	expect_value(_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	size_t expected_size = strlen(expected_message);
	expect_memory(_send, count, &expected_size, sizeof(size_t));

	// act
	ducq_state actual_state = send_ack(ducq, DUCQ_OK);
	

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void srv_parse_send_ack_returns_send_rc(void **state) {
	// arange
	ducq_state expected_state = DUCQ_EWRITE;
	ducq_i *ducq = ducq_new_mock();

	will_return(_send, expected_state);
	expect_any(_send, ducq);
	expect_any(_send, buf);
	expect_any(_send, count);

	// act
	ducq_state actual_state = send_ack(ducq, DUCQ_OK);
	

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void srv_parse_send_nack_ok(void **state) {
	// arange
	char expected_message[] = "NACK *\n11\nmessage too big";
	ducq_i *ducq = ducq_new_mock();
	ducq_state expected_state = DUCQ_OK;

	will_return(_send, expected_state);
	expect_value(_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	size_t expected_size = strlen(expected_message);
	expect_memory(_send, count, &expected_size, sizeof(size_t));

	// act
	ducq_state actual_state = send_ack(ducq, DUCQ_EMSGSIZE);
	

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void srv_parse_send_nack_returns_send_rc(void **state) {
	// arange
	ducq_state expected_state = DUCQ_EWRITE;
	ducq_i *ducq = ducq_new_mock();

	will_return(_send, expected_state);
	expect_any(_send, ducq);
	expect_any(_send, buf);
	expect_any(_send, count);

	// act
	ducq_state actual_state = send_ack(ducq, DUCQ_EMSGSIZE);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}