# Makefile for the SDL_Console example

CC=gcc

LIBS=-L.. `sdl-config --libs` -lm -lSDL_console -lSDL_image
LIBSSTATIC=-L.. `sdl-config --libs` -lm -lSDL_image ../libSDL_console.a
GLLIBS=$(LIBS) -lGL -lGLU -lglut
GLLIBSSTATIC=$(LIBSSTATIC) -lGL -lGLU -lglut

FLAGS=-Wall -g -O2

INCLUDE=`sdl-config --cflags` -I.. -I.
GLINCLUDE=$(INCLUDE) -DGL_DEMO

OBJS=ConsoleExample.o split.o
GLOBJS=ConsoleExampleGL.o split.o

TARGET=ConsoleExample
GLTARGET=ConsoleExampleGL
STATIC=ConsoleExample-static
GLSTATIC=ConsoleExampleGL-static

SUBDIRS=..


all: $(TARGET) $(STATIC) $(GLTARGET) $(GLSTATIC) FORCE
.PHONY: all

$(SUBDIRS): FORCE
	@$(MAKE) -C $@

SUBDIRS_CLEAN = $(SUBDIRS:%=%/clean)
$(SUBDIRS_CLEAN): FORCE
	@$(MAKE) `basename $@` -C `dirname $@`
.PHONY: $(SUBDIRS_CLEAN)

$(TARGET): $(SUBDIRS) $(OBJS)
	$(CC) $(OBJS) $(FLAGS) $(INCLUDE) $(LIBS) -o $@

$(STATIC): $(SUBDIRS) $(OBJS)
	$(CC) $(OBJS) $(FLAGS) $(INCLUDE) $(LIBSSTATIC) -o $@

%.o: %.c
	$(CC) $(FLAGS) $(INCLUDE) -c $< -o $@

%GL.o: %.c
	$(CC) $(FLAGS) $(GLINCLUDE) -c $< -o $@

$(GLTARGET): $(SUBDIRS) $(GLOBJS)
	$(CC) $(FLAGS) $(GLOBJS) $(GLLIBS) -o $@

$(GLSTATIC): $(SUBDIRS) $(GLOBJS)
	$(CC) $(FLAGS) $(GLOBJS) $(GLLIBSSTATIC) -o $@


clean: $(SUBDIRS_CLEAN) FORCE
	$(RM) core $(TARGET) $(GLTARGET) $(STATIC) $(OBJS) $(GLOBJS)
.PHONY: clean

FORCE:
.PHONY: FORCE


