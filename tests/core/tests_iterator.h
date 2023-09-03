#ifndef _ITERATOR_TESTS_HEADER_
#define _ITERATOR_TESTS_HEADER_

#include "unit_tests.h"

int iterator_setup(void **state);
int iterator_teardown(void **state);

#define foreach_iterator_tests(apply) \
	apply(reactor_unsubscribe_remove_first_with_iterator) \
	apply(reactor_unsubscribe_remove_middle_with_iterator) \
	apply(reactor_unsubscribe_remove_last_with_iterator) \
	apply(reactor_unsubscribe_remove_last_with_a_server_iterator)
	
foreach_iterator_tests(build_forward_declarations) 

#define build_iterator_tests_with_setup_and_teardown(test) \
	cmocka_unit_test_setup_teardown(test, iterator_setup, iterator_teardown), 


#define build_iterator_tests() \
	foreach_iterator_tests(build_iterator_tests_with_setup_and_teardown)

#endif // _ITERATOR_TESTS_HEADER_
