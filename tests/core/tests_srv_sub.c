#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests_srv_sub.h"


#include "../src/ducq_srv.h"
#include "mock_ducq_client.h"


//
//
// M I G R A G T I O N
//
//
// void cmd_dispatch_ducq_recv_receive_dispatch_params(void **state) {
// 	// arange
// 	ducq_dispatcher *srv = ducq_dispatcher_new();
// 	ducq_dispatcher_load_commands(srv);
// 	ducq_i *sender = ducq_new_mock(NULL);
// 
// 	ducq_state expected_state = DUCQ_OK;
// 
// 	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "mock_command_a route/\npayload");
// 
// 	size_t size = BUFSIZ;
// 	expect_value    (_recv, ducq, sender);
// 	expect_not_value(_recv, ptr, NULL);
// 	expect_memory   (_recv, count, &size, sizeof(size_t));
// 	will_return     (_recv, DUCQ_OK);
// 
// 	expect_any (_timeout, ducq);
// 	expect_any (_timeout, timeout);
// 	will_return(_timeout, DUCQ_OK);
// 
// 	expect_any  (mock_command_a, srv);
// 	expect_any  (mock_command_a, ducq);
// 	expect_any  (mock_command_a, buffer);
// 	expect_any  (mock_command_a, size);
// 	will_return (mock_command_a, DUCQ_OK);
// 
// 	// act
// 	ducq_state actual_state = ducq_dispatcher_dispatch(srv, sender);
// 
// 	// audit
// 	assert_int_equal(expected_state, actual_state);
// 
// 	// teardown
// 	ducq_dispatcher_free(srv);
// 	ducq_free(sender);
// }
// 
// void cmd_dispatch_recv_fails_return_rc(void **state) {
// 	// arange
// 	ducq_dispatcher *srv = ducq_dispatcher_new();
// 	ducq_dispatcher_load_commands(srv);
// 	ducq_i *sender = ducq_new_mock(NULL);
// 
// 	ducq_state expected_state = DUCQ_EREAD;
// 
// 	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "mock_command_a route/\npayload");
// 	expect_any (_recv, ducq);
// 	expect_any (_recv, ptr);
// 	expect_any (_recv, count);
// 	will_return(_recv, expected_state);
// 
// 	expect_any (_timeout, ducq);
// 	expect_any (_timeout, timeout);
// 	will_return(_timeout, DUCQ_OK);
// 
// 	// act
// 	ducq_state actual_state = ducq_dispatcher_dispatch(srv, sender);
// 
// 	// audit
// 	assert_int_equal(expected_state, actual_state);
// 
// 	// teardown
// 	ducq_dispatcher_free(srv);
// 	ducq_free(sender);
// }
// 
// 
// void cmd_dispatch_recv_msg_buffer_size_minus_one_ok(void **state) {
// 	// arange
// 	ducq_dispatcher *srv = ducq_dispatcher_new();
// 	ducq_dispatcher_load_commands(srv);
// 	ducq_i *sender = ducq_new_mock(NULL);
// 
// 	ducq_state expected_state = DUCQ_OK;
// 
// 	snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "mock_command_a route\npayload");
// 	MOCK_CLIENT_RECV_BUFFER_LEN = BUFSIZ-1;
// 
// 	expect_any (_recv, ducq);
// 	expect_any (_recv, ptr);
// 	expect_any (_recv, count);
// 	will_return(_recv, DUCQ_OK);
// 
// 	expect_any (_timeout, ducq);
// 	expect_any (_timeout, timeout);
// 	will_return(_timeout, DUCQ_OK);
// 
// 	expect_any (mock_command_a, srv);
// 	expect_any (mock_command_a, ducq);
// 	expect_any (mock_command_a, buffer);
// 	expect_any (mock_command_a, size);
// 	will_return(mock_command_a, DUCQ_OK);
// 
// 
// 	// act
// 	ducq_state actual_state = ducq_dispatcher_dispatch(srv, sender);
// 
// 	// audit
// 	assert_int_equal(expected_state, actual_state);
// 
// 	// teardown
// 	ducq_dispatcher_free(srv);
// 	ducq_free(sender);
// }
// 
// 
// 
// void cmd_dispatch_recv_msg_exact_buffer_size_too_big(void **state) {
// 	// arange
// 	ducq_dispatcher *srv = ducq_dispatcher_new();
// 	ducq_dispatcher_load_commands(srv);
// 	ducq_i *sender = ducq_new_mock(NULL);
// 
// 	ducq_state expected_state = DUCQ_EMSGSIZE;
// 
// 	MOCK_CLIENT_RECV_BUFFER_LEN = BUFSIZ;
// 
// 	expect_any (_recv, ducq);
// 	expect_any (_recv, ptr);
// 	expect_any (_recv, count);
// 	will_return(_recv, DUCQ_OK);
// 
// 	expect_any (_timeout, ducq);
// 	expect_any (_timeout, timeout);
// 	will_return(_timeout, DUCQ_OK);
// 
// 	// act
// 	ducq_state actual_state = ducq_dispatcher_dispatch(srv, sender);
// 
// 	// audit
// 	assert_int_equal(expected_state, actual_state);
// 
// 	// teardown
// 	ducq_dispatcher_free(srv);
// 	ducq_free(sender);
// }
// 
// 
// 
// void cmd_dispatch_command_a_receive_params(void **state) {
// 	// arange
// 	ducq_dispatcher *srv = ducq_dispatcher_new();
// 	ducq_dispatcher_load_commands(srv);
// 	ducq_i *sender = ducq_new_mock(NULL);
// 
// 	ducq_state expected_state = DUCQ_OK;
// 	char expected_msg[] = "mock_command_a route/\npayload";
// 	
// 	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "%s", expected_msg);
// 
// 	expect_any   (_recv, ducq);
// 	expect_any   (_recv, ptr);
// 	expect_any   (_recv, count);
// 	will_return  (_recv, DUCQ_OK);
// 
// 	expect_any (_timeout, ducq);
// 	expect_any (_timeout, timeout);
// 	will_return(_timeout, DUCQ_OK);
// 
// 	expect_value (mock_command_a, srv, srv);
// 	expect_value (mock_command_a, ducq, sender);
// 	expect_string(mock_command_a, buffer, expected_msg);
// 	expect_value (mock_command_a, size, strlen(expected_msg) +1);
// 	will_return  (mock_command_a, DUCQ_OK);
// 
// 	// act
// 	ducq_state actual_state = ducq_dispatcher_dispatch(srv, sender);
// 
// 	// audit
// 	assert_int_equal(expected_state, actual_state);
// 
// 	// teardown
// 	ducq_dispatcher_free(srv);
// 	ducq_free(sender);
// }
// 
// 
// void cmd_dispatch_command_b_receive_params(void **state) {
// 	// arange
// 	ducq_dispatcher *srv = ducq_dispatcher_new();
// 	ducq_dispatcher_load_commands(srv);
// 	ducq_i *sender = ducq_new_mock(NULL);
// 
// 	ducq_state expected_state = DUCQ_OK;
// 	char expected_msg[] = "mock_command_b route/\npayload";
// 	
// 	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "%s", expected_msg);
// 
// 	expect_any   (_recv, ducq);
// 	expect_any   (_recv, ptr);
// 	expect_any   (_recv, count);
// 	will_return  (_recv, DUCQ_OK);
// 
// 	expect_any (_timeout, ducq);
// 	expect_any (_timeout, timeout);
// 	will_return(_timeout, DUCQ_OK);
// 
// 	expect_value (mock_command_b, srv, srv);
// 	expect_value (mock_command_b, ducq, sender);
// 	expect_string(mock_command_b, buffer, expected_msg);
// 	expect_value (mock_command_b, size, strlen(expected_msg) +1);
// 	will_return  (mock_command_b, DUCQ_OK);
// 
// 	// act
// 	ducq_state actual_state = ducq_dispatcher_dispatch(srv, sender);
// 
// 	// audit
// 	assert_int_equal(expected_state, actual_state);
// 
// 	// teardown
// 	ducq_dispatcher_free(srv);
// 	ducq_free(sender);
// }
// 
// void cmd_dispatch_command_receive_buffer_always_null_terminated(void **state) {
// 	// arange
// 	ducq_dispatcher *srv = ducq_dispatcher_new();
// 	ducq_dispatcher_load_commands(srv);
// 	ducq_i *sender = ducq_new_mock(NULL);
// 	
// 	ducq_state expected_state = DUCQ_OK;
// 	char expected_msg[] = "mock_command_a route/\npayload";
// 	size_t expected_size = strlen("mock_command_a route/\npayload") + 1;
// 	MOCK_CLIENT_RECV_BUFFER_LEN = snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "%s", expected_msg);
// 	void *ptr = MOCK_CLIENT_RECV_BUFFER + strlen(MOCK_CLIENT_RECV_BUFFER);
// 	memset(ptr, 'A', BUFSIZ - strlen(MOCK_CLIENT_RECV_BUFFER)); // all 'A', including '\0' after "payload:"
// 
// 	expect_any  (_recv, ducq);
// 	expect_any  (_recv, ptr);
// 	expect_any  (_recv, count);
// 	will_return (_recv, DUCQ_OK);
// 
// 	expect_any (_timeout, ducq);
// 	expect_any (_timeout, timeout);
// 	will_return(_timeout, DUCQ_OK);
// 
// 	expect_value (mock_command_a, srv, srv);
// 	expect_value (mock_command_a, ducq, sender);
// 	expect_string(mock_command_a, buffer, expected_msg);
// 	expect_value (mock_command_a, size, expected_size);
// 	will_return  (mock_command_a, DUCQ_OK);
// 
// 	// act
// 	ducq_state actual_state = ducq_dispatcher_dispatch(srv, sender);
// 
// 	// audit
// 	assert_int_equal(expected_state, actual_state);
// 
// 	// teardown
// 	ducq_dispatcher_free(srv);
// 	ducq_free(sender);
// }
// 
// 
// 
// 
// 
/// *********************************************************************


