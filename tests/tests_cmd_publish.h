#ifndef _PUBLISH_TESTS_HEADER_
#define _PUBLISH_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_publish_tests(apply) \
	apply(publish_) \
	

foreach_publish_tests(build_forward_declarations)




#define build_publish_tests() \
	foreach_publish_tests(build_cmocka_unit_test)

int publish_tests_setup(void **state);
int publish_tests_teardown(void **state);


#endif // _PUBLISH_TESTS_HEADER_