#ifndef _CLI_PUB_HEADER_
#define _CLI_PUB_HEADER_

#include "unit_tests.h"

#define foreach_pub_tests(apply) \
	apply(pub_ok) \
	apply(pub_conn_error) \
	apply(pub_emit_error) \
	apply(pub_recv_error) \
	apply(pub_nack_state_returned) \
	apply(pub_close_error)
	
foreach_pub_tests(build_forward_declarations)

#define build_pub_tests() \
	foreach_pub_tests(build_cmocka_unit_test)

#endif // _CLI_PUB_HEADER_