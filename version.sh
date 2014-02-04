#!/bin/sh

if [ -z $1 ]; then
    echo "$0: Destination directory not specified!" >&2
    exit 1
fi

version=$(git describe 2>/dev/null)

if [ -z $version ]; then
    version=PACKAGE_OXYGEN_VERSION
else
    version='"'$version'"'
fi

verString='pcchar OXYGEN_VERSION='$version';'

cd $1

if [ ! -e version.cpp ] || ! grep -q "^$verString\$" version.cpp; then
    echo '#include "config.h"' > version.cpp
    echo '#define pcchar const char*' >> version.cpp # quirk to prevent putting '*' to grep match pattern
    echo $verString >> version.cpp
fi
