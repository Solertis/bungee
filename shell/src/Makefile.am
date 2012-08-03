bin_PROGRAMS = bungee
bungee_SOURCES = bungee.c shell.c shell-readline.c shell-commands.c
noinst_HEADERS = local-defs.h shell.h shell-readline.h shell-commands.h

AM_CFLAGS = -fPIC -Wall -D_FILE_OFFSET_BITS=64 \
	-I$(top_srcdir)/libbungee/src \
	-DDATADIR=\"$(localstatedir)\" \
	-DCONFDIR=\"$(sysconfdir)/bungee\" \
	$(GLIB2_CFLAGS)

bungee_LDADD = $(top_builddir)/libbungee/src/libbungee.la $(GLIB2_LIBS)

CLEANFILES = *~
