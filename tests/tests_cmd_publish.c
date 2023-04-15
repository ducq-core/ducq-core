#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdlib.h>


#include "mock_ducq_client.h"
#include "unit_tests_cmd.h"

#include "../src/ducq.h"
#include "../src/ducq_srv.h"
#include "../src/ducq_srv_int.h"


int publish_tests_setup(void **state) {
	*state = fix_new("publish");
	return *state == NULL;
}
int publish_tests_teardown(void **state) {
	struct fixture *fix = *state;
return fix_free(fix);
}


void publish_msg_invalide_if_cant_parse_route(void **state) {
	//arrange
	command_f publish = get_command(state);
	
	ducq_state expected_state = DUCQ_EMSGINV;

	ducq_srv *srv = ducq_srv_new();
	ducq_i *publisher = ducq_new_mock();
	char buffer[] = "publishroute\npayload";
	size_t size = sizeof(buffer);

	// act
	ducq_state actual_state = publish(srv, publisher, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	ducq_srv_free(srv);
	ducq_free(publisher);
}

void publish_subscribers_has_ducq_send_called(void **state) {
	//arrange
	command_f publish = get_command(state);
	
	ducq_state expected_state = DUCQ_OK;

	ducq_srv *srv = ducq_srv_new();
	ducq_i *publisher = ducq_new_mock();
	char buffer[] = "publish route\npayload";
	size_t size = sizeof(buffer);

	ducq_sub *sub1 = malloc(sizeof(ducq_sub));
	ducq_sub *sub2 = malloc(sizeof(ducq_sub));
	ducq_sub *sub3 = malloc(sizeof(ducq_sub));
	sub1->ducq     = ducq_new_mock();;
	sub2->ducq     = ducq_new_mock();;
	sub3->ducq     = ducq_new_mock();;
	sub1->route    = strdup("route");
	sub2->route    = strdup("not same route");
	sub3->route    = strdup("route");
	sub1->id       = ducq_id(sub1->ducq);
	sub2->id       = ducq_id(sub2->ducq);
	sub3->id       = ducq_id(sub1->ducq);
	sub1->next     = NULL;
	sub2->next     = sub1;
	sub3->next     = sub2;

	srv->subs = sub3;

	expect_value(_send, ducq, sub3->ducq);
	expect_value(_send, ducq, sub1->ducq);
	expect_string_count(_send, buf, buffer, 2);
	size_t count = sizeof(buffer);
	expect_memory_count(_send, count, &count, sizeof(size_t), 2);
	will_return_count(_send, DUCQ_OK, 2);
	
	// act
	ducq_state actual_state = publish(srv, publisher, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	ducq_free(publisher);

	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_srv_free(srv);
}
