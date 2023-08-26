#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>

#include "unit_tests.h"

#include "tests_http.h"

const struct CMUnitTest http[] = { 
	build_http_tests()
};

int main(int argc, char** argv){
	printf("\n\n");
	cmocka_run_group_tests(http, NULL, NULL);

	return 0;
}
