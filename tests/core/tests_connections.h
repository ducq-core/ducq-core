#ifndef _CONNECTIONS_TESTS_HEADER_
#define _CONNECTIONS_TESTS_HEADER_

#include "unit_tests.h"

int connections_setup(void **state);
int connections_teardown(void **state);

#define foreach_connections_tests(apply) \
	apply(reactor_dtor_free_its_connection) \
	apply(reactor_unsubscribe_remove_first_connection) \
	apply(reactor_unsubscribe_remove_middle_connection) \
	apply(reactor_unsubscribe_remove_last_connection) \
	apply(reactor_unsubscribe_remove_last_with_a_server) \
	apply(reactor_unsubscribe_remove_all_connection) \
	apply(reactor_unsubscribe_remove_inexistant_connection) \
	apply(reactor_unsubscribe_ignore_close_connection_error)
	
foreach_connections_tests(build_forward_declarations) 

#define build_connections_tests_with_setup_and_teardown(test) \
	cmocka_unit_test_setup_teardown(test, connections_setup, connections_teardown), 


#define build_connections_tests() \
	foreach_connections_tests(build_connections_tests_with_setup_and_teardown)

#endif // _CONNECTIONS_TESTS_HEADER_
