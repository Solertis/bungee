bin_PROGRAMS = bungee
bungee_SOURCES = bungee.c xmalloc.c system.h shell-readline.c shell.c
noinst_HEADERS = shell-readline.h shell.h

AM_CFLAGS = -fPIC -Wall -D_FILE_OFFSET_BITS=64 \
	-I$(top_srcdir)/libbungee/src \
	-DDATADIR=\"$(localstatedir)\" \
	-DCONFDIR=\"$(sysconfdir)/bungee\" \
	$(ARGP_STANDALONE_CPPFLAGS) $(GLIB2_CFLAGS) $(PYTHON3_CFLAGS)

bungee_LDADD = $(top_builddir)/libbungee/src/libbungee.la $(ARGP_STANDALONE_LDADD) $(GLIB2_LIBS) $(PYTHON3_LIBS) 

CLEANFILES = *~
