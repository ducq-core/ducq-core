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


extern char MOCK_CLIENT_RECV_BUFFER[];
extern unsigned MOCK_CLIENT_RECV_BUFFER_LEN;




void dispatcher_can_load_a_command(void **state) {
	//arrange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);
	ducq_state expected_state = DUCQ_OK;

	// act
	ducq_state actual_state = ducq_dispatcher_load_commands_path(dispatcher, "./commands");

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	ducq_reactor_free(reactor);
}

void dispatcher_correct_number_of_commands(void **state) {
	//arrange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);

	
	ducq_state expected_state = DUCQ_OK;
	int expected_number_of_commands = 3;
	
	// act
	ducq_state actual_state = ducq_dispatcher_load_commands_path(dispatcher, "./commands");
	int actual_number_of_commands = ducq_dispatcher_count_cmds(dispatcher);
	
	//audit
	assert_int_equal(expected_state, actual_state);
	assert_int_equal(expected_number_of_commands, actual_number_of_commands);

	//teardown
	ducq_reactor_free(reactor);
}
 
 


void dispatcher_commands_receive_parameters(void **state) {
	//arrange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);
	ducq_dispatcher_load_commands_path(dispatcher, "./commands");

	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_OK;
	
	// act && audit
	char msg[DUCQ_MSGSZ] = "";
	size_t size = 0;

	size = snprintf(msg, DUCQ_MSGSZ, "mock_command_a *\n");

	expect_value    (mock_command_a, reactor, reactor);
 	expect_value	(mock_command_a, ducq, ducq);
 	expect_string   (mock_command_a, buffer, msg);
 	expect_value    (mock_command_a, size, size);
 	will_return     (mock_command_a, DUCQ_OK);

	ducq_dispatch(dispatcher, ducq, msg, size);

	size = snprintf(msg, DUCQ_MSGSZ, "mock_command_b *\n");

	expect_value    (mock_command_b, reactor, reactor);
 	expect_value	(mock_command_b, ducq, ducq);
 	expect_string   (mock_command_b, buffer, msg);
 	expect_value    (mock_command_b, size, size);
 	will_return     (mock_command_b, DUCQ_OK);

	ducq_dispatch(dispatcher, ducq, msg, size);

	size = snprintf(msg, DUCQ_MSGSZ, "mock_command_c *\n");

	expect_value    (mock_command_c, reactor, reactor);
 	expect_value	(mock_command_c, ducq, ducq);
 	expect_string   (mock_command_c, buffer, msg);
 	expect_value    (mock_command_c, size, size);
 	will_return     (mock_command_c, DUCQ_OK);

	ducq_dispatch(dispatcher, ducq, msg, size);

	//teardown
	ducq_reactor_free(reactor);
	ducq_free(ducq);
}







 
 
void dispatcher_sender_receive_nack_if_command_unknown(void **state) {
	// arange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);
	ducq_dispatcher_load_commands_path(dispatcher, "./commands");

	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_reactor_add_client(reactor, 11, ducq);

	ducq_state expected_state = DUCQ_ENOCMD;
	char expected_nack_msg[100] = "";
	size_t expected_count = snprintf(expected_nack_msg, 100, 
		"NACK *\n%d\n%s", expected_state, ducq_state_tostr(expected_state));
	

	expect_value (_send, ducq, ducq);
	expect_string(_send, buf, expected_nack_msg);
	expect_value(_send, *count, expected_count);
	will_return  (_send, DUCQ_OK);

	// act
	char request[] = "UNKNOWN_COMMAND route/\npayload";
	ducq_state actual_state = ducq_dispatch(dispatcher, ducq, request, sizeof(request));

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	expect_any_count(_close, ducq, 1);
	will_return_count(_close, DUCQ_OK, 1);
	ducq_reactor_free(reactor);
}





void dispatcher_returns_command_state(void **state) {
	// arange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);
	ducq_dispatcher_load_commands_path(dispatcher, "./commands");

	ducq_i *ducq = ducq_new_mock(NULL);

	ducq_state expected_state = DUCQ_EMSGINV;

	// act
	char msg[DUCQ_MSGSZ] = "";
	size_t size = 0;

	size = snprintf(msg, DUCQ_MSGSZ, "mock_command_a *\n");

	expect_any	(mock_command_a, reactor);
 	expect_any	(mock_command_a, ducq);
 	expect_any	(mock_command_a, buffer);
 	expect_any	(mock_command_a, size);
 	will_return	(mock_command_a, expected_state);

	ducq_state actual_state = ducq_dispatch(dispatcher, ducq, msg, size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_reactor_free(reactor);
	ducq_free(ducq);
}
