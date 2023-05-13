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
#include "ducq_srv_int.h"

#ifndef __linux__
	#warning "as of now, ducq server is supported on linux only"
#else
	#include <dirent.h>
	#include <dlfcn.h>
	#include <linux/limits.h> // PATH_MAX



ducq_state send_ack(ducq_i *ducq, ducq_state state) {
	char msg[128];
	size_t size = snprintf(msg, 128, "%s *\n%d\n%s",
		state == DUCQ_OK ? "ACK" : "NACK",
		state,
		ducq_state_tostr(state)
	);
	return ducq_send(ducq, msg, &size);
}



bool ducq_srv_unsubscribe(ducq_srv *srv, ducq_i *ducq) {
	ducq_sub *prev = NULL;
	ducq_sub *curr = srv->subs;
	ducq_sub *next = NULL;

	while(curr) {
		next = curr->next;

		if( ducq_eq(ducq, curr->ducq) ) {
			ducq_sub_free(curr);
			
			if(curr == srv->subs)
				srv->subs = next;
			else
				prev->next = next;
			
			return true;
		}

		prev = curr;
		curr = next;
	}

	return false;
}






//
//			D I S P A T C H
//

ducq_state _recv_msg(ducq_i *ducq, char *buffer, size_t *size) {
	size_t max_size = *size;

	ducq_state recv_state = ducq_recv(ducq, buffer, size);
	if( recv_state != DUCQ_OK )
		return recv_state;
	
	(*size)++;
	if(*size > max_size)
		return DUCQ_EMSGSIZE;
	buffer[*size] = '\0';

	return DUCQ_OK;
}
ducq_state unknown(ducq_srv *srv, ducq_i *ducq, char *buffer, size_t size) {
	char *end   = NULL;
	const char *route = ducq_parse_route(buffer, (const char **)&end);
	*end = '\0';
	ducq_log_warn("%s:%s", ducq_id(ducq), route);

	send_ack(ducq, DUCQ_ENOCMD);
	ducq_close(ducq);
	return DUCQ_ENOCMD;
}
command_f _find_command(ducq_srv* srv, const char *buffer) {
	command_f command = unknown;

	char *end = NULL;
	const char *name = ducq_parse_command(buffer, (const char**) &end);
	if ( !name) return command;
	*end = '\0';

	for(int i = 0; i < srv->ncmd ; i++) {
		if(strcmp(name, srv->cmds[i]->name) == 0) {
			command = srv->cmds[i]->exec;
			break;
		}
	}

	*end = ' ';
	
	return command;
}
ducq_state ducq_srv_dispatch(ducq_srv *srv, ducq_i *ducq) {
	char buffer[BUFSIZ] = "";
	size_t size = sizeof(buffer);

	ducq_state recv_state = _recv_msg(ducq, buffer, &size);
	if(recv_state != DUCQ_OK) return recv_state;

	command_f command     = _find_command(srv, buffer);
	ducq_state cmd_rc     = command(srv, ducq, buffer, size);

	return cmd_rc;
}






//
//			C O M M A N D S   L O A D I N G
//

