#ifndef _LIST_COMMANDS_TESTS_HEADER_
#define _LIST_COMMANDS_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_list_commands_tests(apply) \
	apply(list_commands_list_all_commands)
	

foreach_list_commands_tests(build_forward_declarations)




#define build_list_commands_tests() \
	foreach_list_commands_tests(build_cmocka_unit_test)

int list_commands_tests_setup(void **state);
int list_commands_tests_teardown(void **state);


#endif // _LIST_COMMANDS_TESTS_HEADER_