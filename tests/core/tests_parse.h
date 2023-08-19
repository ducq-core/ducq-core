#ifndef _PARSE_TESTS_HEADER_
#define _PARSE_TESTS_HEADER_

#include "unit_tests.h"

#define foreach_parse_tests(apply) \
	apply(parse_command_ok) \
	apply(parse_command_no_space_returns_null) \
	apply(parse_route_ok) \
	apply(parse_route_after_parse_command_ok) \
	apply(parse_route_no_space_err) \
	apply(parse_route_no_newline_err) \
	apply(parse_payload_ok) \
	apply(parse_payload_no_newline_err) \
	apply(parse_ack) \
	apply(parse_nack) \
	apply(parse_nack_invalid) \
	apply(route_cmp_identical_ok) \
	apply(route_cmp_not_identical_ok) \
	apply(route_cmp_null_sub_is_false) \
	apply(route_cmp_null_pub_is_false) \
	apply(route_cmp_wildcard_at_end_ok) \
	apply(route_cmp_wildcard_not_at_end_ok) \
	apply(route_cmp_wildcard_in_middle_ok) \
	apply(route_cmp_wildcard_is_flush_ok) \
	apply(route_cmp_wildcard_in_middle_flush_ok) \
	apply(route_cmp_wildcard_in_middle_flush_not_matched) \
	apply(route_cmp_wildcard_in_middle_flush_not_matched2) \
	apply(parse_msg)

foreach_parse_tests(build_forward_declarations) 

#define build_parse_tests() \
	foreach_parse_tests(build_cmocka_unit_test)

#endif // _PARSE_TESTS_HEADER_
