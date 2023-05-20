#include <stdio.h>
#include <setjmp.h>	
#include <cmocka.h>

#include <string.h>

#include "../mock_ducq_client.h"
#include "../../src/ducq.h"


extern char MOCK_CLIENT_RECV_BUFFER[];
extern unsigned MOCK_CLIENT_RECV_BUFFER_LEN;

static
int on_msg_abort(char *payload, size_t size, void *ctx) {
	(void)payload;
	(void)size;
	(void)ctx;

	return -1;
}


void sub_ok(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);
	char payload[] = "";
	size_t size = 0;

	expect_value(_conn, ducq, ducq);
	will_return (_conn, DUCQ_OK);

	expect_value (_emit, ducq, ducq);
	expect_string(_emit, command, "subscribe");
	expect_string(_emit, payload, payload);
	expect_value (_emit, payload_size, size);
	will_return  (_emit, DUCQ_OK);

	expect_value (_timeout, ducq, ducq);
	expect_value (_timeout, timeout, 0);
	will_return  (_timeout, DUCQ_OK);

	strcpy(MOCK_CLIENT_RECV_BUFFER, "ACK *\n0\nok");
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen("ACK *\n0\nok");
	expect_value_count(_recv, ducq, ducq, 2);
	expect_any_count  (_recv, ptr,        2);
	expect_any_count  (_recv, count,      2);
	will_return_count (_recv, DUCQ_OK,    2);

	expect_value(_close, ducq, ducq);
	will_return (_close, DUCQ_OK);

	ducq_state expected_state = DUCQ_OK;

	// act
	ducq_state actual_state = ducq_subscribe(ducq, "route", on_msg_abort, NULL);

	// audit
	assert_int_equal(expected_state, actual_state);

	// teardown
	ducq_free(ducq);
}


void sub_conn_error(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);

	expect_value(_conn, ducq, ducq);
	will_return (_conn, DUCQ_ECONNECT);

	ducq_state expected_state = DUCQ_ECONNECT;

	// act
	ducq_state actual_state = ducq_subscribe(ducq, "route", on_msg_abort, NULL);

	// audit
	assert_int_equal(expected_state, actual_state);

		// teardown
	ducq_free(ducq);
}

void sub_emit_error(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);


	expect_value(_conn, ducq, ducq);
	will_return (_conn, DUCQ_OK);

	expect_any (_emit, ducq);
	expect_any (_emit, command);
	expect_any (_emit, payload);
	expect_any (_emit, payload_size);
	will_return(_emit, DUCQ_ECLOSE);

	ducq_state expected_state = DUCQ_ECLOSE;

	// act
	ducq_state actual_state = ducq_subscribe(ducq, "route", on_msg_abort, NULL);

	// audit
	assert_int_equal(expected_state, actual_state);

		// teardown
	ducq_free(ducq);
}



void sub_recv_error(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);

	expect_any (_conn, ducq);
	will_return(_conn, DUCQ_OK);

	expect_any (_emit, ducq);
	expect_any (_emit, command);
	expect_any (_emit, payload);
	expect_any (_emit, payload_size);
	will_return(_emit, DUCQ_OK);

	expect_any (_timeout, ducq);
	expect_any (_timeout, timeout);
	will_return(_timeout, DUCQ_OK);

	expect_value_count(_recv, ducq, ducq, 2);
	expect_any_count  (_recv, ptr,        2);
	expect_any_count  (_recv, count,      2);
	will_return       (_recv, DUCQ_OK);     // ack
	will_return       (_recv, DUCQ_ECLOSE); // recv error

	ducq_state expected_state = DUCQ_ECLOSE;

	// act
	ducq_state actual_state = ducq_subscribe(ducq, "route", on_msg_abort, NULL);

	// audit
	assert_int_equal(expected_state, actual_state);

		// teardown
	ducq_free(ducq);
}

void sub_nack_state_returned(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);

	expect_any (_conn, ducq);
	will_return(_conn, DUCQ_OK);

	expect_any (_emit, ducq);
	expect_any (_emit, command);
	expect_any (_emit, payload);
	expect_any (_emit, payload_size);
	will_return(_emit, DUCQ_OK);
	
	expect_any (_timeout, ducq);
	expect_any (_timeout, timeout);
	will_return(_timeout, DUCQ_OK);

	strcpy(MOCK_CLIENT_RECV_BUFFER, "NACK *\n22\nerror");
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen("NACK *\n22\nerror");
	expect_any (_recv, ducq);
	expect_any (_recv, ptr);
	expect_any (_recv, count);
	will_return(_recv, DUCQ_OK);

	ducq_state expected_state = 22;

	// act
	ducq_state actual_state = ducq_subscribe(ducq, "route", on_msg_abort, NULL);

	// audit
	assert_int_equal(expected_state, actual_state);

		// teardown
	ducq_free(ducq);
}



