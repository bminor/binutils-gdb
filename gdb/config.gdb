#!/bin/sh

#
# Shell script to create proper links to machine-dependent files in
# preparation for compiling gdb.
#
# Usage: config.gdb machine
#
# If config.gdb succeeds, it leaves its status in config.status.
# If config.gdb fails after disturbing the status quo, 
# 	config.status is removed.
#

progname=$0

case $# in
1)
	machine=$1
	paramfile=m-${machine}.h
	initfile=m-${machine}init.h
	pinsnfile=${machine}-pinsn.c
	opcodefile=${machine}-opcode.h

	case $machine in
	hp9k320)
		initfile=m-sun3init.h
		pinsnfile=m68k-pinsn.c
		opcodefile=m68k-opcode.h
		;;
	hp9k320bsd)
		initfile=m-sun3init.h
		pinsnfile=m68k-pinsn.c
		opcodefile=m68k-opcode.h
		;;
	isi)
		# some version of m68k-pinsn.c should work here
		pinsnfile=m68k-pinsn.c
		opcodefile=m68k-opcode.h
		;;
	merlin)
		# m-umaxinit.h?
		initfile=unknown-or-unavailable
		pinsnfile=ns32k-pinsn.c
		opcodefile=ns32k-opcode.h
		;;
	news)
		pinsnfile=m68k-pinsn.c
		opcodefile=m68k-opcode.h
		;;
	npl)
		pinsnfile=gld-pinsn.c
		;;
	pn)
		pinsnfile=gld-pinsn.c
		;;
	sun2)
		pinsnfile=m68k-pinsn.c
		opcodefile=m68k-opcode.h
		;;
	sun3)
		pinsnfile=m68k-pinsn.c
		opcodefile=m68k-opcode.h
		;;
	umax)
		pinsnfile=ns32k-pinsn.c
		opcodefile=ns32k-opcode.h
		;;
	test)
		paramfile=one
		initfile=two
		pinsnfile=three
		opcodefile=four
		;;
	esac

	files="$paramfile $initfile $pinsnfile $opcodefile"
	links="param.h m-init.h pinsn.c opcode.h"	

	while [ -n "$files" ]
	do
		# set file to car of files, files to cdr of files
		set $files; file=$1; shift; files=$*
		set $links; link=$1; shift; links=$*

		if [ ! -r $file ]
		then
			echo "$progname: cannot create a link \`$link',"
			echo "since the file \`$file' does not exist."
			exit 1
		fi

		rm -f $link config.status
		# Make a symlink if possible, otherwise try a hard link
		ln -s $file $link 2>/dev/null || ln $file $link

		if [ ! -r $link ]
		then
			echo "$progname: unable to link \`$link' to \`$file'."
			exit 1
		fi
		echo "Linked \`$link' to \`$file'."
	done

	echo "Links are now set up for use with a $machine." \
		| tee config.status
	exit 0
	;;
*)
	echo "Usage: $progname machine"
	echo -n "Where \`machine' is something like "
	echo "\`vax', \`sun3', \`umax', etc."
	if [ -r config.status ]
	then
		cat config.status
	fi
	exit 1
	;;
esac

