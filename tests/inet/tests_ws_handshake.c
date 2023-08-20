#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h> // snprintf

#include "inet_ws.h"
#include "base64.h"


void base64_ok(void **state) {
	// arrange
	byte_t nonce[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10 };
	char expected_key[] = "AQIDBAUGBwgJCgsMDQ4PEA==";

	// act
	char actual_key[100] = "";
	int b64_err = base64_encode(nonce, sizeof(nonce), actual_key, sizeof(actual_key));

	// audit
	assert_string_equal(expected_key, actual_key);
}

void handshake_accept_key(void **state) {
	// arrange
	unsigned char client_key[]    = "dGhlIHNhbXBsZSBub25jZQ==";
	unsigned char expected_key[]  = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";
	unsigned char actual_key[100] = "";

	// act
	ws_error_t err = ws_make_accept_key(client_key, actual_key, sizeof(actual_key));

	// audit
	assert_int_equal(err, WS_OK);
	assert_string_equal(expected_key, actual_key);
}


void handshake_connection_key_len(void **state) {
	// arrange
	unsigned char connection_key[100] = "";
	size_t expected_len = WS_B64_CONNECTION_KEY_LEN - 1; // without null

	// act
	int seed = 1;
	ws_error_t err = ws_make_connection_key(seed, connection_key, sizeof(connection_key));
	size_t actual_len = strlen(connection_key);

	// audit
	assert_int_equal(expected_len, actual_len);
}

void handshake_connection_key_null_param(void **state) {
	// arrange
	int seed = 1;
	unsigned char *connection_key = NULL;
	
	ws_error_t expected_error = WS_NULL_PARAM;

	// act
	ws_error_t actual_error = ws_make_connection_key(seed, connection_key, sizeof(connection_key));

	// audit
	assert_int_equal(expected_error, actual_error);
}

void handshake_connection_key_buffer_too_short(void **state) {
	// arrange
	int seed = 1;
	unsigned char connection_key[WS_B64_CONNECTION_KEY_LEN-1] = "";
	ws_error_t expected_error = WS_BASE64_OUT_LEN;

	// act
	ws_error_t actual_error = ws_make_connection_key(seed, connection_key, sizeof(connection_key));

	// audit
	assert_int_equal(expected_error, actual_error);
}

void handshake_accept_key_len(void **state) {
	// arrange
	unsigned char client_key[]    = "dGhlIHNhbXBsZSBub25jZQ==";
	unsigned char accept_key[100] = "";
	size_t expected_len = WS_B64_ACCEPT_KEY_LEN - 1; // without null

	// act
	ws_make_accept_key(client_key, accept_key, sizeof(accept_key));
	size_t actual_len = strlen(accept_key);

	// audit
	assert_int_equal(expected_len, actual_len);
}

void handshake_accept_key_null_param(void **state) {
	// arrange
	unsigned char buffer[1] = "";
	ws_error_t expected_error = WS_NULL_PARAM;

	unsigned char *client_key = NULL;
	unsigned char *accept_key = NULL;

	// act && audit
	ws_error_t actual_error = WS_OK;

	client_key = NULL;
	accept_key = buffer;
	actual_error = ws_make_accept_key(client_key, accept_key, sizeof(accept_key));
	assert_int_equal(expected_error, actual_error);

	client_key = buffer;
	accept_key = NULL;
	actual_error = ws_make_accept_key(client_key, accept_key, sizeof(accept_key));
	assert_int_equal(expected_error, actual_error);
}

void handshake_accept_key_connection_key_too_short(void **state) {
	// arrange
	unsigned char client_key[] = "12345678901234567890123";
	unsigned char accept_key[WS_B64_ACCEPT_KEY_LEN] = "";
	ws_error_t expected_error = WS_IN_LEN;

	// act
	ws_error_t actual_error = ws_make_accept_key(client_key, accept_key, sizeof(accept_key));

	// audit
	assert_int_equal(strlen(client_key), WS_B64_CONNECTION_KEY_LEN - 2); // minus 1, minus NULL
	assert_int_equal(expected_error, actual_error);

}
void handshake_accept_key_connection_key_too_long(void **state) {
	// arrange
	unsigned char client_key[] = "12345678901234567890123456";
	unsigned char accept_key[WS_B64_ACCEPT_KEY_LEN] = "";
	ws_error_t expected_error = WS_IN_LEN;

	// act
	ws_error_t actual_error = ws_make_accept_key(client_key, accept_key, sizeof(accept_key));

	// audit
	assert_int_equal(strlen(client_key), WS_B64_CONNECTION_KEY_LEN + 1); // plus 1
	assert_int_equal(expected_error, actual_error);
}


void handshake_accept_key_out_buffer_too_small(void **state) {
	// arrange
	unsigned char client_key[]    = "dGhlIHNhbXBsZSBub25jZQ==";
	unsigned char accept_key[WS_B64_CONNECTION_KEY_LEN - 1] = "";
	ws_error_t expected_error = WS_BASE64_OUT_LEN;

	// act
	ws_error_t actual_error = ws_make_accept_key(client_key, accept_key, sizeof(accept_key));

	// audit
	assert_int_equal(expected_error, actual_error);
}
