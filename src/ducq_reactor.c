#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>

#include "ducq.h"
#include "ducq_reactor.h"
#include "ducq_dispatcher.h"


enum connection_type {
	DUCQ_CONNECTION_CLIENT,
	DUCQ_CONNECTION_SERVER
};

typedef struct connection_t connection_t;
typedef struct connection_t {
	int  fd;
	enum connection_type type;
	union {
		struct {
			ducq_i *ducq;
			char   *route;
		} client;
		struct {
			ducq_accept_f  accept_f;
			void           *ctx;
		} server;
			
	} as;

	connection_t *next;

} connection_t;

struct ducq_reactor {
	connection_t *connections;

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
ducq_reactor *ducq_reactor_new() {
	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return NULL;
		
	ducq_reactor* reactor = malloc(sizeof(ducq_reactor));
	if(!reactor) return NULL;

	reactor->connections = NULL;

	reactor->allow_monitor_route = true;
	reactor->log_ctx = NULL;
	reactor->log     = _no_log;

	// create dispatcher after values are set
	reactor->dispatcher = ducq_dispatcher_new(reactor);
	if( !reactor->dispatcher) {
		free(reactor);
		return NULL;
	}
	return reactor;
}



static
void connection_free(connection_t *conn) {
	if(!conn) return;

	if(conn->type == DUCQ_CONNECTION_CLIENT) {
		if(conn->as.client.route)
			free(conn->as.client.route);
		ducq_i *ducq = conn->as.client.ducq;
		if(ducq) {
			ducq_close(ducq);
			ducq_free(ducq);
		}
	}
	
	free(conn);
}

void ducq_reactor_free(ducq_reactor* reactor) {
	if(!reactor) return;

	connection_t *conn = reactor->connections;
	while(conn) {
		connection_t *next = conn->next;
		connection_free(conn);
		conn = next;
	}
	ducq_dispatcher_free(reactor->dispatcher);
	free(reactor);
}



ducq_dispatcher *ducq_reactor_get_dispatcher(ducq_reactor * reactor) {
	return reactor->dispatcher;
}






//
//			C O N N E C T I O N S	
//

ducq_state ducq_reactor_add_server(ducq_reactor *reactor, int fd, ducq_accept_f accept_f, void *ctx) {
	connection_t *connection = malloc(sizeof(connection_t));
	if(!connection) return DUCQ_EMEMFAIL;

	connection->fd                 = fd;
	connection->type               = DUCQ_CONNECTION_SERVER;
	connection->as.server.accept_f = accept_f;
	connection->as.server.ctx      = ctx;
	connection->next               = reactor->connections;

	reactor->connections = connection;
	return DUCQ_OK;
}
ducq_state ducq_reactor_add_client(ducq_reactor *reactor, int fd, ducq_i *ducq) {
	connection_t *connection = malloc(sizeof(connection_t));
	if(!connection) return DUCQ_EMEMFAIL;

	connection->fd               = fd;	
	connection->type             = DUCQ_CONNECTION_CLIENT;	
	connection->as.client.ducq   = ducq;	
	connection->as.client.route  = NULL;
	connection->next             = reactor->connections;

	reactor->connections = connection;
	return DUCQ_OK;
}

//
//			S U B S C R I P T I O N S
//


typedef ducq_loop_t (*loop_hook_f)(connection_t *connection, void *ctx);

static
ducq_loop_t connection_loop_template(ducq_reactor *reactor, loop_hook_f hook, void *ctx) {	ducq_loop_t control = DUCQ_LOOP_CONTINUE;
	connection_t *prev  = NULL;
	connection_t *iter  = NULL;
	connection_t *next  = reactor->connections;

	while(prev = iter, iter = next)  {
		next = iter->next;


		control = hook(iter, ctx);


		if (control & DUCQ_LOOP_DELETE) {
			if(prev) prev->next = iter->next;
			if(iter == reactor->connections)
				reactor->connections = next;
			connection_free(iter);
			iter = prev;
		}
		if (control & DUCQ_LOOP_BREAK)
			break;
	}

	return control;
}



ducq_state ducq_reactor_subscribe(ducq_reactor *reactor, ducq_i *ducq, const char *route) {
	connection_t *conn = reactor->connections;
	while(conn) {
		if( ducq == conn->as.client.ducq ) {
			conn->as.client.route = strdup(route);
			return !conn->as.client.route ? DUCQ_EMEMFAIL : DUCQ_OK;
		}
		conn = conn->next;
	}

	return DUCQ_ENOTFOUND;
}


struct client_code_closure {
	ducq_loop_f apply;
	void *ctx;
};
static
ducq_loop_t loop_through_client(connection_t *conn, void *ctx) {
	struct client_code_closure *closure = (struct client_code_closure*) ctx;

	return conn->type == DUCQ_CONNECTION_CLIENT
		? closure->apply(conn->as.client.ducq, conn->as.client.route, closure->ctx)
		: DUCQ_LOOP_CONTINUE;

}
ducq_loop_t ducq_reactor_loop(ducq_reactor *reactor, ducq_loop_f apply, void *ctx) {
	struct client_code_closure closure = {.apply = apply, .ctx = ctx};
	return connection_loop_template(reactor, loop_through_client, &closure);
}


static
ducq_loop_t _delete(ducq_i *ducq, char *route, void *ctx) {
	return (ducq == (ducq_i*) ctx)
		? (DUCQ_LOOP_DELETE   | DUCQ_LOOP_BREAK)
		:  DUCQ_LOOP_CONTINUE;
}
bool ducq_reactor_delete(ducq_reactor *reactor, ducq_i *ducq) {
	if(!ducq) return false;

	ducq_loop_t control = ducq_reactor_loop(reactor, _delete, ducq);
	return (control & DUCQ_LOOP_DELETE);
}







//
//			M A I N   L O O P
//
struct round_table_ctx {
	ducq_reactor *reactor;
	fd_set       *fds;
	char         *buffer;
	int           nready;
};
static
ducq_loop_t round_table(connection_t *conn, void *vctx) {
	struct round_table_ctx *ctx = (struct round_table_ctx*) vctx;
	ducq_loop_t control = DUCQ_LOOP_CONTINUE;

	if( ! FD_ISSET(conn->fd, ctx->fds) )
		return control;
	
	if(--ctx->nready <= 0 ) control = DUCQ_LOOP_BREAK;

	if(conn->type == DUCQ_CONNECTION_SERVER) {
		conn->as.server.accept_f(ctx->reactor, conn->fd, conn->as.server.ctx);
		return control;
	}

	ducq_i * ducq = conn->as.client.ducq;
	size_t size = DUCQ_MSGSZ;
	ducq_state state = ducq_recv(ducq, ctx->buffer, &size);

	switch(state) {
		case DUCQ_OK:
			ducq_dispatch(ctx->reactor->dispatcher, ducq, ctx->buffer, size);
			break;
		case DUCQ_PROTOCOL:
			break; // ignore
		default:
			char *route = conn->as.client.route;
			bool is_a_monitor = route && strcmp(route, DUCQ_MONITOR_ROUTE) == 0;
			ducq_reactor_log(ctx->reactor, DUCQ_LOG_INFO, __func__, ducq_id(ducq),
					"disconnecting: %s (%.*s)\n", ducq_state_tostr(state), (int) size, ctx->buffer);
			if(! is_a_monitor) // monitor lopp will have deleted this connection
				control |= DUCQ_LOOP_DELETE;
			break;
	}
	return control;
}
void ducq_loop(ducq_reactor *reactor) {
	fd_set readfds;
	char buffer[DUCQ_MSGSZ] = "";
	struct round_table_ctx ctx = { 
		.reactor =  reactor,
		.fds     = &readfds,
		.buffer  =  buffer
	};

	while(true) {
		FD_ZERO(&readfds);
		int max = 1;

		connection_t *conn = reactor->connections;
		while(conn) {
			FD_SET(conn->fd, &readfds);
			if(conn->fd > max) max = conn->fd;
			conn = conn->next;
		}

		ctx.nready = select(max+1, &readfds, NULL, NULL, NULL);
		if( ctx.nready == -1) {
			ducq_reactor_log(reactor, DUCQ_LOG_ERROR, __func__, "n\\a", "select() failed: %s\n", strerror(errno));
			continue;
		}


		connection_loop_template(reactor, round_table, &ctx);
	}
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

bool ducq_reactor_set_monitor_route(ducq_reactor *reactor, bool is_allowed) {
	bool old = reactor->allow_monitor_route;
	reactor->allow_monitor_route = is_allowed;
	return old;
}

void ducq_reactor_set_log(ducq_reactor *reactor, void* ctx, ducq_log_f log) {
	reactor->log_ctx = ctx;
	reactor->log     = log;
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
void ducq_reactor_set_default_log(ducq_reactor *reactor) {
	reactor->log     = ducq_color_console_log;
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

	if( route && strcmp(route, DUCQ_MONITOR_ROUTE) == 0 ) { // exact match: don't send to * routes
		size_t size = msg->size;
		if( ducq_send(ducq, msg->buffer, &size) != DUCQ_OK)
			loop |= DUCQ_LOOP_DELETE;
	}

	return loop;

}
int ducq_reactor_log(ducq_reactor *reactor, enum ducq_log_level level, const char *function_name, const char *sender_id, const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
		int rc = reactor->log(reactor->log_ctx, level, function_name, sender_id, fmt, args);
	va_end(args);
	

	if(! reactor->allow_monitor_route)
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
	ducq_reactor_loop(reactor, _do_send_to_monitor_routes, &msg);

	return rc;
}






