#ifndef _DISPATCHER_TESTS_HEADER_
#define _DISPATCHER_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_dispatcher_tests(apply) \
	apply(dispatcher_can_load_a_command) \
	apply(dispatcher_correct_number_of_commands) \
	apply(dispatcher_commands_have_expected_name) \
	apply(dispatcher_sender_receive_nack_if_command_unknown) \
	apply(dispatcher_returns_command_state)
	
foreach_dispatcher_tests(build_forward_declarations)



#define build_dispatcher_tests() \
	foreach_dispatcher_tests(build_cmocka_unit_test)

#endif // _DISPATCHER_TESTS_HEADER_
