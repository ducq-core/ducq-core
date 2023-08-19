#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests_connections.h"


#include "../src/ducq_reactor.h"
#include "mock_ducq_client.h"


int connections_setup(void **state) {
	ducq_reactor *reactor = ducq_reactor_new();

	ducq_i *ducq1 = ducq_new_mock("A");
	ducq_i *ducq2 = ducq_new_mock("B");
	ducq_i *ducq3 = ducq_new_mock("C");
	
	ducq_reactor_add_client(reactor, 10, ducq1);
	ducq_reactor_add_client(reactor, 11, ducq2);
	ducq_reactor_add_client(reactor, 12, ducq3);
	
	ducq_reactor_subscribe(reactor, ducq1, "route");
	ducq_reactor_subscribe(reactor, ducq2, "route");
	ducq_reactor_subscribe(reactor, ducq3, "route");
	
	*state = reactor;

	return 0;
}

int connections_teardown(void **state) {
	if(! *state) return 0;

	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	ducq_reactor *reactor = *state;
	ducq_reactor_free(reactor);

	return 0;
}





void reactor_dtor_free_its_connection(void **state) {
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

	int *count = (int*)ctx;
	(*count)++;
	return DUCQ_LOOP_CONTINUE;
}
void reactor_unsubscribe_remove_first_connection(void **state) {
	// arange
	ducq_reactor *reactor = *state;
	ducq_loop_t expected = ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK );
	int expected_count = 2;

	expect_any(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	// act
	ducq_loop_t actual = ducq_reactor_loop(reactor, _remove_first, NULL);
	assert_int_equal(expected, actual);

	// audit 
	int actual_count = 0;
	ducq_reactor_loop(reactor, _check_first_removed, &actual_count);
	assert_int_equal(expected_count, actual_count);
}




ducq_loop_t _remove_middle(ducq_i *ducq, char *route, void *ctx) {
	return ( strcmp(ducq_id(ducq), "B") == 0 )
		? ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK )
		:   DUCQ_LOOP_CONTINUE;
}
ducq_loop_t _check_middle_removed(ducq_i *ducq, char *route, void *ctx) {
	assert_true( strcmp(ducq_id(ducq), "B") != 0 );
	assert_true( strcmp(ducq_id(ducq), "A") == 0
	          || strcmp(ducq_id(ducq), "C") == 0 );

	int *count = (int*)ctx;
	(*count)++;
	return DUCQ_LOOP_CONTINUE;
}
void reactor_unsubscribe_remove_middle_connection(void **state) {
	// arange
	ducq_reactor *reactor = *state;
	ducq_loop_t expected = ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK );
	int expected_count = 2;

	expect_any(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	// act
	ducq_loop_t actual = ducq_reactor_loop(reactor, _remove_middle, NULL);
	assert_int_equal(expected, actual);

	// audit
	int actual_count = 0;
	ducq_reactor_loop(reactor, _check_middle_removed, &actual_count);
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

	int *count = (int*)ctx;
	(*count)++;
	return DUCQ_LOOP_CONTINUE;
}
void reactor_unsubscribe_remove_last_connection(void **state) {
	// arange
	ducq_reactor *reactor = *state;
	ducq_loop_t expected = ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK );
	int expected_count = 2;

	expect_any(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	// act
	ducq_loop_t actual = ducq_reactor_loop(reactor, _remove_last, NULL);
	assert_int_equal(expected, actual);

	// audit
	int actual_count = 0;
	ducq_reactor_loop(reactor, _check_last_removed, &actual_count);
	assert_int_equal(expected_count, actual_count);
}

static
void _mock_accept(ducq_reactor *reactor, int fd, void *ctx) {
	(void) reactor;
	(void) fd;
	(void) ctx;
}

void reactor_unsubscribe_remove_last_with_a_server(void **state) {
	// arange
	ducq_reactor *reactor = *state;
	ducq_loop_t expected = ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK );
	int expected_count = 2;

	expect_any(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	// act
	ducq_reactor_add_server(reactor, 20, _mock_accept, NULL); 
	ducq_loop_t actual = ducq_reactor_loop(reactor, _remove_last, NULL);
	assert_int_equal(expected, actual);

	// audit
	int actual_count = 0;
	ducq_reactor_loop(reactor, _check_last_removed, &actual_count);
	assert_int_equal(expected_count, actual_count);
}




ducq_loop_t _remove_all(ducq_i *ducq, char *route, void *ctx) {
	return DUCQ_LOOP_DELETE | DUCQ_LOOP_CONTINUE;
}
ducq_loop_t _check_all_removed(ducq_i *ducq, char *route, void *ctx) {
	int *count = (int*)ctx;
	(*count)++;
	return DUCQ_LOOP_CONTINUE;
}
void reactor_unsubscribe_remove_all_connection(void **state) {
	// arange
	ducq_reactor *reactor = *state;
	ducq_loop_t expected = ( DUCQ_LOOP_DELETE | DUCQ_LOOP_CONTINUE );
	int expected_count = 0;

	expect_any_count(_close, ducq, 3);
	will_return_count(_close, DUCQ_OK, 3);

	// act
	ducq_loop_t actual = ducq_reactor_loop(reactor, _remove_all, NULL);
	assert_int_equal(expected, actual);

	// audit
	int actual_count = 0;
	ducq_reactor_loop(reactor, _check_all_removed, &actual_count);
	assert_int_equal(expected_count, actual_count);

	// teardown
	ducq_reactor_free(reactor);
	*state = NULL;
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

	int *count = (int*)ctx;
	(*count)++;
	return DUCQ_LOOP_CONTINUE;
}
void reactor_unsubscribe_remove_inexistant_connection(void **state) {
	// arange
	ducq_reactor *reactor = *state;
	ducq_loop_t expected = DUCQ_LOOP_CONTINUE;
	int expected_count = 3;

	// act
	ducq_loop_t actual = ducq_reactor_loop(reactor, _remove_inexistant, NULL);
	assert_int_equal(expected, actual);
	
	// audit
	int actual_count = 0;
	ducq_reactor_loop(reactor, _check_inexistant_removed, &actual_count);
	assert_int_equal(expected_count, actual_count);
}



void reactor_unsubscribe_ignore_close_connection_error(void **state) {
	// arange
	ducq_reactor *reactor = *state;
	ducq_loop_t expected = ( DUCQ_LOOP_DELETE | DUCQ_LOOP_BREAK );
	int expected_count = 2;

	expect_any(_close, ducq);
	will_return_always(_close, DUCQ_ECLOSE);

	// act
	ducq_loop_t actual = ducq_reactor_loop(reactor, _remove_first, NULL);
	assert_int_equal(expected, actual);

	// audit
	int actual_count = 0;
	ducq_reactor_loop(reactor, _check_first_removed, &actual_count);
	assert_int_equal(expected_count, actual_count);
}

