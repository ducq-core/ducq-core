#ifndef __DUCQ_LUA_HEADER__
#define __DUCQ_LUA_HEADER__

#include "ducq.h"
#include "ducq_reactor.h"

#define DUCQ_METATABLE "Ducq.Ducq"
#define DUCQ_REACTOR_METATABLE "Ducq.Reactor"
#define DUCQ_MSG_METATABLE "Ducq.Msg"

void ducq_push_ducq(lua_State *L, ducq_i *ducq); 
void ducq_push_reactor(lua_State *L, ducq_reactor *reactor);
void ducq_push_msg(lua_State *L, struct ducq_msg *msg);

#endif // __DUCQ_LUA_HEADER__
