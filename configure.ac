dnl Process this file with autoconf to produce a configure script.
AC_PREREQ([2.67])
dnl Every other copy of the package version number gets its value from here
AC_INIT([Bungee], 0.1.proto0, bungee-dev@bungeeproject.org, , http://www.bungeeproject.org)
AC_CONFIG_SRCDIR([shell/src/bungee.c])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([gnu dist-bzip2 silent-rules])
AM_SILENT_RULES(yes)

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AM_CONFIG_HEADER(config.h)
AC_GNU_SOURCE
dnl AM_GNU_GETTEXT
dnl AM_GNU_GETTEXT_VERSION
dnl BISON_I18N

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
	contrib/aclocal/Makefile
	doc/Makefile
	doc/publican-bungee/Makefile
	doc/publican-bungee/publican-bungee.spec
	doc/publican-bungee/defaults.cfg
	doc/publican-bungee/publican.cfg
	doc/User_Guide/publican.cfg
	doc/User_Guide/Makefile
	doc/User_Guide/en-US/User_Guide.ent
	doc/User_Guide/en-US/Book_Info.xml
	shell/Makefile
	shell/src/Makefile
	])

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC

dnl Check for libtool
AC_PROG_LIBTOOL

# Check for flex
AC_PROG_LEX
if test  "x${LEX}" != "xflex"; then
   AC_MSG_ERROR([GNU Flex required to build Bungee.])
fi

# Check for bison
AC_PROG_YACC
if test "x${YACC}" != "xbison -y";  then
   AC_MSG_ERROR([GNU Bison required to build Bungee.])
fi

dnl Checks for header files.
AC_CHECK_HEADERS(unistd.h sys/param.h sys/time.h time.h sys/mkdev.h sys/sysmacros.h string.h memory.h fcntl.h dirent.h sys/ndir.h ndir.h alloca.h locale.h )

dnl Check for if pkg-config exists
dnl PKG_PROG_PKG_CONFIG
AC_CHECK_PROG(HAVE_PKG_CONFIG, pkg-config, yes)
if test "x$HAVE_PKG_CONFIG" != "xyes"
then
   AC_MSG_ERROR(["pkg-config not found. Please install "pkgconfig" package.])
fi

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
dnl glib2 library flags
PKG_CHECK_MODULES([GLIB], [glib-2.0])
GLIB2_CFLAGS=$GLIB_CFLAGS
GLIB2_LIBS=$GLIB_LIBS
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

dnl dnl zmq library
dnl AC_CHECK_HEADERS([zmq.h])
dnl AC_SEARCH_LIBS([zmq_init], [zmq], , AC_MSG_ERROR([zmq messaging library not found]))

dnl ###### Enable buulding of publican guides #######
AC_ARG_ENABLE([docs],
    AS_HELP_STRING([--enable-docs], [build publican documentation]),
    [enable_docs="yes"],
    [enable_docs="no"])

if test "$enable_docs" == "yes"; then
    DOC_DIRS="publican-bungee User_Guide"
else
    DOC_DIRS=""
fi
AC_SUBST(DOC_DIRS)

AC_CHECK_PROG(have_publican, publican, yes, no)
if test "$enable_docs" == "yes" -a "x${have_publican}" != "xyes"; then
	AC_MSG_ERROR([cannot find "publican" tool for building documentation])
fi

dnl ###### Optionally set custom publican brand optionally (For example Red Hat theme) #######
dnl "bungee" brand is under $(srcdir)/doc/publican-bungee/. Before you use this option, you must
dnl install "bunge" brand. Run the "sudo $(srcdir)/doc/publican-bungee/install.sh".
AC_ARG_WITH([publican-brand],
    AS_HELP_STRING([--with-publican-brand=BRAND],
	[Use this publican BRAND instead of default "bungee" publican brand.]),
    [publican_brand=$withval])

if test "$publican_brand" == "yes"; then
	AC_MSG_ERROR([must specify publican brand value for --with-publican-brand=BRAND])
fi

if test -n "$publican_brand"; then
    PUBLICAN_BRAND=$publican_brand
else
    PUBLICAN_BRAND="bungee"
fi
AC_SUBST(PUBLICAN_BRAND)

if test -n "$publican_brand" -a "$enable_docs" == "no"; then
	AC_MSG_ERROR([must use --enable-docs along with --with-publican-brand=BRAND])
fi

AC_OUTPUT
