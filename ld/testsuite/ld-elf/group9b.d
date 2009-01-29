#source: group9.s
#ld: -r --gc-sections --entry bar
#readelf: -g --wide
#notarget: ia64-*-* mep-*-*

COMDAT group section \[[ 0-9]+\] `.group' \[foo\] contains 2 sections:
   \[Index\]    Name
   \[[ 0-9]+\]   .text.foo
   \[[ 0-9]+\]   .data.foo

COMDAT group section \[[ 0-9]+\] `.group' \[bar\] contains 1 sections:
   \[Index\]    Name
   \[[ 0-9]+\]   .text.bar
