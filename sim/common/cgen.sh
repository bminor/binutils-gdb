#! /bin/sh
# Generate CGEN simulator files.
#
# Usage: /bin/sh cgen.sh {"arch"|"cpu"|"decode"} srcdir \
#	scheme schemeflags \
#	cgendir cgenflags \
#	arch archflags cpu mach suffix extrafiles
#
# We store the generated files in the source directory until we decide to
# ship a Scheme interpreter with gdb/binutils.  Maybe we never will.

# We want to behave like make, any error forces us to stop.
set -e

action=$1
srcdir=$2
scheme=$3
schemeflags=$4
cgendir=$5
cgenflags=$6
arch=$7
archflags=$8
cpu=$9
mach=${10}
suffix=${11}
extrafiles=${12}

lowercase='abcdefghijklmnopqrstuvwxyz'
uppercase='ABCDEFGHIJKLMNOPQRSTUVWXYZ'
# FIXME: May have to rewrite this.
ARCH=`echo ${arch} | tr "${lowercase}" "${uppercase}"`
CPU=`echo ${cpu} | tr "${lowercase}" "${uppercase}"`

rootdir=${srcdir}/../..

case $action in
arch)
	rm -f tmp-arch.h1 tmp-arch.h
	rm -f tmp-arch.c1 tmp-arch.c
	rm -f tmp-all.h1 tmp-all.h

	${scheme} ${schemeflags} ${cgendir}/cgen-sim.scm \
		-s ${cgendir} \
		${cgenflags} \
		-f "${archflags}" \
		-m all \
		-a ${arch} \
		-A tmp-arch.h1 \
		-B tmp-arch.c1 \
		-N tmp-all.h1
	sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" < tmp-arch.h1 > tmp-arch.h
	${rootdir}/move-if-change tmp-arch.h ${srcdir}/arch.h
	sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" < tmp-arch.c1 > tmp-arch.c
	${rootdir}/move-if-change tmp-arch.c ${srcdir}/arch.c
	sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" < tmp-all.h1 > tmp-all.h
	${rootdir}/move-if-change tmp-all.h ${srcdir}/cpuall.h

	rm -f tmp-arch.h1 tmp-arch.c1 tmp-all.h1
	;;

cpu)
	rm -f tmp-cpu.h1 tmp-ext.c1 tmp-read.c1 tmp-sem.c1 tmp-semsw.c1 tmp-mod.c1
	rm -f tmp-cpu.h tmp-ext.c tmp-read.c tmp-sem.c tmp-semsw.c tmp-mod.c

	${scheme} ${schemeflags} ${cgendir}/cgen-sim.scm \
		-s ${cgendir} \
		${cgenflags} \
		-f "${archflags}" \
		-m ${mach} \
		-a ${arch} \
		-C tmp-cpu.h1 \
		-M tmp-mod.c1 \
		${extrafiles}
	sed -e "s/@ARCH@/${ARCH}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-cpu.h1 > tmp-cpu.h
	${rootdir}/move-if-change tmp-cpu.h ${srcdir}/cpu${suffix}.h
	sed -e "s/@ARCH@/${ARCH}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-mod.c1 > tmp-mod.c
	${rootdir}/move-if-change tmp-mod.c ${srcdir}/model${suffix}.c
	if test -f tmp-ext.c1 ; then \
		sed -e "s/@ARCH@/${ARCH}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-ext.c1 > tmp-ext.c ; \
		${rootdir}/move-if-change tmp-ext.c ${srcdir}/extract${suffix}.c ; \
	fi
	if test -f tmp-read.c1 ; then \
		sed -e "s/@ARCH@/${ARCH}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-read.c1 > tmp-read.c ; \
		${rootdir}/move-if-change tmp-read.c ${srcdir}/read${suffix}.c ; \
	fi
	if test -f tmp-sem.c1 ; then \
		sed -e "s/@ARCH@/${ARCH}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-sem.c1 > tmp-sem.c ; \
		${rootdir}/move-if-change tmp-sem.c ${srcdir}/sem${suffix}.c ; \
	fi
	if test -f tmp-semsw.c1 ; then \
		sed -e "s/@ARCH@/${ARCH}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-semsw.c1 > tmp-semsw.c ; \
		${rootdir}/move-if-change tmp-semsw.c ${srcdir}/sem${suffix}-switch.c ; \
	fi

	rm -f tmp-cpu.h1 tmp-mod.c1
	rm -f tmp-read.c1 tmp-ext.c1 tmp-sem.c1 tmp-semsw.c1
	;;

decode)
	rm -f tmp-dec.h1 tmp-dec.h tmp-dec.c1 tmp-dec.c

	${scheme} ${schemeflags} ${cgendir}/cgen-sim.scm \
		-s ${cgendir} \
		${cgenflags} \
		-f "${archflags}" \
		-m ${mach} \
		-a ${arch} \
		-T tmp-dec.h1 \
		-D tmp-dec.c1
	sed -e "s/@ARCH@/${ARCH}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-dec.h1 > tmp-dec.h
	${rootdir}/move-if-change tmp-dec.h ${srcdir}/decode${suffix}.h
	sed -e "s/@ARCH@/${ARCH}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-dec.c1 > tmp-dec.c
	${rootdir}/move-if-change tmp-dec.c ${srcdir}/decode${suffix}.c

	rm -f tmp-dec.h1 tmp-dec.c1
	;;

*)
	echo "cgen.sh: bad action: ${action}" >&2
	exit 1
	;;

esac

exit 0
