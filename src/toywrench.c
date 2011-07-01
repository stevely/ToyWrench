/*
 * toywrench.c
 *
 * This file contains the source code of the main execution thread for the
 * ToyWrench application. All subsystems of the ToyWrench engine should be
 * first initialized here, and any ToyWrench function call should ultimately be
 * lead back to main_loop() (or main() in the event the engine encounters an
 * error and cannot successfully start the main loop) so that any thrown errors
 * can be caught and dealt with appropriately.
 */

#include "SDL.h"
#include "SDL_main.h"
#include "tw_audio.h"
#include "tw_error.h"
#include "tw_graphics.h"
#include "tw_keyboard.h"
#include "tw_lua.h"
#include "tw_mouse.h"

unsigned long frame_count;

/*
 * Initializes SDL. This initializes the SDL timers as well.
 */
int sdlsetup_init() {
    if( SDL_Init(SDL_INIT_TIMER) ) {
        push_error(SDL_GetError());
        return -1;
    }
    return 0;
}

int handle_events() {
    /* stub */
    return 0;
}

/*
 * The main game loop. This controls when the screen is redrawn, as well as
 * handling events. To ensure that events are handled in a consistent manner,
 * event handling is limited to once per frame.
 */
int main_loop() {
    int status;
    SDL_Event event;
    status = 0;
    while( status == 0 ) {
        SDL_Delay(1000 / FPS);
        frame_count++;
        set_lua_global_n("frameCount", frame_count);
        eventlist_reset();
        while( SDL_PollEvent(&event) ) {
            switch( event.type ) {
                case SDL_KEYDOWN:
                    status = handle_keyboard(&event);
                    break;
                case SDL_KEYUP:
                    status = handle_keyboard(&event);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    status = handle_mouse(&event);
                    break;
                case SDL_QUIT:
                    SDL_Quit();
                    return 0;
                default:
                    break;
            }
        }
        if( status == 0 ) {
            status = handle_events();
        }
        if( status == 0 ) {
            status = run_lua_function("tw_main");
        }
        if( status == 0 ) {
            status = display();
        }
    }
    push_error("Fatal error encountered in main loop!");
    dump_stack_trace();
    return -1;
}

/*
 * Initializes the main components of the ToyWrench framework.
 */
int main(int argc, char** argv) {
    int status;
    status = 0;
    frame_count = 0;
    if( argc != 2 ) {
        push_error("No game file selected!");
        status = -1;
    }
    else {
        if( sdlsetup_init() ) {
            push_error("SDL failed to initialize!");
            status = -1;
        }
        if( lua_init() ) {
            push_error("Lua interface failed to initialize!");
            status = -1;
        }
        if( graphics_init() ) {
            push_error("Graphics interface failed to initialize!");
            status = -1;
        }
        if( audio_init() ) {
            push_error("Audio interface failed to initialize!");
            status = -1;
        }
        if( keyboard_init() ) {
            push_error("Keyboard interface failed to initialize!");
            status = -1;
        }
        if( mouse_init() ) {
            push_error("Mouse interface failed to initialize!");
            status = -1;
        }
        if( eventlist_init() ) {
            push_error("Event list failed to initialize!");
            status = -1;
        }
        if( gamelogic_init(argv[1]) ) {
            push_error("Game logic failed to initialize!");
            status = -1;
        }
    }
    if( status ) {
        push_error("Required component(s) failed to initialize!");
        dump_stack_trace();
        return -1;
    }
    else {
        add_lua_global_n("frameCount", frame_count, NULL);
        return main_loop();
    }
}
