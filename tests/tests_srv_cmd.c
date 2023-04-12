#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests_srv_cmd.h"


#include "../src/ducq.h"
#include "../src/ducq_srv.h"
#include "../src/ducq_srv_int.h"
#include "mock_ducq_client.h"


extern char MOCK_CLIENT_RECV_BUFFER[];
extern unsigned MOCK_CLIENT_RECV_BUFFER_LEN;




void cmd_load_can_load_a_command(void **state) {
	//arrange
	ducq_srv *srv = ducq_srv_new();
	ducq_state expected_state = DUCQ_OK;

	// act
	ducq_state actual_state = ducq_srv_load_commands(srv);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	ducq_srv_free(srv);
}

void cmd_load_correct_number_of_commands(void **state) {
	//arrange
	ducq_srv *srv = ducq_srv_new();
	
	ducq_state expected_state = DUCQ_OK;
	int expected_number_of_commands = 3;
	
	// act
	ducq_state actual_state = ducq_srv_load_commands(srv);
	int actual_number_of_commands = srv->ncmd;
	
	//audit
	assert_int_equal(expected_state, actual_state);
	assert_int_equal(expected_number_of_commands, actual_number_of_commands);

	//teardown
	ducq_srv_free(srv);
}


void cmd_load_commands_have_expected_name(void **state) {
	//arrange
	ducq_srv *srv = ducq_srv_new();

	const char *expected_names[] = {
		"mock_command_a",
		"mock_command_b",
		"mock_command_c"
	};
	ducq_state expected_state = DUCQ_OK;
	
	// act
	ducq_state actual_state = ducq_srv_load_commands(srv);
	const char *actual_name = srv->cmds[0]->name;

	//audit
	assert_int_equal(expected_state, actual_state);

	for(int i = 0; i < srv->ncmd; i++) {
		bool is_found = false;
		for(int j = 0; j < srv->ncmd; j++) {
			if( strcmp(srv->cmds[i]->name, expected_names[j]) == 0 )
				is_found = true;
		}
		assert_true(is_found);
	}

	//teardown
	ducq_srv_free(srv);
}






void cmd_dispatch_ducq_recv_receive_dispatch_params(void **state) {
	// arange
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_load_commands(srv);
	ducq_i *sender = ducq_new_mock();

	ducq_state expected_state = DUCQ_OK;

	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "mock_command_a route/\npayload");

	size_t size = BUFSIZ;
	expect_value    (_recv, ducq, sender);
	expect_not_value(_recv, ptr, NULL);
	expect_memory   (_recv, count, &size, sizeof(size_t));
	will_return     (_recv, DUCQ_OK);

	expect_any  (mock_command_a, srv);
	expect_any  (mock_command_a, ducq);
	expect_any  (mock_command_a, buffer);
	expect_any  (mock_command_a, size);
	will_return (mock_command_a, DUCQ_OK);

	expect_any  (_send, ducq);
	expect_any  (_send, buf);
	expect_any  (_send, count);
	will_return (_send, DUCQ_OK);

	// act
	ducq_state actual_state = ducq_srv_dispatch(srv, sender);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_srv_free(srv);
	ducq_free(sender);
}



void cmd_dispatch_recv_fails_return_rc(void **state) {
	// arange
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_load_commands(srv);
	ducq_i *sender = ducq_new_mock();

	ducq_state expected_state = DUCQ_EREAD;

	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "mock_command_a route/\npayload");
	expect_any (_recv, ducq);
	expect_any (_recv, ptr);
	expect_any (_recv, count);
	will_return(_recv, expected_state);

	// act
	ducq_state actual_state = ducq_srv_dispatch(srv, sender);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_srv_free(srv);
	ducq_free(sender);
}



void cmd_dispatch_recv_msg_buffer_size_minus_one_ok(void **state) {
	// arange
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_load_commands(srv);
	ducq_i *sender = ducq_new_mock();

	ducq_state expected_state = DUCQ_OK;

	snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "mock_command_a route\npayload");
	MOCK_CLIENT_RECV_BUFFER_LEN = BUFSIZ-1;

	expect_any (_recv, ducq);
	expect_any (_recv, ptr);
	expect_any (_recv, count);
	will_return(_recv, DUCQ_OK);

	expect_any (mock_command_a, srv);
	expect_any (mock_command_a, ducq);
	expect_any (mock_command_a, buffer);
	expect_any (mock_command_a, size);
	will_return(mock_command_a, DUCQ_OK);

	expect_any (_send, ducq);
	expect_any (_send, buf);
	expect_any (_send, count);
	will_return(_send, DUCQ_OK);

	// act
	ducq_state actual_state = ducq_srv_dispatch(srv, sender);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_srv_free(srv);
	ducq_free(sender);
}



