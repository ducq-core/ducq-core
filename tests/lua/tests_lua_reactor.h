#ifndef _LUA_REACTOR_TESTS_HEADER_
#define _LUA_REACTOR_TESTS_HEADER_

#include "unit_tests.h"

int lua_reactor_setup(void **state);
int lua_reactor_teardown(void **state);

#define foreach_lua_reactor_tests(apply) \
	apply(lua_reactor_create) \
	apply(lua_reactor_loop_count) \
	apply(lua_reactor_loop_break) \
	apply(lua_reactor_loop_pass_routes)

foreach_lua_reactor_tests(build_forward_declarations)

#define build_lua_reactor_tests_with_setup_and_teardown(test) \
	cmocka_unit_test_setup_teardown(test, lua_reactor_setup, lua_reactor_teardown),


#define build_lua_reactor_tests() \
	foreach_lua_reactor_tests(build_lua_reactor_tests_with_setup_and_teardown) 

#endif // _LUA_REACTOR_TESTS_HEADER_
