#!/bin/sh
## Exit on error.
set -e

## Generate bungee build scripts
aclocal
autoheader
autoconf
(libtoolize --automake --copy --force || glibtoolize --automake --copy --force)
automake --add-missing --copy --foreign
