AM_CPPFLAGS = -I../include

lib_LTLIBRARIES = libSDL_console.la

libSDL_console_la_SOURCES = \
	SDL_console.c \
	DT_drawtext.c \
	internal.c

libSDL_console_la_LDFLAGS = \
	-release $(MAJOR_VERSION).$(MINOR_VERSION) \
	-version-info $(PATCH_VERSION):0:0
libSDL_console_la_LIBADD = @CON_LIBS@

