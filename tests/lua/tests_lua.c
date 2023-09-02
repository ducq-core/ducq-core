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
	ducq_i *ducq;
};
int lua_ducq_setup(void **state) {
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	run_lua_script(L, "local Ducq = require('LuaDucq')\n");
	
	ducq_state expected_state = DUCQ_OK;
	ducq_i *ducq = ducq_new_mock(NULL);
	ducq_push_ducq(L, ducq);
	lua_setglobal(L, "ducq");


	struct test_ctx *ctx = malloc(sizeof(struct test_ctx));
	ctx->L = L;
	ctx->ducq = ducq;
	*state = ctx;

	return 0;
}
int lua_ducq_teardown(void **state) {
	if(! *state) return 0;

	struct test_ctx *ctx = (struct test_ctx*) *state;
	lua_close(ctx->L);
	ducq_free(ctx->ducq);

	free(ctx);

	return 0;
}



void lua_create_mock_as_object(void **state) {
	// arrange
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	run_lua_script(L, "local Ducq = require('LuaDucq')\n");
	
	char expected_id[] = "mock_id";
	ducq_i *ducq = ducq_new_mock(expected_id);
	ducq_push_ducq(L, ducq);
	lua_setglobal(L, "ducq");

	// act
	run_lua_script(L, "ID = ducq:id()\n");
		
	lua_getglobal(L, "ID");
	size_t len = 0;
	const char *actual_id = lua_tolstring(L, -1, &len);

	// audit
	assert_int_equal(strlen(expected_id), len); 
	assert_memory_equal(expected_id, actual_id, len);
	
	// teardown
	lua_close(L);
	ducq_free(ducq);
}

void lua_call_conn(void **state) {
	// arrange
	struct test_ctx *ctx = (struct test_ctx*) *state;
	lua_State *L = ctx->L;
	ducq_i *ducq = ctx->ducq;
	
	ducq_state expected_state = DUCQ_OK;

	// mock
	expect_value(_conn, ducq, ducq);
	will_return (_conn, DUCQ_OK);

	// act
	run_lua_script(L, "STATE = ducq:conn()\n");
		
	lua_getglobal(L, "STATE");
	int isnum;
	ducq_state actual_state = lua_tointegerx(L, -1, &isnum);

	// audit
	assert_true(isnum);
	assert_int_equal(expected_state, actual_state); 
}

void lua_call_conn_error_return_state(void **state) {
	// arrange
	struct test_ctx *ctx = (struct test_ctx*) *state;
	lua_State *L = ctx->L;
	ducq_i *ducq = ctx->ducq;
	
	ducq_state expected_state = DUCQ_ECONNECT;

	// mock
	expect_value(_conn, ducq, ducq);
	will_return (_conn, expected_state);

	// act
	run_lua_script(L, "STATE = ducq:conn()\n");
		
	lua_getglobal(L, "STATE");
	int isnum;
	ducq_state actual_state = lua_tointegerx(L, -1, &isnum);

	// audit
	assert_true(isnum);
	assert_int_equal(expected_state, actual_state); 
}

void lua_call_timeout(void **state) {
	// arrange
	struct test_ctx *ctx = (struct test_ctx*) *state;
	lua_State *L = ctx->L;
	ducq_i *ducq = ctx->ducq;

	ducq_state expected_state = DUCQ_OK;

	// mock
	expect_value(_timeout, ducq, ducq);
	expect_value(_timeout, timeout, 5);
	will_return (_timeout, DUCQ_OK);

	// act
	run_lua_script(L, "STATE = ducq:timeout(5)\n");
		
	lua_getglobal(L, "STATE");
	int isnum;
	ducq_state actual_state = lua_tointegerx(L, -1, &isnum);

	// audit
	assert_true(isnum);
	assert_int_equal(expected_state, actual_state); 
}

void lua_call_recv(void **state) {
	// arrange
	struct test_ctx *ctx = (struct test_ctx*) *state;
	lua_State *L = ctx->L;
	ducq_i *ducq = ctx->ducq;

	ducq_state expected_state = DUCQ_OK;
	char expected_msg[] = "hello, world";

	// mock
	MOCK_CLIENT_RECV_BUFFER_LEN = 
		snprintf(MOCK_CLIENT_RECV_BUFFER, BUFSIZ, "%s", expected_msg);

	expect_value(_recv, ducq, ducq);
	expect_any  (_recv, ptr);
	expect_any  (_recv, count);
	will_return (_recv, DUCQ_OK);

	// act
	run_lua_script(L, "MSG, STATE = ducq:recv()\n");
		
	lua_getglobal(L, "STATE");
	int isnum;
	ducq_state actual_state = lua_tointegerx(L, -1, &isnum);

	lua_getglobal(L, "MSG");
	size_t actual_len;
	const char *actual_msg = lua_tolstring(L, -1, &actual_len);

	// audit
	assert_true(isnum);
	assert_int_equal(expected_state, actual_state); 
	assert_int_equal(strlen(expected_msg), actual_len);
	assert_string_equal(expected_msg, actual_msg); 
}

void lua_call_send(void **state) {
	// arrange
	struct test_ctx *ctx = (struct test_ctx*) *state;
	lua_State *L = ctx->L;
	ducq_i *ducq = ctx->ducq;

	ducq_state expected_state = DUCQ_OK;

	// mock
	expect_value (_send, ducq, ducq);
	expect_string(_send, buf, "hello, world");
	expect_value (_send, *count, strlen("hello, world") );
	will_return  (_send, DUCQ_OK);

	// act
	run_lua_script(L, "STATE = ducq:send(\"hello, world\")\n");
		
	lua_getglobal(L, "STATE");
	int isnum;
	ducq_state actual_state = lua_tointegerx(L, -1, &isnum);

	// audit
	assert_true(isnum);
	assert_int_equal(expected_state, actual_state); 
}

void lua_call_close(void **state) {
	// arrange
	struct test_ctx *ctx = (struct test_ctx*) *state;
	lua_State *L = ctx->L;
	ducq_i *ducq = ctx->ducq;
	
	ducq_state expected_state = DUCQ_OK;

	// mock
	expect_value (_close, ducq, ducq);
	will_return  (_close, DUCQ_OK);

	// act
	run_lua_script(L, "STATE = ducq:close()");
		
	lua_getglobal(L, "STATE");
	int isnum;
	ducq_state actual_state = lua_tointegerx(L, -1, &isnum);

	// audit
	assert_true(isnum);
	assert_int_equal(expected_state, actual_state); 
}
void lua_msg_tostring(void **state) {
	// arrange
	struct test_ctx *ctx = (struct test_ctx*) *state;
	lua_State *L = ctx->L;

	struct	ducq_msg msg = {
		.command = "command",
		.route   = "route",
		.payload = "payload"
	};
	char *expected_string = "command route\npayload";

	// act
	ducq_push_msg(L, &msg);
	lua_setglobal(L, "MSG");
	run_lua_script(L, "msg_to_string = tostring(MSG)");
		
	lua_getglobal(L, "msg_to_string");
	const char *actual_string = lua_tostring(L, -1);

	// audit
	assert_string_equal(expected_string, actual_string); 
}
