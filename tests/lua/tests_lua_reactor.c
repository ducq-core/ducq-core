#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "mock_ducq_client.h"
#include "tests_lua.h"

#include "ducq_lua.h"

extern char MOCK_CLIENT_RECV_BUFFER[];
extern unsigned MOCK_CLIENT_RECV_BUFFER_LEN;

static
void run_lua_script(lua_State *L, char *script) {
	int error = luaL_loadstring(L, script) || lua_pcall(L, 0, 0, 0);
	if(error) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
		fail();
	}	

}
struct test_ctx {
	lua_State *L;
	ducq_reactor *reactor;
};
int lua_reactor_setup(void **state) {
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


	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	run_lua_script(L, "local Ducq = require('LuaDucq')\n");

	ducq_push_reactor(L, reactor);
	lua_setglobal(L, "reactor");


	struct test_ctx *ctx = malloc(sizeof(struct test_ctx));
	ctx->L = L;
	ctx->reactor = reactor;
	*state = ctx;

	return 0;
}
int lua_reactor_teardown(void **state) {
	if(! *state) return 0;

	struct test_ctx *ctx = (struct test_ctx*) *state;
	lua_close(ctx->L);

	expect_any_always(_close, ducq);
	will_return_always(_close, DUCQ_OK);
	ducq_reactor_free(ctx->reactor);

	free(ctx);

	return 0;
}



void lua_reactor_create(void **state) {
	// arrange
	struct test_ctx *ctx = (struct test_ctx*) *state;
	lua_State *L = ctx->L;
	ducq_reactor *reactor = ctx->reactor;

	char *expected_metatable = DUCQ_REACTOR_METATABLE;

	// act
	run_lua_script(L, "METATABLE = getmetatable(reactor)\n");
	lua_getglobal(L, "METATABLE");
	const char *actual_metatable = lua_tostring(L, -1);

	// audit
	assert_string_equal(expected_metatable, actual_metatable);
}

void lua_reactor_loop_count(void **state) {
	// arrange
	struct test_ctx *ctx = (struct test_ctx*) *state;
	lua_State *L = ctx->L;
	ducq_reactor *reactor = ctx->reactor;

	int expected_count = 3;

	// act
	run_lua_script(L,
		"COUNT = 0\n"
		"reactor:loop( function(client, route)\n"
		"	COUNT = COUNT + 1\n"
		"	return reactor.continue\n"
		"end)\n"
	);
	
	lua_getglobal(L, "COUNT");
	int isnum = 0;
	int actual_count = lua_tointegerx(L, -1, &isnum);

	// audit
	assert_true(isnum); 
	assert_int_equal(expected_count, actual_count); 
}

void lua_reactor_loop_break(void **state) {
	// arrange
	struct test_ctx *ctx = (struct test_ctx*) *state;
	lua_State *L = ctx->L;
	ducq_reactor *reactor = ctx->reactor;

	int expected_count = 2;

	// act
	run_lua_script(L,
		"COUNT = 0\n"
		"i = 0\n"
		"reactor:loop( function(client, route)\n"
		"	COUNT = COUNT + 1\n"
		"	i = i + 1\n"
		"	return i >= 2\n"
		"		and reactor.stop\n"
		"		or reactor.continue\n"
		"end)\n"
	);
	
	lua_getglobal(L, "COUNT");
	int isnum = 0;
	int actual_count = lua_tointegerx(L, -1, &isnum);

	// audit
	assert_true(isnum); 
	assert_int_equal(expected_count, actual_count); 
}

void lua_reactor_loop_pass_routes(void **state) {
	// arrange
	struct test_ctx *ctx = (struct test_ctx*) *state;
	lua_State *L = ctx->L;
	ducq_reactor *reactor = ctx->reactor;

	const char expected_routes[] = "routeC, routeB, routeA";

	// act
	run_lua_script(L,
		"ROUTES = {}\n"
		"reactor:loop( function(client, route)\n"
		"	ROUTES[#ROUTES + 1] = route\n"
		"	return reactor.continue\n"
		"end)\n"
		"ROUTES = table.concat(ROUTES, ', ')\n"
	);
	
	lua_getglobal(L, "ROUTES");
	const char *actual_routes = lua_tostring(L, -1);

	// audit
	assert_string_equal(expected_routes, actual_routes); 
}

