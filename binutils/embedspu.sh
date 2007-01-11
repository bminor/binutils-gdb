#! /bin/sh 
# Embed an SPU ELF executable into a PowerPC object file.
#
# Copyright 2006 Free Software Foundation, Inc.
#
# This file is part of GNU Binutils.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
# 02110-1301, USA.

usage ()
{
  echo "Usage: embedspu [flags] symbol_name input_filename output_filename"
  echo
  echo "        input_filename:  SPU ELF executable to be embedded"
  echo "        output_filename: Resulting PowerPC object file"
  echo "        symbol_name:     Name of program handle struct to be defined"
  echo "        flags:           GCC flags defining PowerPC object file format"
  echo "                         (e.g. -m32 or -m64)"
  exit 1
}

program_transform_name=
mydir=`dirname "$0"`

find_prog ()
{
  prog=`echo $1 | sed "$program_transform_name"`
  which $prog > /dev/null 2> /dev/null && return 0
  prog="$mydir/$prog"
  test -x "$prog" && return 0
  prog="$mydir/$1"
  test -x "$prog" && return 0
  prog=`echo $1 | sed "$program_transform_name"`
  return 1
}

SYMBOL=
INFILE=
OUTFILE=
FLAGS=

parse_args ()
{
  while test -n "$1"; do
    case "$1" in
      -*) FLAGS="${FLAGS} $1" ;;
      *)  if test -z "$SYMBOL"; then
	    SYMBOL="$1"
	  elif test -z "$INFILE"; then
	    INFILE="$1"
	  elif test -z "$OUTFILE"; then
	    OUTFILE="$1"
	  else
	    echo "Too many arguments!"
	    usage
	  fi ;;
    esac
    shift
  done
  if test -z "$OUTFILE"; then
    usage
  fi
  if test ! -r "$INFILE"; then
    echo "${INFILE}: File not found"
    usage
  fi
}

main ()
{
  parse_args "$@"

  # Find a powerpc gcc.  Support running from a combined tree build.
  if test -x "$mydir/../gcc/xgcc"; then
    CC="$mydir/../gcc/xgcc -B$mydir/../gcc/"
  else
    find_prog gcc
    if test $? -ne 0; then
      echo "Cannot find $prog"
      exit 1
    fi
    CC="$prog"
  fi

  # Find readelf.  Any old readelf should do.  We only want to read syms.
  find_prog readelf
  if test $? -ne 0; then
    if which readelf > /dev/null 2> /dev/null; then
      prog=readelf
    else
      echo "Cannot find $prog"
      exit 1
    fi
  fi
  READELF="$prog"

  # Sanity check the input file
  if ! ${READELF} -h ${INFILE} | grep 'Class:.*ELF32' >/dev/null 2>/dev/null \
     || ! ${READELF} -h ${INFILE} | grep 'Type:.*EXEC' >/dev/null 2>/dev/null \
     || ! ${READELF} -h ${INFILE} | egrep 'Machine:.*(SPU|17)' >/dev/null 2>/dev/null
  then
    echo "${INFILE}: Does not appear to be an SPU executable"
    exit 1
  fi

  # Build embedded SPU image.
  # 1. The whole SPU ELF file is written to .rodata.speelf
  # 2. Symbols starting with the string "_EAR_" in the SPU ELF image are
  #    special.  They allow an SPU program to access corresponding symbols
  #    (ie. minus the _EAR_ prefix), in the PowerPC program.  _EAR_ without
  #    a suffix is used to refer to the addrress of the SPU image in
  #    PowerPC address space.  _EAR_* symbols must all be defined in one
  #    section at 16 byte intervals.
  #    Find all _EAR_ symbols using readelf, sort by address, and write
  #    the address of the corresponding PowerPC symbol in a table built
  #    in .data.spetoe.
  # 3. Write a struct spe_program_handle to .data.
  ${CC} ${FLAGS} -x assembler-with-cpp -nostartfiles -nostdlib \
	-Wa,-mbig -Wl,-r -Wl,-x -o ${OUTFILE} - <<EOF
 .section .rodata.speelf,"a",@progbits
 .p2align 7
__speelf__:
 .incbin "${INFILE}"

 .section .data.spetoe,"aw",@progbits
 .p2align 7
__spetoe__:
`${READELF} -s ${INFILE} | grep ' _EAR_' | sort -k 2 | awk \
'BEGIN { \
  last_addr = 0; \
  last_sym = ""; \
} \
last_addr != 0 && strtonum("0x" $2) != last_addr + 16 { \
	print "#error Symbols " last_sym " and " $8 " are not 16 bytes apart!"; \
} \
{ last_addr = strtonum("0x" $2); \
  last_sym = $8; \
} \
$8 == "_EAR_" { \
	print "#ifdef _LP64"; \
	print " .quad __speelf__, 0"; \
	print "#else"; \
	print " .int 0, __speelf__, 0, 0"; \
	print "#endif"; \
} \
$8 != "_EAR_" { \
	print "#ifdef _LP64"; \
	print " .quad " substr($8, 6) ", 0"; \
	print "#else"; \
	print " .int 0, " substr($8, 6) ", 0, 0"; \
	print "#endif"; \
}'`

 .section .data,"aw",@progbits
 .globl ${SYMBOL}
# fill in a struct spe_program_handle
#ifdef _LP64
 .p2align 3
${SYMBOL}:
 .int 24
 .int 0
 .quad __speelf__
 .quad __spetoe__
#else
 .p2align 2
${SYMBOL}:
 .int 12
 .int __speelf__
 .int __spetoe__
#endif
EOF
}

main "$@"
