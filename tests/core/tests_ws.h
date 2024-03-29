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
	apply(ws_recv_buffer_too_small) \
	apply(ws_recv_garantees_null_terminated) \
	apply(ws_send_client_ok) \
	apply(ws_send_server_ok) \
	apply(ws_send_client_message_too_big) \
	apply(ws_send_header_write_fail) \
	apply(ws_send_payload_write_fail) \
	apply(ws_send_reorder_len_16) \
	apply(ws_send_reorder_len_64) \
	apply(ws_send_reset_header) \
	apply(ws_close_server) \
	apply(ws_close_client) \
	apply(ws_close_server_init_by_client) \
	apply(ws_close_client_init_by_server) \
	apply(ws_close_wait_max_3_reads) \
	apply(ws_server_recv_ping_send_pong)

foreach_ws_tests(build_forward_declarations)

#define build_ws_tests() \
	foreach_ws_tests(build_cmocka_unit_test) 

#endif // _WS_TESTS_HEADER_