int sub_setup(void **state) {
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_add(srv, ducq_new_mock("A"), NULL);
	ducq_srv_add(srv, ducq_new_mock("B"), NULL);
	ducq_srv_add(srv, ducq_new_mock("C"), NULL);
	
	*state = srv;

	return 0;
}

int sub_teardown(void **state) {
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	ducq_srv *srv = *state;
	ducq_srv_free(srv);

	return 0;
}





void srv_dtor_free_its_sub(void **state) {
	// testing leaks in setup and teardown
}





ducq_loop_t _remove_first(ducq_i *ducq, char *route, void *ctx) {
	return ( strcmp(ducq_id(ducq), "A") == 0 )
		? ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK )
		:   DUCQ_LOOP_CONTINUE;
}
ducq_loop_t _check_first_removed(ducq_i *ducq, char *route, void *ctx) {
	assert_true( strcmp(ducq_id(ducq), "A") != 0 );
	assert_true( strcmp(ducq_id(ducq), "B") == 0
	          || strcmp(ducq_id(ducq), "C") == 0 );

	int * count = (int*)ctx;
	count++;	return DUCQ_LOOP_CONTINUE;
}
void srv_unsubscribe_remove_first_sub(void **state) {
	// arange
	ducq_srv *srv = *state;
	ducq_loop_t expected = ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK );
	int expected_count = 2;

	expect_any(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	// act && audit
	ducq_loop_t actual = ducq_srv_loop(srv, _remove_first, NULL);
	assert_int_equal(expected, actual);

	int actual_count = 0;
	ducq_srv_loop(srv, _check_first_removed, &actual_count);
	assert_int_equal(expected_count, actual_count);
}




