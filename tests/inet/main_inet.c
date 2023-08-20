#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>

#include "unit_tests.h"


#include "tests_inet.h"
#include "tests_http.h"
#include "tests_base64.h"
#include "tests_ws_handshake.h"
#include "tests_ws_header.h"


const struct CMUnitTest inet[] = {
	build_inet_tests()
};
const struct CMUnitTest http[] = {
	build_http_tests()
};
const struct CMUnitTest base64[] = {
	build_base64_tests()
};
const struct CMUnitTest ws_header[] = {
	build_websocket_header_tests()
};
const struct CMUnitTest ws_handshake[] = {
	build_websocket_handshake_tests()
};

int main(int argc, char** argv){

		printf("\n\n");
	cmocka_run_group_tests(inet, NULL, NULL);

		printf("\n\n");
	cmocka_run_group_tests(http, NULL, NULL);

		printf("\n\n");
	cmocka_run_group_tests(base64, NULL, NULL);
		
		printf("\n\n");
	cmocka_run_group_tests(ws_header, NULL, NULL);

		printf("\n\n");
	cmocka_run_group_tests(ws_handshake, NULL, NULL);
		
	return 0;
}
