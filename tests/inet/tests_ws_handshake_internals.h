
#ifndef _WEBSOCKET_HANDSHAKE_INTERNALS_TESTS_HEADER_
#define _WEBSOCKET_HANDSHAKE_INTERNALS_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_websocket_handshake_internals_tests(apply) \
	apply(connection_base64_null_param)                \
	apply(connection_base64_out_len)                   \
	apply(accept_sha1_null)                            \
	apply(accept_sha1_inputTooLong)                    \
	apply(accept_sha1_state)

foreach_websocket_handshake_internals_tests(build_forward_declarations)


#define build_websocket_handshake_internals_tests() \
	foreach_websocket_handshake_internals_tests(build_cmocka_unit_test)


#endif // _WEBSOCKET_HANDSHALE_TINTERNALS_STS_HEADER_
