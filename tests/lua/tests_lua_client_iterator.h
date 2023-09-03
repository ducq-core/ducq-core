#ifndef _LUA_ITERATOR_TESTS_HEADER_
#define _LUA_ITERATOR_TESTS_HEADER_

#include "unit_tests.h"

int lua_iterator_setup(void **state);
int lua_iterator_teardown(void **state);

#define foreach_lua_iterator_tests(apply) \
	apply(lua_iterator_as_generic_for) \

foreach_lua_iterator_tests(build_forward_declarations)

#define build_lua_iterator_tests_with_setup_and_teardown(test) \
	cmocka_unit_test_setup_teardown(test, lua_iterator_setup, lua_iterator_teardown),


#define build_lua_iterator_tests() \
	foreach_lua_iterator_tests(build_lua_iterator_tests_with_setup_and_teardown) 

#endif // _LUA_ITERATOR_TESTS_HEADER_
