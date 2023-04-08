#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>

#include "unit_tests.h"

#include "tests_errors.h"
// #include "test_keylist.h"
// #include "test_base_msg.h"





const struct CMUnitTest errors[] = { 
	foreach_error_tests(build_cmocka_unit_test) 
};
// const struct CMUnitTest keylist[] = { 
// 	foreach_keylist_test(build_cmocka_unit_test) 
// };
// const struct CMUnitTest basemsg[] = { 
// 	foreach_base_msg_test(build_cmocka_unit_test) 
// };


int main(int argc, char** argv){


		printf("\n\n");
	cmocka_run_group_tests(errors, NULL, NULL);
		printf("\n\n");
	// cmocka_run_group_tests(keylist, NULL, NULL);
		
		
	// 	printf("\n\n");
	// cmocka_run_group_tests(basemsg, NULL, NULL);
	// 	printf("\n\n");

	return 0;
}
