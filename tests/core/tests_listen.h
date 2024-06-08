#ifndef _LISTEN_HEADER_
#define _LISTEN_HEADER_

#include "unit_tests.h"

#define foreach_listen_test(apply) \
	apply(listen_pass_ctx_to_callback) \
	apply(listen_pass_ctx_to_callback) \
	apply(listen_return_state) \
	apply(listen_managed_call_on_protocol) \
	apply(listen_managed_call_on_nack) \
	apply(listen_managed_call_on_error) \
	apply(listen_managed_error_on_sending_pong_call_on_error) \
	apply(listen_raw_proto_call_on_message) \
	apply(listen_raw_nack_call_on_message) \
	apply(listen_raw_error_call_on_error) \
	apply(listen_loops_until_return_is_not_zero)


foreach_listen_test(build_forward_declarations)


int listen_setup(void **state);
int listen_teardown(void **state);

#define build_listen_test_setup_teardown(elem) \
	cmocka_unit_test_setup_teardown(elem, listen_setup, listen_teardown),

#define build_listen_tests() \
	foreach_listen_test(build_listen_test_setup_teardown)


#endif // _LISTEN_HEADER_
