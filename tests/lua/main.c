#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>

#include "unit_tests.h"

#include "tests_lua.h"
#include "tests_lua_reactor.h"
#include "tests_lua_client_iterator.h"

const struct CMUnitTest lua[] = {
	build_lua_tests()
};
const struct CMUnitTest lua_reactor[] = {
	build_lua_reactor_tests()
};
const struct CMUnitTest lua_iterator[] = {
	build_lua_iterator_tests()
};


int main(int argc, char** argv){
	cmocka_run_group_tests(lua, NULL, NULL);
		printf("\n\n");

	cmocka_run_group_tests(lua_reactor, NULL, NULL);
		printf("\n\n");

	cmocka_run_group_tests(lua_iterator, NULL, NULL);
		printf("\n\n");

	return 0;
}
