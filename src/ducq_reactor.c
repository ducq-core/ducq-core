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
			char    route[DUCQ_MAX_ROUTE_LEN];
		} client;
		struct {
			ducq_accept_f  accept_f;
			void           *ctx;
		} server;
			
	} as;
} connection_t;

struct last_msg {
	char route[DUCQ_MAX_ROUTE_LEN];
	char msg[DUCQ_MSGSZ];
};

struct ducq_reactor {
	connection_t connections[DUCQ_MAX_CONNECTIONS];

	ducq_dispatcher *dispatcher;
	
	bool allow_log_route;
	void *logger;
	ducq_log_f logfunc;

	struct last_msg last_msgs[DUCQ_MAX_CHANNELS];
};


//
//			C O N S T U C T O R   /   D E S T R U C T O R
//

static
int _no_log(
	void *ctx,
	enum ducq_log_level level,
	const char *function_name,
	const char *sender_id,
	const char *fmt,
	va_list args
) {
	return 0;
}

ducq_reactor *ducq_reactor_new_with_log(void *logger, ducq_log_f logfunc) {
	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return NULL;
		
	ducq_reactor* reactor = malloc(sizeof(ducq_reactor));
	if(!reactor) return NULL;

	connection_t *it  = reactor->connections;
	for(int i = 0; i < DUCQ_MAX_CONNECTIONS; i++) {
		memset(&it[i], 0, sizeof(connection_t));
		it[i].fd = -1;
	}

	reactor->allow_log_route = true;
	reactor->logger          = logger;
	reactor->logfunc         = logfunc ? logfunc : _no_log;


	for(int i = 0; i < DUCQ_MAX_CHANNELS; i++) {
		reactor->last_msgs[i].route[0] = '\0';
		reactor->last_msgs[i].msg[0]   = '\0';
	}

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
	if(!conn)          return;
	if(conn->fd == -1) return;

	conn->fd = -1;

	if(conn->type == DUCQ_CONNECTION_CLIENT) {
		ducq_i *ducq = conn->as.client.ducq;
		if(ducq) {
			ducq_close(ducq);
			ducq_free(ducq);
		}
	}
}
void ducq_reactor_free(ducq_reactor* reactor) {
	if(!reactor) return;


	connection_t *it  = reactor->connections;
	for(int i = 0; i < DUCQ_MAX_CONNECTIONS; i++)
		connection_free(&it[i]);

	ducq_dispatcher_free(reactor->dispatcher);
	free(reactor);
}



ducq_dispatcher *ducq_reactor_get_dispatcher(ducq_reactor * reactor) {
	return reactor->dispatcher;
}

const char *ducq_get_last_msg(ducq_reactor *reactor, const char *route) {
	for(int i = 0; i < DUCQ_MAX_CHANNELS; i++) {
		struct last_msg *last = &reactor->last_msgs[i];

		if (last->route[0] == '\0')
			return "ACK";
		if (strcmp(last->route, route) == 0)
			return last->msg;
	}

	return "ACK\n_";
}

ducq_state ducq_set_last_msg(ducq_reactor *reactor, const char *route, const char *msg) {
	for(int i = 0; i < DUCQ_MAX_CHANNELS; i++) {
		struct last_msg *last = &reactor->last_msgs[i];

		if (last->route[0] == '\0'
		||  strcmp(last->route, route ) == 0) {
			strncpy(last->route, route, DUCQ_MAX_ROUTE_LEN);
			strncpy(last->msg,     msg, DUCQ_MSGSZ);
			return DUCQ_OK;
		}
	}

	return DUCQ_EMAX;
}



//
//			C O N N E C T I O N S	
//
//
ducq_state ducq_reactor_add_server(ducq_reactor *reactor, int fd, ducq_accept_f accept_f, void *ctx) {
	int i;
	connection_t *it  = reactor->connections;
	for(i = 0; i < DUCQ_MAX_CONNECTIONS; i++) {
		if(it[i].fd == -1) break;
	}
	if(i == DUCQ_MAX_CONNECTIONS) return DUCQ_EMAX;

	
	it[i].fd                 = fd;
	it[i].type               = DUCQ_CONNECTION_SERVER;
	it[i].as.server.accept_f = accept_f;
	it[i].as.server.ctx      = ctx;

	return DUCQ_OK;
}
ducq_state ducq_reactor_add_client(ducq_reactor *reactor, int fd, ducq_i *ducq) {
	int i;
	connection_t *it  = reactor->connections;
	for(i = 0; i < DUCQ_MAX_CONNECTIONS; i++) {
		if(it[i].fd == -1) break;
	}
	if(i == DUCQ_MAX_CONNECTIONS) return DUCQ_EMAX;

	it[i].fd                  = fd;	
	it[i].type                = DUCQ_CONNECTION_CLIENT;	
	it[i].as.client.ducq      = ducq;	
	it[i].as.client.route[0]  = '\0';

	return DUCQ_OK;
}

//
//			S U B S C R I P T I O N S
//


typedef ducq_loop_t (*loop_hook_f)(connection_t *connection, void *ctx);

static
ducq_loop_t connection_loop_template(ducq_reactor *reactor, loop_hook_f hook, void *ctx) {
	ducq_loop_t control = DUCQ_LOOP_CONTINUE;
	connection_t *it  = reactor->connections;
	for(int i = 0; i < DUCQ_MAX_CONNECTIONS; i++) {
		if(it[i].fd == -1) continue;

		control = hook(&it[i], ctx);

		if(control & DUCQ_LOOP_DELETE)
			connection_free(&it[i]);
		if(control & DUCQ_LOOP_BREAK)
			break;
	}

	return control;
}



