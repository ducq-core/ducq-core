#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <errno.h>

#include "stdio.h"
#include "string.h"
#include "stdbool.h"

#include "inet_http.h"

#include "tests_http.h"





char example_header[] = 
		"GET /chat HTTP/1.1\r\n"
		"Host: server.example.com\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
		"Origin: http://example.com\r\n"
		"Sec-WebSocket-Protocol: chat, superchat\r\n"
		"Sec-WebSocket-Version: 13\r\n"
		"\r\n";

static
int read_example_header(LargestIntegralType param, LargestIntegralType check) {
	char *buffer = (char*)param;
	int *call_number = (int*)check;
	switch(*call_number) {
		case 0:
			memcpy(buffer, example_header, sizeof(example_header) );
			break;	
		default:
			printf("too many call to read\n");
			return false;

	}
	
	(*call_number)++;
	return true;
}
void http_get_header_ok(void **state) {
	//arrange
	char *expected_header = example_header;
	char *expected_end    = NULL;
	size_t expected_size  = strlen(example_header);

	int fd = 10;
	char actual_header[BUFSIZ] = "";
	char *actual_end = NULL;

	// mock
	int call_number = 0;
	expect_value(mock_read, fd,    fd);
	expect_check(mock_read, buf,   read_example_header, &call_number);
	expect_value(mock_read, count, sizeof(actual_header));
	will_return (mock_read, expected_size);


	// act
	ssize_t actual_size = 
		inet_get_http_header(fd, actual_header, sizeof(actual_header), &actual_end);
	expected_end = strstr(actual_header, "\r\n\r\n");

	// audit
	assert_int_equal(expected_size, actual_size);
	assert_string_equal(expected_header, actual_header);
	assert_ptr_equal(expected_end, actual_end);
}




char unterminated_header[] = 
		"GET /chat HTTP/1.1\r\n"
		"Host: server.example.com\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
		"Origin: http://example.com\r\n"
		"Sec-WebSocket-Protocol: chat, superchat\r\n"
		"Sec-WebSocket-Version: 13\r\n";

static
int read_unterminated_header(LargestIntegralType param, LargestIntegralType check) {
	char *buffer = (char*)param;
	int *call_number = (int*)check;
	switch(*call_number) {
		case 0:
			memcpy(buffer, unterminated_header, sizeof(unterminated_header) );
			break;	
		case 1:
			memcpy(buffer, "", 1);
			errno = EWOULDBLOCK;
			break;
		default:
			printf("too many call to read\n");
			return false;

	}
	
	(*call_number)++;
	return true;
}
void http_get_header_not_terminated(void **state) {
	//arrange
	
	char *expected_header = unterminated_header;
	size_t expected_size  = strlen(unterminated_header);

	int fd = 10;
	char actual_header[BUFSIZ] = "";
	char *actual_end = NULL;

	// mock
	int call_number = 0;
	expect_value(mock_read, fd,    fd);
	expect_check(mock_read, buf,   read_unterminated_header, &call_number);
	expect_value(mock_read, count, sizeof(actual_header));
	will_return (mock_read, expected_size);

	expect_value(mock_read, fd,    fd);
	expect_check(mock_read, buf,   read_unterminated_header, &call_number);
	expect_value(mock_read, count, sizeof(actual_header) - strlen(unterminated_header) );
	will_return (mock_read, -1); // TIMEOUT / EWOULDBLOCK


	// act
	ssize_t actual_size = 
		inet_get_http_header(fd, actual_header, sizeof(actual_header), &actual_end);

	// audit
	assert_int_equal(expected_size, actual_size);
	assert_string_equal(expected_header, actual_header);
	assert_null(actual_end);
}


