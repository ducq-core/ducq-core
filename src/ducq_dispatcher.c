#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifndef __linux__
	#warning "as of now, ducq dispatcher is supported on linux only"
#else
	#include <dirent.h>
	#include <dlfcn.h>
	#include <linux/limits.h> // PATH_MAX
	#include <unistd.h>       // close()
	#include <sys/stat.h>	  // is directory or file
	#include <sys/inotify.h>


#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "ducq.h"
#include "ducq_reactor.h"
#include "ducq_dispatcher.h"
#include "ducq_lua.h"

#define MAX_EXTENSIONS 10

#define log(level, fmt, ...) ducq_reactor_log(reactor, DUCQ_LOG_##level, __func__, "dispatcher", fmt ,##__VA_ARGS__)


struct ducq_dispatcher {
	ducq_reactor *reactor;

	struct ducq_cmd_t **cmds;
	void **hdls;
	int ncmd;

	lua_State* L;
	int notifyfd;
	struct {
		int wd;
		char path[PATH_MAX];
	} extensions[MAX_EXTENSIONS];
};



//
//			C O N S T U C T O R   /   D E S T R U C T O R
//

static
lua_State *_make_lua(ducq_reactor *reactor) {
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	static char require[] = "Ducq = require('" LUA_DUCQ_PACKAGE_NAME "')";
	log(INFO, "loading: '%s'\n", LUA_DUCQ_PACKAGE_NAME);
	int error = luaL_loadstring(L, require) || lua_pcall(L, 0, 0, 0);
	if(error) {
		log(ERROR, "%s failed: %s", require, lua_tostring(L, -1));
		lua_close(L);
		return NULL;
	}

	lua_pushlightuserdata(L, reactor);
	lua_setfield(L, LUA_REGISTRYINDEX, "reactor");

	ducq_push_reactor(L, reactor);
	lua_setglobal(L, "reactor");

	lua_newtable(L);
	lua_setglobal(L, "commands");
	return L;

}
ducq_dispatcher *ducq_dispatcher_new(ducq_reactor *reactor) {
	int fd = inotify_init1(IN_NONBLOCK);
	if(fd == -1) return NULL;

	ducq_dispatcher* dispatcher = malloc(sizeof(ducq_dispatcher));
	if(!dispatcher) return NULL;

	dispatcher->reactor  = reactor;
	dispatcher->cmds     = NULL;
	dispatcher->hdls     = NULL;
	dispatcher->ncmd     = -1;

	for(int i = 0; i < MAX_EXTENSIONS; i++) {
		dispatcher->extensions[i].wd      = -1;
		dispatcher->extensions[i].path[0] = '\0';
	}

	dispatcher->L        = _make_lua(reactor);
	if(dispatcher->L) {
		dispatcher->notifyfd = fd;
		ducq_reactor_add_server(reactor, fd, ducq_dispatcher_accept_notify, dispatcher);
	} else {
		close(fd);
		dispatcher->notifyfd = -1;
	}

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
	if(dispatcher->L)
		lua_close(dispatcher->L);
	if(dispatcher->notifyfd != -1)
		close(dispatcher->notifyfd);

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
	if(!ext || strcmp(ext, ".so") != 0)
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
//			L U A   C O M M A N D   L O A D I N G
//
static
int _is_dir(char *path) {
	struct stat statbuf = {};
	return stat(path, &statbuf) == -1
		? -1
		: S_ISDIR(statbuf.st_mode);
}
static
ducq_state _load_lua_file(lua_State *L, ducq_reactor *reactor, const char *path) {
	log(INFO, "loading '%s'", path);
	int error = luaL_loadfile(L, path) || lua_pcall(L, 0, 0, 0);
	if(! error)
		return DUCQ_OK;
	log(ERROR, "%s", lua_tostring(L, -1));
	lua_pop(L, 1);
	return DUCQ_ELUA;
}
static
ducq_state _load_lua(lua_State *L, ducq_reactor *reactor, const char *path) {
	struct stat statbuf = {};
	if(stat(path, &statbuf) == -1) {
		log(ERROR, "stat() failed: %s", strerror(errno));
		return DUCQ_EFILE;
	}
	if(S_ISREG(statbuf.st_mode))
		return _load_lua_file(L, reactor, path);

	log(INFO, "loading '%s'", path);
	DIR *dirp = opendir(path);
	if(dirp == NULL)
		return DUCQ_EFILE;
	struct dirent *dp;
	while( (dp = readdir(dirp)) ) {
		char *name = dp->d_name;
		char *ext  = strrchr(name, '.');
		if(!ext || strcmp(ext, ".lua") != 0)
			continue;
		char fullpath[PATH_MAX] = "";
		snprintf(fullpath, PATH_MAX, "%s/%s", path, name);
		DUCQ_CHECK( _load_lua_file(L, reactor, fullpath) );
	}

	return closedir(dirp) == -1
		? DUCQ_EFILE
		: DUCQ_OK;
}
ducq_state ducq_dispatcher_add(ducq_dispatcher *dispatcher, const char *path) {
	ducq_reactor *reactor = dispatcher->reactor;
	log(INFO, "request to add '%s'", path);

	int i;
	for(i = 0; i < MAX_EXTENSIONS; i++) {
		if(dispatcher->extensions[i].wd == -1)
			break;
	}
	if(i == MAX_EXTENSIONS) return DUCQ_EMAX;

	int wd = inotify_add_watch(dispatcher->notifyfd, path, IN_CLOSE_WRITE | IN_MOVED_TO);
	if(wd == -1) {
		log(ERROR, "inotify_add_watch() failed: %s", strerror(errno));
		return DUCQ_EFILE;
	}
	
	dispatcher->extensions[i].wd = wd;
	char *name = dispatcher->extensions[i].path;
	strncpy(name, path, PATH_MAX);
	size_t len = strlen(name);
	if(len+2 < PATH_MAX && _is_dir(name) == 1 && name[len-1] != '/') {
		name[len]   = '/';
		name[len+1] = '\0';
	}

	log(INFO, "adding '%s' (%s)", name, _is_dir(name)? "dir" : "file");
	DUCQ_CHECK( _load_lua(dispatcher->L, dispatcher->reactor, name) );
	return  DUCQ_OK;
}

#define NOTIFY_BUFFER_SIZE ( (sizeof(struct inotify_event) + NAME_MAX + 1) )
void ducq_dispatcher_accept_notify(ducq_reactor *reactor, int fd, void *ctx) {
	(void)fd;
	ducq_dispatcher *dispatcher = (ducq_dispatcher*) ctx;

	char buffer[NOTIFY_BUFFER_SIZE] = "";
	int n = read(dispatcher->notifyfd, buffer, NOTIFY_BUFFER_SIZE);
	if(n ==  0) return;
	if(n == -1) {
		log(ERROR, "read() for inotify failed: %s (%d)", strerror(errno), errno);
		return;
	}

	char *ptr = buffer;
	char *end = buffer + n;
	while(ptr < end) {
		struct inotify_event *event = (struct inotify_event*) ptr;
		//log(INFO, "reloading: '%s'", event->name);
		
		int i;
		for(i = 0; i < MAX_EXTENSIONS; i++) {
			if(dispatcher->extensions[i].wd == event->wd)
				break;
		}
		if(i == MAX_EXTENSIONS) {
			log(ERROR, "wd not found for '%s'\n", event->name);
			return;
		}
		char path[PATH_MAX] = "";
		snprintf(path, PATH_MAX, "%s%s", dispatcher->extensions[i].path, event->name);
		log(INFO, "reloading: '%s'", path);
		_load_lua(dispatcher->L, reactor, path);

		ptr += sizeof(struct inotify_event) + event->len;
	}
}







//
//			D I S P A T C H
//


ducq_state unknown(ducq_reactor *reactor, ducq_i *ducq, char *buffer, size_t size) {
	struct ducq_msg msg = ducq_parse_msg(buffer);
	ducq_log(WARNING, "%s,%s", msg.command, msg.route);

	ducq_send_ack(ducq, DUCQ_ENOCMD);
	return DUCQ_ENOCMD;
}

static // function must be on top of lua stack
ducq_state lua_command(ducq_reactor *reactor, ducq_i *ducq, char *buffer, size_t size) {
	ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);
	lua_State *L = dispatcher->L;

	struct ducq_msg msg = ducq_parse_msg(buffer);
	ducq_log(INFO, "runnning command '%s'", msg.command);

	ducq_state state = DUCQ_OK;
	ducq_push_ducq(L, ducq);
	ducq_push_msg(L, &msg);
	if( lua_pcall(L, 2, 1, 0) != LUA_OK ) {
		ducq_log( ERROR, "'%s': %s", msg.command, lua_tostring(L, -1) );
		state = DUCQ_ELUA;
	}
	else if( ! lua_isinteger(L, -1) ) {
		ducq_log(ERROR, "'%s' did not returned a state", msg.command);
		state = DUCQ_ELUA;
	}
	else {
		state = lua_tointeger(L, -1);
	}

	lua_pop(L, 1); // error/state
	return state;
}

