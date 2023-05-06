#ifndef _ACK_TESTS_HEADER_
#define _ACK_TESTS_HEADER_

#include "unit_tests.h"

#define foreach_ack_tests(apply) \
	apply(srv_send_ack_ok) \
	apply(srv_send_ack_returns_send_rc) \
	apply(srv_send_nack_ok) \
	apply(srv_send_nack_returns_send_rc) \

foreach_ack_tests(build_forward_declarations) 

#define build_ack_tests() \
	foreach_ack_tests(build_cmocka_unit_test)

#endif // _ACK_TESTS_HEADER_