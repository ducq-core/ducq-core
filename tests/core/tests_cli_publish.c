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

	expect_value(_conn, ducq, ducq);
	will_return (_conn, DUCQ_OK);

	expect_value (_emit, ducq, ducq);
	expect_string(_emit, command, "publish");
	expect_string(_emit, payload, payload);
	expect_value (_emit, payload_size, size);
	will_return  (_emit, DUCQ_OK);

	strcpy(MOCK_CLIENT_RECV_BUFFER, "ACK *\n0\nok");
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen("ACK *\n0\nok");
	expect_value(_recv, ducq, ducq);
	expect_any(_recv, ptr);
	expect_any(_recv, count);
	will_return (_recv, DUCQ_OK);

	expect_value(_close, ducq, ducq);
	will_return (_close, DUCQ_OK);

	ducq_state expected_state = DUCQ_OK;

	// act
	ducq_state actual_state = ducq_publish(ducq, "route", payload, size);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}


void pub_conn_error(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	char payload[] = "payload";
	size_t size = sizeof(payload);

	expect_value(_conn, ducq, ducq);
	will_return (_conn, DUCQ_ECONNECT);

	ducq_state expected_state = DUCQ_ECONNECT;

	// act
	ducq_state actual_state = ducq_publish(ducq, "route", payload, size);

	// audit
	assert_int_equal(expected_state, actual_state);

		// teardown
	ducq_free(ducq);
}

void pub_emit_error(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	char payload[] = "payload";
	size_t size = sizeof(payload);

	expect_value(_conn, ducq, ducq);
	will_return (_conn, DUCQ_OK);

	expect_value (_emit, ducq, ducq);
	expect_string(_emit, command, "publish");
	expect_string(_emit, payload, payload);
	expect_value (_emit, payload_size, size);
	will_return  (_emit, DUCQ_ECLOSE);


	ducq_state expected_state = DUCQ_ECLOSE;

	// act
	ducq_state actual_state = ducq_publish(ducq, "route", payload, size);

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

	expect_any (_conn, ducq);
	will_return(_conn, DUCQ_OK);

	expect_any (_emit, ducq);
	expect_any (_emit, command);
	expect_any (_emit, payload);
	expect_any (_emit, payload_size);
	will_return(_emit, DUCQ_OK);

	expect_any (_recv, ducq);
	expect_any (_recv, ptr);
	expect_any (_recv, count);
	will_return(_recv, DUCQ_ECLOSE);

	ducq_state expected_state = DUCQ_ECLOSE;

	// act
	ducq_state actual_state = ducq_publish(ducq, "route", payload, size);

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

	expect_any (_conn, ducq);
	will_return(_conn, DUCQ_OK);

	expect_any (_emit, ducq);
	expect_any (_emit, command);
	expect_any (_emit, payload);
	expect_any (_emit, payload_size);
	will_return(_emit, DUCQ_OK);

	strcpy(MOCK_CLIENT_RECV_BUFFER, "NACK *\n22\nerror");
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen("ACK *\n22\nerror");
	expect_any (_recv, ducq);
	expect_any (_recv, ptr);
	expect_any (_recv, count);
	will_return(_recv, DUCQ_OK);


	ducq_state expected_state = 22;

	// act
	ducq_state actual_state = ducq_publish(ducq, "route", payload, size);

	// audit
	assert_int_equal(expected_state, actual_state);

		// teardown
	ducq_free(ducq);
}



void pub_close_error(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	char payload[] = "payload";
	size_t size = sizeof(payload);

	expect_any (_conn, ducq);
	will_return(_conn, DUCQ_OK);

	expect_any (_emit, ducq);
	expect_any (_emit, command);
	expect_any (_emit, payload);
	expect_any (_emit, payload_size);
	will_return(_emit, DUCQ_OK);

	strcpy(MOCK_CLIENT_RECV_BUFFER, "ACK *\n0\nok");
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen("ACK *\n0\nok");
	expect_any (_recv, ducq);
	expect_any (_recv, ptr);
	expect_any (_recv, count);
	will_return(_recv, DUCQ_OK);

	expect_value(_close, ducq, ducq);
	will_return (_close, DUCQ_ECLOSE);

	ducq_state expected_state = DUCQ_ECLOSE;

	// act
	ducq_state actual_state = ducq_publish(ducq, "route", payload, size);

	// audit
	assert_int_equal(expected_state, actual_state);

		// teardown
	ducq_free(ducq);
}
