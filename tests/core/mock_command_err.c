#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "../src/ducq.h"
#include "../src/ducq_srv_int.h"


ducq_state mock_command_err(struct ducq_srv *srv, ducq_i *ducq, char *buffer, size_t size) {
	check_expected(srv);
	check_expected(ducq);
	check_expected(buffer);
	check_expected(size);

	return mock();
}



struct ducq_cmd_t not_a_command = {
	.name = "mock_command_err",
	.doc  = "error command to use in unit tests.",
	.exec =  mock_command_err
};