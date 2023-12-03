#ifndef _RECEIVE_HEADER_
#define _RECEIVE_HEADER_

#include "unit_tests.h"

#define foreach_receive_tests(apply) \
	apply(receive_ok) \
	apply(receive_ack) \
	apply(receive_nack) \
	apply(receive_ping) \
	apply(receive_ping_send_fail) \
	apply(receive_pong) \
	apply(receive_other_message) \
	apply(receive_timeout_send_ping) \
	apply(receive_timeout_expect_pong)

foreach_receive_tests(build_forward_declarations)

#define build_receive_tests() \
	foreach_receive_tests(build_cmocka_unit_test)

#endif // _RECEIVE_HEADER_
