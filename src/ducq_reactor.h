#ifndef _DUCQ_REACTOR_HEADER_
#define _DUCQ_REACTOR_HEADER_

#include <stdarg.h>
#include "ducq.h"


typedef struct ducq_reactor ducq_reactor;



// log
#define FOREACH_DUCQ_LOG(apply) \
	apply(DEBUG) \
	apply(INFO) \
	apply(WARN) \
	apply(ERROR)
enum ducq_log_level {
	#define _build_enum_(str) DUCQ_LOG_##str,
	FOREACH_DUCQ_LOG(_build_enum_)
	#undef _build_enum_
};
char *ducq_loglevel_tostring(enum ducq_log_level level);

// log callbacks
typedef int (*ducq_log_f)(void *ctx, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, va_list args);

int ducq_color_console_log(void *ctx, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, va_list args);
int ducq_no_log(void *ctx, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, va_list args);

void ducq_reactor_set_log(ducq_reactor *reactor, void* ctx, ducq_log_f log);
void ducq_reactor_set_default_log(ducq_reactor *reactor);

// main log function and shortcut
int ducq_reactor_log(ducq_reactor *reactor, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, ...);
#define ducq_log(level, fmt, ...) ducq_reactor_log(reactor, DUCQ_LOG_##level, __func__, ducq_id(ducq), fmt ,##__VA_ARGS__)



// reactor
ducq_reactor *ducq_reactor_new_with_log(ducq_log_f log, void *ctx);
#define ducq_reactor_new() ducq_reactor_new_with_log(ducq_no_log, NULL)
void ducq_reactor_free(ducq_reactor* reactor);

#define DUCQ_MONITOR_ROUTE "__MONITOR__"
bool ducq_reactor_set_monitor_route(ducq_reactor *reactor, bool is_allowed);

void ducq_loop(ducq_reactor *reactor);


// connections
typedef void (*ducq_accept_f)(ducq_reactor *reactor, int fd, void *ctx);
ducq_state ducq_reactor_add_server(ducq_reactor *reactor, int fd, ducq_accept_f accept_f, void *ctx);
ducq_state ducq_reactor_add_client(ducq_reactor *reactor, int fd, ducq_i *ducq);

// iteration
typedef struct ducq_client_it ducq_client_it;
ducq_client_it *ducq_new_client_it(ducq_reactor *reactor);
ducq_i *ducq_next(ducq_client_it *it, char **route);
void ducq_client_it_free(ducq_client_it *it);

typedef enum {
	DUCQ_LOOP_CONTINUE = 0x00,
	DUCQ_LOOP_BREAK    = 0x01,
	DUCQ_LOOP_DELETE   = 0x02
} ducq_loop_t;
typedef ducq_loop_t (*ducq_loop_f)(ducq_i *ducq, char *route, void *ctx);
ducq_state  ducq_reactor_subscribe(ducq_reactor *reactor, ducq_i *ducq, const char *route);
bool        ducq_reactor_delete(ducq_reactor *reactor, ducq_i *ducq);
ducq_loop_t ducq_reactor_loop(ducq_reactor *reactor, ducq_loop_f apply, void *ctx);


// commands
typedef struct ducq_dispatcher ducq_dispatcher;
ducq_dispatcher *ducq_reactor_get_dispatcher(ducq_reactor * reactor);
typedef ducq_state (*ducq_command_f)(ducq_reactor*, ducq_i*, char *, size_t);

struct ducq_cmd_t {
	char *name;
	char *doc;
	ducq_command_f exec;
};



#endif // _DUCQ_REACTOR_HEADER_
