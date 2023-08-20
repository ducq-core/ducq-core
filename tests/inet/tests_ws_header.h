#ifndef _WEBSOCKET_HEADER_TESTS_HEADER_
#define _WEBSOCKET_HEADER_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_websocket_header_tests(apply) \
	apply(sizeof_mask)                    \
	apply(first_two_bytes_1)              \
	apply(first_two_bytes_2)              \
	apply(first_two_bytes_3)              \
	apply(no_mask_small_len)              \
	apply(no_mask_256_len)                \
	apply(no_mask_65536_len)              \
	apply(with_mask_small_len)            \
	apply(with_mask_256_len)              \
	apply(with_mask_65536_len)            \
	apply(reset_mask_256_len)             \
	apply(with_mask_reset_256_len)        \
	apply(mask_hello) 	              \
	apply(get_hdr_len_empty)              \
	apply(get_hdr_len_with_mask)          \
	apply(get_hdr_len_16bits)             \
	apply(get_hdr_len_16bits_with_mask)   \
	apply(get_hdr_len_64bits)             \
	apply(get_hdr_len_64bits_with_mask)

foreach_websocket_header_tests(build_forward_declarations)


#define build_websocket_header_tests() \
	foreach_websocket_header_tests(build_cmocka_unit_test)


#endif // _WEBSOCKET_HEADER_TESTS_HEADER_
