#!/bin/ksh
#
# This script is a "wrapper" to use the so-thresh.mk
# Makefile.  See the comments in so-thresh.exp
# regarding why this script exists.
#

#set -o xtrace
#set -o verbose

MAKEFLAGS=
make -v -f so-thresh.mk require_shlibs all