void sub_close_error(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);

	expect_any (_conn, ducq);
	will_return(_conn, DUCQ_OK);

	expect_any (_emit, ducq);
	expect_any (_emit, command);
	expect_any (_emit, payload);
	expect_any (_emit, payload_size);
	will_return(_emit, DUCQ_OK);
	
	expect_any (_timeout, ducq);
	expect_any (_timeout, timeout);
	will_return(_timeout, DUCQ_OK);

	strcpy(MOCK_CLIENT_RECV_BUFFER, "ACK *\n0\nok");
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen("ACK *\n0\nok");
	expect_value_count(_recv, ducq, ducq, 2);
	expect_any_count  (_recv, ptr,        2);
	expect_any_count  (_recv, count,      2);
	will_return_count (_recv, DUCQ_OK,    2);

	expect_value(_close, ducq, ducq);
	will_return (_close, DUCQ_ECLOSE);

	ducq_state expected_state = DUCQ_ECLOSE;

	// act
	ducq_state actual_state = ducq_subscribe(ducq, "route", on_msg_abort, NULL);

	// audit
	assert_int_equal(expected_state, actual_state);

		// teardown
	ducq_free(ducq);
}



//
//			 O N   M E S S A G E
//

static
int on_msg_mock(char *payload, size_t size, void *ctx) {
	check_expected(payload);
	check_expected(size);

	int context = *(int*)ctx;
	check_expected(context);

	return mock();
}


void sub_on_message_receives_param(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);

	expect_any (_conn, ducq);
	will_return(_conn, DUCQ_OK);

	expect_any (_emit, ducq);
	expect_any (_emit, command);
	expect_any (_emit, payload);
	expect_any (_emit, payload_size);
	will_return(_emit, DUCQ_OK);
	
	expect_any (_timeout, ducq);
	expect_any (_timeout, timeout);
	will_return(_timeout, DUCQ_OK);

	strcpy(MOCK_CLIENT_RECV_BUFFER, "ACK *\n0\nok");
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen("ACK *\n0\nok");
	expect_any_always (_recv, ducq);
	expect_any_always (_recv, ptr);
	expect_any_always (_recv, count);
	will_return_always(_recv, DUCQ_OK);

	expect_value(_close, ducq, ducq);
	will_return (_close, DUCQ_ECLOSE);

	ducq_state expected_state = DUCQ_ECLOSE;

	int ctx = 99;
	expect_string(on_msg_mock, payload, MOCK_CLIENT_RECV_BUFFER);
	expect_value (on_msg_mock, size, MOCK_CLIENT_RECV_BUFFER_LEN);
	expect_value (on_msg_mock, context, ctx);
	will_return  (on_msg_mock, -1);


	// act
	ducq_state actual_state = ducq_subscribe(ducq, "route", on_msg_mock, &ctx);

	// audit
	assert_int_equal(expected_state, actual_state);

		// teardown
	ducq_free(ducq);
}






struct count_t {
	int max;
	int count;
};

static
int on_msg_after(char *payload, size_t size, void *ctx) {
	(void)payload;
	(void)size;
	
	struct count_t *count = (struct count_t*)ctx;
	count->count++;
	return count->count >= count->max;
}

void sub_on_message_abort(void **state) {
	// arange
	ducq_i *ducq = ducq_new_mock(NULL);

	expect_any (_conn, ducq);
	will_return(_conn, DUCQ_OK);

	expect_any (_emit, ducq);
	expect_any (_emit, command);
	expect_any (_emit, payload);
	expect_any (_emit, payload_size);
	will_return(_emit, DUCQ_OK);
	
	expect_any (_timeout, ducq);
	expect_any (_timeout, timeout);
	will_return(_timeout, DUCQ_OK);

	strcpy(MOCK_CLIENT_RECV_BUFFER, "ACK *\n0\nok");
	MOCK_CLIENT_RECV_BUFFER_LEN = strlen("ACK *\n0\nok");
	expect_any_always (_recv, ducq);
	expect_any_always (_recv, ptr);
	expect_any_always (_recv, count);
	will_return_always(_recv, DUCQ_OK);

	expect_value(_close, ducq, ducq);
	will_return (_close, DUCQ_ECLOSE);

	ducq_state expected_state = DUCQ_ECLOSE;
	int expected_count = 5;
	struct count_t count = { .max = expected_count, .count = 0 };

	// act
	ducq_state actual_state = ducq_subscribe(ducq, "route", on_msg_after, &count);
	int actual_count = count.count;

	// audit
	assert_int_equal(expected_state, actual_state);
	assert_int_equal(expected_count, actual_count);

		// teardown
	ducq_free(ducq);
}


