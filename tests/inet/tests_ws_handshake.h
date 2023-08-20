#ifndef _WEBSOCKET_HANDSHAKE_TESTS_HEADER_
#define _WEBSOCKET_HANDSHAKE_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_websocket_handshake_tests(apply)             \
	apply(base64_ok)                                     \
	apply(handshake_accept_key)                          \
	apply(handshake_connection_key_len)                  \
	apply(handshake_connection_key_null_param)           \
	apply(handshake_connection_key_buffer_too_short)     \
	apply(handshake_accept_key_len)                      \
	apply(handshake_accept_key_null_param)               \
	apply(handshake_accept_key_connection_key_too_short) \
	apply(handshake_accept_key_connection_key_too_long)  \
	apply(handshake_accept_key_out_buffer_too_small)

foreach_websocket_handshake_tests(build_forward_declarations)


#define build_websocket_handshake_tests() \
	foreach_websocket_handshake_tests(build_cmocka_unit_test)


#endif // _WEBSOCKET_HANDSHALE_TESTS_HEADER_
