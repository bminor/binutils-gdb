#!/bin/sh

# Shell script to do machine-dependent things in
# preparation for compiling gdb.
#
# Usage: config.gdb machine
#
# If config.gdb succeeds, it leaves its status in config.status.
# If config.gdb fails after disturbing the status quo, 
# 	config.status is removed.

progname=$0
host=
target=
list_hosts=
list_targets=
srcdir=

for arg in $*; do
  case $arg in
    -srcdir=*|+srcdir=*)
      srcdir=`echo $arg | sed 's/[+-]srcdir=//'`
      ;;
    -host|+host)
      list_hosts=true
      ;;
    -target|+target)
      list_targets=true
      ;;
    -host=*|+host=*)
      if [ "$host" = "" ]; then
        host=`echo $arg | sed 's/[+-]host=//'`
      else
        echo Error: Attempt to specify host machine twice
        bad=true
      fi
      ;;
    -target=*|+target=*)
      if [ "$target" = "" ]; then
        target=`echo $arg | sed 's/[+-]target=//'`
      else
        echo Error: Attempt to specify target machine twice
        bad=true
      fi
      ;;
    *)
      if [ "$host" = "" ]; then
	host=$arg
      else
        if [ "$target" = "" ]; then
          target=$arg
        else
          echo Error: More arguments than host and target machine names
	  bad=true
        fi
      fi
      ;;
  esac
done

if [ "$target" = "" ]; then target=$host; fi
if [ "$host" = "" ]; then bad=true; fi

# Find the source files, if location was not specified
if [ "$srcdir" = "" ]; then
  srcdirdefaulted=true
  srcdir=.
  if [ ! -r main.c ]; then
    srcdir=..
  fi
fi
if [ ! -r ${srcdir}/main.c ]; then
  if [ "$srcdirdefaulted" != "true" ]; then
    echo "$progname: Can't find debugger sources in \`${srcdir}'." 1>&2
  else
    echo "$progname: Can't find debugger sources in \`.' or \`..'." 1>&2
  fi
  exit 1
fi

if [ "$list_hosts" = "true" ]; then
  cd $srcdir/xconfig
  for i in * ; do
# The {} in ${i} are required or else /bin/sh in sony newsos 3.2 removes
# the quote after it.
  awk <$i "NR == 1 { lastchar = substr(\"${i}\", length(\"${i}\"), 1)
if (lastchar != \"~\" && lastchar != \"#\") \
printf \"%-12s %s\n\", \"${i}\", substr(\$0,2) }"
  done
fi

if [ "$list_targets" = "true" ]; then
  cd $srcdir/tconfig
  for i in * ; do
  awk <$i "NR == 1 { lastchar = substr(\"${i}\", length(\"${i}\"), 1)
if (lastchar != \"~\" && lastchar != \"#\") \
printf \"%-12s %s\n\", \"${i}\", substr(\$0,2) }"
  done
fi

if [ "$list_hosts" = "true" -o "$list_targets" = "true" ]; then
  exit 0
fi

if [ "$host" != "" -a ! -f $srcdir/xconfig/$host ]; then
  echo "No such host $host"
  bad=true
fi

if [ "$target" != "" -a ! -f $srcdir/tconfig/$target ]; then
  echo "No such target $target"
  bad=true
fi

if [ "$bad" = "true" ] ; then
  echo "Usage: "
  echo "  $progname [+srcdir=\`dir'] machine"
  echo "    For normal usage"
  echo "  $progname [+srcdir=\`dir'] \`host' \`target'"
  echo "  $progname [+srcdir=\`dir'] +host=\`host' +target=\`target'"
  echo "    If you are doing remote debugging between machines of two"
  echo "    different types (cross-debugging).  \`host' is the type of"
  echo "    machine on which GDB will be running.  \`target' is the"
  echo "    machine that the program you are debugging will be"
  echo "    running on."
  echo "  $progname +host"
  echo "    Print a list of valid host machine types."
  echo "  $progname +target"
  echo "    Print a list of valid target machine types."
  echo
  echo "  +srcdir=\`dir' means that the sources are in \`dir'.  For"
  echo "    example, \`cd /obj/hp300; config.gdb +srcdir=/src/gdb hp300'"
  echo "    If +srcdir is not specified, sources can either be in \`.'"
  echo "    or \`..'."
  echo

  if [ -r config.status ]
  then
    cat config.status
  fi
  exit 1
fi

rm -f tm.h xm.h

cat $srcdir/xconfig/$host $srcdir/tconfig/$target | awk '$1 == "#msg" {
  print substr($0,6)}'
paramfile=${srcdir}/`awk '
  $1 == "TM_FILE=" { print $2 }' <$srcdir/tconfig/$target`
if [ "$paramfile" != "${srcdir}/" ] ; then
  # Make a symlink if possible, otherwise try a hard link
  ln -s $paramfile tm.h 2>/dev/null || ln $paramfile tm.h
fi

paramfile=${srcdir}/`awk '
  $1 == "XM_FILE=" { print $2 }' <$srcdir/xconfig/$host`
if [ "$paramfile" != "${srcdir}/" ] ; then
  # Make a symlink if possible, otherwise try a hard link
  ln -s $paramfile xm.h 2>/dev/null || ln $paramfile xm.h
fi

rm -f config.status

case ${srcdir} in
  .)
    ;;
  *)
    echo "srcdir=${srcdir}" >./Makefile.sdir
    grep -s "source ${srcdir}/.gdbinit" .gdbinit 2>/dev/null || \
      echo "source ${srcdir}/.gdbinit" >> .gdbinit
esac

make "srcdir=${srcdir}" \
  "M_MAKEFILE=$srcdir/tconfig/$target $srcdir/xconfig/$host" \
  -f $srcdir/Makefile.dist Makefile

echo "GDB is now set up for host machine $host and target machine $target." \
	| tee config.status
exit 0
