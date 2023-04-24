#include <stdio.h>
#include <setjmp.h>	
#include <cmocka.h>


#include "../../src/ducq.h"



void state_DUCQ_OK_has_value_0(void **state) {
	assert_int_equal(0, DUCQ_OK);
	if(DUCQ_OK) {
		fail();
	}

	if(!DUCQ_OK) {
		;
	}
	else {
		fail();
	}
}

void state_0_return_ok(void **state) {
	// arrange
	const char *expected = "ok";
	ducq_state d_state = DUCQ_OK;

	// act
	const char *actual = ducq_state_tostr(d_state);

	// audit
	assert_string_equal(expected, actual);
}
	

void state_return_msg(void **state) {
	// arrange
	const char *expected = "message invalid";
	ducq_state d_state = DUCQ_EMSGINV;

	// act
	const char *actual = ducq_state_tostr(d_state);

	// audit
	assert_string_equal(expected, actual);
}


void state_negative_return_msg(void **state) {
	// arrange
	const char *expected = "message invalid";
	ducq_state d_state = -DUCQ_EMSGINV;


	// act
	const char *actual = ducq_state_tostr(d_state);

	// audit
	assert_string_equal(expected, actual);
}


void state_out_of_range_return_unknown(void **state) {
	// arrange
	const char *expected = "unknown";
	int out_of_range = 999;
	// act
	const char *actual = ducq_state_tostr(out_of_range);

	// audit
	assert_string_equal(expected, actual);
}


void state_negative_out_of_range_return_unknown(void **state) {
	// arrange
	const char *expected = "unknown";
	int out_of_range = -999;

	// act
	const char *actual = ducq_state_tostr(out_of_range);

	// audit
	assert_string_equal(expected, actual);
}