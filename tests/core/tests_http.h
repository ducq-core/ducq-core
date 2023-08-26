#ifndef _HTTP_TESTS_HEADER_
#define _HTTP_TESTS_HEADER_

#include "unit_tests.h"

#define foreach_http_tests(apply) \
	apply(http_upgrade_to_ws) \
	apply(http_upgrade_to_ws_header_incomplete)

foreach_http_tests(build_forward_declarations)

#define build_http_tests() \
	foreach_http_tests(build_cmocka_unit_test) 

#endif // _HTTP_TESTS_HEADER_
