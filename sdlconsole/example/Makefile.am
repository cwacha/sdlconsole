FLAGS=-Wall -g -O2 -pedantic
INCLUDES= -I../include


example_SOURCES = ConsoleExample.c split.c
example_LDFLAGS = -L../src -lSDL_console

example_static_SOURCES = ConsoleExample.c split.c
example_static_LDFLAGS = -L../src ../src/.libs/libSDL_console.a

if GLDEMO
GLEXAMPLES = exampleGL exampleGL_static

exampleGL_SOURCES = ConsoleExample.c split.c
exampleGL_LDFLAGS = -L../src -lSDL_console
exampleGL_CPPFLAGS = -DGL_DEMO

exampleGL_static_SOURCES = ConsoleExample.c split.c
exampleGL_static_LDFLAGS = -L../src ../src/.libs/libSDL_console.a -lGL
exampleGL_static_CPPFLAGS = -DGL_DEMO
else
GLEXAMPLES =
endif

noinst_PROGRAMS = example example_static $(GLEXAMPLES)


EXTRA_DIST = ConsoleExample.c ConsoleExample.h split.c split.h ConsoleFont.gif LargeFont.gif background.jpg

