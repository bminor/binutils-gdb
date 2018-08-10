#source: pr23494a.s
#PROG: objcopy
#as: --x32
#objcopy: -O elf64-x86-64 -R .note.gnu.property
#readelf: -n
#not-target: x86_64-*-nacl*
