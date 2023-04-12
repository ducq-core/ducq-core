#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>

#include "unit_tests.h"


#include "tests_cmd_publish.h"
#include "tests_cmd_subscribe.h"


const struct CMUnitTest publish_command[] = {
	build_subscribe_tests()
};
const struct CMUnitTest subscribe_command[] = {
	build_subscribe_tests()
};


int main(int argc, char** argv){

		printf("\n\n");
	cmocka_run_group_tests(publish_command, publish_tests_setup, publish_tests_teardown);
		printf("\n\n");
	cmocka_run_group_tests(subscribe_command, NULL, NULL);

	return 0;
}
