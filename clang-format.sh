#/usr/bin/env bash

set -x
set -e

CLANG_FORMAT=${CLANG_FORMAT:-clang-format}

$CLANG_FORMAT -i \
	gdb/*.h gdb/*.c \
	gdb/*/*.h gdb/*/*.c \
	gdbserver/*.h gdbserver/*.cc \
	gdbsupport/*.h gdbsupport/*.cc
