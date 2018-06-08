#source: expdyn2.s
#as: --pic --no-underscore --em=criself -I$srcdir/$subdir
#ld: --shared -m crislinux --version-script $srcdir/$subdir/expalltst3 --hash-style=sysv
#objdump: -s -T

# A DSO that has two versioned symbols, each with a weak alias.
# Each symbol is versioned.

.*:     file format elf32-cris

DYNAMIC SYMBOL TABLE:
#...
0+2238 g[ 	]+DO .data[	 ]+0+4  TST3[ 	]+__expobj2
0+1ba g[ 	]+DF .text[	 ]+0+2  TST3[ 	]+__expfn2
0+1ba  w[ 	]+DF .text[	 ]+0+2  TST3[ 	]+expfn2
0+2238  w[ 	]+DO .data[	 ]+0+4  TST3[ 	]+expobj2
#...
Contents of section .text:
 01b8 0f050f05                             .*
#...
Contents of section .got:
 222c bc210000 00000000 00000000           .*
Contents of section .data:
 2238 00000000                             .*
