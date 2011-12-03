/*
 * tw_graphics.c
 *
 * This file contains the source code pertaining to the graphics handling
 * functions used in the ToyWrench application. The main purpose of this file is
 * to draw and update the display.
 */

#include <stdlib.h>
#include <png.h>
#include "SDL.h"
#include "SDL_image.h"
#include "tw_graphics.h"
#include "tw_lua.h"
#include "tw_error.h"

#define TW_SCREEN_WIDTH 1024
#define TW_SCREEN_HEIGHT 640

#define TW_MAX_SPRITES 64

typedef struct {
    SDL_Surface *src;
    unsigned int width;
    unsigned int height;
} tw_sprite_t;

static int initialized = 0;
static SDL_Surface *screen;
static tw_sprite_t texture_list[TW_MAX_SPRITES];
static unsigned int texture_list_size = 0;

/*
 * Loads the given image file and stores it into the texture list
 */
static int load_sprite(const char *img_file) {
    SDL_Surface *image;
    if( initialized ) {
        if( texture_list_size < TW_MAX_SPRITES ) {
            image = IMG_Load(img_file);
            if( image ) { /* Create optimized sprite */
                texture_list[texture_list_size].src = SDL_DisplayFormatAlpha(image);
                if( texture_list[texture_list_size].src ) {
                    SDL_FreeSurface(image);
                }
                else {
                    push_warning("Failed to create optimized sprite! This may affect performance!");
                    texture_list[texture_list_size].src = image;
                }
                texture_list[texture_list_size].width = texture_list[texture_list_size].src->w;
                texture_list[texture_list_size].height = texture_list[texture_list_size].src->h;
                if( texture_list[texture_list_size].src->format->palette ) {
                    push_warning("Sprite has palette!");
                }
                texture_list_size++;
                return 0;
            }
            else {
                push_error("load_sprite failed: Failed to load given image file!");
                return -1;
            }
        }
        else {
            push_error("load_sprite failed: Too many sprites loaded!");
            return -1;
        }
    }
    else {
        push_error("load_sprite failed: Graphics interface not initialized!");
        return -1;
    }
}

/*
 * Takes the given RGB values and returns a Uint32 of the corresponding color.
 */
static Uint32 get_rgb_color( unsigned int r, unsigned int g, unsigned int b ) {
    return SDL_MapRGB(screen->format, 0xFF & r, 0xFF & g, 0xFF & b);
}

/*
 * Takes the given RGBA values and returns a Uint32 of the corresponding color.
 */
static Uint32 get_rgba_color( unsigned int r, unsigned int g, unsigned int b, unsigned int a ) {
    return SDL_MapRGBA(screen->format, 0xFF & r, 0xFF & g, 0xFF & b, 0xFF & a);
}

/*
 * Draws a line on the screen with the given color using the Bresenham line
 * algorithm.
 */
static int draw_line( unsigned int x0, unsigned int y0,
    unsigned int x1, unsigned int y1, Uint32 color ) {
    unsigned int x, y, x_end, y_end;
    int delta_x, delta_y, error, steep, y_step;
    if( !initialized ) {
        push_error("draw_line failed: Graphics interface not initialized!");
        return -1;
    }
    if( x0 >= TW_SCREEN_WIDTH || x1 >= TW_SCREEN_WIDTH ||
        y0 >= TW_SCREEN_HEIGHT || y1 >= TW_SCREEN_HEIGHT ) {
        push_error("draw_line failed: Out of bounds.");
        return -1;
    }
    if( x0 == x1 ) { /* vertical line */
        x = x0;
        if( y0 < y1 ) {
            y = y0;
            y_end = y1;
        }
        else {
            y = y1;
            y_end = y0;
        }
        for( ; y <= y_end; y++ ) {
            ((Uint32*)(screen->pixels))[TW_SCREEN_WIDTH * y + x] = color;
        }
    }
    else if( y0 == y1 ) { /* horizontal line */
        y = y0;
        if( x0 < x1 ) {
            x = x0;
            x_end = x1;
        }
        else {
            x = x1;
            x_end = x0;
        }
        for( ; x <= x_end; x++ ) {
            ((Uint32*)(screen->pixels))[TW_SCREEN_WIDTH * y + x] = color;
        }
    }
    else { /* diagonal */
        if( (y0 > y1 ? y0 - y1 : y1 - y0) > (x0 > x1 ? x0 - x1 : x1 - x0) ) {
            steep = 1;
        }
        else {
            steep = 0;
        }
        if( steep ) {
            x = y0;
            y = x0;
            x_end = y1;
            y_end = x1;
        }
        else {
            x = x0;
            y = y0;
            x_end = x1;
            y_end = y1;
        }
        if( x > x_end ) {
            delta_x = x;
            delta_y = y;
            x = x_end;
            y = y_end;
            x_end = delta_x;
            y_end = delta_y;
        }
        delta_x = x_end - x;
        delta_y = y > y_end ? y - y_end : y_end - y;
        error = delta_x / 2;
        if( y < y_end ) {
            y_step = 1;
        }
        else {
            y_step = -1;
        }
        for( ; x <= x_end; x++ ) {
            if( steep ) {
                ((Uint32*)(screen->pixels))[TW_SCREEN_WIDTH * x + y] = color;
            }
            else {
                ((Uint32*)(screen->pixels))[TW_SCREEN_WIDTH * y + x] = color;
            }
            error -= delta_y;
            if( error < 0 ) {
                y += y_step;
                error += delta_x;
            }
        }
    }
    return 0;
}

