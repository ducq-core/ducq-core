#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "inet_ws.h"

void sizeof_mask(void **state) {
	// audit
	assert_int_equal( sizeof(ws_mask_t), sizeof(uint32_t) );
}


void first_two_bytes_1(void **state) {
	// arrange
	unsigned char expected[] = {0x81, 0x05};

	// act
	ws_header_t hdr = {};
	hdr[0] = WS_FIN | WS_TEXT;
	ws_set_len(hdr, 5);

	// audit
	unsigned char *actual = (unsigned char*) hdr;
	assert_int_equal(expected[0], actual[0]);
	assert_int_equal(expected[1], actual[1]);
}

void first_two_bytes_2(void **state) {
	// arrange
	unsigned char expected[] = {0x81, 0x85};

	// act
	ws_header_t hdr = {};
	hdr[0] = WS_FIN | WS_TEXT;
	hdr[1] = WS_MASK;
	ws_set_len(hdr, 5);

	// audit
	unsigned char *actual = (unsigned char*) hdr;
	assert_int_equal(expected[0], actual[0]);
	assert_int_equal(expected[1], actual[1]);
}

void first_two_bytes_3(void **state) {
	// arrange
	unsigned char expected[] = {0x01, 0x85};

	// act
	ws_header_t hdr = {};
	hdr[0] = WS_TEXT;
	hdr[1] = WS_MASK;
	ws_set_len(hdr, 5);

	// audit
	unsigned char *actual = (unsigned char*) hdr;
	assert_int_equal(expected[0], actual[0]);
	assert_int_equal(expected[1], actual[1]);
}

void no_mask_small_len(void **state) {
	// arrange
	unsigned char expected[WS_HEADER_SIZE] = {0x81, 0x05};
	uint64_t expected_len = 5;

	// act
	ws_header_t hdr = {};
	hdr[0] = WS_FIN | WS_TEXT;

	ws_set_len(hdr, expected_len);
	uint64_t actual_len = ws_get_len(hdr);

	// audit
	unsigned char *actual = (unsigned char*) hdr;
	assert_memory_equal(expected, actual, WS_HEADER_SIZE);
	assert_int_equal(expected_len, actual_len);
	assert_false(hdr[1] & WS_MASK);
}

void no_mask_256_len(void **state) {
	// arrange
	unsigned char expected[WS_HEADER_SIZE] = {0x81, 0x7e, 0x01, 0x00};
	uint64_t expected_len = 256;

	// act
	ws_header_t hdr = {};
	hdr[0] = WS_FIN | WS_TEXT;
	
	ws_set_len(hdr, expected_len);
	uint64_t actual_len = ws_get_len(hdr);
	
	// audit
	unsigned char *actual = (unsigned char*) hdr;
	assert_memory_equal(expected, actual, WS_HEADER_SIZE);
	assert_int_equal(expected_len, actual_len);
	assert_false(hdr[1] & WS_MASK);
}

