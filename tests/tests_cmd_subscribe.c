#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdlib.h>


#include "mock_ducq_client.h"
#include "unit_tests_cmd.h"

#include "../src/ducq.h"
#include "../src/ducq_srv.h"
#include "../src/ducq_srv_int.h"


int subscribe_tests_setup(void **state) {
	*state = fix_new("subscribe");
	return *state == NULL;
}
int subscribe_tests_teardown(void **state) {
	struct fixture *fix = *state;
return fix_free(fix);
}



void subscribe_msg_invalide_if_cant_parse_route(void **state) {
//arrange
	command_f subscribe = get_command(state);
	
	ducq_state expected_state = DUCQ_EMSGINV;

	ducq_srv *srv = ducq_srv_new();
	ducq_i *subscriber = ducq_new_mock();
	char buffer[] = "subscriberoute\npayload";
	size_t size = sizeof(buffer);

	// act
	ducq_state actual_state = subscribe(srv, subscriber, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	ducq_srv_free(srv);
	ducq_free(subscriber); // err, not freed
}


void subscribe_add_subscriber_to_srv_subs(void **state) {
//arrange
	command_f subscribe = get_command(state);

	ducq_srv *srv = ducq_srv_new();
	ducq_i *subscriber = ducq_new_mock();
	char buffer[] = "subscribe ROUTE\npayload";
	size_t size = sizeof(buffer);

	will_return(_copy, subscriber);

	ducq_state expected_state = DUCQ_OK;
	char  expected_route[] = "ROUTE";
	const char *expected_id = ducq_id(subscriber);

	// act
	ducq_state actual_state = subscribe(srv, subscriber, buffer, size);
	ducq_sub *sub = srv->subs;
	char *actual_route = sub->route;
	const char *actual_id = sub->id;

	//audit
	assert_int_equal(expected_state, actual_state);
	assert_ptr_equal(sub->ducq, subscriber);
	assert_string_equal(actual_route, expected_route);
	assert_string_equal(expected_id, actual_id);

	//teardown
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
}



void subscribe_add_second_subscriber_to_srv_subs(void **state) {
//arrange
	command_f subscribe = get_command(state);

	ducq_srv *srv = ducq_srv_new();
	ducq_i *subscriber1 = ducq_new_mock();
	ducq_i *subscriber2 = ducq_new_mock();
	char buffer[] = "subscribe ROUTE\npayload";
	size_t size = sizeof(buffer);

	will_return(_copy, subscriber1);
	will_return(_copy, subscriber2);

	ducq_state expected_state = DUCQ_OK;
	char  expected_route[] = "ROUTE";
	const char *expected_id1 = ducq_id(subscriber1);
	const char *expected_id2 = ducq_id(subscriber2);

	// act
	ducq_state actual_state1 = subscribe(srv, subscriber1, buffer, size);
	ducq_state actual_state12 = subscribe(srv, subscriber2, buffer, size);
	ducq_sub *sub2 = srv->subs;
	ducq_sub *sub1 = sub2->next;
	char *actual_route1 = sub1->route;
	char *actual_route2 = sub2->route;
	const char *actual_id1 = sub1->id;
	const char *actual_id2 = sub2->id;

	//audit
	assert_int_equal(expected_state, actual_state1);
	assert_ptr_equal(sub1->ducq, subscriber1);
	assert_string_equal(actual_route1, expected_route);
	assert_string_equal(actual_route2, expected_route);
	assert_string_equal(expected_id1, actual_id1);
	assert_string_equal(expected_id2, actual_id2);

	//teardown
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
}



void subscribe_mem_error_cleans_up(void **state) {
//arrange
	command_f subscribe = get_command(state);

	ducq_srv *srv = ducq_srv_new();
	ducq_i *subscriber = ducq_new_mock();
	char buffer[] = "subscribe ROUTE\npayload";
	size_t size = sizeof(buffer);

	will_return(_copy, NULL);

	ducq_state expected_state = DUCQ_EMEMFAIL;

	// act
	ducq_state actual_state = subscribe(srv, subscriber, buffer, size);
	
	//audit
	assert_int_equal(expected_state, actual_state);
	assert_null(srv->subs);

	//teardown
	ducq_srv_free(srv);
	ducq_free(subscriber); // err, not freed
}