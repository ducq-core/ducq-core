#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "tests_tcp.h"

#include "../src/inet_socket.h"
#include "../src/ducq_tcp.h"
#include "../src/ducq_vtbl.h"


void tcp_connect_ok(void **state) {
	// arrange
	int fd = 4;
	will_return_count(inet_tcp_connect, fd, 1);
	ducq_state expected_state = DUCQ_OK;

	// act
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state  = ducq_conn(ducq);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}


void tcp_connect_econnect(void **state) {
	// arrange
	int fd = -1;
	will_return_count(inet_tcp_connect, fd, 1);
	ducq_state expected_state = DUCQ_ECONNECT;

	// act
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state  = ducq_conn(ducq);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}


void tcp_close_ok(void **state) {
	// arrange
	int rc = 0;
	will_return_count(inet_close, rc, 1);
	ducq_state expected_state = DUCQ_OK;

	// act
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state  = ducq_close(ducq);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}


void tcp_close_eclose(void **state) {
	// arrange
	int rc = -1;
	will_return_count(inet_close, rc, 1);
	ducq_state expected_state = DUCQ_ECLOSE;

	// act
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state  = ducq_close(ducq);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}



void tcp_id_ok(void **state) {
	// arrange
	char expected_id[] = "TCP:__id__";
	expect_value(inet_socket_tostring, self, false);

	// act
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	const char *actual_id  = ducq_id(ducq);

	// audit
	assert_string_equal(expected_id, actual_id);

	// teardown
	ducq_free(ducq);
}


void tcp_id_called_only_once(void **state) {
	// arrange
	char expected_id[] = "TCP:__id__";
		expect_value(inet_socket_tostring, self, false);

	// act
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	const char *actual_id1  = ducq_id(ducq);
	const char *actual_id2  = ducq_id(ducq);

	// audit
	assert_string_equal(expected_id, actual_id1);
	assert_string_equal(expected_id, actual_id2);

	// teardown
	ducq_free(ducq);
}



void tcp_eq_ok(void **state) {
	// arrange
	ducq_i *ducq_a = ducq_new_tcp(NULL, NULL);
	ducq_i *ducq_b = ducq_new_tcp(NULL, NULL);

	// act
	bool are_equal = ducq_eq(ducq_a, ducq_b);

	// audit
	assert_ptr_not_equal(ducq_a, ducq_b);
	assert_true(are_equal);

	// teardown
	ducq_free(ducq_a);
	ducq_free(ducq_b);
}

void tcp_eq_ok_fd_not_equal(void **state) {
	// arrange
	ducq_i *ducq_a = ducq_new_tcp(NULL, NULL);
	ducq_i *ducq_b = ducq_new_tcp(NULL, NULL);

	will_return(inet_tcp_connect, 4);
	will_return(inet_tcp_connect, 5);

	ducq_conn(ducq_a);
	ducq_conn(ducq_b);
	
	// act
	bool are_equal = ducq_eq(ducq_a, ducq_b);

	// audit
	assert_ptr_not_equal(ducq_a, ducq_b);
	assert_false(are_equal);

	// teardown
	ducq_free(ducq_a);
	ducq_free(ducq_b);
}

void tcp_eq_ok_vtbl_not_equal(void **state) {
	// arrange
	ducq_i *ducq_a = ducq_new_tcp(NULL, NULL);
	struct ducq_mock_s {
		ducq_vtbl tbl;
	} _ducq_mock = { 0 };
	ducq_i *ducq_mock =(ducq_i *) &_ducq_mock;

	// act
	bool are_equal = ducq_eq(ducq_a, ducq_mock);

	// audit
	assert_ptr_not_equal(ducq_a, ducq_mock);
	assert_false(are_equal);

	// teardown
	ducq_free(ducq_a);
}


