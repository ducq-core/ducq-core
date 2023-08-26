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
	expected_end = strstr(actual_header, "\r\n\r\n") + 2;

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

void http_find_http_header(void **state) {
	//arrange
	
	char *expected_header = strstr(example_header, "websocket");
	if(!expected_header) fail();
	char *expected_end  = expected_header + strlen("websocket");

	// act
	char *actual_end    = NULL;
	char *actual_header = inet_find_http_header(example_header, "Upgrade", &actual_end);

	// audit
	assert_string_equal(expected_header, actual_header);
	assert_ptr_equal(expected_header, actual_header);
	assert_ptr_equal(expected_end, actual_end);
}

void http_find_http_header_fails(void **state) {
	//arrange
	
	// act
	char *actual_end    = NULL;
	char *actual_header = inet_find_http_header(example_header, "non-existent", &actual_end);

	// audit
	assert_null(actual_header);
	assert_null(actual_end);
}



void http_find_http_header_even_if_token_found_elsewhere(void **state) {
	//arrange
	char repeated_token[] = 
		"Connection: keep-alive, Upgrade\r\n"
		"Sec-Fetch-Dest: empty\r\n"
		"Sec-Fetch-Mode: websocket\r\n"
		"Sec-Fetch-Site: cross-site\r\n"
		"Pragma: no-cache\r\n"
		"Upgrade: websocket\r\n"
		"Cache-Control: no-cache\r\n"
		"\r\n";

	char *expected_header = strstr(repeated_token, "Upgrade: websocket\r\n");
	if(!expected_header) fail();
	expected_header += strlen("Upgrade: ");
	char *expected_end  = expected_header + strlen("websocket");

	// act
	char *actual_end    = NULL;
	char *actual_header = inet_find_http_header(repeated_token, "Upgrade", &actual_end);

	// audit
	assert_non_null(actual_header);
	assert_non_null(actual_end);
	assert_string_equal(expected_header, actual_header);
	assert_ptr_equal(expected_header, actual_header);
	assert_ptr_equal(expected_end, actual_end);
}

void http_find_http_header_even_if_last(void **state) {
	//arrange
	char repeated_token[] = 
		"Connection: keep-alive, Upgrade\r\n"
		"Sec-Fetch-Dest: empty\r\n"
		"Sec-Fetch-Mode: websocket\r\n"
		"Sec-Fetch-Site: cross-site\r\n"
		"Pragma: no-cache\r\n"
		"Cache-Control: no-cache\r\n"
		"Upgrade: websocket\r\n"
		"\r\n";

	char *expected_header = strstr(repeated_token, "websocket\r\n\r\n");
	if(!expected_header) fail();
	char *expected_end  = expected_header + strlen("websocket");

	// act
	char *actual_end    = NULL;
	char *actual_header = inet_find_http_header(repeated_token, "Upgrade", &actual_end);

	// audit
	assert_non_null(actual_header);
	assert_non_null(actual_end);
	assert_string_equal(expected_header, actual_header);
	assert_ptr_equal(expected_header, actual_header);
	assert_ptr_equal(expected_end, actual_end);
}

void http_find_http_header_not_found_both_return_and_end_are_null(void **state) {
	//arrange
	char repeated_token[] = 
		"Connection: keep-alive, Upgrade\r\n"
		"Sec-Fetch-Dest: empty\r\n"
		"Sec-Fetch-Mode: websocket\r\n"
		"Sec-Fetch-Site: cross-site\r\n"
		"Pragma: no-cache\r\n"
		"Cache-Control: no-cache\r\n"
		"\r\n";

	// act
	char *actual_end    = NULL;
	char *actual_header = inet_find_http_header(repeated_token, "Upgrade", &actual_end);

	// audit
	assert_null(actual_header);
	assert_null(actual_end);
}

