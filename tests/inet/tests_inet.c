#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <errno.h>

#include "mock_os.h"

#include "../src/inet_socket.h"


#ifdef __linux__
 		#include <netinet/in.h> // sockaddr_in
 		#include <arpa/inet.h>	// inet_pton
#elif ESP_PLATFORM
	#include "lwip/sockets.h"
#endif


extern struct sockaddr_storage MOCK_ADDR;
extern socklen_t MOCK_ADDR_LEN;


void inet_readn_mock_read_get_called(void **state) {
	//arrange
	int     expected_fd      = 1;
	char    expected_buf[BUFSIZ];
	size_t  expected_count = BUFSIZ;
	ssize_t expected_size  = BUFSIZ;

	expect_value(mock_read, fd,    expected_fd);
	expect_value(mock_read, buf,   expected_buf);
	expect_value(mock_read, count, expected_count);
	will_return(mock_read, expected_count);
	
	// act
	ssize_t actual_size = readn(expected_fd, expected_buf, expected_count);

	// audit
	assert_int_equal(expected_size, actual_size);
}


void inet_readn_minus_one_on_error(void **state) {
	//arrange
	int read1 = 10;
	errno = 0;

	int     expected_fd = 1;
	char    expected_buf[BUFSIZ];
	size_t  expected_count = BUFSIZ;
	ssize_t expected_size  = -1;


	expect_value_count(mock_read, fd, expected_fd, 2);

	expect_value(mock_read, buf,   expected_buf);
	expect_value(mock_read, buf,   expected_buf + read1);

	expect_value(mock_read, count, expected_count);
	expect_value(mock_read, count, expected_count - read1);

	will_return(mock_read, read1);
	will_return(mock_read, -1);
	
	// act
	ssize_t actual_size = readn(expected_fd, expected_buf, expected_count);

	// audit
	assert_int_equal(expected_size, actual_size);
}

void inet_readn_conn_closed_return_read_so_far(void **state) {
	//arrange
	int read1 = 10;
	int read2 = 20;
	int read3 = 30;
	errno = 0;

	int expected_fd = 1;
	expect_value_count(mock_read, fd, expected_fd, 4);

	char expected_buf[BUFSIZ];
	char* ptr = expected_buf;
	expect_value(mock_read, buf, ptr);
	expect_value(mock_read, buf, ptr += read1);
	expect_value(mock_read, buf, ptr += read2);
	expect_value(mock_read, buf, ptr += read3);

	size_t expected_count = BUFSIZ;
	size_t count = expected_count;
	expect_value(mock_read, count, count);
	expect_value(mock_read, count, count -= read1);
	expect_value(mock_read, count, count -= read2);
	expect_value(mock_read, count, count -= read3);

	will_return(mock_read, read1);
	will_return(mock_read, read2);
	will_return(mock_read, read3);
	will_return(mock_read, 0);
	ssize_t expected_size  = read1 + read2 + read3;

	// act
	ssize_t actual_size = readn(expected_fd, expected_buf, expected_count);

	// audit
	assert_int_equal(expected_size, actual_size);
}


void inet_readn_continue_on_interrupt(void **state) {
	//arrange
	int read1 = 10;
	int read2 = 20;
	int read3 = 30;
	

	int expected_fd = 1;
	expect_value_count(mock_read, fd, expected_fd, 5);

	char expected_buf[BUFSIZ];
	char* ptr = expected_buf;
	expect_value(mock_read, buf, ptr);
	expect_value(mock_read, buf, ptr += read1);
	expect_value(mock_read, buf, ptr += read2);
	expect_value(mock_read, buf, ptr);
	expect_value(mock_read, buf, ptr += read3);

	size_t expected_count = BUFSIZ;
	size_t count = expected_count;
	expect_value(mock_read, count, count);
	expect_value(mock_read, count, count -= read1);
	expect_value(mock_read, count, count -= read2);
	expect_value(mock_read, count, count);
	expect_value(mock_read, count, count -= read3);

	errno = EINTR;
	will_return(mock_read, read1);
	will_return(mock_read, read2);
	will_return(mock_read, -1);
	will_return(mock_read, read3);
	will_return(mock_read, 0);
	ssize_t expected_size  = read1 + read2 + read3;

	// act
	ssize_t actual_size = readn(expected_fd, expected_buf, expected_count);

	// audit
	assert_int_equal(expected_size, actual_size);
}



