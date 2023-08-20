#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef __linux__
	#warning "as of now, ducq dispatcher is supported on linux only"
#else
	#include <dirent.h>
	#include <dlfcn.h>
	#include <linux/limits.h> // PATH_MAX


#include "ducq.h"
#include "ducq_reactor.h"
#include "ducq_dispatcher.h"


struct ducq_dispatcher {
	ducq_reactor *reactor;
	struct ducq_cmd_t **cmds;
	void **hdls;
	int ncmd;
};



//
//			C O N S T U C T O R   /   D E S T R U C T O R
//

ducq_dispatcher *ducq_dispatcher_new(ducq_reactor *reactor) {
	ducq_dispatcher* dispatcher = malloc(sizeof(ducq_dispatcher));
	if(!dispatcher) return NULL;

	dispatcher->reactor  = reactor;
	dispatcher->cmds = NULL;
	dispatcher->hdls = NULL;
	dispatcher->ncmd = 0;

	return dispatcher;
}

void ducq_dispatcher_free(ducq_dispatcher* dispatcher) {
	if(!dispatcher) return;

	if(dispatcher->hdls) {
		for(int i = 0; i < dispatcher->ncmd; i++)
			dlclose(dispatcher->hdls[i]);
	}

	if(dispatcher->cmds)
		free(dispatcher->cmds);


	free(dispatcher);
}


int ducq_dispatcher_count_cmds(ducq_dispatcher *dispatcher) {
	return dispatcher->ncmd;
}
	
//
//			C O M M A N D S   L O A D I N G
//
static
int _count_commands(const char *path) {
	int count = 0;

	DIR *dirp = opendir(path);
	if(dirp == NULL)
		return -1;

	struct dirent *dp;
	while( (dp = readdir(dirp)) ) {
		char *ext = strrchr(dp->d_name, '.');
		if(strcmp(ext, ".so") == 0)
			count++;
	}

	if(closedir(dirp) == -1)
		return -1;
	
	return count;
}
static
void _load_command(ducq_dispatcher* dispatcher, const char *path, struct dirent *dp) {
	char fullpath[PATH_MAX];
	char *name = dp->d_name;
	char *ext  = strrchr(name, '.');
	if(strcmp(ext, ".so") != 0)
		return;
	snprintf(fullpath, PATH_MAX, "%s/%s", path, name);

	void *handle = dlopen(fullpath, RTLD_NOW | RTLD_LOCAL);
	if(!handle) {
		ducq_reactor_log(dispatcher->reactor, DUCQ_LOG_ERROR, __func__, "server", "dlopen() failed for: %s", dlerror());
		return;
	}
	
	(void)dlerror(); // clear
	struct ducq_cmd_t *cmd = dlsym(handle, "command");
	char *err = dlerror();
	if(!cmd || err) {
		ducq_reactor_log(dispatcher->reactor, DUCQ_LOG_ERROR, __func__, "server", "dlsym() failed for %s: %s\n", name, err);
		dlclose(handle);
		return;
	}
	
	*(dispatcher->cmds + dispatcher->ncmd) = cmd;
	*(dispatcher->hdls + dispatcher->ncmd) = handle;

	dispatcher->ncmd++;
}
ducq_state ducq_dispatcher_load_commands_path(ducq_dispatcher *dispatcher, const char *path) {
	if(path == NULL)
		 path = "/usr/local/lib/ducq_commands";

	int ncmd = _count_commands(path);
	if( ncmd < 0 ) return DUCQ_EFILE;

	void *mem = malloc( sizeof(void*) * ncmd * 2 );
	if(!mem) return DUCQ_EMEMFAIL;
	dispatcher->cmds = mem;
	dispatcher->hdls = mem + (sizeof(void*) * ncmd);
	dispatcher->ncmd = 0;


	DIR *dirp = opendir(path);
	if(dirp == NULL) return DUCQ_EFILE;

	struct dirent *dp;
	while( (dp = readdir(dirp)) && dispatcher->ncmd < ncmd )
		_load_command(dispatcher, path, dp);

	if(closedir(dirp) == -1) return DUCQ_EFILE;


	return DUCQ_OK;
}



//
//			D I S P A T C H
//


ducq_state unknown(ducq_reactor *reactor, ducq_i *ducq, char *buffer, size_t size) {
	struct ducq_msg msg = ducq_parse_msg(buffer);
	ducq_log(WARN, "%s,%s", msg.command, msg.route);

	ducq_send_ack(ducq, DUCQ_ENOCMD);
	return DUCQ_ENOCMD;
}
ducq_state list_commands(ducq_reactor *reactor, ducq_i *ducq, char *buffer, size_t size) {
	(void) buffer;
	(void) size;
	ducq_log(INFO, "");

	struct ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);

	char payload[DUCQ_MSGSZ] =
		"list_commands,list all loaded commands as csv: <name>,<doc>\n";
	size_t len = 0;
	char *ptr = payload + strlen(payload);
	char *end = payload + DUCQ_MSGSZ;
	for(int i = 0; i < dispatcher->ncmd; i++) {
		struct ducq_cmd_t *cmd = dispatcher->cmds[i];
	
		size_t left = end-ptr;
		len = snprintf(ptr, left, "%s,%s\n", cmd->name, cmd->doc);

		ptr += len;
		if(ptr >= end) break;
	}

	len = ptr - payload;
	ducq_state state = ducq_send(ducq, payload, &len);
	return state;
}
static
ducq_command_f _find_command(ducq_dispatcher *dispatcher, const char *msg) {
	char *end = NULL;
	const char *name = ducq_parse_command(msg, (const char**) &end);
	if (!name) return unknown;
	*end = '\0';

	if(strcmp(name, "list_commands") == 0 || strcmp(name, "help") == 0)
		return list_commands;

	ducq_command_f command = unknown;

	for(int i = 0; i < dispatcher->ncmd ; i++) {
		if(strcmp(name, dispatcher->cmds[i]->name) == 0) {
			command = dispatcher->cmds[i]->exec;
			break;
		}
	}

	*end = ' ';
	return command;
}
ducq_state ducq_dispatch(ducq_dispatcher *dispatcher, ducq_i *ducq, char *msg, size_t size) {
	ducq_command_f command = _find_command(dispatcher, msg);
	return command(dispatcher->reactor, ducq, msg, size);
}



#endif // #ifndef __linux__
