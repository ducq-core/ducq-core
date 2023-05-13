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
	apply(WARNING) \
	apply(ERROR)
enum ducq_log_level {
	#define _build_enum_(str) DUCQ_LOG_##str,
	FOREACH_DUCQ_LOG(_build_enum_)
	#undef _build_enum_
};
char *ducq_loglovel_tostring(enum ducq_log_level level);

#define DUCQ_MONITOR_ROUTE "__MONITOR__"
bool ducq_srv_set_monitor_route(ducq_srv *srv, bool is_allowed);

typedef void (*ducq_log_f)(void *ctx, const char *function_name, enum ducq_log_level level, const char *fmt, va_list args);
void ducq_srv_set_log(ducq_srv *srv, void* ctx, ducq_log_f log);
void ducq_srv_set_default_log(ducq_srv *srv);

void ducq_srv_log(ducq_srv *srv, const char *function_name, enum ducq_log_level level, const char *fmt, ...);
#define ducq_log_debug(fmt, ...) ducq_srv_log(srv, __func__, DUCQ_LOG_DEBUG,    fmt __VA_OPT__(,) __VA_ARGS__)
#define ducq_log_info(fmt, ...) ducq_srv_log(srv, __func__, DUCQ_LOG_INFO,     fmt __VA_OPT__(,) __VA_ARGS__)
#define ducq_log_warn(fmt, ...) ducq_srv_log(srv, __func__, DUCQ_LOG_WARNING,  fmt __VA_OPT__(,)  __VA_ARGS__)
#define ducq_log_error(fmt, ...) ducq_srv_log(srv, __func__, DUCQ_LOG_ERROR,    fmt __VA_OPT__(,) __VA_ARGS__)



#endif // _DUCQ_SRV_HEADER_