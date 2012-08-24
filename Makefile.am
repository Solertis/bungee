EXTRA_DIST = bungee.lsm bungee.spec LICENSE autogen.sh COPYING README AUTHORS THANKS

#  if you write a self-test script named `chk', uncomment the
#  following and add `chk' to the EXTRA_DIST list
#TESTS = chk

SUBDIRS = contrib libbungee shell doc

maintainer-clean-local:
	-find . -name publican-bungee -prune -o -name Makefile -exec rm -f {} \;
	-find . -name "*~" -exec rm -f {} \;
	-find . -name ".#*" -exec rm -f {} \;
	-find . -name Makefile.in -exec rm -f {} \;
	-rm -fr autom4te.cache
	-rm -f missing aclocal.m4 config.h.in config.guess config.sub ltmain.sh \
		install-sh configure depcomp
	-rm -f bungee-@PACKAGE_VERSION@.tar.gz bungee-@PACKAGE_VERSION@.tar.bz2
	-cd doc/publican-bungee/ && make clean
