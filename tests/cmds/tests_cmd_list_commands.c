#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include "mock_ducq_client.h"
#include "unit_tests_cmd.h"

#include "../src/ducq.h"
#include "../src/ducq_srv.h"
#include "../src/ducq_srv_int.h"




#if DUCQ_MSGSZ != 256
#error "expect DUCQ_MSGSZ == 256 for list_commands tests"
#endif

ducq_state mock_cmd(ducq_srv *srv, ducq_i *ducq, char *message, size_t size) {
	return DUCQ_OK;
}

struct ducq_cmd_t cmds[] = {
	{ .name = "name1", .doc = "doc1", .exec =  mock_cmd},
	{ .name = "name2", .doc = "doc2", .exec =  mock_cmd},
	{ .name = "name3", .doc = "doc3", .exec =  mock_cmd},
	{ .name = "name4", .doc = "doc4", .exec =  mock_cmd}
};

size_t ncmds = sizeof(cmds) / sizeof(cmds[0]);


int list_commands_tests_setup(void **state) {
	*state = fix_new("list_commands");
	return *state == NULL;
}
int list_commands_tests_teardown(void **state) {
	struct fixture *fix = *state;
	return fix_free(fix);
}



void list_commands_list_all_commands(void **state) {
	//arrange
	command_f list_commands = get_command(state);
	

	ducq_srv *srv = ducq_srv_new();
	srv->cmds = calloc(ncmds, sizeof(struct ducq_cmd_t*));
	if(!srv->cmds)
		fail();
		
	for(int i = 0; i < ncmds; i++) {
		srv->cmds[i] = &cmds[i];
	}
	srv->ncmd = ncmds;
	
	ducq_i *emitter = ducq_new_mock(NULL);
	char request[] = "list_commands *\n";
	size_t req_size = sizeof(request);
	
	ducq_state expected_state = DUCQ_OK;

	expect_value_count(_send, ducq, emitter, 4);
	expect_string(_send, buf, "name1,doc1\n");
	expect_string(_send, buf, "name2,doc2\n");
	expect_string(_send, buf, "name3,doc3\n");
	expect_string(_send, buf, "name4,doc4\n");
	expect_value(_send, *count, strlen("name1,doc1\n"));
	expect_value(_send, *count, strlen("name2,doc2\n"));
	expect_value(_send, *count, strlen("name3,doc3\n"));
	expect_value(_send, *count, strlen("name4,doc4\n"));
	will_return_count(_send, DUCQ_OK, 4);

	
	expect_value(_close, ducq, emitter);
	will_return(_close, DUCQ_OK);

	// // act
		ducq_state actual_state = list_commands(srv, emitter, request, req_size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	ducq_srv_free(srv);
	ducq_free(emitter);
}



