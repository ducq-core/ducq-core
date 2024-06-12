#ifndef _LIST_CONNECTIONS_TESTS_HEADER_
#define _LIST_CONNECTIONS_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_list_connections_tests(apply) \
	apply(list_connections_list_all_connections_id) \
	apply(list_connections_list_all_non_subscribers)

foreach_list_connections_tests(build_forward_declarations)


#define build_list_connections_tests() \
	foreach_list_connections_tests(build_cmocka_unit_test)

int list_connections_tests_setup(void **state);
int list_connections_tests_teardown(void **state);


#endif // _LIST_CONNECTIONS_TESTS_HEADER_
