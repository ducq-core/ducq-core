#ifndef _STRING_TESTS_HEADER_
#define _STRING_TESTS_HEADER_

#include "unit_tests.h"

#define foreach_error_tests(apply) \
	apply(error_0_return_ok) \
	apply(error_return_msg) \
	apply(error_negative_return_msg) \
	apply(error_out_of_range_return_unknown) \
	apply(error_negative_out_of_range_return_unknown)
	
foreach_error_tests(build_forward_declarations)


#endif // _STRING_TESTS_HEADER_