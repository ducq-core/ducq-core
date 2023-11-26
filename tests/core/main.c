#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>

#include "unit_tests.h"

#include "tests_state.h"
#include "tests_tcp.h"
#include "tests_ws.h"
#include "tests_parse.h"
#include "tests_dispatcher.h"
#include "tests_dispatcher_lua.h"
#include "tests_ack.h"
#include "tests_log.h"
#include "tests_connections.h"
#include "tests_iterator.h"
#include "tests_receive.h"
#include "tests_listen.h"
#include "tests_cli_publish.h"



const struct CMUnitTest state[] = { 
	build_state_tests()
};
const struct CMUnitTest tcp[] = { 
	build_tcp_tests()
};
const struct CMUnitTest ws[] = { 
	build_ws_tests()
};
const struct CMUnitTest parse[] = { 
	build_parse_tests()
};
const struct CMUnitTest ack[] = { 
	build_ack_tests()
};
const struct CMUnitTest connections[] = {
	build_connections_tests()
};
const struct CMUnitTest iterator[] = {
	build_iterator_tests()
};
const struct CMUnitTest dispatcher[] = {
	build_dispatcher_tests()
};
const struct CMUnitTest dispatcher_lua[] = {
	build_dispatcher_lua_tests()
};
const struct CMUnitTest log_func[] = {
	build_log_tests()
};
const struct CMUnitTest receive[] = {
	build_receive_tests()
};
const struct CMUnitTest listen[] = {
	build_listen_tests()
};
const struct CMUnitTest cli_pub[] = {
	build_cli_pub_tests()
};


int main(int argc, char** argv){

		printf("\n\n");
	cmocka_run_group_tests(state, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(tcp, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(ws, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(parse, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(ack, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(iterator, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(connections, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(dispatcher, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(dispatcher_lua, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(log_func, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(receive, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(listen, NULL, NULL);
		printf("\n\n");
	cmocka_run_group_tests(cli_pub, NULL, NULL);
		printf("\n\n");

	return 0;
}
