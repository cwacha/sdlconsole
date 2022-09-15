#Makefile for SDL_Console

OBJS=SDL_console.o DT_drawtext.o internal.o
FLAGS=-Wall -g -O2
INCDIR=`sdl-config --cflags`
LIBS=`sdl-config --libs` -lSDL_image
CC=gcc

all: libSDL_console.a libSDL_console.so

libSDL_console.so:
	$(LD) -shared $(OBJS) -soname libSDL_console.so.2 -o libSDL_console.so.2

libSDL_console.a: $(OBJS)
	ar cru libSDL_console.a $(OBJS)

%.o: %.c
	$(CC) $(FLAGS) $(INCDIR) -fPIC -c $< -o $@

install:
	install -d /usr/local/lib
	install -d /usr/local/include
	install libSDL_console.so.2 /usr/local/lib
	ln -sf libSDL_console.so.2 /usr/local/lib/libSDL_console.so
	install libSDL_console.a /usr/local/lib
	install SDL_console.h /usr/local/include
	install DT_drawtext.h /usr/local/include

uninstall:
	rm -f /usr/local/lib/libSDL_console.so.2
	rm -f /usr/local/lib/libSDL_console.so
	rm -f /usr/local/lib/libSDL_console.a
	rm -f /usr/local/include/SDL_console.h
	rm -f /usr/local/include/DT_drawtext.h

clean:
	rm -f $(OBJS) libSDL_console.so.2 libSDL_console.a core