static
int _count_commands(const char *path) {
	int count = 0;

	DIR *dirp = opendir(path);
	if(dirp == NULL)
		return DUCQ_EFILE;

	struct dirent *dp;
	while( (dp = readdir(dirp)) ) {
		char *ext = strrchr(dp->d_name, '.');
		if(strcmp(ext, ".so") == 0)
			count++;
	}

	if(closedir(dirp) == -1)
		return DUCQ_EFILE;
	
	return count;
}
static
void _load_command(ducq_srv* srv, const char *path, struct dirent *dp) {
	char fullpath[PATH_MAX];
	char *name = dp->d_name;
	char *ext  = strrchr(name, '.');
	if(strcmp(ext, ".so") != 0)
		return;
	snprintf(fullpath, PATH_MAX, "%s/%s", path, name);

	void *handle = dlopen(fullpath, RTLD_NOW | RTLD_LOCAL);
	if(!handle) {
		ducq_log_error("server,dlopen() failed for: %s", dlerror());
		return;
	}
	
	(void)dlerror(); // clear
	struct ducq_cmd_t *cmd = dlsym(handle, "command");
	char *err = dlerror();
	if(!cmd || err) {
		ducq_log_error("server,dlsym() failed for %s: %s\n", name, err);
		dlclose(handle);
		return;
	}
	
	*(srv->cmds + srv->ncmd) = cmd;
	*(srv->hdls + srv->ncmd) = handle;

	srv->ncmd++;
}
ducq_state ducq_srv_load_commands_path(ducq_srv* srv, const char *path) {
	if(path == NULL)
		 path = "/usr/local/lib/ducq_commands";

	int ncmd = _count_commands(path);
	if( ncmd < 0 ) return ncmd;

	void *mem = malloc( sizeof(void*) * ncmd * 2 );
	if(!mem) return DUCQ_EMEMFAIL;
	srv->cmds = mem;
	srv->hdls = mem + (sizeof(void*) * ncmd);
	srv->ncmd = 0;


	DIR *dirp = opendir(path);
	if(dirp == NULL) return DUCQ_EFILE;

	struct dirent *dp;
	while( (dp = readdir(dirp)) && srv->ncmd < ncmd )
		_load_command(srv, path, dp);

	if(closedir(dirp) == -1) return DUCQ_EFILE;


	return DUCQ_OK;
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


static
void _no_log(void *ctx, const char *function_name, enum ducq_log_level level, const char *fmt, va_list args) {
	;
}
static
void _color_console_log(void *ctx, const char *function_name, enum ducq_log_level level, const char *fmt, va_list args) {
	(void) ctx; // unused
	
	char now[] = "YYYY-MM-DDTHH:MM:SS";
	time_t timer = time(NULL);
	strftime(now, sizeof(now), "%F%T", localtime(&timer));
	
	switch(level) {
		case DUCQ_LOG_DEBUG   : printf("\033[92m"); break;
		case DUCQ_LOG_INFO    : printf("\033[39m"); break;
		case DUCQ_LOG_WARNING : printf("\033[93m"); break;
		case DUCQ_LOG_ERROR   : printf("\033[91m"); break;
	}
	printf("%s  %s  ", now, function_name);
	vprintf(fmt, args);
	printf("\n");
	printf("\033[39m");
}
void ducq_srv_set_default_log(ducq_srv *srv) {
	srv->log     = _color_console_log;
}


void ducq_srv_log(ducq_srv *srv, const char *function_name, enum ducq_log_level level, const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
		srv->log(srv->log_ctx, function_name, level, fmt, args);
	va_end(args);
	

	if(! srv->allow_monitor_route)
		return;

	va_start(args, fmt);
		char buffer[DUCQ_MSGSZ] = "";
		size_t len = 0;
		len +=  snprintf(buffer    , DUCQ_MSGSZ    , "%s,%s,", ducq_loglevel_tostring(level), function_name);
		len += vsnprintf(buffer+len, DUCQ_MSGSZ-len, fmt    , args);
	va_end(args);

	ducq_sub *next = NULL;
	for(ducq_sub *sub = srv->subs; sub; sub = next) {
		next = sub->next;
		size_t size = len;

		if( strcmp(sub->route, DUCQ_MONITOR_ROUTE) == 0 ) { // exact match: don't send to * routes
			if( ducq_send(sub->ducq, buffer, &size) != DUCQ_OK)
				ducq_srv_unsubscribe(srv, sub->ducq);
		}
	}
}






//
//			C O N S T U C T O R   /   D E S T R U C T O R
//

ducq_srv *ducq_srv_new() {
	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return NULL;
		
	ducq_srv* srv = malloc(sizeof(ducq_srv));
	if(!srv) return NULL;

	srv->subs = NULL;
	srv->cmds = NULL;
	srv->hdls = NULL;
	srv->ncmd = 0;

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

	if(srv->hdls) {
		for(int i = 0; i < srv->ncmd; i++)
			dlclose(srv->hdls[i]);
	}

	if(srv->cmds)
		free(srv->cmds);

	ducq_sub *sub = srv->subs;
	while(sub) {
		ducq_sub *next = sub->next;
		ducq_sub_free(sub);
		sub = next;
	}

	free(srv);
}




#endif // #ifndef __linux__