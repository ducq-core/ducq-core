#ifndef _PUB_SUB_LAST_MSG_TESTS_HEADER_
#define _PUB_SUB_LAST_MSG_TESTS_HEADER_

#include "unit_tests.h"


int pub_sub_last_msg_group_setup(void **state);
int pub_sub_last_msg_tests_setup(void **state);
int pub_sub_last_msg_tests_teardown(void **state);
int pub_sub_last_msg_group_teardown(void **state);


#define foreach_pub_sub_last_msg_tests(apply) \
	apply(pub_sub_last_msg_send_ack_if_not_requested) \
	apply(pub_sub_last_msg_send_last_if_requested) \
	apply(pub_sub_last_msg_send_last_of_route) \
	apply(pub_sub_last_msg_ignore_wildcards)

foreach_pub_sub_last_msg_tests(build_forward_declarations)


#define build_pub_sub_last_msg_setup_teardown(elem) \
	cmocka_unit_test_setup_teardown(elem, \
		pub_sub_last_msg_tests_setup, \
		pub_sub_last_msg_tests_teardown),

#define build_pub_sub_last_msg_tests() \
	foreach_pub_sub_last_msg_tests(build_pub_sub_last_msg_setup_teardown)

#endif // _PUB_SUB_LAST_MSG_TESTS_HEADER_
