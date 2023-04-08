#include <stdio.h>
#include <setjmp.h>	
#include <cmocka.h>


#include "../src/ducq.h"


void error_0_return_ok(void** state) {
	// arrange
	const char *expected = "ok";
	ducq_state d_state = DUCQ_OK;

	// act
	const char *actual = ducq_state_tostr(d_state);

	// audit
	assert_string_equal(expected, actual);
}
	

void error_return_msg(void** state) {
	// arrange
	const char *expected = "message invalid";
	ducq_state d_state = DUCQ_EMSGINV;

	// act
	const char *actual = ducq_state_tostr(d_state);

	// audit
	assert_string_equal(expected, actual);
}


void error_negative_return_msg(void** state) {
	// arrange
	const char *expected = "message invalid";
	ducq_state d_state = -DUCQ_EMSGINV;


	// act
	const char *actual = ducq_state_tostr(d_state);

	// audit
	assert_string_equal(expected, actual);
}


void error_out_of_range_return_unknown(void** state) {
	// arrange
	const char *expected = "unknown";
	int out_of_range = 999;
	// act
	const char *actual = ducq_state_tostr(out_of_range);

	// audit
	assert_string_equal(expected, actual);
}


void error_negative_out_of_range_return_unknown(void** state) {
	// arrange
	const char *expected = "unknown";
	int out_of_range = -999;

	// act
	const char *actual = ducq_state_tostr(out_of_range);

	// audit
	assert_string_equal(expected, actual);
}