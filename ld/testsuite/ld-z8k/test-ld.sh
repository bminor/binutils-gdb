#!/bin/bash
# use files from ~/tmp/binutils-gdb/ld/testsuite/ld-z8k

SRCDIR=~/tmp/binutils-gdb/ld/testsuite/ld-z8k

set -e

z8k-coff-as -I $SRCDIR -o this-file.o $SRCDIR/this-file.s
z8k-coff-as -I $SRCDIR -o other-file.o $SRCDIR/other-file.s
z8k-coff-ld -o reloc.coff -L$SRCDIR -T reloc.ld this-file.o other-file.o
z8k-coff-objdump -dr reloc.coff

z8k-coff-as -z8001 -I $SRCDIR -o this-fileseg.o $SRCDIR/this-file.s
z8k-coff-as -z8001 -I $SRCDIR -o other-fileseg.o $SRCDIR/other-file.s
z8k-coff-ld -o relocseg.coff -mz8001 -L$SRCDIR -T reloc.ld this-fileseg.o other-fileseg.o
z8k-coff-objdump -dr relocseg.coff

z8k-coff-ld -o relocseg2.coff -mz8001 -L$SRCDIR -T relocseg.ld this-fileseg.o other-fileseg.o
z8k-coff-objdump -dr relocseg2.coff