ducq_loop_t _remove_middle(ducq_i *ducq, char *route, void *ctx) {
	return ( strcmp(ducq_id(ducq), "/") == 0 )
		? ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK )
		:   DUCQ_LOOP_CONTINUE;
}
ducq_loop_t _check_middle_removed(ducq_i *ducq, char *route, void *ctx) {
	assert_true( strcmp(ducq_id(ducq), "B") != 0 );
	assert_true( strcmp(ducq_id(ducq), "A") == 0
	          || strcmp(ducq_id(ducq), "C") == 0 );

	int * count = (int*)ctx;
	count++;	return DUCQ_LOOP_CONTINUE;
}
void srv_unsubscribe_remove_middle_sub(void **state) {
	// arange
	ducq_srv *srv = *state;
	ducq_loop_t expected = ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK );
	int expected_count = 2;

	expect_any(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	// act && audit
	ducq_loop_t actual = ducq_srv_loop(srv, _remove_middle, NULL);
	assert_int_equal(expected, actual);

	int actual_count = 0;
	ducq_srv_loop(srv, _check_middle_removed, &actual_count);
	assert_int_equal(expected_count, actual_count);
}



ducq_loop_t _remove_last(ducq_i *ducq, char *route, void *ctx) {
	return ( strcmp(ducq_id(ducq), "C") == 0 )
		? ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK )
		:   DUCQ_LOOP_CONTINUE;
}
ducq_loop_t _check_last_removed(ducq_i *ducq, char *route, void *ctx) {
	assert_true( strcmp(ducq_id(ducq), "C") != 0 );
	assert_true( strcmp(ducq_id(ducq), "A") == 0
	          || strcmp(ducq_id(ducq), "B") == 0 );

	int * count = (int*)ctx;
	count++;	return DUCQ_LOOP_CONTINUE;
}
void srv_unsubscribe_remove_last_sub(void **state) {
	// arange
	ducq_srv *srv = *state;
	ducq_loop_t expected = ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK );
	int expected_count = 2;

	expect_any(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	// act && audit
	ducq_loop_t actual = ducq_srv_loop(srv, _remove_last, NULL);
	assert_int_equal(expected, actual);

	int actual_count = 0;
	ducq_srv_loop(srv, _check_last_removed, &actual_count);
	assert_int_equal(expected_count, actual_count);
}