void cmd_dispatch_recv_msg_exact_buffer_size_too_big(void **state) {
	// arange
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_load_commands(srv);
	ducq_i *sender = ducq_new_mock();

	ducq_state expected_state = DUCQ_EMSGSIZE;

	MOCK_CLIENT_RECV_BUFFER_LEN = BUFSIZ;

	expect_any (_recv, ducq);
	expect_any (_recv, ptr);
	expect_any (_recv, count);
	will_return(_recv, DUCQ_OK);

	// act
	ducq_state actual_state = ducq_srv_dispatch(srv, sender);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_srv_free(srv);
	ducq_free(sender);
}






void cmd_dispatch_command_a_receive_params(void **state) {
	// arange
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_load_commands(srv);
	ducq_i *sender = ducq_new_mock();

	ducq_state expected_state = DUCQ_OK;
	char expected_msg[] = "mock_command_a route/\npayload";
	
	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "%s", expected_msg);

	expect_any   (_recv, ducq);
	expect_any   (_recv, ptr);
	expect_any   (_recv, count);
	will_return  (_recv, DUCQ_OK);

	expect_value (mock_command_a, srv, srv);
	expect_value (mock_command_a, ducq, sender);
	expect_string(mock_command_a, buffer, expected_msg);
	expect_value (mock_command_a, size, strlen(expected_msg) +1);
	will_return  (mock_command_a, DUCQ_OK);

	expect_any   (_send, ducq);
	expect_any   (_send, buf);
	expect_any   (_send, count);
	will_return  (_send, DUCQ_OK);

	// act
	ducq_state actual_state = ducq_srv_dispatch(srv, sender);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_srv_free(srv);
	ducq_free(sender);
}


void cmd_dispatch_command_b_receive_params(void **state) {
	// arange
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_load_commands(srv);
	ducq_i *sender = ducq_new_mock();

	ducq_state expected_state = DUCQ_OK;
	char expected_msg[] = "mock_command_b route/\npayload";
	
	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "%s", expected_msg);

	expect_any   (_recv, ducq);
	expect_any   (_recv, ptr);
	expect_any   (_recv, count);
	will_return  (_recv, DUCQ_OK);

	expect_value (mock_command_b, srv, srv);
	expect_value (mock_command_b, ducq, sender);
	expect_string(mock_command_b, buffer, expected_msg);
	expect_value (mock_command_b, size, strlen(expected_msg) +1);
	will_return  (mock_command_b, DUCQ_OK);

	expect_any   (_send, ducq);
	expect_any   (_send, buf);
	expect_any   (_send, count);
	will_return  (_send, DUCQ_OK);

	// act
	ducq_state actual_state = ducq_srv_dispatch(srv, sender);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_srv_free(srv);
	ducq_free(sender);
}

void cmd_dispatch_command_receive_buffer_always_null_terminated(void **state) {
	// arange
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_load_commands(srv);
	ducq_i *sender = ducq_new_mock();
	
	ducq_state expected_state = DUCQ_OK;
	char expected_msg[] = "mock_command_a route/\npayload";
	size_t expected_size = strlen("mock_command_a route/\npayload") + 1;
	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "%s", expected_msg);
	void *ptr = MOCK_CLIENT_RECV_BUFFER + strlen(MOCK_CLIENT_RECV_BUFFER);
	memset(ptr, 'A', BUFSIZ - strlen(MOCK_CLIENT_RECV_BUFFER)); // all 'A', including '\0' after "payload:"

	expect_any  (_recv, ducq);
	expect_any  (_recv, ptr);
	expect_any  (_recv, count);
	will_return (_recv, DUCQ_OK);

	expect_value (mock_command_a, srv, srv);
	expect_value (mock_command_a, ducq, sender);
	expect_string(mock_command_a, buffer, expected_msg);
	expect_value (mock_command_a, size, expected_size);
	will_return  (mock_command_a, DUCQ_OK);

	expect_any   (_send, ducq);
	expect_any   (_send, buf);
	expect_any   (_send, count);
	will_return  (_send, DUCQ_OK);

	// act
	ducq_state actual_state = ducq_srv_dispatch(srv, sender);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_srv_free(srv);
	ducq_free(sender);
}




