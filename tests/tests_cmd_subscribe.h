#ifndef _SUBSCRIBE_TESTS_HEADER_
#define _SUBSCRIBE_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_subscribe_tests(apply) \
	apply(subscribe_) \
	

foreach_subscribe_tests(build_forward_declarations)




#define build_subscribe_tests() \
	foreach_subscribe_tests(build_cmocka_unit_test)

#endif // _SUBSCRIBE_TESTS_HEADER_