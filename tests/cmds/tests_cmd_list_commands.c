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
#include "../src/ducq_dispatcher.h"




#if DUCQ_MSGSZ != 256
#error "expect DUCQ_MSGSZ == 256 for list_commands tests"
#endif


int _check_function(const LargestIntegralType param, LargestIntegralType check_data) {
	(void) check_data;
	char *reply   = (char*) param;

	int subscribe_count          = 0;
	int publish_count            = 0;
	int list_subscriptions_count = 0;
	int list_commands_count      = 0;

	char *ptr = strtok(reply, ",");
	while(ptr) {
		     if( 0 == strcmp(ptr, "subscribe"))
			subscribe_count++;
		else if( 0 == strcmp(ptr, "publish"))
			publish_count++;
		else if( 0 == strcmp(ptr, "list_commands"))
			list_commands_count++;
		else if( 0 == strcmp(ptr, "list_subscriptions"))
			list_subscriptions_count++;

		ptr = strtok(NULL, "\n");
		ptr = strtok(NULL,  ",");
	}

	assert_int_equal(subscribe_count,          1);
	assert_int_equal(publish_count,            1);
	assert_int_equal(list_subscriptions_count, 1);
	assert_int_equal(list_commands_count,      1);
	return true;
}
void list_commands_list_all_commands(void **state) {
	//arrange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, NULL, mock_log);

	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);

	ducq_i *emitter = ducq_new_mock(NULL);
	ducq_reactor_add_client(reactor, 14, emitter);


	char expected_msg[] = "";
	ducq_state expected_state = DUCQ_OK;

	expect_value_count(_send, ducq, emitter, 1);
	expect_check(_send, buf, _check_function, NULL);
	expect_any(_send, *count);
	will_return(_send, DUCQ_OK);

	expect_string(mock_log, function_name, "list_commands");
	expect_value(mock_log, level, DUCQ_LOG_INFO);
	

	// act
	char request[] = "list_commands *\n";
	size_t req_size = sizeof(request);

	ducq_dispatcher_load_commands_path(dispatcher, "./commands");
	ducq_state actual_state = ducq_dispatch(dispatcher, emitter, request, req_size);


	//audit
	assert_int_equal(expected_state, actual_state);

	
	//teardown
	expect_any_count(_close, ducq, 1);
	will_return_count(_close, DUCQ_OK, 1);
	ducq_reactor_free(reactor);
}
