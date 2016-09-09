#!/bin/sh

VERSION="$(git describe --tags --long --always 2> /dev/null)"

cat <<!
#ifdef VERSION
#error VERSION should not be defined before
#endif

#define VERSION "${VERSION:-unknown}"
!
