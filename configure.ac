dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.67])
dnl Every other copy of the package version number gets its value from here
AC_INIT([Bungee], 0.1.0proto0, bungee-dev@bungeeproject.org, , http://www.bungeeproject.org)
AC_CONFIG_SRCDIR([shell/src/bungee.c])
AM_INIT_AUTOMAKE([gnu dist-bzip2 silent-rules])
AM_SILENT_RULES

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AM_CONFIG_HEADER(config.h)
AC_GNU_SOURCE
AC_SUBST(VERSION)
ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)
AC_CANONICAL_HOST
AC_CONFIG_FILES([bungee.lsm
		bungee.spec
		Makefile
		libbungee/Makefile
		libbungee/src/Makefile
		contrib/Makefile
		doc/Makefile
		shell/Makefile
		shell/src/Makefile
		])

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC
# Turn off shared libraries during beta-testing, since they
# make the build process take too long. For release builds, remove
# disable-shared option or comment LT_INIT line and uncomment
# AC_PROG_LIBTOOL. 
# LT_INIT([disable-shared])
AC_PROG_LIBTOOL

dnl Checks for header files.
AC_CHECK_HEADERS(unistd.h sys/param.h sys/time.h time.h sys/mkdev.h sys/sysmacros.h string.h memory.h fcntl.h dirent.h sys/ndir.h ndir.h alloca.h locale.h )

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
dnl glib2 library flags
PKG_CHECK_MODULES([GLIB2], [glib-2.0])
AC_SUBST(GLIB2_CFLAGS)
AC_SUBST(GLIB2_LIBS)

dnl python3 library flags
PKG_CHECK_MODULES([PYTHON3], [python3])
AC_SUBST(PYTHON3_CFLAGS)
AC_SUBST(PYTHON3_LIBS)

dnl readline library flags
AC_SEARCH_LIBS([readline], [readline], , AC_MSG_ERROR([ERROR! libreadline not found...]))

dnl msgpack library
AC_CHECK_HEADERS([msgpack.h])
AC_SEARCH_LIBS([msgpack_version], [msgpack], , AC_MSG_ERROR([msgpack serialization library not found]))

dnl zmq library
AC_CHECK_HEADERS([zmq.h])
AC_SEARCH_LIBS([zmq_init], [zmq], , AC_MSG_ERROR([zmq messaging library not found]))

AC_OUTPUT
