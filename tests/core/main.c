#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>

#include "unit_tests.h"

#include "tests_state.h"
#include "tests_tcp.h"
#include "tests_parse.h"
#include "tests_srv_ack.h"
#include "tests_srv_log.h"
#include "tests_srv_sub.h"
#include "tests_srv_cmd.h"
#include "tests_cli_publish.h"
#include "tests_cli_subscribe.h"



const struct CMUnitTest state[] = { 
	build_state_tests()
};
const struct CMUnitTest tcp[] = { 
	build_tcp_tests()
};
const struct CMUnitTest parse[] = { 
	build_parse_tests()
};
const struct CMUnitTest ack[] = { 
	build_ack_tests()
};
const struct CMUnitTest sub[] = {
	build_sub_tests()
};
const struct CMUnitTest cmd[] = {
	build_cmd_tests()
};
const struct CMUnitTest log_func[] = {
	build_log_tests()
};
const struct CMUnitTest cli_pub[] = {
	build_cli_pub_tests()
};
const struct CMUnitTest cli_sub[] = {
	build_cli_sub_tests()
};


int main(int argc, char** argv){

		printf("\n\n");
	cmocka_run_group_tests(state, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(tcp, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(parse, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(ack, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(sub, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(cmd, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(log_func, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(cli_pub, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(cli_sub, NULL, NULL);
		printf("\n\n");

	return 0;
}
