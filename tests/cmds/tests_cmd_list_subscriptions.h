#ifndef _LIST_SUBSCRIPTIONS_TESTS_HEADER_
#define _LIST_SUBSCRIPTIONS_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_list_subscriptions_tests(apply) \
	apply(list_subscriptions_list_all_subscribers_id)

foreach_list_subscriptions_tests(build_forward_declarations)


#define build_list_subscriptions_tests() \
	foreach_list_subscriptions_tests(build_cmocka_unit_test)

int list_subscriptions_tests_setup(void **state);
int list_subscriptions_tests_teardown(void **state);


#endif // _LIST_SUBSCRIPTIONS_TESTS_HEADER_
