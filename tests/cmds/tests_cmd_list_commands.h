#ifndef _LIST_COMMANDS_TESTS_HEADER_
#define _LIST_COMMANDS_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_list_commands_tests(apply) \
	apply(list_commands_list_all_commands)
	

foreach_list_commands_tests(build_forward_declarations)


#define build_list_commands_tests() \
	foreach_list_commands_tests(build_cmocka_unit_test)

#endif // _LIST_COMMANDS_TESTS_HEADER_
