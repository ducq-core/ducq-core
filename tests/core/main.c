#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>

#include "unit_tests.h"

#include "tests_state.h"
#include "tests_tcp.h"
#include "tests_srv_parse.h"
#include "tests_srv_sub.h"
#include "tests_srv_cmd.h"





const struct CMUnitTest state[] = { 
	build_state_tests()
};
const struct CMUnitTest tcp[] = { 
	build_tcp_tests()
};
const struct CMUnitTest srv[] = { 
	build_srv_tests()
};
const struct CMUnitTest sub[] = {
	build_sub_tests()
};
const struct CMUnitTest cmd[] = {
	build_cmd_tests()
};


int main(int argc, char** argv){

		printf("\n\n");
	cmocka_run_group_tests(state, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(tcp, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(srv, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(sub, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(cmd, NULL, NULL);
		printf("\n\n");

	return 0;
}
