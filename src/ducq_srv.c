#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>

#include "ducq.h"
#include "ducq_srv.h"
#include "ducq_dispatcher.h"

typedef struct ducq_sub ducq_sub;
typedef struct ducq_sub {
	ducq_i     *ducq; // first: extends ducq_i
	const char *id;   // unused ?
	int         fd;
	char       *route;
	ducq_sub   *next;
} ducq_sub;

struct ducq_srv {
	ducq_sub *subs;
	ducq_dispatcher *dispatcher;
	bool allow_monitor_route;
	ducq_log_f log;
	void *log_ctx;
};


//
//			C O N S T U C T O R   /   D E S T R U C T O R
//
static
int _no_log(void *ctx, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, va_list args) {
	return 0;
}
ducq_srv *ducq_srv_new() {
	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return NULL;
		
	ducq_srv* srv = malloc(sizeof(ducq_srv));
	if(!srv) return NULL;

	srv->dispatcher = ducq_dispatcher_new(srv);
	if( !srv->dispatcher) {
		free(srv);
		return NULL;
	}
	srv->subs = NULL;

	srv->allow_monitor_route = true;
	srv->log_ctx = NULL;
	srv->log     = _no_log;
	return srv;
}




void ducq_sub_free(ducq_sub *sub) {
	if(!sub) return;
	if(sub->route) free(sub->route);
	if(sub->ducq) {
		ducq_close(sub->ducq);
		ducq_free(sub->ducq);
	}
	free(sub);
}

void ducq_srv_free(ducq_srv* srv) {
	if(!srv) return;

	ducq_sub *sub = srv->subs;
	while(sub) {
		ducq_sub *next = sub->next;
		ducq_sub_free(sub);
		sub = next;
	}
	ducq_dispatcher_free(srv->dispatcher);
	free(srv);
}



ducq_dispatcher *ducq_srv_get_dispatcher(ducq_srv * srv) {
	return srv->dispatcher;
}





//
//			H E L P E R S
//





ducq_state _recv_msg(ducq_i *ducq, char *buffer, size_t *size) {
	size_t max_size = *size;

	DUCQ_CHECK(ducq_timeout(ducq, 5));
	DUCQ_CHECK(ducq_recv(ducq, buffer, size));
	
	(*size)++;
	if(*size > max_size)
		return DUCQ_EMSGSIZE;
	buffer[*size] = '\0';

	return DUCQ_OK;
}




//
//			S U B S C R I B E R S
//

ducq_state ducq_srv_add(ducq_srv *srv, ducq_i *ducq, const char *route) {
	ducq_sub *sub = calloc(1, sizeof(ducq_sub));
	if(!sub) return DUCQ_EMEMFAIL;

	if (! ( sub->ducq  = ducq_copy(ducq)    )) goto mem_failed;
	if (! ( sub->route = strdup(route)      )) goto mem_failed;
	if (! ( sub->id    = ducq_id(sub->ducq) )) goto mem_failed; // TODO: remove id field?

	sub->next = srv->subs;
	srv->subs = sub;

	return DUCQ_OK;

mem_failed: // must stop as soon as an error occur
	ducq_sub_free(sub);
	return DUCQ_EMEMFAIL;

}


ducq_loop_t ducq_srv_loop(ducq_srv *srv, ducq_loop_f apply, void *ctx) {
	ducq_loop_t control = DUCQ_LOOP_CONTINUE;

	ducq_sub *prev = NULL;
	ducq_sub *iter = NULL;
	ducq_sub *next = srv->subs;

	while(prev = iter, iter = next)  {
		next = iter->next;
		control = apply(iter->ducq, iter->route, ctx);

		if (control & DUCQ_LOOP_DELETE) {
			if(prev) prev->next = iter->next;
			ducq_sub_free(iter);
			if(iter == srv->subs && next == NULL)
				srv->subs = NULL;	
			iter = prev;
		}
		if (control & DUCQ_LOOP_BREAK)
			break;
	}

	return control;
}

