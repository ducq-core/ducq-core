#include <stdio.h>
#include <setjmp.h>	
#include <cmocka.h>

#include <string.h>

#include "../mock_ducq_client.h"
#include "../../src/ducq.h"


extern char MOCK_CLIENT_RECV_BUFFER[];
extern unsigned MOCK_CLIENT_RECV_BUFFER_LEN;



void pub_ok(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	char payload[] = "payload";
	size_t size = sizeof(payload);
	ducq_state expected_state = DUCQ_OK;

	// mock
	char expected_message[] = "pub route\npayload";
	expect_value (_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	expect_value (_send, *count, strlen(expected_message));
	will_return  (_send, DUCQ_OK);

	strcpy(MOCK_CLIENT_RECV_BUFFER, "ACK *\n0\nok");
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen("ACK *\n0\nok");
	expect_value(_recv, ducq, ducq);
	expect_any(_recv, ptr);
	expect_any(_recv, count);
	will_return (_recv, DUCQ_OK);

	// act
	ducq_state actual_state = ducq_pub(ducq, "route", payload, size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}


void pub_send_error(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	char payload[] = "payload";
	size_t size = sizeof(payload);
	ducq_state expected_state = DUCQ_ECLOSE;

	// mock
	char expected_message[] = "pub route\npayload";
	expect_value (_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	expect_value (_send, *count, strlen(expected_message));
	will_return  (_send, DUCQ_ECLOSE);

	// act
	ducq_state actual_state = ducq_pub(ducq, "route", payload, size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}



void pub_recv_error(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	char payload[] = "payload";
	size_t size = sizeof(payload);
	ducq_state expected_state = DUCQ_ECLOSE;

	// mock
	char expected_message[] = "pub route\npayload";
	expect_value (_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	expect_value (_send, *count, strlen(expected_message));
	will_return  (_send, DUCQ_OK);

	strcpy(MOCK_CLIENT_RECV_BUFFER, "ACK *\n0\nok");
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen("ACK *\n0\nok");
	expect_value(_recv, ducq, ducq);
	expect_any(_recv, ptr);
	expect_any(_recv, count);
	will_return (_recv, DUCQ_ECLOSE);

	// act
	ducq_state actual_state = ducq_pub(ducq, "route", payload, size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void pub_nack_state_returned(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	char payload[] = "payload";
	size_t size = sizeof(payload);
	ducq_state expected_state = DUCQ_NACK;

	// mock
	char expected_message[] = "pub route\npayload";
	expect_value (_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	expect_value (_send, *count, strlen(expected_message));
	will_return  (_send, DUCQ_OK);

	char nack[] = "NACK _\n22\nerror";
	strcpy(MOCK_CLIENT_RECV_BUFFER, nack);
	MOCK_CLIENT_RECV_BUFFER_LEN = sizeof(nack);
	expect_value(_recv, ducq, ducq);
	expect_any(_recv, ptr);
	expect_any(_recv, count);
	will_return (_recv, DUCQ_OK);

	// act
	ducq_state actual_state = ducq_pub(ducq, "route", payload, size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}
