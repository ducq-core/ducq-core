#ifndef __TESTS_BASE64_HEADER__
#define __TESTS_BASE64_HEADER__


#include "unit_tests.h"


#define foreach_base64_tests(apply) \
	apply(test_hello_world) \
	apply(test_one_padding) \
	apply(test_two_padding) \
	apply(test_long_text) \
	apply(test_enough_space) \
	apply(test_not_enough_space) \
	apply(test_wiki_3) \
	apply(test_wiki_2) \
	apply(test_wiki_1)

foreach_base64_tests(build_forward_declarations)

#define build_base64_tests() \
	foreach_base64_tests(build_cmocka_unit_test)



#endif // __TESTS_BASE64_HEADER__

