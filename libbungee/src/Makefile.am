lib_LTLIBRARIES = libbungee.la

libbungee_la_LDFLAGS =
libbungee_la_CFLAGS = -fPIC -Wall -shared -nostartfiles $(GLIB2_CFLAGS) $(PYTHON3_CFLAGS)
libbungee_la_CPPFLAGS = -D_FILE_OFFSET_BITS=64 -D__USE_FILE_OFFSET64 -DBISON_LOCALEDIR='"$(BISON_LOCALEDIR)"'
libbungee_la_LIBADD = $(GLIB2_LIBS) $(PYTHON3_LIBS)

# This is a work around for a bug in ylwrap. It removes generated scanner.h header file.
scanner.h:
	$(LEX) $(LFLAGS) $(AM_LFLAGS) scanner.l

AM_YFLAGS = -d
AM_LFLAGS = --header-file=scanner.h
BUILT_SOURCES = scanner.h parser.h
parser_sources = scanner.l parser.y

libbungee_la_SOURCES = libbungee.c logger.c python-embedding.c $(parser_sources) parser-interface.c \
	python-module-bungee.c python-bungee-globals.c python-module-rules.c

# public header file that needs to be installed
include_HEADERS =
# local header files necessary to build this library
noinst_HEADERS = bungee.h libbungee.h logger.h local-defs.h python-embedding.h parser-interface.h \
	python-module-bungee.h python-bungee-globals.h python-module-rules.c scanner.h parser.h

CLEANFILES = *~ scanner.c scanner.h parser.c parser.h
