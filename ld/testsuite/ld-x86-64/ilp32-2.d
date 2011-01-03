#source: dummy.s
#as: --n32
#ld: -m elf32_x86_64 tmpdir/startn32.o tmpdir/foo32.o
#error: .*i386 architecture of input file `tmpdir/foo32.o' is incompatible with i386:x64-32 output
