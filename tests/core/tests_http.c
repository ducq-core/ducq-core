#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>

#include "tests_http.h"

#include "ducq_http.h"

static
int _fill_request(const LargestIntegralType buffer, const LargestIntegralType request) {
	strcpy((char*)buffer, (char*)request);
	return true;
}
static
int _buffer_size(const LargestIntegralType buffer_size, const LargestIntegralType _) {
	int *size = (int*) buffer_size;
	(void) _;

	int expected_size = 1024; // websocket upgrade header can be ~512 with User-Agent and other headers
	return true;
}



void http_upgrade_to_ws(void **state) {
	// arrange
	int fd = 10;
	ducq_i *ducq = ducq_new_http_connection(fd);
	char msg[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;

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
	expect_value (mock_read,  fd,     fd);
	expect_check (mock_read,  buf,   _fill_request, request);
	expect_check (mock_read,  count, _buffer_size, NULL);
	will_return  (mock_read,  strlen(request));

	expect_value (mock_write, fd,    fd);
	expect_string(mock_write, buf,   expected_reply);
	expect_value (mock_write, count, strlen(expected_reply));
	will_return  (mock_write, strlen(expected_reply));

	// act
	ducq_state actual_state = ducq_recv(ducq, msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_non_null(ducq);

	// teardown
	ducq_free(ducq);
}

static
int _fill_incomplete_request(const LargestIntegralType buffer, const LargestIntegralType request) {
	static int count = 0;
	count++;
	switch(count) {
		case 1: 
			strcpy((char*)buffer, (char*)request);
			return true;
		default:
			count = 0;
			return true;
	}
}
void http_upgrade_to_ws_header_incomplete(void **state) {
	// arrange
	int fd = 10;
	ducq_i *ducq = ducq_new_http_connection(fd);
	char msg[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;

	ducq_state expected_state = DUCQ_EMSGINV;
	
	char request[] =
		"GET /chat HTTP/1.1\r\n"
		"Host: server.example.com\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
		"Origin: http://example.com\r\n"
		"Sec-WebSocket-Version: 13\r\n"
	//	"\r\n"
		;
	char expected_reply[] = "HTTP/1.1 400 Bad Request\r\n"
		"\r\n";
	
	// mock
	expect_value (mock_read,  fd,     fd);
	expect_check (mock_read,  buf,   _fill_request, request);
	expect_check (mock_read,  count, _buffer_size, NULL);
	will_return  (mock_read,  strlen(request));

	expect_value (mock_read,  fd,     fd);
	expect_any   (mock_read,  buf);
	expect_any   (mock_read,  count);
	will_return  (mock_read,  0);

	expect_value (mock_write, fd,    fd);
	expect_string(mock_write, buf,   expected_reply);
	expect_value (mock_write, count, strlen(expected_reply));
	will_return  (mock_write, strlen(expected_reply));

	// act
	ducq_state actual_state = ducq_recv(ducq, msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}


