/*
 * tw_mouse.c
 *
 * This file contains the source code pertaining to the mouse handing fuctnions
 * used in the ToyWrench application. The main purpose of this file is to pass
 * on SDL mouse events to the Lua interface so that the mouse events table can
 * be built.
 */

#include "tw_error.h"
#include "tw_lua.h"
#include "tw_mouse.h"

int handle_mouse( SDL_Event *event ) {
    if( event->type == SDL_MOUSEBUTTONDOWN && event->button.state == SDL_PRESSED ) {
        return lua_mouse(1, event->button.button, event->button.x, event->button.y);
    }
    else if( event->type == SDL_MOUSEBUTTONUP && event->button.state == SDL_RELEASED ) {
        return lua_mouse(0, event->button.button, event->button.x, event->button.y);
    }
    else {
        push_error("Event passed to handle_mouse that is not a mouse button event!");
        return -1;
    }
}

int lua_setShowCursor( lua_State *L ) {
    if( lua_gettop(L) < 1 ) {
        push_error("Lua: Error while setting showCursor: Not enough arguments!");
        lua_pushstring(L, "Too few arguments.");
        lua_error(L);
        return -1;
    }
    SDL_ShowCursor(lua_toboolean(L, -1));
    lua_pop(L, 1);
    return 0;
}

int mouse_init() {
    return add_lua_global_n("showCursor", 1, lua_setShowCursor);
}
