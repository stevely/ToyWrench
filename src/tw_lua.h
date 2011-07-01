/*
 * tw_lua.h
 */

#ifndef TWLUA
#define TWLUA

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "SDL.h"

int add_lua_global_s( const char *name, const char *value, lua_CFunction fn );

int set_lua_global_s( const char *name, const char *value );

int add_lua_global_n( const char *name, int value, lua_CFunction fn );

int set_lua_global_n( const char *name, int value );

int add_lua_function( const char *name, lua_CFunction fn );

int add_lua_function( const char *name, lua_CFunction fn );

int run_lua_function( const char *name );

int lua_keyboard( SDL_Event *event );

int lua_mouse( unsigned int down, unsigned int button, int x, int y );

int eventlist_reset();

int eventlist_init();

int gamelogic_init();

int lua_init();

#endif
