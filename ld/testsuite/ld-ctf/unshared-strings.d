#as:
#source: unshared-strings-A.c
#source: unshared-strings-B.c
#objdump: --ctf
#ld: -shared
#name: Verify proper sharing (and non-sharing) of strings

.*: +file format .*

Contents of CTF section .ctf:

  Header:
    Magic number: 0xdff2
    Version: 5 \(CTF_VERSION_4\)
#...
  Strings:
    0x0: 
    0x[0-9a-f]*: A
#...
    0x[0-9a-f]*: C1
    0x[0-9a-f]*: C2
#...
    0x[0-9a-f]*: aaa
#...
CTF archive member: .*/unshared-strings-A.c:
#...
  Types:
    0x[0-9a-f]*: \(kind 6\) struct A .*
                \[0x0\] wurblefrotz: ID .*

  Strings:
    0x0: .ctf
#...
    0x[0-9a-f]*: wurblefrotz

CTF archive member: .*/unshared-strings-B.c:
#...
  Types:
    0x[0-9a-f]*: \(kind 6\) struct A .*
                \[0x0\] wurblefritz: ID .*

  Strings:
    0x0: .ctf
#...
    0x[0-9a-f]*: wurblefritz
