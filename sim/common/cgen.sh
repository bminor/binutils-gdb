#! /bin/sh
# Generate CGEN simulator files.
#
# Usage: /bin/sh cgen.sh {"arch"|"cpu"|"decode"|"cpu-decode"} srcdir \
#	cgen cgendir cgenflags \
#	arch archflags cpu mach suffix extrafiles
#
# We store the generated files in the source directory until we decide to
# ship a Scheme interpreter (or other implementation) with gdb/binutils.
# Maybe we never will.

# We want to behave like make, any error forces us to stop.
set -e

action=$1
srcdir=$2
cgen=$3
cgendir=$4
cgenflags=$5
arch=$6
archflags=$7
cpu=$8
isa=$9
# bring parms past 9 portably into view
shift ; mach=$9
shift ; suffix=$9
shift ; extrafiles=$9

rootdir=${srcdir}/../..

lowercase='abcdefghijklmnopqrstuvwxyz'
uppercase='ABCDEFGHIJKLMNOPQRSTUVWXYZ'
ARCH=`echo ${arch} | tr "${lowercase}" "${uppercase}"`
CPU=`echo ${cpu} | tr "${lowercase}" "${uppercase}"`

if test -z "$isa" ; then
  isa=all
fi

case $action in
arch)
	rm -f tmp-arch.h1 tmp-arch.h
	rm -f tmp-arch.c1 tmp-arch.c
	rm -f tmp-all.h1 tmp-all.h

	${cgen} -s ${cgendir}/cgen-sim.scm \
		-s ${cgendir} \
		${cgenflags} \
		-f "${archflags}" \
		-m ${mach} \
		-a ${arch} \
		-i ${isa} \
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

cpu | decode | cpu-decode)

	fileopts=""
	case $action in
	*cpu*)
		rm -f tmp-cpu.h1 tmp-cpu.c1
		rm -f tmp-ext.c1 tmp-read.c1 tmp-write.c1
		rm -f tmp-sem.c1 tmp-semsw.c1
		rm -f tmp-mod.c1
		rm -f tmp-cpu.h tmp-cpu.c
		rm -f tmp-ext.c tmp-read.c tmp-write.c
		rm -f tmp-sem.c tmp-semsw.c tmp-mod.c
		fileopts="$fileopts \
			-C tmp-cpu.h1 \
			-U tmp-cpu.c1 \
			-M tmp-mod.c1 \
			${extrafiles}"
		;;
	esac
	case $action in
	*decode*)
		rm -f tmp-dec.h1 tmp-dec.h tmp-dec.c1 tmp-dec.c
		fileopts="$fileopts \
			-T tmp-dec.h1 \
			-D tmp-dec.c1"
		;;
	esac

	${cgen} -s ${cgendir}/cgen-sim.scm \
		-s ${cgendir} \
		${cgenflags} \
		-f "${archflags}" \
		-m ${mach} \
		-a ${arch} \
		-i ${isa} \
		${fileopts}

	case $action in
	*cpu*)
		sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-cpu.h1 > tmp-cpu.h
		${rootdir}/move-if-change tmp-cpu.h ${srcdir}/cpu${suffix}.h
		sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-cpu.c1 > tmp-cpu.c
		${rootdir}/move-if-change tmp-cpu.c ${srcdir}/cpu${suffix}.c
		sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-mod.c1 > tmp-mod.c
		${rootdir}/move-if-change tmp-mod.c ${srcdir}/model${suffix}.c
		if test -f tmp-ext.c1 ; then \
			sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-ext.c1 > tmp-ext.c ; \
			${rootdir}/move-if-change tmp-ext.c ${srcdir}/extract${suffix}.c ; \
		fi
		if test -f tmp-read.c1 ; then \
			sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-read.c1 > tmp-read.c ; \
			${rootdir}/move-if-change tmp-read.c ${srcdir}/read${suffix}.c ; \
		fi
		if test -f tmp-write.c1 ; then \
			sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-write.c1 > tmp-write.c ; \
			${rootdir}/move-if-change tmp-write.c ${srcdir}/write${suffix}.c ; \
		fi
		if test -f tmp-sem.c1 ; then \
			sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-sem.c1 > tmp-sem.c ; \
			${rootdir}/move-if-change tmp-sem.c ${srcdir}/sem${suffix}.c ; \
		fi
		if test -f tmp-semsw.c1 ; then \
			sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-semsw.c1 > tmp-semsw.c ; \
			${rootdir}/move-if-change tmp-semsw.c ${srcdir}/sem${suffix}-switch.c ; \
		fi

		rm -f tmp-cpu.h1 tmp-cpu.c1
		rm -f tmp-ext.c1 tmp-read.c1 tmp-write.c1
		rm -f tmp-sem.c1 tmp-semsw.c1 tmp-mod.c1
		;;
	esac

	case $action in
	*decode*)
		sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-dec.h1 > tmp-dec.h
		${rootdir}/move-if-change tmp-dec.h ${srcdir}/decode${suffix}.h
		sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" -e "s/@CPU@/${CPU}/g" -e "s/@cpu@/${cpu}/g" < tmp-dec.c1 > tmp-dec.c
		${rootdir}/move-if-change tmp-dec.c ${srcdir}/decode${suffix}.c

		rm -f tmp-dec.h1 tmp-dec.c1
		;;
	esac

	;;

desc)
	rm -f tmp-desc.h1 tmp-desc.h
	rm -f tmp-desc.c1 tmp-desc.c
	rm -f tmp-opc.h1 tmp-opc.h

	${cgen} -s ${cgendir}/cgen-opc.scm \
		-s ${cgendir} \
		${cgenflags} \
		-f "${archflags}" \
		-m ${mach} \
		-a ${arch} \
		-i ${isa} \
		-H tmp-desc.h1 \
		-C tmp-desc.c1 \
		-O tmp-opc.h1
	sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" < tmp-desc.h1 > tmp-desc.h
	${rootdir}/move-if-change tmp-desc.h ${srcdir}/${arch}-desc.h
	sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" \
		-e "s/@prefix@/${arch}/" < tmp-desc.c1 > tmp-desc.c
	${rootdir}/move-if-change tmp-desc.c ${srcdir}/${arch}-desc.c
	sed -e "s/@ARCH@/${ARCH}/g" -e "s/@arch@/${arch}/g" < tmp-opc.h1 > tmp-opc.h
	${rootdir}/move-if-change tmp-opc.h ${srcdir}/${arch}-opc.h

	rm -f tmp-desc.h1 tmp-desc.c1 tmp-opc.h1
	;;

*)
	echo "cgen.sh: bad action: ${action}" >&2
	exit 1
	;;

esac

exit 0
