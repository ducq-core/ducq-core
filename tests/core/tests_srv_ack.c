#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tests_srv_ack.h"


#include "../src/ducq_srv.h"
#include "../src/ducq_srv_int.h"
#include "mock_ducq_client.h"


void srv_ctor_dtor_no_leak(void **state) {
	ducq_srv *srv = ducq_srv_new();
	ducq_srv_free(srv);
}



void srv_send_ack_ok(void **state) {
	// arange
	char expected_message[] = "ACK *\n0\nok";
	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_OK;

	will_return(_send, expected_state);
	expect_value(_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	expect_value(_send, *count,strlen(expected_message));

	// act
	ducq_state actual_state = send_ack(ducq, DUCQ_OK);
	

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void srv_send_ack_returns_send_rc(void **state) {
	// arange
	ducq_state expected_state = DUCQ_EWRITE;
	ducq_i *ducq = ducq_new_mock(NULL);

	will_return(_send, expected_state);
	expect_any(_send, ducq);
	expect_any(_send, buf);
	expect_any(_send, *count);

	// act
	ducq_state actual_state = send_ack(ducq, DUCQ_OK);
	

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void srv_send_nack_ok(void **state) {
	// arange
	char expected_message[] = "NACK *\n11\nmessage too big";
	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_state expected_state = DUCQ_OK;

	will_return(_send, expected_state);
	expect_value(_send, ducq, ducq);
	expect_string(_send, buf, expected_message);
	expect_value(_send, *count,strlen(expected_message));


	// act
	ducq_state actual_state = send_ack(ducq, DUCQ_EMSGSIZE);
	

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}

void srv_send_nack_returns_send_rc(void **state) {
	// arange
	ducq_state expected_state = DUCQ_EWRITE;
	ducq_i *ducq = ducq_new_mock(NULL);

	will_return(_send, expected_state);
	expect_any(_send, ducq);
	expect_any(_send, buf);
	expect_any(_send, *count);

	// act
	ducq_state actual_state = send_ack(ducq, DUCQ_EMSGSIZE);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}