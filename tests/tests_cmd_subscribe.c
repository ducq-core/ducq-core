#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "mock_ducq_client.h"



#include "../src/ducq.h"
#include "../src/ducq_srv.h"
#include "../src/ducq_srv_int.h"



ducq_state subscribe(struct ducq_srv *srv, ducq_i *ducq, char *buffer, size_t size);


void subscribe_(void **state) {
	//arrange
	ducq_srv *srv = ducq_srv_new();
	ducq_i *sender = ducq_new_mock();
	char buffer[] = "publish route\npayload";
	size_t size = sizeof(buffer);

	ducq_state expected_state = DUCQ_OK;

	// act
	ducq_state actual_state = subscribe(srv, sender, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	ducq_srv_free(srv);
}
