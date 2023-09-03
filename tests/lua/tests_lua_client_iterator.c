#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <fcntl.h>    // options and rights on files
#include <sys/stat.h> // mkdir

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "mock_ducq_client.h"
#include "tests_lua.h"

#include "ducq_reactor.h"
#include "ducq_dispatcher.h"
#include "ducq_lua.h"

#define EXTENSION_FOLDER "./extensions/"
#define EXTENSION_FILE   "lua_file.lua"

int lua_iterator_setup(void **state) {
	ducq_reactor *reactor = ducq_reactor_new();

	ducq_i *ducq1 = ducq_new_mock("A");
	ducq_i *ducq2 = ducq_new_mock("B");
	ducq_i *ducq3 = ducq_new_mock("C");
	
	ducq_reactor_add_client(reactor, 10, ducq1);
	ducq_reactor_add_client(reactor, 11, ducq2);
	ducq_reactor_add_client(reactor, 12, ducq3);
	
	ducq_reactor_subscribe(reactor, ducq1, "routeA");
	ducq_reactor_subscribe(reactor, ducq2, "routeB");
	ducq_reactor_subscribe(reactor, ducq3, "routeC");
	*state = reactor;

	return 0;
}
int lua_iterator_teardown(void **state) {
	if(! *state) return 0;

	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);

	ducq_reactor *reactor = *(ducq_reactor**)state;
	ducq_reactor_free(reactor);

	return 0;
}

static
void _make_test_file() {
	int error = mkdir(EXTENSION_FOLDER, O_CREAT | S_IRUSR | S_IWUSR);
	if(error && errno != EEXIST) {
		fprintf(stderr, "mkdir() failded:(%d) %s\n", errno, strerror(errno));
		fail();
	}
	char filename[] = EXTENSION_FOLDER EXTENSION_FILE;
	FILE *lua_file = fopen(filename, "w");
	fprintf(lua_file,
		"function TestIdAndRoute(ducq, msg)\n"
		"	ducq:sendack()\n"
		"	for client, route in ducq:clients() do\n"
		"		client:send( client:id() )\n"
		"		client:send( route       )\n"
		"	end\n"
		"	return 0;\n"
		"end\n"
	);
	fclose(lua_file);
}


void lua_iterator_as_generic_for(void **state) {
	// arrange
	_make_test_file();
	ducq_reactor *reactor = *(ducq_reactor**)state;
	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);
	ducq_dispatcher_add(dispatcher, "./extensions");
	ducq_i *ducq = ducq_new_mock(NULL);

	// mock
	expect_any_always(_send, ducq);
	expect_any_always(_send, *count);
	will_return_always (_send, DUCQ_OK);
	expect_any   (_send, buf); // skip ACK
	expect_string(_send, buf, "A");
	expect_string(_send, buf, "routeA");
	expect_string(_send, buf, "B");
	expect_string(_send, buf, "routeB");
	expect_string(_send, buf, "C");
	expect_string(_send, buf, "routeC");

	// act
	char command[] = "TestIdAndRoute *\n";
	ducq_dispatch(dispatcher, ducq, command, strlen(command));

	// teardown
	ducq_free(ducq);
}

