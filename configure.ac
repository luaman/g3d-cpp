# Process this file with autoconf to produce a configure script.
AC_INIT([g3d-cpp], [5.01], [matrix@graphics3d.com])
AM_INIT_AUTOMAKE([1.6.1 foreign no-installinfo no-installman])
AC_CONFIG_SRCDIR([source/G3Dcpp/Box.cpp])
AM_CONFIG_HEADER(source/config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

AC_PROG_LIBTOOL

# Check for X paths
AC_PATH_X
LIBS="$LIBS -L$x_libraries"

# Checks for libraries.
AC_CHECK_LIB([GL], [main], [], [
			  AC_MSG_ERROR([Can't find libGL])])
AC_CHECK_LIB([GLU], [main], LIBGLU="-lGLU", LIBGLU="", -pthread)
#AC_CHECK_LIB([GLU], [main], [], [
#			  AC_MSG_ERROR([Can't find libGLU])])
AC_CHECK_LIB([jpeg], [main], [], [
			  AC_MSG_ERROR([Can't find libjpeg])])
# FIXME: Replace `main' with a function in `-lz':
AC_CHECK_LIB([z], [main], [], [
			  AC_MSG_ERROR([Can't find libz])])

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

AM_PATH_SDL(1.2.0, [], [AC_MSG_ERROR([Can't find SDL on your system])])
## Check for sdl-config program
#AC_PATH_PROG([SDL_CONFIG], [sdl-config], [no])
#if test x$SDL_CONFIG != xno ; then
#    SDL_INCDIR=`$SDL_CONFIG --cflags`
#	SDL_LDFLAGS=`$SDL_CONFIG --libs`
#	AC_SUBST(SDL_INCDIR)
#	AC_SUBST(SDL_LDFLAGS)
#else
#  	AC_MSG_ERROR([sdl-config not found!])
#fi

#AC_MSG_CHECKING([whether to enable debugging])
#AC_ARG_ENABLE(debugging, [
#  --enable-debugging		(Default: disabled)], [
#	    if test x$enablevar = xno; then
#            AC_MSG_RESULT(no)
#        else
#            AC_MSG_RESULT(yes)
#            AC_DEFINE(_DEBUG, 1, [Debugging on])
#        fi
#    ],[AC_MSG_RESULT(no)])

AC_CONFIG_FILES([Makefile
                 temp/Makefile
                 temp/debug/Makefile
                 temp/debug/g3d/Makefile
                 temp/debug/glg3d/Makefile
                 temp/release/Makefile
                 temp/release/g3d/Makefile
                 temp/release/glg3d/Makefile
				 source/Makefile
                 source/test/Makefile
				 source/demos/Makefile
                 source/demos/MD2Model_Demo/Makefile
				 source/demos/Collision_Demo/Makefile
				 source/demos/VAR_Demo/Makefile
				 source/include/Makefile])

#				 source/G3Dcpp/Makefile
#				 source/GLG3Dcpp/Makefile
AC_OUTPUT
