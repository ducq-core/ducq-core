#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>

#include "tests_ws.h"

#include "inet_http.h"
#include "ducq_ws.h"


void ws_upgrade_from_http(void **state) {
	// arrange
	int fd = 4;
	ducq_state expected_state = DUCQ_PROTOCOL;
	char request[] =
		"GET /chat HTTP/1.1\r\n"
		"Host: server.example.com\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
		"Origin: http://example.com\r\n"
		"Sec-WebSocket-Version: 13\r\n"
		"\r\n";
	char expected_reply[] =
		"HTTP/1.1 101 Switching Protocols\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n"
		"\r\n";
	
	// mock
	expect_string(writen, vptr, expected_reply);
	will_return  (writen, strlen(expected_reply));

	// act
	ducq_i *ducq = NULL;
	ducq_state actual_state =
		ducq_new_ws_upgrade_from_http(&ducq, fd, request);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_non_null(ducq);

	// teardown
	ducq_free(ducq);
}
void ws_upgrade_from_http_msg_invalid_if_no_key(void **state) {
	// arrange
	int fd = 4;
	ducq_state expected_state = DUCQ_EMSGINV;
	char request[] =
		"GET /chat HTTP/1.1\r\n"
		"Host: server.example.com\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
	//	"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
		"Origin: http://example.com\r\n"
		"Sec-WebSocket-Version: 13\r\n"
		"\r\n";
	char expected_reply[] = HTTP_BAD_REQUEST_400;
	
	// mock
	expect_string(writen, vptr, expected_reply);
	will_return  (writen, strlen(expected_reply));

	// act
	ducq_i *ducq = NULL;
	ducq_state actual_state =
		ducq_new_ws_upgrade_from_http(&ducq, fd, request);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_null(ducq);

	// teardown
	ducq_free(ducq);
}

void ws_upgrade_from_http_return_ewrite(void **state) {
	// arrange
	int fd = 4;
	ducq_state expected_state = DUCQ_EWRITE;
	char request[] =
		"GET /chat HTTP/1.1\r\n"
		"Host: server.example.com\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
		"Origin: http://example.com\r\n"
		"Sec-WebSocket-Version: 13\r\n"
		"\r\n";
	
	// mock
	expect_any(writen, vptr);
	will_return  (writen, -1); // write error

	// act
	ducq_i *ducq = NULL;
	ducq_state actual_state =
		ducq_new_ws_upgrade_from_http(&ducq, fd, request);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_null(ducq);

	// teardown
	ducq_free(ducq);
}

