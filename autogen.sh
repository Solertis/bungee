#!/bin/sh

## Generate bungee build scripts
aclocal=$(which aclocal)
[ -z $aclocal ] && {
    echo "Please install automake, autoconf, libtool from your relevant distros"
    exit 127 # exit with proper bash $? for "Command not found"
}
aclocal
autoheader
autoconf
(libtoolize --automake --copy --force || glibtoolize --automake --copy --force)
automake --add-missing --copy --foreign
