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
#include "../src/ducq_srv_int.h"
#include "mock_ducq_client.h"




int sub_setup(void **state) {
	ducq_i *ducq_a = ducq_new_mock();
	ducq_i *ducq_b = ducq_new_mock();
	ducq_i *ducq_c = ducq_new_mock();

	ducq_sub *sub_c = malloc(sizeof(ducq_sub));
	sub_c->ducq  = ducq_c;
	sub_c->id    = ducq_id(ducq_c);
	sub_c->route = NULL;
	sub_c->next  = NULL;
	
	ducq_sub *sub_b = malloc(sizeof(ducq_sub));
	sub_b->ducq  = ducq_b;
	sub_b->id    = ducq_id(ducq_b);
	sub_b->route = NULL;
	sub_b->next  = sub_c;

	ducq_sub *sub_a = malloc(sizeof(ducq_sub));
	sub_a->ducq  = ducq_a;
	sub_a->id    = ducq_id(ducq_a);
	sub_a->route = NULL;
	sub_a->next  = sub_b;

	ducq_srv *srv = ducq_srv_new();
	srv->subs = sub_a;
	
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


void srv_free_sub_no_leak(void **state) {
	expect_any(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	ducq_i *ducq = ducq_new_mock();

	ducq_sub *sub = malloc(sizeof(ducq_sub));
	sub->ducq  = ducq;
	sub->id    = ducq_id(ducq);
	sub->route = strdup("route");
	sub->next  = NULL;

	ducq_sub_free(sub);
}



void srv_dtor_free_its_sub(void **state) {
	// testing leaks in setup and teardown
}


void srv_unsubscribe_remove_first_sub(void **state) {
	// arange
	ducq_srv *srv = *state;

	ducq_sub *first          = srv->subs;
	ducq_sub *expected_first = first->next;
	ducq_sub *expected_last  = expected_first->next;
	bool expected_has_unsubscribed = true;

	ducq_i * ducq = first->ducq;

	expect_value(_close, ducq, ducq);
	will_return_always(_close, DUCQ_OK);

	// act
	bool actual_has_unsubscribed = ducq_srv_unsubscribe(srv, ducq);

	// audit
	assert_int_equal(expected_has_unsubscribed, actual_has_unsubscribed);

	ducq_sub *actual_first = srv->subs;
	ducq_sub *actual_last  = srv->subs->next;
	assert_ptr_equal(expected_first, actual_first);
	assert_ptr_equal(expected_last,  actual_last);
	assert_null(actual_last->next);
}

void srv_unsubscribe_remove_middle_sub(void **state) {
	// arange
	ducq_srv *srv = *state;

	ducq_sub *expected_first = srv->subs;
	ducq_sub *second         = expected_first->next;
	ducq_sub *expected_last  = second->next;
	bool expected_has_unsubscribed = true;

	ducq_i * ducq = second->ducq;

	expect_value(_close, ducq, ducq);
	will_return_always(_close, DUCQ_OK);

	// act
	bool actual_has_unsubscribed = ducq_srv_unsubscribe(srv, ducq);

	// audit
	assert_int_equal(expected_has_unsubscribed, actual_has_unsubscribed);

	ducq_sub *actual_first = srv->subs;
	ducq_sub *actual_last  = srv->subs->next;
	assert_ptr_equal(expected_first, actual_first);
	assert_ptr_equal(expected_last,  actual_last);
	assert_null(actual_last->next);
}

void srv_unsubscribe_remove_last_sub(void **state) {
	// arange
	ducq_srv *srv = *state;

	ducq_sub *expected_first = srv->subs;
	ducq_sub *expected_last  = expected_first->next;
	ducq_sub *third          = expected_last->next;
	bool expected_has_unsubscribed = true;

	ducq_i * ducq = third->ducq;

	expect_value(_close, ducq, ducq);
	will_return_always(_close, DUCQ_OK);

	// act
	bool actual_has_unsubscribed = ducq_srv_unsubscribe(srv, ducq);

	// audit
	assert_int_equal(expected_has_unsubscribed, actual_has_unsubscribed);

	ducq_sub *actual_first = srv->subs;
	ducq_sub *actual_last  = srv->subs->next;
	assert_ptr_equal(expected_first, actual_first);
	assert_ptr_equal(expected_last,  actual_last);
	assert_null(actual_last->next);
}

void srv_unsubscribe_inexistant_return_false(void **state) {
	// arange
	ducq_srv *srv = *state;

	ducq_sub *expected_first  = srv->subs;
	ducq_sub *expected_second = expected_first->next;
	ducq_sub *expected_last   = expected_second->next;
	bool expected_has_unsubscribed = false;

	ducq_i * ducq = ducq_new_mock();

	// act
	bool actual_has_unsubscribed = ducq_srv_unsubscribe(srv, ducq);

	// audit
	assert_int_equal(expected_has_unsubscribed, actual_has_unsubscribed);

	ducq_sub *actual_first  = srv->subs;
	ducq_sub *actual_second = actual_first->next;
	ducq_sub *actual_last   = actual_second->next;

	assert_ptr_equal(expected_first,  actual_first);
	assert_ptr_equal(expected_second, actual_second);
	assert_ptr_equal(expected_last,   actual_last);
	assert_null(actual_last->next);

	// teardown
	ducq_free(ducq);
}


void srv_unsubscribe_ignore_close_connection_error(void **state) {
	// arange
	ducq_srv *srv = *state;
	ducq_sub *first          = srv->subs;
	ducq_sub *expected_first = first->next;
	ducq_sub *expected_last  = expected_first->next;
	bool expected_has_unsubscribed = true;

	ducq_i * ducq = first->ducq;
	
	expect_value(_close, ducq, ducq);
	will_return_always(_close, DUCQ_ECLOSE);

	// act
	bool actual_has_unsubscribed = ducq_srv_unsubscribe(srv, ducq);

	// audit
	assert_int_equal(expected_has_unsubscribed, actual_has_unsubscribed);

	ducq_sub *actual_first = srv->subs;
	ducq_sub *actual_last  = srv->subs->next;
	assert_ptr_equal(expected_first, actual_first);
	assert_ptr_equal(expected_last,  actual_last);
	assert_null(actual_last->next);
}