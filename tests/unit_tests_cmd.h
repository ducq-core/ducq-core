#ifndef _UNIT_TEST_CMD_HEADER_
#define _UNIT_TEST_CMD_HEADER_

#include "../src/ducq_srv_int.h"

struct fixture {
	void *handle;
	command_f command;
};

#define get_command(state) ((struct fixture*)*state)->command

struct fixture * fix_new(const char *cmd_name);
int fix_free(struct fixture *fix);


#endif //_UNIT_TEST_CMD_HEADER_