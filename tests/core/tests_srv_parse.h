#ifndef _SRV_TESTS_HEADER_
#define _SRV_TESTS_HEADER_

#include "unit_tests.h"

#define foreach_srv_tests(apply) \
	apply(srv_ctor_dtor_no_leak) \
	apply(srv_parse_command_ok) \
	apply(srv_parse_command_no_space_returns_null) \
	apply(srv_parse_route_ok) \
	apply(srv_parse_route_after_parse_command_ok) \
	apply(srv_parse_route_no_space_err) \
	apply(srv_parse_route_no_newline_err) \
	apply(srv_parse_send_ack_ok) \
	apply(srv_parse_send_ack_returns_send_rc) \
	apply(srv_parse_send_nack_ok) \
	apply(srv_parse_send_nack_returns_send_rc) \

foreach_srv_tests(build_forward_declarations) 

#define build_srv_tests() \
	foreach_srv_tests(build_cmocka_unit_test)

#endif // _SRV_TESTS_HEADER_