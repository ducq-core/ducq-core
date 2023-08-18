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
	

	ducq_srv *srv = ducq_srv_new();
	ducq_srv_set_log(srv, NULL, mock_log);

	ducq_i *ducq1 =  ducq_new_mock("sub_id_1");
	ducq_i *ducq2 =  ducq_new_mock("sub_id_2");
	ducq_i *ducq3 =  ducq_new_mock("sub_id_3");
	will_return( _copy, ducq_new_mock(ducq_id(ducq1)) );
	will_return( _copy, ducq_new_mock(ducq_id(ducq2)) );
	will_return( _copy, ducq_new_mock(ducq_id(ducq3)) );
	ducq_srv_add(srv, ducq1, "route_1");
	ducq_srv_add(srv, ducq2, "route_2");
	ducq_srv_add(srv, ducq3, "route_3");

	ducq_i *emitter = ducq_new_mock(NULL);
	char request[] = "list_subscriptions *\n";
	size_t req_size = sizeof(request);
	
	ducq_state expected_state = DUCQ_OK;
	char expected_msg[] = 
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

	expect_value(_close, ducq, emitter);
	will_return(_close, DUCQ_OK);

	// act
	ducq_state actual_state = list_subscriptions(srv, emitter, request, req_size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
	ducq_free(emitter);
	ducq_free(ducq1);
	ducq_free(ducq2);
	ducq_free(ducq3);
}

