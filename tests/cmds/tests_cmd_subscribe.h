#ifndef _SUBSCRIBE_TESTS_HEADER_
#define _SUBSCRIBE_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_subscribe_tests(apply) \
	apply(subscribe_msg_invalide_if_cant_parse_route) \
	apply(subscribe_add_subscriber_to_reactor_subs) \
	apply(subscribe_add_second_subscriber_to_reactor_subs) \
	apply(subscribe_subscribe_inexistent_return_not_found) \
	apply(subscribe_send_ack_fail_cleans_up)
	

foreach_subscribe_tests(build_forward_declarations)




#define build_subscribe_tests() \
	foreach_subscribe_tests(build_cmocka_unit_test)


int subscribe_tests_setup(void **state);
int subscribe_tests_teardown(void **state);


#endif // _SUBSCRIBE_TESTS_HEADER_
