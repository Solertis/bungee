lib_LTLIBRARIES = libbungee.la

libbungee_la_LDFLAGS =
libbungee_la_CFLAGS = -fPIC -Wall -shared -nostartfiles $(GLIB2_CFLAGS) $(PYTHON3_CFLAGS)
libbungee_la_CPPFLAGS = -D_FILE_OFFSET_BITS=64 -D__USE_FILE_OFFSET64
libbungee_la_LIBADD = $(GLIB2_LIBS) $(PYTHON3_LIBS)

libbungee_la_SOURCES = bungee.c python-embedding.c logger.c
# public header file that needs to be installed
include_HEADERS =
# local header files necessary to build this library
noinst_HEADERS = bungee.h python-embedding.h logger.h local-defs.h

CLEANFILES = *~
