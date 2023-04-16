#ifndef _INET_TESTS_HEADER_
#define _INET_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_inet_tests(apply) \
	apply(inet_readn_mock_read_get_called) \
	apply(inet_readn_minus_one_on_error) \
	apply(inet_readn_conn_closed_return_read_so_far) \
	apply(inet_readn_continue_on_interrupt) \
	apply(inet_readn_stop_when_buffer_full) \
	apply(inet_writen_return_writen_count) \
	apply(inet_writen_minus_one_on_error) \
	apply(inet_writen_continue_on_interrup) \
	apply(inet_close_call_close) \
	apply(inet_close_returns_close_rc) \
	apply(inet_address_tostring_ipv4) \
	apply(inet_address_tostring_ipv6) \
	apply(inet_address_tostring_ipv4_buffer_too_small_for_addr) \
	apply(inet_address_tostring_ipv4_buffer_too_small_for_port) \
	apply(inet_address_tostring_ipv4_buffer_too_small_for_null_terminator) \
	apply(inet_self_socket_tostring_calls_getsockname) \
	apply(inet_self_socket_tostring_returns_null_if_getsockname_fails) \
	apply(inet_peer_socket_tostring_calls_getpeername)
	
foreach_inet_tests(build_forward_declarations)

#define build_inet_tests() \
	foreach_inet_tests(build_cmocka_unit_test)


#endif // _INET_TESTS_HEADER_