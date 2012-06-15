EXTRA_DIST = bungee.lsm bungee.spec bungee.texinfo LICENSE autogen.sh \
	COPYING README AUTHORS THANKS NEWS

#  if you write a self-test script named `chk', uncomment the
#  following and add `chk' to the EXTRA_DIST list
#TESTS = chk

SUBDIRS = contrib libbungee shell doc 

maintainer-clean-local: 
	-find . -name "*~" -exec rm -f {} \;
	-find . -name ".#*" -exec rm -f {} \;
	-find . -name Makefile.in -exec rm -f {} \;
	-find . -name Makefile -exec rm -f {} \;
	-rm -fr autom4te.cache
	-rm -f missing aclocal.m4 config.h.in config.guess config.sub ltmain.sh \
		install-sh configure depcomp
	-rm -fr contrib/argp-standalone/autom4te.cache
	-rm -f contrib/argp-standalone/aclocal.m4 contrib/argp-standalone/config.h.in \
		contrib/argp-standalone/configure contrib/argp-standalone/depcomp \
		contrib/argp-standalone/install-sh contrib/argp-standalone/missing