ducq_state ducq_reactor_subscribe(ducq_reactor *reactor, ducq_i *ducq, const char *route) {
	connection_t *it  = reactor->connections;
	for(int i = 0; i < DUCQ_MAX_CONNECTIONS; i++) {
		if( ducq != it[i].as.client.ducq )
			continue;
		strncpy(it[i].as.client.route, route, DUCQ_MAX_ROUTE_LEN);
		return DUCQ_OK;
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
//			I T E R A T O R
//

typedef struct ducq_client_it {
	int i;
	connection_t *conn;
} ducq_client_it;

ducq_client_it *ducq_new_client_it(ducq_reactor *reactor) {
	ducq_client_it *it = malloc(sizeof(ducq_client_it));
	it->i    = 0;
	it->conn = reactor->connections;
	return it;
}
ducq_i *ducq_next(ducq_client_it *it, char **route) {
	int i              = it->i;
	connection_t *conn = it->conn;
	for(; i < DUCQ_MAX_CONNECTIONS; i++) {
		if(conn[i].fd != -1
		&& conn[i].type == DUCQ_CONNECTION_CLIENT
		) break;
	}

	if(i >= DUCQ_MAX_CONNECTIONS)
		return NULL;

	*route       = conn[i].as.client.route;
	ducq_i *ducq = conn[i].as.client.ducq;

	it->i = i+1;
	return  ducq;
}

void ducq_client_it_free(ducq_client_it *it) {
	if(it) free(it);
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
	memset(ctx->buffer, 0, DUCQ_MSGSZ);
	ducq_state state = ducq_recv(ducq, ctx->buffer, &size);

	enum ducq_log_level level = state == DUCQ_ECONNCLOSED ? DUCQ_LOG_INFO : DUCQ_LOG_WARNING;
	switch(state) {
		case DUCQ_OK:
			ducq_dispatch(ctx->reactor->dispatcher, ducq, ctx->buffer, size);
			break;
		case DUCQ_PROTOCOL:
			break; // ignore
		default:
			ducq_send_ack(ducq, state);
			ducq_reactor_log(ctx->reactor, level, __func__, ducq_id(ducq),
				"disconnecting: %s (%s) "
				"buffer[%ld]\"%.*s\"",
				ducq_state_tostr(state), strerror(errno), 
				size, (int)size, ctx->buffer);
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

		connection_t *it  = reactor->connections;
		for(int i = 0; i < DUCQ_MAX_CONNECTIONS; i++) {
			if(it[i].fd == -1) continue;
			FD_SET(it[i].fd, &readfds);
			if(it[i].fd > max) max = it[i].fd;
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

bool ducq_reactor_allow_log_route(ducq_reactor *reactor, bool is_allowed) {
	bool old = reactor->allow_log_route;
	reactor->allow_log_route = is_allowed;
	return old;
}

void ducq_reactor_set_log(ducq_reactor *reactor, void* logger, ducq_log_f logfunc) {
	reactor->logger  = logger;
	reactor->logfunc = logfunc;
}

static
const char *_map_color(enum ducq_log_level level) {
	switch(level) {
		case DUCQ_LOG_DEBUG   : return FG_LITE_BLACK ;
		case DUCQ_LOG_INFO    : return FG_NORMAL     ;
		case DUCQ_LOG_WARNING : return FG_LITE_YELLOW;
		case DUCQ_LOG_ERROR   : return FG_LITE_RED   ;
		default               : return FG_NORMAL     ;
	}
}
int ducq_log_tofile(
	void *logger,
	enum ducq_log_level level,
	const char *function_name,
	const char *sender_id,
	const char *fmt,
	va_list args
) {
	struct ducq_file_logger *l = (struct ducq_file_logger*) logger;
	FILE * file = l ? l->file  : stdout;
	int color   = l ? l->color : true;


	char now[DUCQ_TIMESTAMP_SIZE] = "";
	ducq_getnow(now, sizeof(now));

	if(color) fprintf(file, "%s", _map_color(level));
	 fprintf(file, "%s %s %s ", now, function_name, sender_id);
	vfprintf(file, fmt, args);
	 fprintf(file, "\n");
	 if(color) fprintf(file, FG_NORMAL);

	return 0;
}


struct msg {
	char *buffer;
	size_t size;
};
static
ducq_loop_t _do_send_to_log_route(ducq_i *ducq, char *route, void *ctx) {
	if( !route || strcmp(route, DUCQ_LOG_ROUTE) != 0 )
		return DUCQ_LOOP_CONTINUE; // exact match: don't send to * routes

	struct msg *msg  = (struct msg*) ctx;
	size_t size      = msg->size;

	return ducq_send(ducq, msg->buffer, &size)
		? DUCQ_LOOP_DELETE
		: DUCQ_LOOP_CONTINUE;

}
int ducq_reactor_log(
	ducq_reactor *reactor,
	enum ducq_log_level level,
	const char *function_name,
	const char *sender_id,
	const char *fmt, ...
) {
	va_list args;

	va_start(args, fmt);
	int rc = reactor->logfunc(reactor->logger, level,
		function_name, sender_id, fmt, args);
	va_end(args);
	

	if(! reactor->allow_log_route)
		return rc;


	va_start(args, fmt);
	char buffer[DUCQ_MSGSZ] = "";
	size_t size = 0;
	size += snprintf(buffer, DUCQ_MSGSZ, "%s,%s,%s,",
		ducq_level_tostr(level), function_name, sender_id);
	size += vsnprintf(buffer+size, DUCQ_MSGSZ-size, fmt, args);
	va_end(args);
	
	struct msg msg = {.buffer = buffer, .size = size};
	ducq_reactor_loop(reactor, _do_send_to_log_route, &msg);

	return rc;
}

