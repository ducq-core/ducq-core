#ifndef _WS_TESTS_HEADER_
#define _WS_TESTS_HEADER_

#include "unit_tests.h"

#define foreach_ws_tests(apply) \
	apply(ws_upgrade_from_http) \
	apply(ws_upgrade_from_http_msg_invalid_if_no_key) \
	apply(ws_upgrade_from_http_return_ewrite) \
	apply(ws_recv_client_ok) \
	apply(ws_recv_server_ok) \
	apply(ws_recv_message_read_error) \
	apply(ws_recv_mask_read_error) \
	apply(ws_recv_header_read_error) \
	apply(ws_recv_buffer_too_small)


foreach_ws_tests(build_forward_declarations)

#define build_ws_tests() \
	foreach_ws_tests(build_cmocka_unit_test) 

#endif // _WS_TESTS_HEADER_
