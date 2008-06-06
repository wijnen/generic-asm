#!/bin/sh -e

export AUTOMAKE=automake-1.10
export ACLOCAL=aclocal-1.10

for i in README NEWS Changelog ; do
	echo "This file exists only to keep automake happy." > $i
done
echo "Bas Wijnen <wijnen@debian.org>" > AUTHORS

autoreconf --force --install --symlink
./configure "$@"
