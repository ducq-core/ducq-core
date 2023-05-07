#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests_parse.h"

#include "ducq.h"


void parse_command_ok(void **state) {
	// arange
	char msg[] = "COMMAND route\npayload";
	char expected_command[] = "COMMAND";
	char *expected_command_end = msg + strlen(expected_command);

	// act
	const char *actual_command_end = NULL;
	const char *actual_command = ducq_parse_command(msg, &actual_command_end);

	// audit
	assert_ptr_equal(expected_command_end, actual_command_end);
	assert_memory_equal(expected_command, actual_command, actual_command_end-actual_command);
}

void parse_command_no_space_returns_null(void **state) {
	// arange
	char msg[] = "COMMANDroute\npayload";

	// act
	const char *actual_command_end = NULL;
	const char *actual_command = ducq_parse_command(msg, &actual_command_end);

	// audit
	assert_null(actual_command);
	assert_null(actual_command_end);
}

void parse_route_ok(void **state) {
	// arange
	char msg[] = "command ROUTE\npayload";
	char expected_route[] = "ROUTE";
	char *expected_route_end = strchr(msg, '\n');

	// act
	const char *actual_route_end = NULL;
	const char *actual_route = ducq_parse_route(msg, &actual_route_end);

	// audit
	assert_ptr_equal(expected_route_end, actual_route_end);
	assert_memory_equal(expected_route, actual_route, actual_route_end-actual_route);
}

void parse_route_after_parse_command_ok(void **state) {
	// arange
	char msg[] = "command ROUTE\npayload";
	char expected_route[] = "ROUTE";
	char *expected_route_end = strchr(msg, '\n');

	// act
	const char *actual_route_end = NULL;
	const char *command      = ducq_parse_command(msg, &actual_route_end);
	const char *actual_route = ducq_parse_route(actual_route_end, &actual_route_end);

	// audit
	assert_ptr_equal(expected_route_end, actual_route_end);
	assert_memory_equal(expected_route, actual_route, actual_route_end-actual_route);
}

void parse_route_no_space_err(void **state) {
	// arange
	char msg[] = "commandROUTE\npayload";

	// act
	const char *actual_route_end = NULL;
	const char *actual_route = ducq_parse_route(msg, &actual_route_end);

	// audit
	assert_null(actual_route);
	assert_null(actual_route_end);
}

void parse_route_no_newline_err(void **state) {
	// arange
	char msg[] = "command ROUTEpayload";

	// act
	const char *actual_route_end = NULL;
	const char *actual_route = ducq_parse_route(msg, &actual_route_end);

	// audit
	assert_null(actual_route);
	assert_null(actual_route_end);
}


void parse_payload_ok(void **state) {
	// arange
	char msg[] = "command route\npayload";
	char expected_payload[] = "payload";

	// act
	const char *actual_payload = ducq_parse_payload(msg);

	// audit
	assert_string_equal(expected_payload, actual_payload);
}

void parse_payload_no_newline_err(void **state) {
	// arange
	char msg[] = "command routePAYLOAD";

	// act
	const char *actual_payload = ducq_parse_payload(msg);

	// audit
	assert_null(actual_payload);
}




void route_cmp_identical_ok(void **state) {
	// arange
	char sub_route[] = "route";
	char pub_route[] = "route";
	bool expected_result = true;

	// act
	bool actual_result = ducq_route_cmp(sub_route, pub_route);

	// audit
	assert_int_equal(expected_result, actual_result);
}

void route_cmp_not_identical_ok(void **state) {
	// arange
	char sub_route[] = "route";
	char pub_route[] = "routeB";
	bool expected_result = false;

	// act
	bool actual_result = ducq_route_cmp(sub_route, pub_route);

	// audit
	assert_int_equal(expected_result, actual_result);
}

void route_cmp_wildcard_at_end_ok(void **state) {
	// arange
	char sub_route[] = "route/*";
	char pub_route[] = "route/subroute";
	bool expected_result = true;

	// act
	bool actual_result = ducq_route_cmp(sub_route, pub_route);

	// audit
	assert_int_equal(expected_result, actual_result);
}


void route_cmp_wildcard_not_at_end_ok(void **state) {
	// arange
	char sub_route[] = "route/*/wrong";
	char pub_route[] = "route/subroute";
	bool expected_result = false;

	// act
	bool actual_result = ducq_route_cmp(sub_route, pub_route);

	// audit
	assert_int_equal(expected_result, actual_result);
}


void route_cmp_wildcard_in_middle_ok(void **state) {
	// arange
	char sub_route[] = "route/*route";
	char pub_route[] = "route/subroute";
	bool expected_result = true;

	// act
	bool actual_result = ducq_route_cmp(sub_route, pub_route);

	// audit
	assert_int_equal(expected_result, actual_result);
}


void route_cmp_wildcard_is_flush_ok(void **state) {
	// arange
	char sub_route[] = "route/*";
	char pub_route[] = "route/";
	bool expected_result = true;

	// act
	bool actual_result = ducq_route_cmp(sub_route, pub_route);

	// audit
	assert_int_equal(expected_result, actual_result);
}

void route_cmp_wildcard_in_middle_flush_ok(void **state) {
	// arange
	char sub_route[] = "route/*!";
	char pub_route[] = "route/subroute!";
	bool expected_result = true;

	// act
	bool actual_result = ducq_route_cmp(sub_route, pub_route);

	// audit
	assert_int_equal(expected_result, actual_result);
}

void route_cmp_wildcard_in_middle_flush_not_matched(void **state) {
	// arange
	char sub_route[] = "route/*!";
	char pub_route[] = "route/subroute";
	bool expected_result = false;

	// act
	bool actual_result = ducq_route_cmp(sub_route, pub_route);

	// audit
	assert_int_equal(expected_result, actual_result);
}

void route_cmp_wildcard_in_middle_flush_not_matched2(void **state) {
	// arange
	char sub_route[] = "hello*!";
	char pub_route[] = "hello";
	bool expected_result = false;

	// act
	bool actual_result = ducq_route_cmp(sub_route, pub_route);

	// audit
	assert_int_equal(expected_result, actual_result);
}