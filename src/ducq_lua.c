#include "lua.h"
#include "lauxlib.h"

#include "ducq.h"

#include "ducq_lua.h"


static
int _conn(lua_State *L) {
	ducq_i *ducq = *(ducq_i**) luaL_checkudata(L, 1, DUCQ_METATABLE);

	ducq_state state = ducq_conn(ducq);
	lua_pushinteger(L, state);
	return 1;
}


static
int _id(lua_State *L) {
	ducq_i *ducq = *(ducq_i**) luaL_checkudata(L, 1, DUCQ_METATABLE);
	const char *id = ducq_id(ducq);
	lua_pushstring(L, id);
	return 1;
}
static
int _eq(lua_State *L) {
	ducq_i *ducq_a = *(ducq_i**) luaL_checkudata(L, 1, DUCQ_METATABLE);
	ducq_i *ducq_b = *(ducq_i**) luaL_checkudata(L, 2, DUCQ_METATABLE);

	bool equal = ducq_eq(ducq_a, ducq_b);

	lua_pushboolean(L, equal);
	return 1;
}static
int _timeout(lua_State *L) {
	ducq_i *ducq = *(ducq_i**) luaL_checkudata(L, 1, DUCQ_METATABLE);
	int isnum = 0;
	int timeout = luaL_checkinteger(L, 2);

	ducq_state state = ducq_timeout(ducq, timeout);

	lua_pushinteger(L, state);
	return 1;
}

static
int _recv(lua_State *L) {
	ducq_i *ducq = *(ducq_i**) luaL_checkudata(L, 1, DUCQ_METATABLE);
	char msg[DUCQ_MSGSZ] = "";
	size_t len = DUCQ_MSGSZ;
	
	ducq_state state = ducq_recv(ducq, msg, &len);

	lua_pushlstring(L, msg, len);
	lua_pushinteger(L, state);
	return 2;
}

static
int _send(lua_State *L) {
	ducq_i *ducq = *(ducq_i**) luaL_checkudata(L, 1, DUCQ_METATABLE);
	size_t len = 0;
	const char *msg = luaL_checklstring(L, -1, &len);
	
	ducq_state state = ducq_send(ducq, msg, &len);
	
	lua_pushinteger(L, state);
	return 1;
}

static
int _close(lua_State *L) {
	ducq_i *ducq = *(ducq_i**) luaL_checkudata(L, 1, DUCQ_METATABLE);
	
	ducq_state state = ducq_close(ducq);
	
	lua_pushinteger(L, state);
	return 1;
}
static
int _sendack(lua_State *L) {
	ducq_i *ducq = *(ducq_i**) luaL_checkudata(L, 1, DUCQ_METATABLE);

	ducq_state state = ducq_send_ack(ducq, DUCQ_OK);

	lua_pushinteger(L, state);
	return 1;
}


static
int _iterator_gc(lua_State *L) {
	ducq_client_it *it = *(ducq_client_it**)luaL_checkudata(L, 1, DUCQ_ITERATOR_METATABLE);
	ducq_client_it_free(it);
	return 0;
}
static
int _clients_iteration(lua_State *L) {
	ducq_client_it *it = *(ducq_client_it**)lua_touserdata(L, lua_upvalueindex(1));

	char *route = NULL;
	ducq_i *ducq = ducq_next(it, &route);
	if(! ducq)
		return 0;

	ducq_push_ducq(L, ducq);
	lua_pushstring(L, route);
	return 2;
}
static
int _clients(lua_State *L) {
	ducq_i *ducq = *(ducq_i**) luaL_checkudata(L, 1, DUCQ_METATABLE);

	lua_getfield(L, LUA_REGISTRYINDEX, "reactor");
	ducq_reactor *reactor = (ducq_reactor*)lua_touserdata(L, -1);
	if(!reactor)
		luaL_error(L, "reactor not found, probably not server-side instance");

	ducq_client_it **it = (ducq_client_it**)lua_newuserdata(L, sizeof(ducq_client_it*));
	*it = NULL;

	luaL_getmetatable(L, DUCQ_ITERATOR_METATABLE);
	lua_setmetatable(L, -2);

	*it = ducq_new_client_it(reactor);
	if(*it == NULL)
		luaL_error(L, "ducq_newclient_it() failed.");

	lua_pushcclosure(L, _clients_iteration, 1);
	return 1;
}

