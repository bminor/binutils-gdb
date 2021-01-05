#as:
#source: function.c
#objdump: --ctf=.ctf
#ld: -shared
#name: Function

.*: +file format .*

Contents of CTF section .ctf:

  Header:
    Magic number: 0xdff2
    Version: 4 \(CTF_VERSION_3\)
#...
    Compilation unit name: .*function.c
    Function info section:	.* \(0x4 bytes\)
    Type section:	.* \(0x90 bytes\)
    String section:	.*
#...
  Function objects:
    foo -> 0x[0-9a-f]*: \(kind 5\) int \(\*\) \(char, int, float, void \*, void \(\*\)\(\*\) \(int\)\) \(aligned at 0x[0-9a-f]*\)
#...
  Types:
#...
    0x[0-9a-f]*: \(kind 5\) int \(\*\) \(char, int, float, void \*, void \(\*\)\(\*\) \(int\)\) \(aligned at 0x[0-9a-f]*\)
#...
