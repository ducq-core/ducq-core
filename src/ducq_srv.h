#ifndef _DUCQ_SRV_HEADER_
#define _DUCQ_SRV_HEADER_

#include <stdarg.h>

#include "ducq.h"


typedef struct ducq_srv ducq_srv;


ducq_srv *ducq_srv_new();
void ducq_srv_free(ducq_srv* srv);

bool ducq_srv_unsubscribe(ducq_srv *srv, ducq_i *ducq);

ducq_state ducq_srv_load_commands_path(ducq_srv* srv, const char *path);
#define ducq_srv_load_commands(srv) ducq_srv_load_commands_path(srv, "./commands");

ducq_state ducq_srv_dispatch(ducq_srv *srv, ducq_i *ducq);



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