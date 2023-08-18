#ifndef _DUCQ_SRV_HEADER_
#define _DUCQ_SRV_HEADER_

#include <stdarg.h>
#include "ducq.h"


typedef struct ducq_srv ducq_srv;

ducq_srv *ducq_srv_new();
void ducq_srv_free(ducq_srv* srv);


ducq_state ducq_srv_send_ack(ducq_i *ducq, ducq_state state);


// iteration
ducq_state ducq_srv_add(ducq_srv *srv, ducq_i *ducq, const char *route);
bool ducq_srv_delete(ducq_srv *srv, ducq_i *ducq);
typedef enum {
	DUCQ_LOOP_CONTINUE = 0x00,
	DUCQ_LOOP_BREAK    = 0x01,
	DUCQ_LOOP_DELETE   = 0x02
} ducq_loop_t;
typedef ducq_loop_t (*ducq_loop_f)(ducq_i *ducq, char *route, void *ctx);
ducq_loop_t ducq_srv_loop(ducq_srv *srv, ducq_loop_f apply, void *ctx);


// commands
typedef struct ducq_dispatcher ducq_dispatcher;
ducq_dispatcher *ducq_srv_get_dispatcher(ducq_srv * srv);
typedef ducq_state (*ducq_command_f)(ducq_srv*, ducq_i*, char *, size_t);

struct ducq_cmd_t {
	char *name;
	char *doc;
	ducq_command_f exec;
};


struct ducq_cmd_paramt {
	char *name;
	char *doc;
	ducq_command_f exec;
};





//
// LOG
//

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

#define DUCQ_MONITOR_ROUTE "__MONITOR__"
bool ducq_srv_set_monitor_route(ducq_srv *srv, bool is_allowed);

typedef int (*ducq_log_f)(void *ctx, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, va_list args);
void ducq_srv_set_log(ducq_srv *srv, void* ctx, ducq_log_f log);
void ducq_srv_set_default_log(ducq_srv *srv);
int ducq_srv_log(ducq_srv *srv, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, ...);
#define ducq_log(level, fmt, ...) ducq_srv_log(srv, DUCQ_LOG_##level, __func__, ducq_id(ducq), fmt ,##__VA_ARGS__)

int ducq_color_console_log(void *ctx, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, va_list args);


#endif // _DUCQ_SRV_HEADER_
