#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "inet_ws.h"
#include "sha1.h"
#include "base64.h"



void connection_base64_null_param(void **state) {
	// arrange
	int seed = 1;
	char key[WS_B64_CONNECTION_KEY_LEN] = "";

	ws_error_t expected_error = WS_BASE64_NULL;
	will_return(base64_encode, -1);

	// act
	ws_error_t actual_error = ws_make_connection_key(seed, key, sizeof(key));

	// audit
	assert_int_equal(expected_error, actual_error);
}

void connection_base64_out_len(void **state) {
	// arrange
	int seed = 1;
	char key[WS_B64_CONNECTION_KEY_LEN] = "";
	
	ws_error_t expected_error = WS_BASE64_OUT_LEN;
	will_return(base64_encode, -2);

	// act
	ws_error_t actual_error = ws_make_connection_key(seed, key, sizeof(key));

	// audit
	assert_int_equal(expected_error, actual_error);
}


void accept_sha1_null(void **state) {
	// arrange
	char client_key[WS_B64_CONNECTION_KEY_LEN] = "dGhlIHNhbXBsZSBub25jZQ==";
	char accept_key[WS_B64_ACCEPT_KEY_LEN]     = "";
	
	ws_error_t expected_error = WS_SHA1_NULL;
	will_return(SHA1Reset, shaNull);

	// act
	ws_error_t actual_error = ws_make_accept_key(client_key, accept_key, sizeof(accept_key));

	// audit
	assert_int_equal(expected_error, actual_error);
}

void accept_sha1_inputTooLong(void **state) {
	// arrange
	char client_key[WS_B64_CONNECTION_KEY_LEN] = "dGhlIHNhbXBsZSBub25jZQ==";
	char accept_key[WS_B64_ACCEPT_KEY_LEN]     = "";

	ws_error_t expected_error = WS_SHA1_IN_LEN;
	will_return_always(SHA1Reset, shaSuccess);
	will_return(SHA1Input, shaInputTooLong);

	// act
	ws_error_t actual_error = ws_make_accept_key(client_key, accept_key, sizeof(accept_key));

	// audit
	assert_int_equal(expected_error, actual_error);
}

void accept_sha1_state(void **state) {
	// arrange
	char client_key[WS_B64_CONNECTION_KEY_LEN] = "dGhlIHNhbXBsZSBub25jZQ==";
	char accept_key[WS_B64_ACCEPT_KEY_LEN]     = "";


	ws_error_t expected_error = WS_SHA1_STATE;
	will_return_always(SHA1Reset, shaSuccess);
	will_return_always(SHA1Input, shaSuccess);
	will_return(SHA1Result, shaStateError);

	// act
	ws_error_t actual_error = ws_make_accept_key(client_key, accept_key, sizeof(accept_key));

	// audit
	assert_int_equal(expected_error, actual_error);
}
