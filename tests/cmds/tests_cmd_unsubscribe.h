#ifndef _UNSUBSCRIBE_TESTS_HEADER_
#define _UNSUBSCRIBE_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_unsubscribe_tests(apply) \
	apply(unsubscribe_close_connection_and_remove_client) \
	apply(unsubscribe_client_not_found) \
	apply(unsubscribe_no_payload) \
	apply(unsubscribe_empty_payload)

foreach_unsubscribe_tests(build_forward_declarations)


#define build_unsubscribe_tests() \
	foreach_unsubscribe_tests(build_cmocka_unit_test)

int unsubscribe_tests_setup(void **state);
int unsubscribe_tests_teardown(void **state);


#endif // _UNSUBSCRIBE_TESTS_HEADER_
