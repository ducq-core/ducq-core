#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests_dispatcher.h"

#include "../src/ducq.h"
#include "../src/ducq_dispatcher.h"
#include "mock_ducq_client.h"



void dispatcher_lua_can_load_a_command(void **state) {
	//arrange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);

	ducq_i *ducq = ducq_new_mock(NULL);
	char message[] = "ReplyHello *\n";
	char expected_message[] = "hello, world";

	ducq_state expected_state = DUCQ_OK;

	// mock
	expect_value (_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	expect_value(_send, *count, strlen(expected_message));
	will_return(_send, DUCQ_OK);

	// act
	ducq_state actual_add_state = ducq_dispatcher_add(dispatcher, ".");
	ducq_state actual_dispatch_state =
		ducq_dispatch(dispatcher, ducq, message, strlen(message)); 

	//audit
	assert_int_equal(expected_state, actual_add_state);
	assert_int_equal(expected_state, actual_dispatch_state);

	//teardown
	ducq_reactor_free(reactor);
	ducq_free(ducq);
}

void dispatcher_lua_choose_right_command(void **state) {
	//arrange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);

	char client_id[] = "client_id";
	ducq_i *ducq = ducq_new_mock(client_id);
	char message[] = "MyId *\n";
	char *expected_message = client_id;

	ducq_state expected_state = DUCQ_OK;

	// mock
	expect_value (_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	expect_value(_send, *count, strlen(expected_message));
	will_return(_send, DUCQ_OK);

	// act
	ducq_state actual_add_state = ducq_dispatcher_add(dispatcher, ".");
	ducq_state actual_dispatch_state =
		ducq_dispatch(dispatcher, ducq, message, strlen(message)); 

	//audit
	assert_int_equal(expected_state, actual_add_state);
	assert_int_equal(expected_state, actual_dispatch_state);

	//teardown
	ducq_reactor_free(reactor);
	ducq_free(ducq);
}

void dispatcher_lua_give_msg_as_userdata(void **state) {
	//arrange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);

	ducq_i *ducq = ducq_new_mock(NULL);
	char message[] = "echo *\nhello, world";
	char expected_message[sizeof(message)] = ""; // copy, msg
	strcpy(expected_message, message);            // splited by parser

	ducq_state expected_state = DUCQ_OK;

	// mock
	expect_value (_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	expect_value(_send, *count, strlen(expected_message));
	will_return(_send, DUCQ_OK);

	// act
	ducq_state actual_add_state = ducq_dispatcher_add(dispatcher, ".");
	ducq_state actual_dispatch_state =
		ducq_dispatch(dispatcher, ducq, message, strlen(message));

	//audit
	assert_int_equal(expected_state, actual_add_state);
	assert_int_equal(expected_state, actual_dispatch_state);

	//teardown
	ducq_reactor_free(reactor);
	ducq_free(ducq);
}

void dispatcher_lua_not_found_return_unknown(void **state) {
	//arrange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);

	ducq_i *ducq = ducq_new_mock(NULL);
	char message[] = "NotAFunction *\n";

	ducq_state expected_state = DUCQ_ENOCMD;
	char expected_message[DUCQ_MSGSZ] = "";
	snprintf(expected_message, DUCQ_MSGSZ, "NACK *\n%d\n%s",
		DUCQ_ENOCMD, ducq_state_tostr(DUCQ_ENOCMD));


	// mock
	expect_value (_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	expect_value(_send, *count, strlen(expected_message));
	will_return(_send, DUCQ_OK);

	// act
	ducq_dispatcher_add(dispatcher, ".");
	ducq_state actual_state =
		ducq_dispatch(dispatcher, ducq, message, strlen(message)); 

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	ducq_reactor_free(reactor);
	ducq_free(ducq);
}

void dispatcher_lua_returns_command_value(void **state) {
	//arrange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);

	ducq_i *ducq = ducq_new_mock(NULL);
	char message[] = "Return *\n";

	ducq_state expected_state = DUCQ_EWRITE;

	// mock
	expect_any (_send, ducq);
	expect_any (_send, buf);
	expect_any (_send, *count);
	will_return(_send, expected_state);

	// act
	ducq_dispatcher_add(dispatcher, ".");
	ducq_state actual_state =
		ducq_dispatch(dispatcher, ducq, message, strlen(message)); 

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	ducq_reactor_free(reactor);
	ducq_free(ducq);
}
