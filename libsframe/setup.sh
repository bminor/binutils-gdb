#!/bin/sh
# Make symlinks from tmpdir/libsframe to the linker and assembler in the
# build tree, so that we can use a -B option to gcc to force it to use
# the newly built linker and assembler.
#
# The libsframest library (used in the testsuite) needs to be built with
# SFrame info.
DIRECTORY="tmpdir/libsframe"
if [ ! -d "$DIRECTORY" ]; then
  mkdir -p $DIRECTORY
fi

if [ ! -L "$DIRECTORY/ld" ]; then
  ln -s ../../../ld/ld-new $DIRECTORY/ld
fi

if [ ! -L "$DIRECTORY/collect-ld" ]; then
  ln -s ld $DIRECTORY/collect-ld
fi

if [ ! -L "$DIRECTORY/as" ]; then
  ln -s ../../../gas/as-new $DIRECTORY/as
fi
