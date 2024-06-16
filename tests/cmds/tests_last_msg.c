
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>

#include "unit_tests_cmd.h"

#include "ducq_reactor.h"



int last_msg_tests_setup(void **state) {
	ducq_reactor *reactor = ducq_reactor_new();
	ducq_reactor_set_log(reactor, NULL, mock_log);

	*state = reactor;

	return reactor == NULL;
}



int last_msg_tests_teardown(void **state) {
	ducq_reactor *reactor = *state;
	ducq_reactor_free(reactor);
	return 0;
}






void last_msg_no_msg_ret_ack(void **state) {
	ducq_reactor *reactor = *state;

	const char * last_msg = ducq_get_last_msg(reactor, "route");

	assert_string_equal(last_msg, "ACK");
}



void last_msg_one_msg_ret_last_msg(void **state) {
	ducq_reactor *reactor = *state;

	ducq_set_last_msg(reactor, "route",  "last message");

	const char *last_msg = ducq_get_last_msg(reactor, "route");

	assert_string_equal(last_msg, "last message");
}



void last_msg_two_msg_ret_last_msg(void **state) {
	ducq_reactor *reactor = *state;

	ducq_set_last_msg(reactor, "route", "first message");
	ducq_set_last_msg(reactor, "route", "second message");
	ducq_set_last_msg(reactor, "route", "last message");

	const char *last_msg = ducq_get_last_msg(reactor, "route");

	assert_string_equal(last_msg, "last message");
}



void last_msg_one_msg_many_channels_ret_last_msg(void **state) {
	ducq_reactor *reactor = *state;

	ducq_set_last_msg(reactor, "route", "last message");
	ducq_set_last_msg(reactor, "other", "other message");

	const char *last_msg = ducq_get_last_msg(reactor, "route");

	assert_string_equal(last_msg, "last message");
}



void last_msg_two_msg_many_channels_ret_last_msg(void **state) {
	ducq_reactor *reactor = *state;

	ducq_set_last_msg(reactor, "route", "last message");
	ducq_set_last_msg(reactor, "other", "other message");

	const char *last_msg = ducq_get_last_msg(reactor, "route");

	assert_string_equal(last_msg, "last message");
}



void last_msg_set_reach_max(void **state) {
	ducq_reactor *reactor = *state;

	for(int i = 0; i < DUCQ_MAX_CHANNELS; i++) {
		char route[DUCQ_MAX_ROUTE_LEN] = "";
		snprintf(route, DUCQ_MAX_ROUTE_LEN, "route%d", i);
		ducq_set_last_msg(reactor, route, "last message");
	}

	ducq_state ret = ducq_set_last_msg(reactor, "route error", "last message");

	assert_int_equal(ret, DUCQ_EMAX);
}



void last_msg_get_reach_max(void **state) {
	ducq_reactor *reactor = *state;

	for(int i = 0; i < DUCQ_MAX_CHANNELS; i++) {
		char route[DUCQ_MAX_ROUTE_LEN] = "";
		snprintf(route, DUCQ_MAX_ROUTE_LEN, "route%d", i);
		ducq_set_last_msg(reactor, route, route);
	}

	const char * last_msg = ducq_get_last_msg(reactor, "route");

	assert_string_equal(last_msg, "ACK\n_");
}



void last_msg_make_copy(void **state) {
	ducq_reactor *reactor = *state;

	char last_msg_set[] = "last message";
	ducq_set_last_msg(reactor, "route", last_msg_set);
	strcpy(last_msg_set, "changed!");

	const char *last_msg_get = ducq_get_last_msg(reactor, "route");

	assert_string_equal(last_msg_set, "changed!");
	assert_string_equal(last_msg_get, "last message");
}
