#ifndef _LOG_TESTS_HEADER_
#define _LOG_TESTS_HEADER_

#include "unit_tests.h"

#define foreach_log_tests(apply) \
	apply(log_calls_log_callback) \
	apply(log_sent_to_monitor_route) \
	apply(log_dont_sent_to_monitor_route_if_not_set) \
	apply(log_warn_macro)

foreach_log_tests(build_forward_declarations) 

#define build_log_tests() \
	foreach_log_tests(build_cmocka_unit_test)

#endif // _LOG_TESTS_HEADER_
