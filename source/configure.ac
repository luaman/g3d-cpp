# Process this file with autoconf to produce a configure script.
AC_INIT([g3d-cpp], [5.01], [matrix@graphics3d.com])
AM_INIT_AUTOMAKE([1.6.1 foreign no-installinfo no-installman subdir-objects])
AC_CONFIG_SRCDIR([G3Dcpp/Box.cpp])
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

AC_PROG_LIBTOOL

# Check for X paths
AC_PATH_X
LIBS="$LIBS -L$x_libraries"

# Checks for libraries.
AC_CHECK_LIB([GL], [glBegin], [], [
			  AC_MSG_ERROR([Can't find libGL])])
AC_CHECK_LIB([GLU], [gluBuild2DMipmaps], [], [
			  AC_MSG_ERROR([Can't find libGLU])])
AC_CHECK_LIB([SDL], [SDL_Init], [], [
			  AC_MSG_ERROR([Can't find libSDL])]) 
AC_CHECK_LIB([jpeg], [main], [], [
			  AC_MSG_ERROR([Can't find libjpeg])])
# FIXME: Replace `main' with a function in `-lz':
AC_CHECK_LIB([z], [main])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h float.h malloc.h memory.h netdb.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit floor gethostbyname gethostname inet_ntoa memset mkdir pow select socket sqrt strchr strdup strrchr strstr])

# Check for sdl-config program
AC_PATH_PROG([SDL_CONFIG], [sdl-config], [no])
if test x$SDL_CONFIG != xno ; then
    SDL_INCDIR=`$SDL_CONFIG --cflags`
	SDL_LDFLAGS=`$SDL_CONFIG --libs`
	AC_SUBST(SDL_INCDIR)
	AC_SUBST(SDL_LDFLAGS)
else
  	AC_MSG_ERROR([sdl-config not found!])
fi

AC_MSG_CHECKING([whether to enable debugging])
AC_ARG_ENABLE(debugging, [
  --enable-debugging		(Default: disabled)], [
	    if test x$enablevar = xno; then
            AC_MSG_RESULT(no)
        else
            AC_MSG_RESULT(yes)
            AC_DEFINE(_DEBUG, 1, [Debugging on])
        fi
    ],[AC_MSG_RESULT(no)])

AC_CONFIG_FILES([Makefile
                 G3Dcpp/Makefile
                 test/Makefile
				 demos/Makefile
				 demos/IFS_Demo/Makefile
				 demos/GLG3D_Demo/Makefile
				 include/Makefile
                 GLG3Dcpp/Makefile])
#                 include/glh/extgen/Makefile])
AC_OUTPUT