ducq_loop_t _remove_inexistant(ducq_i *ducq, char *route, void *ctx) {
	return ( strcmp(ducq_id(ducq), "Z") == 0 )
		? ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK )
		:   DUCQ_LOOP_CONTINUE;
}
ducq_loop_t _check_inexistant_removed(ducq_i *ducq, char *route, void *ctx) {
	assert_true( strcmp(ducq_id(ducq), "A") == 0
	          || strcmp(ducq_id(ducq), "B") == 0
	          || strcmp(ducq_id(ducq), "C") == 0 );

	int * count = (int*)ctx;
	count++;	return DUCQ_LOOP_CONTINUE;
}
void srv_unsubscribe_remove_inexistant_sub(void **state) {
	// arange
	ducq_srv *srv = *state;
	ducq_loop_t expected = DUCQ_LOOP_CONTINUE;
	int expected_count = 3;

	expect_any(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	// act && audit
	ducq_loop_t actual = ducq_srv_loop(srv, _remove_inexistant, NULL);
	assert_int_equal(expected, actual);

	int actual_count = 0;
	ducq_srv_loop(srv, _check_inexistant_removed, &actual_count);
	assert_int_equal(expected_count, actual_count);
}



void srv_unsubscribe_ignore_close_connection_error(void **state) {
	// arange
	ducq_srv *srv = *state;
	ducq_loop_t expected = ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK );
	int expected_count = 2;

	expect_any(_close, ducq);
	will_return_always(_close, DUCQ_ECLOSE);

	// act && audit
	ducq_loop_t actual = ducq_srv_loop(srv, _remove_first, NULL);
	assert_int_equal(expected, actual);

	int actual_count = 0;
	ducq_srv_loop(srv, _check_first_removed, &actual_count);
	assert_int_equal(expected_count, actual_count);
}

