#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tests_ws.h"

#include "inet_http.h"
#include "inet_ws.h"
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

void ws_recv_garantees_null_terminated(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_OK;

	// mock
	char   expected_message[] = "new shorter message";
	size_t expected_size =  strlen(expected_message);
	pos = 0;
	snprintf(READ_BUFFER, DUCQ_MSGSZ, "00%s", expected_message);
	READ_BUFFER[0] = (WS_FIN|WS_TEXT);
	READ_BUFFER[1] = (unsigned)expected_size;

	will_return(readn,  2);
	will_return(readn, expected_size);


	// act
	char   actual_message[DUCQ_MSGSZ] = "1234567890 previous, garbage message to be overriden.";
	size_t actual_size =  DUCQ_MSGSZ;

	ducq_i *ducq = ducq_new_ws_client("host", "port");
	ducq_state actual_state = ducq_recv(ducq, actual_message, &actual_size);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_string_equal(expected_message, actual_message);

	// teardown
	ducq_free(ducq);
}


void ws_send_client_ok(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_OK;

	// mock
	char   expected_header[]  = { 0x81, 0x85 };
	char   expected_message[] = "Hello";
	size_t expected_header_size  =  sizeof(expected_header);
	size_t expected_message_size =  strlen(expected_message);
	
	expect_any_count(writen, vptr, 2);
	will_return  (writen, 6);
	will_return  (writen, 5);

	// act
	ducq_i *ducq = ducq_new_ws_client("path", "port");
	char   message[] = "Hello";
	size_t actual_message_size =  strlen(message);

	ducq_state actual_state = ducq_send(ducq, message, &actual_message_size);

	// audit
	assert_int_equal(expected_state, actual_state);

	assert_int_equal(expected_message_size, actual_message_size);

	ws_header_t *actual_header = (ws_header_t*) ducq_buffer(ducq);
	
	byte_t *header = ducq_buffer(ducq);
	assert_memory_equal(expected_header, header, expected_header_size);
	
	char *actual_message = header + sizeof(ws_header_t);
	assert_memory_not_equal(expected_message, actual_message, expected_message_size);
	ws_mask_t *mask = ws_get_msk(header);
	ws_mask_message(mask, actual_message, actual_message_size);
	assert_memory_equal(expected_message, actual_message, expected_message_size);


	// teardown
	ducq_free(ducq);
}

