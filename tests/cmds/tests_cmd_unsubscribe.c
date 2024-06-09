#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include "mock_ducq_client.h"
#include "unit_tests_cmd.h"

#include "../src/ducq.h"
#include "../src/ducq_reactor.h"


int unsubscribe_tests_setup(void **state) {
	*state = fix_new("unsubscribe");
	return *state == NULL;
}
int unsubscribe_tests_teardown(void **state) {
	struct fixture *fix = *state;
return fix_free(fix);
}



void unsubscribe_close_connection_and_remove_client(void **state) {
	//arrange
	ducq_command_f unsubscribe = get_command(state);
	
	ducq_state expected_state = DUCQ_OK;

	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, NULL, mock_log);

	ducq_i *sender = ducq_new_mock("sender");
	ducq_i *to_unsub = ducq_new_mock("to_unsub");
	ducq_i *other = ducq_new_mock("other");

	ducq_reactor_add_client(reactor, 10, sender);
	ducq_reactor_add_client(reactor, 11, to_unsub);
	ducq_reactor_add_client(reactor, 12, other);

	char buffer[] = "unsubscribe *\nto_unsub";
	size_t size = sizeof(buffer);

	// mock
	expect_string(mock_log, function_name, "unsubscribe");
	expect_value(mock_log, level, DUCQ_LOG_INFO);

	expect_value(_close, ducq, to_unsub);
	will_return(_close, DUCQ_OK);

	// act
	ducq_state actual_state = unsubscribe(reactor, sender, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	expect_value(_close, ducq, sender);
	expect_value(_close, ducq, other);
	will_return_count(_close, DUCQ_OK, 2);
	ducq_reactor_free(reactor);
}

void unsubscribe_client_not_found(void **state) {
	//arrange
	ducq_command_f unsubscribe = get_command(state);
	
	ducq_state expected_state = DUCQ_ENOTFOUND;

	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, NULL, mock_log);

	ducq_i *sender = ducq_new_mock("sender");
	ducq_i *other = ducq_new_mock("other");

	ducq_reactor_add_client(reactor, 10, sender);
	ducq_reactor_add_client(reactor, 11, other);

	char buffer[] = "unsubscribe *\nto_unsub";
	size_t size = sizeof(buffer);

	// mock
	expect_string(mock_log, function_name, "unsubscribe");
	expect_value(mock_log, level, DUCQ_LOG_WARNING);

	// act
	ducq_state actual_state = unsubscribe(reactor, sender, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	expect_value(_close, ducq, sender);
	expect_value(_close, ducq, other);
	will_return_count(_close, DUCQ_OK, 2);
	ducq_reactor_free(reactor);
}

void unsubscribe_no_payload(void **state) {
	//arrange
	ducq_command_f unsubscribe = get_command(state);
	
	ducq_state expected_state = DUCQ_EMSGINV;

	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, NULL, mock_log);

	ducq_i *sender = ducq_new_mock("sender");
	ducq_i *to_unsub = ducq_new_mock("to_unsub");
	ducq_i *other = ducq_new_mock("other");

	ducq_reactor_add_client(reactor, 10, sender);
	ducq_reactor_add_client(reactor, 11, to_unsub);
	ducq_reactor_add_client(reactor, 12, other);

	char buffer[] = "unsubscribe *";
	size_t size = sizeof(buffer);

	// mock
	expect_string(mock_log, function_name, "unsubscribe");
	expect_value(mock_log, level, DUCQ_LOG_WARNING);

	// act
	ducq_state actual_state = unsubscribe(reactor, sender, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	expect_value(_close, ducq, sender);
	expect_value(_close, ducq, to_unsub);
	expect_value(_close, ducq, other);
	will_return_count(_close, DUCQ_OK, 3);
	ducq_reactor_free(reactor);
}

void unsubscribe_empty_payload(void **state) {
	//arrange
	ducq_command_f unsubscribe = get_command(state);
	
	ducq_state expected_state = DUCQ_EMSGINV;

	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, NULL, mock_log);

	ducq_i *sender = ducq_new_mock("sender");
	ducq_i *to_unsub = ducq_new_mock("to_unsub");
	ducq_i *other = ducq_new_mock("other");

	ducq_reactor_add_client(reactor, 10, sender);
	ducq_reactor_add_client(reactor, 11, to_unsub);
	ducq_reactor_add_client(reactor, 12, other);

	char buffer[] = "unsubscribe *\n";
	size_t size = sizeof(buffer);

	// mock
	expect_string(mock_log, function_name, "unsubscribe");
	expect_value(mock_log, level, DUCQ_LOG_WARNING);

	// act
	ducq_state actual_state = unsubscribe(reactor, sender, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	expect_value(_close, ducq, sender);
	expect_value(_close, ducq, to_unsub);
	expect_value(_close, ducq, other);
	will_return_count(_close, DUCQ_OK, 3);
	ducq_reactor_free(reactor);
}

