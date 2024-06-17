#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests_ack.h"


#include "mock_ducq_client.h"




void send_ack_ok(void **state) {
	// arange
	char expected_message[] = "ACK";
	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_OK;

	will_return(_send, expected_state);
	expect_value(_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	expect_value(_send, *count,strlen(expected_message));

	// act
	ducq_state actual_state = ducq_send_ack(ducq, DUCQ_OK);
	

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void send_ack_returns_send_rc(void **state) {
	// arange
	ducq_state expected_state = DUCQ_EWRITE;
	ducq_i *ducq = ducq_new_mock(NULL);

	will_return(_send, expected_state);
	expect_any(_send, ducq);
	expect_any(_send, buf);
	expect_any(_send, *count);

	// act
	ducq_state actual_state = ducq_send_ack(ducq, DUCQ_OK);
	

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void send_nack_ok(void **state) {
	// arange
	ducq_state error = DUCQ_EMSGSIZE;
	char expected_message[256] = "";
	snprintf(expected_message, 256, "NACK *\n%d\n%s", error, ducq_state_tostr(error));

	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_OK;

	will_return(_send, expected_state);
	expect_value(_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	expect_value(_send, *count,strlen(expected_message));

	// act
	ducq_state actual_state = ducq_send_ack(ducq, error);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void send_nack_returns_send_rc(void **state) {
	// arange
	ducq_state expected_state = DUCQ_EWRITE;
	ducq_i *ducq = ducq_new_mock(NULL);

	will_return(_send, expected_state);
	expect_any(_send, ducq);
	expect_any(_send, buf);
	expect_any(_send, *count);

	// act
	ducq_state actual_state = ducq_send_ack(ducq, DUCQ_EMSGSIZE);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}
