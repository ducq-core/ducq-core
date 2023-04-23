#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include <stdio.h>

#include "unit_tests.h"


#include "tests_inet.h"



const struct CMUnitTest inet[] = {
	build_inet_tests()
};


int main(int argc, char** argv){

		printf("\n\n");
	cmocka_run_group_tests(inet, NULL, NULL);

	return 0;
}
