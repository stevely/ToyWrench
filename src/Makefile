# Makefile for ToyWrench
# See ../INSTALL for installation instructions

# CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT

# Your platform. See PLATS for possible values.
PLAT= none

CC= gcc
CFLAGS= -g -02 -Wall `sdl-config --cflags` $(MYCFLAGS)
AR= ar cru
RANLIB= ranlib
RM= rm -f
LIBS= -llua -lpng $(MYLIBS) `sdl-config --libs`

MYCFLAGS= 
MYLDFLAGS=
MYLIBS= -lSDL_image
EXTRAS=

# END OF USER SETTINGS. PLEASE DO NOT CHANGE ANYTHING BELOW THIS LINE

PLATS= linux macosx

TW_E= toywrench
TW_S= toywrench.c tw_audio.c tw_error.c tw_graphics.c tw_keyboard.c tw_lua.c tw_mouse.c

all:	$(TW_E)

$(TW_E): $(TW_S)
	@echo "++Compiling the ToyWrench source files..."
	$(CC) $(CFLAGS) -o $@ $? $(LIBS)

clean:
	$(RM) $(TW_E)

none:
	@echo "Please choose a platform:"
	@echo "  $(PLATS)"

linux:
	$(MAKE) all

macosx:
	$(MAKE) all

.PHONY: all $(PLATS) o a clean none