void tcp_copy_is_equal(void **state) {
	// arrange
	ducq_i *ducq_a = ducq_new_tcp(NULL, NULL);

	// act
	ducq_i *ducq_b = ducq_copy(ducq_a);
	bool are_equal = ducq_eq(ducq_a, ducq_b);

	// audit
	assert_ptr_not_equal(ducq_a, ducq_b);
	assert_true(are_equal);

	// teardown
	ducq_free(ducq_a);
	ducq_free(ducq_b);
}



void tcp_timeout_ok(void **state) {
	// arrange
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state expected_state = DUCQ_OK;

	int rc = 0;
	will_return(inet_set_read_timeout, rc);

	// act
	ducq_state actual_state = ducq_timeout(ducq, 10);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}
void tcp_timeout_err(void **state) {
	// arrange
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state expected_state = DUCQ_ECOMMLAYER;

	int rc = -1;
	will_return(inet_set_read_timeout, rc);

	// act
	ducq_state actual_state = ducq_timeout(ducq, 10);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}
void tcp_recv_timeout(void **state) {
	// arrange
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state expected_state = DUCQ_ETIMEOUT;

	char msg[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;

	// mock
	errno = EWOULDBLOCK;
	will_return(inet_set_read_timeout, DUCQ_OK);
	will_return(readn, -DUCQ_EREAD);

	// act
	if( ducq_timeout(ducq, 10) ) fail();
	ducq_state actual_state = ducq_recv(ducq, msg, &size); 

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	errno = 0;
	ducq_free(ducq);
}





void tcp_send_count_ok(void **state) {
	// arrange
	char payload[] = "hello, world";
	int payload_count = strlen(payload);
	char header[10];
	int header_count = snprintf(header, 10, "%d\n", payload_count);

	ducq_state expected_state = DUCQ_OK;
	size_t expected_count = payload_count;
	
	expect_string(writen, vptr, header);
	expect_string(writen, vptr, payload);

	will_return(writen, header_count);
	will_return(writen, payload_count);

	// act
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	size_t actual_count = strlen(payload);
	ducq_state actual_state  = ducq_send(ducq, payload, &actual_count);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_int_equal(expected_count, actual_count);

	// teardown
	ducq_free(ducq);
}


void tcp_send_ewrite_header(void **state) {
	// arrange
	char payload[] = "hello, world";
	int payload_count = strlen(payload);
	char header[10];
	int header_count = snprintf(header, 10, "%d\n", payload_count);

	expect_any(writen, vptr);
	will_return(writen, 0);

	ducq_state expected_state = DUCQ_EWRITE;

	// act
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	size_t actual_count = strlen(payload);
	ducq_state actual_state  = ducq_send(ducq, payload, &actual_count);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void tcp_send_ewrite_payload(void **state) {
	// arrange
	char payload[] = "hello, world";
	int payload_count = strlen(payload);
	char header[10];
	int header_count = snprintf(header, 10, "%d\n", payload_count);


	expect_any_always(writen, vptr);
	will_return(writen, header_count);
	will_return(writen, 0);

	ducq_state expected_state = DUCQ_EWRITE;

	// act
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	size_t actual_count = strlen(payload);
	ducq_state actual_state  = ducq_send(ducq, payload, &actual_count);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}


extern char READ_BUFFER[];
extern int  pos;

void tcp_recv_ok(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_OK;
	char expected_msg[] = "1234567890";
	
	snprintf(READ_BUFFER, BUFSIZ, "%ld\n%s", strlen(expected_msg), expected_msg);
	pos = 0;
	will_return(readn, 1);
	will_return(readn, 1);
	will_return(readn, 1);
	will_return(readn, 10);

	// act
	char actual_msg[BUFSIZ];
	size_t size = BUFSIZ;
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state = ducq_recv(ducq, actual_msg, &size);
	
	// audit
	assert_int_equal(expected_state, actual_state);
	assert_string_equal(expected_msg, actual_msg);

	// teardown
	ducq_free(ducq);
}

void tcp_recv_read_err_on_parsing_length(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EREAD;
	char expected_msg[] = "1234567890";

	snprintf(READ_BUFFER, BUFSIZ, "%ld\n%s", strlen(expected_msg), expected_msg);
	pos = 0;
	will_return(readn, 1);
	will_return(readn, -1); // read() error

	// act
	char actual_msg[BUFSIZ] = {};
	size_t size = BUFSIZ;
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state = ducq_recv(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void tcp_recv_connclosed_on_parsing_length(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_ECONNCLOSED;
	char expected_msg[] = "1234567890";

	snprintf(READ_BUFFER, BUFSIZ, "%ld\n%s", strlen(expected_msg), expected_msg);
	pos = 0;
	will_return(readn, 1);
	will_return(readn, 0); // connection closed

	// act
	char actual_msg[BUFSIZ] = {};
	size_t size = BUFSIZ;
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state = ducq_recv(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void tcp_recv_parsed_length_too_big_for_buffer_size(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EMSGSIZE;
	int four_digits_length = 9999;

	snprintf(READ_BUFFER, BUFSIZ, "%d\n___", four_digits_length);
	pos = 0;
	will_return_count(readn, 1, 2);

	// act
#define THREE_DIGITS_BUFFER 3
	char actual_msg[THREE_DIGITS_BUFFER] = {};
	size_t size = THREE_DIGITS_BUFFER;
#undef THREE_DIGITS_BUFFER
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state = ducq_recv(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}


void tcp_recv_no_endline_after_length(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EMSGINV;
	char expected_msg[] = "1234567890";

	snprintf(READ_BUFFER, BUFSIZ, "%ld%s", strlen(expected_msg), expected_msg); // no '\n'
	pos = 0;
	will_return_always(readn, 1);

	// act
	char actual_msg[BUFSIZ] = {};
	size_t size = BUFSIZ;
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state = ducq_recv(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void tcp_recv_parsed_length_stop_at_buf_size(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EMSGSIZE;
	char expected_msg[] =
		"12345678901234567890123456789012345678901234567890"
		"12345678901234567890123456789012345678901234567890";

	snprintf(READ_BUFFER, BUFSIZ, "%ld%s", strlen(expected_msg), expected_msg);
	pos = 0;
	will_return_always(readn, 1);

	// act
#define SMALL_BUFFER 10
	char actual_msg[SMALL_BUFFER] = {};
	size_t size = SMALL_BUFFER;
#undef SMALL_BUFFER
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state = ducq_recv(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}


void tcp_recv_msg_size_too_big_for_buffer(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EMSGSIZE;

	snprintf(READ_BUFFER, BUFSIZ, "%d\n<payload>", BUFSIZ * 2);
	pos = 0;
	will_return_always(readn, 1);

	// act
	char actual_msg[BUFSIZ] = {};
	size_t size = BUFSIZ;
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state = ducq_recv(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}


void tcp_recv_read_err_on_reading_payload(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EREAD;
	char expected_msg[] = "1234567890";

	snprintf(READ_BUFFER, BUFSIZ, "%ld\n%s", strlen(expected_msg), expected_msg);
	pos = 0;
	will_return_count(readn, 1, strlen("10\n"));
	will_return(readn, -1); // read() error

	// act
	char actual_msg[BUFSIZ] = {};
	size_t size = BUFSIZ;
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state = ducq_recv(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void tcp_recv_connclosed_on_reading_payload(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_ECONNCLOSED;
	char expected_msg[] = "1234567890";

	snprintf(READ_BUFFER, BUFSIZ, "%ld\n%s", strlen(expected_msg), expected_msg);
	pos = 0;
	will_return_count(readn, 1, strlen("10\n"));
	will_return(readn, 0); // connection closed

	// act
	char actual_msg[BUFSIZ] = {};
	size_t size = BUFSIZ;
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state = ducq_recv(ducq, actual_msg, &size);
	
	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void tcp_recv_payload_length_smaller_than_expected(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EREAD;
	char expected_msg[] = "1234567890";
	int payload_length_smaller = strlen(expected_msg) / 2;
	
	snprintf(READ_BUFFER, BUFSIZ, "%d\n%s", payload_length_smaller, expected_msg);
	pos = 0;
	will_return_count(readn, 1, strlen("5\n"));
	will_return(readn, strlen(expected_msg));

	// act
	char actual_msg[BUFSIZ] = {};
	size_t size = BUFSIZ;
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state = ducq_recv(ducq, actual_msg, &size);
	
	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void tcp_recv_payload_length_bigger_than_expected(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_EREAD;
	char expected_msg[] = "1234567890";
	int payload_length_bigger = strlen(expected_msg) * 2;
	
	snprintf(READ_BUFFER, BUFSIZ, "%d\n%s", payload_length_bigger, expected_msg);
	pos = 0;
	will_return_count(readn, 1, strlen("10\n"));
	will_return(readn, strlen(expected_msg));

	// act
	char actual_msg[BUFSIZ] = {};
	size_t size = BUFSIZ;
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state = ducq_recv(ducq, actual_msg, &size);
	
	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void tcp_recv_payload_size_minus_one_buffer_is_null_terminated(void **state) {
	// arrange
#define BUFFER_LENGTH  11
	ducq_state expected_state = DUCQ_OK;
	char expected_msg[BUFFER_LENGTH] = "1234567890";
	
	snprintf(READ_BUFFER, BUFSIZ, "%ld\n%s", strlen(expected_msg), expected_msg);
	pos = 0;
	will_return_count(readn, 1, strlen("10\n"));
	will_return(readn, strlen(expected_msg));

	// act
	char actual_msg[BUFFER_LENGTH] = {};
	size_t size = BUFFER_LENGTH;
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state = ducq_recv(ducq, actual_msg, &size);
	
	// audit
	assert_int_equal(expected_state, actual_state);
	assert_string_equal(expected_msg, actual_msg);

	// teardown
	ducq_free(ducq);
#undef BUFFER_LENGTH
}


void tcp_recv_payload_exactly_same_as_buffer_is_err(void **state) {
	// arrange
#define MESSAGE_LENGTH 11
#define BUFFER_LENGTH  10
	ducq_state expected_state = DUCQ_EMSGSIZE;
	char expected_msg[MESSAGE_LENGTH] = "1234567890";
	
	snprintf(READ_BUFFER, BUFSIZ, "%ld\n%s", strlen(expected_msg), expected_msg);
	pos = 0;
	will_return_count(readn, 1, strlen("10\n"));

	// act
	char actual_msg[BUFFER_LENGTH] = {};
	size_t size = BUFFER_LENGTH;
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);
	ducq_state actual_state = ducq_recv(ducq, actual_msg, &size);
	
	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
#undef BUFFER_LENGTH
}

void tcp_parts_end(void **state) {
	// arrange
	ducq_state expected_state = DUCQ_OK;
	ducq_i *ducq = ducq_new_tcp(NULL, NULL);

	// mock
	expect_string(writen, vptr, "5\n");
	expect_string(writen, vptr, "PARTS");
	will_return(writen, 2);
	will_return(writen, 5);

	expect_string(writen, vptr, "6\n");
	expect_string(writen, vptr, "part1\n");
	will_return(writen, 2);
	will_return(writen, 6);

	expect_string(writen, vptr, "6\n");
	expect_string(writen, vptr, "part2\n");
	will_return(writen, 2);
	will_return(writen, 6);

	expect_string(writen, vptr, "6\n");
	expect_string(writen, vptr, "part3\n");
	will_return(writen, 2);
	will_return(writen, 6);

	expect_string(writen, vptr, "3\n");
	expect_string(writen, vptr, "END");
	will_return(writen, 2);
	will_return(writen, 3);
	
	
	// act
	ducq_state actual_state = DUCQ_OK;
	size_t size = 0;
	actual_state += ducq_parts(ducq);
	size = 6;
	actual_state += ducq_send(ducq, "part1\n", &size);
	size = 6;
	actual_state += ducq_send(ducq, "part2\n", &size);
	size = 6;
	actual_state += ducq_send(ducq, "part3\n", &size);
	actual_state += ducq_end(ducq);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}
