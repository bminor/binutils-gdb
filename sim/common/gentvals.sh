#!/bin/sh
# Usage: gentvals.sh type dir files pattern cpp

type=$1
dir=$2
# FIXME: Would be nice to process #include's in these files.
files=$3
pattern=$4
cpp=$5

# FIXME: need trap to remove tmp files.

rm -f tmpvals.list tmpvals.uniq
for f in $files
do
	if test -f $dir/$f ; then
		grep "#define[ 	]$pattern" $dir/$f | sed -e "s/^.*#define[ 	]\($pattern\)[ 	]*\([^ 	][^ 	]*\).*$/\1/" >> tmpvals.list
	fi
done

sort <tmpvals.list | uniq >tmpvals.uniq

rm -f tmpvals.h
for f in $files
do
	if test -f $dir/$f ; then
		echo "#include <$f>" >>tmpvals.h
	fi
done

cat tmpvals.uniq |
while read sym
do
	echo "#ifdef $sym" >>tmpvals.h
	echo 'DEFVAL { "'$sym'", '$sym ' },' >>tmpvals.h
	echo "#endif" >>tmpvals.h
done

echo "#ifdef ${type}_defs"
for f in $files
do
	if test -f $dir/$f ; then
		echo "/* from $f */"
	fi
done
echo "/* begin $type target macros */"
$cpp tmpvals.h | grep DEFVAL | sed -e 's/DEFVAL//' -e 's/  / /'
echo "/* end $type target macros */"
echo "#endif"

rm -f tmpvals.list tmpvals.uniq tmpvals.h
