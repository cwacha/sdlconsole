#Makefile for SDL_Console

OBJECTS=SDL_console.o DT_drawtext.o internal.o
FLAGS=-Wall -g -O2
INCLUDE=`sdl-config --cflags`
LIBS=`sdl-config --libs` -lSDL_image
CC=gcc

all: libSDL_console.a libSDL_console.so

libSDL_console.so: $(OBJECTS)
	$(CC) $(OBJECTS) $(FLAGS) -fpic -shared -o libSDL_console.so

libSDL_console.a: $(OBJECTS)
	ar cru libSDL_console.a $(OBJECTS)

%.o: %.c
	$(CC) $(FLAGS) $(INCLUDE) -fpic -c $< -o $@

install:
	cp libSDL_console.so /usr/local/lib
	cp libSDL_console.a /usr/local/lib
	cp SDL_console.h /usr/local/include
	cp DT_drawtext.h /usr/local/include

uninstall:
	rm -f /usr/local/lib/libSDL_console.so
	rm -f /usr/local/lib/libSDL_console.a
	rm -f /usr/local/include/SDL_console.h
	rm -f /usr/local/include/DT_drawtext.h

clean:
	rm -f $(OBJECTS) libSDL_console.so libSDL_console.a core
