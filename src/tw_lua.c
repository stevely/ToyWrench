/*
 * tw_lua.c
 *
 * This file contains the source code pertaining to the Lua handling funtions
 * used in the ToyWrench application. The main purpose of this group of
 * functions is to abstract away the actual Lua state and API from the rest of
 * the ToyWrench engine. Any interaction between the C source code and the Lua
 * state should be done through one of the Lua handler functions.
 */

#include "tw_lua.h"
#include "tw_error.h"
#include "tw_keyboard.h"
#include "tw_mouse.h"

#define GLOBALS "GLOBALS"

static lua_State *state;
static int initialized = 0;
static int sticky_keys = 0;

/*
 * Adds the given string to the GLOBALS Lua table. This value may later be modified.
 * An optional pointer to a Lua-C function can be passed that will act as a
 * callback whenever the value is modified.
 */
int add_lua_global_s( const char *name, const char *value, lua_CFunction fn ) {
    if( initialized ) {
        lua_getglobal(state, GLOBALS);
        lua_pushstring(state, "_values");
        lua_rawget(state, -2);
        lua_pushstring(state, name);
        lua_pushstring(state, value);
        lua_rawset(state, -3);
        lua_pop(state, 1);
        if( fn != NULL ) {
            lua_pushstring(state, "_fns");
            lua_rawget(state, -2);
            lua_pushstring(state, name);
            lua_pushcfunction(state, fn);
            lua_rawset(state, -3);
            lua_pop(state, 1);
        }
        lua_pop(state, 1);
        return 0;
    }
    else {
        push_error("add_lua_global_s failed: Lua interface not initialized!");
        return -1;
    }
}

/*
 * Sets the given string in the GLOBALS Lua table. This value may later be
 * modified. This function should only be used to modify an existing value. To
 * add a new value, use add_lua_global_s instead.
 */
int set_lua_global_s( const char *name, const char *value ) {
    if( initialized ) {
        lua_getglobal(state, GLOBALS);
        lua_pushstring(state, "_values");
        lua_rawget(state, -2);
        lua_pushstring(state, name);
        lua_pushstring(state, value);
        lua_rawset(state, -3);
        lua_pop(state, 2);
        return 0;
    }
    else {
        push_error("set_lua_global_n failed: Lua interface not initialized!");
        return -1;
    }
}

/*
 * Adds the given number to the GLOBALS Lua table. This value may later be modified.
 * An optional pointer to a Lua-C function can be passed that will act as a
 * callback whenever the value is modified.
 */
int add_lua_global_n( const char *name, int value, lua_CFunction fn ) {
    if( initialized ) {
        lua_getglobal(state, GLOBALS);
        lua_pushstring(state, "_values");
        lua_rawget(state, -2);
        lua_pushstring(state, name);
        lua_pushnumber(state, value);
        lua_rawset(state, -3);
        lua_pop(state, 1);
        if( fn != NULL ) {
            lua_pushstring(state, "_fns");
            lua_rawget(state, -2);
            lua_pushstring(state, name);
            lua_pushcfunction(state, fn);
            lua_rawset(state, -3);
            lua_pop(state, 1);
        }
        lua_pop(state, 1);
        return 0;
    }
    else {
        push_error("add_lua_global_n failed: Lua interface not initialized!");
        return -1;
    }
}

/*
 * Sets the given number in the GLOBALS Lua table. This value may later be
 * modified. This function should only be used to modify an existing value. To
 * add a new value, use add_lua_global_n instead.
 */
int set_lua_global_n( const char *name, int value ) {
    if( initialized ) {
        lua_getglobal(state, GLOBALS);
        lua_pushstring(state, "_values");
        lua_rawget(state, -2);
        lua_pushstring(state, name);
        lua_pushnumber(state, value);
        lua_rawset(state, -3);
        lua_pop(state, 2);
        return 0;
    }
    else {
        push_error("set_lua_global_n failed: Lua interface not initialized!");
        return -1;
    }
}

/*
 * Binds the given properly formatted C function to a Lua function of the given
 * name. Consult the Lua documentation for information on how to properly format
 * a C function so that it may be bound in such a way.
 */
int add_lua_function( const char *name, lua_CFunction fn ) {
    if( initialized ) {
        lua_pushcfunction(state, fn);
        lua_setglobal(state, name);
        return 0;
    }
    else {
        push_error("add_lua_function failed: Lua interface not initialized!");
        return -1;
    }
}