static
ducq_loop_t _delete(ducq_i *ducq, char *route, void *ctx) {
	return ducq_eq(ducq, (ducq_i*) ctx)
		? (DUCQ_LOOP_DELETE   | DUCQ_LOOP_BREAK)
		:  DUCQ_LOOP_CONTINUE;
}
bool ducq_srv_delete(ducq_srv *srv, ducq_i *ducq) {
	if(!ducq) return false;

	ducq_loop_t control = ducq_srv_loop(srv, _delete, ducq);
	return (control & DUCQ_LOOP_DELETE);
}

//
//			L O G
//

char *ducq_loglevel_tostring(enum ducq_log_level level) {
	switch(level) {
		
#define _build_enum_tostring_(str) case DUCQ_LOG_##str: return #str;
FOREACH_DUCQ_LOG(_build_enum_tostring_)
#undef _build_enum_tostring_

		default: return "UNKNOWN";
	}
}

bool ducq_srv_set_monitor_route(ducq_srv *srv, bool is_allowed) {
	bool old = srv->allow_monitor_route;
	srv->allow_monitor_route = is_allowed;
	return old;
}

void ducq_srv_set_log(ducq_srv *srv, void* ctx, ducq_log_f log) {
	srv->log_ctx = ctx;
	srv->log     = log;
}




int ducq_color_console_log(void *ctx, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, va_list args) {
	(void) ctx; // unused
	
	char now[] = "YYYY-MM-DDTHH:MM:SS";
	time_t timer = time(NULL);
	strftime(now, sizeof(now), "%FT%T", localtime(&timer));
	
	switch(level) {
		case DUCQ_LOG_DEBUG : printf("\033[92m"); break;
		case DUCQ_LOG_INFO  : printf("\033[39m"); break;
		case DUCQ_LOG_WARN  : printf("\033[93m"); break;
		case DUCQ_LOG_ERROR : printf("\033[91m"); break;
	}
	printf("%s  %s  %s: ", now, function_name, sender_id);
	vprintf(fmt, args);
	printf("\n");
	printf("\033[39m");

	return 0;
}
void ducq_srv_set_default_log(ducq_srv *srv) {
	srv->log     = ducq_color_console_log;
}



// LEVEL,funcname,id,msg
struct pub_ctx {
	char *route;
	char *buffer;
	size_t size;
	int count;
};
ducq_loop_t _do_send_to_monitor_routes(ducq_i *ducq, char *route, void *ctx) {
	struct pub_ctx *msg = (struct pub_ctx*) ctx;

	ducq_loop_t loop = DUCQ_LOOP_CONTINUE;

	if( strcmp(route, DUCQ_MONITOR_ROUTE) == 0 ) { // exact match: don't send to * routes
		size_t size = msg->size;
		if( ducq_send(ducq, msg->buffer, &size) != DUCQ_OK)
			loop |= DUCQ_LOOP_DELETE;
	}

	return loop;

}
int ducq_srv_log(ducq_srv *srv, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
		int rc = srv->log(srv->log_ctx, level, function_name, sender_id, fmt, args);
	va_end(args);
	

	if(! srv->allow_monitor_route)
		return rc;


	va_start(args, fmt);
		char buffer[DUCQ_MSGSZ] = "";
		size_t size = 0;
		size +=  snprintf(buffer    , DUCQ_MSGSZ    , "%s,%s,%s,", ducq_loglevel_tostring(level), function_name, sender_id);
		size += vsnprintf(buffer+size, DUCQ_MSGSZ-size, fmt    , args);
	va_end(args);
	
	struct pub_ctx msg = {
		.route  = NULL,
		.buffer = buffer,
		.size   = size,
		.count  = 0
	};
	ducq_srv_loop(srv, _do_send_to_monitor_routes, &msg);

	return rc;
}






