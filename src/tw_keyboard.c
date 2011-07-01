/*
 * tw_keyboard.c
 *
 * This file contains the source code pertaining to the keyboard handling
 * functions used in the ToyWrench application. The main purpose of this file is
 * to pass on SDL keyboard events to the Lua interface so that the keyboard
 * events table can be built.
 */

#include "tw_lua.h"
#include "tw_keyboard.h"

static char* sdl_to_string_map[] = {
    "left",
    "right",
    "up",
    "down",
    "space",
    "escape",
    "a",
    "b",
    "c",
    "d",
    "e",
    "f",
    "g",
    "h",
    "i",
    "j",
    "k",
    "l",
    "m",
    "n",
    "o",
    "p",
    "q",
    "r",
    "s",
    "t",
    "u",
    "v",
    "w",
    "x",
    "y",
    "z",
    "unknown"
};

int handle_keyboard( SDL_Event *event ) {
    return lua_keyboard(event);
}

char* event_to_string( SDLKey key ) {
    switch( key ) {
        case SDLK_LEFT:  return sdl_to_string_map[0];
        case SDLK_RIGHT: return sdl_to_string_map[1];
        case SDLK_UP:    return sdl_to_string_map[2];
        case SDLK_DOWN:  return sdl_to_string_map[3];
        case SDLK_SPACE: return sdl_to_string_map[4];
        case SDLK_ESCAPE: return sdl_to_string_map[5];
        case SDLK_a: return sdl_to_string_map[6];
        case SDLK_b: return sdl_to_string_map[7];
        case SDLK_c: return sdl_to_string_map[8];
        case SDLK_d: return sdl_to_string_map[9];
        case SDLK_e: return sdl_to_string_map[10];
        case SDLK_f: return sdl_to_string_map[11];
        case SDLK_g: return sdl_to_string_map[12];
        case SDLK_h: return sdl_to_string_map[13];
        case SDLK_i: return sdl_to_string_map[14];
        case SDLK_j: return sdl_to_string_map[15];
        case SDLK_k: return sdl_to_string_map[16];
        case SDLK_l: return sdl_to_string_map[17];
        case SDLK_m: return sdl_to_string_map[18];
        case SDLK_n: return sdl_to_string_map[19];
        case SDLK_o: return sdl_to_string_map[20];
        case SDLK_p: return sdl_to_string_map[21];
        case SDLK_q: return sdl_to_string_map[22];
        case SDLK_r: return sdl_to_string_map[23];
        case SDLK_s: return sdl_to_string_map[24];
        case SDLK_t: return sdl_to_string_map[25];
        case SDLK_u: return sdl_to_string_map[26];
        case SDLK_v: return sdl_to_string_map[27];
        case SDLK_w: return sdl_to_string_map[28];
        case SDLK_x: return sdl_to_string_map[29];
        case SDLK_y: return sdl_to_string_map[30];
        case SDLK_z: return sdl_to_string_map[31];
        default: return sdl_to_string_map[32];
    }
    return sdl_to_string_map[6];
}

int keyboard_init() {
    /* stub */
    return 0;
}
