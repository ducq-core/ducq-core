#ifndef _STATE_TESTS_HEADER_
#define _STATE_TESTS_HEADER_

#include "unit_tests.h"

#define foreach_state_tests(apply) \
	apply(state_DUCQ_OK_has_value_0) \
	apply(state_0_return_ok) \
	apply(state_return_msg) \
	apply(state_negative_return_msg) \
	apply(state_out_of_range_return_unknown) \
	apply(state_negative_out_of_range_return_unknown)
	
foreach_state_tests(build_forward_declarations)

#define build_state_tests() \
	foreach_state_tests(build_cmocka_unit_test)

#endif // _STATE_TESTS_HEADER_