#! /bin/sh
# Generate CGEN opcode files: arch-opc.[ch], arch-asm.c, arch-asm.c.
#
# We store the generated files in the source directory until we decide to
# ship a scheme with gdb/binutils.  Maybe we never will.

# We want to behave like make, any error forces us to stop.
set -e

srcdir=$1
cgendir=$2
cgenflags=$3
scheme=$4
schemeflags=$5
arch=$6

rm -f tmp-opc.h tmp-opc.c tmp-asm.c tmp-dis.c
rm -f tmp-opc.h1 tmp-opc.c1 tmp-asm.c1 tmp-dis.c1
rm -f tmp-asm.in tmp-asm.in1 tmp-dis.in tmp-dis.in1

$scheme $schemeflags $cgendir/cgen-opc.scm \
	-s $cgendir \
	$cgenflags \
	-m all \
	-a $arch \
	-h tmp-opc.h1 \
	-t tmp-opc.c1 \
	-A tmp-asm.in1 \
	-D tmp-dis.in1

sed -e "s/@arch@/${arch}/g" < tmp-opc.h1 > tmp-opc.h
${srcdir}/../move-if-change tmp-opc.h ${srcdir}/${arch}-opc.h
sed -e "s/@arch@/${arch}/g" < tmp-opc.c1 > tmp-opc.c
${srcdir}/../move-if-change tmp-opc.c ${srcdir}/${arch}-opc.c
sed -e "/ -- assembler routines/ r tmp-asm.in1" ${srcdir}/cgen-asm.in \
  | sed -e "s/@arch@/${arch}/g" > tmp-asm.c
${srcdir}/../move-if-change tmp-asm.c ${srcdir}/${arch}-asm.c
sed -e "/ -- disassembler routines/ r tmp-dis.in1" ${srcdir}/cgen-dis.in \
  | sed -e "s/@arch@/${arch}/g" > tmp-dis.c
${srcdir}/../move-if-change tmp-dis.c ${srcdir}/${arch}-dis.c

rm -f tmp-opc.h1 tmp-opc.c1 tmp-asm.c1 tmp-dis.c1
rm -f tmp-asm.in tmp-asm.in1 tmp-dis.in tmp-dis.in1