void inet_readn_stop_when_buffer_full(void **state) {
	//arrange
	int read1 = 10;
	int read2 = 20;
	int read3 = 30;
	errno = 0;

	int expected_fd = 1;
	expect_value_count(mock_read, fd, expected_fd, 3);

	char expected_buf[BUFSIZ];
	char* ptr = expected_buf;
	expect_value(mock_read, buf, ptr);
	expect_value(mock_read, buf, ptr += read1);
	expect_value(mock_read, buf, ptr += read2);

	size_t expected_count = read1 + read2 + read3;
	size_t count = expected_count;
	expect_value(mock_read, count, count);
	expect_value(mock_read, count, count -= read1);
	expect_value(mock_read, count, count -= read2);

	will_return(mock_read, read1);
	will_return(mock_read, read2);
	will_return(mock_read, read3);
	ssize_t expected_size  = expected_count;

	// act
	ssize_t actual_size = readn(expected_fd, expected_buf, expected_count);

	// audit
	assert_int_equal(expected_size, actual_size);
}





//
//
//


void inet_writen_return_writen_count(void **state) {
	//arrange
	char message[] = "message_message_message_message_message";
	ssize_t expected_size  = strlen(message);

	int write1 = 5;
	int write2 = 10;
	int write3 = 15;
	int final_write = expected_size - write1 - write2 - write3;
	errno = 0;


	int expected_fd = 1;
	expect_value_count(mock_write, fd, expected_fd, 4);


	char* ptr = message;
	expect_value(mock_write, buf, ptr);
	expect_value(mock_write, buf, ptr += write1);
	expect_value(mock_write, buf, ptr += write2);
	expect_value(mock_write, buf, ptr += write3);

	size_t count = strlen(message);
	expect_value(mock_write, count, count);
	expect_value(mock_write, count, count -= write1);
	expect_value(mock_write, count, count -= write2);
	expect_value(mock_write, count, count -= write3);

	will_return(mock_write, write1);
	will_return(mock_write, write2);
	will_return(mock_write, write3);
	will_return(mock_write, final_write);

	// act
	ssize_t actual_size = writen(expected_fd, message, expected_size);

	// audit
	assert_int_equal(expected_size, actual_size);
}

void inet_writen_minus_one_on_error(void **state) {
	//arrange
	int write1 = 10;
	errno = 0;

	int     expected_fd = 1;
	char    expected_buf[BUFSIZ];
	size_t  expected_count = BUFSIZ;
	ssize_t expected_size  = -1;


	expect_value_count(mock_write, fd, expected_fd, 2);

	expect_value(mock_write, buf,   expected_buf);
	expect_value(mock_write, buf,   expected_buf + write1);

	expect_value(mock_write, count, expected_count);
	expect_value(mock_write, count, expected_count - write1);

	will_return(mock_write, write1);
	will_return(mock_write, -1);
	
	// act
	ssize_t actual_size = writen(expected_fd, expected_buf, expected_count);

	// audit
	assert_int_equal(expected_size, actual_size);
}


void inet_writen_continue_on_interrup(void **state) {
	//arrange
	char message[] = "message_message_message_message_message";
	ssize_t expected_size  = strlen(message);

	int write1 = 5;
	int write2 = 10;
	int write3 = 15;
	int final_write = expected_size - write1 - write2 - write3;
	errno = 0;


	int expected_fd = 1;
	expect_value_count(mock_write, fd, expected_fd, 5);


	char* ptr = message;
	expect_value(mock_write, buf, ptr);
	expect_value(mock_write, buf, ptr += write1);
	expect_value(mock_write, buf, ptr += write2);
	expect_value(mock_write, buf, ptr);
	expect_value(mock_write, buf, ptr += write3);

	size_t count = strlen(message);
	expect_value(mock_write, count, count);
	expect_value(mock_write, count, count -= write1);
	expect_value(mock_write, count, count -= write2);
	expect_value(mock_write, count, count);
	expect_value(mock_write, count, count -= write3);

	errno = EINTR;
	will_return(mock_write, write1);
	will_return(mock_write, write2);
	will_return(mock_write, -1);
	will_return(mock_write, write3);
	will_return(mock_write, final_write);

	// act
	ssize_t actual_size = writen(expected_fd, message, expected_size);

	// audit
	assert_int_equal(expected_size, actual_size);
}

//
//
//


void inet_close_call_close(void **state) {
	//arrange
	int fd = 1;
	int expected_rc = 0;

	expect_value(mock_close, fd, fd);
	will_return(mock_close, expected_rc);

	// act
	int actual_rc = close(fd);

	// audit
	assert_int_equal(expected_rc, actual_rc);
}
void inet_close_returns_close_rc(void **state) {
	//arrange
	int fd = 1;
	int expected_rc = -1;

	expect_value(mock_close, fd, fd);
	will_return(mock_close, expected_rc);

	// act
	int actual_rc = close(fd);

	// audit
	assert_int_equal(expected_rc, actual_rc);
}