/*
 * Runs the given Lua function name.
 */
int run_lua_function( const char *name ) {
    if( initialized ) {
        lua_getfield(state, LUA_GLOBALSINDEX, name);
        if( !lua_isfunction(state, lua_gettop(state)) ) {
            push_error("run_lua_function failed: Value is not a function!");
            return -1;
        }
        else {
            lua_call(state, 0, 0);
            return 0;
        }
    }
    else {
        push_error("run_lua_function failed: Lua interface not initialized!");
        return -1;
    }
}

int lua_keyboard_sticky( SDL_Event *event ) {
    if( event->key.type == SDL_KEYDOWN ) {
        lua_getglobal(state, "eventList");
        lua_pushstring(state, "keyPressed");
        lua_rawget(state, -2);
        if( lua_isnoneornil(state, -1) ) {
            lua_pop(state, 1);
            lua_pushstring(state, "keyPressed");
            lua_newtable(state);
            lua_rawset(state, -3);
            lua_pushstring(state, "keyPressed");
            lua_rawget(state, -2);
            lua_pushstring(state, "key");
            lua_newtable(state);
            lua_rawset(state, -3);
        }
        lua_pushstring(state, "key");
        lua_rawget(state, -2);
        lua_pushstring(state, event_to_string(event->key.keysym.sym));
        lua_pushboolean(state, 1);
        lua_rawset(state, -3);
        lua_pop(state, 3);
        return 0;
    }
    else {
        lua_getglobal(state, "eventList");
        lua_pushstring(state, "keyPressed");
        lua_rawget(state, -2);
        lua_pushstring(state, "key");
        lua_rawget(state, -2);
        lua_pushstring(state, event_to_string(event->key.keysym.sym));
        lua_pushboolean(state, 0);
        lua_rawset(state, -3);
        lua_pop(state, 3);
        return 0;
    }
}

/*
 * Reports a keyboard event to the Lua interface so that the Lua table
 * "eventList" can be properly updated.
 */
int lua_keyboard( SDL_Event *event ) {
    char *keystate;
    if( sticky_keys ) {
        return lua_keyboard_sticky(event);
    }
    if( event->key.type == SDL_KEYDOWN ) {
        keystate = "keyDown";
    }
    else {
        keystate = "keyUp";
    }
    lua_getglobal(state, "eventList");
    lua_pushstring(state, keystate);
    lua_rawget(state, -2);
    if( lua_isnoneornil(state, -1) ) {
        lua_pop(state, 1);
        lua_pushstring(state, keystate);
        lua_newtable(state);
        lua_rawset(state, -3);
        lua_pushstring(state, keystate);
        lua_rawget(state, -2);
        lua_pushstring(state, "key");
        lua_newtable(state);
        lua_rawset(state, -3);
    }
    lua_pushstring(state, "key");
    lua_rawget(state, -2);
    lua_pushstring(state, event_to_string(event->key.keysym.sym));
    lua_pushboolean(state, 1);
    lua_rawset(state, -3);
    lua_pop(state, 3);
    return 0;
}

/*
 * Reports a mouse event to the Lua interface so that the Lua table
 * "eventList" can be properly updated.
 */
int lua_mouse( unsigned int down, unsigned int button, int x, int y ) {
    lua_getglobal(state, "eventList");
    lua_pushstring(state, "mouse");
    lua_rawget(state, -2); /* eventList eventList.mouse */
    if( lua_isnoneornil(state, -1) ) {
        lua_pop(state, 1);
        lua_pushstring(state, "mouse");
        lua_newtable(state);
        lua_rawset(state, -3);
        lua_pushstring(state, "mouse");
        lua_rawget(state, -2);
        /*
        lua_pushnumber(state, button);
        lua_newtable(state);
        lua_rawset(state, -3);
        */
    }
    lua_pushnumber(state, button); /* eventList eventList.mouse button */
    lua_rawget(state, -2); /* eventList eventList.mouse eventList.mouse[button] */
    if( lua_isnoneornil(state, -1) ) {
        lua_pop(state, 1);
        lua_pushnumber(state, button);
        lua_newtable(state);
        lua_rawset(state, -3);
        lua_pushnumber(state, button);
        lua_rawget(state, -2);
    }
    /*lua_newtable(state);*/
    lua_pushstring(state, "down");
    lua_pushboolean(state, down);
    lua_rawset(state, -3);
    lua_pushstring(state, "x");
    lua_pushnumber(state, x);
    lua_rawset(state, -3);
    lua_pushstring(state, "y");
    lua_pushnumber(state, y);
    lua_rawset(state, -3); /* eventList eventList.mouse eventList.mouse[button] */
    /*lua_rawset(state, -3); *//* eventList.mouse[button] = newtable */
    lua_pop(state, 3);
    return 0;
}


