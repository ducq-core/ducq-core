#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>

#include "unit_tests.h"

#include "tests_state.h"
#include "tests_tcp.h"





const struct CMUnitTest errors[] = { 
	foreach_state_tests(build_cmocka_unit_test) 
};
const struct CMUnitTest tcp[] = { 
	foreach_tcp_tests(build_cmocka_unit_test) 
};


int main(int argc, char** argv){


		printf("\n\n");
	cmocka_run_group_tests(errors, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(tcp, NULL, NULL);
		
		
	// 	printf("\n\n");
	// cmocka_run_group_tests(basemsg, NULL, NULL);
	// 	printf("\n\n");

	return 0;
}
