#ifndef _LUA_TESTS_HEADER_
#define _LUA_TESTS_HEADER_

#include "unit_tests.h"

int lua_ducq_setup(void **state);
int lua_ducq_teardown(void **state);

#define foreach_lua_tests(apply) \
	apply(lua_create_mock_as_object) \
	apply(lua_call_conn) \
	apply(lua_call_conn_error_return_state) \
	apply(lua_call_timeout) \
	apply(lua_call_send) \
	apply(lua_call_recv) \
	apply(lua_call_close) \
	apply(lua_msg_tostring)

foreach_lua_tests(build_forward_declarations)

#define build_lua_tests_with_setup_and_teardown(test) \
	cmocka_unit_test_setup_teardown(test, lua_ducq_setup, lua_ducq_teardown),


#define build_lua_tests() \
	foreach_lua_tests(build_lua_tests_with_setup_and_teardown) 


#endif // _LUA_TESTS_HEADER_
