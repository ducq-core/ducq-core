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

extern char READ_BUFFER[];
extern int  pos;

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




void ws_recv_client_ok(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_OK;
	char   expected_message[] = "Hello";
	size_t expected_size = strlen(expected_message);
	
	// mock
	char   message[] = { 0x81, 0x05, 0x48, 0x65, 0x6c, 0x6c, 0x6f };
	size_t size = sizeof(message);
	memcpy(READ_BUFFER, message, sizeof(message));
	pos = 0;

	will_return(readn, 2);
	will_return(readn, 5);

	// act
	ducq_i *ducq = ducq_new_ws_client("path", "port");
	char   actual_message[DUCQ_MSGSZ] = "";
	size_t actual_size =  DUCQ_MSGSZ;

	ducq_state actual_state = ducq_recv(ducq, actual_message, &actual_size);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_string_equal(expected_message, actual_message);
	assert_int_equal(expected_size, actual_size);

	// teardown
	ducq_free(ducq);
}

void ws_recv_server_ok(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_OK;
	char   expected_message[] = "Hello";
	size_t expected_size = strlen(expected_message);
	
	// mock
	char   message[] = { 0x81, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58 };
	size_t size = sizeof(message);
	memcpy(READ_BUFFER, message, sizeof(message));
	pos = 0;

	will_return(readn, 2);
	will_return(readn, 4);
	will_return(readn, 5);

	// act
	ducq_i *ducq = ducq_new_ws_connection(-1);
	char   actual_message[DUCQ_MSGSZ] = "";
	size_t actual_size =  DUCQ_MSGSZ;
	ducq_state actual_state = ducq_recv(ducq, actual_message, &actual_size);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_string_equal(expected_message, actual_message);
	assert_int_equal(expected_size, actual_size);

	// teardown
	ducq_free(ducq);
}

void ws_recv_message_read_error(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EREAD;
	
	// mock
	char   message[] = { 0x81, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58 };
	size_t size = sizeof(message);
	memcpy(READ_BUFFER, message, sizeof(message));
	pos = 0;

	will_return(readn,  2);
	will_return(readn,  4);
	will_return(readn, -1);

	// act
	ducq_i *ducq = ducq_new_ws_connection(-1);
	char   actual_message[DUCQ_MSGSZ] = "";
	size_t actual_size =  DUCQ_MSGSZ;
	ducq_state actual_state = ducq_recv(ducq, actual_message, &actual_size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void ws_recv_mask_read_error(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EREAD;
	
	// mock
	char   message[] = { 0x81, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58 };
	size_t size = sizeof(message);
	memcpy(READ_BUFFER, message, sizeof(message));
	pos = 0;

	will_return(readn,  2);
	will_return(readn, -1);

	// act
	ducq_i *ducq = ducq_new_ws_connection(-1);
	char   actual_message[DUCQ_MSGSZ] = "";
	size_t actual_size =  DUCQ_MSGSZ;
	ducq_state actual_state = ducq_recv(ducq, actual_message, &actual_size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}
void ws_recv_header_read_error(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EREAD;
	
	// mock
	char   message[] = { 0x81, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58 };
	size_t size = sizeof(message);
	memcpy(READ_BUFFER, message, sizeof(message));
	pos = 0;

	will_return(readn, -1);

	// act
	ducq_i *ducq = ducq_new_ws_connection(-1);
	char   actual_message[DUCQ_MSGSZ] = "";
	size_t actual_size =  DUCQ_MSGSZ;
	ducq_state actual_state = ducq_recv(ducq, actual_message, &actual_size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void ws_recv_buffer_too_small(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EMSGSIZE;
	char   expected_message[] = "Hello";
	size_t expected_size = strlen(expected_message);
	
	// mock
	char   message[] = { 0x81, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58 };
	size_t size = sizeof(message);
	memcpy(READ_BUFFER, message, sizeof(message));
	pos = 0;

	will_return(readn, 2);
	will_return(readn, 4);

	// act
	ducq_i *ducq = ducq_new_ws_connection(-1);
	char   actual_message[3] = "";
	size_t actual_size =  3; // too small
	ducq_state actual_state = ducq_recv(ducq, actual_message, &actual_size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}