void no_mask_65536_len(void **state) {
	// arrange
	unsigned char expected[WS_HEADER_SIZE] = {0x81, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
	uint64_t expected_len = 65536;

	// act
	ws_header_t hdr = {};
	hdr[0] = WS_FIN | WS_TEXT;
	
	ws_set_len(hdr, expected_len);
	uint64_t actual_len = ws_get_len(hdr);

	// audit
	unsigned char *actual = (unsigned char*) hdr;
	assert_memory_equal(expected, actual, WS_HEADER_SIZE);
	assert_int_equal(expected_len, actual_len);
	assert_false(hdr[1] & WS_MASK);
}


void with_mask_small_len(void **state) {
	// arrange
	unsigned char expected[WS_HEADER_SIZE] = {0x81, 0x85, 0x01, 0x02, 0x03, 0x04};
	ws_mask_t expected_mask = {.bytes = {0x01, 0x02, 0x03, 0x04} };
	uint64_t expected_len = 5;

	// act
	ws_header_t hdr = {};
	hdr[0] = WS_FIN | WS_TEXT;
	ws_set_len(hdr, expected_len);
	ws_set_msk(hdr, &expected_mask);

	uint64_t actual_len = ws_get_len(hdr);
	ws_mask_t *actual_mask = ws_get_msk(hdr);
	
	// audit
	unsigned char *actual = (unsigned char*) hdr;
	assert_memory_equal(expected, actual, WS_HEADER_SIZE);
	assert_int_equal(expected_len, actual_len);
	assert_memory_equal(&expected_mask.bytes, actual_mask->bytes, sizeof(ws_mask_t));
	assert_int_equal(expected_mask.u32, actual_mask->u32);
	assert_true(hdr[1] & WS_MASK);
}

void with_mask_256_len(void **state) {
	// arrange
	unsigned char expected[WS_HEADER_SIZE] = {0x81, 0xfe, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04};
	ws_mask_t expected_mask = {.bytes = {0x01, 0x02, 0x03, 0x04} };
	uint64_t expected_len = 256;

	// act
	ws_header_t hdr = {};
	hdr[0] = WS_FIN | WS_TEXT;
	ws_set_len(hdr, expected_len);
	ws_set_msk(hdr, &expected_mask);

	uint64_t actual_len = ws_get_len(hdr);
	ws_mask_t *actual_mask = ws_get_msk(hdr);

 	// audit
	unsigned char *actual = (unsigned char*) hdr;
	assert_memory_equal(expected, actual, WS_HEADER_SIZE);
	assert_int_equal(expected_len, actual_len);
	assert_memory_equal(&expected_mask.bytes, actual_mask->bytes, sizeof(ws_mask_t));
	assert_int_equal(expected_mask.u32, actual_mask->u32);
	assert_true(hdr[1] & WS_MASK);
}

void with_mask_65536_len(void **state) {
	// arrange
	unsigned char expected[WS_HEADER_SIZE] = {0x81, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04};
	ws_mask_t expected_mask = {.bytes = {0x01, 0x02, 0x03, 0x04} };
	uint64_t expected_len = 65536;

	// act
	ws_header_t hdr = {};
	hdr[0] = WS_FIN | WS_TEXT;
	ws_set_len(hdr, expected_len);
	ws_set_msk(hdr, &expected_mask);

	uint64_t actual_len = ws_get_len(hdr);
	ws_mask_t *actual_mask = ws_get_msk(hdr);
	
	// audit
	unsigned char *actual = (unsigned char*) hdr;
	assert_memory_equal(expected, actual, WS_HEADER_SIZE);
	assert_int_equal(expected_len, actual_len);
	assert_memory_equal(&expected_mask.bytes, actual_mask->bytes, sizeof(ws_mask_t));
	assert_int_equal(expected_mask.u32, actual_mask->u32);
	assert_true(hdr[1] & WS_MASK);
}

void reset_mask_256_len(void **state) {
	// arrange
	unsigned char expected[WS_HEADER_SIZE] = {0x81, 0xfe, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04};
	ws_mask_t expected_mask = {.bytes = {0x01, 0x02, 0x03, 0x04} };
	uint64_t expected_len = 256;

	// act
	ws_header_t hdr = {};
	hdr[0] = WS_FIN | WS_TEXT;

	ws_mask_t old_mask = {.bytes = {0xAA, 0xBB, 0xCC, 0xDD} };
	ws_set_msk(hdr, &old_mask);
	ws_set_len(hdr,  expected_len);
	ws_set_msk(hdr, &expected_mask);

	uint64_t actual_len    = ws_get_len(hdr);
	ws_mask_t *actual_mask = ws_get_msk(hdr);

	// audit
	unsigned char *actual = (unsigned char*) hdr;
	assert_memory_equal(expected, actual, WS_HEADER_SIZE);
	assert_int_equal(expected_len, actual_len);
	assert_memory_equal(&expected_mask.bytes, actual_mask->bytes, sizeof(ws_mask_t));
	assert_int_equal(expected_mask.u32, actual_mask->u32);
	assert_true(hdr[1] & WS_MASK);
}

void with_mask_reset_256_len(void **state) {
	// arrange
	unsigned char expected[WS_HEADER_SIZE] = {0x81, 0xfe, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04};
	ws_mask_t expected_mask = {.bytes = {0x01, 0x02, 0x03, 0x04} };
	uint64_t expected_len = 256;

	// act
	ws_header_t hdr = {};
	hdr[0] = WS_FIN | WS_TEXT;

	ws_set_len(hdr, 1);
	ws_set_msk(hdr, &expected_mask);
	ws_set_len(hdr,  expected_len);
	
	uint64_t actual_len    = ws_get_len(hdr);
	ws_mask_t *actual_mask = ws_get_msk(hdr);
	
	// audit
	unsigned char *actual = (unsigned char*) hdr;
	assert_memory_equal(expected, actual, WS_HEADER_SIZE);
	assert_int_equal(expected_len, actual_len);
	assert_memory_equal(&expected_mask.bytes, actual_mask->bytes, sizeof(ws_mask_t));
	assert_int_equal(expected_mask.u32, actual_mask->u32);
	assert_true(hdr[1] & WS_MASK);
}

void mask_hello(void **state) {
	// see rfc6455, 5.7, second example.
	// arrange
	unsigned char expected_msg[] = {0x7f, 0x9f, 0x4d, 0x51, 0x58};
	unsigned char actual_msg[] = "Hello";
	
	ws_mask_t mask = { .bytes = {0x37, 0xfa, 0x21, 0x3d} };

	ws_header_t hdr = {0x00, WS_MASK};

	// act
	ws_set_msk(hdr, &mask);
	ws_mask_message(ws_get_msk(hdr), actual_msg, strlen(actual_msg)); 

	// audit
	assert_memory_equal(expected_msg, actual_msg, sizeof(expected_msg));
}



void get_hdr_len_empty(void **state) {
	// arrange
	ws_header_t hdr = {};
	size_t expected_len = 2;

	// act
	size_t actual_len = ws_get_hdr_len(hdr);

	// audit
	assert_int_equal(expected_len, actual_len);
}
void get_hdr_len_with_mask(void **state) {
	// arrange
	ws_header_t hdr = {};
	size_t expected_len = 2 + sizeof(ws_mask_t);

	ws_mask_t mask = {};
	ws_set_msk(hdr, &mask);

	// act
	size_t actual_len = ws_get_hdr_len(hdr);

	// audit
	assert_int_equal(expected_len, actual_len);
}
void get_hdr_len_16bits(void **state) {
	// arrange
	ws_header_t hdr = {};
	size_t expected_len = 2 + sizeof(uint16_t);

	ws_set_len(hdr, 128);

	// act
	size_t actual_len = ws_get_hdr_len(hdr);

	// audit
	assert_int_equal(expected_len, actual_len);
}
void get_hdr_len_16bits_with_mask(void **state) {
	// arrange
	ws_header_t hdr = {};
	size_t expected_len = 2 + sizeof(uint16_t) + sizeof(ws_mask_t);

	ws_set_len(hdr, 128);
	ws_mask_t mask = {};
	ws_set_msk(hdr, &mask);

	// act
	size_t actual_len = ws_get_hdr_len(hdr);

	// audit
	assert_int_equal(expected_len, actual_len);
}
void get_hdr_len_64bits(void **state) {
	// arrange
	ws_header_t hdr = {};
	size_t expected_len = 2 + sizeof(uint64_t);

	ws_set_len(hdr, UINT16_MAX+1);

	// act
	size_t actual_len = ws_get_hdr_len(hdr);

	// audit
	assert_int_equal(expected_len, actual_len);
}
void get_hdr_len_64bits_with_mask(void **state) {
	// arrange
	ws_header_t hdr = {};
	size_t expected_len = 2 + sizeof(uint64_t) + sizeof(ws_mask_t);

	ws_set_len(hdr, UINT16_MAX+1);
	ws_mask_t mask = {};
	ws_set_msk(hdr, &mask);

	// act
	size_t actual_len = ws_get_hdr_len(hdr);

	// audit
	assert_int_equal(expected_len, actual_len);
}