/*
 * Draws all necessary sprites
 */
static int draw_sprite( int texture, int x, int y ) {
    SDL_Rect dest;
    if( initialized ) {
        if( (unsigned int)texture < texture_list_size ) {
            dest.x = x;
            dest.y = y;
            dest.w = 0; /* width and height are ignored */
            dest.h = 0;
            SDL_BlitSurface(texture_list[texture].src, NULL, screen, &dest);
            return 0;
        }
        else {
            push_error("draw_sprite failed: Invalid texture!");
            return -1;
        }
    }
    else {
        push_error("draw_sprite failed: Graphics interface not initialized!");
        return -1;
    }
}

/*
 * Redraws the screen.
 */
int display() {
    if( initialized ) {
        SDL_FillRect(screen, NULL, 0);
        if( run_lua_function("tw_display") ) {
            push_error("Call to Lua function display failed!");
            return -1;
        }
        SDL_Flip(screen);
        return 0;
    }
    else {
        push_error("display failed: Graphics interface not initialized!");
        return -1;
    }
}

/*
 * Lua hook to the function
 * load_sprite( const char *img_file )
 */
int lua_loadTexture( lua_State *L ) {
    const char *filename;
    if( lua_gettop(L) == 0 ) {
        push_error("Lua: Error while calling loadTexture: Not enough arguments!");
        lua_pushstring(L, "Too few arguments.");
        lua_error(L);
        return -1;
    }
    else {
        filename = lua_tostring(L, 1);
        if( load_sprite(filename) ) {
            push_error("Lua: Error while calling loadTexture!");
            lua_pushstring(L, "Error while loading texture.");
            lua_error(L);
            return -1;
        }
        else {
            lua_pop(L, lua_gettop(L)); /* clear stack */
            lua_pushnumber(L, texture_list_size-1);
            return 1;
        }
    }
}

/*
 * Returns the properly formatted color when converted from the table at the
 * given index in the given Lua state containing either a set of RGB or RGBA
 * colors.
 */
Uint32 lua_convertColor( lua_State *L, int index ) {
    unsigned int r, g, b, a;
    size_t length;
    if( lua_istable(L, index) ) {
        length = lua_objlen(L, index);
        if( length < 3 ) {
            push_error("Lua: Error while trying to convert color: Not enough parameters!");
            lua_pushstring(L, "Too few parameters.");
            lua_error(L);
            return 0;
        }
        else {
            lua_pushnumber(L, 1); /* Lua indices start at 1 */
            lua_gettable(L, index);
            r = lua_tonumber(L, -1);
            lua_pop(L, 1);
            lua_pushnumber(L, 2);
            lua_gettable(L, index);
            g = lua_tonumber(L, -1);
            lua_pop(L, 1);
            lua_pushnumber(L, 3);
            lua_gettable(L, index);
            b = lua_tonumber(L, -1);
            lua_pop(L, 1);
            if( length > 3 ) {
                lua_pushnumber(L, 4);
                lua_gettable(L, index);
                a = lua_tonumber(L, -1);
                lua_pop(L, 1);
                return get_rgba_color(r, g, b, a);
            }
            else {
                return get_rgb_color(r, g, b);
            }
        }
    }
    else {
        push_error("Lua: Error while trying to convert color: Given index is not a table!");
        lua_pushstring(L, "Given parameter not a table.");
        lua_error(L);
        return 0;
    }
}

