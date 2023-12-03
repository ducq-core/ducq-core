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

struct command_count {
	int subscribe;
	int publish;
	int list_subscriptions;
	int list_commands;
	int lua_command;

};
int _check_function(const LargestIntegralType param, LargestIntegralType check_data) {
	char *reply   = (char*) param;
	struct command_count *count = (struct command_count *) check_data;

	char *ptr = strtok(reply, "\",");
	if(!ptr) fail();

	     if( 0 == strcmp(ptr, "subscribe"))
		count->subscribe++;
	else if( 0 == strcmp(ptr, "publish"))
		count->publish++;
	else if( 0 == strcmp(ptr, "list_commands"))
		count->list_commands++;
	else if( 0 == strcmp(ptr, "list_subscriptions"))
		count->list_subscriptions++;
	else if( 0 == strcmp(ptr, "lua_command"))
		count->lua_command++;

	return true;
}
int _check_log(const LargestIntegralType param, LargestIntegralType check_data) {
	char *name = (char*) param;
	int *count = (int *) check_data;

	if( 0 == strcmp(name, "list_commands"))
		(*count)++;
	else
		expect_check(mock_log, function_name, _check_log, count);

	return true;
}
void list_commands_list_all_commands(void **state) {
	//arrange
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, NULL, mock_log);

	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);

	ducq_i *emitter = ducq_new_mock(NULL);
	ducq_reactor_add_client(reactor, 14, emitter);

	ducq_state expected_state = DUCQ_OK;

	struct command_count count = {};
	int list_command_got_logged_count = 0;

	
	// mock
	expect_value(_parts, ducq, emitter);
	will_return(_parts, DUCQ_OK);

	expect_value_count(_send, ducq, emitter, 5);
	expect_check(_send, buf, _check_function, &count);
	expect_check(_send, buf, _check_function, &count);
	expect_check(_send, buf, _check_function, &count);
	expect_check(_send, buf, _check_function, &count);
	expect_check(_send, buf, _check_function, &count);
	expect_any_always(_send, *count);
	will_return_always(_send, DUCQ_OK);

	expect_check(mock_log, function_name, _check_log, &list_command_got_logged_count);
	expect_any_always(mock_log, level);	

	expect_value(_end, ducq, emitter);
	will_return(_end, DUCQ_OK);


	// act
	ducq_dispatcher_load_commands_path(dispatcher, "./commands");
	ducq_dispatcher_add(dispatcher, "./commands");

	char request[] = "list_commands _\n";
	size_t req_size = sizeof(request);
	ducq_state actual_state = ducq_dispatch(dispatcher, emitter, request, req_size);


	//audit
	assert_int_equal(expected_state, actual_state);
	assert_int_equal(count.subscribe,          1);
	assert_int_equal(count.publish,            1);
	assert_int_equal(count.list_commands,      1);
	assert_int_equal(count.list_subscriptions, 1);
	assert_int_equal(count.lua_command,        1);
	assert_int_equal(list_command_got_logged_count, 1);

	
	//teardown
	expect_any_count(_close, ducq, 1);
	will_return_count(_close, DUCQ_OK, 1);
	ducq_reactor_free(reactor);
}
