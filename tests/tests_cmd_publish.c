#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <dlfcn.h>

#include "mock_ducq_client.h"

#include "../src/ducq.h"
#include "../src/ducq_srv.h"
#include "../src/ducq_srv_int.h"

struct fixture {
	void *handle;
	command_f command;
};

int publish_tests_setup(void **state) {
	void *handle = dlopen("./commands/publish.so", RTLD_NOW | RTLD_LOCAL);
	if(handle) goto error;

	command_f command = dlsym(handle, "publish");
	if(!command) goto error;

	struct fixture *fix = malloc(sizeof(struct fixture));
	if(!fix) goto error;

	fix->handle  = handle;
	fix->command = command;
	*state = fix;
	return 0;

	error:
		if(handle) dlclose(handle);
		if(fix)    free(fix);
		return -1;
}
int publish_tests_teardown(void **state) {
	struct fixture *fix = *state;
	
	if(!fix) return -1;
	if(fix->handle) dlclose(fix->handle);
	free(fix);
	return 0;
}



void publish_(void **state) {
	//arrange
	command_f publish = *state;

	ducq_srv *srv = ducq_srv_new();
	ducq_i *sender = ducq_new_mock();
	char buffer[] = "publish route\npayload";
	size_t size = sizeof(buffer);

	ducq_state expected_state = DUCQ_OK;

	// act
	ducq_state actual_state = publish(srv, sender, buffer, size);

	//audit
	assert_int_equal(expected_state, actual_state);

	//teardown
	ducq_srv_free(srv);
}
