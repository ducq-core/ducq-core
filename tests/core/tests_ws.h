#ifndef _WS_TESTS_HEADER_
#define _WS_TESTS_HEADER_

#include "unit_tests.h"

#define foreach_ws_tests(apply) \
	apply(ws_upgrade_from_http) \
	apply(ws_upgrade_from_http_msg_invalid_if_no_key) \
	apply(ws_upgrade_from_http_return_ewrite)

foreach_ws_tests(build_forward_declarations)

#define build_ws_tests() \
	foreach_ws_tests(build_cmocka_unit_test) 

#endif // _WS_TESTS_HEADER_