ducq_state list_commands(ducq_reactor *reactor, ducq_i *ducq, char *_buffer, size_t _size) {
	(void) _buffer;
	(void) _size;
	ducq_log(INFO, "");

	struct ducq_dispatcher *dispatcher = ducq_reactor_get_dispatcher(reactor);

	ducq_parts(ducq);

	char payload[DUCQ_MSGSZ] =
		"\"list_commands\",\"list all loaded commands as csv: <name>,<doc>\"\n";
	size_t size = strlen(payload);
	DUCQ_CHECK( ducq_send(ducq, payload, &size) );

	for(int i = 0; i < dispatcher->ncmd; i++) {
		struct ducq_cmd_t *cmd = dispatcher->cmds[i];
		size = snprintf(payload, DUCQ_MSGSZ, "\"%s\",\"%s\"\n", cmd->name, cmd->doc);
		DUCQ_CHECK( ducq_send(ducq, payload, &size) );
	}


	lua_State *L = dispatcher->L;
	if( lua_getglobal(L, "commands") != LUA_TTABLE) {
		return ducq_end(ducq);;
	}

	for(lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1) ) {
		if( !lua_isstring(L, -2) || !lua_istable(L, -1) ) continue;
		if( lua_getfield(L, -1, "doc") == LUA_TSTRING) {
			size = snprintf(payload, DUCQ_MSGSZ,
				"\"%s\",\"%s\"\n",
				lua_tostring(L, -3), lua_tostring(L, -1) );
			DUCQ_CHECK( ducq_send(ducq, payload, &size) );
		}
		lua_pop(L, 1);
	}

	return ducq_end(ducq);;
}

static
ducq_command_f _find_lua_command(ducq_dispatcher *dispatcher, const char *name) {
	lua_State *L = dispatcher->L;
	if( lua_getglobal(L, "commands") != LUA_TTABLE ) return unknown;

	lua_pushstring(L, name);
	if( lua_gettable(L, -2) != LUA_TTABLE ) return unknown;

	lua_pushstring(L, "exec");
	if( lua_gettable(L, -2) != LUA_TFUNCTION ) return unknown;

	return lua_command;
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

	if(command == unknown)
		command = _find_lua_command(dispatcher, name);

	*end = ' ';
	return command;
}
ducq_state ducq_dispatch(ducq_dispatcher *dispatcher, ducq_i *ducq, char *msg, size_t size) {
	ducq_command_f command = _find_command(dispatcher, msg);
	return command(dispatcher->reactor, ducq, msg, size);
}


#undef log
#endif // #ifndef __linux__
