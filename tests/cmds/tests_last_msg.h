#ifndef _LAST_MSG_TESTS_HEADER_
#define _LAST_MSG_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_last_msg_tests(apply) \
	apply(last_msg_no_msg_ret_ack) \
	apply(last_msg_one_msg_ret_last_msg) \
	apply(last_msg_two_msg_ret_last_msg) \
	apply(last_msg_one_msg_many_channels_ret_last_msg) \
	apply(last_msg_two_msg_many_channels_ret_last_msg) \
	apply(last_msg_many_msg_many_channels_dont_overflow) \
	apply(last_msg_null_route_dont_stop) \
	apply(last_msg_set_reach_max) \
	apply(last_msg_get_reach_max) \
	apply(last_msg_make_copy)

foreach_last_msg_tests(build_forward_declarations)



int last_msg_tests_setup(void **state);
int last_msg_tests_teardown(void **state);

#define build_last_msg_setup_teardown(elem) \
	cmocka_unit_test_setup_teardown(elem, \
		last_msg_tests_setup, \
		last_msg_tests_teardown), 

#define build_last_msg_tests() \
	foreach_last_msg_tests(build_last_msg_setup_teardown)

#endif // _LAST_MSG_TESTS_HEADER_
