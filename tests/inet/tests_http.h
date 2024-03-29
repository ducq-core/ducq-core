#ifndef __TESTS_HTTP_HEADER__
#define __TESTS_HTTP_HEADER__


#include "unit_tests.h"


#define foreach_http_tests(apply) \
	apply(http_get_header_ok) \
	apply(http_get_header_not_terminated) \
	apply(http_find_http_header) \
	apply(http_find_http_header_fails) \
	apply(http_find_http_header_even_if_token_found_elsewhere) \
	apply(http_find_http_header_even_if_last) \
	apply(http_find_http_header_not_found_both_return_and_end_are_null)

foreach_http_tests(build_forward_declarations)

#define build_http_tests() \
	foreach_http_tests(build_cmocka_unit_test)



#endif // __TESTS_HTTP_HEADER__