void ws_send_server_ok(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_OK;

	// mock
	char   expected_header[]  = { 0x81, 0x05 };
	char   expected_message[] = { 0x48, 0x65, 0x6c, 0x6c, 0x6f };
	size_t expected_header_size  =  sizeof(expected_header);
	size_t expected_message_size =  sizeof(expected_message);
	
	expect_memory(writen, vptr, expected_header,  expected_header_size);
	expect_memory(writen, vptr, expected_message, expected_message_size);
	will_return  (writen, expected_header_size);
	will_return  (writen, expected_message_size);

	// act
	ducq_i *ducq = ducq_new_ws_connection(-1);
	char   message[] = "Hello";
	size_t message_size =  strlen(message);

	ducq_state actual_state = ducq_send(ducq, message, &message_size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}
void ws_send_client_message_too_big(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EMSGSIZE;

	// act
	ducq_i *ducq = ducq_new_ws_client("path", "port");
	char   message[DUCQ_MSGSZ+1] = ""; // too big
	size_t message_size =  sizeof(message);
	
	ducq_state actual_state = ducq_send(ducq, message, &message_size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void ws_send_header_write_fail(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EWRITE;

	// mock
	expect_any   (writen, vptr);
	will_return  (writen, -1);

	// act
	ducq_i *ducq = ducq_new_ws_connection(-1);
	char   message[] = "Hello";
	size_t message_size =  strlen(message);

	ducq_state actual_state = ducq_send(ducq, message, &message_size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void ws_send_payload_write_fail(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EWRITE;

	// mock
	expect_any_count(writen, vptr, 2);
	will_return     (writen,  2);
	will_return     (writen, -1);

	// act
	ducq_i *ducq = ducq_new_ws_connection(-1);
	char   message[] = "Hello";
	size_t message_size =  strlen(message);

	ducq_state actual_state = ducq_send(ducq, message, &message_size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void ws_send_reorder_len_16(void **state) {
	// arrange
#define TEST_LEN_BITS 0x1234U
	uint16_t test_int = TEST_LEN_BITS;
	if( WS_IS_NETWORK_BYTE_ORDER )
		skip();

	ducq_state expected_state = DUCQ_OK;
	byte_t expected_header[] = { 
		(WS_FIN | WS_TEXT),
		126,         // len on 16 bits, no mask
		0x12U, 0x34U
	};
	char message[TEST_LEN_BITS] = "";
	size_t message_size = sizeof(message);
	memset(message, 0, TEST_LEN_BITS);

	// mock
	expect_memory(writen, vptr, expected_header, sizeof(expected_header));
	will_return  (writen, sizeof(expected_header));
	expect_memory(writen, vptr, message, sizeof(message));
	will_return  (writen, sizeof(message));

	// act
	ducq_i *ducq = ducq_new_ws_connection(-1);
	ducq_state actual_state = ducq_send(ducq, message, &message_size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
#undef TEST_LEN_BITS
}

void ws_send_reorder_len_64(void **state) {
	printf("valgrind warning ok for this test, but test should pass.\n");
	// arrange
#define TEST_LEN_BITS 0x12345678U
	uint64_t test_int = TEST_LEN_BITS;
	if( WS_IS_NETWORK_BYTE_ORDER )
		skip();

	ducq_state expected_state = DUCQ_OK;
	byte_t expected_header[] = { 
		(WS_FIN | WS_TEXT),
		127,         // len on 64 bits, no mask
		0x00U, 0x00U, 0x00U, 0x00U,
		0x12U, 0x34U, 0x56U, 0x78U
	};
	char *message = malloc(TEST_LEN_BITS);
	size_t message_size = TEST_LEN_BITS;
	memset(message, 0, TEST_LEN_BITS);


	// mock
	expect_memory(writen, vptr, expected_header, sizeof(expected_header));
	will_return  (writen, sizeof(expected_header));
	expect_any(writen, vptr);
	will_return  (writen, TEST_LEN_BITS);

	// act
	ducq_i *ducq = ducq_new_ws_connection(-1);
	ducq_state actual_state = ducq_send(ducq, message, &message_size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
	free(message);
#undef TEST_LEN_BITS
}



void ws_send_reset_header(void **state) {
	// arrange
	char send_message[] = "sent_message";
	size_t send_size = strlen(send_message);

	byte_t expected_send_header[] = { (WS_FIN|WS_TEXT), send_size } ;

	// mock
	pos = 0;
	snprintf(READ_BUFFER, DUCQ_MSGSZ, "HEADER1234567890");
	READ_BUFFER[0] = WS_FIN | WS_TEXT;
	READ_BUFFER[1] = WS_MASK | 10U;
	READ_BUFFER[2] = 'a';
	READ_BUFFER[3] = 'b';
	READ_BUFFER[4] = 'c';
	READ_BUFFER[5] = 'd';

	will_return  (readn,  2); // header
	will_return  (readn,  4); // mask
	will_return  (readn, 10); // message

	expect_memory(writen, vptr, expected_send_header, 2);
	expect_any   (writen, vptr);
	will_return  (writen, 2);
	will_return  (writen, send_size);

	// act
	ducq_i *ducq = ducq_new_ws_connection(-1);
	char recv_buffer[DUCQ_MSGSZ] = "";
	size_t recv_size = DUCQ_MSGSZ;
	ducq_recv(ducq, recv_buffer, &recv_size);

	ducq_send(ducq, send_message, &send_size);

	// audit
	// (in mock)

	// teardown
	ducq_free(ducq);
}



void ws_close_server(void **state) {
	// arrange
	int fd = -1;
	ducq_state expected_state  = DUCQ_OK;
	uint16_t expected_status = WS_CLOSE_GOING_AWAY;
	if( ! WS_IS_NETWORK_BYTE_ORDER)
		expected_status = ws_reorder_16(expected_status);
	
	byte_t *expected_message = (byte_t*) &expected_status;
	byte_t expected_header[] = {
		(WS_FIN | WS_CLOSE),
		sizeof(uint16_t),
		expected_message[0],
		expected_message[1]
	};


	// mock
	expect_memory(writen, vptr, expected_header,     2 );
	expect_memory(writen, vptr, expected_header + 2, 2 );
	will_return  (writen, 2);
	will_return  (writen, 2);


	will_return(readn, 10); // client ws  close frame
	will_return(readn,  0); // client tcp close connection

	will_return(inet_set_read_timeout, 0);
	expect_value(inet_shutdown_write, fd, fd);
	will_return(inet_shutdown_write, 0);
	will_return(inet_close, 0);


	// act
	ducq_i *ducq = ducq_new_ws_connection(fd);
	ducq_state actual_state = ducq_close(ducq);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

struct check_message_ctx {
	byte_t *actual_hdr;
	byte_t *expected_hdr;
	int expected_hdr_size;
	byte_t *expected_msg;
	int expected_msg_size;
	int count;
};
static
int _check_masked_message(LargestIntegralType param, LargestIntegralType ctx) {	
	struct check_message_ctx *msg = (struct check_message_ctx*) ctx;

	if(msg->count == 0) {
		byte_t *actual_hdr = (byte_t*)param;
		size_t size = ws_get_hdr_len(actual_hdr);

		assert_int_equal(msg->expected_hdr_size, size);

		assert_memory_equal(
			msg->expected_hdr,
			actual_hdr,
			2	// don't check random mask
		);
		msg->actual_hdr = actual_hdr;
		msg->count++;
	}
	else {
		byte_t *actual_msg = (byte_t*)param;
		size_t size = ws_get_len(msg->actual_hdr);

		assert_int_equal(msg->expected_msg_size, size);

		ws_mask_message(
			ws_get_msk(msg->actual_hdr),
			actual_msg,
			size
		);
		assert_memory_equal(
			msg->expected_msg,
			actual_msg,
			msg->expected_msg_size
		);
	} 	

	return true;
}
void ws_close_client(void **state) {
	// arrange
	int fd = -1;
	ducq_state expected_state  = DUCQ_OK;
	uint16_t expected_status = WS_CLOSE_NORMAL;
	if( ! WS_IS_NETWORK_BYTE_ORDER)
		expected_status = ws_reorder_16(expected_status);
	
	byte_t *expected_message = (byte_t*) &expected_status;
	byte_t expected_header[] = {
		(WS_FIN | WS_CLOSE),
		(WS_MASK | sizeof(uint16_t)),
		0x00, 0x00, 0x00, 0x00, // mask placeholder 
		expected_message[0],
		expected_message[1]
	};

	// mock
	struct check_message_ctx ctx = {
		.count = 0,
		.expected_hdr = expected_header,
		.expected_hdr_size = 6,
		.expected_msg = expected_message,
		.expected_msg_size = 2
	};
	expect_check(writen, vptr, _check_masked_message, &ctx);
	expect_check(writen, vptr, _check_masked_message, &ctx);
	will_return (writen, 6);
	will_return (writen, 2);


	will_return(readn, 10); // server ws  close frame
	will_return(readn,  0); // server tcp close connection

	will_return(inet_set_read_timeout, 0);
	expect_value(inet_shutdown_write, fd, fd);
	will_return(inet_shutdown_write, 0);
	will_return(inet_close, 0);


	// act
	ducq_i *ducq = ducq_new_ws_client("host", "port");
	ducq_state actual_state = ducq_close(ducq);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void ws_close_server_init_by_client(void **state) {
	// arrange
	int fd = -1;
	ducq_state expected_state  = DUCQ_OK;
	uint16_t expected_status = WS_CLOSE_NORMAL;
	if( ! WS_IS_NETWORK_BYTE_ORDER)
		expected_status = ws_reorder_16(expected_status);
	
	byte_t *expected_message = (byte_t*) &expected_status;
	byte_t expected_header[] = {
		(WS_FIN | WS_CLOSE),
		sizeof(uint16_t),
		expected_message[0],
		expected_message[1]
	};


	// mock
	expect_memory(writen, vptr, expected_header,     2 );
	expect_memory(writen, vptr, expected_header + 2, 2 );
	will_return  (writen, 2);
	will_return  (writen, 2);

	byte_t client_close_frame[] = {
		(WS_FIN | WS_CLOSE),
		(WS_MASK | sizeof(uint16_t)),
		0x00, 0x00, 0x00, 0x00, // mask placeholder 
		expected_message[0],
		expected_message[1]
	};
	pos = 0;
	memcpy(READ_BUFFER, client_close_frame, sizeof(client_close_frame));
	will_return(readn,  2); // client ws  close frame
	will_return(readn,  4); // client ws  close frame
	will_return(readn,  2); // client ws  close frame

	will_return(inet_close, 0);


	// act
	char buffer[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;

	ducq_i *ducq = ducq_new_ws_connection(fd);
	ducq_state recv_state   = ducq_recv(ducq, buffer, &size);
	ducq_state actual_state = ducq_close(ducq);

	// audit
	assert_int_equal(DUCQ_ECONNCLOSED, recv_state);
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}
void ws_close_client_init_by_server(void **state) {
	// arrange
	int fd = -1;
	ducq_state expected_state  = DUCQ_OK;
	uint16_t expected_status = WS_CLOSE_NORMAL;
	if( ! WS_IS_NETWORK_BYTE_ORDER)
		expected_status = ws_reorder_16(expected_status);
	
	byte_t *expected_message = (byte_t*) &expected_status;
	byte_t expected_header[] = {
		(WS_FIN | WS_CLOSE),
		(WS_MASK | sizeof(uint16_t)),
		0x00, 0x00, 0x00, 0x00, // mask placeholder 
		expected_message[0],
		expected_message[1]
	};

	// mock
	struct check_message_ctx ctx = {
		.count = 0,
		.expected_hdr = expected_header,
		.expected_hdr_size = 6,
		.expected_msg = expected_message,
		.expected_msg_size = 2
	};
	expect_check(writen, vptr, _check_masked_message, &ctx);
	expect_check(writen, vptr, _check_masked_message, &ctx);
	will_return (writen, 6);
	will_return (writen, 2);


	byte_t server_close_frame[] = {
		(WS_FIN | WS_CLOSE),
		sizeof(uint16_t),
		expected_message[0],
		expected_message[1]
	};
	pos = 0;
	memcpy(READ_BUFFER, server_close_frame, sizeof(server_close_frame));
	will_return(readn, 2); // server ws  close frame
	will_return(readn, 2); // server ws  close frame

	will_return(inet_close, 0);


	// act
	char buffer[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;

	ducq_i *ducq = ducq_new_ws_client("host", "port");
	ducq_state recv_state   = ducq_recv(ducq, buffer, &size);
	ducq_state actual_state = ducq_close(ducq);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}
void ws_close_wait_max_3_reads(void **state) {
	// arrange
	int fd = -1;
	ducq_state expected_state  = DUCQ_OK;
	uint16_t expected_status = WS_CLOSE_GOING_AWAY;
	if( ! WS_IS_NETWORK_BYTE_ORDER)
		expected_status = ws_reorder_16(expected_status);
	
	byte_t *expected_message = (byte_t*) &expected_status;
	byte_t expected_header[] = {
		(WS_FIN | WS_CLOSE),
		sizeof(uint16_t),
		expected_message[0],
		expected_message[1]
	};


	// mock
	expect_memory(writen, vptr, expected_header,     2 );
	expect_memory(writen, vptr, expected_header + 2, 2 );
	will_return  (writen, 2);
	will_return  (writen, 2);


	will_return(readn, 10); // client ws  close frame
	will_return(readn,  1); // client tcp tray bytes
	will_return(readn,  2); // client tcp tray bytes
	will_return(readn,  3); // client tcp tray bytes
	will_return(readn,  4); // client tcp tray bytes

	will_return(inet_set_read_timeout, 0);
	expect_value(inet_shutdown_write, fd, fd);
	will_return(inet_shutdown_write, 0);
	will_return(inet_close, 0);


	// act
	ducq_i *ducq = ducq_new_ws_connection(fd);
	ducq_state actual_state = ducq_close(ducq);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

