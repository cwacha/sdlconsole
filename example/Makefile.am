FLAGS=-Wall -g -O2 -pedantic
AM_CPPFLAGS= -I../include

ConsoleExample_static_SOURCES = ConsoleExample.c split.c
ConsoleExample_static_LDFLAGS = -L../src ../src/.libs/libSDL_console.a

if HAVE_OPENGL
GLEXAMPLES = exampleGL exampleGL_static

exampleGL_static_SOURCES = ConsoleExample.c split.c
exampleGL_static_LDFLAGS = -L../src ../src/.libs/libSDL_console.a @EXAMPLE_LIBS@
exampleGL_static_CPPFLAGS = -DHAVE_OPENGL

exampleGL_SOURCES = ConsoleExample.c split.c
exampleGL_LDFLAGS = -L../src -lSDL_console  @EXAMPLE_LIBS@
exampleGL_CPPFLAGS = -DHAVE_OPENGL
else
GLEXAMPLES =
endif

ConsoleExample_SOURCES = ConsoleExample.c split.c
ConsoleExample_LDFLAGS = -L../src -lSDL_console

noinst_PROGRAMS = ConsoleExample ConsoleExample_static $(GLEXAMPLES)


EXTRA_DIST = *.h ConsoleFont.bmp LargeFont.gif background.jpg

