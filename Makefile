# Makefile for ToyWrench
# See ../INSTALL for installation instructions

CC= clang
ANALYZER= clang --analyze
CFLAGS= -g -W -Wall -Werror $(shell sdl-config --cflags) $(MYCFLAGS)
AR= ar cru
RANLIB= ranlib
RM= rm -f
LIBS= -llua -lpng $(MYLIBS) $(shell sdl-config --libs)
SRC= src/

MYCFLAGS= 
MYLDFLAGS=
MYLIBS= -lSDL_image
EXTRAS=

TW_E= toywrench
TW_S= toywrench.c tw_audio.c tw_error.c tw_graphics.c tw_keyboard.c tw_lua.c \
                  tw_mouse.c

all: $(TW_E)

$(TW_E): $(addprefix $(SRC), $(TW_S:.c=.o))
	@echo "+++Building ToyWrench..."
	$(CC) $(CFLAGS) -o $@ $? $(LIBS)

%.o: %.c
	$(ANALYZER) $(CFLAGS) $?
	$(CC) $(CFLAGS) -I $(SRC) -c -o $@ $<
