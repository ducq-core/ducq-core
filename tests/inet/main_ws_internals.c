#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>

#include "unit_tests.h"

#include "tests_ws_handshake_internals.h"

const struct CMUnitTest ws_handshake_internals[] = {
	build_websocket_handshake_internals_tests()
};


int main(int argc, char** argv){
		printf("\n\n");
	cmocka_run_group_tests(ws_handshake_internals, NULL, NULL);

	return 0;
}
