#ifndef _MOCK_COMMAND_HEADER_
#define _MOCK_COMMAND_HEADER_

#include "../src/ducq.h"
#include "../src/ducq_srv_int.h"

ducq_state mock_command(struct ducq_srv *srv, ducq_i *ducq, char *buffer, size_t size);
struct ducq_cmd_t command;

#endif // _MOCK_COMMAND_HEADER_