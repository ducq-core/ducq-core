#ifndef _DUCQ_DISPATCHER_HEADER_
#define _DUCQ_DISPATCHER_HEADER_

#include <stddef.h>

#include "ducq.h"
#include "ducq_reactor.h"

typedef struct ducq_dispatcher ducq_dispatcher;

ducq_dispatcher *ducq_dispatcher_new(ducq_reactor *reactor);
void ducq_dispatcher_free(ducq_dispatcher* dispatcher);

ducq_state ducq_dispatcher_load_commands_path(ducq_dispatcher *dispatcher, const char *path);
ducq_state ducq_dispatcher_add(ducq_dispatcher *dispatcher, const char *path);

ducq_state ducq_dispatch(ducq_dispatcher *dispatcher, ducq_i *ducq, char *msg, size_t size);

int ducq_dispatcher_count_cmds(ducq_dispatcher *dispatcher);
#endif // _DUCQ_DISPATCHER_HEADER_