static
ducq_loop_t _do_lua_callback(ducq_i *ducq, char *route, void *ctx) {
	lua_State *L = (lua_State*)ctx;

	lua_pushvalue(L, -1); // copy function: will be poped on lua_pcall()
	ducq_push_ducq(L, ducq);
	lua_pushstring(L, route);
	if(lua_pcall(L, 2, 1, 0) != LUA_OK)
		luaL_error(L, "error in reactor:loop() callback: %s", lua_tostring(L, -1));
	
	int isnum = 0;
	ducq_loop_t control = lua_tonumberx(L, -1, &isnum);
	if(!isnum)
		luaL_error(L, "error reactor:loop() callback must return a ducq_loop_t");

	lua_pop(L, 1);
	return control;
}
static
int _loop(lua_State *L) {
	ducq_reactor *reactor = *(ducq_reactor**) luaL_checkudata(L, 1, DUCQ_REACTOR_METATABLE);
	if( ! lua_isfunction(L, -1) )
		luaL_error(L, "reactor:loop() expected parameter to be a function");
	
	ducq_loop_t control = ducq_reactor_loop(reactor, _do_lua_callback, L);
	lua_pushinteger(L, control);
	return 1;
}

static
int _msg_tostring(lua_State *L) {
	luaL_checktype(L, 1, LUA_TTABLE);

	lua_getfield(L, 1, "command");
	lua_getfield(L, 1, "route");
	lua_getfield(L, 1, "payload");

	lua_pushfstring(L, "%s %s\n%s",
		lua_tostring(L, -3),
		lua_tostring(L, -2),
		lua_tostring(L, -1)
	);
	return 1;
}

static
const struct luaL_Reg ducqlib_f[] = {
	{ NULL,      NULL }
};


const struct luaL_Reg ducq_m[] = {
	{ "conn",    _conn    },
	{ "id",      _id      },
	{ "timeout", _timeout },
	{ "recv",    _recv    },
	{ "send",    _send    },
	{ "__eq",    _eq   },
	{ "close",   _close   },

	{ "sendack", _sendack },
	{ "clients", _clients },

	{ NULL, NULL }
};

static
const struct luaL_Reg reactor_m[] = {
	{ "loop",   _loop   },

	{ NULL, NULL }
};

static
const struct luaL_Reg msg_m[] = {
	{ "__tostring",   _msg_tostring },

	{ NULL, NULL }
};

int luaopen_LuaDucq(lua_State *L) {
	// ducq
	luaL_newmetatable(L, DUCQ_METATABLE);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pushstring(L, DUCQ_METATABLE);
	lua_setfield(L, -2, "__metatable");
	luaL_setfuncs(L, ducq_m, 0);

	// reactor
	luaL_newmetatable(L, DUCQ_REACTOR_METATABLE);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pushstring(L, DUCQ_REACTOR_METATABLE);
	lua_setfield(L, -2, "__metatable");

	lua_pushinteger(L, DUCQ_LOOP_CONTINUE);
	lua_setfield(L, -2, "continue");
	lua_pushinteger(L, DUCQ_LOOP_BREAK);
	lua_setfield(L, -2, "stop");
	lua_pushinteger(L, DUCQ_LOOP_DELETE);
	lua_setfield(L, -2, "delete");

	luaL_setfuncs(L, reactor_m, 0);

	// iterator
	luaL_newmetatable(L, DUCQ_ITERATOR_METATABLE);
	lua_pushcfunction(L, _iterator_gc);
	lua_setfield(L, -2, "__gc");
	lua_pushstring(L, DUCQ_ITERATOR_METATABLE);
	lua_setfield(L, -2, "__metatable");


	// msg
	luaL_newmetatable(L, DUCQ_MSG_METATABLE);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pushstring(L, DUCQ_MSG_METATABLE);
	lua_setfield(L, -2, "__metatable");

	luaL_setfuncs(L, msg_m, 0);

	// library
	luaL_newlib(L, ducqlib_f);
	return 1;

}




void ducq_push_ducq(lua_State *L, ducq_i *ducq) {
	ducq_i **ud = (ducq_i**) lua_newuserdata(L, sizeof(ducq_i**));
	*ud = ducq;
	luaL_getmetatable(L, DUCQ_METATABLE);
	lua_setmetatable(L, -2);
}
void ducq_push_reactor(lua_State *L, ducq_reactor *reactor) {
	ducq_reactor **ud = (ducq_reactor**) lua_newuserdata(L, sizeof(ducq_reactor**));
	*ud = reactor;
	luaL_getmetatable(L, DUCQ_REACTOR_METATABLE);
	lua_setmetatable(L, -2);
}
void ducq_push_msg(lua_State *L, struct ducq_msg *msg) {
	lua_createtable(L, 0, 3);

	lua_pushstring(L, msg->command);
	lua_setfield(L, -2, "command");

	lua_pushstring(L, msg->route);
	lua_setfield(L, -2, "route");

	lua_pushstring(L, msg->payload);
	lua_setfield(L, -2, "payload");

	luaL_getmetatable(L, DUCQ_MSG_METATABLE);
	lua_setmetatable(L, -2);
}
