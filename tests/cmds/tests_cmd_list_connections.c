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
#error "expect DUCQ_MSGSZ == 256 for list_connections tests"
#endif



int list_connections_tests_setup(void **state) {
	*state = fix_new("lsconn");
	return *state == NULL;
}
int list_connections_tests_teardown(void **state) {
	struct fixture *fix = *state;
	return fix_free(fix);
}


void list_connections_list_all_connections_id(void **state) {
	//arrange
	ducq_command_f list_connections = get_command(state);
	

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

	char request[] = "lsconn *\n";
	size_t req_size = sizeof(request);
	
	ducq_state expected_state = DUCQ_OK;

	// mock
	expect_value(_parts, ducq, emitter);
	will_return(_parts, DUCQ_OK);

	expect_value_count(_send, ducq, emitter, 4);
	expect_string(_send, buf,          "sub_id_1,route_1\n") ;
	expect_value(_send, *count, strlen("sub_id_1,route_1\n"));
	expect_string(_send, buf,          "sub_id_2,route_2\n") ;
	expect_value(_send, *count, strlen("sub_id_2,route_2\n"));
	expect_string(_send, buf,          "sub_id_3,route_3\n") ;
	expect_value(_send, *count, strlen("sub_id_3,route_3\n"));
	expect_string(_send, buf,          "emitter,\n") ;
	expect_value(_send, *count, strlen("emitter,\n"));
	will_return_always(_send, DUCQ_OK);

	will_return(_end, DUCQ_OK);
	expect_value(_end, ducq, emitter);


	expect_string(mock_log, function_name, "lsconn");
	expect_value(mock_log, level, DUCQ_LOG_INFO);


	// act
	ducq_state actual_state = list_connections(reactor, emitter, request, req_size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	expect_any_count(_close, ducq, 4);
	will_return_count(_close, DUCQ_OK, 4);
	ducq_reactor_free(reactor);
}

void list_connections_list_all_non_subscribers(void **state) {
	//arrange
	ducq_command_f list_connections = get_command(state);
	

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

	char request[] = "lsconn *\n";
	size_t req_size = sizeof(request);
	
	ducq_state expected_state = DUCQ_OK;

	// mock
	expect_value(_parts, ducq, emitter);
	will_return(_parts, DUCQ_OK);

	expect_value_count(_send, ducq, emitter, 4);
	expect_string(_send, buf,          "sub_id_1,route_1\n") ;
	expect_value(_send, *count, strlen("sub_id_1,route_1\n"));
	expect_string(_send, buf,          "sub_id_2,\n") ;
	expect_value(_send, *count, strlen("sub_id_2,\n"));
	expect_string(_send, buf,          "sub_id_3,route_3\n") ;
	expect_value(_send, *count, strlen("sub_id_3,route_3\n"));
	expect_string(_send, buf,          "emitter,\n") ;
	expect_value(_send, *count, strlen("emitter,\n"));
	will_return_always(_send, DUCQ_OK);

	will_return(_end, DUCQ_OK);
	expect_value(_end, ducq, emitter);


	expect_string(mock_log, function_name, "lsconn");
	expect_value(mock_log, level, DUCQ_LOG_INFO);


	// act
	ducq_state actual_state = list_connections(reactor, emitter, request, req_size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	expect_any_count(_close, ducq, 4);
	will_return_count(_close, DUCQ_OK, 4);
	ducq_reactor_free(reactor);
}

