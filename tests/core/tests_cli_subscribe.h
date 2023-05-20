#ifndef _CLI_SUB_HEADER_
#define _CLI_SUB_HEADER_

#include "unit_tests.h"

#define foreach_cli_sub_tests(apply) \
	apply(sub_ok) \
	apply(sub_conn_error) \
	apply(sub_emit_error) \
	apply(sub_recv_error) \
	apply(sub_nack_state_returned) \
	apply(sub_close_error) \
	apply(sub_on_message_receives_param) \
	apply(sub_on_message_abort)
	
foreach_cli_sub_tests(build_forward_declarations)

#define build_cli_sub_tests() \
	foreach_cli_sub_tests(build_cmocka_unit_test)

#endif // _CLI_SUB_HEADER_