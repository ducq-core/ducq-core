#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>

#include "unit_tests.h"


#include "tests_cmd_publish.h"
#include "tests_cmd_subscribe.h"
#include "tests_cmd_list_subscriptions.h"
#include "tests_cmd_list_commands.h"
#include "tests_cmd_unsubscribe.h"


const struct CMUnitTest publish_command[] = {
	build_publish_tests()
};
const struct CMUnitTest subscribe_command[] = {
	build_subscribe_tests()
};
const struct CMUnitTest list_subscriptions_command[] = {
	build_list_subscriptions_tests()
};
const struct CMUnitTest list_commands_command[] = {
	build_list_commands_tests()
};
const struct CMUnitTest list_unsubscribe_command[] = {
	build_unsubscribe_tests()
};


int main(int argc, char** argv){
	printf("\n\n");
	cmocka_run_group_tests(publish_command, publish_tests_setup, publish_tests_teardown);
	printf("\n\n");
	cmocka_run_group_tests(subscribe_command, subscribe_tests_setup, subscribe_tests_teardown);
	printf("\n\n");
	cmocka_run_group_tests(list_subscriptions_command, list_subscriptions_tests_setup, list_subscriptions_tests_teardown);
	printf("\n\n");
	cmocka_run_group_tests(list_commands_command, NULL, NULL);
	printf("\n\n");
	cmocka_run_group_tests(list_unsubscribe_command,
		unsubscribe_tests_setup, unsubscribe_tests_teardown);

	return 0;
}
