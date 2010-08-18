#readelf: -g
#name: automatic section group
#source: groupauto.s

#...
COMDAT group section \[    1\] `some_group' \[some_group\] contains 2 sections:
[ 	]+\[Index\][ 	]+Name
[ 	]+\[.*\][ 	]+.text
[ 	]+\[.*\][ 	]+.note.bar
#pass
