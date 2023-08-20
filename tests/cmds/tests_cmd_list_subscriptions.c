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
	ducq_command_f list_subscriptions = get_command(state);
	

	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, NULL, mock_log);

	ducq_i *ducq1 =  ducq_new_mock("sub_id_1");
	ducq_i *ducq2 =  ducq_new_mock("sub_id_2");
	ducq_i *ducq3 =  ducq_new_mock("sub_id_3");
	ducq_reactor_add_client(reactor, 11, ducq1);
	ducq_reactor_add_client(reactor, 12, ducq2);
	ducq_reactor_add_client(reactor, 13, ducq3);
	ducq_reactor_subscribe(reactor, ducq1, "route_1");
	ducq_reactor_subscribe(reactor, ducq2, "route_2");
	ducq_reactor_subscribe(reactor, ducq3, "route_3");

	ducq_i *emitter = ducq_new_mock("emitter");
	ducq_reactor_add_client(reactor, 14, emitter);

	char request[] = "list_subscriptions *\n";
	size_t req_size = sizeof(request);
	
	ducq_state expected_state = DUCQ_OK;
	char expected_msg[] = 
		"emitter\n"
		"sub_id_3,route_3\n"
		"sub_id_2,route_2\n"
		"sub_id_1,route_1\n"
	;


	expect_value(_send, ducq, emitter);
	expect_string(_send, buf, expected_msg);
	expect_value(_send, *count, strlen(expected_msg));
	will_return(_send, DUCQ_OK);

	expect_string(mock_log, function_name, "list_subscriptions");
	expect_value(mock_log, level, DUCQ_LOG_INFO);


	// act
	ducq_state actual_state = list_subscriptions(reactor, emitter, request, req_size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	expect_any_count(_close, ducq, 4);
	will_return_count(_close, DUCQ_OK, 4);
	ducq_reactor_free(reactor);
}

void list_subscriptions_list_all_non_subscribers(void **state) {
	//arrange
	ducq_command_f list_subscriptions = get_command(state);
	

	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, NULL, mock_log);

	ducq_i *ducq1 =  ducq_new_mock("sub_id_1");
	ducq_i *ducq2 =  ducq_new_mock("sub_id_2");
	ducq_i *ducq3 =  ducq_new_mock("sub_id_3");
	ducq_reactor_add_client(reactor, 11, ducq1);
	ducq_reactor_add_client(reactor, 12, ducq2);
	ducq_reactor_add_client(reactor, 13, ducq3);
	ducq_reactor_subscribe(reactor, ducq1, "route_1");
//	ducq_reactor_subscribe(reactor, ducq2, "route_2");
	ducq_reactor_subscribe(reactor, ducq3, "route_3");

	ducq_i *emitter = ducq_new_mock("emitter");
	ducq_reactor_add_client(reactor, 14, emitter);

	char request[] = "list_subscriptions *\n";
	size_t req_size = sizeof(request);
	
	ducq_state expected_state = DUCQ_OK;
	char expected_msg[] = 
		"emitter\n"
		"sub_id_3,route_3\n"
		"sub_id_2\n"
		"sub_id_1,route_1\n"
	;


	expect_value(_send, ducq, emitter);
	expect_string(_send, buf, expected_msg);
	expect_value(_send, *count, strlen(expected_msg));
	will_return(_send, DUCQ_OK);

	expect_string(mock_log, function_name, "list_subscriptions");
	expect_value(mock_log, level, DUCQ_LOG_INFO);


	// act
	ducq_state actual_state = list_subscriptions(reactor, emitter, request, req_size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	expect_any_count(_close, ducq, 4);
	will_return_count(_close, DUCQ_OK, 4);
	ducq_reactor_free(reactor);
}

