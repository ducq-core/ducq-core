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
#include "../src/ducq_srv.h"
#include "../src/ducq_srv_int.h"




#if DUCQ_MSGSZ != 256
#error "expect DUCQ_MSGSZ == 256 for list_subscriptions tests"
#endif



int list_subscriptions_tests_setup(void **state) {
	*state = fix_new("list_subscriptions");
	return *state == NULL;
}
int list_subscriptions_tests_teardown(void **state) {
	struct fixture *fix = *state;
	return fix_free(fix);
}


void list_subscriptions_list_all_subscribers_id(void **state) {
	//arrange
	command_f list_subscriptions = get_command(state);
	

	ducq_srv *srv = ducq_srv_new();

	ducq_sub *sub1 = malloc(sizeof(ducq_sub));
	ducq_sub *sub2 = malloc(sizeof(ducq_sub));
	ducq_sub *sub3 = malloc(sizeof(ducq_sub));
	sub1->ducq     = ducq_new_mock("sub_id_1");
	sub2->ducq     = ducq_new_mock("sub_id_2");
	sub3->ducq     = ducq_new_mock("sub_id_3");
	sub1->route    = strdup("route_1");
	sub2->route    = strdup("route_2");
	sub3->route    = strdup("route_3");
	sub1->id       = ducq_id(sub1->ducq);
	sub2->id       = ducq_id(sub2->ducq);
	sub3->id       = ducq_id(sub3->ducq);
	sub1->next     = NULL;
	sub2->next     = sub1;
	sub3->next     = sub2;

	srv->subs = sub3;


	ducq_i *emitter = ducq_new_mock(NULL);
	char request[] = "list_subscriptions *\n";
	size_t req_size = sizeof(request);
	
	char expected_reply[] =
		"sub_id_3,route_3\n"
		"sub_id_2,route_2\n"
		"sub_id_1,route_1\n"
	;
	ducq_state expected_state = DUCQ_OK;

	expect_value(_send, ducq, emitter);
	expect_string(_send, buf, expected_reply);
	expect_value(_send, *count, strlen(expected_reply));
	will_return(_send, DUCQ_OK);
	
	expect_value(_close, ducq, emitter);
	will_return(_close, DUCQ_OK);

	// // act
		ducq_state actual_state = list_subscriptions(srv, emitter, request, req_size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
	ducq_free(emitter);
}





void list_subscriptions_split_message_if_too_big(void **state) {
	//arrange
	command_f list_subscriptions = get_command(state);
	

	ducq_srv *srv = ducq_srv_new();

	ducq_sub *sub1 = malloc(sizeof(ducq_sub));
	ducq_sub *sub2 = malloc(sizeof(ducq_sub));
	ducq_sub *sub3 = malloc(sizeof(ducq_sub));
	sub1->ducq     = ducq_new_mock("sub_id_1");
	sub2->ducq     = ducq_new_mock("sub_id_2");
	sub3->ducq     = ducq_new_mock("sub_id_3");
	sub1->route    = strdup("an excessively long route 1 an excessively long route 1 an excessively long route 1");
	sub2->route    = strdup("an excessively long route 2 an excessively long route 2 an excessively long route 2");
	sub3->route    = strdup("an excessively long route 3 an excessively long route 3 an excessively long route 3");
	sub1->id       = ducq_id(sub1->ducq);
	sub2->id       = ducq_id(sub2->ducq);
	sub3->id       = ducq_id(sub3->ducq);
	sub1->next     = NULL;
	sub2->next     = sub1;
	sub3->next     = sub2;

	srv->subs = sub3;


	ducq_i *emitter = ducq_new_mock(NULL);
	char request[] = "list_subscriptions *\n";
	size_t req_size = sizeof(request);
	
	char expected_reply1[] =
		"sub_id_3,an excessively long route 3 an excessively long route 3 an excessively long route 3\n"
		"sub_id_2,an excessively long route 2 an excessively long route 2 an excessively long route 2\n";
	char expected_reply2[] =
		"sub_id_1,an excessively long route 1 an excessively long route 1 an excessively long route 1\n";
	
	ducq_state expected_state = DUCQ_OK;

	expect_value_count(_send, ducq, emitter, 2);
	expect_memory(_send, buf, expected_reply1, strlen(expected_reply1));
	expect_memory(_send, buf, expected_reply2, strlen(expected_reply2));
	expect_value(_send, *count, strlen(expected_reply1));
	expect_value(_send, *count, strlen(expected_reply2));
	will_return_count(_send, DUCQ_OK, 2);
	
	expect_value(_close, ducq, emitter);
	will_return(_close, DUCQ_OK);

	// // act
		ducq_state actual_state = list_subscriptions(srv, emitter, request, req_size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
	ducq_free(emitter);
}





void list_subscriptions_close_connection_if_inner_send_fails(void **state) {
	//arrange
	command_f list_subscriptions = get_command(state);
	

	ducq_srv *srv = ducq_srv_new();

	ducq_sub *sub1 = malloc(sizeof(ducq_sub));
	ducq_sub *sub2 = malloc(sizeof(ducq_sub));
	ducq_sub *sub3 = malloc(sizeof(ducq_sub));
	sub1->ducq     = ducq_new_mock("sub_id_1");
	sub2->ducq     = ducq_new_mock("sub_id_2");
	sub3->ducq     = ducq_new_mock("sub_id_3");
	sub1->route    = strdup("an excessively long route 1 an excessively long route 1 an excessively long route 1");
	sub2->route    = strdup("an excessively long route 2 an excessively long route 2 an excessively long route 2");
	sub3->route    = strdup("an excessively long route 3 an excessively long route 3 an excessively long route 3");
	sub1->id       = ducq_id(sub1->ducq);
	sub2->id       = ducq_id(sub2->ducq);
	sub3->id       = ducq_id(sub3->ducq);
	sub1->next     = NULL;
	sub2->next     = sub1;
	sub3->next     = sub2;

	srv->subs = sub3;


	ducq_i *emitter = ducq_new_mock(NULL);
	char request[] = "list_subscriptions *\n";
	size_t req_size = sizeof(request);
	
	char expected_reply[] =
		"sub_id_3,an excessively long route 3 an excessively long route 3 an excessively long route 3\n"
		"sub_id_2,an excessively long route 2 an excessively long route 2 an excessively long route 2\n";
	
	ducq_state expected_state = DUCQ_OK;

	expect_value(_send, ducq, emitter);
	expect_memory(_send, buf, expected_reply, strlen(expected_reply));
	expect_value(_send, *count, strlen(expected_reply));
	will_return(_send, DUCQ_EWRITE);
	
	expect_value(_close, ducq, emitter);
	will_return(_close, DUCQ_OK);

	// // act
		ducq_state actual_state = list_subscriptions(srv, emitter, request, req_size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
	ducq_free(emitter);
}