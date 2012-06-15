#!/bin/sh
## Exit on error.
set -e

## Generate bungee build scripts
aclocal
autoheader
autoconf
(libtoolize --automake --copy --force || glibtoolize --automake --copy --force)
automake --add-missing --copy --foreign

## Generate argp-standalone build scripts
cd contrib/argp-standalone;./autogen.sh