/*
 * Resets the Lua table "eventList", allowing new events to be entered.
 */
int eventlist_reset() {
    if( !sticky_keys ) {
        lua_newtable(state);
        lua_setglobal(state, "eventList");
    }
    return 0;
}

/*
 * Lua callback to set the sticky_keys variable whenever GLOBALS.stickyKeys
 * is changed.
 */
int lua_setStickyKeys( lua_State *L ) {
    if( lua_gettop(L) < 1 ) {
        push_error("Lua: Error while setting stickyKeys: Not enough arguments!");
        lua_pushstring(L, "Too few arguments.");
        lua_error(L);
        return -1;
    }
    sticky_keys = lua_toboolean(L, -1);
    lua_pop(L, 1);
    return 0;
}

/*
 * Lua callback to generate a SDL_Quit event, causing the ToyWrench engine to
 * quit.
 */
int lua_signalQuit( lua_State *L ) {
    SDL_Event quit_event;
    quit_event.type = SDL_QUIT;
    if( SDL_PushEvent(&quit_event) ) {
        push_error("Lua: Failed to push quit event!");
        lua_pushstring(L, "Failed to signal for quit.");
        lua_error(L);
        return -1;
    }
    return 0;
}

/*
 * Initializes the Lua table "eventList".
 */
int eventlist_init() {
    lua_newtable(state);
    lua_setglobal(state, "eventList");
    return 0;
}

/*
 * Initializes the game logic, opening and running the given game file.
 */
int gamelogic_init( const char *file ) {
    int status;
    add_lua_global_n("stickyKeys", 0, lua_setStickyKeys);
    status = luaL_dofile(state, file);
    if( status ) {
        push_error(lua_tostring(state, -1));
    }
    return status;
}

/*
 * Initializes the GLOBALS meta table, as well as initializing the setMain and
 * setDisplay Lua functions.
 */
static int setup_lua_globals() {
    int status;
    status = luaL_dostring(state,
        "globals_meta = {\n"
        "    __newindex = function(t, k, v)\n"
        "        t._values[k] = v\n"
        "        if(t._fns[k] ~= nil) then\n"
        "            t._fns[k](v)\n"
        "        end\n"
        "    end,\n"
        "    __index = function(t, k)\n"
        "        return rawget(t, '_values')[k]\n"
        "    end\n"
        "}");
    if( !status ) {
        status = luaL_dostring(state, "setmetatable(GLOBALS, globals_meta)");
        if( !status ) {
            status = luaL_dostring(state,
                "tw_main = function()\n"
                "    print(\"Main not set!\")\n"
                "end");
            if( !status ) {
                status = luaL_dostring(state,
                    "setMain = function(fn)\n"
                    "    tw_main = fn\n"
                    "end");
                if( !status ) {
                    status = luaL_dostring(state,
                        "tw_display = function()\n"
                        "    print(\"Display not set!\")\n"
                        "end");
                    if( !status ) {
                        status = luaL_dostring(state,
                            "setDisplay = function(fn)\n"
                            "    tw_display = fn\n"
                            "end");
                    }
                }
            }
        }
    }
    if( status ) {
        push_error(lua_tostring(state, -1));
    }
    return status;
}

/*
 * Initializes the Lua subsystem and Lua state.
 */
int lua_init() {
    if( initialized ) {
        push_warning("Lua interface already initialized!");
        return 0;
    }
    else {
        state = lua_open();
        if( state == NULL ) {
            push_error("Failed to create Lua state!");
            return 1;
        }
        luaL_openlibs(state);
        lua_newtable(state);
        lua_setglobal(state, GLOBALS);
        lua_getglobal(state, GLOBALS);
        lua_pushstring(state, "_values");
        lua_newtable(state);
        lua_rawset(state, -3);
        lua_pushstring(state, "_fns");
        lua_newtable(state);
        lua_rawset(state, -3);
        lua_pop(state, 1);
        setup_lua_globals();
        initialized = 1;
        if( add_lua_function("quit", lua_signalQuit) ) {
            push_error("Failed to add quit function during Lua initialization!");
            initialized = 0;
            return 1;
        }
        return 0;
    }
}