void inet_address_tostring_ipv4(void **state) {
	//arrange
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(9090)
	};
	int pton_rc = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	if(pton_rc != 1)
		fail();

	char *expected_addr	 = "127.0.0.1:9090";

	// act
	char buf[25] = "";
	char *actual_addr = inet_address_tostring((struct sockaddr*)&addr, sizeof(addr), buf, sizeof(buf));

	// audit
	assert_non_null(actual_addr);
	assert_string_equal(expected_addr, actual_addr);
}



void inet_address_tostring_ipv6(void **state) {
	//arrange
	struct sockaddr_in6 addr = {
		.sin6_family = AF_INET6,
		.sin6_port  = htons(9090) 
	};
	int pton_rc = inet_pton(AF_INET6, "::1", &addr.sin6_addr);
	if(pton_rc != 1)
		fail();

	char *expected_addr	 = "::1:9090";

	// act
	char buf[25] = "";
	char *actual_addr = inet_address_tostring((struct sockaddr*)&addr, sizeof(addr), buf, sizeof(buf));

	// audit
	assert_non_null(actual_addr);
	assert_string_equal(expected_addr, actual_addr);
}


void inet_address_tostring_ipv4_buffer_too_small_for_addr(void **state) {
	//arrange
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(9090)
	};
	int pton_rc = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	if(pton_rc != 1)
		fail();

	char *expected_addr = "127.0.0.1:9090";

	// act
	char buf[4] = "";
	char *actual_addr = inet_address_tostring((struct sockaddr*)&addr, sizeof(addr), buf, sizeof(buf));

	// audit
	assert_null(actual_addr);
}
void inet_address_tostring_ipv4_buffer_too_small_for_port(void **state) {
	//arrange
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(9090)
	};
	int pton_rc = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	if(pton_rc != 1)
		fail();

	char *expected_addr = "127.0.0.1:9090";

	// act
	char buf[9] = "";
	char *actual_addr = inet_address_tostring((struct sockaddr*)&addr, sizeof(addr), buf, sizeof(buf));

	// audit
	assert_null(actual_addr);
}

void inet_address_tostring_ipv4_buffer_too_small_for_null_terminator(void **state) {
	//arrange
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(9090)
	};
	int pton_rc = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	if(pton_rc != 1)
		fail();

	char *expected_addr = "127.0.0.1:9090";

	// act
	char buf_fail[14] = "";
	char buf_ok  [15] = "";
	char *actual_addr_fail = inet_address_tostring((struct sockaddr*)&addr, sizeof(addr), buf_fail, sizeof(buf_fail));
	char *actual_addr_ok   = inet_address_tostring((struct sockaddr*)&addr, sizeof(addr), buf_ok,   sizeof(buf_ok)  );

	// audit
	assert_null(actual_addr_fail);
	assert_string_equal(expected_addr, actual_addr_ok);
}







void inet_self_socket_tostring_calls_getsockname(void **state) {
	//arrange
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(9090)
	};
	int pton_rc = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	if(pton_rc != 1)
		fail();

	memcpy(&MOCK_ADDR, &addr, sizeof(addr));
	MOCK_ADDR_LEN = sizeof(addr);

	int expected_fd = 1;
	char *expected_addr = "127.0.0.1:9090";

	expect_value(mock_getsockname, fd, expected_fd);
	will_return(mock_getsockname, 0);

	// act
	char buf[25] = "";

	char *actual_addr = inet_self_socket_tostring(expected_fd, buf, sizeof(buf));

	// audit
	assert_non_null(actual_addr);
	assert_string_equal(expected_addr, actual_addr);
}


void inet_self_socket_tostring_returns_null_if_getsockname_fails(void **state) {
	//arrange
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(9090)
	};
	int pton_rc = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	if(pton_rc != 1)
		fail();

	memcpy(&MOCK_ADDR, &addr, sizeof(addr));
	MOCK_ADDR_LEN = sizeof(addr);

	int expected_fd = 1;

	expect_value(mock_getsockname, fd, expected_fd);
	will_return(mock_getsockname, -1);

	// act
	char buf[25] = "";
	char *actual_addr = inet_self_socket_tostring(expected_fd, buf, sizeof(buf));

	// audit
	assert_null(actual_addr);
}

void inet_peer_socket_tostring_calls_getpeername(void **state) {
	//arrange
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(9090)
	};
	int pton_rc = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	if(pton_rc != 1)
		fail();

	memcpy(&MOCK_ADDR, &addr, sizeof(addr));
	MOCK_ADDR_LEN = sizeof(addr);

	int expected_fd = 1;
	char *expected_addr = "127.0.0.1:9090";

	expect_value(mock_getpeername, fd, expected_fd);
	will_return(mock_getpeername, 0);

	// act
	char buf[25] = "";

	char *actual_addr = inet_peer_socket_tostring(expected_fd, buf, sizeof(buf));

	// audit
	assert_non_null(actual_addr);
	assert_string_equal(expected_addr, actual_addr);
}