#ifndef _TCP_TESTS_HEADER_
#define _TCP_TESTS_HEADER_

#include "unit_tests.h"

#define foreach_tcp_tests(apply) \
	apply(tcp_connect_ok) \
	apply(tcp_connect_econnect) \
	apply(tcp_close_ok) \
	apply(tcp_close_eclose) \
	apply(tcp_id_ok) \
	apply(tcp_id_called_only_once) \
	apply(tcp_eq_ok) \
	apply(tcp_eq_ok_fd_not_equal) \
	apply(tcp_eq_ok_vtbl_not_equal) \
	apply(tcp_copy_is_equal) \
	apply(tcp_timeout_ok) \
	apply(tcp_timeout_err) \
	apply(tcp_send_count_ok) \
	apply(tcp_send_ewrite_header) \
	apply(tcp_send_ewrite_payload) \
	apply(tcp_recv_ok) \
	apply(tcp_recv_read_err_on_parsing_length) \
	apply(tcp_recv_connclosed_on_parsing_length) \
	apply(tcp_recv_parsed_length_stop_at_buf_size) \
	apply(tcp_recv_parsed_length_too_big_for_buffer_size) \
	apply(tcp_recv_no_endline_after_length) \
	apply(tcp_recv_msg_size_too_big_for_buffer) \
	apply(tcp_recv_read_err_on_reading_payload) \
	apply(tcp_recv_connclosed_on_reading_payload) \
	apply(tcp_recv_payload_length_smaller_than_expected) \
	apply(tcp_recv_payload_length_bigger_than_expected) \
	apply(tcp_recv_payload_size_minus_one_buffer_is_null_terminated) \
	apply(tcp_recv_payload_exactly_same_as_buffer_is_err)
	
foreach_tcp_tests(build_forward_declarations)

#define build_tcp_tests() \
	foreach_tcp_tests(build_cmocka_unit_test) 

#endif // _TCP_TESTS_HEADER_