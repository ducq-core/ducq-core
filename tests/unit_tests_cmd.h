#ifndef _UNIT_TEST_CMD_HEADER_
#define _UNIT_TEST_CMD_HEADER_

#include "../src/ducq_reactor.h"

struct fixture {
	void *handle;
	ducq_command_f command;
};

#define get_command(state) ((struct fixture*)*state)->command

struct fixture * fix_new(const char *cmd_name);
int fix_free(struct fixture *fix);


int mock_log(void *ctx, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, va_list args);

#endif //_UNIT_TEST_CMD_HEADER_
