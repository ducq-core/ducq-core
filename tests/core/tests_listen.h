#ifndef _LISTEN_HEADER_
#define _LISTEN_HEADER_

#include "unit_tests.h"

#define foreach_listen_tests(apply) \
	apply(listen_ok)
	
foreach_listen_tests(build_forward_declarations)

#define build_listen_tests() \
	foreach_listen_tests(build_cmocka_unit_test)

#endif // _LISTEN_HEADER_