/*
 * Lua hook to the function
 * draw_line( unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, Uint32 color )
 */
int lua_drawLine( lua_State *L ) {
    unsigned int x0, y0, x1, y1;
    Uint32 color;
    if( lua_gettop(L) != 5 ) {
        push_error("Lua: Error while calling drawLine: Not enough arguments!");
        lua_pushstring(L, "Too few arguments.");
        lua_error(L);
        return -1;
    }
    else {
        x0 = (unsigned int)lua_tonumber(L, 1);
        y0 = (unsigned int)lua_tonumber(L, 2);
        x1 = (unsigned int)lua_tonumber(L, 3);
        y1 = (unsigned int)lua_tonumber(L, 4);
        color = lua_convertColor(L, 5);
        if( draw_line(x0, y0, x1, y1, color) ) {
            push_error("Lua: Error while calling drawLine!");
            lua_pushstring(L, "Error while drawing line.");
            lua_error(L);
            return -1;
        }
        else {
            lua_pop(L, lua_gettop(L)); /* clear stack */
            return 0;
        }
    }
}

/*
 * Lua hook to the function
 * draw_sprite( int texture, int x, int y )
 */
int lua_drawTexture( lua_State *L ) {
    int texture;
    int x, y;
    switch( lua_gettop(L) ) {
        case 0:
        case 1:
        case 2:
            push_error("Lua: Error while calling drawTexture: Not enough arguments!");
            lua_pushstring(L, "Too few arguments.");
            lua_error(L);
            return -1;
        default:
            texture = lua_tonumber(L, 1);
            x = lua_tonumber(L, 2);
            y = lua_tonumber(L, 3);
    }
    if( (unsigned int)texture < texture_list_size ) {
        draw_sprite(texture, x, y);
        lua_pop(L, lua_gettop(L)); /* clear stack */
        return 0;
    }
    else {
        push_error("Lua: Error while calling drawTexture: Invalid texture!");
        lua_pushstring(L, "Invalid texture.");
        lua_error(L);
        return -1;
    }
}

/*
 * Lua callback to set the caption whenever GLOBALS.gameName is changed.
 */
int lua_setCaption( lua_State *L ) {
    char *cap;
    if( lua_gettop(L) < 1 ) {
        push_error("Lua: Error while setting caption: Not enough arguments!");
        lua_pushstring(L, "Too few arguments.");
        lua_error(L);
        return -1;
    }
    cap = (char*)lua_tolstring(L, -1, NULL);
    SDL_WM_SetCaption(cap, cap);
    lua_pop(L, 1);
    return 0;
}

/*
 * Lua callback to set the FPS cap whenever GLOBALS.fpsCap is changed.
 */
int lua_setFpsCap( lua_State *L ) {
    unsigned int cap;
    if( lua_gettop(L) < 1 ) {
        push_error("Lua: Error while setting fpsCap: Not enough arguments!");
        lua_pushstring(L, "Too few arguments.");
        lua_error(L);
        return -1;
    }
    cap = (unsigned int)lua_tonumber(L, -1);
    FPS = cap;
    lua_pop(L, 1);
    return 0;
}

/*
 * Initializes the graphics subsystem.
 */
int graphics_init() {
    if( initialized ) {
        push_warning("Graphics interface already initialized!");
        return 0;
    }
    else {
        if( SDL_InitSubSystem(SDL_INIT_VIDEO) ) {
            push_error("SDL Video failed to initialize!");
            return -1;
        }
        else {
            screen = SDL_SetVideoMode(TW_SCREEN_WIDTH, TW_SCREEN_HEIGHT, 32, SDL_FULLSCREEN|SDL_HWSURFACE|SDL_DOUBLEBUF); /*SDL_SWSURFACE);*/
            if( screen == NULL ) {
                push_error(SDL_GetError());
                push_error("SDL failed to set video mode!");
                return -1;
            }
            FPS = 40;
            add_lua_function("loadTexture", lua_loadTexture);
            add_lua_function("drawTexture", lua_drawTexture);
            add_lua_function("drawLine", lua_drawLine);
            add_lua_global_s("gameName", "Untitled", lua_setCaption);
            add_lua_global_n("fpsCap", 40, lua_setFpsCap);
            add_lua_global_n("screenWidth", TW_SCREEN_WIDTH, NULL);
            add_lua_global_n("screenHeight", TW_SCREEN_HEIGHT, NULL);
            SDL_WM_SetCaption("Untitled", "Untitled");
            initialized = 1;
            return 0;
        }
    }
}
