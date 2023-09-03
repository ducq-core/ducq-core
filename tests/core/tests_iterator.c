#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests_iterator.h"


#include "../src/ducq_reactor.h"
#include "mock_ducq_client.h"


int iterator_setup(void **state) {
	ducq_reactor *reactor = ducq_reactor_new();

	ducq_i *ducq1 = ducq_new_mock("A");
	ducq_i *ducq2 = ducq_new_mock("B");
	ducq_i *ducq3 = ducq_new_mock("C");
	
	ducq_reactor_add_client(reactor, 10, ducq1);
	ducq_reactor_add_client(reactor, 11, ducq2);
	ducq_reactor_add_client(reactor, 12, ducq3);
	
	ducq_reactor_subscribe(reactor, ducq1, "routeA");
	ducq_reactor_subscribe(reactor, ducq2, "routeB");
	ducq_reactor_subscribe(reactor, ducq3, "routeC");
	
	*state = reactor;

	return 0;
}

int iterator_teardown(void **state) {
	if(! *state) return 0;

	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	ducq_reactor *reactor = *state;
	ducq_reactor_free(reactor);

	return 0;
}





void reactor_unsubscribe_remove_first_with_iterator(void **state) {
	// arange
	ducq_reactor *reactor = *state;
	int  expected_count   = 2;
	char expected_id[]    = "A";
	char expected_route[] = "routeA";
	ducq_client_it *it    = NULL;
	ducq_i *ducq          = NULL;

	// mock
	expect_any(_close, ducq);
	will_return(_close, DUCQ_OK);

	// act
	char *actual_route = NULL;
	const char *actual_id    = NULL;

	it        = ducq_new_client_it(reactor);
	ducq      = ducq_next(it, &actual_route);
	actual_id = ducq_id(ducq);

	ducq_client_it_free(it);

	// audit 
	assert_string_equal(expected_route, actual_route);
	assert_string_equal(expected_id,    actual_id   );

	ducq_reactor_delete(reactor, ducq);

	it = ducq_new_client_it(reactor);
	int actual_count = 0;
	while( ducq = ducq_next(it, &actual_route) )
		actual_count++;
	assert_int_equal(expected_count, actual_count);

	// teardown
	ducq_client_it_free(it);
}



void reactor_unsubscribe_remove_middle_with_iterator(void **state) {
	// arange
	ducq_reactor *reactor = *state;
	int  expected_count   = 2;
	char expected_id[]    = "B";
	char expected_route[] = "routeB";
	ducq_client_it *it    = NULL;
	ducq_i *ducq          = NULL;

	// mock
	expect_any(_close, ducq);
	will_return(_close, DUCQ_OK);

	// act
	char *actual_route = NULL;
	const char *actual_id    = NULL;

	it        = ducq_new_client_it(reactor);
	ducq      = ducq_next(it, &actual_route); // A
	ducq      = ducq_next(it, &actual_route); // B
	actual_id = ducq_id(ducq);

	ducq_client_it_free(it);

	// audit 
	assert_string_equal(expected_route, actual_route);
	assert_string_equal(expected_id,    actual_id   );

	ducq_reactor_delete(reactor, ducq);

	it = ducq_new_client_it(reactor);
	int actual_count = 0;
	while( ducq = ducq_next(it, &actual_route) )
		actual_count++;
	assert_int_equal(expected_count, actual_count);

	// teardown
	ducq_client_it_free(it);
}

void reactor_unsubscribe_remove_last_with_iterator(void **state) {
	// arange
	ducq_reactor *reactor = *state;
	int  expected_count   = 2;
	char expected_id[]    = "C";
	char expected_route[] = "routeC";
	ducq_client_it *it    = NULL;
	ducq_i *ducq          = NULL;

	// mock
	expect_any(_close, ducq);
	will_return(_close, DUCQ_OK);

	// act
	char *actual_route = NULL;
	const char *actual_id    = NULL;

	it        = ducq_new_client_it(reactor);
	ducq      = ducq_next(it, &actual_route); // A
	ducq      = ducq_next(it, &actual_route); // B
	ducq      = ducq_next(it, &actual_route); // C
	actual_id = ducq_id(ducq);

	ducq_client_it_free(it);

	// audit 
	assert_string_equal(expected_route, actual_route);
	assert_string_equal(expected_id,    actual_id   );

	ducq_reactor_delete(reactor, ducq);

	it = ducq_new_client_it(reactor);
	int actual_count = 0;
	while( ducq = ducq_next(it, &actual_route) )
		actual_count++;
	assert_int_equal(expected_count, actual_count);

	// teardown
	ducq_client_it_free(it);
}



static
void _mock_accept(ducq_reactor *reactor, int fd, void *ctx) {
	(void) reactor;
	(void) fd;
	(void) ctx;
}
void reactor_unsubscribe_remove_last_with_a_server_iterator(void **state) {
	// arange
	ducq_reactor *reactor = *state;
	int  expected_count   = 2;
	char expected_id[]    = "C";
	char expected_route[] = "routeC";
	ducq_client_it *it    = NULL;
	ducq_i *ducq          = NULL;

	// mock
	expect_any(_close, ducq);
	will_return(_close, DUCQ_OK);

	// act
	ducq_reactor_add_server(reactor, 20, _mock_accept, NULL); 
	char *actual_route = NULL;
	const char *actual_id    = NULL;

	it        = ducq_new_client_it(reactor);
	ducq      = ducq_next(it, &actual_route); // A
	ducq      = ducq_next(it, &actual_route); // B
	ducq      = ducq_next(it, &actual_route); // C
	actual_id = ducq_id(ducq);

	ducq_client_it_free(it);

	// audit 
	assert_string_equal(expected_route, actual_route);
	assert_string_equal(expected_id,    actual_id   );

	ducq_reactor_delete(reactor, ducq);

	it = ducq_new_client_it(reactor);
	int actual_count = 0;
	while( ducq = ducq_next(it, &actual_route) )
		actual_count++;
	assert_int_equal(expected_count, actual_count);

	// teardown
	ducq_client_it_free(it);
}

