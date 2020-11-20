#as:
#source: array-char.c
#source: array-int.c
#objdump: --ctf=.ctf
#ld: -shared --ctf-variables --hash-style=sysv
#name: Arrays

.*: +file format .*

Contents of CTF section .ctf:

  Header:
    Magic number: dff2
    Version: 4 \(CTF_VERSION_3\)
#...
    Data object section:	.* \(0x8 bytes\)
    Type section:	.* \(0x5c bytes\)
    String section:	.*

  Labels:

  Data objects:
    digits -> int \[10\]
    digits_names -> char \*\[10\]

  Function objects:

  Variables:

  Types:
#...
     [0-9a-f]*: .*\[10\] .*
#...
     [0-9a-f]*: .*\[10\] .*
#...
