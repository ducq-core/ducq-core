#include <stdio.h>
#include <setjmp.h>	
#include <cmocka.h>

#include <string.h>

#include "../mock_ducq_client.h"
#include "../../src/ducq.h"


extern char MOCK_CLIENT_RECV_BUFFER[];
extern unsigned MOCK_CLIENT_RECV_BUFFER_LEN;


void receive_ok(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_OK;
	char expected_msg[] = "command route\npayload";

	// mock
	strcpy(MOCK_CLIENT_RECV_BUFFER, expected_msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(expected_msg);
	expect_value(_recv, ducq, ducq);
	expect_any  (_recv, ptr);
	expect_any  (_recv, count);
	will_return (_recv, DUCQ_OK);

	// act
	char actual_msg[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;
	ducq_state actual_state = ducq_receive(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_string_equal(expected_msg, actual_msg);

	// teardown
	ducq_free(ducq);
}

void receive_ack(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_PROTOCOL;
	char expected_msg[] = "ACK _\n0";

	// mock
	strcpy(MOCK_CLIENT_RECV_BUFFER, expected_msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(expected_msg);
	expect_value(_recv, ducq, ducq);
	expect_any  (_recv, ptr);
	expect_any  (_recv, count);
	will_return (_recv, DUCQ_OK);

	// act
	char actual_msg[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;
	ducq_state actual_state = ducq_receive(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_string_equal(expected_msg, actual_msg);

	// teardown
	ducq_free(ducq);
}

void receive_nack(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_ECLOSE;
	char expected_msg[] = "NACK _\n12";

	// mock
	strcpy(MOCK_CLIENT_RECV_BUFFER, expected_msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(expected_msg);
	expect_value(_recv, ducq, ducq);
	expect_any  (_recv, ptr);
	expect_any  (_recv, count);
	will_return (_recv, DUCQ_OK);

	// act
	char actual_msg[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;
	ducq_state actual_state = ducq_receive(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_string_equal(expected_msg, actual_msg);

	// teardown
	ducq_free(ducq);
}

void receive_ping(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_PROTOCOL;
	char expected_msg[] = "PING _\nping";

	// mock
	strcpy(MOCK_CLIENT_RECV_BUFFER, expected_msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(expected_msg);
	expect_value(_recv, ducq, ducq);
	expect_any  (_recv, ptr);
	expect_any  (_recv, count);
	will_return (_recv, DUCQ_OK);

	char pong[] = "PONG _\nping";
	expect_value (_send, ducq, ducq);
	expect_string(_send, buf, pong);
	expect_value (_send, *count, strlen(pong));
	will_return  (_send, DUCQ_OK);

	// act
	char actual_msg[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;
	ducq_state actual_state = ducq_receive(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void receive_ping_send_fail(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_ECLOSE;
	char expected_msg[] = "PING _\nping";

	// mock
	strcpy(MOCK_CLIENT_RECV_BUFFER, expected_msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(expected_msg);
	expect_value(_recv, ducq, ducq);
	expect_any  (_recv, ptr);
	expect_any  (_recv, count);
	will_return (_recv, DUCQ_OK);

	char pong[] = "PONG _\nping";
	expect_value (_send, ducq, ducq);
	expect_string(_send, buf, pong);
	expect_value (_send, *count, strlen(pong));
	will_return  (_send, DUCQ_ECLOSE);

	// act
	char actual_msg[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;
	ducq_state actual_state = ducq_receive(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void receive_pong(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_PROTOCOL;
	char expected_msg[] = "PONG _\nping";

	// mock
	strcpy(MOCK_CLIENT_RECV_BUFFER, expected_msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(expected_msg);
	expect_value(_recv, ducq, ducq);
	expect_any  (_recv, ptr);
	expect_any  (_recv, count);
	will_return (_recv, DUCQ_OK);

	// act
	char actual_msg[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;
	ducq_state actual_state = ducq_receive(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_string_equal(expected_msg, actual_msg);

	// teardown
	ducq_free(ducq);
}

void receive_freeform_message(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_FREEFORM;
	char expected_msg[] = "UNKNOWN _\npayload";

	// mock
	strcpy(MOCK_CLIENT_RECV_BUFFER, expected_msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(expected_msg);
	expect_value(_recv, ducq, ducq);
	expect_any  (_recv, ptr);
	expect_any  (_recv, count);
	will_return (_recv, DUCQ_OK);

	// act
	char actual_msg[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;
	ducq_state actual_state = ducq_receive(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_string_equal(expected_msg, actual_msg);

	// teardown
	ducq_free(ducq);
}

void receive_timeout_send_ping(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_PROTOCOL;
	char expected_msg[] = "PING _\nping";

	// mock
	strcpy(MOCK_CLIENT_RECV_BUFFER, "");
	MOCK_CLIENT_RECV_BUFFER_LEN = 0;
	expect_value(_recv, ducq, ducq);
	expect_any  (_recv, ptr);
	expect_any  (_recv, count);
	will_return (_recv, DUCQ_ETIMEOUT);

	char ping[] = "PING _\nping";
	expect_value (_send, ducq, ducq);
	expect_string(_send, buf, ping);
	expect_value (_send, *count, strlen(ping));
	will_return  (_send, DUCQ_OK);

	// act
	char actual_msg[DUCQ_MSGSZ] = "";
	size_t size = DUCQ_MSGSZ;
	ducq_state actual_state = ducq_receive(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_string_equal(expected_msg, actual_msg);

	// teardown
	ducq_free(ducq);
}

void receive_timeout_expect_pong(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_ETIMEOUT;
	char expected_msg[] = "";

	// mock
	strcpy(MOCK_CLIENT_RECV_BUFFER, expected_msg);
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen(expected_msg);
	expect_value(_recv, ducq, ducq);
	expect_any  (_recv, ptr);
	expect_any  (_recv, count);
	will_return (_recv, DUCQ_ETIMEOUT);

	// act
	char actual_msg[DUCQ_MSGSZ] = "PING _\nping";
	size_t size = DUCQ_MSGSZ;
	ducq_state actual_state = ducq_receive(ducq, actual_msg, &size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}
