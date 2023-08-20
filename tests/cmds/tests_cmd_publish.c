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


int publish_tests_setup(void **state) {
	*state = fix_new("publish");
	return *state == NULL;
}
int publish_tests_teardown(void **state) {
	struct fixture *fix = *state;
return fix_free(fix);
}





void publish_send_msg_invalid_and_disconnect_if_cant_parse_route(void **state) {
	//arrange
	ducq_command_f publish = get_command(state);
	
	ducq_state expected_state = DUCQ_EMSGINV;

	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, NULL, mock_log);

	ducq_i *publisher = ducq_new_mock(NULL);
	ducq_reactor_add_client(reactor, 10, publisher);

	char buffer[] = "publishroute\npayload";
	size_t size = sizeof(buffer);
	char expected_msg[128];
	snprintf(expected_msg, 128, "NACK *\n%d\n%s",
		DUCQ_EMSGINV,
		ducq_state_tostr(DUCQ_EMSGINV)
	);
	expect_value(_send, ducq, publisher);
	expect_string(_send, buf, expected_msg);
	expect_value(_send, *count, strlen(expected_msg));
	will_return(_send, DUCQ_OK);

	expect_string(mock_log, function_name, "publish");
	expect_value(mock_log, level, DUCQ_LOG_WARN);

	expect_value(_close, ducq, publisher);
	will_return(_close, DUCQ_OK);

	// act
	ducq_state actual_state = publish(reactor, publisher, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	ducq_reactor_free(reactor);
}

void publish_subscribers_has_ducq_send_called(void **state) {
	//arrange
	ducq_command_f publish = get_command(state);
	
	ducq_state expected_state = DUCQ_OK;

	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, NULL, mock_log);

	ducq_i *publisher = ducq_new_mock(NULL);
	char buffer[] = "publish route\npayload";
	size_t size = sizeof(buffer);


	ducq_i *ducq1 = ducq_new_mock(NULL);
	ducq_i *ducq2 = ducq_new_mock(NULL);
	ducq_i *ducq3 = ducq_new_mock(NULL);
	ducq_reactor_add_client(reactor, 10, ducq1);
	ducq_reactor_add_client(reactor, 11, ducq2);
	ducq_reactor_add_client(reactor, 12, ducq3);
	ducq_reactor_subscribe(reactor, ducq1, "route");
	ducq_reactor_subscribe(reactor, ducq2, "not same route");
	ducq_reactor_subscribe(reactor, ducq3, "route");

	expect_value(_send, ducq, publisher);
	char ack_msg[] = "ACK *\n0\nok";
	expect_string(_send, buf, ack_msg);
	expect_value(_send, *count, sizeof(ack_msg)-1);
	will_return(_send, DUCQ_OK);

	expect_value(_send, ducq, ducq3);
	expect_value(_send, ducq, ducq1);
	expect_string_count(_send, buf, buffer, 2);
	expect_value_count(_send, *count, sizeof(buffer), 2);
	will_return_count(_send, DUCQ_OK, 2);
	
	expect_string(mock_log, function_name, "publish"); // error
	expect_value(mock_log, level, DUCQ_LOG_INFO);
	expect_string(mock_log, function_name, "publish"); // subscriber count
	expect_value(mock_log, level, DUCQ_LOG_INFO);


	// act
	ducq_state actual_state = publish(reactor, publisher, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_free(publisher);
	ducq_reactor_free(reactor);
}


struct sub_check_ctx {
	int count;
	int match;
};
static
ducq_loop_t _check_sub_deleted(ducq_i *ducq, char *actual_route, void *ctx) {
	struct sub_check_ctx *expected = (struct sub_check_ctx*) ctx;
	if( strcmp(ducq_id(ducq), "A") || strcmp(ducq_id(ducq), "C") ) {
		expected->match++;
	}

	expected->count++;
	return DUCQ_LOOP_CONTINUE;
}
void publish_unsubcribe_sub_on_write_error(void **state) {
	//arrange
	ducq_command_f publish = get_command(state);
	
	ducq_state expected_state = DUCQ_OK;

	ducq_reactor *reactor = ducq_reactor_new();
	ducq_i *publisher = ducq_new_mock(NULL);
	char buffer[] = "publish route\npayload";
	size_t size = strlen(buffer);

	ducq_i *ducq1 =  ducq_new_mock("A");
	ducq_i *ducq2 =  ducq_new_mock("B");
	ducq_i *ducq3 =  ducq_new_mock("C");
	ducq_reactor_add_client(reactor, 10, ducq1);
	ducq_reactor_add_client(reactor, 11, ducq2);
	ducq_reactor_add_client(reactor, 12, ducq3);
	ducq_reactor_subscribe(reactor, ducq1, "route");
	ducq_reactor_subscribe(reactor, ducq2, "route");
	ducq_reactor_subscribe(reactor, ducq3, "route");


	expect_value(_send, ducq, publisher);
	char ack_msg[] = "ACK *\n0\nok";
	expect_string(_send, buf, ack_msg);
	expect_value(_send, *count, sizeof(ack_msg)-1);
	will_return(_send, DUCQ_OK);
	
	expect_value(_send, ducq, ducq3);
	expect_value(_send, ducq, ducq2);
	expect_value(_send, ducq, ducq1);
	expect_string_count(_send, buf, buffer, 3);
	expect_value_count(_send, *count, size, 3);
	will_return(_send, DUCQ_OK);
	will_return(_send, DUCQ_EWRITE);
	will_return(_send, DUCQ_OK);

	expect_value(_close, ducq, ducq2);
	will_return(_close, DUCQ_OK);

	int expected_count = 2;
	int expected_match = 2;


	// act
	ducq_state actual_state = publish(reactor, publisher, buffer, size);

	//audit
	struct sub_check_ctx ctx = {
		.count = 0,
		.match = 0
	};
	assert_int_equal(expected_state, actual_state);
	ducq_reactor_loop(reactor, _check_sub_deleted, &ctx);
	assert_int_equal(expected_count, ctx.count);
	assert_int_equal(expected_match, ctx.match);


	//teardown
	ducq_free(publisher);

	expect_any_count(_close, ducq, 2);
	will_return_count(_close, DUCQ_OK, 2);
	ducq_reactor_free(reactor);
}