void cmd_dispatch_sender_receive_ack(void **state) {
	// arange
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_load_commands(srv);
	ducq_i *sender = ducq_new_mock();

	ducq_state expected_state = DUCQ_OK;
	char expected_ack_msg[] = "ACK *\n0\nok";
	size_t expected_count = strlen(expected_ack_msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "mock_command_a route/\npayload");


	expect_any   (_recv, ducq);
	expect_any   (_recv, ptr);
	expect_any   (_recv, count);
	will_return  (_recv, DUCQ_OK);

	expect_any   (mock_command_a, srv);
	expect_any   (mock_command_a, ducq);
	expect_any   (mock_command_a, buffer);
	expect_any   (mock_command_a, size);
	will_return  (mock_command_a, DUCQ_OK);

	expect_value (_send, ducq, sender);
	expect_string(_send, buf, expected_ack_msg);
	expect_memory(_send, count, &expected_count, sizeof(size_t));
	will_return  (_send, DUCQ_OK);

	// act
	ducq_state actual_state = ducq_srv_dispatch(srv, sender);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_srv_free(srv);
	ducq_free(sender);
}

void cmd_dispatch_sender_receive_nack_with_command_rc(void **state) {
	// arange
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_load_commands(srv);
	ducq_i *sender = ducq_new_mock();

	ducq_state expected_state = DUCQ_EMEMFAIL;
	char expected_nack_msg[100];
	size_t expected_count = snprintf(expected_nack_msg, 100, 
		"NACK *\n%d\n%s", DUCQ_EMEMFAIL, ducq_state_tostr(DUCQ_EMEMFAIL));

	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "mock_command_a route/\npayload");	

	expect_any   (_recv, ducq);
	expect_any   (_recv, ptr);
	expect_any   (_recv, count);
	will_return  (_recv, DUCQ_OK);

	expect_any   (mock_command_a, srv);
	expect_any   (mock_command_a, ducq);
	expect_any   (mock_command_a, buffer);
	expect_any   (mock_command_a, size);
	will_return  (mock_command_a, expected_state); // command's rc

	expect_value (_send, ducq, sender);
	expect_string(_send, buf, expected_nack_msg);
	expect_memory(_send, count, &expected_count, sizeof(size_t));
	will_return  (_send, DUCQ_OK);

	// act
	ducq_state actual_state = ducq_srv_dispatch(srv, sender);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_srv_free(srv);
	ducq_free(sender);
}



void cmd_dispatch_sender_receive_nack_if_command_unknown(void **state) {
	// arange
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_load_commands(srv);
	ducq_i *sender = ducq_new_mock();

	ducq_state expected_state = DUCQ_ENOCMD;
	char expected_nack_msg[100];
	size_t expected_count = snprintf(expected_nack_msg, 100, 
		"NACK *\n%d\n%s", expected_state, ducq_state_tostr(expected_state));
	
	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "UNKNOWN_COMMAND route/\npayload");

	expect_any   (_recv, ducq);
	expect_any   (_recv, ptr);
	expect_any   (_recv, count);
	will_return  (_recv, DUCQ_OK);

	expect_value (_send, ducq, sender);
	expect_string(_send, buf, expected_nack_msg);
	expect_memory(_send, count, &expected_count, sizeof(size_t));
	will_return  (_send, DUCQ_OK);

	// act
	ducq_state actual_state = ducq_srv_dispatch(srv, sender);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_srv_free(srv);
	ducq_free(sender);
}





void cmd_dispatch_returns_send_ack_state_over_command_state(void **state) {
	// arange
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_load_commands(srv);
	ducq_i *sender = ducq_new_mock();

	ducq_state expected_state         = DUCQ_EWRITE;
	ducq_state command_returned_state = DUCQ_EMEMFAIL;

	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "mock_command_a route/\npayload");

	expect_any  (_recv, ducq);
	expect_any  (_recv, ptr);
	expect_any  (_recv, count);
	will_return (_recv, DUCQ_OK);

	expect_any  (mock_command_a, srv);
	expect_any  (mock_command_a, ducq);
	expect_any  (mock_command_a, buffer);
	expect_any  (mock_command_a, size);
	will_return (mock_command_a, command_returned_state);

	expect_any  (_send, ducq);
	expect_any  (_send, buf);
	expect_any  (_send, count);
	will_return (_send, expected_state);

	// act
	ducq_state actual_state = ducq_srv_dispatch(srv, sender);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_srv_free(srv);
	ducq_free(sender);
}
