#as:
#source: diag-parlabel.s
#objdump: --ctf=.ctf
#ld: -shared --ctf-variables
#name: Diagnostics - Non-zero parlabel in parent

.*: +file format .*

Contents of CTF section .ctf:

  Header:
    Magic number: dff2
    Version: 4 \(CTF_VERSION_3\)
#...
    Compilation unit name: .*A.c
    Data object section:	0x0 -- 0x3 \(0x4 bytes\)
    Type section:	0x4 -- 0x33 \(0x30 bytes\)
    String section:	.*

  Labels:

  Data objects:
    a -> struct A
#...
  Function objects:

  Variables:

  Types:
#...
     [0-9a-f]*: struct A \(.*
        \[0x0\] \(ID 0x[0-9a-f]*\) \(kind 6\) struct A \(.*
            \[0x0\] \(ID 0x[0-9a-f]*\) \(kind 3\) struct B \* b \(.*
#...
  Strings:
    0: 
#...
    [0-9a-f]*: A
#...
