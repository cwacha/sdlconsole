#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.71])

AC_INIT(SDL_console, [2.1.0])
AC_CONFIG_SRCDIR([src/SDL_console.c])
AC_CONFIG_HEADERS([config.h])

# Version (ensure this is in sync with line 5: AC_INIT)
MAJOR_VERSION=2
MINOR_VERSION=1
PATCH_VERSION=0
VERSION=$MAJOR_VERSION.$MINOR_VERSION.$PATCH_VERSION
AC_SUBST(MAJOR_VERSION)
AC_SUBST(MINOR_VERSION)
AC_SUBST(PATCH_VERSION)
AC_SUBST(VERSION)

dnl Setup for automake
AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

dnl Check for tools
LT_INIT
AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

# Check for SDL
SDL_VERSION=1.2.4
AM_PATH_SDL($SDL_VERSION,
		  :,
	AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

# Check for SDL_image
AC_ARG_ENABLE(sdlimage,
AS_HELP_STRING([--enable-sdlimage],[support SDL_image (default=yes)]),
,
enable_sdlimage=yes)
if test x$enable_sdlimage = xyes; then
	AC_CHECK_LIB([SDL_image], [IMG_Load],have_sdlimage=yes)
	if test x$have_sdlimage = xyes; then
	  CFLAGS="$CFLAGS -DHAVE_SDLIMAGE"
	  CON_LIBS="$CON_LIBS -lSDL_image"
	  LIBS="$LIBS $CON_LIBS"
	else
	  enable_sdlimage=no
	fi
fi
AC_SUBST(CON_LIBS)


AC_ARG_ENABLE(gldemo,
AS_HELP_STRING([--enable-opengl],[enable OpenGL support for the example (default=yes)]),
,
enable_gldemo=yes)
if test x$enable_gldemo = xyes; then
	have_opengl=yes
	AC_CHECK_LIB([GL], [glLoadIdentity],have_gl=yes,have_opengl=no)
	AC_CHECK_LIB([GLU], [gluPerspective],have_glu=yes,have_opengl=no,-lGL)
	AC_CHECK_LIB([glut], [glutSolidTeapot],have_glut=yes,have_opengl=no,-lGL -lGLU)
	if test x$have_opengl = xyes; then
	  EXAMPLE_LIBS="$EXAMPLE_LIBS -lGL -lGLU -lglut"
	else
	  enable_gldemo=no
	fi
fi
AC_SUBST(EXAMPLE_LIBS)
AM_CONDITIONAL(HAVE_OPENGL,test x$enable_gldemo = xyes)


AC_CONFIG_FILES([Makefile
                 src/Makefile
                 include/Makefile
				 example/Makefile])
AC_OUTPUT

# Print some useful information
echo
echo "Configuration:"
echo "   Target system type: $target"
echo "   Build static library: $enable_static"
echo "   Build shared library: $enable_shared"
echo "   Build with SDL_image: $enable_sdlimage"
echo "   Build OpenGL Demos: $enable_gldemo"
echo
echo "   Headers will be installed in $prefix/include"
echo "   Libraries will be installed in $prefix/lib"
echo

