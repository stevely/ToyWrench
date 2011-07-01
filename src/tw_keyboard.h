/*
 * tw_keyboard.h
 */

#ifndef TWKEYBOARD
#define TWKEYBOARD

#include "SDL.h"

int handle_keyboard( SDL_Event *event );

int keyboard_init();

char* event_to_string( SDLKey key );

#endif
