#source: group8.s
#ld: -r --gc-sections --entry foo
#readelf: -g --wide
#notarget: ia64-*-* mep-*-*

COMDAT group section \[[ 0-9]+\] `.group' \[foo\] contains 1 sections:
   \[Index\]    Name
   \[[ 0-9]+\]   .text.foo
