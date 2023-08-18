#ifndef _SUB_TESTS_HEADER_
#define _SUB_TESTS_HEADER_

#include "unit_tests.h"

int sub_setup(void **state);
int sub_teardown(void **state);

#define foreach_sub_tests(apply) \
	apply(srv_dtor_free_its_sub) \
	apply(srv_unsubscribe_remove_first_sub) \
	apply(srv_unsubscribe_remove_middle_sub) \
	apply(srv_unsubscribe_remove_last_sub) \
	apply(srv_unsubscribe_remove_inexistant_sub) \
	apply(srv_unsubscribe_ignore_close_connection_error)
	
foreach_sub_tests(build_forward_declarations) 

#define build_sub_tests_with_setup_and_teardown(test) \
	cmocka_unit_test_setup_teardown(test, sub_setup, sub_teardown), 


#define build_sub_tests() \
	foreach_sub_tests(build_sub_tests_with_setup_and_teardown)

#endif // _SUB_TESTS_HEADER_
