#!/bin/sh

#  Multi-build script for testing compilation of all maintained configs of GDB.
#  Copyright (C) 2002  Free Software Foundation, Inc.
#  Contributed by Richard Earnshaw  (rearnsha@arm.com)

#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.

#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

usage() {
    echo "Usage: gdb_mbuild.sh <srcdir> <builddir> [<parjobs>]"
    echo " Environment variables examined (with default if not defined):"
    echo "  AWK (awk) -- must be GNU awk"
    echo "  MAKE (make)"
    echo
    echo " Note: Everything in <builddir>/gdb-allcross will be blown away."
    exit 1;
}

if [ $# -ne 2 -a $# -ne 3 ] ; then
    usage
fi

### COMMAND LINE PARAMETERS

# Where the sources live
srcdir=$1

# Where the builds occur
buildbase=$2

# Number of parallel make jobs (you probably want about 2 jobs per cpu for
# maximum throughput)
if [ $# -eq 3 ]; then
    par=$3
else
    par="1"
fi

### ENVIRONMENT PARAMETERS
# Must be GNU awk
awk=${AWK:-awk}

# Version of make to use
make=${MAKE:-make}


# Where builds will live
builddir=${buildbase}/gdb-allcross

# Where logs will go.  NB. Must not be a sub-dir of builddir or you will loose
# them.
logdir=${buildbase}/gdb-logdir

# Where to look for the list of targets to test
maintainers=${srcdir}/gdb/MAINTAINERS

# Get the list of targets and the build options
alltarg=`${awk} < "${maintainers}" '
  $2 ~ /--target=.*/ {
    targets = gensub (/^.*--target=/, "", 1, $2)
    warnings = gensub (/[)]*$/, "", 1, $3)
    split (targets, targ, /,/)
    for (i in targ) {
        print targ[i], warnings
    }
  }'`

# Back up the log files
cd ${logdir}

if [ -f build.out ]
then
	mv build.out build.old
fi
if [ -f config.out ]
then
	mv config.out config.old
fi
if [ -f fail.sum ]
then
	mv fail.sum fail.old
fi

if [ ! -d ${builddir} ]
then
	echo ${builddir} does not exist
	exit 1
fi

cd ${builddir}
rm -rf *

MAKE=${make}
export MAKE

jobs=1
# For each target, configure and build it.
while read targ opts
do
	if [ ${opts} != "broken" ]
	then
		trap 'echo cleaning up ...; rm -rf ${builddir}/*; exit 1' 1 2 15
		echo ${targ}
		mkdir ${targ}
		cd ${targ}
		${srcdir}/configure --target=$targ \
		  --enable-gdb-build-warnings=$opts \
		  >> ${logdir}/config.tout.$targ 2>&1 &
		cd ..
		jobs=`expr ${jobs} + 1`
		if [ ${jobs} -gt ${par} ]
		then
			wait
			jobs=1
		fi
	fi
done << EOF
$alltarg
EOF

wait

cat ${logdir}/config.tout.* > ${logdir}/config.out
rm -f ${logdir}/config.tout.*

for targ in *
do
	cd $targ
	if  ${make} -j ${par} all-gdb >> ${logdir}/build.out 2>&1
	then
		true
	else
		echo ">>>>>>>>>>>>>" >> ${logdir}/fail.sum
		echo "$targ (${opts})" >> ${logdir}/fail.sum 
		tail -20 ${logdir}/build.out >> ${logdir}/fail.sum
		echo >> ${logdir}/fail.sum
		echo $targ build failed
	fi
	rm -rf *
	cd ..
done
