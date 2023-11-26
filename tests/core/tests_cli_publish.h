#ifndef _CLI_PUB_HEADER_
#define _CLI_PUB_HEADER_

#include "unit_tests.h"

#define foreach_cli_pub_tests(apply) \
	apply(pub_ok) \
	apply(pub_send_error) \
	apply(pub_recv_error) \
	apply(pub_nack_state_returned)

foreach_cli_pub_tests(build_forward_declarations)

#define build_cli_pub_tests() \
	foreach_cli_pub_tests(build_cmocka_unit_test)

#endif // _CLI_PUB_HEADER_
