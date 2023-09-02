#ifndef _DISPATCHER_LUA_TESTS_HEADER_
#define _DISPATCHER_LUA_TESTS_HEADER_

#include "unit_tests.h"


#define foreach_dispatcher_lua_tests(apply) \
	apply(dispatcher_lua_can_load_a_command) \
	apply(dispatcher_lua_not_found_return_unknown) \
	apply(dispatcher_lua_choose_right_command) \
	apply(dispatcher_lua_returns_command_value) \
	apply(dispatcher_lua_give_msg_as_userdata) \
	apply(dispatcher_lua_can_reload_an_extension) \
	apply(dispatcher_lua_watch_is_nonblock)


foreach_dispatcher_lua_tests(build_forward_declarations)



#define build_dispatcher_lua_tests() \
	foreach_dispatcher_lua_tests(build_cmocka_unit_test)

#endif // _DISPATCHER_LUA_TESTS_HEADER_
