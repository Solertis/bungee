bin_PROGRAMS = bungee
bungee_SOURCES = bungee.c system.h shell-readline.c shell.c
noinst_HEADERS = shell-readline.h shell.h

AM_CFLAGS = -fPIC -Wall -D_FILE_OFFSET_BITS=64 \
	-I$(top_srcdir)/libbungee/src \
	-DDATADIR=\"$(localstatedir)\" \
	-DCONFDIR=\"$(sysconfdir)/bungee\" \
	$(GLIB2_CFLAGS)

bungee_LDADD = $(top_builddir)/libbungee/src/libbungee.la $(GLIB2_LIBS)

CLEANFILES = *~
